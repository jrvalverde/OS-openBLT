/* $Id$ */

/*	$OpenBSD: strcpy.S,v 1.2 1996/09/27 06:47:50 mickey Exp $	*/

/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

/*
 * NOTE: I've unrolled the loop eight times: large enough to make a
 * significant difference, and small enough not to totally trash the
 * cache.
 */

#include <i386/asm.h>

FUNCTION (strcpy)
	movl	4(%esp),%ecx		/* dst address */
	movl	8(%esp),%edx		/* src address */
	pushl	%ecx			/* push dst address */

	.align 2,0x90
L1:	movb	(%edx),%al		/* unroll loop, but not too much */
	movb	%al,(%ecx)
	testb	%al,%al
	jz	L2
	movb	1(%edx),%al
	movb	%al,1(%ecx)
	testb	%al,%al
	jz	L2
	movb	2(%edx),%al
	movb	%al,2(%ecx)
	testb	%al,%al
	jz	L2
	movb	3(%edx),%al
	movb	%al,3(%ecx)
	testb	%al,%al
	jz	L2
	movb	4(%edx),%al
	movb	%al,4(%ecx)
	testb	%al,%al
	jz	L2
	movb	5(%edx),%al
	movb	%al,5(%ecx)
	testb	%al,%al
	jz	L2
	movb	6(%edx),%al
	movb	%al,6(%ecx)
	testb	%al,%al
	jz	L2
	movb	7(%edx),%al
	movb	%al,7(%ecx)
	addl	$8,%edx
	addl	$8,%ecx
	testb	%al,%al
	jnz	L1
L2:	popl	%eax			/* pop dst address */
	ret
