## Makefile.am for libgdiplus/tests

LIBS = $(GDIPLUS_LIBS)

AM_CPPFLAGS =					\
	-I$(top_srcdir)				\
	-I$(top_builddir)/src			\
	-I$(top_srcdir)/src			\
	$(GDIPLUS_CFLAGS)

DEPS =						\
	$(top_builddir)/src/libgdiplus.la


LDADDS =					\
	$(top_builddir)/src/libgdiplus.la \
	-lm

noinst_PROGRAMS =			\
	testadjustablearrowcap \
	testbitmap \
	testbits \
	testbmpcodec \
	testbrush \
	testclip \
	testcodecs \
	testcustomlinecap \
	testemfcodec testfont testgeneral \
	testgifcodec \
	testgpimage \
	testgraphics \
	testgraphicsdraw \
	testgraphicsfill \
	testgraphicspath \
	testhatchbrush \
	testicocodec \
	testimageattributes \
	testjpegcodec testlineargradientbrush \
	testmatrix testmetafile testpathgradientbrush \
	testpen testpng testpngcodec testregion testreversepath testsolidbrush \
	teststringformat \
	testtext \
	testtexturebrush \
	testtiffcodec \
	testwmfcodec

#if HAVE_X11
#noinst_PROGRAMS += testgdi
#
#testgdi_DEPENDENCIES = $(TEST_DEPS)
#testgdi_LDADD = $(LDADDS)
#
#testgdi_SOURCES =		\
#	testgdi.c
#endif HAVE_X11

testadjustablearrowcap_SOURCES =		\
	testadjustablearrowcap.c

testadjustablearrowcap_DEPENDENCIES = $(TEST_DEPS)
testadjustablearrowcap_LDADD = $(LDADDS)

testbitmap_SOURCES =		\
	testbitmap.c

testbitmap_DEPENDENCIES = $(TEST_DEPS)
testbitmap_LDADD = $(LDADDS)

testbits_DEPENDENCIES = $(TEST_DEPS)
testbits_LDADD = $(LDADDS)

testbits_SOURCES =		\
	testbits.c

testbmpcodec_SOURCES =		\
	testbmpcodec.c

testbmpcodec_DEPENDENCIES = $(TEST_DEPS)
testbmpcodec_LDADD = $(LDADDS)

testbrush_SOURCES =		\
	testbrush.c

testbrush_DEPENDENCIES = $(TEST_DEPS)
testbrush_LDADD = $(LDADDS)

testclip_SOURCES =	\
	testclip.c

testclip_DEPENDENCIES = $(TEST_DEPS)
testclip_LDADD = $(LDADDS)

testcodecs_SOURCES =		\
	testcodecs.c

testcodecs_DEPENDENCIES = $(TEST_DEPS)
testcodecs_LDADD = $(LDADDS)

testcustomlinecap_SOURCES =		\
	testcustomlinecap.c

testcustomlinecap_DEPENDENCIES = $(TEST_DEPS)
testcustomlinecap_LDADD = $(LDADDS)

testemfcodec_SOURCES =		\
	testemfcodec.c

testemfcodec_DEPENDENCIES = $(TEST_DEPS)
testemfcodec_LDADD = $(LDADDS)

testfont_DEPENDENCIES = $(TEST_DEPS)
testfont_LDADD = $(LDADDS)

testfont_SOURCES =		\
	testfont.c

testgeneral_SOURCES =		\
	testgeneral.c

testgeneral_DEPENDENCIES = $(TEST_DEPS)
testgeneral_LDADD = $(LDADDS)

testgifcodec_SOURCES =		\
	testgifcodec.c

testgifcodec_DEPENDENCIES = $(TEST_DEPS)
testgifcodec_LDADD = $(LDADDS)

testgpimage_SOURCES =		\
	testgpimage.c

testgpimage_DEPENDENCIES = $(TEST_DEPS)
testgpimage_LDADD = $(LDADDS)

testgraphics_SOURCES =		\
	testgraphics.c

testgraphics_DEPENDENCIES = $(TEST_DEPS)
testgraphics_LDADD = $(LDADDS)

testgraphicsdraw_SOURCES =		\
	testgraphicsdraw.c

testgraphicsdraw_DEPENDENCIES = $(TEST_DEPS)
testgraphicsdraw_LDADD = $(LDADDS)

testgraphicsfill_SOURCES =		\
	testgraphicsfill.c

testgraphicsfill_DEPENDENCIES = $(TEST_DEPS)
testgraphicsfill_LDADD = $(LDADDS)

testgraphicspath_SOURCES =		\
	testgraphicspath.c

testgraphicspath_DEPENDENCIES = $(TEST_DEPS)
testgraphicspath_LDADD = $(LDADDS)

testhatchbrush_SOURCES =		\
	testhatchbrush.c

testhatchbrush_DEPENDENCIES = $(TEST_DEPS)
testhatchbrush_LDADD = $(LDADDS)

testicocodec_SOURCES =		\
	testicocodec.c

testicocodec_DEPENDENCIES = $(TEST_DEPS)
testicocodec_LDADD = $(LDADDS)

testimageattributes_SOURCES =		\
	testimageattributes.c

testimageattributes_DEPENDENCIES = $(TEST_DEPS)
testimageattributes_LDADD = $(LDADDS)

testjpegcodec_SOURCES =		\
	testjpegcodec.c

testjpegcodec_DEPENDENCIES = $(TEST_DEPS)
testjpegcodec_LDADD = $(LDADDS)

testlineargradientbrush_SOURCES =		\
	testlineargradientbrush.c

testlineargradientbrush_DEPENDENCIES = $(TEST_DEPS)
testlineargradientbrush_LDADD = $(LDADDS)

testmatrix_SOURCES =		\
	testmatrix.c

testmatrix_DEPENDENCIES = $(TEST_DEPS)
testmatrix_LDADD = $(LDADDS)

testmetafile_SOURCES =		\
	testmetafile.c

testmetafile_DEPENDENCIES = $(TEST_DEPS)
testmetafile_LDADD = $(LDADDS)

testpngcodec_SOURCES =		\
	testpngcodec.c

testpngcodec_DEPENDENCIES = $(TEST_DEPS)
testpngcodec_LDADD = $(LDADDS)

testregion_SOURCES =		\
	testregion.c

testregion_DEPENDENCIES = $(TEST_DEPS)
testregion_LDADD = $(LDADDS)

testreversepath_SOURCES =	\
	testreversepath.c

testreversepath_DEPENDENCIES = $(TEST_DEPS)
testreversepath_LDADD = $(LDADDS)

testsolidbrush_SOURCES =		\
	testsolidbrush.c

testsolidbrush_DEPENDENCIES = $(TEST_DEPS)
testsolidbrush_LDADD = $(LDADDS)

teststringformat_SOURCES =		\
	teststringformat.c

teststringformat_DEPENDENCIES = $(TEST_DEPS)
teststringformat_LDADD = $(LDADDS)

testtext_SOURCES =		\
	testtext.c

testtext_DEPENDENCIES = $(TEST_DEPS)
testtext_LDADD = $(LDADDS)

testtexturebrush_SOURCES =		\
	testtexturebrush.c

testtexturebrush_DEPENDENCIES = $(TEST_DEPS)
testtexturebrush_LDADD = $(LDADDS)

testpathgradientbrush_SOURCES =		\
	testpathgradientbrush.c

testpathgradientbrush_DEPENDENCIES = $(TEST_DEPS)
testpathgradientbrush_LDADD = $(LDADDS)

testpen_SOURCES =		\
	testpen.c

testpen_DEPENDENCIES = $(TEST_DEPS)
testpen_LDADD = $(LDADDS)

testpng_DEPENDENCIES = $(TEST_DEPS)
testpng_LDADD = $(LDADDS)

testpng_SOURCES =		\
	testpng.c

testtiffcodec_SOURCES =		\
	testtiffcodec.c

testtiffcodec_DEPENDENCIES = $(TEST_DEPS)
testtiffcodec_LDADD = $(LDADDS)

testwmfcodec_SOURCES =		\
	testwmfcodec.c

testwmfcodec_DEPENDENCIES = $(TEST_DEPS)
testwmfcodec_LDADD = $(LDADDS)

EXTRA_DIST =			\
	$(testadjustablearrowcap_SOURCES) \
	$(testbitmap_SOURCES) \
	$(testbits_SOURCES)	\
	$(testbmpcodec_SOURCES)	\
	$(testbrush_SOURCES) \
	$(testclip_SOURCES)	\
	$(test_codecs_SOURCES)	\
	$(testcustomlinecap_SOURCES)	\
	$(testemfcodec_SOURCES)	\
	$(testfont_SOURCES)	\
	$(testgeneral_SOURCES) \
	$(testgifcodec_SOURCES) \
	$(testgpimage_SOURCES) \
	$(testgraphics_SOURCES)	\
	$(testgraphicsdraw_SOURCES)	\
	$(testgraphicsfill_SOURCES)	\
	$(testgraphicspath_SOURCES) \
	$(testhatchbrush_SOURCES)	\
	$(testicocodec_SOURCES) \
	$(testimageattributes_SOURCES) \
	$(testjpegcodec_SOURCES) \
	$(testlineargradientbrush_SOURCES)	\
	$(testmatrix_SOURCES) \
	$(testmetafile_SOURCES)	\
	$(testpathgradientbrush_SOURCES) \
	$(testpen_SOURCES) \
	$(testpng_SOURCES) \
	$(testpngcodec_SOURCES) \
	$(testregion_SOURCES) \
	$(testreversepath_SOURCES) \
	$(testsolidbrush_SOURCES)	\
	$(teststringformat_SOURCES)	\
	$(testtexturebrush_SOURCES)	\
	$(testtext_SOURCES)	\
	$(testtiffcodec_SOURCES) \
	$(testwmfcodec_SOURCES)

#	$(testgdi_SOURCES)

TESTS = \
	testadjustablearrowcap \
	testbitmap \
	testbits \
	testbmpcodec \
	testbrush \
	testclip \
	testcodecs \
	testcustomlinecap \
	testemfcodec \
	testfont \
	testgifcodec \
	testgeneral \
	testgpimage \
	testgraphics \
	testgraphicsdraw \
	testgraphicsfill \
	testgraphicspath \
	testhatchbrush \
	testicocodec \
	testimageattributes \
	testjpegcodec \
	testlineargradientbrush \
	testmatrix \
	testmetafile \
	testpathgradientbrush \
	testpen \
	testpng \
	testpngcodec \
	testregion \
	testreversepath \
	testsolidbrush \
	teststringformat \
	testtexturebrush \
	testtext \
	testtiffcodec \
	testwmfcodec \
	$(NULL)

if HAVE_X11
# TESTS += testgdi
endif

EXTRA_DIST = \
	AdobeRGB1998.icc \
	test-gsa.png \
	test-trns.png \
	test.bmp \
	test.emf \
	test.gif \
	test.ico \
	test.jpg \
	test.otf \
	test.png \
	test.tif \
	test.ttf \
	test.wmf \
	testhelpers.h
