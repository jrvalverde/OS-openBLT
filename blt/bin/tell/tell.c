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
#include <blt/namer.h>

int main (int argc, char **argv)
{
	char *text;
	int i, nh, loc_port, rem_port;
	msg_hdr_t mh;

	if (argc < 3)
	{
		printf ("tell: syntax: tell [server] [message]\n");
		return 0;
	}
	text = malloc (256);
	strlcpy (text, argv[1], 256);
	strlcat (text, ":tell", 256);

	nh = namer_newhandle ();
	rem_port = namer_find (nh, text);
	if (!rem_port)
	{
		printf ("tell: no such server or server not using tell\n");
		return 0;
	}
	loc_port = port_create (rem_port,"port");
	strlcpy (text, argv[2], 256);
	for (i = 3; i < argc; i++)
	{
		strlcat (text, " ", 256);
		strlcat (text, argv[i], 256);
	}

	mh.src = loc_port;
	mh.dst = rem_port;
	mh.data = text;
	mh.size = strlen (text) + 1;
	port_send (&mh);
	mh.src = rem_port;
	mh.dst = loc_port;
	port_recv (&mh);
	return 0;
}

