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
#include "task.h"

extern char *gdt;
void thread_bootstrap(void);
void kthread_bootstrap(void);

uint32 wait_on(resource_t *rsrc)
{
	task_t *task = current;
	task->status = 0;
	rsrc_enqueue(rsrc, task);	
	swtch();
	return current->status;
}


/* create a new task, complete with int stack */
task_t *task_create(aspace_t *a, uint32 ip, uint32 sp, int kernel) 
{
    task_t *t = kmalloc(task_t);	
	uint32 *SP;
	
	t->kstack = kgetpages(1,7);
    t->cr3  = (((uint32) a->pdir[0]) & 0xFFFFFF00) - 4096;
	t->esp = (uint32) ( ((char *) t->kstack) + 4092 );
	t->esp0 = t->esp;
	t->scount = 0;
	
	/* prep the kernel stack for first switch 
	** SS
	** ESP
	** EFLAGS
	** CS
	** EIP            -- thread_bootstrap will iret into the thread 
	**
	** <thread_bootstrap>
	** EFLAGS 
	** EBP (0)
	** ESI (0)
	** EDI (0)        -- stuff for _context_switch to pop off / return to
	** EBX (0)
	*/
	
	SP = (uint32*) t->esp;
	
	if(kernel) {
		SP--; *SP = SEL_KDATA; 
		SP--; *SP = sp - 4*5;
		SP--; *SP = 0x3202;
		SP--; *SP = SEL_KCODE;
		SP--; *SP = ip;
		SP--; *SP = (uint32) thread_bootstrap;
	} else {
		SP--; *SP = SEL_UDATA | 3; 
		SP--; *SP = sp - 4*5;
		SP--; *SP = 0x3202;
		SP--; *SP = SEL_UCODE | 3;
		SP--; *SP = ip;
		SP--; *SP = (uint32) thread_bootstrap;
	}	
	SP--; *SP = 0x3002;
	SP--; *SP = 0;
	SP--; *SP = 0;
	SP--; *SP = 0;
	SP--; *SP = 0;
	
	t->esp = (uint32) SP;
	
//	kprintf("thr:%x/%x:%d",sp,ip,(kernel ? SEL_KCODE : (SEL_UCODE | 3)));
	
	t->resources = NULL;
    t->addr = a;
    t->irq = 0;
    t->flags = tREADY;
	t->waiting_on = NULL;
	t->queue_next = NULL;
	t->queue_prev = NULL;
    return t;
}

int thr_spawn (int area_id, int addr, char * const *argv, char * const *envp,
	volatile uint32 **stack)
{
	int i, temp_area;
	void *src, *dst, *phys;
	area_t *text;
	aspace_t *a;

	if ((text = rsrc_find_area (area_id)) == NULL)
		return -1;

	rsrc_set_name((resource_t*)current,argv[0]);
	a = current->addr;

	/* beware, the old argv and envp are no good any longer. */
	aspace_clr (a, 0, 512);
	src = (void *) (text->virt_addr << 12);
	dst = kgetpages2 (text->length, 3, (uint32 *) &phys);
	current->text_area = area_create (a, text->length * 0x1000, addr,
		&phys, 0x1010);
	for (i = 0; i < text->length * 0x1000; i++)
		*((char *) dst + i) = *((char *) src + i);
	a->heap_id = area_create (a, 0x2000, 0x1000 + text->length * 0x1000,
		&phys, 0);

	return 0;
}

