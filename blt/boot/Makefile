BLTHOME := ../

include $(BLTHOME)make.conf

CFLAGS		+= -I../kernel
BINARY		:= boot.bin
OBJS		:= boot.o
LIBS		:= -lconsole
ENTRY		:= $(ENTRY_BOOTSTRAP)
CRT0		:= bootstub.o

include $(BLTHOME)make.actions
