
if CAIRO_HAS_PS_SURFACE
libcairo_ps_headers = cairo-ps.h
libcairo_ps_sources = cairo-ps-surface.c
libcairo_font_subset_sources = cairo-font-subset.c cairo-font-subset-private.h
endif

if CAIRO_HAS_PDF_SURFACE
libcairo_pdf_headers = cairo-pdf.h
libcairo_pdf_sources = cairo-pdf-surface.c
libcairo_font_subset_sources = cairo-font-subset.c cairo-font-subset-private.h
endif

if CAIRO_HAS_PNG_FUNCTIONS
libcairo_png_sources = cairo-png.c
endif

if CAIRO_HAS_XLIB_SURFACE
libcairo_xlib_headers = cairo-xlib.h cairo-xlib-xrender.h
libcairo_xlib_sources = cairo-xlib-surface.c cairo-xlib-screen.c cairo-xlib-private.h cairo-xlib-test.h
endif

if CAIRO_HAS_QUARTZ_SURFACE
libcairo_quartz_headers = cairo-quartz.h
libcairo_quartz_sources = cairo-quartz-surface.c
endif

if CAIRO_HAS_XCB_SURFACE
libcairo_xcb_headers = cairo-xcb.h cairo-xcb-xrender.h
libcairo_xcb_sources = cairo-xcb-surface.c
endif

libcairo_win32_sources =
if CAIRO_HAS_WIN32_SURFACE
libcairo_win32_headers = cairo-win32.h
libcairo_win32_sources += cairo-win32-surface.c cairo-win32-private.h
endif
if CAIRO_HAS_WIN32_FONT
libcairo_win32_sources += cairo-win32-font.c
endif

if CAIRO_HAS_GLITZ_SURFACE
libcairo_glitz_headers = cairo-glitz.h
libcairo_glitz_sources = cairo-glitz-surface.c
endif

if CAIRO_HAS_ATSUI_FONT
libcairo_atsui_headers = cairo-atsui.h
libcairo_atsui_sources = cairo-atsui-font.c
endif

if CAIRO_HAS_FT_FONT
libcairo_ft_headers = cairo-ft.h
libcairo_ft_sources = cairo-ft-font.c cairo-ft-private.h
endif

# These names match automake style variable definition conventions so
# without these lines, automake will complain during the handling of 
# the libcairo_la_LIBADD below.  (The INCLUDES is an autoconf only
# term and automake does not care about it)
FONTCONFIG_LIBS=@FONTCONFIG_LIBS@
XRENDER_LIBS=@XRENDER_LIBS@

cairoincludedir = $(includedir)/cairo
cairoinclude_HEADERS =			\
	cairo.h				\
	cairo-features.h		\
	$(libcairo_atsui_headers)	\
	$(libcairo_ft_headers)		\
	$(libcairo_glitz_headers)	\
	$(libcairo_pdf_headers)		\
	$(libcairo_ps_headers)		\
	$(libcairo_quartz_headers)	\
	$(libcairo_win32_headers)	\
	$(libcairo_xcb_headers)		\
	$(libcairo_xlib_headers)

lib_LTLIBRARIES = libcairo.la

libcairo_la_SOURCES =				\
	cairo.c					\
	cairo.h					\
	cairo-private.h				\
	cairo-arc.c				\
	cairo-arc-private.h			\
	cairo-array.c				\
	cairo-cache.c				\
	cairo-color.c				\
	cairo-fixed.c				\
	cairo-font.c				\
	cairo-font-options.c			\
	cairo-gstate.c				\
	cairo-gstate-private.h			\
	cairo-hull.c				\
	cairo-image-surface.c			\
	cairo-matrix.c				\
	cairo-path.c				\
	cairo-path-bounds.c			\
	cairo-path-data.c			\
	cairo-path-data-private.h		\
	cairo-path-fill.c			\
	cairo-path-fixed-private.h		\
	cairo-path-stroke.c			\
	cairo-pen.c				\
	cairo-polygon.c				\
	cairo-slope.c				\
	cairo-spline.c				\
	cairo-surface.c				\
	cairo-traps.c				\
	cairo-pattern.c				\
	cairo-unicode.c				\
	cairo-output-stream.c			\
	cairo-wideint.c				\
	cairo-wideint.h				\
	cairo-meta-surface.c			\
	cairo-meta-surface-private.h		\
	$(libcairo_atsui_sources)		\
	$(libcairo_ft_sources)			\
	$(libcairo_ps_sources)			\
	$(libcairo_pdf_sources)			\
	$(libcairo_font_subset_sources)		\
	$(libcairo_png_sources)			\
	$(libcairo_xlib_sources)		\
	$(libcairo_quartz_sources)		\
	$(libcairo_xcb_sources)			\
	$(libcairo_glitz_sources)		\
	$(libcairo_win32_sources)		\
	cairoint.h

libcairo_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

INCLUDES = -I$(srcdir) $(CAIRO_CFLAGS)

libcairo_la_LIBADD = $(CAIRO_LIBS)

install-data-local:
	@if test -f "$(DESTDIR)$(includedir)/cairo.h" || test -f "$(DESTDIR)$(includedir)/cairo-features.h" ; then \
	  echo "****************************************************************" ;	\
	  echo "*** Error: Old headers found. You should remove the following" ;	\
	  echo "***        files and then type 'make install' again." ;			\
	  ls $(includedir)/cairo*.h ;							\
	  echo "****************************************************************" ;	\
	  false ;									\
        fi
