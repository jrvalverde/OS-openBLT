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

#include "kernel.h"
#include "memory.h"

static unsigned char size_map[513] = {
    KM16,                                      
    
    KM16, KM16, KM16, KM16, KM16, KM16, KM16, KM16,
    KM16, KM16, KM16, KM16, KM16, KM16, KM16, KM16,

    KM32, KM32, KM32, KM32, KM32, KM32, KM32, KM32,
    KM32, KM32, KM32, KM32, KM32, KM32, KM32, KM32,

    KM64, KM64, KM64, KM64, KM64, KM64, KM64, KM64,
    KM64, KM64, KM64, KM64, KM64, KM64, KM64, KM64,
    KM64, KM64, KM64, KM64, KM64, KM64, KM64, KM64,
    KM64, KM64, KM64, KM64, KM64, KM64, KM64, KM64,

    KM96, KM96, KM96, KM96, KM96, KM96, KM96, KM96,
    KM96, KM96, KM96, KM96, KM96, KM96, KM96, KM96,
    KM96, KM96, KM96, KM96, KM96, KM96, KM96, KM96,
    KM96, KM96, KM96, KM96, KM96, KM96, KM96, KM96,
	
    KM128, KM128, KM128, KM128, KM128, KM128, KM128, KM128,
    KM128, KM128, KM128, KM128, KM128, KM128, KM128, KM128,
    KM128, KM128, KM128, KM128, KM128, KM128, KM128, KM128,
    KM128, KM128, KM128, KM128, KM128, KM128, KM128, KM128,

	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
	KM192, KM192, KM192, KM192, KM192, KM192, KM192, KM192,
    
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
    KM256, KM256, KM256, KM256, KM256, KM256, KM256, KM256,
};


void *kmallocB(int size)
{
	if(size < 256) return kmallocP(size_map[size]);
	panic("invalid kmallocB");
}

void kfreeB(int size, void *block)
{
	if(size < 256) return kfreeP(size_map[size],block);
	panic("invalid kmallocB");
}



struct km_mnode 
{
    struct km_mnode *next;    
};

static struct _km_map
{
    int size;
    int used_count;
    int fresh_count;
    int free_count;
    struct km_mnode *free_chain;
    void *fresh_chain;    
} km_map[KMMAX] = {
    {   16,   0, 256, 0, NULL, NULL },
    {   32,   0, 128, 0, NULL, NULL },
    {   64,   0,  64, 0, NULL, NULL },
	{   96,   0,  42, 0, NULL, NULL },
    {  128,   0,  32, 0, NULL, NULL },
    {  192,   0,  21, 0, NULL, NULL },
    {  256,   0,  16, 0, NULL, NULL } 
};        

extern uint32 memsize;
extern uint32 memtotal;

void memory_status(void)
{
    int i;
	int inuse = 0, allocated = 0;
	
    kprintf("");
    kprintf("size used free fresh");    
    for(i=0;i<KMMAX;i++){
        kprintf("%U %U %U %U", km_map[i].size, km_map[i].used_count,
                km_map[i].free_count, km_map[i].fresh_count);
		inuse += km_map[i].size * km_map[i].used_count;
		allocated += km_map[i].size * 
		    (km_map[i].free_count+km_map[i].used_count+km_map[i].fresh_count);
		
    }
	inuse /= 1024;
	allocated /= 1024;
	kprintf("");
	kprintf("%Ukb allocated, %Ukb in use",allocated,inuse);
	kprintf("%U (of %U) pages in use",memtotal-memsize,memtotal);
	 
}

#if 0
/* return phys page number of first page of allocated group */
uint32 getpages(int count)
{
    memsize -= count;
    Assert(memsize > 512);
    
    return memsize;
}

/* alloc count physical pages, map them into kernel space, return virtaddr AND phys */
void *kgetpages2(int count, int flags, uint32 *phys)
{
    nextmem -= 4096*count;
    *phys = getpages(count);
    aspace_maphi(flat, *phys, nextmem/0x1000, count, flags);
    *phys *= 4096;
    return (void *) nextmem;
}

/* alloc count physical pages, map them into kernel space, return virtaddr */
void *kgetpages(int count, int flags)
{
    nextmem -= 4096*count;
    aspace_maphi(flat, getpages(count), nextmem/0x1000, count, flags); 
    return (void *) nextmem;
}
#endif

/* kernel 'heap' is allocated top down ... top three pages used by the bootstrap */
static uint32 nextmem = 0x80400000 - 4*4096;

static uint32 *pagelist = NULL;
static uint32 freepage = 0;

extern aspace_t *flat;

void putpage(uint32 number)
{
	pagelist[number] = freepage;
	freepage = number;
}

uint32 getpage(void)
{
	uint32 n = freepage;
	if(n){
		freepage = pagelist[n];
	} else {
		asm("hlt");
		
		panic("Out of physical memory");
	}
	return n;
}

void kfreepage(void *vaddr)
{
}

void *kgetpage(uint32 *phys)
{
	uint32 pg = getpage();
	*phys = pg * 0x1000;
	nextmem -= 4096;
	aspace_maphi(flat, pg, nextmem/0x1000, 1, 3);
	return (void *) nextmem;
}

void *kgetpages(int count)
{
	int i,n;
    nextmem -= 4096*count;
	for(n=nextmem/0x1000,i=0;i<count;n++,i++){
		aspace_maphi(flat, getpage(), n, 1, 3); 
	}
    return (void *) nextmem;
}

/* map specific physical pages into kernel space, return virtaddr */
void *kmappages(int phys, int count, int flags)
{
    nextmem -= 4096*count;
    aspace_maphi(flat, phys, nextmem/0x1000, count, flags); 
    return (void *) nextmem;
}

void memory_init(uint32 bottom_page, uint32 top_page) 
{
	int i,count;
	
	/* we can track 1024 pages for every 4K of pagelist */
	count = ((top_page - bottom_page) / 1024) + 1;
	
	/* allocate the pagelist */
	top_page -= count;
	
#if 0
	top_page = 2048+32;
	count = 4;
#endif
	
	nextmem -= 4096*count;
	pagelist = (uint32 *) nextmem;
	aspace_maphi(flat, top_page, nextmem/0x1000, count, 3);

#if 0
	bottom_page = 1024;
	top_page = 2048;	
#endif
		
	/* setup the pagelist */
	freepage = 0;
	for(i=top_page;i>=bottom_page;i--){
		pagelist[i] = freepage;
		freepage = i;
	}
	
	/* setup the memory pools */
    for(i=0;i<KMMAX;i++){
        km_map[i].fresh_chain = kgetpages(1);
	}
}


void *kmallocP(int size)
{
    struct _km_map *km;    
    void *block;    
    if(size >= KMMAX) panic("illegal size in kmalloc()");
    km = &km_map[size];

    km->used_count++;

    if(km->free_chain) {
            /* recycle free'd blocks if available */
        km->free_count--;
        block = (void *) km->free_chain;
        km->free_chain = km->free_chain->next;
    } else {
            /* shave a new block off of the fresh page if
               we can't recycle */
        km->fresh_count--;
        block = km->fresh_chain;

        if(km->fresh_count){
                /* still some left, just bump us to the next chunk */
            km->fresh_chain = (void *)
                (((char *) km->fresh_chain) + km->size);        
        } else {
                /* gotta grab a new page */
            km->fresh_count = 4096 / km->size;        
            km->fresh_chain = kgetpages(1);            
        }
    }
    
    return block;    
}

void kfreeP(int size, void *block)
{
    struct _km_map *km;    
    if(size > KMMAX) panic("illegal size in kmalloc()");
    km = &km_map[size];
    
    km->free_count++;    
    km->used_count--;
    ((struct km_mnode *) block)->next = km->free_chain;    
    km->free_chain = (struct km_mnode *) block;    
}

