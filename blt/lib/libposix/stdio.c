/* Copyright 1999, Sidney Cammeresi. All rights reserved.
** Distributed under the terms of the OpenBLT License
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <blt/fdl.h>
#include <blt/syscall.h>
#include <blt/namer.h>
#include <blt/libsyms.h>

static int input_remote_port, input_local_port;
static fdl_type console_input_fdl_imp = { "console_input", _console_read,
    NULL, NULL, NULL };

FILE *stdin, *stdout, *stderr;

weak_alias (_getc, getc)
weak_alias (_getchar, getchar)

void __libc_init_console_input ()
{
	int fd;

	input_remote_port = namer_find ("console_input", 0);
	input_local_port = port_create (input_remote_port, "input_remote_port");
	fd = _fdl_alloc_descriptor (&console_input_fdl_imp, NULL);
	if (fd)
		_printf ("__libc_init_input: console input not on fd 0\n");
	stdin = malloc (sizeof (FILE));
	stdin->fd = 0;
}

int _console_read (void *cookie, void *buf, size_t count)
{
	char data;
	msg_hdr_t msg;

	data = count;
	msg.src = input_local_port;
	msg.dst = input_remote_port;
	msg.data = &data;
	msg.size = 1;
	port_send (&msg);

	msg.src = input_remote_port;
	msg.dst = input_local_port;
	msg.data = buf;
	msg.size = count;
	port_recv (&msg);
	return msg.size;
}

int _getc (FILE *stream)
{
	char c;

	_read (stream->fd, &c, 1);
	return c;
}

int _getchar (void)
{
	return _getc (stdin);
}

