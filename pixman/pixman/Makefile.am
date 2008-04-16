noinst_LTLIBRARIES = libpixman-1.la
libpixman_1_la_LDFLAGS = -version-info $(LT_VERSION_INFO)
libpixman_1_la_LIBADD = @DEP_LIBS@ -lm
libpixman_1_la_SOURCES =		\
	pixman.h			\
	pixman-access.c			\
	pixman-access-accessors.c	\
	pixman-embed.h			\
	pixman-region.c			\
	pixman-private.h		\
	pixman-image.c			\
	pixman-combine.c		\
	pixman-compose.c		\
	pixman-compose-accessors.c	\
	pixman-pict.c			\
	pixman-source.c			\
	pixman-transformed.c		\
	pixman-transformed-accessors.c	\
	pixman-utils.c			\
	pixman-edge.c			\
	pixman-edge-accessors.c		\
	pixman-edge-imp.h		\
	pixman-trap.c			\
	pixman-compute-region.c		\
	pixman-timer.c

#libpixmanincludedir = $(includedir)/pixman-1/
noinst_HEADERS = pixman.h pixman-version.h

EXTRA_DIST = Makefile.win32

# mmx code
if USE_MMX
noinst_LTLIBRARIES += libpixman-mmx.la
libpixman_mmx_la_SOURCES = \
	pixman-mmx.c \
	pixman-mmx.h
libpixman_mmx_la_CFLAGS = $(DEP_CFLAGS) $(MMX_CFLAGS)
libpixman_mmx_la_LIBADD = $(DEP_LIBS)
libpixman_1_la_LIBADD += libpixman-mmx.la
endif


# sse2 code
if USE_SSE2
noinst_LTLIBRARIES += libpixman-sse.la
libpixman_sse_la_SOURCES = \
	pixman-sse.c \
	pixman-sse.h
libpixman_sse_la_CFLAGS = $(DEP_CFLAGS) $(SSE_CFLAGS)
libpixman_sse_la_LIBADD = $(DEP_LIBS)
libpixman_1_la_LIBADD += libpixman-sse.la
endif

