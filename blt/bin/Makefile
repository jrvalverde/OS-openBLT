# $Id$

all: vfs_test bltsh ls cat

vfs_test::
	@echo "--- bin/vfs_test --------"
	@cd vfs_test ; $(MAKE)

bltsh::
	@echo "--- bin/bltsh --------"
	@cd bltsh ; $(MAKE)

ls::
	@echo "--- bin/ls --------"
	@cd ls ; $(MAKE)

cat::
	@echo "--- bin/cat --------"
	@cd cat ; $(MAKE)

clean:
	@cd vfs_test ; $(MAKE) clean
	@cd bltsh ; $(MAKE) clean
	@cd ls ; $(MAKE) clean
	@cd cat ; $(MAKE) clean

