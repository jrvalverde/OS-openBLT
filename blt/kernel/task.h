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

#ifndef _TASK_H_
#define _TASK_H_

#include "resource.h"
#include "list.h"

#define tKERNEL       0
#define tRUNNING      1
#define tREADY        2
#define tDEAD         3
#define tWAITING      4
#define tSLEEP_IRQ    5
#define tSLEEP_TIMER  6

#define PARANOID 0

#if PARANOID
#define TMAGIC1 0x327610ae
#define TMAGIC2 0x55716621
#endif

struct __task_t {
    resource_t rsrc;

#if PARANOID
	uint32 magic1;
#endif
		
	/* wait_queue support */
	resource_t *waiting_on;
	node_t node;
	int   status; /* status code for the task that has just been awakened */
	uint32 wait_time; /* for timer queues */
	
	uint32 flags;
    uint32 irq;
	uint32 esp; /* saved stack */
	uint32 esp0; /* kernel entry stack -- to stuff in the TSS */
#if PARANOID
	uint32 magic2;
#endif
	uint32 cr3;
	uint32 scount;
	void *kstack, *ustack;

	area_t *stack_area;
#ifdef __SMP__
    int has_cpu, processor, last_processor;
#endif
};

#if PARANOID
#define TSETMAGIC(t) { t->magic1 = TMAGIC1; t->magic2 = TMAGIC2; }
#define TCLRMAGIC(t) { t->magic1 = 0; t->magic2 = 0; }
#define TCHKMAGIC(t) { if((t->magic1 != TMAGIC1) || (t->magic2 != TMAGIC2)) panic("bad thread magic");}
#else
#define TSETMAGIC(t) ((void)0)
#define TCLRMAGIC(t) ((void)0)
#define TCHKMAGIC(t) ((void)0)
#endif

task_t *task_create(team_t *team, uint32 ip, uint32 sp, int kernel);
void task_destroy(task_t *task);
void task_wait_on(task_t *task, resource_t *rsrc);
void task_wake(task_t *task, int status);
int wait_on(resource_t *rsrc);

void task_call(task_t *t);

int thr_kill(int task_id);
int thr_wait(int task_id);
int thr_spawn(uint32 ip, uint32 sp, 
			  uint32 area0, uint32 vaddr0, 
			  uint32 area1, uint32 vaddr1,
			  const char *name);
#endif

