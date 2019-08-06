lib_LTLIBRARIES = libgdiplus.la

libgdiplus_la_SOURCES = \
	adjustablearrowcap.c		\
	adjustablearrowcap.h		\
	adjustablearrowcap-private.h	\
	alpha-premul-table.inc		\
	bitmap.c			\
	bitmap.h			\
	bitmap-private.h		\
	brush.c				\
	brush.h				\
	brush-private.h			\
	carbon-private.c		\
	carbon-private.h		\
	codecs.h			\
	codecs-private.h		\
	customlinecap.c			\
	customlinecap.h			\
	customlinecap-private.h		\
	dstream.c 			\
	dstream.h			\
	font.c				\
	font.h				\
	font-private.h			\
	fontcollection.h		\
	fontcollection-private.h	\
	fontfamily.h			\
	fontfamily-private.h		\
	gdi32.h				\
	gdi32.c				\
	GdiPlusFlat.h			\
	gdiplus-private.h		\
	gdip.h				\
	gdipenums.h			\
	gdiplusimaging.h			\
	gdipluspixelformats.h			\
	gdipstructs.h			\
	general.c			\
	general.h			\
	general-private.h		\
	graphics.c			\
	graphics.h			\
	graphics-cairo.c		\
	graphics-cairo-private.h	\
	graphics-metafile.c		\
	graphics-metafile-private.h	\
	graphics-private.h		\
	graphics-path.c			\
	graphics-path.h			\
	graphics-path-private.h		\
	graphics-pathiterator.c		\
	graphics-pathiterator.h		\
	graphics-pathiterator-private.h	\
	hatchbrush.c			\
	hatchbrush.h			\
	hatchbrush-private.h		\
	icocodec.c			\
	icocodec.h			\
	image.c				\
	image.h				\
	image-private.h			\
	imageattributes.c 		\
	imageattributes.h		\
	imageattributes-private.h	\
	lineargradientbrush.c 		\
	lineargradientbrush.h 		\
	lineargradientbrush-private.h	\
	matrix.c			\
	matrix.h			\
	matrix-private.h		\
	metafile.c			\
	metafile.h			\
	metafile-private.h		\
	pathgradientbrush.c 		\
	pathgradientbrush.h 		\
	pathgradientbrush-private.h	\
	pen.c				\
	pen.h				\
	pen-private.h			\
	print.c				\
	region.c			\
	region.h			\
	region-private.h		\
	region-bitmap.c			\
	region-bitmap.h			\
	region-path-tree.c		\
	region-path-tree.h		\
	solidbrush.c			\
	solidbrush.h			\
	solidbrush-private.h		\
	stringformat.c			\
	stringformat.h			\
	stringformat-private.h		\
	text.c				\
	text.h				\
	text-metafile.c			\
	text-metafile-private.h		\
	texturebrush.c			\
	texturebrush.h			\
	texturebrush-private.h		\
	win32structs.h			\
	bmpcodec.h			\
	bmpcodec.c			\
	emfcodec.c			\
	emfcodec.h			\
	emfplus.c			\
	emfplus.h			\
	gifcodec.h			\
	gifcodec.c			\
	jpegcodec.h			\
	jpegcodec.c			\
	pngcodec.h			\
	pngcodec.c			\
	tiffcodec.h			\
	tiffcodec.c			\
	wmfcodec.c			\
	wmfcodec.h

if USE_PANGO_RENDERING
libgdiplus_la_SOURCES += \
	text-pango.c			\
	text-pango-private.h
else
libgdiplus_la_SOURCES += \
	text-cairo.c			\
	text-cairo-private.h
endif

libgdiplus_la_LIBADD = $(GDIPLUS_LIBS)

AM_CPPFLAGS = $(GDIPLUS_CFLAGS)

EXTRA_DIST = libgdiplus.vcxproj
