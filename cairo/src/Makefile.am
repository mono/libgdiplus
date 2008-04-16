backend_pkgconfigs =

cairo_base_headers = cairo.h cairo-embed.h

cairo_base_sources =				\
	cairo.c					\
	cairo.h					\
	cairo-private.h				\
	cairo-arc.c				\
	cairo-arc-private.h			\
	cairo-array.c				\
	cairo-atomic.c				\
	cairo-atomic-private.h			\
	cairo-base85-stream.c			\
	cairo-bentley-ottmann.c			\
	cairo-cache.c				\
	cairo-clip.c				\
	cairo-clip-private.h			\
	cairo-color.c				\
	cairo-compiler-private.h		\
	cairo-debug.c				\
	cairo-fixed.c				\
	cairo-font-face.c			\
	cairo-font-options.c			\
	cairo-freelist.c			\
	cairo-freelist-private.h		\
	cairo-gstate.c				\
	cairo-gstate-private.h			\
	cairo-hash.c				\
	cairo-hash-private.h			\
	cairo-hull.c				\
	cairo-image-surface.c			\
	cairo-lzw.c				\
	cairo-misc.c				\
	cairo-malloc-private.h			\
	cairo-matrix.c				\
	cairo-mutex.c				\
	cairo-mutex-private.h			\
	cairo-mutex-type-private.h		\
	cairo-path.c				\
	cairo-path-private.h			\
	cairo-path-bounds.c			\
	cairo-path-fill.c			\
	cairo-path-fixed.c			\
	cairo-path-fixed-private.h		\
	cairo-path-stroke.c			\
	cairo-pen.c				\
	cairo-polygon.c				\
	cairo-rectangle.c			\
	cairo-reference-count-private.h		\
	cairo-region.c				\
	cairo-region-private.h			\
	cairo-scaled-font.c			\
	cairo-scaled-font-private.h		\
	cairo-skiplist.c			\
	cairo-skiplist-private.h		\
	cairo-slope.c				\
	cairo-spline.c				\
	cairo-stroke-style.c			\
	cairo-surface.c				\
	cairo-surface-fallback.c		\
	cairo-surface-fallback-private.h	\
	cairo-surface-private.h			\
	cairo-traps.c				\
	cairo-pattern.c				\
	cairo-unicode.c				\
	cairo-output-stream.c			\
	cairo-output-stream-private.h		\
	cairo-wideint.c				\
	cairo-wideint-private.h			\
	cairo-wideint-type-private.h		\
	cairo-meta-surface.c			\
	cairo-meta-surface-private.h		\
	cairo-paginated-private.h		\
	cairo-paginated-surface.c		\
	cairo-paginated-surface-private.h	\
	cairo-analysis-surface.c		\
	cairo-analysis-surface-private.h	\
	cairoint.h
cairo_all_sources = $(cairo_base_sources) $(cairo_base_headers)
cairo_headers = $(cairo_base_headers)
cairo_sources = $(cairo_base_sources)

font_subset_sources_base =			\
	cairo-cff-subset.c			\
	cairo-truetype-subset.c			\
	cairo-type1-fallback.c			\
	cairo-type1-private.h			\
	cairo-truetype-subset-private.h		\
	cairo-scaled-font-subsets.c		\
	cairo-scaled-font-subsets-private.h
cairo_all_sources += $(font_subset_sources_base)

font_subset_sources_ft = cairo-type1-subset.c
cairo_all_sources += $(font_subset_sources_ft)
font_subset_sources = $(font_subset_sources_base)
if CAIRO_HAS_FT_FONT
font_subset_sources += $(font_subset_sources_ft)
endif

ps_headers = cairo-ps.h
ps_sources = cairo-ps-surface.c cairo-ps-surface-private.h
cairo_all_sources += $(ps_headers) $(ps_sources)
if CAIRO_HAS_PS_SURFACE
libcairo_font_subset_sources =	$(font_subset_sources)
cairo_headers += $(ps_headers)
cairo_sources += $(ps_sources)
backend_pkgconfigs += cairo-ps.pc
endif

pdf_headers = cairo-pdf.h
pdf_sources = cairo-pdf-surface.c cairo-pdf-surface-private.h \
	      cairo-pdf-operators.c cairo-pdf-operators-private.h \
	      cairo-deflate-stream.c
cairo_all_sources += $(pdf_headers) $(pdf_sources)
if CAIRO_HAS_PDF_SURFACE
libcairo_font_subset_sources =	$(font_subset_sources)
cairo_headers += $(pdf_headers)
cairo_sources += $(pdf_sources)
backend_pkgconfigs += cairo-pdf.pc
endif

png_sources = cairo-png.c
cairo_all_sources += $(png_sources)
if CAIRO_HAS_PNG_FUNCTIONS
cairo_sources += $(png_sources)
backend_pkgconfigs += cairo-png.pc
endif

svg_headers = cairo-svg.h
svg_sources = cairo-svg-surface.c cairo-svg-surface-private.h
cairo_all_sources += $(svg_headers) $(svg_sources)
if CAIRO_HAS_SVG_SURFACE
libcairo_font_subset_sources =	$(font_subset_sources)
cairo_headers += $(svg_headers)
cairo_sources += $(svg_sources)
backend_pkgconfigs += cairo-svg.pc
endif

cairo_sources += $(libcairo_font_subset_sources)

test_sources = test-fallback-surface.c test-fallback-surface.h \
	       test-meta-surface.c test-meta-surface.h		\
	       test-paginated-surface.c test-paginated-surface.h
cairo_all_sources += $(test_sources)
if CAIRO_HAS_TEST_SURFACES
cairo_sources += $(test_sources)
endif

xlib_headers =  cairo-xlib.h
xlib_sources =  cairo-xlib-surface.c \
		cairo-xlib-surface-private.h \
		cairo-xlib-display.c \
		cairo-xlib-screen.c \
		cairo-xlib-visual.c \
		cairo-xlib-private.h \
		cairo-xlib-xrender-private.h
cairo_all_sources += $(xlib_headers) $(xlib_sources)
if CAIRO_HAS_XLIB_SURFACE
cairo_headers += $(xlib_headers)
cairo_sources += $(xlib_sources)
backend_pkgconfigs += cairo-xlib.pc
endif

xlib_xrender_headers = cairo-xlib-xrender.h
cairo_all_sources += $(xlib_xrender_headers)
if CAIRO_HAS_XLIB_XRENDER_SURFACE
cairo_headers += $(xlib_xrender_headers)
backend_pkgconfigs += cairo-xlib-xrender.pc
endif

xcb_headers = cairo-xcb.h cairo-xcb-xrender.h
xcb_sources = cairo-xcb-surface.c
cairo_all_sources += $(xcb_headers) $(xcb_sources)
if CAIRO_HAS_XCB_SURFACE
cairo_headers += $(xcb_headers)
cairo_sources += $(xcb_sources)
backend_pkgconfigs += cairo-xcb.pc
endif

quartz_headers = cairo-quartz.h
quartz_sources = cairo-quartz-surface.c cairo-quartz-image-surface.c cairo-quartz-private.h
cairo_all_sources += $(quartz_headers) $(quartz_sources)
if CAIRO_HAS_QUARTZ_SURFACE
cairo_headers += $(quartz_headers)
cairo_sources += $(quartz_sources)
backend_pkgconfigs += cairo-quartz.pc
endif
if CAIRO_HAS_QUARTZ_IMAGE_SURFACE
cairo_headers += cairo-quartz-image.h
backend_pkgconfigs += cairo-quartz-image.pc
endif

quartz_font_sources = cairo-quartz-font.c
cairo_all_sources += $(quartz_font_sources)
if CAIRO_HAS_QUARTZ_FONT
cairo_sources += $(quartz_font_sources)
endif

os_win32_sources = cairo-win32.c
cairo_all_sources += $(os_win32_sources)
if OS_WIN32
cairo_sources += $(os_win32_sources)
export_symbols = -export-symbols cairo.def
cairo_def_dependency = cairo.def
endif

win32_headers = cairo-win32.h
win32_sources = cairo-win32-surface.c cairo-win32-printing-surface.c cairo-win32-private.h
cairo_all_sources += $(win32_headers) $(win32_sources)
if CAIRO_HAS_WIN32_SURFACE
cairo_headers += $(win32_headers)
cairo_sources += $(win32_sources)
backend_pkgconfigs += cairo-win32.pc
endif

win32_font_sources = cairo-win32-font.c
cairo_all_sources += $(win32_font_sources)
if CAIRO_HAS_WIN32_FONT
cairo_sources += $(win32_font_sources)
backend_pkgconfigs += cairo-win32-font.pc
endif

os2_headers = cairo-os2.h
os2_sources = cairo-os2-surface.c cairo-os2-private.h
cairo_all_sources += $(os2_headers) $(os2_sources)
if CAIRO_HAS_OS2_SURFACE
cairo_headers += $(os2_headers)
cairo_sources += $(os2_sources)
backend_pkgconfigs += cairo-os2.pc
endif

beos_headers = cairo-beos.h
beos_sources = cairo-beos-surface.cpp
cairo_all_sources += $(beos_headers) $(beos_sources)
if CAIRO_HAS_BEOS_SURFACE
cairo_headers += $(beos_headers)
backend_pkgconfigs += cairo-beos.pc

#noinst_LTLIBRARIES = libcairo_beos.la
#libcairo_beos_la_SOURCES = $(libcairo_beos_sources)
# BeOS system headers trigger this warning
#libcairo_beos_la_CXXFLAGS = -Wno-multichar
endif

glitz_headers = cairo-glitz.h
glitz_sources = cairo-glitz-surface.c cairo-glitz-private.h
cairo_all_sources += $(glitz_headers) $(glitz_sources)
if CAIRO_HAS_GLITZ_SURFACE
cairo_headers += $(glitz_headers)
cairo_sources += $(glitz_sources)
backend_pkgconfigs += cairo-glitz.pc
endif

directfb_headers = cairo-directfb.h
directfb_sources = cairo-directfb-surface.c
cairo_all_sources += $(directfb_headers) $(directfb_sources)
if CAIRO_HAS_DIRECTFB_SURFACE
cairo_headers += $(directfb_headers)
cairo_sources += $(directfb_sources)
backend_pkgconfigs += cairo-directfb.pc
endif

ft_headers = cairo-ft.h
ft_sources = cairo-ft-font.c cairo-ft-private.h
cairo_all_sources += $(ft_headers) $(ft_sources)
if CAIRO_HAS_FT_FONT
cairo_headers += $(ft_headers)
cairo_sources += $(ft_sources)
backend_pkgconfigs += cairo-ft.pc
endif

# These names match automake style variable definition conventions so
# without these lines, automake will complain during the handling of 
# the libcairo_la_LIBADD below.  (The INCLUDES is an autoconf only
# term and automake does not care about it)
FONTCONFIG_LIBS=@FONTCONFIG_LIBS@
XRENDER_LIBS=@XRENDER_LIBS@

cairoincludedir = $(includedir)/cairo
noinst_HEADERS =			\
	cairo-deprecated.h		\
	$(cairo_headers)

nodist_noinst_HEADERS =		\
	cairo-features.h

noinst_LTLIBRARIES = libcairo.la

# source files that cannot be passed to compiler directly.
# mostly when they depend recursively on cairoint.h.  that is,
# cairoint.h includes them in the middle.
cairo_special_sources =				\
	cairo-cache-private.h			\
	cairo-fixed-private.h			\
	cairo-fixed-type-private.h		\
	cairo-mutex-list-private.h		\
	cairo-types-private.h

libcairo_la_SOURCES =				\
	$(cairo_sources)			\
	$(cairo_special_sources)

cairo_all_source_file =				\
	$(cairo_all_sources)			\
	$(cairo_special_sources)

libcairo_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined $(export_symbols)

libcairo_la_CFLAGS = -I$(srcdir) $(CAIRO_CFLAGS)

libcairo_la_LIBADD = $(CAIRO_LIBS) $(CAIRO_LDADD)

libcairo_la_DEPENDENCIES = $(cairo_def_dependency)

pkgconfigdir = $(libdir)/pkgconfig
noinst_DATA = cairo.pc $(backend_pkgconfigs)
DISTCLEANFILES = $(backend_pkgconfigs)
EXTRA_DIST = cairo-backend.pc.in
$(backend_pkgconfigs): $(top_srcdir)/configure
	cd $(top_builddir) && ./config.status --recheck

$(top_builddir)/config.h: $(top_srcdir)/config.h.in
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) config.h

DISTCLEANFILES += cairo-features.h cairo-no-features.h
cairo-features.h cairo-no-features.h:
	cd $(top_builddir) && ./config.status src/$@

CLEANFILES = cairo.def
cairo.def: $(cairo_headers) cairo-features.h
	@echo Generating $@
	@(echo EXPORTS; \
	(cd $(srcdir); cat $(cairo_headers) || echo 'cairo_ERROR ()' ) | \
	grep -v -E '^# *include' | \
	( gcc -include cairo-features.h -E - || echo 'cairo_ERROR ()' ) | \
	grep -E '^cairo_.* \(' | \
	sed -e 's/[ 	].*//' | \
	sort; \
	echo LIBRARY libcairo-$(LT_CURRENT_MINUS_AGE).dll; \
	) >$@
	@ ! grep -q cairo_ERROR $@ || ($(RM) $@; false)

install-data-local:
	@if test -f "$(DESTDIR)$(includedir)/cairo.h" || test -f "$(DESTDIR)$(includedir)/cairo-features.h" ; then \
	  echo "****************************************************************" ;	\
	  echo "*** Error: Old headers found. You should remove the following" ;	\
	  echo "***        files and then type 'make install' again." ;			\
	  ls $(includedir)/cairo*.h ;							\
	  echo "****************************************************************" ;	\
	  false ;									\
        fi

TESTS_ENVIRONMENT = srcdir="$(srcdir)" MAKE="$(MAKE)" cairo_all_source_file="$(cairo_all_source_file)"
TESTS = check-def.sh check-plt.sh check-headers.sh check-cairoint.sh check-doc-syntax.sh
EXTRA_DIST += $(TESTS) check-has-hidden-symbols.c
DISTCLEANFILES += check-has-hidden-symbols.i

# The pre-processed result is used by check-{def,plt}.sh to determine whether
# cairo has been compiled with symbol hiding.
.c.i: $(noinst_HEADERS) $(nodist_noinst_HEADERS) cairoint.h $(top_builddir)/config.h
	$(CPP) -DHAVE_CONFIG_H -I$(top_builddir) -I. $(libcairo_la_CFLAGS) $< -o $@

SPARSE = sparse
sparse:
	@status=true; for f in $(cairo_sources); do \
		echo sparse $$f; \
		$(SPARSE) -I$(top_builddir) $(libcairo_la_CFLAGS) -DHAVE_CONFIG_H $$f || status=false; \
	done; $$status

SPLINT = splint -badflag
splint:
	@status=true; for f in $(cairo_sources); do \
		test "$$f" = "`echo "$$f" | sed 's/[.]h$$//'`" || continue; \
		echo sparse $$f; \
		$(SPLINT) -I$(top_builddir) $(libcairo_la_CFLAGS) -DHAVE_CONFIG_H $$f || status=false; \
	done; $$status

EXTRA_DIST += Makefile.win32
