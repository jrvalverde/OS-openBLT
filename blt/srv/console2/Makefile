# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lconsole -lblt -lc

all: console.bin

console.bin: console.o vt100.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o console.bin ../../lib/crt0.o console.o vt100.o $(LIBS)
	$(NM) -nC console.bin > console.map
	$(ST) console.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
