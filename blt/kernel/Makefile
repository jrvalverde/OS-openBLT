# $Id$

include ../make.conf

CFLAGS = $(KCF) -I../include -DASSERT $(SERIALDEBUG)
LIBS = -L../lib/obj -lkern -lconsole
DEP_FLAGS = -M -MG $(CFLAGS)
 
all: kernel.bin

OBJS = stub.o kernel.o i386.o jump.o ktrace.o \
	memory.o resource.o port.o sem.o aspace.o task.o rights.o \
	fault.o syscall.o debug.o assert.o 

include $(OBJS:.o=.d)

ifeq ($(SMP),-D__SMP__)
OBJS += smp.o cpuid.o trampoline.o
endif

kernel.bin: $(OBJS)
	$(LD) -dN -Ttext $(ENTRY_KERNEL) -o kernel.bin $(OBJS)	$(LIBS)
	$(NM) -nC kernel.bin > kernel.map
	$(ST) kernel.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

%.o: %.S
	$(CC) $(KCF) -I../include -D__ASM__ -c $<

%.d: %.c
	$(CC) $(DEP_FLAGS) $< > $@
%.d: %.S
	$(CC) $(DEP_FLAGS) $< > $@
