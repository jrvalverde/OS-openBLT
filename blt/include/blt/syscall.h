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

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <blt/types.h>
#include <blt/os.h>

int   os_thread(void *addr);
void  os_terminate(int status);
void  os_console(char *string);
int   os_brk(int addr);
void  os_handle_irq(int irq);
void  os_sleep_irq(void);
void  os_debug(void);
void  os_sleep(int ticks);
int   os_identify(int rsrc); /* returns the thread_id of the owner of a resource */

int sem_create(int value);
int sem_destroy(int sem);
int sem_acquire(int sem);
int sem_release(int sem);

typedef struct {
    int flags;
    int src;
    int dst;
    int size;
    void *data;    
} msg_hdr_t;

int port_create(int restrict);
int port_destroy(int port);
int port_option(int port, uint32 opt, uint32 arg);
int port_send(msg_hdr_t *mh);
int port_recv(msg_hdr_t *mh);

#define port_set_restrict(port, restrict) port_option(port,PORT_OPT_SETRESTRICT,restrict);
#define port_slave(master, slave) port_option(slave,PORT_OPT_SLAVE,master)

int thr_create(void *addr, void *data);
int thr_resume(int thr_id);
int thr_suspend(int thr_id);
int thr_kill(int thr_id);
int thr_detach(void (*addr)(void));
int thr_join(int thr_id, int options);
int thr_spawn (int area_id, int virt, char * const *argv, char * const *envp);

int area_create(off_t size, off_t virt, void **addr, uint32 flags);
int area_clone(int area_id, off_t virt, void **addr, uint32 flags);
int area_destroy(int area_id);
int area_resize(int area_id, off_t size);   

int right_create(int rsrc_id, uint32 flags);
int right_destroy(int right_id);
int right_revoke(int right_id, int thread_id); 
int right_grant(int right_id, int thread_id);

typedef union
{
	thread_info t_info;
	sys_info s_info;
} rsrc_info;

/* look up a resource by name or id number and fill in information about it */
int rsrc_find_id (rsrc_info *info, int rsrc_id, int rsrc_type);
int rsrc_find_name (rsrc_info *info, const char *name, int rsrc_type);
 
#endif
