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

#include <stdlib.h>
#include <blt/syscall.h>
#include <blt/qsem.h>

qsem_t *qsem_create (int count)
{
	qsem_t *s;

	s = (qsem_t *) malloc (sizeof (qsem_t));
#ifdef I386
	s->mutex = sem_create (count);
	s->count = 0;
#else
	s->mutex = sem_create (0);
	s->count = count;
#endif
	return s;
}

void qsem_destroy (qsem_t *s)
{
	sem_destroy (s->mutex);
	free (s);
}

void qsem_acquire (qsem_t *s)
{
#ifdef I386
	/* no atomic_add() in i386 */
	sem_acquire(s->mutex);
#else
	if (atomic_add (&s->count, -1) <= 0)
		sem_acquire (s->mutex);
#endif
}

void qsem_release (qsem_t *s)
{
#ifdef I386
	/* no atomic_add() in i386 */
	sem_release(s->mutex);
#else
	if (atomic_add (&s->count, 1) < 0)
		sem_release (s->mutex);
#endif
}

