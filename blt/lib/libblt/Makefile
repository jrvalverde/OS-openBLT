# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include

all: libblt.a libblt.so

OBJS = namer.o syscalls.o hash.o qsem.o atomic.o

syscalls.o: syscalls.S
	$(CC) $(CFLAGS) -c -o syscalls.o syscalls.S
 
libblt.a: $(OBJS)
	rm -f libblt.a
	$(AR) r libblt.a $(OBJS)

libblt.so: $(OBJS)
	$(LD) -Bshareable -o libblt.so $(OBJS)

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin
