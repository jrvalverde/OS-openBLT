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

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"

#define KM16   0
#define KM32   1
#define KM64   2
#define KM96   3
#define KM128  4
#define KM192  5
#define KM256  6
#define KMMAX  7

uint32 getpage(void);                 /* allocate a single physical page */
void putpage(uint32);                 /* release a single physical page */

void *kgetpage(uint32 *phys);         /* allocate one page (and return phys addr) */
void *kgetpages(int count);           /* allocate count pages */

void *kmallocP(int pool);             /* allocate from pool (eg KM16, KM128, ...)*/
void kfreeP(int pool, void *block);   /* release back to pool */

void *kmallocB(int size);             /* allocate from appropriate pool for size */
void kfreeB(int size, void *block);   /* release back to appropriate pool */
#define kmalloc(type) kmallocB(sizeof(type))
#define kfree(type,ptr) kfreeB(sizeof(type),ptr)

void memory_init(uint32 bottom, uint32 top); /* only call once - on kernel launch */
void memory_status(void);             /* dump memory usage information */

#endif

