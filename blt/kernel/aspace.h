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

#ifndef _ASPACE_H_
#define _ASPACE_H_

#include "resource.h"

#define SEL_KCODE  (1*8)
#define SEL_KDATA  (2*8)
#define SEL_KDATA2 (3*8)
#define SEL_UCODE  (4*8)
#define SEL_UDATA  (5*8)
#define SEL_KTSS   (6*8)
#define SEL_UTSS   (7*8)

typedef struct __phys_page_t {
    struct __phys_page_t *next;
    uint32 lockcount;
    uint32 addr[6];
} phys_page_t; /* 32 bytes */

typedef struct __pagegroup_t {
    uint32 size;           /* number of pages */
    uint32 refcount;       /* number of areamaps */
    uint32 flags;          /* page info */
    anode_t *areas;        /* areas sharing this pagegroup */
    phys_page_t *pages;    /* physical pages that compose this pagegroup */
    uint32 _pad0;
    uint32 _pad1;
    uint32 _pad2;
} pagegroup_t;  /* 32 bytes */    
        
struct __area_t {
    struct __resource_t rsrc;
    struct __pagegroup_t *pgroup;
    uint32 virt_addr;
    uint32 length;        /* length in pages */
    uint32 maxlength;     /* maxlength in pages */
}; /* 32 bytes */

struct __aspace_t {
    struct __resource_t rsrc;
    anode_t *areas;       /* list of mapped areas            */
    uint32 maxvirt;       /* highest virt addr pagetabs maps */
    uint32 heap_id;       /* this area is the heap... */
        /* platform specific */
    uint32 *pdir;         /* page directory  -- 4k */
    uint32 *ptab;         /* page table      -- 4k */
    uint32 *high;         /* high page table -- 4k (0x8000000 -> ) */
#if 0
    uint32 *pagedir;      /* the page directory */
    uint32 *pagetabs[];   /* pointers to the page tables */
#endif
};


aspace_t *aspace_create(void);
void aspace_map(aspace_t *a, uint32 phys, uint32 virt,
                uint32 len, uint32 flags);
void aspace_maphi(aspace_t *a, uint32 phys, uint32 virt,
                  uint32 len, uint32 flags);
void aspace_clr(aspace_t *a, uint32 virt, uint32 len);
void aspace_print(aspace_t *a);
void aspace_protect(aspace_t *a, uint32 virt, uint32 flags);

/* userland stuff */
int area_create(aspace_t *aspace, off_t size, off_t virt, void **addr, uint32 flags);
int area_clone(aspace_t *aspace, int area_id, off_t virt, void **addr, uint32 flags);
int area_destroy(aspace_t *aspace, int area_id);
int area_resize(aspace_t *aspace, int area_id, off_t size);   


#endif
