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
#include <errno.h>

const static char *copyright = "\
OpenBLT Release I (built " __DATE__ ", " __TIME__ ")
    Copyright (c) 1998-1999 The OpenBLT Dev Team.  All rights reserved.
";

char **params;

void run (void)
{
	extern char _end[], __bss_start[];

    __libc_init_console ();
    __libc_init_vfs ();
    execve (params[0], params, NULL);
	if ((errno == ENOENT) || (errno == ENOTDIR) || (errno == ENOSYS))
		printf ("bltsh: no such file or directory: %s\n", params[0]);
	else
    	printf ("execve failed %d\n", errno);
    os_terminate (1);
}

int main (void)
{
	char line[256], *c;
	int fd, len, space, i, p_argc;

	__libc_init_console ();
	__libc_init_vfs ();
	__libc_init_console_input ();

	//printf ("\n\n%s\n", copyright);

	for (;;)
	{
		printf ("$ ");

		*line = len = 0;
		while (read (0, line + len++, 1) > 0)
		{
			if ((line[len - 1] == 8) && (len > 1)) /* BS */
				len -= 2;
			else if (line[len - 1] == '\n')
			{
				line[len-- - 1] = 0;
				for (i = space = 0, p_argc = 2; i < len; i++)
					if ((line[i] == ' ') && !space)
						space = 1;
					else if ((line[i] != ' ') && space)
					{
						p_argc++;
						space = 0;
					}
				if ((*line != '#') && *line)
				{
					params = malloc (sizeof (char *) * p_argc);
/*
					params[0] = malloc (strlen (line) + 1);
					strcpy (params[0], line);
					params[1] = NULL;
*/
					c = line;
					for (i = 0; i < p_argc - 1; i++)
					{
						for (len = 0; c[len] && (c[len] != ' '); len++) ;
						params[i] = malloc (len + 1);
						strlcpy (params[i], c, len + 1);
						c += len + 1;
					}
					params[p_argc - 1] = NULL;
					if (!strcmp (params[0], "exit"))
						os_terminate (1);
					thr_join (thr_detach (run), 0);
				}
				len = 0;
				break;
			}
		}
	}

	return 0;
}

