font_subset_sources =				\
	cairo-font-subset.c			\
	cairo-font-subset-private.h		\
	cairo-type1-subset.c			\
	cairo-scaled-font-subsets.c		\
	cairo-scaled-font-subsets-private.h

backend_pkgconfigs =

if CAIRO_HAS_PS_SURFACE
libcairo_ps_headers = cairo-ps.h
libcairo_ps_sources = cairo-ps-surface.c cairo-ps-test.h
libcairo_font_subset_sources =	$(font_subset_sources)
backend_pkgconfigs += cairo-ps.pc
endif

if CAIRO_HAS_PDF_SURFACE
libcairo_pdf_headers = cairo-pdf.h
libcairo_pdf_sources = cairo-pdf-surface.c cairo-pdf-test.h
libcairo_font_subset_sources =	$(font_subset_sources)
backend_pkgconfigs += cairo-pdf.pc
endif

if CAIRO_HAS_PNG_FUNCTIONS
libcairo_png_sources = cairo-png.c
backend_pkgconfigs += cairo-png.pc
endif

if CAIRO_HAS_SVG_SURFACE
libcairo_svg_headers = cairo-svg.h
libcairo_svg_sources = cairo-svg-surface.c cairo-svg-test.h
libcairo_font_subset_sources =	$(font_subset_sources)
backend_pkgconfigs += cairo-svg.pc
endif

if CAIRO_HAS_TEST_SURFACES
libcairo_test_sources = test-fallback-surface.c test-fallback-surface.h \
			test-meta-surface.c test-meta-surface.h		\
			test-paginated-surface.c test-paginated-surface.h
endif

if CAIRO_HAS_XLIB_SURFACE
libcairo_xlib_headers = cairo-xlib.h cairo-xlib-xrender.h
libcairo_xlib_sources = cairo-xlib-surface.c cairo-xlib-screen.c cairo-xlib-private.h cairo-xlib-test.h
backend_pkgconfigs += cairo-xlib.pc
endif

if CAIRO_HAS_XLIB_XRENDER_SURFACE
libcairo_xlib_xrender_headers = cairo-xlib-xrender.h
backend_pkgconfigs += cairo-xlib-xrender.pc
endif

if CAIRO_HAS_QUARTZ_SURFACE
libcairo_quartz_headers = cairo-quartz.h
libcairo_quartz_sources = cairo-quartz-surface.c cairo-quartz-private.h
backend_pkgconfigs += cairo-quartz.pc
endif

if CAIRO_HAS_XCB_SURFACE
libcairo_xcb_headers = cairo-xcb.h cairo-xcb-xrender.h
libcairo_xcb_sources = cairo-xcb-surface.c
backend_pkgconfigs += cairo-xcb.pc
endif

libcairo_win32_sources =
if CAIRO_HAS_WIN32_SURFACE
libcairo_win32_headers = cairo-win32.h
libcairo_win32_sources += cairo-win32-surface.c cairo-win32-private.h
export_symbols = -export-symbols cairo.def
cairo_def_dependency = cairo.def
backend_pkgconfigs += cairo-win32.pc
endif
# This is not really a separate conditional.  Is TRUE iff the previous one is.
if CAIRO_HAS_WIN32_FONT
libcairo_win32_sources += cairo-win32-font.c
endif

libcairo_beos_sources =
if CAIRO_HAS_BEOS_SURFACE
libcairo_beos_headers = cairo-beos.h
libcairo_beos_sources += cairo-beos-surface.cpp
backend_pkgconfigs += cairo-beos.pc

#noinst_LTLIBRARIES = libcairo_beos.la
#libcairo_beos_la_SOURCES = $(libcairo_beos_sources)
# BeOS system headers trigger this warning
#libcairo_beos_la_CXXFLAGS = -Wno-multichar
endif

if CAIRO_HAS_GLITZ_SURFACE
libcairo_glitz_headers = cairo-glitz.h
libcairo_glitz_sources = cairo-glitz-surface.c
backend_pkgconfigs += cairo-glitz.pc
endif

if CAIRO_HAS_DIRECTFB_SURFACE
libcairo_directfb_headers = cairo-directfb.h
libcairo_directfb_sources = cairo-directfb-surface.c
backend_pkgconfigs += cairo-directfb.pc
endif

if CAIRO_HAS_FT_FONT
libcairo_ft_headers = cairo-ft.h
libcairo_ft_sources = cairo-ft-font.c cairo-ft-private.h
backend_pkgconfigs += cairo-ft.pc
endif

if CAIRO_HAS_ATSUI_FONT
libcairo_atsui_headers = cairo-atsui.h
libcairo_atsui_sources = cairo-atsui-font.c
backend_pkgconfigs += cairo-atsui.pc
endif

# Headers that declare the functions in the cairo DLL API on Windows
cairo_win32_api_headers = \
	$(srcdir)/cairo.h \
	$(srcdir)/cairo-win32.h

if CAIRO_HAS_GLITZ_SURFACE
cairo_win32_api_headers += \
	$(srcdir)/cairo-glitz.h
endif

if CAIRO_HAS_FT_FONT
cairo_win32_api_headers += \
	$(srcdir)/cairo-ft.h \
	$(srcdir)/cairo-pdf.h \
	$(srcdir)/cairo-ps.h
endif

# These names match automake style variable definition conventions so
# without these lines, automake will complain during the handling of 
# the libcairo_la_LIBADD below.  (The INCLUDES is an autoconf only
# term and automake does not care about it)
FONTCONFIG_LIBS=@FONTCONFIG_LIBS@
XRENDER_LIBS=@XRENDER_LIBS@

BUILT_SOURCES = cairo-features.h

#cairoincludedir = $(includedir)/cairo
headers = \
	cairo.h				\
	cairo-features.h		\
	$(libcairo_atsui_headers)	\
	$(libcairo_ft_headers)		\
	$(libcairo_glitz_headers)	\
	$(libcairo_pdf_headers)		\
	$(libcairo_svg_headers)		\
	$(libcairo_ps_headers)		\
	$(libcairo_quartz_headers)	\
	$(libcairo_win32_headers)	\
	$(libcairo_beos_headers)	\
	$(libcairo_xcb_headers)		\
	$(libcairo_xlib_headers)	\
	$(libcairo_xlib_xrender_headers)	\
	$(libcairo_directfb_headers)

noinst_LTLIBRARIES = libcairo.la

libcairo_la_SOURCES =				\
	cairo.c					\
	cairo.h					\
	cairo-private.h				\
	cairo-arc.c				\
	cairo-arc-private.h			\
	cairo-array.c				\
	cairo-base85-stream.c			\
	cairo-cache.c				\
	cairo-cache-private.h			\
	cairo-clip.c				\
	cairo-clip-private.h			\
	cairo-color.c				\
	cairo-debug.c				\
	cairo-fixed.c				\
	cairo-font.c				\
	cairo-font-options.c			\
	cairo-gstate.c				\
	cairo-gstate-private.h			\
	cairo-hash.c				\
	cairo-hash-private.h			\
	cairo-hull.c				\
	cairo-image-surface.c			\
	cairo-lzw.c				\
	cairo-matrix.c				\
	cairo-operator.c			\
	cairo-path.c				\
	cairo-path-bounds.c			\
	cairo-path-data.c			\
	cairo-path-data-private.h		\
	cairo-path-fill.c			\
	cairo-path-fixed-private.h		\
	cairo-path-stroke.c			\
	cairo-pen.c				\
	cairo-polygon.c				\
	cairo-rectangle.c			\
	cairo-region.c				\
	cairo-scaled-font.c			\
	cairo-scaled-font-test.h		\
	cairo-slope.c				\
	cairo-spline.c				\
	cairo-stroke-style.c			\
	cairo-surface.c				\
	cairo-surface-fallback.c		\
	cairo-surface-fallback-private.h	\
	cairo-traps.c				\
	cairo-pattern.c				\
	cairo-unicode.c				\
	cairo-output-stream.c			\
	cairo-output-stream-private.h		\
	cairo-wideint.c				\
	cairo-wideint-private.h			\
	cairo-meta-surface.c			\
	cairo-meta-surface-private.h		\
	cairo-paginated-surface.c		\
	cairo-paginated-surface-private.h	\
	cairo-analysis-surface.c		\
	cairo-analysis-surface-private.h	\
	$(libcairo_atsui_sources)		\
	$(libcairo_ft_sources)			\
	$(libcairo_ps_sources)			\
	$(libcairo_pdf_sources)			\
	$(libcairo_png_sources)			\
	$(libcairo_svg_sources)			\
	$(libcairo_test_sources)		\
	$(libcairo_font_subset_sources)		\
	$(libcairo_xlib_sources)		\
	$(libcairo_quartz_sources)		\
	$(libcairo_xcb_sources)			\
	$(libcairo_glitz_sources)		\
	$(libcairo_win32_sources)		\
	$(libcairo_directfb_sources)		\
	cairoint.h

libcairo_la_LDFLAGS = -static -version-info @VERSION_INFO@ -no-undefined $(export_symbols)

INCLUDES = -I$(srcdir) -I$(top_srcdir)/pixman/src $(CAIRO_CFLAGS)

libcairo_la_LIBADD = $(top_builddir)/pixman/src/libpixman.la $(CAIRO_LIBS) $(noinst_LTLIBRARIES)

libcairo_la_DEPENDENCIES = $(cairo_def_dependency) $(top_builddir)/pixman/src/libpixman.la 

#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = cairo.pc $(backend_pkgconfigs)
DISTCLEANFILES = $(backend_pkgconfigs)
EXTRA_DIST = cairo-backend.pc.in $(headers)
$(backend_pkgconfigs): $(top_srcdir)/configure
	$(top_builddir)/config.status --recheck

$(top_builddir)/config.h: $(top_srcdir)/config.h.in
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) config.h

cairo-features.h:
	cd $(top_builddir) && ./config.status src/cairo-features.h

cairo.def: $(cairo_win32_api_headers)
	(echo EXPORTS; \
	cat $(cairo_win32_api_headers) | \
	grep -v -E '^# *include' | \
	gcc -include cairo-features.h -E - |\
	grep -E '^cairo_.* \(' | \
	sed -e 's/\([^ ]*\).*/\1/' | \
	sort; \
	echo LIBRARY libcairo-$(LT_CURRENT_MINUS_AGE).dll; \
	) >$@

install-data-local:
	@if test -f "$(DESTDIR)$(includedir)/cairo.h" || test -f "$(DESTDIR)$(includedir)/cairo-features.h" ; then \
	  echo "****************************************************************" ;	\
	  echo "*** Error: Old headers found. You should remove the following" ;	\
	  echo "***        files and then type 'make install' again." ;			\
	  ls $(includedir)/cairo*.h ;							\
	  echo "****************************************************************" ;	\
	  false ;									\
        fi

CLEANFILES = cairo.def cairo-features.h
