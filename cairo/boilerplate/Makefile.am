EXTRA_LTLIBRARIES = libcairoboilerplate.la

libcairoboilerplate_la_SOURCES =\
cairo-boilerplate.c	\
cairo-boilerplate.h	\
xmalloc.c		\
xmalloc.h
libcairoboilerplate_la_LIBADD = $(top_builddir)/src/libcairo.la

if CAIRO_HAS_BEOS_SURFACE
libcairoboilerplate_la_SOURCES += cairo-test-beos.cpp cairo-test-beos.h
# BeOS system headers trigger this warning
libcairoboilerplate_la_CXXFLAGS = -Wno-multichar
endif

if CAIRO_HAS_DIRECTFB_SURFACE
libcairoboilerplate_la_SOURCES += cairo-test-directfb.c cairo-test-directfb.h
endif

if CAIRO_CAN_TEST_GLITZ_AGL_SURFACE
libcairoboilerplate_la_LIBADD += $(GLITZ_AGL_LIBS)
endif
if CAIRO_CAN_TEST_GLITZ_EGL_SURFACE
libcairoboilerplate_la_LIBADD += $(GLITZ_EGL_LIBS)
endif
if CAIRO_CAN_TEST_GLITZ_GLX_SURFACE
libcairoboilerplate_la_LIBADD += $(GLITZ_GLX_LIBS)
endif
if CAIRO_CAN_TEST_GLITZ_WGL_SURFACE
libcairoboilerplate_la_LIBADD += $(GLITZ_WGL_LIBS)
endif

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

CLEANFILES =					\
	$(EXTRA_LTLIBRARIES)
