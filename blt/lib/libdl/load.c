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
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <blt/libsyms.h>
#include "dl-int.h"

weak_alias (_dlopen, dlopen)
weak_alias (_dlclose, dlclose)
weak_alias (_dlerror, dlerror)

static int error;

void *_dlopen (const char *filename, int flag)
{
	char *c;
	int i, size, fd, res, len;
	struct stat buf;
	lib_t *lib;
	elf32_pgm_hdr_t *pgm;

	if (_stat (filename, &buf))
	{
		errno = ENOENT;
		return NULL;
	}
	size = buf.st_size;
	size = (size & ~3) ? (size & ~3) + 0x1000 : size;
	fd = _open (filename, O_RDONLY, 0);
	if (fd < 0)
		return NULL;
	lib = malloc (sizeof (lib_t));
	lib->area = area_create (size, 0, (void **) &c, 0);
	len = 0;
	while ((res = read (fd, c + len, 0x2000)) > 0)
		len += res;
	close (fd);

	lib->hdr = (elf32_hdr_t *) c;
	pgm = (elf32_pgm_hdr_t *) ((unsigned int) lib->hdr + lib->hdr->e_phoff);
	for (i = 0; i < lib->hdr->e_phnum; i++)
		memmove ((void *) ((unsigned int) lib->hdr + pgm[i].p_vaddr),
			(void *) ((unsigned int) lib->hdr + pgm[i].p_offset),
			pgm[i].p_filesz);
	lib->dynstr = elf_find_section_hdr (lib->hdr, ".dynstr");
	lib->dynstr_data = elf_find_section_data (lib->hdr, ".dynstr");
	lib->dynsym = elf_find_section_hdr (lib->hdr, ".dynsym");
	lib->dynsym_data = elf_find_section_data (lib->hdr, ".dynsym");
	//printf ("str %x sym %x\n", lib->dynstr_data, lib->dynsym_data);
	//printf ("first is %s\n", lib->dynstr_data + lib->dynsym_data[1].st_name);
	return lib;
}

int _dlclose (void *handle)
{
	lib_t *lib;

	lib = handle;
	area_destroy (lib->area);
	free (handle);
	return 0;
}

const char *_dlerror (void)
{
	return NULL;
}

