# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt
OBJS = tell.o

all: tell.bin

tell.o: tell.c

tell.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o tell.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC tell.bin > tell.map
	$(ST) tell.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

