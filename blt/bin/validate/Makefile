# $Id$

include ../../make.conf

TARGET = validate
OBJS = validate.o

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib/obj -lc -lblt

all: $(TARGET).bin

$(TARGET).bin: $(OBJS) ../../lib/crt0.o ../../lib/obj/libc.a ../../lib/obj/libblt.a
	$(LD) -dN -Ttext $(ENTRY_USER) -o $(TARGET).bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC $(TARGET).bin > $(TARGET).map
	$(ST) $(TARGET).bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

