## Makefile.am for libgdiplus/tests

INCLUDES =					\
	-I$(top_srcdir)				\
	-I$(top_builddir)/src			\
	-I$(top_srcdir)/src			\
	$(GDIPLUS_CFLAGS)

DEPS =						\
	$(top_builddir)/src/libgdiplus.la


LDADDS =					\
	$(top_builddir)/src/libgdiplus.la	\
	$(GDIPLUS_LIBS)

noinst_PROGRAMS =			\
	testgdi testbits

testgdi_DEPENDENCIES = $(TEST_DEPS)
testgdi_LDADD = $(LDADDS)

testgdi_SOURCES =		\
	testgdi.c

testbits_DEPENDENCIES = $(TEST_DEPS)
testbits_LDADD = $(LDADDS)

testbits_SOURCES =		\
	testbits.c

EXTRA_DIST =			\
	$(testgdi_SOURCES)	\
	$(testbits_SOURCES)



