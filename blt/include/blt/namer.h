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

#ifndef _NAMER_H_
#define _NAMER_H_

#include <blt/types.h>

#define NAMER_PORT 1

#define OP_WALK    0   /* token, name             -> token */
#define OP_STAT    1   /* token, name             -> token, type, size */
#define OP_CREATE  2   /* token, name, type       -> token, status */
#define OP_DELETE  3   /* token                   -> status */
#define OP_WRITE   4   /* token, type, data[size] -> status */
#define OP_READ    5   /* token, type             -> data[size], status */

typedef unsigned int token_t;

typedef struct 
{
	token_t token;
	uint32 type;
	uint32 size;
} token_info_t;

#define TOKEN_NONE 0
#define TOKEN_ROOT 1

#define TYPE_NONEXIST  0x00000000
#define TYPE_DIRECTORY 0x5f646972  /* _dir */
#define TYPE_ENTRIES   0x5f656e74  /* _ent */
#define TYPE_INT32     0x5f696e74  /* _int */
#define TYPE_STRING    0x5f737472  /* _str */
#define TYPE_NAME      0x5f6e616d  /* _nam */

#define MAXMSG 1024
#define HDRSIZE 16
#define INFOSIZE 12
#define MAXMSGDATA (MAXMSG - HDRSIZE)

typedef struct
{
	int opcode;
	token_t token;
	uint32 type;
	uint32 size;
	uchar data[4];
} namer_request_t;

#ifdef __cplusplus
extern "C" {
#endif

token_t namer_walk         (token_t token, const char *name);
int     namer_stat         (token_t token, const char *name, token_info_t *info);
token_t namer_create       (token_t token, const char *name, uint32 type);
int     namer_delete       (token_t token);
int     namer_write        (token_t token, uint32 type, uint32 size, void *data);
int     namer_read         (token_t token, uint32 type, uint32 *size, void *data);

token_t namer_resolve      (const char *path, int create);

int     namer_read_data    (const char *path, uint32 type, uint32 *size, void **data);
int     namer_write_data   (const char *path, uint32 type, uint32 size, void *data);
int     namer_read_int32   (const char *path, int32 *value);
int     namer_write_int32  (const char *path, int32 value);
char *  namer_read_string  (const char *path);
int     namer_write_string (const char *path, const char *string);

int     namer_newhandle    (void);
int     namer_delhandle    (int nh);
int     namer_register     (int nh, int port, char *name);
int     namer_find         (int nh, char *name);

#endif
#ifdef __cplusplus
}

#endif

