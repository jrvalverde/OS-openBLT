/* Copyright 1998-1999, Brian J. Swetland. All rights reserved.
** Distributed under the terms of the OpenBLT License
*/

#ifndef _PORT_H_
#define _PORT_H_

#include "resource.h"
#include "types.h"

#define PORT_OPT_NOWAIT        1
#define PORT_OPT_SETRESTRICT   2
#define PORT_OPT_SETDEFAULT    3
#define PORT_OPT_SLAVE         4

typedef struct __msg_hdr_t {
    int flags;
    int src;
    int dst;
    int size;
    void *data;    
} msg_hdr_t;

typedef struct __chain_t 
{
    struct __chain_t *next;
} chain_t;

typedef struct __message_t {
    struct __message_t *next;
    uint32 size;
    int from_port;
    int to_port;    
    void  *data;    
} message_t;

struct __port_t 
{
	resource_t rsrc;
	
    int msgcount;          /* number of messages waiting in the queue */
    int slaved;            /* deliver my messages to a master port */     
    message_t *first;      /* head of the queue */
    message_t *last;       /* tail of the queue */
    int refcount;          /* counts owner and any slaved ports */
	int nowait;            /* do we block on an empty queue? */
	int restrict;          /* is port public or private? */
	resource_t *sendqueue;
};


int port_create(int restrict, const char *name);
int port_destroy(int port);
uint32 port_option(uint32 port, uint32 opt, uint32 arg);
int port_send(msg_hdr_t *mh);
int port_recv(msg_hdr_t *mh);

#endif
