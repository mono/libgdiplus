if HAVE_GTK
TESTPROGRAMS = \
	composite-test		\
	gradient-test		\
	fetch-test

noinst_PROGRAMS = $(TESTPROGRAMS)

INCLUDES = -I$(top_srcdir)/pixman $(GTK_CFLAGS)

composite_test_LDADD =	$(top_builddir)/pixman/libpixman-1.la $(GTK_LIBS)
gradient_test_LDADD = $(top_builddir)/pixman/libpixman-1.la $(GTK_LIBS)
fetch_test_LDADD = $(top_builddir)/pixman/libpixman-1.la

endif
