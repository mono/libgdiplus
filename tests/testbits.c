#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_GDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus")
#else
#include <GdiPlusFlat.h>
#endif

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testhelpers.h"

/* FIXME:
 * libgdiplus version is larger than it should be. The library writes
 * to the extended fields, overwriting other data if the non-extended
 * version is used. This causes the failure of this test because other
 * parts of the stack are unexpectedly overwritten.
 */
typedef union {
	BitmapData origBitmapData;
	/* This structure is from src/bitmap-private.h */
	struct {
		unsigned int	width;
		unsigned int	height;
		int		stride;
		int		pixel_format;
		BYTE 		*scan0;
		unsigned int	reserved;
		/* the rest of the structure isn't part of MS GDI+ definition */
		ColorPalette	*palette;
		int		property_count;		/* Number of properties */
		PropertyItem 	*property;		/* Properties associated with image */

		float 		dpi_horz;		/* */
		float 		dpi_vert;		/* */
		ImageFlags	image_flags;		/* Alpha, ColorSpace, etc. */

		unsigned int	left;			/* left display coordinate of frame */
		unsigned int	top;			/* top display coordinate of frame */
		unsigned int	x;			/* LockBits: left coordinate of locked rectangle */
		unsigned int	y;			/* LockBits: top coordinate of locked rectangle */

		int		transparent;		/* Index of transparent color (<24bit only) */
	} extendedBitmapData;
} IncompatibleBitmapData;

static void test_lockBits ()
{
	GpBitmap *bitmap;
	IncompatibleBitmapData d, q;
	Rect r = {0, 0, 10, 10};
	int i, j;
	unsigned char *cptr;

	BYTE *scan0 = (BYTE*) GdipAlloc(10 * 10 * 4);
	GpStatus status = GdipCreateBitmapFromScan0 (10, 10, 10 * 4, PixelFormat32bppARGB, scan0, &bitmap);
	assertEqualInt (status, Ok);

	// Full rectangle, no format conversion, read only.
	status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &d.origBitmapData);
	assertEqualInt (status, Ok);

	// Relock.
	status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &q.origBitmapData);
	assertEqualInt (status, WrongState);

	// Unlock.
	status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
	assertEqualInt (status, Ok);

	// Half rectangle, no format conversion, read only
	memset (&d, 0x00, sizeof (IncompatibleBitmapData));
	status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &d.origBitmapData);
	assertEqualInt (status, Ok);

	for (j = 0; j < 5; j++) {
		cptr = (unsigned char *) d.origBitmapData.Scan0 + j * d.origBitmapData.Stride;
		printf ("%d: ", j);
		for (i = 0; i < 5; i++) {
			printf ("%04x ", *cptr++);
		}
		printf ("\n");
	}

	printf ("Modifying (setting to 0xff)\n");
	memset (d.origBitmapData.Scan0, 0xff, d.origBitmapData.Stride * d.origBitmapData.Height);

	status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
	assertEqualInt (status, Ok);

	// Half rectangle, 32bpp ARGB -> 32bpp RGB, read only
	memset (&d, 0x00, sizeof (IncompatibleBitmapData));
	r.X = 5; r.Y = 5;
	r.Width = 5; r.Height = 5;
	status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppRGB, &d.origBitmapData);
	assertEqualInt (status, Ok);

	for (j = 0; j < 5; j++) {
		cptr = (unsigned char *) d.origBitmapData.Scan0 + j * d.origBitmapData.Stride;
		printf ("%d: ", j);
		for (i = 0; i < 5; i++) {
			printf ("%04x ", *cptr++);
		}
		printf ("\n");
	}

	status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
	assertEqualInt (status, Ok);

	// nHalf rectangle, 32bpp ARGB -> 24bpp RGB, read/write only
	memset (&d, 0x00, sizeof (IncompatibleBitmapData));
	r.X = 5; r.Y = 5;
	r.Width = 5; r.Height = 5;
	status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead | ImageLockModeWrite, PixelFormat24bppRGB, &d.origBitmapData);
	assertEqualInt (status, Ok);

	for (j = 0; j < 5; j++) {
		cptr = ((unsigned char *)d.origBitmapData.Scan0) + (j * d.origBitmapData.Stride);
		printf ("%d: ", j);
		for (i = 0; i < 5; i++) {
			printf ("%02x%02x%02x ", cptr[0], cptr[1], cptr[2]);
			cptr += 3;
		}
		printf ("\n");
	}

	printf ("Modifying (setting to 0xaabbcc)\n");
	for (j = 0; j < 5; j++) {
		cptr = ((unsigned char *)d.origBitmapData.Scan0) + (j * d.origBitmapData.Stride);
		for (i = 0; i < 5; i++) {
			*cptr++ = 0xcc;
			*cptr++ = 0xbb;
			*cptr++ = 0xaa;
		}
	}

	status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
	assertEqualInt (status, Ok);

	GdipFree (scan0);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_unlockBits ()
{
	GpStatus status;
	GpBitmap *bitmap;
	IncompatibleBitmapData data;

	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppARGB, NULL, &bitmap);

	// Negative tests.
	memset (&data, 0x00, sizeof (IncompatibleBitmapData));

	status = GdipBitmapUnlockBits (NULL, &data.origBitmapData);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapUnlockBits (bitmap, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapUnlockBits (bitmap, &data.origBitmapData);
	assertEqualInt (status, Win32Error);

	GdipDisposeImage ((GpImage *) bitmap);
}

int
main (int argc, char **argv)
{
	STARTUP;

	test_lockBits ();
	test_unlockBits ();

	SHUTDOWN;
	return 0;
}
