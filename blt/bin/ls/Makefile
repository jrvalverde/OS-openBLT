# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt
OBJS = ls.o

all: ls.bin

ls.o: ls.c

ls.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o ls.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC ls.bin > ls.map
	$(ST) ls.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

