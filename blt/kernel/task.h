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
 
#define tKERNEL       0
#define tRUNNING      1
#define tREADY        2
#define tDEAD         3
#define tWAITING      4
#define tSLEEP_IRQ    5
#define tSLEEP_TIMER  6

struct __task_t {
    struct __resource_t rsrc;

	/* wait_queue support */
	struct __resource_t *waiting_on;
	struct __task_t *queue_next;
	struct __task_t *queue_prev;
	int   status; /* status code for the task that has just been awakened */
	uint32 wait_time; /* for timer queues */
	
    char *iomap;
    struct __aspace_t *addr;
	uint32 flags;
    uint32 irq;
	uint32 esp; /* saved stack */
	uint32 esp0; /* kernel entry stack -- to stuff in the TSS */
	uint32 cr3;
	uint32 scount;
	void *kstack, *ustack;
    resnode_t *resources;
    int text_area;
#ifdef __SMP__
    int has_cpu, processor, last_processor;
#endif
};

task_t *task_create(aspace_t *a, uint32 ip, uint32 sp, int kernel);
void task_wait_on(task_t *task, resource_t *rsrc);
void task_wake(task_t *task, int status);
int wait_on(resource_t *rsrc);

void task_call(task_t *t);
int thr_spawn (int area_id, int addr, char * const *argv, char * const *envp,
    volatile uint32 **stack);

#endif

