# $Id$

all: namer console2 ne2000 video vfs init fish

console2::
	@echo "--- srv/console2 --------"
	@cd console2 ; $(MAKE)

namer::
	@echo "--- srv/namer -----------"
	@cd namer ; $(MAKE)

ne2000::
	@echo "--- srv/ne2000 ----------"
	@cd ne2000 ; $(MAKE)

fish::
	@echo "--- srv/fish ------------"
	@cd fish ; $(MAKE)

video::
	@echo "--- srv/video -----------"
	@cd video ; $(MAKE)

vfs::
	@echo "--- srv/vfs -------------"
	@cd vfs ; $(MAKE)

init::
	@echo "--- srv/init -------------"
	@cd init ; $(MAKE)

clean:
	@cd console2 ; $(MAKE) clean
	@cd namer ; $(MAKE) clean
	@cd ne2000 ; $(MAKE) clean
	@cd video ; $(MAKE) clean
	@cd fish ; $(MAKE) clean
	@cd vfs ; $(MAKE) clean
	@cd init ; $(MAKE) clean
