# $Id$

include ../../../../make.conf

CFLAGS = $(CF) -I../../../../include -D_NETWORK

all: ipv4.so

OBJS = iface.o

ipv4.so: $(OBJS)
	$(LD) -Bshareable -o ipv4.so $(OBJS)

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin

