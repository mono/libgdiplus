## Makefile.am for libgdiplus/tests

LIBS = $(GDIPLUS_LIBS)

AM_CPPFLAGS =					\
	-I$(top_srcdir)				\
	-I$(top_builddir)/src		\
	-I$(top_srcdir)/src			\
	$(GDIPLUS_CFLAGS)

LDADDS =					\
	$(top_builddir)/src/libgdiplus.la \
	-lm

if HAVE_CMAKE

GTEST_BUILDDIR := $(top_builddir)/gtest_build
GTEST_LIBS := $(GTEST_BUILDDIR)/lib/libgtest.a $(GTEST_BUILDDIR)/lib/libgtest_main.a
$(GTEST_BUILDDIR):
	mkdir -p $(GTEST_BUILDDIR)

$(GTEST_BUILDDIR)/.stamp-gtest: | $(GTEST_BUILDDIR)
	cd $(GTEST_BUILDDIR) && $(CMAKE) . $(top_srcdir)/external/googletest && $(MAKE)
	touch $@

$(GTEST_LIBS): $(GTEST_BUILDDIR)/.stamp-gtest

gtest_SOURCES = \
	testadjustablearrowcap.cpp \
	testbitmap.cpp \
	testbits.cpp \
	testbmpcodec.cpp \
	testbrush.cpp \
	testclip.cpp \
	testcodecs.cpp \
	testcustomlinecap.cpp \
	testemfcodec.cpp \
	testfont.cpp \
	testgeneral.cpp \
	testgifcodec.cpp \
	testgpimage.cpp \
	testgraphics.cpp \
	testgraphicsdraw.cpp \
	testgraphicsfill.cpp \
	testgraphicspath.cpp \
	testhatchbrush.cpp \
	testicocodec.cpp \
	testimageattributes.cpp \
	testlineargradientbrush.cpp \
	testmatrix.cpp \
	testmetafile.cpp \
	testpathgradientbrush.cpp \
	testpen.cpp \
	testpng.cpp \
	testpngcodec.cpp \
	testregion.cpp \
	testreversepath.cpp \
	testsolidbrush.cpp \
	teststringformat.cpp \
	testtexturebrush.cpp \
	testtext \
	testwmfcodec \
	gtest.cpp

if HAVE_LIBJPEG
gtest_SOURCES += testjpegcodec.cpp
endif HAVE_LIBJPEG

if HAVE_LIBTIFF
gtest_SOURCES += testtiffcodecc.pp
endif HAVE_LIBTIFF

gtest_DEPENDENCIES = $(TEST_DEPS) $(GTEST_LIBS)
gtest_LDADD = $(LDADDS) $(GTEST_LIBS)
gtest_CPPFLAGS = -pthread -std=c++11 -I$(top_srcdir)/external/googletest/googletest/include -I$(top_srcdir)/src $(GDIPLUS_INCLUDES)

noinst_PROGRAMS = gtest

run-gtest: gtest
	./gtest --gtest_output=xml:TestResult.xml

endif
