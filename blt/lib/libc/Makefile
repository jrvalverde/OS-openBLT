BLTHOME := ../../
include $(BLTHOME)make.conf

MINIMAL = string.o snprintf.o bcmp.o bzero.o strcmp.o strcpy.o strlen.o strlcat.o strlcpy.o
EXTRA = malloc.o memory.o printf.o vfs.o errno.o exec.o fdl.o stdio.o qsort.o stdlib.o ctype.o getopt.o

TARGETS		:= libkern.a
LIBRARY		:= libc.a
SHLIB		:= libc.so
SHLIB_MAJOR	:= 1
SHLIB_MINOR	:= 0
OBJS		:= $(MINIMAL) $(EXTRA)

libkern.a: $(MINIMAL)
	@rm -f libkern.a
	$(AR) r libkern.a $(MINIMAL)

include $(BLTHOME)make.actions
