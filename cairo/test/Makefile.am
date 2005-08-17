# All test cases go here
TESTS = 			\
a8-mask				\
clip-nesting			\
clip-operator			\
clip-twice			\
composite-integer-translate-source \
composite-integer-translate-over \
composite-integer-translate-over-repeat \
create-from-png			\
create-from-png-stream		\
fill-and-stroke			\
fill-rule			\
filter-nearest-offset		\
get-and-set			\
gradient-alpha			\
leaky-polygon			\
line-width			\
linear-gradient			\
mask				\
mask-ctm			\
mask-surface-ctm		\
move-to-show-surface		\
paint				\
paint-with-alpha		\
path-data			\
pixman-rotate			\
scale-source-surface-paint	\
select-font-no-show-text	\
self-copy			\
self-intersecting		\
set-source			\
source-clip			\
source-surface-scale-paint	\
surface-finish-twice		\
surface-pattern			\
text-antialias-gray		\
text-antialias-none		\
text-antialias-subpixel		\
text-cache-crash		\
text-rotate			\
transforms			\
translate-show-surface		\
trap-clip			\
unantialiased-shapes		\
unbounded-operator		\
user-data			\
rel-path

if CAIRO_HAS_PDF_SURFACE
TESTS += pdf-surface pdf-clip
endif

if CAIRO_HAS_PS_SURFACE
TESTS += ps-surface
endif

if CAIRO_HAS_XLIB_SURFACE
TESTS += xlib-surface
endif

# All tests which have a reference image go here.
# I really don't like having to repeat this list. Anyone know a good
# way to avoid it? Can I use a wildcard here?
EXTRA_DIST =				\
a8-mask-ref.png				\
clip-nesting-ref.png			\
clip-operator-ref.png			\
clip-twice-ref.png			\
composite-integer-translate-source-ref.png		\
composite-integer-translate-over-ref.png		\
composite-integer-translate-over-repeat-ref.png		\
create-from-png-ref.png			\
create-from-png-stream-ref.png		\
fill-and-stroke-ref.png			\
fill-rule-ref.png			\
filter-nearest-offset-ref.png		\
gradient-alpha-ref.png			\
leaky-polygon-ref.png			\
line-width-ref.png			\
linear-gradient-ref.png			\
mask-ref.png				\
mask-ctm-ref.png			\
mask-surface-ctm-ref.png		\
move-to-show-surface-ref.png		\
paint-ref.png				\
paint-with-alpha-ref.png		\
path-data-ref.png			\
pixman-rotate-ref.png			\
romedalen.png				\
self-copy-ref.png			\
self-intersecting-ref.png		\
scale-source-surface-paint-ref.png	\
set-source-ref.png			\
source-clip-ref.png			\
source-surface-scale-paint-ref.png	\
surface-pattern-ref.png			\
text-antialias-gray-ref.png		\
text-antialias-none-ref.png		\
transforms-ref.png			\
translate-show-surface-ref.png		\
trap-clip-ref.png			\
unantialiased-shapes-ref.png		\
unbounded-operator-ref.png		\
rel-path-ref.png

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
filter-nearest-offset		\
pixman-rotate			\
self-intersecting		\
text-antialias-subpixel		\
text-rotate

check_PROGRAMS = $(TESTS)

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

noinst_LTLIBRARIES = libcairotest.la

libcairotest_la_SOURCES =\
buffer-diff.c	\
buffer-diff.h	\
cairo-test.c	\
cairo-test.h	\
read-png.c	\
read-png.h	\
write-png.c	\
write-png.h	\
xmalloc.c	\
xmalloc.h

LDADDS = libcairotest.la $(top_builddir)/src/libcairo.la

if HAVE_PTHREAD
LDADDS += -lpthread
endif

# ARGH! I have to repeat the list of tests a third time. Maybe it's
# time to break down and auto-generate the Makefile.am or something
# from autogen.sh. My, but this is painful...
a8_mask_LDADD = $(LDADDS)
clip_nesting_LDADD = $(LDADDS)
clip_operator_LDADD = $(LDADDS)
clip_twice_LDADD = $(LDADDS)
composite_integer_translate_source_LDADD = $(LDADDS)
composite_integer_translate_over_LDADD = $(LDADDS)
composite_integer_translate_over_repeat_LDADD = $(LDADDS)
create_from_png_LDADD = $(LDADDS)
create_from_png_stream_LDADD = $(LDADDS)
fill_and_stroke_LDADD = $(LDADDS)
fill_rule_LDADD = $(LDADDS)
filter_nearest_offset_LDADD = $(LDADDS)
get_and_set_LDADD = $(LDADDS)
gradient_alpha_LDADD = $(LDADDS)
leaky_polygon_LDADD = $(LDADDS)
line_width_LDADD = $(LDADDS)
linear_gradient_LDADD = $(LDADDS)
mask_LDADD = $(LDADDS)
mask_ctm_LDADD = $(LDADDS)
mask_surface_ctm_LDADD = $(LDADDS)
move_to_show_surface_LDADD = $(LDADDS)
paint_LDADD = $(LDADDS)
paint_with_alpha_LDADD = $(LDADDS)
path_data_LDADD = $(LDADDS)
pdf_surface_LDADD = $(LDADDS)
pdf_clip_LDADD = $(LDADDS)
ps_surface_LDADD = $(LDADDS)
pixman_rotate_LDADD = $(LDADDS)
scale_source_surface_paint_LDADD = $(LDADDS)
select_font_no_show_text_LDADD = $(LDADDS)
self_copy_LDADD = $(LDADDS)
self_intersecting_LDADD = $(LDADDS)
set_source_LDADD = $(LDADDS)
source_clip_LDADD = $(LDADDS)
source_surface_scale_paint_LDADD = $(LDADDS)
surface_finish_twice_LDADD = $(LDADDS)
surface_pattern_LDADD = $(LDADDS)
text_antialias_gray_LDADD = $(LDADDS)
text_antialias_none_LDADD = $(LDADDS)
text_antialias_subpixel_LDADD = $(LDADDS)
text_cache_crash_LDADD = $(LDADDS)
text_rotate_LDADD = $(LDADDS)
transforms_LDADD = $(LDADDS)
translate_show_surface_LDADD = $(LDADDS)
trap_clip_LDADD = $(LDADDS)
unantialiased_shapes_LDADD = $(LDADDS)
unbounded_operator_LDADD = $(LDADDS)
user_data_LDADD = $(LDADDS)
rel_path_LDADD = $(LDADDS)
xlib_surface_LDADD = $(LDADDS)

noinst_PROGRAMS = imagediff
imagediff_LDADD = $(LDADDS)

CLEANFILES =					\
	*-out.png				\
	*-diff.png				\
	*.log					\
	ps-surface.ps				\
	pdf-surface.pdf				\
	pdf-clip.pdf

check-valgrind:
	TESTS_ENVIRONMENT="libtool --mode=execute valgrind --tool=memcheck --suppressions=./.valgrind-suppressions --leak-check=yes --show-reachable=yes" $(MAKE) check 2>&1 | tee valgrind.log
