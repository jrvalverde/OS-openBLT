# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include

all: libconsole.a libconsole.so

KERN = string.o snprintf.o bcmp.o bzero.o strcmp.o strcpy.o strlen.o
USER = syscall.o namer.o 
MEM = malloc.o memory.o

libconsole.a: conio.o
	rm -rf libconsole.a
	$(AR) r libconsole.a conio.o

libconsole.so: conio.o
	$(LD) -Bshareable -o libconsole.so conio.o

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin
