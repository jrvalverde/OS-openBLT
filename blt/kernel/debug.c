/* $Id$
**
** Copyright 1998 Brian J. Swetland
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions, and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions, and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef KDEBUG

#include <i386/io.h>
#include <blt/conio.h>

#include <string.h>
#include "kernel.h"
#include "port.h"
#include "rights.h"
#include "resource.h"
typedef struct { uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; } regs;


extern resnode_t *resource_list;


#define RMAX 1024

static char *tstate[] =
{ "KERNL", "RUNNG", "READY", "DEAD ", "S/PRT", "S/IRQ", "S/TMR", "S/SEM",
  "S/THR" };


void printres(resource_t *r)
{
    switch(r->type){
    case RSRC_PORT:
        kprintf("    PORT %U: (slave %U) (size %U)",r->id,
		((port_t*)r)->slaved, ((port_t*)r)->msgcount);
        break;
    case RSRC_TASK:
        kprintf("    TASK %U: (state %s/%U) '%s'",r->id,
		tstate[((task_t*)r)->flags],
                ((task_t*)r)->sleeping_on, ((task_t*)r)->name);
        break;
    case RSRC_ASPACE:
        kprintf("  ASPACE %U: @ %x",r->id,((aspace_t*)r)->pdir[0]&0xFFFFF000);
        break;
    case RSRC_RIGHT:
        kprintf("   RIGHT %U: %x",r->id,((right_t*)r)->flags);
        break;
    case RSRC_SEM:
        kprintf("     SEM %U: (count %U)",r->id,((sem_t*)r)->count);
        break;
    case RSRC_AREA:
        kprintf("    AREA %U: virt %x size %x pgroup %x",r->id,
                ((area_t*)r)->virt_addr * 0x1000, ((area_t*)r)->length * 0x1000,
                ((area_t*)r)->pgroup);
    }
}

void dumprsrc(resnode_t *rn)
{
	while(rn) {
		printres(rn->resource);
		rn = rn->next;
	}
	
}

void dumptasks(void)
{
    int i,j,n;
    task_t *t;
    aspace_t *a;
    
    kprintf("Task Prnt Addr State Wait esp      scount   Name");
    kprintf("---- ---- ---- ----- ---- -------- -------- --------------------------------");

    for(i=1;i<RMAX;i++){
        if((t = rsrc_find_task(i))){
            a = t->addr;
            {
                area_t *area = rsrc_find_area(t->addr->heap_id);
                if(area) j = area->virt_addr + area->length;
                else j =0;
                
            }
            
            kprintf("%U %U %U %s %U %x %x %s",
                    i,t->rsrc.owner->rsrc.id,t->addr->rsrc.id,tstate[t->flags],
			t->sleeping_on,t->esp /*j*4096*/,t->scount,t->name);
            
        }
    }
}

void dumptask(int id)
{
    int i,j,n;
    task_t *t;
    aspace_t *a;

	if(!(t = rsrc_find_task(id))) {
		kprintf("no such task %X",id);
		return;
	}
	
    
    kprintf("Task Prnt Addr State Wait brk      Name");
    kprintf("---- ---- ---- ----- ---- -------- --------------------------------");

	a = t->addr;
    {
        area_t *area = rsrc_find_area(t->addr->heap_id);
        if(area) j = area->virt_addr + area->length;
        else j = 0;
    }	
	kprintf("%U %U %U %s %U %x %s",
	id,t->rsrc.owner->rsrc.id,t->addr->rsrc.id,tstate[t->flags],
	t->sleeping_on,j*4096,t->name);
	

	kprintf("");
	dumprsrc(t->resources);
}

void dumpports()
{
    int i;
    port_t *p;

    kprintf("Port Task Rstr Slav Size Owner Name");
    kprintf("---- ---- ---- ---- ---- --------------------------------");
    
    for(i=1;i<RMAX;i++){
        if((p = rsrc_find_port(i))){
            kprintf("%U %U %U %U %U %s",
                    i, p->rsrc.owner->rsrc.id, p->restrict, p->slaved,
                    p->msgcount, p->rsrc.owner->name);
        }
    }

}


static void trace(uint32 ebp,uint32 eip)
{
    int f = 1;

    kprintf("f# EBP      EIP");    
    kprintf("00 xxxxxxxx %x",eip);    
    do {        
/*        kprintf("%X %x %x",f,ebp, *((uint32*)ebp));*/
        kprintf("%X %x %x",f,ebp+4, *((uint32*)(ebp+4)));
        ebp = *((uint32*)ebp);
        f++;        
    } while(ebp < 0x00400000 && ebp > 0x00200000);    
}


static void dump(uint32 addr, int sections)
{
    int i;
    unsigned char *x;
    
    for(i=0;i<sections;i++){
        x = (unsigned char *) (addr + 16*i);
        kprintf("%x: %X %X %X %X  %X %X %X %X  %X %X %X %X  %X %X %X %X",
                addr+16*i,x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],
                x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15]);
    }
}

static char *hex = "0123456789abcdef";

uint32 readhex(char *s)
{
    uint32 n=0;
    char *x;
    while(*s) {
        x = hex;
        while(*x) {
            if(*x == *s){
                n = n*16 + (x - hex);
                break;
            }
            x++;
        }
        s++;
    }
    return n;
}

uint32 readnum(char *s)
{
	uint32 n=0;
	if((*s == '0') && (*(s+1) == 'x')) return readhex(s+2);
	//while(isdigit(*s)) {
	while(*s) {
		n = n*10 + (*s - '0');
		s++;
	}
	return n;
}

void reboot(void)
{
    i386lidt(0,0);
    asm("int $0");
}

extern aspace_t *flat;

void dumpaddr(int id)
{
    anode_t *an;
	aspace_t *a = rsrc_find_aspace(id);
	
	if(id == 0){
		aspace_kprint(flat);
		return;
	}
	
	if(!a) {
		kprintf("no such aspace %X",id);
		return;
	}
	
	aspace_print(a);
    for(an = a->areas; an; an = an->next){
        kprintf("area %U virtaddr %x size %x pgroup %x",
                an->area->rsrc.id, 
                an->area->virt_addr * 0x1000, 
                an->area->length * 0x1000, 
                an->area->pgroup);
    }
    	
}

void memory_status(void);
void print_regs(regs *r, uint32 eip, uint32 cs, uint32 eflags);


static char linebuf[80];
void k_debugger(regs *r,uint32 eip, uint32 cs, uint32 eflags)
{
    char *line;
    uint32 n;
	
    kprintf("OpenBLT Kernel Debugger");

#if 0
	asm("pushl %%ds; popl %0":"=r"(n));
	kprintf("ds = %x",n);
	asm("pushl %%cs; popl %0":"=r"(n));
	kprintf("cs = %x",n);
	asm("pushl %%ss; popl %0":"=r"(n));
	kprintf("ss = %x",n);
#endif	

    for(;;){
        krefresh();
        line = kgetline(linebuf,80);
		
        if(!strcmp(line,"resources")) { dumprsrc(resource_list); continue; }
        if(!strcmp(line,"tasks")) { dumptasks(); continue; }
        if(!strncmp(line,"task ",5)) { dumptask(readnum(line+5)); continue; }
        if(!strcmp(line,"ports")) { dumpports(); continue; }
        if(!strcmp(line,"memory")) { memory_status(); continue; }
        if(!strcmp(line,"trace")) { trace(r->ebp,eip); continue; }
        if(!strcmp(line,"regs")) { print_regs(r,eip,cs,eflags); continue; }
        if(!strncmp(line,"dump ",5)) { dump(readnum(line+5),16); continue; }
        if(!strncmp(line,"aspace ",7)) { dumpaddr(readnum(line+7)); continue; }
        if(!strcmp(line,"reboot")) { reboot(); }
        if(!strcmp(line,"exit")) break;
        if(!strcmp(line,"x")) break;
        if(!strcmp(line,"c")) break;
    }
}

void DEBUGGER(void)
{
	regs r;
	k_debugger(&r, 0, 0, 0);
}

#endif   
