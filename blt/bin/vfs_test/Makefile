# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt -ldl
OBJS = vfs_test.o

all: vfs_test.bin foo.so

vfs_test.o: vfs_test.c

vfs_test.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a ../../lib/obj/libdl.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o vfs_test.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC vfs_test.bin > vfs_test.map
	$(ST) vfs_test.bin

foo.so: foo.o
	ld -Bshareable -o foo.so foo.o

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

