# $Id$

include ../../../../make.conf

CFLAGS = $(CF) -I../../../../include -D_NETWORK

all: eth.so

OBJS = eth.o

eth.so: $(OBJS)
	$(LD) -Bshareable -o eth.so $(OBJS)

clean:
	rm -f *.a *.so *.o *~ os.* core *.core *.s *.map *.bin

