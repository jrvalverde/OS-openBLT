# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include

all: libdl.a libdl.so

OBJS = load.o sym.o elf.o

libdl.a: $(OBJS)
	rm -f libdl.a
	$(AR) r libdl.a $(OBJS)

libdl.so: $(OBJS)
	$(LD) -Bshareable -o libdl.so $(OBJS)

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin
