/* $Id$ */

/* ---------- To make a malloc.h, start cutting here ------------ */

/* 
  A version of malloc/free/realloc written by Doug Lea and released to the 
  public domain.  Send questions/comments/complaints/performance data
  to dl@cs.oswego.edu

* VERSION 2.6.5  Wed Jun 17 15:55:16 1998  Doug Lea  (dl at gee)

   Note: There may be an updated version of this malloc obtainable at
           ftp://g.oswego.edu/pub/misc/malloc.c
         Check before installing!

   Note: This version differs from 2.6.4 only by correcting a
         statement ordering error that could cause failures only
         when calls to this malloc are interposed with calls to
         other memory allocators.

* Why use this malloc?

  This is not the fastest, most space-conserving, most portable, or
  most tunable malloc ever written. However it is among the fastest
  while also being among the most space-conserving, portable and tunable.
  Consistent balance across these factors results in a good general-purpose 
  allocator. For a high-level description, see 
     http://g.oswego.edu/dl/html/malloc.html

* Synopsis of public routines

  (Much fuller descriptions are contained in the program documentation below.)

  malloc(size_t n);
     Return a pointer to a newly allocated chunk of at least n bytes, or null
     if no space is available.
  free(Void_t* p);
     Release the chunk of memory pointed to by p, or no effect if p is null.
  realloc(Void_t* p, size_t n);
     Return a pointer to a chunk of size n that contains the same data
     as does chunk p up to the minimum of (n, p's size) bytes, or null
     if no space is available. The returned pointer may or may not be
     the same as p. If p is null, equivalent to malloc.  Unless the
     #define REALLOC_ZERO_BYTES_FREES below is set, realloc with a
     size argument of zero (re)allocates a minimum-sized chunk.
  memalign(size_t alignment, size_t n);
     Return a pointer to a newly allocated chunk of n bytes, aligned
     in accord with the alignment argument, which must be a power of
     two.
  valloc(size_t n);
     Equivalent to memalign(pagesize, n), where pagesize is the page
     size of the system (or as near to this as can be figured out from
     all the includes/defines below.)
  pvalloc(size_t n);
     Equivalent to valloc(minimum-page-that-holds(n)), that is,
     round up n to nearest pagesize.
  calloc(size_t unit, size_t quantity);
     Returns a pointer to quantity * unit bytes, with all locations
     set to zero.
  cfree(Void_t* p);
     Equivalent to free(p).
  malloc_trim(size_t pad);
     Release all but pad bytes of freed top-most memory back 
     to the system. Return 1 if successful, else 0.
  malloc_usable_size(Void_t* p);
     Report the number usable allocated bytes associated with allocated
     chunk p. This may or may not report more bytes than were requested,
     due to alignment and minimum size constraints.
  malloc_stats();
     Prints brief summary statistics on stderr.
  mallinfo()
     Returns (by copy) a struct containing various summary statistics.
  mallopt(int parameter_number, int parameter_value)
     Changes one of the tunable parameters described below. Returns
     1 if successful in changing the parameter, else 0.

* Vital statistics:

  Alignment:                            8-byte
       8 byte alignment is currently hardwired into the design.  This
       seems to suffice for all current machines and C compilers.

  Assumed pointer representation:       4 or 8 bytes
       Code for 8-byte pointers is untested by me but has worked
       reliably by Wolfram Gloger, who contributed most of the
       changes supporting this.

  Assumed size_t  representation:       4 or 8 bytes
       Note that size_t is allowed to be 4 bytes even if pointers are 8.        

  Minimum overhead per allocated chunk: 4 or 8 bytes
       Each malloced chunk has a hidden overhead of 4 bytes holding size
       and status information.  

  Minimum allocated size: 4-byte ptrs:  16 bytes    (including 4 overhead)
                          8-byte ptrs:  24/32 bytes (including, 4/8 overhead)
                                     
       When a chunk is freed, 12 (for 4byte ptrs) or 20 (for 8 byte
       ptrs but 4 byte size) or 24 (for 8/8) additional bytes are 
       needed; 4 (8) for a trailing size field
       and 8 (16) bytes for free list pointers. Thus, the minimum
       allocatable size is 16/24/32 bytes.

       Even a request for zero bytes (i.e., malloc(0)) returns a
       pointer to something of the minimum allocatable size.

  Maximum allocated size: 4-byte size_t: 2^31 -  8 bytes
                          8-byte size_t: 2^63 - 16 bytes

       It is assumed that (possibly signed) size_t bit values suffice to
       represent chunk sizes. `Possibly signed' is due to the fact
       that `size_t' may be defined on a system as either a signed or
       an unsigned type. To be conservative, values that would appear
       as negative numbers are avoided.  
       Requests for sizes with a negative sign bit will return a
       minimum-sized chunk.

  Maximum overhead wastage per allocated chunk: normally 15 bytes

       Alignnment demands, plus the minimum allocatable size restriction
       make the normal worst-case wastage 15 bytes (i.e., up to 15
       more bytes will be allocated than were requested in malloc), with 
       two exceptions:
         1. Because requests for zero bytes allocate non-zero space,
            the worst case wastage for a request of zero bytes is 24 bytes.
         2. For requests >= mmap_threshold that are serviced via
            mmap(), the worst case wastage is 8 bytes plus the remainder
            from a system page (the minimal mmap unit); typically 4096 bytes.

* Limitations

    Here are some features that are NOT currently supported

    * No user-definable hooks for callbacks and the like.
    * No automated mechanism for fully checking that all accesses
      to malloced memory stay within their bounds.
    * No support for compaction.

* Synopsis of compile-time options:

    People have reported using previous versions of this malloc on all
    versions of Unix, sometimes by tweaking some of the defines
    below. It has been tested most extensively on Solaris and
    Linux. It is also reported to work on WIN32 platforms.
    People have also reported adapting this malloc for use in
    stand-alone embedded systems.

    The implementation is in straight, hand-tuned ANSI C.  Among other
    consequences, it uses a lot of macros.  Because of this, to be at
    all usable, this code should be compiled using an optimizing compiler
    (for example gcc -O2) that can simplify expressions and control
    paths.

  __STD_C                  (default: derived from C compiler defines)
     Nonzero if using ANSI-standard C compiler, a C++ compiler, or
     a C compiler sufficiently close to ANSI to get away with it.
  DEBUG                    (default: NOT defined)
     Define to enable debugging. Adds fairly extensive assertion-based 
     checking to help track down memory errors, but noticeably slows down
     execution.
  REALLOC_ZERO_BYTES_FREES (default: NOT defined) 
     Define this if you think that realloc(p, 0) should be equivalent
     to free(p). Otherwise, since malloc returns a unique pointer for
     malloc(0), so does realloc(p, 0).
  HAVE_MEMCPY               (default: defined)
     Define if you are not otherwise using ANSI STD C, but still 
     have memcpy and memset in your C library and want to use them.
     Otherwise, simple internal versions are supplied.
  USE_MEMCPY               (default: 1 if HAVE_MEMCPY is defined, 0 otherwise)
     Define as 1 if you want the C library versions of memset and
     memcpy called in realloc and calloc (otherwise macro versions are used). 
     At least on some platforms, the simple macro versions usually
     outperform libc versions.
  HAVE_MMAP                 (default: defined as 1)
     Define to non-zero to optionally make malloc() use mmap() to
     allocate very large blocks.  
  HAVE_MREMAP                 (default: defined as 0 unless Linux libc set)
     Define to non-zero to optionally make realloc() use mremap() to
     reallocate very large blocks.  
  malloc_getpagesize        (default: derived from system #includes)
     Either a constant or routine call returning the system page size.
  HAVE_USR_INCLUDE_MALLOC_H (default: NOT defined) 
     Optionally define if you are on a system with a /usr/include/malloc.h
     that declares struct mallinfo. It is not at all necessary to
     define this even if you do, but will ensure consistency.
  INTERNAL_SIZE_T           (default: size_t)
     Define to a 32-bit type (probably `unsigned int') if you are on a 
     64-bit machine, yet do not want or need to allow malloc requests of 
     greater than 2^31 to be handled. This saves space, especially for
     very small chunks.
  INTERNAL_LINUX_C_LIB      (default: NOT defined)
     Defined only when compiled as part of Linux libc.
     Also note that there is some odd internal name-mangling via defines
     (for example, internally, `malloc' is named `mALLOc') needed
     when compiling in this case. These look funny but don't otherwise
     affect anything.
  WIN32                     (default: undefined)
     Define this on MS win (95, nt) platforms to compile in sbrk emulation.
  LACKS_UNISTD_H            (default: undefined)
     Define this if your system does not have a <unistd.h>.
  MORECORE                  (default: sbrk)
     The name of the routine to call to obtain more memory from the system.
  MORECORE_FAILURE          (default: -1)
     The value returned upon failure of MORECORE.
  MORECORE_CLEARS           (default 1)
     True (1) if the routine mapped to MORECORE zeroes out memory (which
     holds for sbrk).
  DEFAULT_TRIM_THRESHOLD
  DEFAULT_TOP_PAD       
  DEFAULT_MMAP_THRESHOLD
  DEFAULT_MMAP_MAX      
     Default values of tunable parameters (described in detail below)
     controlling interaction with host system routines (sbrk, mmap, etc).
     These values may also be changed dynamically via mallopt(). The
     preset defaults are those that give best performance for typical
     programs/systems.


*/

/* Preliminaries */

#ifndef __STD_C
#ifdef __STDC__
#define __STD_C     1
#else
#if __cplusplus
#define __STD_C     1
#else
#define __STD_C     0
#endif /*__cplusplus*/
#endif /*__STDC__*/
#endif /*__STD_C*/

#ifndef Void_t
#if __STD_C
#define Void_t      void
#else
#define Void_t      char
#endif
#endif /*Void_t*/

#if __STD_C
#include <stddef.h>   /* for size_t */
#else
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>    /* needed for malloc_stats */

/*
  Compile-time options
*/

/*
    Debugging:

    Because freed chunks may be overwritten with link fields, this
    malloc will often die when freed memory is overwritten by user
    programs.  This can be very effective (albeit in an annoying way)
    in helping track down dangling pointers.

    If you compile with -DDEBUG, a number of assertion checks are
    enabled that will catch more memory errors. You probably won't be
    able to make much sense of the actual assertion errors, but they
    should help you locate incorrectly overwritten memory.  The
    checking is fairly extensive, and will slow down execution
    noticeably. Calling malloc_stats or mallinfo with DEBUG set will
    attempt to check every non-mmapped allocated and free chunk in the
    course of computing the summmaries. (By nature, mmapped regions
    cannot be checked very much automatically.)

    Setting DEBUG may also be helpful if you are trying to modify 
    this code. The assertions in the check routines spell out in more 
    detail the assumptions and invariants underlying the algorithms.

*/

#if DEBUG 
#include <assert.h>
#else
#define assert(x) ((void)0)
#endif

/*
  INTERNAL_SIZE_T is the word-size used for internal bookkeeping
  of chunk sizes. On a 64-bit machine, you can reduce malloc
  overhead by defining INTERNAL_SIZE_T to be a 32 bit `unsigned int'
  at the expense of not being able to handle requests greater than
  2^31. This limitation is hardly ever a concern; you are encouraged
  to set this. However, the default version is the same as size_t.
*/

#ifndef INTERNAL_SIZE_T
#define INTERNAL_SIZE_T size_t
#endif

/*
  REALLOC_ZERO_BYTES_FREES should be set if a call to
  realloc with zero bytes should be the same as a call to free.
  Some people think it should. Otherwise, since this malloc
  returns a unique pointer for malloc(0), so does realloc(p, 0). 
*/


/*   #define REALLOC_ZERO_BYTES_FREES */

/*
  HAVE_MEMCPY should be defined if you are not otherwise using
  ANSI STD C, but still have memcpy and memset in your C library
  and want to use them in calloc and realloc. Otherwise simple
  macro versions are defined here.

  USE_MEMCPY should be defined as 1 if you actually want to
  have memset and memcpy called. People report that the macro
  versions are often enough faster than libc versions on many
  systems that it is better to use them. 

*/

#define HAVE_MEMCPY 

#ifndef USE_MEMCPY
#ifdef HAVE_MEMCPY
#define USE_MEMCPY 1
#else
#define USE_MEMCPY 0
#endif
#endif

#if (__STD_C || defined(HAVE_MEMCPY)) 

#if __STD_C
void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
#else
Void_t* memset();
Void_t* memcpy();
#endif
#endif

#if USE_MEMCPY

/* The following macros are only invoked with (2n+1)-multiples of
   INTERNAL_SIZE_T units, with a positive integer n. This is exploited
   for fast inline execution when n is small. */

#define MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  INTERNAL_SIZE_T mzsz = (nbytes);                                            \
  if(mzsz <= 9*sizeof(mzsz)) {                                                \
    INTERNAL_SIZE_T* mz = (INTERNAL_SIZE_T*) (charp);                         \
    if(mzsz >= 5*sizeof(mzsz)) {     *mz++ = 0;                               \
                                     *mz++ = 0;                               \
      if(mzsz >= 7*sizeof(mzsz)) {   *mz++ = 0;                               \
                                     *mz++ = 0;                               \
        if(mzsz >= 9*sizeof(mzsz)) { *mz++ = 0;                               \
                                     *mz++ = 0; }}}                           \
                                     *mz++ = 0;                               \
                                     *mz++ = 0;                               \
                                     *mz   = 0;                               \
  } else memset((charp), 0, mzsz);                                            \
} while(0)

#define MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  INTERNAL_SIZE_T mcsz = (nbytes);                                            \
  if(mcsz <= 9*sizeof(mcsz)) {                                                \
    INTERNAL_SIZE_T* mcsrc = (INTERNAL_SIZE_T*) (src);                        \
    INTERNAL_SIZE_T* mcdst = (INTERNAL_SIZE_T*) (dest);                       \
    if(mcsz >= 5*sizeof(mcsz)) {     *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++;                     \
      if(mcsz >= 7*sizeof(mcsz)) {   *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++;                     \
        if(mcsz >= 9*sizeof(mcsz)) { *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++; }}}                 \
                                     *mcdst++ = *mcsrc++;                     \
                                     *mcdst++ = *mcsrc++;                     \
                                     *mcdst   = *mcsrc  ;                     \
  } else memcpy(dest, src, mcsz);                                             \
} while(0)

#else /* !USE_MEMCPY */

/* Use Duff's device for good zeroing/copying performance. */

#define MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  INTERNAL_SIZE_T* mzp = (INTERNAL_SIZE_T*)(charp);                           \
  long mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T), mcn;                         \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mzp++ = 0;                                             \
    case 7:           *mzp++ = 0;                                             \
    case 6:           *mzp++ = 0;                                             \
    case 5:           *mzp++ = 0;                                             \
    case 4:           *mzp++ = 0;                                             \
    case 3:           *mzp++ = 0;                                             \
    case 2:           *mzp++ = 0;                                             \
    case 1:           *mzp++ = 0; if(mcn <= 0) break; mcn--; }                \
  }                                                                           \
} while(0)

#define MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  INTERNAL_SIZE_T* mcsrc = (INTERNAL_SIZE_T*) src;                            \
  INTERNAL_SIZE_T* mcdst = (INTERNAL_SIZE_T*) dest;                           \
  long mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T), mcn;                         \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mcdst++ = *mcsrc++;                                    \
    case 7:           *mcdst++ = *mcsrc++;                                    \
    case 6:           *mcdst++ = *mcsrc++;                                    \
    case 5:           *mcdst++ = *mcsrc++;                                    \
    case 4:           *mcdst++ = *mcsrc++;                                    \
    case 3:           *mcdst++ = *mcsrc++;                                    \
    case 2:           *mcdst++ = *mcsrc++;                                    \
    case 1:           *mcdst++ = *mcsrc++; if(mcn <= 0) break; mcn--; }       \
  }                                                                           \
} while(0)

#endif

/*
  Define HAVE_MMAP to optionally make malloc() use mmap() to
  allocate very large blocks.  These will be returned to the
  operating system immediately after a free().
*/

#ifndef HAVE_MMAP
#define HAVE_MMAP 0
#endif

/*
  Define HAVE_MREMAP to make realloc() use mremap() to re-allocate
  large blocks.  This is currently only possible on Linux with
  kernel versions newer than 1.3.77.
*/

#ifndef HAVE_MREMAP
#ifdef INTERNAL_LINUX_C_LIB
#define HAVE_MREMAP 1
#else
#define HAVE_MREMAP 0
#endif
#endif

#if HAVE_MMAP

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

#endif /* HAVE_MMAP */

/*
  Access to system page size. To the extent possible, this malloc
  manages memory from the system in page-size units.
  
  The following mechanics for getpagesize were adapted from 
  bsd/gnu getpagesize.h 
*/

#ifndef LACKS_UNISTD_H
#  include <unistd.h>
#endif

#if 0
#ifndef malloc_getpagesize
#  ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
#    ifndef _SC_PAGE_SIZE
#      define _SC_PAGE_SIZE _SC_PAGESIZE
#    endif
#  endif
#  ifdef _SC_PAGE_SIZE
#    define malloc_getpagesize sysconf(_SC_PAGE_SIZE)
#  else
#    if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
       extern size_t getpagesize();
#      define malloc_getpagesize getpagesize()
#    else
#      include <sys/param.h>
#      ifdef EXEC_PAGESIZE
#        define malloc_getpagesize EXEC_PAGESIZE
#      else
#        ifdef NBPG
#          ifndef CLSIZE
#            define malloc_getpagesize NBPG
#          else
#            define malloc_getpagesize (NBPG * CLSIZE)
#          endif
#        else 
#          ifdef NBPC
#            define malloc_getpagesize NBPC
#          else
#            ifdef PAGESIZE
#              define malloc_getpagesize PAGESIZE
#            else
#              define malloc_getpagesize (4096) /* just guess */
#            endif
#          endif
#        endif 
#      endif
#    endif 
#  endif
#endif
#endif

#define malloc_getpagesize (0x1000)

/*

  This version of malloc supports the standard SVID/XPG mallinfo
  routine that returns a struct containing the same kind of
  information you can get from malloc_stats. It should work on
  any SVID/XPG compliant system that has a /usr/include/malloc.h
  defining struct mallinfo. (If you'd like to install such a thing
  yourself, cut out the preliminary declarations as described above
  and below and save them in a malloc.h file. But there's no
  compelling reason to bother to do this.)

  The main declaration needed is the mallinfo struct that is returned
  (by-copy) by mallinfo().  The SVID/XPG malloinfo struct contains a
  bunch of fields, most of which are not even meaningful in this
  version of malloc. Some of these fields are are instead filled by
  mallinfo() with other numbers that might possibly be of interest.

  HAVE_USR_INCLUDE_MALLOC_H should be set if you have a
  /usr/include/malloc.h file that includes a declaration of struct
  mallinfo.  If so, it is included; else an SVID2/XPG2 compliant
  version is declared below.  These must be precisely the same for
  mallinfo() to work.

*/

/* #define HAVE_USR_INCLUDE_MALLOC_H */

#if HAVE_USR_INCLUDE_MALLOC_H
#include "/usr/include/malloc.h"
#else

/* SVID2/XPG mallinfo structure */

struct mallinfo {
  int arena;    /* total space allocated from system */
  int ordblks;  /* number of non-inuse chunks */
  int smblks;   /* unused -- always zero */
  int hblks;    /* number of mmapped regions */
  int hblkhd;   /* total space in mmapped regions */
  int usmblks;  /* unused -- always zero */
  int fsmblks;  /* unused -- always zero */
  int uordblks; /* total allocated space */
  int fordblks; /* total non-inuse space */
  int keepcost; /* top-most, releasable (via malloc_trim) space */
};	

/* SVID2/XPG mallopt options */

#define M_MXFAST  1    /* UNUSED in this malloc */
#define M_NLBLKS  2    /* UNUSED in this malloc */
#define M_GRAIN   3    /* UNUSED in this malloc */
#define M_KEEP    4    /* UNUSED in this malloc */

#endif

/* mallopt options that actually do something */

#define M_TRIM_THRESHOLD    -1
#define M_TOP_PAD           -2
#define M_MMAP_THRESHOLD    -3
#define M_MMAP_MAX          -4

#ifndef DEFAULT_TRIM_THRESHOLD
#define DEFAULT_TRIM_THRESHOLD (128 * 1024)
#endif

/*
    M_TRIM_THRESHOLD is the maximum amount of unused top-most memory 
      to keep before releasing via malloc_trim in free().

      Automatic trimming is mainly useful in long-lived programs.
      Because trimming via sbrk can be slow on some systems, and can
      sometimes be wasteful (in cases where programs immediately
      afterward allocate more large chunks) the value should be high
      enough so that your overall system performance would improve by
      releasing.  

      The trim threshold and the mmap control parameters (see below)
      can be traded off with one another. Trimming and mmapping are
      two different ways of releasing unused memory back to the
      system. Between these two, it is often possible to keep
      system-level demands of a long-lived program down to a bare
      minimum. For example, in one test suite of sessions measuring
      the XF86 X server on Linux, using a trim threshold of 128K and a
      mmap threshold of 192K led to near-minimal long term resource
      consumption.  

      If you are using this malloc in a long-lived program, it should
      pay to experiment with these values.  As a rough guide, you
      might set to a value close to the average size of a process
      (program) running on your system.  Releasing this much memory
      would allow such a process to run in memory.  Generally, it's
      worth it to tune for trimming rather tham memory mapping when a
      program undergoes phases where several large chunks are
      allocated and released in ways that can reuse each other's
      storage, perhaps mixed with phases where there are no such
      chunks at all.  And in well-behaved long-lived programs,
      controlling release of large blocks via trimming versus mapping
      is usually faster.

      However, in most programs, these parameters serve mainly as
      protection against the system-level effects of carrying around
      massive amounts of unneeded memory. Since frequent calls to
      sbrk, mmap, and munmap otherwise degrade performance, the default
      parameters are set to relatively high values that serve only as
      safeguards.

      The default trim value is high enough to cause trimming only in
      fairly extreme (by current memory consumption standards) cases.
      It must be greater than page size to have any useful effect.  To
      disable trimming completely, you can set to (unsigned long)(-1);
*/

#ifndef DEFAULT_TOP_PAD
#define DEFAULT_TOP_PAD        (0)
#endif

/*
    M_TOP_PAD is the amount of extra `padding' space to allocate or 
      retain whenever sbrk is called. It is used in two ways internally:

      * When sbrk is called to extend the top of the arena to satisfy
        a new malloc request, this much padding is added to the sbrk
        request.

      * When malloc_trim is called automatically from free(),
        it is used as the `pad' argument.

      In both cases, the actual amount of padding is rounded 
      so that the end of the arena is always a system page boundary.

      The main reason for using padding is to avoid calling sbrk so
      often. Having even a small pad greatly reduces the likelihood
      that nearly every malloc request during program start-up (or
      after trimming) will invoke sbrk, which needlessly wastes
      time. 

      Automatic rounding-up to page-size units is normally sufficient
      to avoid measurable overhead, so the default is 0.  However, in
      systems where sbrk is relatively slow, it can pay to increase
      this value, at the expense of carrying around more memory than 
      the program needs.

*/

#ifndef DEFAULT_MMAP_THRESHOLD
#define DEFAULT_MMAP_THRESHOLD (128 * 1024)
#endif

/*
    M_MMAP_THRESHOLD is the request size threshold for using mmap() 
      to service a request. Requests of at least this size that cannot 
      be allocated using already-existing space will be serviced via mmap.  
      (If enough normal freed space already exists it is used instead.)

      Using mmap segregates relatively large chunks of memory so that
      they can be individually obtained and released from the host
      system. A request serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequesthrICES; LOreusF USEuest DATA reusverhroS; s neUSIugh msed bRUPTION) HOWvicuseAUSEDced tONy anr reusEORYusF r reservi, WHEusERest ed TRA   oSTRICT r reservi, s nTORed threquest se NEGLIGENCE s nOusERWISE)ny
IS se INy an byYusU by ah mmUSEby r reused by any
 ,rvice IF ADVISEDcy ah mmby SI servicy anyCH   ovalerequest nuere_ never H_
p is nev_ never H_
est se*__er reuseluereq);est se_eques (st se*);est se*__ service(st se*y
      );est se*__   ovalueeluereqy
      );est se*__vr reuseluereq);est se*__pvr reuseluereq);est se*__cr reuseluereq);est se_ecques (st se*);e  ovaluerequestseluereq);eluerequest serviced throu (st se*);est sep is never re(st s);e  ov__er repticedtd by any
 d by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by                                       ovaluerequesrequesct serviced through mmap is never reused by                                     ap is never reused by any
     6137luerequest servic46 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest servicccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccany
      ovaluerequest srequesc#4hrough mmap is never reuBeusn J. Sis neve ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequesthrICES; LOreusF USEuest DATA reusverhroS; s neUSIugh msed bRUPTION) HOWvicuseAUSEDced tONy anr reusEORYusF r reservi, WHEusERest ed TRA   oSTRICT r reservi, s nTORed threquest se NEGLIGENCE s nOusERWISE)ny
IS se INy an byYusU by ah mmUSEby r reused by any
 ,rvice IF ADVISEDcy ah mmby SI servicy anyCH   ovalerequestvaluer <stdest.h>estvaluer <unreqd.h>estvaluer <requtypes.h>estvaluer <requsyscr r.h>estvaluer <requestsyms.h>estvaluer "serviced"

weak_ovaare(_ is nervice
weak_ovaare(_ never r nevere
weak_ovaare(_ed by ed be
weak_ovaare(_y any
   y any
 ) ovaluerethrough mmapv__ap ismax;ovaluerethrough mmapv__ap iscur;ovalueremapvsem_ never;
est se*_ap iap isservicis nevifeluer < 0 )r reused b__ap iscuris n never reused       o(st se*)v__ap iscur;osed }st serrough mmap irough mmapvtmap i__ap iscur;osed ed b__ap iscuri+ n never reused ife__ap iscuri>v__ap ismax)r reused bed b__ap isough m__ap iscur;osed ed bbbbbosough(__ap ismax)rough mmap is never       o(st se*)vtmaviced through      o(st se*)v__ap iscur;o}
est se*_ never reused servicis nevst se*red by anm_ac   ov(sem_ neverluerequeh m__ never reus)ed by anm_s never(sem_ neverluereque     or;o}
est se_ed b(st se*y
  
{d by anm_ac   ov(sem_ neverluerequ_eques(y
  ed by anm_s never(sem_ neverlue}
est se*_y any
 (st se*y
 y
      oservicis nevst se*red by anm_ac   ov(sem_ neverluerequeh m__y any
 (y
 yreus)ed by anm_s never(sem_ neverluereque     or;o}
eest se* _    ova_oughcughused oservicis nevst se*rerequ;hrough  requeny
    reus)ed by eused requen=o(st se*)v-1)is never       oNULLuereque     orerequ;h}
est se__requ_roug_sreque( irough mmapvtop_of_viced ,osed ed bbbbbbbbbbbbbbbbb irough mmapvrvice_bsed  irough mmapvbse_lengthicis	r reused bb irough mh mm *gh m( irough mh mm *)vrvice_bsesed bb irough mmapvtoy an(top_of_viced /4096+2)*4096sed bbosough(toyany
	ced throu0h mmapbse_lengthh m++ap ix[i]erviced t b__ap isough m__ap iscur est sb;
	d by anm_er reus= anm_c sere(1,"requ_p is nevem"lue}
y
      ovaluerequest servicccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc  ovaluerequeqsortsct serviced through mmap is never reused by                                      ap is never reused by any
    11630luerequest servi62d through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest requeqsortsc#1 $    any
OpenBSD: qsortsc,v byused 7/06/20 11:19:38 is aadt Exp $    rvicemmap is neve(c)servi,r reucem	rougReghrougp is neUniough tny
  Covacednia. aluerequest serviced t icemm mmap is never reused by any
      ovaluerequest serviced through m ap is never reused by any
      ovaluerequest serviced through mmap i  never reus d by any
      ovaluerequest serviced through mmap is never reused b ovaluerequest serviced through mmap ough mmap is never reused by an 
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap ough mmap is never reused by valuereq est serviced through mmap is never reused by any
      ovaluerequest servi ed thlueradviceiby aner reused mer reuiced  by anslueresmap is  by any
   q est sh mmaerepreusemap is never acequeledgreque:cem	rost serhrou ovaluerey any
    dy anopany
  s neUniough tny
 cem	Covacedniarvierk neyough  mmauereuest map i es4. Neis nevtough mmap is neUniough tnynoevtough mmuereq mmauereuest mapq est sh            ovaluerequest serviced throug is never reused by any
   q est s  ovaluerequest serviced through mmap is ne icemmused by any
      ovaluerequest sREGENTSced ted TRIBUTORSd through mmapcemmis never reusey any
      ovaluerequest serviced through mmap is nevecemm any
      ovaluerovaluerequest serviced through mmap is never reused by acemmi
      ovaluer est serviced through mmREGENTSceused TRIBUTORSdever reuscemmd by any
      ovaluerequ serviced through mmap is never reused by any
   cemm  ovaluerequest servicehrough mmap is never reused by any
      ovaluercemmquesthrICES; LOreusF USEu DATA reusverhroS; s neUSIugh msed bRUPTION)cemmHOWvicuseAUSEDced tONy aneusEORYusF r reservi, WHEusERest ed TRA   oSTRICTcemmr reservi, s nTORehrequest se NEGLIGENCE s nOusERWISE)ny
IS se INy an byYcemmqU by ah mmUSEby eused by any
 ,rvice IF ADVISEDcy ah mmby SI servicy cemmnyCH   ovaler equestvaluer <sysutypes.h>estvaluer <stdest.h>eovaluere__rol nevh mm	*med3ugh mme*y
h mme*y
h mme*y
mapv(*)(ed tvaluere__rol nevst s	by anfuncugh mme*y
h mme*y
edtd by any
  is nevmin(arvb)	(ar reub) ? a : b rvicemmQsortrviced th reusBuereeyo& McIlroy's "Engd ty any
a SortrFunc reu"er equ  is nevy anrvic(TYPEced tmiced tmj, ned b		\
	sed ohrou(n thrreusreq(TYPE);b			\
	y
  hrougTYPEe*yhrou(TYPEe*)valuemi);b		\
	y
  hrougTYPEe*yjrou(TYPEe*)valuemj);b		\
	dod b						\
		y
  hrougTYPE	ueny*yh;		\
		*yh++eny*yj;				\
		*yj++enyt;				\
ugh mmap map le (--i >serv				\
}y
  is nevSWAPINIT(arvquest antypes nevh mm *)arough mm *)0   oreusreused buere\
	mue oreusreused bu? 2 : er reureusreused b? 0 : 1;eovaluere__rol nevst s
y anfunc(arvb,any
  antypeap h mm *arv*b;
	mapvny
  antype;cis	req(t antypes<=p is
		y anrvic(sed uservb,anap ever 		y anrvic(h mmuservb,anap}y
  is nevy an(arvb)					\
	req(t antypesreused 				\
		sed oueny*used o*)(arv			\
		*used o*)(areny*used o*)(b);		\
		*used o*)(b)enyt;			\
	}st se						\
		y anfunc(arvb,aesy
  antypeap
  is nevvecy an(arvb, ned	req((n t>ser y anfunc(arvb,any
  antypeapovaluere__rol nevh mm icmed3(arvb,arequmpap h mm *arv*brv*c;
	mapv(*umpased {
	y
ny
   mn(arvb) < 0 ?
	gh mmap( mn(b,ar) < 0 ? b : ( mn(arvr) < 0 ? c : a eused by any




:( mn(b,ar) > 0 ? b : ( mn(arvr) < 0 ? a : c )lue}
est s
qsort(aa,any
esy
umpap st se*aa;
	ed by any
 thr	mapv(*umpased {
	h mm *parv*pbrv*pcrv*pdrv*plrv*pmrv*pnhr	mapvdrvry
  antypey
  an_cp is	y
  hrough mm *aevera;

loop:	SWAPINIT(arvque;
	e an_cp ervice	req(n < 7ed 
		equestmp is nev *)ar+
 th tmp<is nev *) ar+
rviceth tmp+=vque
			equestis npmh tl > s nev *) arp ismn(tl -
esy
pl) > 0;
			




tl -=vque
				e an(plrvtl -
ese;
		y
ny
 ;
	}
	tmp is nev *)ar+
(n / 2)vicethe	req(n > 7ed 
		tis ns nev *)a;
		pnp is nev *)ar+
(n ap is nethe		req(n > 40ed 
			drou(n / 8is nethe			tis nmed3(plrvtl +vdrvtl +v2s ndy
umpahe			tms nmed3(paluedrvtm, tmp+ndy
umpahe			tns nmed3(pn ap2s ndy
pn apdy
pny
umpahe		}
		tms nmed3(plrvtm, tny
umpahe	}
	e an(arvpalue	paevepbp is nev *)ar+
 th
e	pus= pdp is nev *)ar+
(n ap is nethe	eques;;ed 
		ap le (pbp<=ppused tp is mn(tbrvavalueused e			eusedsreused 
				e an_cp erv1;
				e an(parvpbahe				pae+=vquhe			}e			tbe+=vquhe		}
		ap le (pbp<=ppused tp is mn(tcrvaval>eused e			eusedsreused 
				e an_cp erv1;
				e an(p anydahe				pd -=vquhe			}e			tc -=vquhe		}e		req(pbp>ppue
			break;
		e an(pbrvpc);
		e an_cp erv1;
		tbe+=vquhe		tc -=vquhe	}
	req(t an_cp ereused bby an serv  ovinquesi servrp is 		equestmp is nev *) ar+
 th tmp<is nev *) ar+
rviceth tmp+=vque
			equestis npmh tl > s nev *) arp ismn(tl -
esy
pl) > 0; 
			




tl -=vque
				e an(plrvtl -
ese;
		y
ny
 ;
	}

	pnp is nev *)ar+
ns nethe	p ismin(parough mm *)arvpb
    e;
	vecy an(arvpb
  iced)he	p ismin(p  ovp anyn app  ovese;
	vecy an(pbrvpn
  iced)he	req((revepbp    e > que
		qsort(aced / esy
esy
umpahe	req((revep  ovp e > quever 		ny
 rouany
 as nevtoa orecurequeused byvalver rece is 		aevepn
  i;
		np id / es;
		equesloop;
	}
/*		qsort(pn
  iced / esy
esy
umpahy
   rviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requesnever rsct serviced through mmap is never reused by                                   ap is never reused by any
    13551luerequest serv5317 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest snever rsc through mmap is never reuBeusn J. Sis neve ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequesthrICES; LOreusF USEuest DATA reusverhroS; s neUSIugh msed bRUPTION) HOWvicuseAUSEDced tONy anr reusEORYusF r reservi, WHEusERest ed TRA   oSTRICT r reservi, s nTORed threquest se NEGLIGENCE s nOusERWISE)ny
IS se INy an byYusU by ah mmUSEby r reused by any
 ,rvice IF ADVISEDcy ah mmby SI servicy anyCH   ovalereque/mmap is never r7,uBeusn J. Sis neve < is neve@neoguere>bbbbbbbbbbbbbbbbbr reFues equeicedsed brcsed hroug Sh    any
 njoysed ed bbbbbbbbbbbbbbbbb ever reM   o mmanever r() func reureque%s mmat any




%  ovrough mmapv


%x ov32 thrhexvaluereq(0iced th)eque%c -gh mmacroug %u -g irough mmapv %X -g 8 thrhexvaluereq(0iced th) requestvaluer <stdarg.h>eovaluereh mm hexmap[]ervis nev'0', '1', '2', '3', '4', '5', '6', '7',osed '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' 
};
est seva_anever r(h mm *by
mapvly
h mme*fmtgh m_iced pvaris nevethrougn,used bb irough mused bb irough msed osed oull mmap i mme*t,d[10],p is_l,p is_llough mmap(!fmtuereqbuere(lp<i1      ovaough mm is_l ismis_llrou0h  t serviap le(lpp i*fmt)rrough mmapap(*fmtereu'%')r reused bed bap(!(--l) sereak;
ed bed bagh m: reused bed bfmtver reusbbbbbbb ereusbbbbbbb t servi*fmt)r reused bed bhroug'l':gh mmot sers neerviced               used mis_lused by any










 is_l is1;sed by any






t serused mis_llused by any






{sed by any










 is_l is0;sed by any










 is_ll is1;sed by any






}sed by any






equesagh m;
 reused bed bhroug's':gh mat any
iced               th mma_arg(pvar,h mm *);sed by any






ap le(lpp i*t)r*b++eny*tve, l--
      ovaluereqused by any






ereak;
ed bed baluereqused by any


hroug'c':gh maviceegh mmacrougiced               *b++enyma_arg(pvar,h mm);sed by any






l--
      ovaluereqused by any






ereak;

			hroug'S':gh murou32erequeshvrp ... is 				re(lp<i4everlrou0h ereak; }e				uenyma_arg(pvar,through mmap he				equ(i=3;i>=0;i--) 
					b[i]ervhexmap[u & 0x0F]he					ue>>= 4he				}e				b+=4he				l-=4he				ereak;  ovaluereqused 
			hroug'x':
			hroug'p':ed               used mis_lleverh m8uergitd  irough m32- thrhexvmmapgougiced                   us(lp<i8everlrou0h ereak; }ed                   uenyma_arg(pvar,through mmap hed                   equ(i=7;i>=0;i--) 
d                   



e[i]ervhexmap[u & 0x0F]hed                   



ue>>= 4he                



}
					b+=8he					l-=8;sed by any






}sed by any






t serusedmis_lleverh m16uergitd  irough m64- thrhexvmmapgougiced                   use(lp<i16everlrou0h ereak; }ed                   ull isma_arg (pvar,b irough msed osed  hed                   equ throug5h mm>ou0h m--)
{sed by any














e[i]ervhexmap[ull & 0x0f]hed                   



ull >>= 4he                



}
y any














ep+=v16he                



l -=v16he                }
y any










 is_l ismis_llrou0hsed by any






ereak;

y any






hroug'd':gh mavugh mmappgougiced               nenyma_arg(pvar,map hed               us(n < 0)
{sed by any










ueny-nhe                



*b++eny'-'he                



ap(!(--l) sereak;                



e                }st serrough mmapy










uenynhe                }sed by any






equesu2
      ovaluerequs
y any






hroug'u':gh munavugh mmappgougiced               uenyma_arg(pvar,through mmap h            



e              u2:ed               ueny9he                dod ough mmapy










d[i]erv(u % 10requ'0'he                



u /=v10;sed by any










m--he                }
ap le(uequesl>euse;sed by any






ap le(++mmap10r ough mmapy










*b++enyd[i]he                



ap(!(--l) sereak;                



e                }sed by any






ereak;
ed bed baluereqused by any


hroug'U':ed               uenyma_arg(pvar,through mmap hed               ueny9he                d[8]enyd[7]enyd[6]eny' 'he                dod ough mmapy










d[i]erv(u % 10requ'0'he                



u /=v10;sed by any










m--he                }
ap le(uequesl>euse;sed by any






ueny5;sed by any






ap le(++mmap10r ough mmapy










*b++enyd[i]he                



ap(!(--l) sereak;e                }sed by any






ereak;
ed bed baluereqused by any


hroug'X':gh m2uergitd  irough m8 thrhexvmmaviced               us(lp<i2everlrou0h ereak; }ed               nenyma_arg(pvar,map hed               *b++enyhexmap[(n & 0xF0re>> 4]he                *b++enyhexmap[n & 0x0F]hed               l-=2
      ovaluereqused by any






ereak;
ed bed balue    ovaluereque          *b++eny*fmt
      ovaluereqused by any


}ed       }st serrough mmapy


*b++eny*fmt
ough mmapy


l--
      ovalueed       }ed       fmtver      ovalueed   }ed   *brou0hs}
est seanever r(h mm *sted by aleny
h mme*fmtgh...icis nevsm_iced pvarh  t serviva_avice(pvar,fmt);serviva_anever r(stedlenyfmtgpvari;serviva_end(pvari;  t s}
eey
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requestdest.ct serviced through mmap is never reused by                                     ap is never reused by any
     341aluerequest service2 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest requestdest.c#1 $ough mmap is never r9used by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequesthrICES; LOreusF USEuest DATA reusverhroS; s neUSIugh msed bRUPTION) HOWvicuseAUSEDced tONy anr reusEORYusF r reservi, WHEusERest ed TRA   oSTRICT r reservi, s nTORed threquest se NEGLIGENCE s nOusERWISE)ny
IS se INy an byYusU by ah mmUSEby r reused by any
 ,rvice IF ADVISEDcy ah mmby SI servicy anyCH   ovalerequestvaluer <stdest.h>estvaluer <at any.h>estvaluer <requestsyms.h>e
weak_ovaare(_atoiis toi)
e  ov_ toi (gh mtgh mm *aicis	r rexd b, power is1;se	equesgh mat len (are- 1, hrou0h xm>ou0h x--, power *=v10)e		req(!xsed ta[x]ereu'-')e
			i *=v-1;
		ever 			i += power * ta[x]e-u'0')he	p
ny
  ihs}
ey
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requestrcmp.Siiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiap is never reused by any
     2447luerequest servic26 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest strcmp.S#1 $    
/*	
OpenBSD: strcmp.S,v by2ny
  /09/27neve47:49 mickby Exp $	   rvicemmW througby J.T.mapnklalu<jtc@netbsd.org> i esPublt sdoy aner equevicemmrouE: I'questrosed bs neloop e neve
    :map is rviced t througacemmroug neveapvdy
     uest nevsp is rviced  througequeslever shuereq escacheer equestvaluer <i386/asm.h>e
FUNCTION (strcmp)	
	movl	0x04(%esp),%eax
	movl	0x08(%esp),%edx
	jmp	L2			ny
Jumpsed by  neloopeused
	.ovalu	2,0x90
L1:	tval	%eax
	tval	%edx
L2:	movb	(%eax),%cl
	uestb	%cl,%cl			ny
null t mman tor?? thro	jz	L3
	cmpb	%cl,(%edx)		ny
h mmeusetch?? thro	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	jne	L3
	tval	%eax
	tval	%edx
	movb	(%eax),%cl
	uestb	%cl,%clo	jz	L3
	cmpb	%cl,(%edx)o	je	L1
	.ovalu 2, 0x90
L3:	movzbl	(%eax),%eax		ny
 irough mhluerequesthro	movzbl	(%edx),%edx
	subl	%edx,%eax
	p
ney
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requestrcpy.Siiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiap is never reused by any
     206aluerequest servic35 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest strcpy.S#1 $    
/*	
OpenBSD: strcpy.S,v by2ny
  /09/27neve47:y amickby Exp $	   rvicemmW througby J.T.mapnklalu<jtc@netbsd.org> i esPublt sdoy aner equevicemmrouE: I'questrosed bs neloop e neve
    :map is rviced t througacemmroug neveapvdy
     uest nevsp is rviced  througequeslever shuereq escacheer equestvaluer <i386/asm.h>e
FUNCTION (strcpy)
	movl	4(%esp),%ecx		ny
dmtgd by anyhro	movl	8(%esp),%edx		ny
srcgd by anyhro	pushl	%ecx			ny
push
dmtgd by anyhro
	.ovalu 2,0x90
L1:	movb	(%edx),%al		ny
 iroseeloop,valuerequto thuchyhro	movb	%al,(%ecx)o	uestb	%al,%alo	jz	L2o	movb	1(%edx),%alo	movb	%al,1(%ecx)o	uestb	%al,%alo	jz	L2o	movb	2(%edx),%alo	movb	%al,2(%ecx)o	uestb	%al,%alo	jz	L2o	movb	3(%edx),%alo	movb	%al,3(%ecx)o	uestb	%al,%alo	jz	L2o	movb	4(%edx),%alo	movb	%al,4(%ecx)o	uestb	%al,%alo	jz	L2o	movb	5(%edx),%alo	movb	%al,5(%ecx)o	uestb	%al,%alo	jz	L2o	movb	6(%edx),%alo	movb	%al,6(%ecx)o	uestb	%al,%alo	jz	L2o	movb	7(%edx),%alo	movb	%al,7(%ecx)o	d bl	$8,%edx
	d bl	$8,%ecxo	uestb	%al,%alo	jnz	L1
L2:	popl	%eax			ny
pop dmtgd by anyhro	p
ney
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requestrany.ct serviced through mmap is never reused by                                     ap is never reused by any
     6200luerequest servi733 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest requestrany.c#4hrough mmap is never reuBeusn J. Sis neve ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequesthrICES; LOreusF USEuest DATA reusverhroS; s neUSIugh msed bRUPTION) HOWvicuseAUSEDced tONy anr reusEORYusF r reservi, WHEusERest ed TRA   oSTRICT r reservi, s nTORed threquest se NEGLIGENCE s nOusERWISE)ny
IS se INy an byYusU by ah mmUSEby r reused by any
 ,rvice IF ADVISEDcy ah mmby SI servicy anyCH   ovalerequestvaluer <st any.h>e
h mm *stencpy(h mm *dmt, gh mtgh mm *s thr      oservicis nevh mm *denydsu;hroughap le(luer p i*s t)oserv--, *dmt++eny*s tver      ovalueed   ifeluer) *dmterviced t bp
ny
  dr      ovs}
emapvrvencmp(gh mtgh mm *s1, gh mtgh mm *s2hr      onicialueed   ap le(n p i*s1 p i*s2r ough mmapap(*s1 !=i*s2rbp
ny
  -1;sed by ans1ver reusbbbbs2ver reusbbbbn--he        ed   }ed   us(nrbp
ny
  -1;sd t bp
ny
  0;  t s}
eh mm *stechr (gh mtgh mm *csd by acicis	gh mtgh mm *s;se	s is s;
	ap le (*ue
		 e			euse*r reuue
				p
ny
  s nev *) uhe			sver 		}e	      oNULLue}
est se*memset(st se*sd by achr      onicid t b irough mh mm *uh m( irough mh mm *)vr;ed   ap le(n)
{sed by an*uh mche        uver reusbbbbn--he    }d t bp
ny
  sue}
est se*memhrou(st se*demt, gh mtgst se*srchr      onicid t b irough mh mm *dsed bb irough mh mm *sough mmap(demt <
srcr ough mmapdh m( irough mh mm *)demtr reusbbbbsh m( irough mh mm *)srcr reusbbbbap le(n)rough mmapy


*deny*s;
ed bed balue ver reusbbbbbbb sver reusbbbbbbb n--he        }d t b}st serrough mmapdh m(( irough mh mm *)vdemt)+nr reusbbbbsh m(( irough mh mm *)vrrcr+nr reusbbbbap le(n)rough mmapy


d--he            s--he            *deny*s;
ed bed baluen--he        }        e    }d t bp
ny
  demtr }
emapvmemcmp(gh mtgst se*dmt, gh mtgst se*srchr      oservicis nevap le(luered 
	ap(*evh mm *)dmt) !=i*evh mm *)rrcrrbp
ny
  1;
	evh mm *)ydsu)ver 	evh mm *)yrrcr+er reusbbbbserv--;        e    }d t bp
ny
  0;  t s}
est se*memcpy(st se*dmt, gh mtgst se*srchr      oservicis nevst se*renydsu;h    e    ap le(luered 
        *((( irough mh mm *)vdsu)vereny*u(( irough mh mm *)vrrcr++)r reusbbbbserv--;        e    }d t bp
ny
  rhs}
ey
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requestrlver.ct serviced through mmap is never reused by                                    ap is never reused by any
     4i73luerequest serv5113 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiany
      ovaluerequest requestrlver.c#1 $    any
OpenBSD: strlver.c,v bymmap 8/07/01 01:29:45 millert Exp $    rvicemmap is neve(c)serv8 TeuseC. Miller <Teus.Miller@cesttesanuere>cemmiuerequest serviced t icemm mmap is never reused by any
      ovaluerequest serviced through m ap is never reused by any
      ovaluerequest serviced through mmap i  never reus d by any
      ovaluerequest serviced through mmap is never reused b ovaluerequest serviced through mmap ough mmap is never reused by an 
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap ough mmap is never reused by valuereq est serviced through mmap is never reused by any
      ovaluerequest servi ed through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is ne icemmused by any
      ovaluere through mmap is never reusey any
      ovaluerecemm est serviced through mmap is never reused by any
   ovaluerequest servicemmis through mmap is never reused by any
      ovaluer est serviced throucemmusmmap is never reused by any
      ovaluerequ serviced through mmacemm is never reused by any
      ovaluerequest servicethrough mmap is i esPer reused by any
      ovaluerequesthrICES; LOreusF USEu DATA reusverhroS;cemmqueeUSIugh msed bRUPTION) HOWvicuseAUSEDced tONy aneusEORYusF r reservi,cemmWHEusERest ed TRA   oSTRICT r reservi, s nTORehrequest se NEGLIGENCE s cemmqusERWISE)ny
IS se INy an byYusU by ah mmUSEby eused by any
 ,rvice IFcemmiDVISEDcy ah mmby SI servicy anyCH   ovaler equestvaluer <st any.h>e
vicemmAppends
srcgeuset any
ded thrluer luem( ilikevrvencaqy
     byereq esfull luer thrdmt, by a rece left). al never lue-1gh mmacrough mmwillany
 anied  aluwaysoNUL t mman tesm( il any    reuse.cemm mny
 smat len(rrcr; useougvalm>ou   st runver re occurred t i/eluerequstrlver(dmt, srchr   ap h mm *dsu;h	gh mtgh mm *s t;
	ed by aed d {
	y
  hrough mm *denydsu;h	y
  hroughh mtgh mm *sh ma t;
	y
  hroug      on =aed d 	ed by adlen;se	anyFigh mmapend thrdmtough adj mmabytesmleftyhro	ap le (*d !=i'\0' p in !=i0)e		 ver 	dlenenyd apdsu;h	n -=vdlen;se	req(n ==i0)e		p
ny
 (dlene+mat len(r)lue	ap le (*u !=i'\0'ed 
		req(n !=p is e			*d++eny*she			n--he		}e		sver 	}e	*deny'\0';se	p
ny
 (dlene+m(s mmarcrr;	ny
houapvdoesmby atvaluer NUL y
   ey
      ovaluerequest serviiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiirequest requestrlvpesct serviced through mmap is never reused by                                    ap is never reused by any
     4i26luerequest serv5135 through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest servvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvany
      ovaluerequest requestrlvpesc#1 $    any
OpenBSD: strlvpesc,v by4er r9/05/01 18:56:41 millert Exp $    rvicemmap is neve(c)serv8 TeuseC. Miller <Teus.Miller@cesttesanuere>cemmiuerequest serviced t icemm mmap is never reused by any
      ovaluerequest serviced through m ap is never reused by any
      ovaluerequest serviced through mmap i  never reus d by any
      ovaluerequest serviced through mmap is never reused b ovaluerequest serviced through mmap ough mmap is never reused by an 
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap ough mmap is never reused by valuereq est serviced through mmap is never reused by any
      ovaluerequest servi ed through mmap is never reused by any
      ovaluerequest serviced through mmap is never reused by any
      ovaluerequest serviced through mmap is ne icemmused by any
      ovaluere through mmap is never reusey any
      ovaluerecemm est serviced through mmap is never reused by any
   ovaluerequest servicemmis through mmap i