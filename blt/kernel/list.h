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

#ifndef _LIST_H
#define _LIST_H

#include "types.h"

struct __list_t
{
	node_t *next; /* aka head */
	node_t *prev; /* aka tail */
	uint32 count;
};

/* generic or template dlinklist node */
struct __node_t
{
	node_t *next;
	node_t *prev;
	void *data;
};

void list_init(list_t *list);

/* these functions allocate and destroy node_t's to store the items */
void list_add_head(list_t *list, void *data);
void list_add_tail(list_t *list, void *data);
void *list_peek_head(list_t *list);
void *list_peek_tail(list_t *list);
void *list_remove_head(list_t *list);
void *list_remove_tail(list_t *list);
int list_remove(list_t *list, void *data);

/* these functions are for items that "own" the node_t */
void list_attach_head(list_t *list, node_t *node);
void list_attach_tail(list_t *list, node_t *node);
void *list_detach_head(list_t *list);
void *list_detach_tail(list_t *list);
int list_detach(list_t *list, void *data);

#endif