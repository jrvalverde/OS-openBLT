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
#include "list.h"

team_t *team_create(void)
{
	team_t *team = kmalloc(team_t);
	
	if(!(team->aspace = aspace_create())) return NULL;

	list_init(&team->resources);
	team->refcount = 0;
	team->text_area = 0;
	team->heap_id = 0;
	
	rsrc_bind(&team->rsrc, RSRC_TEAM, kernel_team);
	rsrc_set_owner(&team->aspace->rsrc, team);

	return team;
}

void team_destroy(team_t *team)
{
	resource_t *rsrc;
	
	kprintf("death to team #%d (%s)",team->rsrc.id,team->rsrc.name);
//	DEBUGGER();
	
	while(rsrc = list_remove_head(&team->resources)){
		rsrc->owner = NULL;
//		kprintf("- %x %s %d",rsrc,rsrc_typename(rsrc),rsrc->id);
		
		switch(rsrc->type){
		case RSRC_TASK:
			kprintf("oops... cannot destroy team %d because of task %d!?",
					team->rsrc.id,rsrc->id);
			DEBUGGER();
		case RSRC_PORT:
			port_destroy(rsrc->id);
			break;
		case RSRC_SEM:
			sem_destroy(rsrc->id);
			break;
		case RSRC_ASPACE:
			aspace_destroy((aspace_t*) rsrc);
			break;
		case RSRC_AREA:
		case RSRC_QUEUE:
		case RSRC_TEAM:
			/* skip for now - teams don't get destroyed, areas and queues get
			destroyed with their relatives */
			break;
			
		default:
			kprintf("what the hell is %d (type %d)?",rsrc->id,rsrc->type);
		}
	}
	
	rsrc_release(team);
	kfree(team_t, team);
}

