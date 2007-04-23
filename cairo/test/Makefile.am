SUBDIRS=pdiff .

# Here are all the tests that are run unconditionally
TESTS = 			\
a8-mask				\
big-trap			\
caps-joins			\
caps-joins-alpha		\
caps-sub-paths			\
clip-all			\
clip-fill-rule			\
clip-fill-rule-pixel-aligned	\
clip-nesting			\
clip-operator			\
clip-push-group			\
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
dash-state			\
degenerate-path			\
device-offset			\
device-offset-positive		\
extend-pad			\
fill-and-stroke			\
fill-and-stroke-alpha		\
fill-and-stroke-alpha-add	\
fill-degenerate-sort-order	\
fill-missed-stop		\
fill-rule			\
filter-nearest-offset		\
font-face-get-type		\
font-matrix-translation		\
glyph-cache-pressure		\
get-and-set			\
get-clip                        \
get-group-target		\
get-path-extents                \
gradient-alpha			\
infinite-join			\
in-fill-empty-trapezoid		\
invalid-matrix			\
leaky-dash			\
leaky-polygon			\
line-width			\
line-width-scale		\
line-width-zero			\
linear-gradient			\
linear-gradient-reflect		\
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
paint-repeat			\
paint-source-alpha		\
paint-with-alpha		\
pattern-get-type		\
pattern-getters			\
pixman-rotate			\
radial-gradient			\
random-intersections		\
rectangle-rounding-error	\
rectilinear-stroke		\
rotate-image-surface-paint	\
scale-down-source-surface-paint	\
scale-source-surface-paint	\
select-font-face		\
select-font-no-show-text	\
self-copy			\
self-intersecting		\
set-source			\
show-text-current-point		\
skew-extreme			\
solid-pattern-cache-stress	\
source-clip			\
source-clip-scale		\
source-surface-scale-paint	\
surface-finish-twice		\
surface-pattern			\
surface-pattern-scale-down	\
surface-pattern-scale-up	\
text-antialias-gray		\
text-antialias-none		\
text-antialias-subpixel		\
text-cache-crash		\
text-pattern			\
text-rotate			\
text-zero-len			\
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

# XXX: Here are some existing tests that are currently disabled for
# one reason or another. They can still be built and run (manually)
# but we don't currently run them as part of 'make check' to avoid
# inflicting some particularly nasty problems on the user of the test
# suite (described below).
#
# It would definitely be worthwhile to address these problems so that
# these tests can get back into the test suite proper, (since the
# problems do indicated bugs somewhere and we're less likely to get
# those bugs fixed without the attention due to having these tests in
# the suite).
#
# The reasons for disabling the tests are as follows:
#
# extend-reflect - Triggers an infinite (or near-infinite) bug in some
#	X servers in some circumstances. Details and cause unknown.
#
# show-glyphs-many - This stress test was exercising a particular bug
#	in the xlib surface code (exceeding the X11 protocol request
#	limit) when rendering several thousand glyphs at once. The
#	original xlib-surface bug is fixed now, but the test continues
#	to stress some other aspects of the test suite. For example,
#	when doing text as paths, the resuilting PostScript file is one
#	giant path that ghostscript has a particularly hard time
#	with. I'm disabling this test for now, since I don't care about
#	that performance problem in ghostscript. (But, there is a
#	similar performance problem when using cairo to rasterize the
#	equivalen giant path---from an SVG files say---so this might be
#	a useful kind of test to bring back again for performance (not
#	correctness) testing.
# text-glyph-range - This test triggers the following assertion in cairo:
#
#	lt-text-glyph-range: cairo-scaled-font-subsets.c:350:
#	_cairo_sub_font_collect: Assertion `collection->num_glyphs ==
#	collection->max_glyph + 1' failed.
#
#	And as a result causes a failure to be recorded by the test
#	suite, (in spite of the XFAIL status of the test). So maybe
#	that's just a bug in the test rig that should just consider
#	the abort an XFAIL like any other.
DISABLED_TESTS =		\
extend-reflect			\
show-glyphs-many		\
text-glyph-range

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

# All tests which have a reference image go here.
REFERENCE_IMAGES = \
	a8-mask-ref.png	\
	bitmap-font-pdf-argb32-ref.png	\
	bitmap-font-ref.png	\
	bitmap-font-rgb24-ref.png	\
	caps-joins-alpha-quartz-ref.png	\
	caps-joins-alpha-ref.png	\
	caps-joins-alpha-svg-ref.png	\
	caps-joins-ref.png	\
	caps-sub-paths-ref.png	\
	clip-all-ref.png	\
	clip-fill-rule-pixel-aligned-ref.png	\
	clip-fill-rule-pixel-aligned-rgb24-ref.png	\
	clip-fill-rule-ps-argb32-ref.png	\
	clip-fill-rule-ref.png	\
	clip-fill-rule-rgb24-ref.png	\
	clip-nesting-ps-argb32-ref.png	\
	clip-nesting-quartz-ref.png	\
	clip-nesting-quartz-rgb24-ref.png	\
	clip-nesting-ref.png	\
	clip-nesting-rgb24-ref.png	\
	clip-operator-ref.png	\
	clip-operator-rgb24-ref.png	\
	clip-push-group-quartz-ref.png	\
	clip-push-group-ref.png	\
	clip-twice-ps-argb32-ref.png	\
	clip-twice-quartz-ref.png	\
	clip-twice-quartz-rgb24-ref.png	\
	clip-twice-ref.png	\
	clip-twice-rgb24-ref.png	\
	close-path-ref.png	\
	composite-integer-translate-over-ref.png	\
	composite-integer-translate-over-repeat-ref.png	\
	composite-integer-translate-source-ref.png	\
	copy-path-ps-argb32-ref.png	\
	copy-path-ref.png	\
	create-from-png-ref.png	\
	create-from-png-stream-ref.png	\
	dash-caps-joins-ps-argb32-ref.png	\
	dash-caps-joins-quartz-ref.png	\
	dash-caps-joins-ref.png	\
	dash-no-dash-ref.png	\
	dash-offset-negative-ref.png	\
	dash-scale-ps-argb32-ref.png	\
	dash-scale-ref.png	\
	dash-state-ps-argb32-ref.png	\
	dash-state-ref.png	\
	dash-zero-length-ps-argb32-ref.png	\
	dash-zero-length-ref.png	\
	dash-zero-length-rgb24-ref.png	\
	degenerate-path-ps-argb32-ref.png	\
	degenerate-path-ref.png	\
	degenerate-path-rgb24-ref.png	\
	device-offset-positive-ref.png	\
	device-offset-positive-rgb24-ref.png	\
	device-offset-ref.png	\
	device-offset-rgb24-ref.png	\
	extend-pad-ref.png	\
	extend-reflect-ref.png	\
	fill-and-stroke-alpha-add-quartz-ref.png	\
	fill-and-stroke-alpha-add-ref.png	\
	fill-and-stroke-alpha-quartz-ref.png	\
	fill-and-stroke-alpha-ref.png	\
	fill-and-stroke-ps-argb32-ref.png	\
	fill-and-stroke-quartz-ref.png	\
	fill-and-stroke-quartz-rgb24-ref.png	\
	fill-and-stroke-ref.png	\
	fill-and-stroke-rgb24-ref.png	\
	fill-degenerate-sort-order-quartz-ref.png	\
	fill-degenerate-sort-order-quartz-rgb24-ref.png	\
	fill-degenerate-sort-order-ref.png	\
	fill-degenerate-sort-order-rgb24-ref.png	\
	fill-missed-stop-ps-argb32-ref.png	\
	fill-missed-stop-ref.png	\
	fill-missed-stop-rgb24-ref.png	\
	fill-rule-ps-argb32-ref.png	\
	fill-rule-quartz-ref.png	\
	fill-rule-quartz-rgb24-ref.png	\
	fill-rule-ref.png	\
	fill-rule-rgb24-ref.png	\
	filter-nearest-offset-ref.png	\
	font-matrix-translation-ps-argb32-ref.png	\
	font-matrix-translation-ref.png	\
	font-matrix-translation-svg-ref.png	\
	ft-text-antialias-none-ps-argb32-ref.png	\
	ft-text-antialias-none-ref.png	\
	ft-text-vertical-layout-type1-ps-argb32-ref.png	\
	ft-text-vertical-layout-type1-ref.png	\
	ft-text-vertical-layout-type1-svg-ref.png	\
	ft-text-vertical-layout-type3-ps-argb32-ref.png	\
	ft-text-vertical-layout-type3-ref.png	\
	ft-text-vertical-layout-type3-svg-ref.png	\
	get-group-target-ref.png	\
	glyph-cache-pressure-ref.png	\
	gradient-alpha-ref.png	\
	gradient-alpha-rgb24-ref.png	\
	infinite-join-ref.png	\
	leaky-dash-ps-argb32-ref.png	\
	leaky-dash-ref.png	\
	leaky-polygon-ref.png	\
	linear-gradient-reflect-ref.png	\
	linear-gradient-ref.png	\
	linear-gradient-svg-ref.png	\
	line-width-ref.png	\
	line-width-scale-ps-argb32-ref.png	\
	line-width-scale-ref.png	\
	long-lines-ref.png	\
	mask-ctm-ref.png	\
	mask-ctm-rgb24-ref.png	\
	mask-ctm-svg-argb32-ref.png	\
	mask-ref.png	\
	mask-rgb24-ref.png	\
	mask-surface-ctm-ref.png	\
	mask-surface-ctm-rgb24-ref.png	\
	mask-surface-ctm-svg-argb32-ref.png	\
	mask-svg-argb32-ref.png	\
	move-to-show-surface-ref.png	\
	new-sub-path-ps-argb32-ref.png	\
	new-sub-path-quartz-ref.png	\
	new-sub-path-quartz-rgb24-ref.png	\
	new-sub-path-ref.png	\
	new-sub-path-rgb24-ref.png	\
	nil-surface-ref.png	\
	nil-surface-rgb24-ref.png	\
	operator-clear-quartz-ref.png	\
	operator-clear-quartz-rgb24-ref.png	\
	operator-clear-ref.png	\
	operator-clear-rgb24-ref.png	\
	operator-source-ref.png	\
	operator-source-rgb24-ref.png	\
	paint-ref.png	\
	paint-repeat-ref.png	\
	paint-source-alpha-pdf-argb32-ref.png	\
	paint-source-alpha-ref.png	\
	paint-source-alpha-svg-ref.png	\
	paint-with-alpha-ref.png	\
	paint-with-alpha-svg-ref.png	\
	pattern-getters-ref.png	\
	pixman-rotate-ref.png	\
	pixman-rotate-rgb24-ref.png	\
	push-group-ref.png	\
	push-group-rgb24-ref.png	\
	push-group-svg-argb32-ref.png	\
	radial-gradient-ref.png	\
	random-intersections-ps-argb32-ref.png	\
	random-intersections-ref.png	\
	rectangle-rounding-error-ref.png	\
	rectilinear-stroke-ref.png	\
	rel-path-quartz-ref.png	\
	rel-path-quartz-rgb24-ref.png	\
	rel-path-ref.png	\
	rel-path-rgb24-ref.png	\
	rotate-image-surface-paint-pdf-argb32-ref.png	\
	rotate-image-surface-paint-ps-argb32-ref.png	\
	rotate-image-surface-paint-ref.png	\
	rotate-image-surface-paint-svg-ref.png	\
	scale-down-source-surface-paint-ref.png	\
	scale-source-surface-paint-pdf-argb32-ref.png	\
	scale-source-surface-paint-ref.png	\
	scale-source-surface-paint-rgb24-ref.png	\
	scale-source-surface-paint-svg-argb32-ref.png	\
	scale-source-surface-paint-svg-rgb24-ref.png	\
	select-font-face-ref.png	\
	self-copy-ref.png	\
	self-intersecting-ref.png	\
	self-intersecting-rgb24-ref.png	\
	set-source-ref.png	\
	set-source-rgb24-ref.png	\
	set-source-svg-argb32-ref.png	\
	show-glyphs-many-ref.png	\
	show-text-current-point-ref.png	\
	skew-extreme-ref.png \
	source-clip-ref.png	\
	source-clip-scale-quartz-ref.png	\
	source-clip-scale-ref.png	\
	source-clip-scale-svg-ref.png	\
	source-surface-scale-paint-ref.png	\
	source-surface-scale-paint-rgb24-ref.png	\
	surface-pattern-pdf-argb32-ref.png	\
	surface-pattern-ps-argb32-ref.png	\
	surface-pattern-ref.png	\
	surface-pattern-scale-down-pdf-argb32-ref.png	\
	surface-pattern-scale-down-ps-argb32-ref.png	\
	surface-pattern-scale-down-ref.png	\
	surface-pattern-scale-up-pdf-argb32-ref.png	\
	surface-pattern-scale-up-ps-argb32-ref.png	\
	surface-pattern-scale-up-ref.png	\
	surface-pattern-svg-ref.png	\
	text-antialias-gray-ref.png	\
	text-antialias-none-ref.png	\
	text-antialias-subpixel-ref.png	\
	text-glyph-range-ref.png	\
	text-glyph-range-rgb24-ref.png	\
	text-pattern-ref.png	\
	text-pattern-rgb24-ref.png	\
	text-pattern-svg-argb32-ref.png	\
	text-pattern-svg-rgb24-ref.png	\
	text-rotate-ps-argb32-ref.png	\
	text-rotate-ref.png	\
	text-rotate-svg-ref.png	\
	text-zero-len-ref.png	\
	transforms-ref.png	\
	translate-show-surface-ref.png	\
	trap-clip-quartz-ref.png	\
	trap-clip-quartz-rgb24-ref.png	\
	trap-clip-ref.png	\
	trap-clip-rgb24-ref.png	\
	unantialiased-shapes-ref.png	\
	unbounded-operator-ref.png	\
	unbounded-operator-rgb24-ref.png	\
	zero-alpha-ref.png

EXTRA_DIST =		\
6x13.pcf		\
make-html.pl		\
romedalen.png		\
$(REFERENCE_IMAGES)

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
big-trap			\
extend-pad			\
filter-nearest-offset		\
long-lines			\
self-intersecting		\
surface-pattern			\
surface-pattern-scale-down	\
surface-pattern-scale-up

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
EXTRA_PROGRAMS =

# We're using _GNU_SOURCE to get the prototype for asprintf. This may
# not be the most portable approach, but it is pragmatic and I'm
# willing to do something cleaner as soon as it causes someone a
# problem.
INCLUDES =					\
	-D_GNU_SOURCE				\
	-I$(srcdir)				\
	-I$(srcdir)/pdiff			\
	-I$(top_srcdir)/boilerplate		\
	-I$(top_srcdir)/pixman/src		\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src			\
	$(CAIRO_CFLAGS)

EXTRA_LTLIBRARIES = libcairotest.la

libcairotest_la_SOURCES =\
	buffer-diff.c	\
	buffer-diff.h	\
	cairo-test.c	\
	cairo-test.h
libcairotest_la_LIBADD =\
	$(top_builddir)/test/pdiff/libpdiff.la \
        $(top_builddir)/boilerplate/libcairoboilerplate.la \
	$(top_builddir)/src/libcairo.la \
	$(CAIRO_LDADD)

LDADD = $(CAIRO_LDADD) libcairotest.la

$(top_builddir)/boilerplate/libcairoboilerplate.la: $(top_builddir)/src/libcairo.la
	cd $(top_builddir)/boilerplate && $(MAKE) $(AM_MAKEFLAGS) libcairoboilerplate.la

$(top_builddir)/src/libcairo.la:
	cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libcairo.la

$(top_builddir)/test/pdiff/libpdiff.la:
	cd $(top_builddir)/test/pdiff && $(MAKE) $(AM_MAKEFLAGS) libpdiff.la

if HAVE_PTHREAD
LDADD += -lpthread
endif

SUPPORT_PROGS += imagediff$(EXEEXT) png-flatten$(EXEEXT)

if CAIRO_CAN_TEST_PDF_SURFACE
SUPPORT_PROGS += pdf2png$(EXEEXT)
pdf2png_CFLAGS = $(POPPLER_CFLAGS)
# add LDADD, so poppler uses "our" cairo
pdf2png_LDADD  = $(LDADD) $(POPPLER_LIBS)
if CAIRO_HAS_SVG_SURFACE
EXTRA_PROGRAMS += pdf2svg$(EXEEXT)
pdf2svg_CFLAGS = $(POPPLER_CFLAGS)
# add LDADD, so poppler uses "our" cairo
pdf2svg_LDADD  = $(LDADD) $(POPPLER_LIBS)
endif
endif

if CAIRO_CAN_TEST_SVG_SURFACE
SUPPORT_PROGS += svg2png$(EXEEXT)
svg2png_CFLAGS = $(LIBRSVG_CFLAGS)
# add LDADD, so librsvg uses "our" cairo
svg2png_LDADD  = $(LDADD) $(LIBRSVG_LIBS)
if CAIRO_HAS_PDF_SURFACE
EXTRA_PROGRAMS += svg2pdf$(EXEEXT)
svg2pdf_CFLAGS = $(LIBRSVG_CFLAGS)
# add LDADD, so librsvg uses "our" cairo
svg2pdf_LDADD  = $(LDADD) $(LIBRSVG_LIBS)
endif
endif

EXTRA_PROGRAMS += $(TESTS) $(DISABLED_TESTS) $(SUPPORT_PROGS)

# Do a funny transition of CAIRO_TEST_TARGET through TARGETS such that
# one can limit tested targets both through CAIRO_TEST_TARGET env var
# and TARGETS make var on the command line.
TARGETS = $(CAIRO_TEST_TARGET)

TESTS_ENVIRONMENT = CAIRO_XFAIL_TESTS="$(XFAIL_TESTS)" CAIRO_TEST_TARGET="$(TARGETS)"

EXTRA_VALGRIND_FLAGS = $(CAIRO_EXTRA_VALGRIND_FLAGS)
VALGRIND_FLAGS = \
	--tool=memcheck --suppressions=$(srcdir)/.valgrind-suppressions \
	--leak-check=yes --show-reachable=yes $(EXTRA_VALGRIND_FLAGS)

CLEANFILES =					\
	valgrind-log				\
	index.html				\
	ref.hash				\
	$(EXTRA_LTLIBRARIES)			\
	$(EXTRA_PROGRAMS)

# This used to be a simple 'echo ${RM} *.ps *.pdf *.svg *.etc', but
# most systems cannot handle all of our clean files together.
# Then it became a fancy find using many GNU extensions, but then the ugly
# reality of portability was raised and it became....
clean-local:
	-${FIND} . -name '*.ps'       -print | ${XARGS} ${RM}
	-${FIND} . -name '*.pdf'      -print | ${XARGS} ${RM}
	-${FIND} . -name '*.svg'      -print | ${XARGS} ${RM}
	-${FIND} . -name '*-out.png'  -print | ${XARGS} ${RM}
	-${FIND} . -name '*-diff.png' -print | ${XARGS} ${RM}
	-${FIND} . -name '*.log'      -print | ${XARGS} ${RM}

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
