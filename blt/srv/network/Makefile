# $Id$

include ../../make.conf

CFLAGS = $(CF) -I../../include -D_NETWORK
LIBS = -L../../lib/obj -lc -lblt -ldl
OBJS = network.o mbuf.o

all: network.bin device protocol

network.o: network.c

network.bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a ../../lib/obj/libdl.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o network.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC network.bin > network.map
	$(ST) network.bin

device::
	@cd device ; $(MAKE)

protocol::
	@cd protocol ; $(MAKE)

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
	@cd device ; $(MAKE) clean
	@cd protocol ; $(MAKE) clean

