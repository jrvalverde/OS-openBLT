/* $Id$
**
** Copyright 1998 Brian J. Swetland
** All rights reserved.
** Copyright 1998-1999 Sidney Cammeresi
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
#include <i386/io.h>
#include "kernel.h"
#include "memory.h"
#include "resource.h"
#include "boot.h"
#include "aspace.h"
#include "task.h"
#include "smp.h"

#include <blt/syscall_id.h>

extern task_t *irq_task_map[16];

typedef struct { uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; } regs;
void k_debugger(regs *r, uint32 eip, uint32 cs, uint32 eflags);

extern int live_tasks;

extern boot_dir *bdir;

void terminate(void)
{
    task_t *t = current, *task;
	resnode_t *p;
	extern resnode_t *resource_list;
    
    //kprintf("Task %X terminated.",current->rsrc.id);
    live_tasks--;    
	rsrc_enqueue(reaper_queue,current);
    current->flags = tDEAD;

	p = resource_list;
	while (p != NULL)
	{
		if (p->resource->type == RSRC_TASK)
		{
			task = (task_t *) p->resource;
			if(task->waiting_on == ((resource_t*)current)){
				rsrc_enqueue(run_queue, task);
			}
		}
		p = p->next;
	}

    swtch();
    //kprintf("panic: HUH? Back from the dead? %x / %x",t,current);
    asm("hlt");
}

void sleep(int ticks)
{
	if(ticks) {
		rsrc_enqueue_ordered(timer_queue, current, kernel_timer + ticks);
	}
	swtch();	
}

#define p_uint32(n) (esp[n])
#define p_voidptr(n) ((void *) esp[n])
#define p_charptr(n) ((char *) esp[n])
#define res r.eax
extern char *screen;

void syscall(regs r, volatile uint32 eip, uint32 cs, uint32 eflags,
	volatile uint32 *esp)
{
	char *c, **argv, **temp_argv, **orig_argv;
	int i, j, len, total;
    unsigned int config;
//	kprintf("%d %x:%d #%d@%x",current->rsrc.id,eip,cs,r.eax,(uint32)esp);
	
    switch(r.eax){

    case BLT_SYS_os_thread :
    {
        int i;    
        task_t *t;
        
        t = new_thread(current->addr, p_uint32(1), 0);
		t->text_area = current->text_area;
        for(i=0;current->rsrc.name[i] && i<31;i++) t->rsrc.name[i] = current->rsrc.name[i];
        if(i<30){
            t->rsrc.name[i++] = '+';
        }
        t->rsrc.name[i] = 0;    
        t->rsrc.owner = current;
        
        res = t->rsrc.id;
    }
    break;

    case BLT_SYS_os_terminate :
        //kprintf("task %X: os_terminate(%x)",current->rsrc.id,r.eax);
        if(p_uint32(1) == 0x00420023) {
            kprintf("");
            kprintf("Kernel terminated by user process");
            destroy_kspace();
        }
        terminate();    
        break;

    case BLT_SYS_os_console :
        kprintf("task %X> %s",current->rsrc.id,p_charptr(1));
        break;

    case BLT_SYS_os_brk :
        res = brk(p_uint32(1));
        break;

    case BLT_SYS_os_handle_irq :
            /* thread wants to listen to irq in eax */
        if(p_uint32(1) > 0 && p_uint32(1) < 16) {
            current->irq = p_uint32(1);
            irq_task_map[p_uint32(1)] = current;
        };
        
        eflags |= 2<<12 | 2<<13;
        break;

    case BLT_SYS_os_sleep_irq :
        if(current->irq){
                /* sleep */
            current->flags = tSLEEP_IRQ;
            unmask_irq(current->irq);
        }
        swtch();
        break;

    case BLT_SYS_os_debug :
#ifdef __SMP__
        if (smp_configured)
          {
            config = apic_read (APIC_LVTT);
            apic_write (APIC_LVTT, config | 0x10000);
            smp_begun = 0;
            ipi_all_but_self (IPI_STOP);
          }
#endif
        k_debugger(&r, eip, cs, eflags);
		kprintf("bye bye debugger");
		
#ifdef __SMP__
				if (smp_configured)
          {
            smp_begin ();
            apic_write (APIC_LVTT, config);
          }
#endif
        break;

    case BLT_SYS_os_sleep :
        sleep(p_uint32(1));
        break;
	
	case BLT_SYS_os_identify :
		res = rsrc_identify(p_uint32(1));
		break;
		
    case BLT_SYS_sem_create :
        res = sem_create(p_uint32(1));
        break;

    case BLT_SYS_sem_destroy :
        res = sem_destroy(p_uint32(1));
        break;

    case BLT_SYS_sem_acquire :
        res = sem_acquire(p_uint32(1));
        break;

    case BLT_SYS_sem_release :
        res = sem_release(p_uint32(1));
        break;

    case BLT_SYS_port_create :
        res = port_create(p_uint32(1));
        break;

    case BLT_SYS_port_destroy :
        res = port_destroy(p_uint32(1));    
        break;

    case BLT_SYS_port_option :
        res = port_option(p_uint32(1), p_uint32(2), p_uint32(3));
        break;

    case BLT_SYS_port_send :
        res = port_send(p_voidptr(1));
        break;

    case BLT_SYS_port_recv :
        res = port_recv(p_voidptr(1));
        break;        
		
	case BLT_SYS_right_create :
		res = right_create(p_uint32(1), p_uint32(1));
		break;
		
	case BLT_SYS_right_destroy :
		res = right_destroy(p_uint32(1));
		break;
		
	case BLT_SYS_right_revoke :
		res = right_revoke(p_uint32(1), p_uint32(2));
		break;
		
	case BLT_SYS_right_grant :
		res = right_grant(p_uint32(1), p_uint32(2));
		break;		
        
    case BLT_SYS_area_create :
        res = area_create(current->addr, p_uint32(1), p_uint32(2), (void **)p_voidptr(3), p_uint32(4));
        break;

    case BLT_SYS_area_clone :
        res = area_clone(current->addr, p_uint32(1), p_uint32(2), (void **)p_voidptr(3), p_uint32(4));
        break;
        
    case BLT_SYS_area_destroy :
        res = area_destroy(current->addr, p_uint32(1));
        break;
        
    case BLT_SYS_area_resize :
        res = area_resize(current->addr, p_uint32(1), p_uint32(2));
        break;

	case BLT_SYS_rsrc_find_id :
		break;

	case BLT_SYS_rsrc_find_name :
		break;

	case BLT_SYS_thr_create :
		break;

	case BLT_SYS_thr_resume :
		break;

	case BLT_SYS_thr_suspend :
		break;

	case BLT_SYS_thr_kill :
		break;

	case BLT_SYS_thr_detach :
		res = thr_detach (p_uint32 (1));
		break;

	case BLT_SYS_thr_join :
		res = thr_join (p_uint32 (1), p_uint32 (2));
		break;

	case BLT_SYS_thr_spawn :
		/*
		 * if thr_spawn is successful, the current userspace stack will
		 * be gone when we get back here, so we have to copy the arguments
		 * now in case it works.
		 */
 		i = -1;
		orig_argv = p_voidptr (3);
		while (orig_argv[i] != NULL)
			i++;
		temp_argv = kmallocB (i * sizeof (char *));
		for (j = total = 0; j < i; j++)
		{
			temp_argv[j] = kmallocB (total += len = strlen (orig_argv[j]) + 1);
			strlcpy (temp_argv[j], orig_argv[j], len);
		}

		if (!(res = thr_spawn (p_uint32 (1), p_uint32 (2),
				(char * const *) p_voidptr (3),
				(char * const *) p_voidptr (4), &esp)))
		{
			total = (total & ~3) ? (total & ~3) + 4 : total;
			c = (char *) (0x400000 - total);
			argv = (char **) (c - i * sizeof (char *));
			for (j = 0; j < i; j++)
			{
				strcpy (c, temp_argv[j]);
				argv[j] = c;
				c += strlen (temp_argv[j]) + 1;
			}

			eip = p_uint32 (2) + 0x74;
			esp = (uint32 *) 0x400000 - (total + i * sizeof (char *) + 12);
			*(esp + 1) = i;
			*(esp + 2) = (uint32) argv;
		}

		for (j = 0; j < i; j++)
			kfreeB (strlen (temp_argv[j] + 1), temp_argv[j]);
		kfreeB (i * sizeof (char *), temp_argv);
		break;
    }
}
