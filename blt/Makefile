BLTHOME := .
include make.conf

ifeq ($(BUILDLOG),)
BUILDLOG := `pwd`/build.log
endif

SUBDIRS		:= lib kernel srv boot bin util

image: subdirs
	./util/bootmaker boot/openblt.ini boot/openblt.boot

floppy:
	./util/bootmaker boot/openblt.ini boot/openblt.boot -floppy

bfloppy: floppy
	dd if=boot/openblt.boot of=/dev/disk/floppy/raw bs=18k
	
lfloppy: floppy
	dd if=boot/openblt.boot of=/dev/fd0 bs=18k

run: image
	./util/netboot boot/openblt.boot $(IP)

go:
	./util/bootmaker boot/openblt.ini boot/openblt.boot
	./util/netboot boot/openblt.boot $(IP)

all::
	@echo "### `date`" > $(BUILDLOG)

clean::
	@rm -f boot/openblt.boot

include make.actions
