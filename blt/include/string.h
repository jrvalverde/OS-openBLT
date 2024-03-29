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

#ifndef _STRING_H_
#define _STRING_H_

#include <blt/types.h>

#ifdef __cplusplus
extern "C" {
#endif

	char *strcpy(char *dst, const char *src);
	char *strcat(char *dst, const char *src);
	char *strncpy(char *dst, const char *src, size_t size);
	int strncmp(const char *s1, const char *s2, size_t n);
	int strcmp(const char *s1, const char *s2);
	size_t strlen(const char *str);
	char *strchr (const char *cs, int c);
	size_t strlcpy (char *dst, const char *src, size_t size);
	size_t strlcat (char *dst, const char *src, size_t size);
	
	void *memset(void *s, int c, size_t n);
	int memcmp(const void *dst, const void *src, size_t size);
	void *memcpy(void *dst, const void *src, size_t size);
	void *memmove(void *dest, const void *src, size_t n);
	
	int bcmp (const void *s, const void *t, size_t len);
	void bzero (void *b, size_t len);
	
	char *_strerror (int errnum);
	char *strerror (int errnum);

#ifdef __cplusplus
}
#endif


#endif
