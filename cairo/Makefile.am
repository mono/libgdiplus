DIST_SUBDIRS = src #boilerplate test perf doc util
SUBDIRS = src #doc
# libpng is required for our test programs
if CAIRO_HAS_PNG_FUNCTIONS
#SUBDIRS += boilerplate test perf
endif


.PHONY: doc test retest recheck check-valgrind
# We have some generated header files, so we really need to build the
# library before we can build the docs
doc: all
	cd doc && $(MAKE) $(AM_MAKEFLAGS) doc
test: all
	cd test && $(MAKE) $(AM_MAKEFLAGS) test
retest: all
	cd test && $(MAKE) $(AM_MAKEFLAGS) retest
recheck: all
	cd test && $(MAKE) $(AM_MAKEFLAGS) recheck
check-valgrind: all
	cd test && $(MAKE) $(AM_MAKEFLAGS) check-valgrind
	cd perf && $(MAKE) $(AM_MAKEFLAGS) check-valgrind
perf: all
	cd perf && $(MAKE) $(AM_MAKEFLAGS) perf

if CAIRO_HAS_LCOV
# use recursive makes in order to ignore errors during check/perf
lcov:
	-$(MAKE) $(AM_MAKEFLAGS) check
	$(MAKE) $(AM_MAKEFLAGS) genlcov
lcov-perf:
	-$(MAKE) $(AM_MAKEFLAGS) perf
	$(MAKE) $(AM_MAKEFLAGS) genlcov

# we have to massage the lcov.info file slightly to hide the effect of libtool
# placing the objects files in the .libs/ directory separate from the *.c
genlcov:
	$(LTP) --directory $(top_builddir) --path $(top_builddir) --capture --output-file cairo-lcov.info --test-name CAIRO_TEST --no-checksum
	$(SED) -e 's#.libs/##' \
	       -e 's#boilerplate/src#src#' \
	       -e 's#$(shell pwd)#$(shell cd $(top_srcdir) && pwd)#' \
	       < cairo-lcov.info > cairo-lcov.info.tmp
	LANG=C $(LTP_GENHTML) --prefix $(top_builddir) --output-directory cairo-lcov --title "Cairo Code Coverage" --show-details cairo-lcov.info.tmp
	$(RM) cairo-lcov.info.tmp
else
lcov lcov-perf genlcov:
	@echo You need to configure Cairo with support for gcov enabled.
	@echo e.g, ./configure --enable-gcov
endif

lcov-clean:
if CAIRO_HAS_LCOV
	-$(LTP) --directory $(top_builddir) -z
endif
	-$(RM) -r cairo-lcov.info cairo-lcov
	-$(FIND) -name '*.gcda' -print | $(XARGS) $(RM)

EXTRA_DIST = \
	AUTHORS \
	BIBLIOGRAPHY \
	BUGS \
	CODING_STYLE \
	COPYING \
	COPYING-LGPL-2.1 \
	COPYING-MPL-1.1 \
	INSTALL \
	NEWS \
	PORTING_GUIDE \
	README \
	RELEASING
DISTCLEANFILES = config.cache
distclean-local: lcov-clean

MAINTAINERCLEANFILES = \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/autoscan.log \
	$(srcdir)/compile \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in \
	$(srcdir)/config.sub \
	$(srcdir)/configure.scan \
	$(srcdir)/depcomp \
	$(srcdir)/install-sh \
	$(srcdir)/ltmain.sh \
	$(srcdir)/missing \
	$(srcdir)/mkinstalldirs \
	`find "$(srcdir)" -type f -name Makefile.in -print`

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-test-surfaces

include $(srcdir)/ChangeLog.mk

# Some custom targets to make it easier to release things.
#
# To make real stable releases or devel snapshots, use either:
#		make release-check
# or		make release-publish
#
# To make a quick properly named (date and git hash stamped) tarball:
#		make snapshot

snapshot:
	distdir="$(distdir)-`date '+%Y%m%d'`"; \
	test -d "$(srcdir)/.git" && distdir=$$distdir-`cd "$(srcdir)" && git-rev-parse HEAD | cut -c 1-6`; \
	$(MAKE) $(AM_MAKEFLAGS) distdir="$$distdir" dist

RELEASE_OR_SNAPSHOT = $$(if test "x$(CAIRO_VERSION_MINOR)" = "x$$(echo "$(CAIRO_VERSION_MINOR)/2*2" | bc)" ; then echo release; else echo snapshot; fi)
RELEASE_UPLOAD_HOST =   cairographics.org
RELEASE_UPLOAD_BASE =	/srv/cairo.freedesktop.org/www
RELEASE_UPLOAD_DIR =	$(RELEASE_UPLOAD_BASE)/$(RELEASE_OR_SNAPSHOT)s
RELEASE_URL_BASE = 	http://cairographics.org/$(RELEASE_OR_SNAPSHOT)s
RELEASE_ANNOUNCE_LIST = cairo-announce@cairographics.org (and CC gnome-announce-list@gnome.org)

MANUAL_DATED =		cairo-manual-`date +%Y%m%d`
MANUAL_TAR_FILE = 	$(MANUAL_DATED).tar.gz
MANUAL_UPLOAD_DIR =	$(RELEASE_UPLOAD_BASE)

tar_file = $(PACKAGE)-$(VERSION).tar.gz
sha1_file = $(tar_file).sha1
gpg_file = $(sha1_file).asc

$(sha1_file): $(tar_file)
	sha1sum $^ > $@

$(gpg_file): $(sha1_file)
	@echo "Please enter your GPG password to sign the checksum."
	gpg --armor --sign $^ 

# Win32 package zipfiles
runtime_zip_file = /tmp/$(PACKAGE)-$(VERSION).zip
developer_zip_file = /tmp/$(PACKAGE)-dev-$(VERSION).zip

$(runtime_zip_file): install
	-$(RM) $@
	cd $(prefix); \
	zip $@ bin/libcairo-$(LT_CURRENT_MINUS_AGE).dll

$(developer_zip_file): install
	-$(RM) $@
	cd $(prefix); \
	zip -r $@ include/cairo lib/libcairo.dll.a lib/cairo.lib lib/pkgconfig/cairo.pc lib/pkgconfig/cairo-*.pc share/gtk-doc/html/cairo

zips: $(runtime_zip_file) $(developer_zip_file)

release-verify-sane-changelogs: changelogs
	@echo -n "Checking that the ChangeLog files are sane..."
	@if grep -q "is required to generate" $(CHANGELOGS); then \
		(echo "Ouch." && echo "Some of the ChangeLogs are not generated correctly." \
		&& echo "Remove ChangeLog* and make changelogs" \
		&& false); else :; fi
	@echo "Good."

release-verify-sane-tests:
	@echo "Checking that the test suite is sane..."
	@cd test && $(MAKE) $(AM_MAKEFLAGS) release-verify-sane-tests

release-verify-soname-major:
	@echo -n "Checking that the cairo soname major number is 2..."
	@test "$(LT_CURRENT_MINUS_AGE)" = "2" \
		|| (echo "Ouch." && echo "The soname major number is $(LT_CURRENT_MINUS_AGE) instead of 2." \
		&& echo "The libtool shared library version numbers in configure.in must be incremented properly." \
		&& false)
	@echo "Good."

release-verify-even-micro:
	@echo -n "Checking that $(VERSION) has an even micro component..."
	@test "$(CAIRO_VERSION_MICRO)" = "`echo $(CAIRO_VERSION_MICRO)/2*2 | bc`" \
		|| (echo "Ouch." && echo "The version micro component '$(CAIRO_VERSION_MICRO)' is not an even number." \
		&& echo "The version in configure.in must be incremented before a new release." \
		&& false)
	@echo "Good."

release-verify-newer:
	@echo -n "Checking that no $(VERSION) release already exists..."
	@ssh $(RELEASE_UPLOAD_HOST) test ! -e $(RELEASE_UPLOAD_DIR)/$(tar_file) \
		|| (echo "Ouch." && echo "Found: $(RELEASE_UPLOAD_HOST):$(RELEASE_UPLOAD_DIR)/$(tar_file)" \
		&& echo "Are you sure you have an updated checkout?" \
		&& echo "This should never happen." \
		&& false)
	@echo "Good."

release-remove-old:
	$(RM) $(tar_file) $(sha1_file) $(gpg_file)

# Maybe it's just my system, but somehow group sticky bits keep
# getting set and this causes failures in un-tarring on some systems.
# Until I figure out where the sticky bit is coming from, just clean
# these up before building a release.
release-cleanup-group-sticky:
	find . -type f | xargs chmod g-s

release-check: release-verify-sane-changelogs release-verify-sane-tests release-verify-soname-major release-verify-even-micro release-verify-newer release-remove-old release-cleanup-group-sticky distcheck

release-upload: release-check $(tar_file) $(sha1_file) $(gpg_file)
	mkdir -p releases
	scp $(tar_file) $(sha1_file) $(gpg_file) $(RELEASE_UPLOAD_HOST):$(RELEASE_UPLOAD_DIR)
	mv $(tar_file) $(sha1_file) $(gpg_file) releases
	ssh $(RELEASE_UPLOAD_HOST) "rm -f $(RELEASE_UPLOAD_DIR)/LATEST-$(PACKAGE)-[0-9]* && ln -s $(tar_file) $(RELEASE_UPLOAD_DIR)/LATEST-$(PACKAGE)-$(VERSION)"
	git-tag -s  -m "cairo $(CAIRO_VERSION_MAJOR).$(CAIRO_VERSION_MINOR).$(CAIRO_VERSION_MICRO) release" $(CAIRO_VERSION_MAJOR).$(CAIRO_VERSION_MINOR).$(CAIRO_VERSION_MICRO)

release-publish-message: releases/$(sha1_file)
	@echo "Please follow the instructions in RELEASING to push stuff out and"
	@echo "send out the announcement mails.  Here is the excerpt you need:"
	@echo ""
	@echo "Subject: $(PACKAGE) $(RELEASE_OR_SNAPSHOT) $(VERSION) now available"
	@echo ""
	@echo "============================== CUT HERE =============================="
	@echo "A new $(PACKAGE) $(RELEASE_OR_SNAPSHOT) $(VERSION) is now available from:"
	@echo ""
	@echo "	$(RELEASE_URL_BASE)/$(tar_file)"
	@echo ""
	@echo "    which can be verified with:"
	@echo ""
	@echo "	$(RELEASE_URL_BASE)/$(sha1_file)"
	@echo -n "	"
	@cat releases/$(sha1_file)
	@echo ""
	@echo "	$(RELEASE_URL_BASE)/$(gpg_file)"
	@echo "	(signed by `getent passwd "$$USER" | cut -d: -f 5 | cut -d, -f 1`)"
	@echo ""
	@echo "  Additionally, a git clone of the source tree:"
	@echo ""
	@echo "	git clone git://git.cairographics.org/git/cairo"
	@echo ""
	@echo "    will include a signed $(VERSION) tag which points to a commit named:"
	@echo "	`git cat-file tag $(VERSION) | grep ^object | sed -e 's,object ,,'`"
	@echo ""
	@echo "    which can be verified with:"
	@echo "	git verify-tag $(VERSION)"
	@echo ""
	@echo "    and can be checked out with a command such as:"
	@echo "	git checkout -b build $(VERSION)"
	@echo ""
	@echo "============================== CUT HERE =============================="

release-publish: release-upload release-publish-message

doc-publish: doc
	rm -rf ./$(MANUAL_DATED)
	cp -a doc/public/html $(MANUAL_DATED)
	tar czf $(MANUAL_TAR_FILE) $(MANUAL_DATED)
	scp $(MANUAL_TAR_FILE) $(RELEASE_UPLOAD_HOST):$(MANUAL_UPLOAD_DIR)
	ssh $(RELEASE_UPLOAD_HOST) "cd $(MANUAL_UPLOAD_DIR) && tar xzf $(MANUAL_TAR_FILE) && rm -f manual && ln -s $(MANUAL_DATED) manual && ln -sf $(MANUAL_TAR_FILE) cairo-manual.tar.gz"

.PHONY: release-verify-even-micro release-verify-newer release-remove-old release-cleanup-group-sticky release-check release-upload release-publish docs-publish lcov lcov-perf lcov-clean genlcov

EXTRA_DIST += Makefile.win32
