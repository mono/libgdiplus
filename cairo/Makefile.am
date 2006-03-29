DIST_SUBDIRS = pixman src test 
SUBDIRS = pixman src 
#doc

# libpng is required for our test programs
if CAIRO_HAS_PNG_FUNCTIONS
SUBDIRS += test
endif

EXTRA_DIST = \
	COPYING \
	COPYING-LGPL-2.1 \
	COPYING-MPL-1.1 \
	cairo.pc.in
MAINTAINERCLEANFILES = \
	$(srcdir)/INSTALL \
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

#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = cairo.pc

check-valgrind: all
	$(MAKE) -C test check-valgrind

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

# Creating ChangeLog from git log:

MAINTAINERCLEANFILES += ChangeLog

EXTRA_DIST += ChangeLog

ChangeLog: $(srcdir)/ChangeLog

$(srcdir)/ChangeLog:
	@if test -d "$(srcdir)/.git"; then \
	  (cd "$(srcdir)" && \
	  ./missing --run git-log) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi

.PHONY: ChangeLog $(srcdir)/ChangeLog

# Some custom targets to make it easier to release things.
# Use either:
#		make release-check
# or		make release-publish

RELEASE_UPLOAD_HOST =   cairographics.org
RELEASE_UPLOAD_BASE =	/srv/cairo.freedesktop.org/www
RELEASE_UPLOAD_DIR =	$(RELEASE_UPLOAD_BASE)/releases
RELEASE_URL_BASE = 	http://cairographics.org/releases
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
	-rm $@
	cd $(prefix); \
	zip $@ bin/libcairo-$(LT_CURRENT_MINUS_AGE).dll

$(developer_zip_file): install
	-rm $@
	cd $(prefix); \
	zip -r $@ include/cairo lib/libcairo.dll.a lib/cairo.lib lib/pkgconfig/cairo.pc share/gtk-doc/html/cairo

zips: $(runtime_zip_file) $(developer_zip_file)

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
	rm -f $(tar_file) $(sha1_file) $(gpg_file)

# Maybe it's just my system, but somehow group sticky bits keep
# getting set and this causes failures in un-tarring on some systems.
# Until I figure out where the sticky bit is coming from, just clean
# these up before building a release.
release-cleanup-group-sticky:
	find . -type f | xargs chmod g-s

release-check: release-verify-even-micro release-verify-newer release-remove-old release-cleanup-group-sticky distcheck

release-upload: release-check $(tar_file) $(sha1_file) $(gpg_file)
	mkdir -p releases
	scp $(tar_file) $(sha1_file) $(gpg_file) $(RELEASE_UPLOAD_HOST):$(RELEASE_UPLOAD_DIR)
	mv $(tar_file) $(sha1_file) $(gpg_file) releases
	ssh $(RELEASE_UPLOAD_HOST) "rm -f $(RELEASE_UPLOAD_DIR)/LATEST-$(PACKAGE)-[0-9]* && ln -s $(tar_file) $(RELEASE_UPLOAD_DIR)/LATEST-$(PACKAGE)-$(VERSION)"
	git tag -s $(CAIRO_VERSION_MAJOR).$(CAIRO_VERSION_MINOR).$(CAIRO_VERSION_MICRO)

release-publish: release-upload releases/$(sha1_file)
	@echo ""
	@echo "Please send an announcement to $(RELEASE_ANNOUNCE_LIST)"
	@echo "including the following:"
	@echo ""
	@echo "Subject: $(PACKAGE) release $(VERSION) now available"
	@echo ""
	@echo "============================== CUT HERE =============================="
	@echo "A new $(PACKAGE) release $(VERSION) is now available from:"
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
	@echo "============================== CUT HERE =============================="
	@echo "Also, please include the new entries from the NEWS file."
	@echo ""
	@echo "Last but not least, do not forget to bump up the micro"
	@echo "version component to the next (odd) number and commit."

# XXX: Depending on all here is rather overkill. We don't really need
# the library built in order to create the documentation.
docs-publish: all
	cp -a doc/public/html $(MANUAL_DATED)
	tar czf $(MANUAL_TAR_FILE) $(MANUAL_DATED)
	scp $(MANUAL_TAR_FILE) $(RELEASE_UPLOAD_HOST):$(MANUAL_UPLOAD_DIR)
	ssh $(RELEASE_UPLOAD_HOST) "cd $(MANUAL_UPLOAD_DIR) && tar xzf $(MANUAL_TAR_FILE) && rm -f manual && ln -s $(MANUAL_DATED) manual && ln -sf $(MANUAL_TAR_FILE) cairo-manual.tar.gz"

.PHONY: release-verify-even-micro release-verify-newer release-remove-old release-cleanup-group-sticky release-check release-upload release-publish docs-publish
