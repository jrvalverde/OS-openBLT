# $Id$

all: vfs_test bltsh ls cat tell validate

validate::
	@echo "--- bin/validate --------"
	@cd validate ; $(MAKE)

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

tell::
	@echo "--- bin/tell --------"
	@cd tell ; $(MAKE)

clean:
	@cd vfs_test ; $(MAKE) clean
	@cd validate ; $(MAKE) clean
	@cd bltsh ; $(MAKE) clean
	@cd ls ; $(MAKE) clean
	@cd cat ; $(MAKE) clean
	@cd tell ; $(MAKE) clean

