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
#include "resource.h"

typedef struct _rtab {
    resource_t *resource;
    int next;
} rtab;

static resource_t null_resource;
	
static rtab *rmap;
static uint32 rmax = 0;
static uint32 rfree = 0;

resnode_t *resource_list = NULL;

void rsrc_init(void *map, int size)
{
    int i;

    null_resource.id = 0;
    null_resource.type = RSRC_NONE;
    null_resource.owner = NULL;
    null_resource.rights = NULL;
    
    rfree = 1;    
    rmax = size / sizeof(rtab);
    rmap = (rtab *) map;
    for(i = 0; i < rmax; i++) {
        rmap[i].resource = &null_resource;
        rmap[i].next = i+1;        
    }
    rmap[rmax-1].next = 0;            
}

void *rsrc_find(int type, int id)
{    
    if((id < rmax) && (rmap[id].resource->type == type)) {
        return rmap[id].resource;
    } else {
        return NULL;    
    }
}

void rsrc_set_owner(resource_t *r, task_t *owner) 
{
    resnode_t *rn;
    
    if(r->owner){
            /* unchain it from the owner */
        for(rn = r->owner->resources; rn; rn=rn->next){
            if(rn->resource == r){
                if(rn->prev) {
                    rn->prev->next = rn->next;
                } else {
                    r->owner->resources = rn->next;
                }
                if(rn->next) {
                    rn->next->prev = rn->prev;
                }
                kfree(resnode_t,rn);
                break;
            }
        }		
    }
    
    r->owner = owner;
    
    if(owner){
        rn = (resnode_t *) kmalloc(resnode_t);
        rn->resource = r;
        rn->prev = NULL;
        rn->next = owner->resources;
        owner->resources = rn;	
    }
}


int rsrc_identify(uint32 id) 
{
    if((id >= rmax) || (rmap[id].resource->type == RSRC_NONE)) return 0;
    return rmap[id].resource->owner->rsrc.id; 
}

int queue_create(const char *name)
{
	resource_t *rsrc = (resource_t*) kmalloc(resource_t);
	rsrc_bind(rsrc,RSRC_QUEUE,kernel);
	rsrc_set_name(rsrc,name);
	return rsrc->id;
}

void rsrc_bind(resource_t *rsrc, rsrc_type type, task_t *owner)
{
    uint32 id;
    resnode_t *rn;
    
    if(rfree){
        id = rfree;
        rfree = rmap[rfree].next;
    } else {
        panic("resource exhaustion");
    }
    
    rmap[id].resource = rsrc;
    rsrc->id = id;
    rsrc->type = type;
    rsrc->owner = owner;
    rsrc->rights = NULL;
	rsrc->name[0] = 0;
	rsrc->queue_head = NULL;
	rsrc->queue_tail = NULL;
	rsrc->queue_count = 0;
	rsrc->lock = 0;
	
    if(owner){
        rn = (resnode_t *) kmalloc(resnode_t);
        rn->resource = rsrc;
        rn->prev = NULL;
        rn->next = owner->resources;
		if(owner->resources) owner->resources->prev = rn;
        owner->resources = rn;	
    }
    rn = (resnode_t *) kmalloc(resnode_t);
    rn->resource = rsrc;
    rn->prev = NULL;
    rn->next = resource_list;
	if(resource_list) resource_list->prev = rn;
    resource_list = rn;
}

void rsrc_release(resource_t *r)
{
    uint32 id = r->id;
    resnode_t *rn;
    
	/* unchain it from the global pool */
    for(rn = resource_list; rn; rn=rn->next){
        if(rn->resource == r){
            if(rn->prev) {
                rn->prev->next = rn->next;
            } else {
                resource_list = rn->next;
            }
            if(rn->next) {
                rn->next->prev = rn->prev;
            }
            kfree(resnode_t,rn);
            break;
        }
    }
    
    if(r->owner){
            /* unchain it from the owner */
        for(rn = r->owner->resources; rn; rn=rn->next){
            if(rn->resource == r){
                if(rn->prev) {
                    rn->prev->next = rn->next;
                } else {
                    r->owner->resources = rn->next;
                }
                if(rn->next) {
                    rn->next->prev = rn->prev;
                }
                kfree(resnode_t,rn);
                break;
            }
        }	
    }
	
	/* wait all blocking objects */
	while(r->queue_head){
		task_t *t = rsrc_dequeue(r);
		task_wake(t,ERR_RESOURCE);
	}
    
    r->type = RSRC_NONE;
    r->id = 0;
	rmap[id].resource = &null_resource;
	rmap[id].next = rfree;
    rfree = id;
}

void rsrc_set_name(resource_t *r, const char *name)
{
	if(name){
		int i;
		for(i=0;*name && (i<31);i++){
			r->name[i] = *name;
			name++;
		}
		r->name[i] = 0;
	} else {
		r->name[0] = 0;
	}
}

void rsrc_enqueue_ordered(resource_t *rsrc, task_t *task, uint32 wake_time)
{
	task_t *t = rsrc->queue_head;
	task->wait_time = wake_time;
	task->flags = tWAITING;
	task->waiting_on = rsrc;
	rsrc->queue_count++;
	while(t){
		if(wake_time < t->wait_time){
			/* add before an item (possibly at head of list) */
			task->queue_prev = t->queue_prev;
			task->queue_next = t;
			if(task->queue_prev) {
				task->queue_prev->queue_next = task;
			} else {
				rsrc->queue_head = task;
			}
			t->queue_prev = task;
			return;
		}
		if(!t->queue_next){
			/* add after last item (tail of list) */
			task->queue_prev = t;
			task->queue_next = NULL;
			t->queue_next = task;
			rsrc->queue_tail = task;
			return;
		}
		t = t->queue_next;
	}
	
	/* add the only item */
	rsrc->queue_tail = task;
	rsrc->queue_head = task;
	task->queue_prev = NULL;
	task->queue_next = NULL;
}

void rsrc_enqueue(resource_t *rsrc, task_t *task)
{
	task->wait_time = 0;
	task->flags = tWAITING;
	if(rsrc->queue_tail){
		rsrc->queue_tail->queue_next = task;
		task->queue_prev = rsrc->queue_tail;
		task->queue_next = NULL;
		rsrc->queue_tail = task;
	} else {
		rsrc->queue_tail = task;
		rsrc->queue_head = task;
		task->queue_prev = NULL;
		task->queue_next = NULL;
	}
	rsrc->queue_count++;
	task->waiting_on = rsrc;
}

task_t *rsrc_dequeue(resource_t *rsrc)
{
	task_t *task;
	task = rsrc->queue_head;
	if(task){
		rsrc->queue_head = task->queue_next;
		if(rsrc->queue_head){
			rsrc->queue_head->queue_prev = NULL;
		} else {
			rsrc->queue_tail = NULL;
		}
		rsrc->queue_count--;
		task->queue_next = NULL;
		task->queue_prev = NULL;
		task->waiting_on = NULL;
		task->flags = tREADY;
	}
	return task;
}

const char *rsrc_typename(resource_t *rsrc)
{
	switch(rsrc->type){
	case RSRC_NONE: return "none";
	case RSRC_TASK: return "task";
	case RSRC_ASPACE: return "address space";
	case RSRC_PORT: return "port";
	case RSRC_SEM: return "semaphore";
	case RSRC_RIGHT: return "right";
	case RSRC_AREA: return "area";
	case RSRC_QUEUE: return "queue";
	default: return "????";
	}
}
