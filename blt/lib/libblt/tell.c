/* $Id$
**
** Copyright 1999 Sidney Cammeresi
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <blt/syscall.h>
#include <blt/libsyms.h>
#include <blt/namer.h>
#include <blt/tell.h>

static char *name;
static volatile int ready = 0;
static void (*callback)(const char *);

weak_alias (_tell_init, tell_init)

void __tell_impl (void)
{
	char *buf, junk;
	int port, nh, len;
	msg_hdr_t mh;

	port = port_create (0, "tell_listen_port");
	buf = malloc (len = TELL_MAX_LEN);
	strlcpy (buf, name, len);
	strlcat (buf, ":tell", len);
	nh = namer_newhandle ();
	namer_register (nh, port, buf);
	namer_delhandle (nh);
	ready = 1;

	for (;;)
	{
		mh.src = 0;
		mh.dst = port;
		mh.data = buf;
		mh.size = len;
		port_recv (&mh);
		(*callback) (buf);
		mh.dst = mh.src;
		mh.src = port;
		mh.data = &junk;
		mh.size = 1;
		port_send (&mh);
	}
}

int _tell_init (char *n, void (*c)(const char *))
{
	name = n;
	callback = c;
	os_thread (__tell_impl);
	while (!ready) ;
	return ready;
}

