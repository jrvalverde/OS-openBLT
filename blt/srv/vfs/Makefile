# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include -Wall
LIBS = -L../../lib/obj -lblt -lc
OBJS = vfs.o vnode.o shm.o rootfs.o bootfs.o

all: vfs.bin

vfs.o: vfs.c vfs-int.h

vfs.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o vfs.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC vfs.bin > vfs.map
	$(ST) vfs.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
