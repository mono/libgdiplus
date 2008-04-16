# We're using _GNU_SOURCE to get the prototype for asprintf. This may
# not be the most portable approach, but it is pragmatic and I'm
# willing to do something cleaner as soon as it causes someone a
# problem.
INCLUDES =					\
	-D_GNU_SOURCE				\
	-I$(srcdir)				\
	-I$(top_srcdir)/boilerplate		\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src			\
	$(CAIRO_CFLAGS)

EXTRA_PROGRAMS = cairo-perf cairo-perf-diff-files
EXTRA_DIST = cairo-perf-diff

cairo_perf_SOURCES =		\
	cairo-perf.c		\
	cairo-perf.h		\
	cairo-perf-cover.c	\
	cairo-stats.c		\
	cairo-stats.h		\
	box-outline.c		\
	fill.c			\
	long-lines.c		\
	mosaic.c		\
	mosaic.h		\
	paint.c			\
	paint-with-alpha.c	\
	pattern_create_radial.c \
	rectangles.c		\
	stroke.c		\
	subimage_copy.c		\
	tessellate.c		\
	unaligned-clip.c	\
	text.c			\
	world-map.c		\
	world-map.h		\
	zrusin.c		\
	zrusin-another.h	\
	long-dashed-lines.c

if CAIRO_HAS_WIN32_SURFACE
cairo_perf_SOURCES += cairo-perf-win32.c
else
if CAIRO_HAS_OS2_SURFACE
cairo_perf_SOURCES += cairo-perf-os2.c
else
cairo_perf_SOURCES += cairo-perf-posix.c
endif
endif

cairo_perf_diff_files_SOURCES =	\
	cairo-perf-diff-files.c \
	cairo-stats.c		\
	cairo-stats.h

LDADD = $(top_builddir)/boilerplate/libcairoboilerplate.la \
	$(top_builddir)/src/libcairo.la \
	$(CAIROPERF_LIBS)

$(top_builddir)/boilerplate/libcairoboilerplate.la: $(top_builddir)/src/libcairo.la
	cd $(top_builddir)/boilerplate && $(MAKE) $(AM_MAKEFLAGS) libcairoboilerplate.la

$(top_builddir)/src/libcairo.la:
	cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libcairo.la


# Do a funny transition of CAIRO_TEST_TARGET through TARGETS such that
# one can limit tested targets both through CAIRO_TEST_TARGET env var
# and TARGETS make var on the command line.  Same for the rest.
TARGETS = $(CAIRO_TEST_TARGET)
TARGETS_EXCLUDE = $(CAIRO_TEST_TARGET_EXCLUDE)
ITERS = $(CAIRO_PERF_ITERATIONS)

CAIRO_PERF_ENVIRONMENT = CAIRO_PERF_ITERATIONS="$(ITERS)" CAIRO_TEST_TARGET="$(TARGETS)" CAIRO_TEST_TARGET_EXCLUDE="$(TARGETS_EXCLUDE)"

# Cap the maximum number of iterations during 'make check'
TESTS_ENVIRONMENT = CAIRO_TEST_TARGET="$(TARGETS)" CAIRO_PERF_ITERATIONS="10"

TESTS = cairo-perf$(EXEEXT)

perf: cairo-perf$(EXEEXT)
	$(CAIRO_PERF_ENVIRONMENT) ./cairo-perf$(EXEEXT)

html: index.html

index.html: cairo-perf$(EXEEXT)
	$(CAIRO_PERF_ENVIRONMENT) ./cairo-perf-diff -h HEAD > $@


EXTRA_VALGRIND_FLAGS = $(CAIRO_EXTRA_VALGRIND_FLAGS)
VALGRIND_MEMCHECK_FLAGS = \
	--tool=memcheck \
	--suppressions=$(top_srcdir)/test/.valgrind-suppressions \
	--leak-check=yes --show-reachable=yes
VALGRIND_CALLGRIND_FLAGS = \
	--tool=callgrind
CLEANFILES = \
	$(EXTRA_PROGRAMS) \
	valgrind-log \
	callgrind.out.* \
	index.html

check-valgrind perf-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check \
	    TESTS_ENVIRONMENT='$(TESTS_ENVIRONMENT) \
	    $(top_builddir)/libtool --mode=execute \
	    valgrind $(VALGRIND_MEMCHECK_FLAGS) $(EXTRA_VALGRIND_FLAGS)' \
	    | tee valgrind-log

perf-callgrind:
	$(MAKE) $(AM_MAKEFLAGS) check \
	    TESTS_ENVIRONMENT='$(TESTS_ENVIRONMENT) \
	    $(top_builddir)/libtool --mode=execute \
	    valgrind $(VALGRIND_CALLGRIND_FLAGS) $(EXTRA_VALGRIND_FLAGS)'

.PHONY: perf html check-valgrind perf-valgrind perf-callgrind

EXTRA_DIST += Makefile.win32
