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

#ifndef __BLT_OS_H__
#define __BLT_OS_H__

/*
 * this is generic stuff that is used both in userspace and in the kernel
 * that doesn't really fit elsewhere.
 *
 * this file is included in both apps and the kernel, so don't include
 * silly userland files here!
 */

#define BLT_MAX_NAME_LENGTH          32
#define BLT_MAX_CPUS                 8

#define PORT_OPT_NOWAIT        1
#define PORT_OPT_SETRESTRICT   2
#define PORT_OPT_SETDEFAULT    3
#define PORT_OPT_SLAVE         4

#define RIGHT_PERM_READ      0x0001  /* allow 'read' access to something     */
#define RIGHT_PERM_WRITE     0x0002  /* allow 'write' access to something    */
#define RIGHT_PERM_DESTROY   0x0004  /* allow the something to be destroyed  */
#define RIGHT_PERM_ATTACH    0x0008  /* allows other rights to be attached   */
#define RIGHT_PERM_GRANT     0x0010  /* this right may be granted to another */
                                     /* thread by a thread that is not the   */
                                     /* owner                                */
#define RIGHT_MODE_INHERIT   0x0020  /* automatically granted to child       */
#define RIGHT_MODE_DISSOLVE  0x0040  /* When the owner thread terminates,    */
                                     /* the right is destroyed               */

#define AREA_PHYSMAP        0x00001010

#define JOIN_NO_HANG        0x00000001

#ifndef __ASM__

typedef enum
{
	CPU_INTEL_386,
	CPU_INTEL_486,
	CPU_INTEL_PENTIUM,
	CPU_INTEL_PENTIUM_PRO,
	CPU_INTEL_PENTIUM_II,
	CPU_INTEL_PENTIUM_III,
	CPU_MOT_PPC_601,
	CPU_MOT_PPC_603,
	CPU_MOT_PPC_603e,
	CPU_MOT_PPC_604,
	CPU_MOT_PPC_604e
} cpu_type;

typedef struct
{
	cpu_type c_type;
	char c_revision, c_active;
} cpu_info;

typedef struct
{
	char *name;
} thread_info;

typedef struct
{
	int num_cpus;
	cpu_info c_info[BLT_MAX_CPUS];
	char kernel_name[BLT_MAX_NAME_LENGTH];
	char kernel_version[BLT_MAX_NAME_LENGTH];
	char kernel_build_date[BLT_MAX_NAME_LENGTH];
	char kernel_build_time[BLT_MAX_NAME_LENGTH];
} sys_info;

#endif

#endif

