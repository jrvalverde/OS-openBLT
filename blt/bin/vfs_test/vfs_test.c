/* $Id$
**
** Copyright 1999 Sidney Cammeresi.
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR `AS IS' AND ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
** NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <blt/syscall.h>
#include <blt/namer.h>
#include <blt/qsem.h>
#include <blt/fdl.h>

/*
extern void __vfs_scroll_area (int, int);

static int input_remote_port, input_local_port;
int _console_read (void *, void *, size_t);
static fdl_type console_input_fdl_imp = { "console_input", _console_read,
	NULL, NULL, NULL };
typedef struct
{
	int fd;
} FILE;
FILE *stdin, *stdout, *stderr;

void __libc_init_input ()
{
	int nh, fd;

	nh = namer_newhandle ();
	input_remote_port = namer_find (nh, "console_input");
	namer_delhandle (nh);
	input_local_port = port_create (input_remote_port);
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

int getc (FILE *stream)
{
	char c;
	msg_hdr_t msg;

	read (stream->fd, &c, 1);
	return c;
}

int getchar (void)
{
	return getc (stdin);
}
*/

int main (void)
{
	int i, j, k;
	msg_hdr_t mh;
	qsem_t *sem;
	DIR *dir1, *dir2;
	struct dirent *dirent;
	struct stat buf;
	char c;
	void *ptr;
	int (*strlen_fn)(char *);

	__libc_init_console ();
	__libc_init_vfs ();

	printf ("hello from vfs_test\n");
/*
	dir1 = opendir ("/");
	while ((dirent = readdir (dir1)) != NULL)
		printf ("readdir says %d %s\n", dirent->d_fileno, dirent->d_name);
	closedir (dir1);
	dir2 = opendir ("/");
	while ((dirent = readdir (dir2)) != NULL)
		printf ("readdir says %d %s\n", dirent->d_fileno, dirent->d_name);
	closedir (dir2);
	dir1 = opendir ("/boot");
	while ((dirent = readdir (dir1)) != NULL)
		printf ("readdir says %d %s\n", dirent->d_fileno, dirent->d_name);
	closedir (dir1);
*/
/*
	i = open ("/boot/rc.boot", O_RDONLY, 0);
	//i = open ("/boot/text", O_RDONLY, 0);
	if (i >= 0)
		while (read (i, &c, 1))
			printf ("%c", c);
*/
/*
	dir1 = opendir ("/portal");
	if (dir1 != NULL)
		while ((dirent = readdir (dir1)) != NULL)
			printf ("readdir says %d %s\n", dirent->d_fileno, dirent->d_name);
	closedir (dir1);
*/
	//i = stat ("/boot/rc.boot", &buf);
/*
	if (i)
		printf ("stat failed\n");
	else
	{
		printf ("stat is good\n");
		printf ("size is %d\n", buf.st_size);
	}
*/
/*
	printf ("going for input...\n");
	for (;;)
	{
		c = getchar ();
		printf ("we got %d\n", c);
	}
*/

	ptr = dlopen ("/boot/libc.so", 0);
	printf ("ptr is %x\n", ptr);
	strlen_fn = dlsym (ptr, "strlen");
	printf ("strlen is %x\n", strlen_fn);
	printf ("len is %d\n", (*strlen_fn) ("foobarbaz"));
	dlclose (ptr);

	__libc_fini_vfs ();
	return 0;
}

