BLTHOME := ../
include $(BLTHOME)make.conf

TARGETS		:= obj crt0.o crtb.o c++rt0.o version.o subdirs

SUBDIRS		:= libc libblt libc++ libconsole libdl libwin

obj:
	mkdir obj
	ln -fs ../libblt/libblt.a obj
	ln -fs ../libblt/libblt.so obj
	ln -fs ../libc/libc.a obj
	ln -fs ../libc/libc.so obj
	ln -fs ../libc/libkern.a obj
	ln -fs ../libc++/libc++.a obj
	ln -fs ../libc++/libc++.so obj
	ln -fs ../libconsole/libconsole.a obj
	ln -fs ../libconsole/libconsole.so obj
	ln -fs ../libdl/libdl.a obj
	ln -fs ../libdl/libdl.so obj
	ln -fs ../libwin/libwin.a obj
	ln -fs ../libwin/libwin.so obj

clean::
	@rm -rf obj

include $(BLTHOME)make.actions
