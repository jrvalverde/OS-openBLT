BLTHOME := ../../
include $(BLTHOME)make.conf

LIBRARY		:= libblt.a
SHLIB		:= libblt.so
SHLIB_MAJOR	:= 1
SHLIB_MINOR	:= 0
OBJS 		:= namer.o syscalls.o hash.o qsem.o atomic.o tell.o blkdev.o disk.o
LIBOBJS		:= ../libc/strlcat.o ../libc/strlcpy.o

include $(BLTHOME)make.actions
