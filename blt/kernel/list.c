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

#include "list.h"
#include "memory.h"

#include <blt/error.h>

void list_init(list_t *list)
{
	list->next = (node_t *) list;
	list->prev = (node_t *) list;
	list->count = 0;
}

void list_add_head(list_t *list, void *data)
{
	node_t *node = kmalloc(node_t);
	node->data = data;
	node->next = list->next;
	node->next->prev = node;
	node->prev = (node_t*) list;
	list->next = node;
	list->count++;
}

void list_add_tail(list_t *list, void *data)
{
	node_t *node = kmalloc(node_t);
	node->data = data;
	node->prev = list->prev;
	node->prev->next = node;
	node->next = (node_t*) list;
	list->prev = node;
	list->count++;
}

void *list_peek_head(list_t *list)
{
	if(list->next == (node_t*) list) {
		return NULL;
	} else {
		return list->next->data;
	}
}

void *list_peek_tail(list_t *list)
{
	if(list->prev == (node_t*) list) {
		return NULL;
	} else {
		return list->prev->data;
	}
}

void *list_remove_head(list_t *list)
{
	node_t *node = list->next;
	void *data;
	
	if(node == (node_t*) list){
		return NULL;
	} else {
		data = node->data;
		node->prev->next = node->next;
		node->next->prev = node->prev;
		kfree(node_t, node);
		list->count--;
		return data;
	}
}

void *list_remove_tail(list_t *list)
{
	node_t *node = list->prev;
	void *data;
	
	if(node == (node_t*) list){
		return NULL;
	} else {
		data = node->data;
		node->prev->next = node->next;
		node->next->prev = node->prev;
		kfree(node_t, node);
		list->count--;
		return data;
	}
}

int list_remove(list_t *list, void *data)
{
	node_t *node = list->next;
	while(node != (node_t *) list){
		if(node->data == data){
			node->next->prev = node->prev;
			node->prev->next = node->next;
			kfree(node_t, node);
			list->count--;
			return ERR_NONE;
		}
		node = node->next;
	}
	return -1;
}


void list_attach_head(list_t *list, node_t *node)
{
	node->next = list->next;
	node->next->prev = node;
	node->prev = (node_t*) list;
	list->next = node;
	list->count++;
}

void list_attach_tail(list_t *list, node_t *node)
{
	node->prev = list->prev;
	node->prev->next = node;
	node->next = (node_t*) list;
	list->prev = node;
	list->count++;
}

void *list_detach_head(list_t *list)
{
	node_t *node = list->next;
	if(node == (node_t*) list){
		return NULL;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		list->count--;
		return node->data;
	}
}

void *list_detach_tail(list_t *list)
{
	node_t *node = list->prev;
	if(node == (node_t*) list){
		return NULL;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		list->count--;
		return node->data;
	}
}

int list_detach(list_t *list, void *data)
{
	node_t *node = list->next;
	while(node != (node_t *) list){
		if(node->data == data){
			node->next->prev = node->prev;
			node->prev->next = node->next;
			list->count--;
			return ERR_NONE;
		}
		node = node->next;
	}
	return -1;
}
