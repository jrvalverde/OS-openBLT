BLTHOME := ../../
include $(BLTHOME)make.conf

SRCS		:= vfs_test.c
BINARY		:= vfs_test.bin
LIBS		:= -lc -lblt -ldl

TARGETS		:= foo.so

foo.so: foo.o
	$(LD) -Bshareable -o foo.so foo.o

include $(BLTHOME)make.actions

