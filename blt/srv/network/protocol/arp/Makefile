# $Id$

include ../../../../make.conf

CFLAGS = $(CF) -I../../../../include -D_NETWORK

all: arp.so

OBJS = arp.o

arp.so: $(OBJS)
	$(LD) -Bshareable -o arp.so $(OBJS)

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin

