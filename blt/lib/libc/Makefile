# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include

all: libc.a libc.so libkern.a

MINIMAL = string.o snprintf.o bcmp.o bzero.o strcmp.o strcpy.o strlen.o misc.o strlcat.o strlcpy.o
EXTRA = malloc.o memory.o printf.o vfs.o errno.o exec.o fdl.o stdio.o qsort.o stdlib.o

libc.a: $(MINIMAL) $(EXTRA)
	rm -f libc.a
	$(AR) r libc.a $(MINIMAL) $(EXTRA)

libc.so: $(MINIMAL) $(EXTRA)
	$(LD) -Bshareable -o libc.so $(MINIMAL) $(EXTRA)

libkern.a: $(MINIMAL)
	rm -f libkern.a
	$(AR) r libkern.a $(MINIMAL)

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin
