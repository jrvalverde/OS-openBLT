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

#include "kernel.h"
#include "memory.h"
#include "boot.h"
#include "port.h"

#include <i386/io.h>
#include <string.h>
#include "resource.h"
#include "aspace.h"
#include "task.h"
#include "smp.h"

#include "assert.h"

void DEBUGGER(void);

void restore_idt(void);
void init_idt(char *idt);

void _context_switch(uint32 *old_stack, uint32 new_stack, uint32 pdbr);
 
aspace_t *flat = NULL;
boot_dir *bdir = NULL;

char *idt;
char *gdt;
char *gdt2;
char *toppage;
task_t *current;
task_t *kernel;
uint32 gdt2len;
uint32 entry_ebp;
uint32 _cr3;
task_t *idle_task;
int live_tasks = 0;

resource_t *run_queue, *timer_queue, *reaper_queue;

uint32 memsize; /* pages */
uint32 memtotal;

static int uberportnum = 0, uberareanum = 0;


void destroy_kspace(void)
{
/*  memory_status();*/
    
    asm("mov %0, %%cr3"::"r" (_cr3));   /* insure we're in kernel map */
    restore_idt();
    
    i386lgdt((uint32)gdt2,gdt2len);    
    asm ("mov %0, %%esp; ret": :"r" (entry_ebp + 4));
}

void panic(char *reason)
{
    kprintf("");
    kprintf("PANIC: %s",reason);    
    asm("hlt");    
}

void idler(void)
{    
    for(;;){
        asm("hlt");        
    }    
}

static aspace_t _flat;
static TSS *ktss;

void init_kspace(int membottom)
{
	uint32 *raw;
	
    /* there's an existing aspace at vaddr 0x803FD000 that was initialized
       by the 2nd stage loader */
	raw = (uint32 *) 0x803FD000;
    flat = &_flat;
	flat->pdir = raw;
	flat->ptab = &raw[1024];
	flat->high = &raw[2048];
	memtotal = memsize;
	
    memsize -= 3; /* three pages already in use */
    
    memory_init(membottom, memsize);

	
    toppage = (char *) kgetpages(3); /* kernel startup stack */
    gdt = (char *) kgetpages(1);
    rsrc_init(kgetpages(6),4096*6);
    

	kernel = (task_t *) kmalloc(task_t);
	ktss = (TSS *) kgetpages(1);
    kernel->flags = tKERNEL;
	kernel->rsrc.id = 0;
	kernel->rsrc.owner = NULL;
	kernel->rsrc.rights = NULL;
	kernel->rsrc.queue_count = 0;
	kernel->rsrc.queue_head = NULL;
	kernel->rsrc.queue_tail = NULL;
    current = kernel;

    init_idt(idt = kgetpages(1));              /* init the new idt, save the old */
    gdt2 = (void *) i386sgdt(&gdt2len);        /* save the old gdt */

    i386SetSegment(gdt + SEL_KCODE,      /* #01 - 32bit DPL0 CODE */
                   0, 0xFFFFF,
                   i386rPRESENT | i386rDPL0 | i386rCODE,
                   i386g4K | i386g32BIT);

    i386SetSegment(gdt + SEL_KDATA,      /* #02 - 32bit DPL0 DATA */
                   0, 0xFFFFF,
                   i386rPRESENT | i386rDPL0 | i386rDATA,
                   i386g4K | i386g32BIT);

    i386SetSegment(gdt + SEL_KDATA2,        /* #02 - 32bit DPL0 DATA */
                   0, 0xFFFFF,
                   i386rPRESENT | i386rDPL0 | i386rDATA,
                   i386g4K | i386g32BIT);

    i386SetSegment(gdt + SEL_UCODE,        /* #03 - 32bit DPL3 CODE */
                   0, 0xFFFFF,
                   i386rPRESENT | i386rDPL3 | i386rCODE,
                   i386g4K | i386g32BIT);

    i386SetSegment(gdt + SEL_UDATA,        /* #04 - 32bit DPL3 DATA */
                   0, 0xFFFFF,
                   i386rPRESENT | i386rDPL3 | i386rDATA,
                   i386g4K | i386g32BIT);

    i386SetSegment(gdt + SEL_KTSS,        /* #05 - DPL0 TSS (Kernel) */
                   (uint32) ktss, 104,
                   i386rPRESENT | i386rDPL0 | i386rTSS,
                   0);
    
    i386lgdt((uint32) gdt, 1024/8);

    asm("mov %%cr3, %0":"=r" (_cr3));
    
        /* setup the kernel TSS to insure that it's happy */
    ktss->cs = SEL_KCODE;
    ktss->ds = ktss->es = ktss->ss = ktss->fs = ktss->gs = SEL_KDATA; 
    ktss->ldts = ktss->debugtrap = ktss->iomapbase = 0;
    ktss->cr3 = _cr3;
	ktss->ss0 = SEL_KDATA;
    ktss->esp1 = ktss->esp2 = ktss->ss1 = ktss->ss2 = 0;
    i386ltr(SEL_KTSS);
}

/* current is running -- we want to throw it on the run queue and
   transfer control to the preemptee */
void preempt(task_t *task, int status)
{
	uint32 *savestack = &(current->esp);

	/* current thread MUST be running, requeue it */	
	if(current != idle_task) rsrc_enqueue(run_queue, current);

	/* transfer control to the preemtee -- it had better not be on any queues */	
    current = task;
	current->status = status;
    current->flags = tRUNNING;
	current->scount++;
	ktss->esp0 = current->esp0;    
	if(*savestack != current->esp){
		_context_switch(savestack, current->esp, current->cr3);
	}
}

void swtch(void)
{
	/* current == running thread */
	uint32 *savestack = &(current->esp);
    
	/* fast path for the case of only one running thread */
	if(!run_queue->queue_count && (current->flags == tRUNNING)) return;
	
#ifdef __SMP__
    if (smp_my_cpu ())
        return;
#endif
    
    if(current->flags == tRUNNING) {
        if(current != idle_task) rsrc_enqueue(run_queue, current);
    }

	/* get the next candidate */
	current = rsrc_dequeue(run_queue);
	
    if(!current) {
        if(live_tasks){
            current = idle_task;
        } else {
            kprintf("");
            kprintf("No runnable tasks.  Exiting.");
            destroy_kspace();
        }
    }
    
    current->flags = tRUNNING;    
    current->scount++;
    ktss->esp0 = current->esp0;
    if(*savestack != current->esp){
        _context_switch(savestack, current->esp, current->cr3);
    }    
}

task_t *new_thread(aspace_t *a, uint32 ip, int kernelspace)
{
    task_t *t;
    int stack;
    void *addr;
    int i;

    for(i=1023;i>0;i--){
        if(!a->ptab[i]) break;
    }
    stack = area_create(a, 4096, i*4096, &addr, 0);
    if(!stack) panic("cannot create a stack area. eek");

    t = task_create(a, ip, i*4096+4092, kernelspace);
    t->ustack = (void *) (i << 12);
    rsrc_set_owner(rsrc_find(RSRC_AREA,stack), t);
    rsrc_bind(&t->rsrc, RSRC_TASK, kernel);
    t->flags = tREADY;
    if(!kernelspace) {
		rsrc_enqueue(run_queue, t);
        live_tasks++;
    }

    return t;
}

int brk(uint32 addr)
{
    int i;
    aspace_t *a = current->addr;
    area_t *area = rsrc_find_area(a->heap_id);
    
    if(area){
        return area_resize(a,a->heap_id, (addr - area->virt_addr) & 0xFFFFF000);
    }
    return ERR_MEMORY;
}

/*
  0 = directory
  1 = bootstrap
  2 = kernel
*/


void go_kernel(void)
{
    task_t *t;
    int i,j,n,len;
    aspace_t *a;
    void *ptr,*src,*phys;
    port_t *uberport;
	area_t *uberarea;

	for (i = 0, len = 1; (bdir->bd_entry[i].be_type != BE_TYPE_NONE); i++){
		len += bdir->bd_entry[i].be_size;
	}
	len *= 0x1000;

    uberport = rsrc_find_port(uberportnum = port_create(0));
	uberarea = rsrc_find_area(uberareanum = area_create_uber(len, 0x100000));
    kprintf("uberport allocated with rid = %d",uberportnum);
    kprintf("uberarea allocated with rid = %d",uberareanum);

	run_queue = rsrc_find_queue(queue_create("run queue"));
	reaper_queue = rsrc_find_queue(queue_create("reaper queue"));
	timer_queue = rsrc_find_queue(queue_create("timer queue"));
	    
    for(i=3;bdir->bd_entry[i].be_type;i++){
        if(bdir->bd_entry[i].be_type != BE_TYPE_CODE) continue;
        
        a = aspace_create();        
        t = new_thread(a, 0x1074 /*bdir->bd_entry[i].be_code_ventr*/, 0);
        current = t;

        phys = (void *) (bdir->bd_entry[i].be_offset*0x1000 + 0x100000);
//        ptr = kgetpages2 (bdir->bd_entry[i].be_size, 3, (uint32 *) &phys);
//        for (j = 0; j < bdir->bd_entry[i].be_size * 0x1000; j++)
//            *((char *) ptr + j) = *((char *) src + j);
        t->text_area = area_create(a,bdir->bd_entry[i].be_size*0x1000,
            0x1000, &phys, 0x1010);
        a->heap_id = area_create(a,0x2000,0x1000 + bdir->bd_entry[i].be_size*
            0x1000, &ptr, 0);

        /* make the thread own it's address space */
        rsrc_set_owner(&a->rsrc, t);

        if (!strcmp (bdir->bd_entry[i].be_name, "namer")) {
            rsrc_set_owner(&uberport->rsrc, t);
        }
        
		rsrc_set_name((resource_t*)t,bdir->bd_entry[i].be_name);
		
        kprintf("task %X @ 0x%x, size = 0x%x (%s)",t->rsrc.id,
                bdir->bd_entry[i].be_offset*4096+0x100000,
                bdir->bd_entry[i].be_size*4096,
                t->rsrc.name);
    }

    kprintf("creating idle task...");    
    a = aspace_create();    
    idle_task = new_thread(a, (int) idler, 1);
    rsrc_set_owner(&a->rsrc, idle_task);
	rsrc_set_name((resource_t*)idle_task,"idler");
	rsrc_set_name((resource_t*)kernel,"kernel");

#ifdef __SMP__
    smp_init ();
#endif

    DEBUGGER();
    kprintf("starting scheduler...");    

#ifdef __SMP__
    if (smp_configured)
    {
        smp_final_setup ();
        kprintf ("smp: signaling other processors");
        smp_begin ();
    }
#endif

    /*
     * when the new vm stuffas are done, we can at this point discard any
     * complete pages in the .text.init and .data.init sections of the kernel
     * by zeroing them and adding them to the free physical page pool.
     */
        
    current = kernel;
    current->flags = tDEAD;
    swtch();
    
    kprintf("panic: returned from scheduler?");
    asm("hlt");
}

struct _kinfo
{
    uint32 memsize;
    uint32 entry_ebp;
    boot_dir *bd;
    unsigned char *params;
} *kinfo = (struct _kinfo *) 0x80000000;

const static char *copyright1 =
	"OpenBLT Release I (built "__DATE__ ", " __TIME__ ")";
const static char *copyright2 =
    "    Copyright (c) 1998-1999 The OpenBLT Dev Team.  All rights reserved.";
 
void kmain(void)
{
    int n,len;
    memsize = ((kinfo->memsize) / 4096) & ~0xff;
    entry_ebp = kinfo->entry_ebp;
    bdir = kinfo->bd;
	
	for (n = 0, len = 1; (bdir->bd_entry[n].be_type != BE_TYPE_NONE); n++)
		len += bdir->bd_entry[n].be_size;

    init_timer();
    init_kspace(256 + len + 16); /* bottom of kernel memory */
    
    kprintf_init();
#ifdef DPRINTF
	dprintf_init();
#endif

    kprintf("");
	kprintf (copyright1);
	kprintf (copyright2);
    kprintf("");
#ifdef DPRINTF
	dprintf ("");
	dprintf (copyright1);
	dprintf (copyright2);
	dprintf ("");
	kprintf ("serial port is in dprintf mode");
	dprintf ("serial port is in dprintf mode");
	dprintf ("");
#endif

    kprintf("system memory 0x%x",memsize*4096);
    
    n = ((uint32) toppage) + 4080;
    asm("mov %0, %%esp"::"r" (n) );
	
	n = SEL_UDATA | 3;
	asm("pushl %0; popl %%ds"::"r" (n));
	asm("pushl %0; popl %%es"::"r" (n));
	asm("pushl %0; popl %%fs"::"r" (n));
	asm("pushl %0; popl %%gs"::"r" (n));
	
    kprintf("kernel space initialized");    
    go_kernel();
}

