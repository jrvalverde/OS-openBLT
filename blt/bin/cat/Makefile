# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt
OBJS = cat.o

all: cat.bin

cat.o: cat.c

cat.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o cat.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC cat.bin > cat.map
	$(ST) cat.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

