# $Id$

include ../../../../make.conf

CFLAGS = $(CF) -I../../../../include -D_NETWORK

all: ne.so

OBJS = wrap.o support.o ne2000.o

ne.so: $(OBJS)
	$(LD) -Bshareable -o ne.so $(OBJS)
	nm -nC ne.so > ne.map

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin

