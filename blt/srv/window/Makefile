BLTHOME := ../../
include $(BLTHOME)make.conf

CFLAGS		+= -fno-rtti -fno-exceptions -fno-pic -O2
CRT0		:= $(BLTHOME)lib/c++rt0.o
LIBS		:= -lc -lblt -lc++ -lconsole 
BINARY		:= window_server.bin

OBJS := main.o GraphicsContext.o Region.o  \
	Window.o WindowManager.o Renderer_8bpp.o \
	vga.o Renderer_vga.o SerialMouse.o

include $(BLTHOME)make.actions

