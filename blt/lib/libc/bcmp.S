/*	$OpenBSD: bcmp.S,v 1.2 1996/09/27 06:47:44 mickey Exp $	*/

/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <i386/asm.h>

FUNCTION (bcmp)
	pushl	%edi
	pushl	%esi
	movl	12(%esp),%edi
	movl	16(%esp),%esi
	xorl	%eax,%eax		/* clear return value */
	cld				/* set compare direction forward */

	movl	20(%esp),%ecx		/* compare by words */
	shrl	$2,%ecx
	repe
	cmpsl
	jne	L1

	movl	20(%esp),%ecx		/* compare remainder by bytes */
	andl	$3,%ecx
	repe
	cmpsb
	je	L2

L1:	incl	%eax
L2:	popl	%esi
	popl	%edi
	ret
