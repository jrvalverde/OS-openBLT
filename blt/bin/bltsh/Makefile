# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt
OBJS = bltsh.o

all: bltsh.bin

bltsh.o: bltsh.c

bltsh.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o bltsh.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC bltsh.bin > bltsh.map
	$(ST) bltsh.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

