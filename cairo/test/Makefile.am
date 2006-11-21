# Here are all the tests that are run unconditionally
TESTS = 			\
a8-mask				\
caps-joins			\
caps-joins-alpha		\
caps-sub-paths			\
clip-all			\
clip-fill-rule			\
clip-fill-rule-pixel-aligned	\
clip-nesting			\
clip-operator			\
clip-twice			\
close-path			\
composite-integer-translate-source \
composite-integer-translate-over \
composite-integer-translate-over-repeat \
copy-path			\
create-for-stream		\
create-from-png			\
create-from-png-stream		\
dash-caps-joins			\
dash-no-dash			\
dash-offset-negative		\
dash-scale			\
dash-zero-length		\
degenerate-path			\
device-offset			\
device-offset-positive		\
extend-reflect			\
fill-and-stroke			\
fill-and-stroke-alpha		\
fill-and-stroke-alpha-add	\
fill-rule			\
filter-nearest-offset		\
font-face-get-type		\
font-matrix-translation		\
glyph-cache-pressure		\
get-and-set			\
get-group-target		\
gradient-alpha			\
infinite-join			\
leaky-dash			\
leaky-polygon			\
line-width			\
line-width-scale		\
linear-gradient			\
long-lines			\
mask				\
mask-ctm			\
mask-surface-ctm		\
move-to-show-surface		\
new-sub-path			\
nil-surface			\
operator-clear			\
operator-source			\
paint				\
paint-source-alpha		\
paint-with-alpha		\
pattern-get-type		\
pixman-rotate			\
rectangle-rounding-error	\
scale-source-surface-paint	\
select-font-face		\
select-font-no-show-text	\
self-copy			\
self-intersecting		\
set-source			\
show-text-current-point		\
source-clip			\
source-clip-scale		\
source-surface-scale-paint	\
surface-finish-twice		\
surface-pattern			\
text-antialias-gray		\
text-antialias-none		\
text-antialias-subpixel		\
text-cache-crash		\
text-pattern			\
text-rotate			\
transforms			\
translate-show-surface		\
trap-clip			\
truetype-tables			\
unantialiased-shapes		\
unbounded-operator		\
user-data			\
rel-path                        \
push-group			\
zero-alpha

# Then we have a collection of tests that are only run if certain
# features are compiled into cairo
if HAVE_PTHREAD
TESTS += pthread-show-text
endif

if CAIRO_HAS_FT_FONT
TESTS += bitmap-font
TESTS += ft-font-create-for-ft-face
TESTS += ft-text-vertical-layout-type1
TESTS += ft-text-vertical-layout-type3
TESTS += ft-text-antialias-none
endif

if CAIRO_HAS_SVG_SURFACE
TESTS += svg-surface svg-clip
endif

if CAIRO_HAS_PDF_SURFACE
TESTS += pdf-features
endif

if CAIRO_HAS_PS_SURFACE
TESTS += ps-features
endif

if CAIRO_HAS_XLIB_SURFACE
TESTS += xlib-surface
endif

if CAIRO_HAS_MULTI_PAGE_SURFACES
TESTS += multi-page
endif

if CAIRO_HAS_SVG_SURFACE
if CAIRO_HAS_PDF_SURFACE
if CAIRO_HAS_PS_SURFACE
TESTS += fallback-resolution
endif
endif
endif

# XXX: Here are some existing tests that are currently disabled for
# one reason or another.
# 
# show-glyphs-many - this stress test was exercising a particular bug
#      in the xlib surface code (exceeding the X11 protocol request
#      limit) when rendering several thousand glyphs at once. The
#      original xlib-surface bug is fixed now, but the test continues
#      to stress some other aspects of the test suite. For example,
#      when doing text as paths, the resuilting PostScript file is one
#      giant path that ghostscript has a particularly hard time
#      with. I'm disabling this test for now, since I don't care about
#      that performance problem in ghostscript. (But, there is a
#      similar performance problem when using cairo to rasterize the
#      equivalen giant path---from an SBG files say---so this might be
#      a useful kind of test to bring back again for performance (not
#      correctness) testing.

# All tests which have a reference image go here.
# I really don't like having to repeat this list. Anyone know a good
# way to avoid it? Can I use a wildcard here?
EXTRA_DIST =						\
6x13.pcf						\
make-html.pl						\
a8-mask-ref.png						\
bitmap-font-ref.png					\
bitmap-font-rgb24-ref.png				\
bitmap-font-pdf-argb32-ref.png				\
caps-joins-ref.png					\
caps-joins-ps-argb32-ref.png				\
caps-joins-alpha-ref.png				\
caps-joins-alpha-svg-ref.png				\
caps-sub-paths-ref.png					\
caps-sub-paths-ps-argb32-ref.png			\
clip-all-ref.png					\
clip-fill-rule-ref.png					\
clip-fill-rule-rgb24-ref.png				\
clip-fill-rule-ps-argb32-ref.png			\
clip-fill-rule-pixel-aligned-ref.png			\
clip-fill-rule-pixel-aligned-rgb24-ref.png		\
clip-nesting-ref.png					\
clip-nesting-rgb24-ref.png				\
clip-nesting-ps-argb32-ref.png				\
clip-operator-ref.png					\
clip-operator-rgb24-ref.png				\
clip-twice-ref.png					\
clip-twice-rgb24-ref.png				\
clip-twice-ps-argb32-ref.png				\
close-path-ref.png					\
close-path-ps-argb32-ref.png				\
composite-integer-translate-over-ref.png		\
composite-integer-translate-over-pdf-argb32-ref.png	\
composite-integer-translate-over-svg-ref.png		\
composite-integer-translate-over-repeat-ref.png		\
composite-integer-translate-source-ref.png		\
copy-path-ref.png					\
copy-path-ps-argb32-ref.png				\
create-from-png-ref.png					\
create-from-png-stream-ref.png				\
dash-caps-joins-ref.png					\
dash-caps-joins-ps-argb32-ref.png			\
dash-no-dash-ref.png					\
dash-offset-negative-ref.png				\
dash-offset-negative-ps-argb32-ref.png			\
dash-scale-ref.png					\
dash-scale-ps-argb32-ref.png				\
dash-zero-length-ref.png				\
dash-zero-length-rgb24-ref.png				\
dash-zero-length-ps-argb32-ref.png			\
degenerate-path-ref.png					\
degenerate-path-rgb24-ref.png				\
degenerate-path-ps-argb32-ref.png			\
device-offset-ref.png					\
device-offset-rgb24-ref.png				\
device-offset-positive-ref.png				\
device-offset-positive-rgb24-ref.png			\
fill-and-stroke-ref.png					\
fill-and-stroke-rgb24-ref.png				\
fill-and-stroke-ps-argb32-ref.png			\
fill-and-stroke-alpha-ref.png				\
fill-and-stroke-alpha-svg-ref.png			\
fill-and-stroke-alpha-add-ref.png			\
fill-rule-ref.png					\
fill-rule-rgb24-ref.png					\
fill-rule-ps-argb32-ref.png				\
filter-nearest-offset-ref.png				\
ft-text-antialias-none-ref.png				\
ft-text-antialias-none-ps-argb32-ref.png		\
ft-text-vertical-layout-type1-ref.png			\
ft-text-vertical-layout-type1-svg-ref.png		\
ft-text-vertical-layout-type3-ref.png		\
ft-text-vertical-layout-type3-svg-ref.png		\
font-matrix-translation-ref.png				\
font-matrix-translation-ps-argb32-ref.png		\
font-matrix-translation-svg-ref.png			\
get-group-target-ref.png				\
glyph-cache-pressure-ref.png				\
glyph-cache-pressure-ps-argb32-ref.png			\
glyph-cache-pressure-svg-ref.png			\
gradient-alpha-ref.png					\
gradient-alpha-rgb24-ref.png				\
infinite-join-ref.png					\
infinite-join-ps-argb32-ref.png				\
leaky-dash-ref.png					\
leaky-polygon-ref.png					\
leaky-polygon-ps-argb32-ref.png				\
linear-gradient-ref.png					\
linear-gradient-svg-ref.png				\
line-width-ref.png					\
line-width-ps-argb32-ref.png				\
line-width-scale-ref.png				\
line-width-scale-ps-argb32-ref.png			\
long-lines-ref.png					\
mask-ref.png						\
mask-rgb24-ref.png					\
mask-beos-bitmap-argb32-ref.png				\
mask-beos-bitmap-rgb24-ref.png				\
mask-beos-rgb24-ref.png					\
mask-svg-argb32-ref.png					\
mask-svg-rgb24-ref.png					\
mask-ctm-ref.png					\
mask-ctm-rgb24-ref.png					\
mask-ctm-svg-argb32-ref.png				\
mask-ctm-svg-rgb24-ref.png				\
mask-surface-ctm-ref.png				\
mask-surface-ctm-rgb24-ref.png				\
mask-surface-ctm-svg-argb32-ref.png			\
mask-surface-ctm-svg-rgb24-ref.png			\
move-to-show-surface-ref.png				\
new-sub-path-ref.png					\
new-sub-path-rgb24-ref.png				\
new-sub-path-ps-argb32-ref.png				\
nil-surface-ref.png					\
nil-surface-rgb24-ref.png				\
operator-clear-ref.png					\
operator-clear-rgb24-ref.png				\
operator-source-ref.png					\
operator-source-rgb24-ref.png				\
paint-ref.png						\
paint-source-alpha-ref.png				\
paint-source-alpha-pdf-argb32-ref.png			\
paint-source-alpha-svg-ref.png				\
paint-with-alpha-ref.png				\
paint-with-alpha-svg-ref.png				\
pixman-rotate-ref.png					\
pixman-rotate-rgb24-ref.png				\
push-group-ref.png					\
push-group-rgb24-ref.png				\
push-group-svg-argb32-ref.png				\
push-group-svg-rgb24-ref.png				\
rectangle-rounding-error-ref.png			\
rel-path-ref.png					\
rel-path-rgb24-ref.png					\
romedalen.png						\
scale-source-surface-paint-ref.png			\
scale-source-surface-paint-rgb24-ref.png		\
scale-source-surface-paint-pdf-argb32-ref.png		\
scale-source-surface-paint-svg-argb32-ref.png		\
scale-source-surface-paint-svg-rgb24-ref.png		\
select-font-face-ref.png				\
select-font-face-ps-argb32-ref.png			\
select-font-face-svg-ref.png				\
self-copy-ref.png					\
self-intersecting-ref.png				\
self-intersecting-rgb24-ref.png				\
set-source-ref.png					\
set-source-rgb24-ref.png				\
set-source-beos-bitmap-rgb24-ref.png			\
set-source-beos-rgb24-ref.png				\
set-source-svg-argb32-ref.png				\
set-source-svg-rgb24-ref.png				\
show-glyphs-many-ref.png				\
show-text-current-point-ref.png				\
show-text-current-point-ps-argb32-ref.png		\
show-text-current-point-svg-ref.png			\
source-clip-ref.png					\
source-clip-scale-ref.png				\
source-clip-scale-svg-ref.png				\
source-surface-scale-paint-ref.png			\
source-surface-scale-paint-rgb24-ref.png		\
surface-pattern-ref.png					\
text-antialias-gray-ref.png				\
text-antialias-none-ref.png				\
text-antialias-subpixel-ref.png				\
text-pattern-ref.png					\
text-pattern-rgb24-ref.png				\
text-pattern-svg-argb32-ref.png				\
text-pattern-svg-rgb24-ref.png				\
text-rotate-ref.png					\
text-rotate-rgb24-ref.png				\
transforms-ref.png					\
transforms-ps-argb32-ref.png				\
translate-show-surface-ref.png				\
trap-clip-ref.png					\
trap-clip-rgb24-ref.png					\
trap-clip-beos-bitmap-rgb24-ref.png			\
trap-clip-beos-rgb24-ref.png				\
trap-clip-svg-argb32-ref.png				\
trap-clip-svg-rgb24-ref.png				\
unantialiased-shapes-ref.png				\
unbounded-operator-ref.png				\
unbounded-operator-rgb24-ref.png			\
zero-alpha-ref.png

# Any test for which the code committed to CVS is expected to fail
# should be listed here.
#
# This way, we can avoid being bothered by reports of bugs we are
# aware of, but users can still report when tests start behaving in
# unexpected ways on their system.
#
# Of course, before any "release" of cairo we should eliminate
# everything from this list by fixing the bugs. (We don't necessarily
# have to be that strict for "snapshots" though.)
#
# Also, any test listed here should call cairo_test_expect_failure and
# provide an explanation for the expected failure.
XFAIL_TESTS =			\
a8-mask				\
extend-reflect			\
filter-nearest-offset		\
ft-text-vertical-layout-type1	\
ft-text-vertical-layout-type3	\
leaky-dash			\
long-lines			\
self-intersecting		\
text-rotate

# Any test that doesn't generate a log file goes here
NOLOG_TESTS =			\
create-for-stream		\
user-data			\
svg-surface			\
svg-clip			\
pdf-features			\
ps-features			\
multi-page			\
fallback-resolution

SUPPORT_PROGS =

# We're using _GNU_SOURCE to get the prototype for asprintf. This may
# not be the most portable approach, but it is pragmatic and I'm
# willing to do something cleaner as soon as it causes someone a
# problem.
INCLUDES =					\
	-D_GNU_SOURCE				\
	-I$(srcdir)				\
	-I$(top_srcdir)/pixman/src		\
	-I$(top_builddir)/src			\
	-I$(top_srcdir)/src			\
	$(CAIRO_CFLAGS)

EXTRA_LTLIBRARIES = libcairotest.la

libcairotest_la_SOURCES =\
buffer-diff.c		\
buffer-diff.h		\
cairo-test.c		\
cairo-test.h		\
xmalloc.c		\
xmalloc.h

if CAIRO_HAS_BEOS_SURFACE
libcairotest_la_SOURCES += cairo-test-beos.cpp cairo-test-beos.h
# BeOS system headers trigger this warning
libcairotest_la_CXXFLAGS = -Wno-multichar
endif

if CAIRO_HAS_DIRECTFB_SURFACE
libcairotest_la_SOURCES += cairo-test-directfb.c cairo-test-directfb.h
endif

LDADD = libcairotest.la $(top_builddir)/src/libcairo.la

if CAIRO_CAN_TEST_GLITZ_AGL_SURFACE
LDADD += $(GLITZ_AGL_LIBS)
endif
if CAIRO_CAN_TEST_GLITZ_EGL_SURFACE
LDADD += $(GLITZ_EGL_LIBS)
endif
if CAIRO_CAN_TEST_GLITZ_GLX_SURFACE
LDADD += $(GLITZ_GLX_LIBS)
endif
if CAIRO_CAN_TEST_GLITZ_WGL_SURFACE
LDADD += $(GLITZ_WGL_LIBS)
endif

if HAVE_PTHREAD
LDADD += -lpthread
endif

SUPPORT_PROGS += imagediff png-flatten

if CAIRO_CAN_TEST_PDF_SURFACE
SUPPORT_PROGS += pdf2png
pdf2png_CFLAGS = $(POPPLER_CFLAGS)
pdf2png_LDADD = $(LDADD) $(POPPLER_LIBS)
endif

if CAIRO_CAN_TEST_SVG_SURFACE
SUPPORT_PROGS += svg2png
svg2png_CFLAGS = $(LIBRSVG_CFLAGS)
svg2png_LDADD = $(LDADD) $(LIBRSVG_LIBS)
endif

EXTRA_PROGRAMS = $(TESTS) $(SUPPORT_PROGS)

# Do a funny transition of CAIRO_TEST_TARGET through TARGETS such that
# one can limit tested targets both through CAIRO_TEST_TARGET env var
# and TARGETS make var on the command line.
TARGETS = $(CAIRO_TEST_TARGET)

TESTS_ENVIRONMENT = CAIRO_XFAIL_TESTS="$(XFAIL_TESTS)" CAIRO_TEST_TARGET="$(TARGETS)"

VALGRIND_FLAGS = \
	--tool=memcheck --suppressions=$(srcdir)/.valgrind-suppressions \
	--leak-check=yes --show-reachable=yes

CLEANFILES =					\
	*.ps					\
	*.pdf					\
	*.svg					\
	*-out.png				\
	*-diff.png				\
	*.log					\
	valgrind-log				\
	index.html				\
	ref.hash				\
	$(EXTRA_LTLIBRARIES)			\
	$(EXTRA_PROGRAMS)

# Check tests under valgrind
# Saves log to valgrind-log
check-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS_ENVIRONMENT='$(TESTS_ENVIRONMENT) $(top_srcdir)/libtool --mode=execute valgrind $(VALGRIND_FLAGS)' 2>&1 | tee valgrind-log

# The following definitions both should work.
#FAILED_TESTS = `grep -l '\<FAIL\>' $(TESTS:=.log) 2>/dev/null | sed -e 's/[.]log$$//' | xargs echo`
FAILED_TESTS = `grep -l '\<FAIL\>' $(TESTS:=.log) 2>/dev/null | tr '\n' ' ' | sed -e 's/[.]log  */ /g; s/^ //; s/ $$//'`

recheck = check TESTS="$(FAILED_TESTS)"

# Re-checks all failed tests, i.e. tests with a log file that has a failure
recheck:
	@echo Re-checking failed tests
	@$(MAKE) $(AM_MAKEFLAGS) $(recheck)

# Checks tests and creates index.html.
# Target doesn't fail if tests fail.
test:
	@$(MAKE) $(AM_MAKEFLAGS) check; \
	$(MAKE) $(AM_MAKEFLAGS) html

# Re-checks tests and creates index.html.
# Target doesn't fail if tests fail.
retest:
	@TESTS="$(FAILED_TESTS)"; \
	$(MAKE) $(AM_MAKEFLAGS) TESTS="$$TESTS" check; \
	$(MAKE) $(AM_MAKEFLAGS) TESTS="$$TESTS" html

# Make index.html with no dependency tracking.
html:
	@echo Creating index.html
	@perl $(srcdir)/make-html.pl $(TESTS:=.log) > index.html

# Make index.html with no dependency tracking, containing only the failed tests.
rehtml:
	@$(MAKE) $(AM_MAKEFLAGS) TESTS="$(FAILED_TESTS)" html

$(TESTS): $(SUPPORT_PROGS)

%.log: %
	-./$<

NOLOG_TESTS_LOG = $(NOLOG_TESTS:=.log)

$(NOLOG_TESTS_LOG):
	echo dummy > $@

index.html: $(srcdir)/make-html.pl $(TESTS:=.log)
	@echo Creating index.html
	@perl $^ > $@

# Identify identical reference images
check-ref-dups:
	@sha1sum *-ref.png > ref.hash
	@join ref.hash ref.hash | grep -v -E '( .*-ref.png).*\1' | cut -d' ' -f 1-2 | sort -u

.PHONY: check-valgrind test recheck retest html rehtml check-ref-dups
