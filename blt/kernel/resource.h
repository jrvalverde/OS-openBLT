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

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "types.h"

typedef enum 
{
    RSRC_NONE, RSRC_TASK, RSRC_ASPACE, RSRC_PORT, RSRC_SEM, RSRC_RIGHT,
    RSRC_AREA, RSRC_QUEUE, RSRC_MAX
} rsrc_type;

struct __resource_t
{
    uint32 id;               
    rsrc_type type;          
    struct __task_t *owner;  
    struct __rnode_t *rights;
	
	struct __task_t *queue_head;
	struct __task_t *queue_tail;    
	uint32 queue_count;
	uint32 lock; /* unused */
	
	char name[32];
};

/* generic or template dlinklist node */
struct __node_t
{
    struct __node_t *next;
    struct __node_t *prev;
    void *data;
    uint32 _reserved;
};

struct __anode_t
{
    struct __anode_t *next;
    struct __anode_t *prev;
    struct __area_t *area;
    uint32 _reserved;
};

struct __rnode_t
{
    struct __rnode_t *next;
    struct __rnode_t *prev;
    struct __right_t *right;
    uint32 _reserved;
};

struct __tnode_t
{
    struct __tnode_t *next;
    struct __tnode_t *prev;
    struct __task_t *task;
    uint32 _reserved;
};

struct __resnode_t
{
    struct __resnode_t *next;
    struct __resnode_t *prev;
    struct __resource_t *resource;
    uint32 _reserved;
};

/* initialize the resource map */
void rsrc_init(void *map, int size);

/* locate a specifically typed resource */
void *rsrc_find(int type, int id);

/* remove the resource from the table */
void rsrc_release(resource_t *r);

/* assign a portnumber and put it in the resource table */
void rsrc_bind(resource_t *r, rsrc_type type, task_t *owner);

int queue_create(const char *name);

void rsrc_set_owner(resource_t *r, task_t *owner);
void rsrc_set_name(resource_t *r, const char *name);

/* usercall - return the rsrc_id of the owner of the resource */
int rsrc_identify(uint32 id);

void rsrc_enqueue(resource_t *rsrc, task_t *task);
void rsrc_enqueue_ordered(resource_t *rsrc, task_t *task, uint32 wake_time);
task_t *rsrc_dequeue(resource_t *rsrc);
const char *rsrc_typename(resource_t *rsrc);

#define rsrc_find_task(id)   ((task_t *) rsrc_find(RSRC_TASK,   id))
#define rsrc_find_port(id)   ((port_t *) rsrc_find(RSRC_PORT,   id))
#define rsrc_find_aspace(id) ((aspace_t *) rsrc_find(RSRC_ASPACE, id))
#define rsrc_find_sem(id)    ((sem_t *) rsrc_find(RSRC_SEM, id))
#define rsrc_find_area(id)   ((area_t *) rsrc_find(RSRC_AREA, id))
#define rsrc_find_right(id)  ((right_t *) rsrc_find(RSRC_RIGHT, id))
#define rsrc_find_queue(id)  ((resource_t *) rsrc_find(RSRC_QUEUE, id));

#endif