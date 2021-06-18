ACLOCAL_AMFLAGS = -I m4

if BUILD_UNIT_TESTS
SUBDIRS_TESTS = tests
endif

SUBDIRS = src $(SUBDIRS_TESTS)
DIST_SUBDIRS = src $(SUBDIRS_TESTS)

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA= libgdiplus.pc

DISTCLEANFILES= libgdiplus.pc

EXTRA_DIST = libgdiplus.pc.in README.md LICENSE libgdiplus.sln winconfig.h.in update_submodules.sh external/googletest

all: update_submodules

update_submodules:
	@cd $(top_srcdir) && ./update_submodules.sh

.PHONY: update_submodules
