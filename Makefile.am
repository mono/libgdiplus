ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tests
DIST_SUBDIRS = src tests

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA= libgdiplus.pc

DISTCLEANFILES= libgdiplus.pc

EXTRA_DIST = libgdiplus.pc.in README.md LICENSE libgdiplus.sln winconfig.h.in update_submodules.sh external/googletest

all: update_submodules

update_submodules:
	@cd $(top_srcdir) && ./update_submodules.sh

.PHONY: update_submodules
