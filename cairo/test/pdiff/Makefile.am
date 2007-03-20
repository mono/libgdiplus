EXTRA_PROGRAMS = perceptualdiff

noinst_LTLIBRARIES = libpdiff.la
libpdiff_la_SOURCES = 		\
	pdiff.h			\
	lpyramid.c		\
	lpyramid.h		\
	pdiff.c

perceptualdiff_SOURCES =	\
	args.c			\
	args.h			\
	perceptualdiff.c

INCLUDES = -I$(top_srcdir)/src -I$(top_builddir)/src $(CAIRO_CFLAGS)
LDADD = libpdiff.la $(top_builddir)/src/libcairo.la
