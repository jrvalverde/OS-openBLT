# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt
OBJS = init.o

all: init.bin

init.o: init.c

init.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o init.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC init.bin > init.map
	$(ST) init.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

