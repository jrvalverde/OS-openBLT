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

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "types.h"
#include "i386.h"
#include "aspace.h"
#include "task.h"
#include "port.h"
#include "sem.h"
#include "team.h"

#include <blt/types.h>
#include <blt/error.h>

void panic(char *reason);

task_t *new_thread(team_t *team, uint32 ip, int kernel);

int brk(uint32 addr);

void destroy_kspace(void);

/* debugger functions */
void kprintf_init(void);
void kprintf(const char *fmt, ...);
char *kgetline(char *line, int maxlen);
void krefresh(void);

#ifdef SERIAL
void dprintf_init(void);
void dprintf(const char *fmt, ...);
#endif

void preempt(task_t *t, int status);
void swtch(void);
extern char *idt, *gdt;
extern uint32 _cr3;

extern int kernel_timer;

extern task_t *current;
extern resource_t *run_queue;
extern resource_t *reaper_queue;
extern resource_t *timer_queue;
extern team_t     *kernel_team;

#endif
