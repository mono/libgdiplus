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

ATTRIBUTE_USED static void verifyBitmapDataImpl (GpBitmap *bitmap, BYTE *expectedData, INT expectedDataCount, const char *file, const char *function, int line)
{
	GpStatus status;
	PixelFormat format;
	BitmapData data;
	memset (&data, 0, sizeof (data));

	status = GdipGetImagePixelFormat ((GpImage *) bitmap, &format);
	assertEqualIntImpl (status, Ok, "GdipGetImagePixelFormat", file, function, line);

	status = GdipBitmapLockBits (bitmap, NULL, ImageLockModeRead, format, &data);
	assertEqualIntImpl (status, Ok, "GdipBitmapLockBits", file, function, line);

	if (data.Stride * data.Height != expectedDataCount) {
		dumpBytes ((BYTE *) data.Scan0, data.Stride * data.Height);
		assertEqualIntImpl (data.Stride * data.Height, expectedDataCount, "Bitmap Data Count", file, function, line);
	}
	assertEqualBytesImpl ((BYTE *) data.Scan0, expectedData, expectedDataCount, "Bitmap Data", file, function, line);

	status = GdipBitmapUnlockBits (bitmap, &data);
	assertEqualIntImpl (status, Ok, "GdipBitmapUnlockBits", file, function, line);
}

#define verifyBitmapData(bitmap, expectedData) verifyBitmapDataImpl (bitmap, expectedData, sizeof (expectedData), __FILE__, __func__, __LINE__)

static void test_createBitmapFromStream ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// Negative tests.
	status = GdipCreateBitmapFromStream (NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipCreateBitmapFromStream (&temp, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromStream (&temp, &bitmap);
	assertEqualInt (status, NotImplemented);
#endif    
}

static void test_createBitmapFromFile ()
{
	GpStatus status;
	GpBitmap *bitmap;
	WCHAR *bitmapFile = createWchar ("test.png");
	WCHAR *metafileFile = createWchar ("test.wmf");
	WCHAR *enhancedMetafileFile = createWchar ("test.emf");
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	status = GdipCreateBitmapFromFile (bitmapFile, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap ((GpImage *) bitmap, pngRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);
	
	status = GdipCreateBitmapFromFile (metafileFile, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 770, 649, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);
	
	status = GdipCreateBitmapFromFile (enhancedMetafileFile, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 100, 100, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	// Negative tests.
	status = GdipCreateBitmapFromFile (NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromFile (noSuchFile, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromFile (invalidFile, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromFile (noSuchFile, NULL);
	assertEqualInt (status, InvalidParameter);

	freeWchar (bitmapFile);
	freeWchar (metafileFile);
	freeWchar (enhancedMetafileFile);
	freeWchar (noSuchFile);
	freeWchar (invalidFile);
}

static void test_createBitmapFromStreamICM ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// Negative tests.
	status = GdipCreateBitmapFromStreamICM (NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipCreateBitmapFromStreamICM (&temp, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromStreamICM (&temp, &bitmap);
	assertEqualInt (status, NotImplemented);
#endif    
}

static void test_createBitmapFromFileICM ()
{
	GpStatus status;
	GpBitmap *bitmap;
	WCHAR *bitmapFile = createWchar ("test.png");
	WCHAR *metafileFile = createWchar ("test.wmf");
	WCHAR *enhancedMetafileFile = createWchar ("test.emf");
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	status = GdipCreateBitmapFromFileICM (bitmapFile, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap ((GpImage *) bitmap, pngRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);
	
	status = GdipCreateBitmapFromFileICM (metafileFile, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 770, 649, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromFileICM (enhancedMetafileFile, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 100, 100, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	// Negative tests.
	status = GdipCreateBitmapFromFileICM (NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromFileICM (noSuchFile, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromFileICM (invalidFile, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromFileICM (noSuchFile, NULL);
	assertEqualInt (status, InvalidParameter);

	freeWchar (bitmapFile);
	freeWchar (metafileFile);
	freeWchar (enhancedMetafileFile);
	freeWchar (noSuchFile);
	freeWchar (invalidFile);
}

static void test_createBitmapFromScan064bppARGB ()
{
	GpStatus status;
	GpBitmap *bitmap;
	
	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat64bppARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0x00000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat64bppARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0x00000000,
		0x00000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat64bppARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0x00000000, 0x00000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat64bppARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// Scan - 1, 2.
	BYTE bpp64ArgbData[] = {
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80,
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 8, PixelFormat64bppARGB, bpp64ArgbData, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	// It appears that GDI+ ignores the data.
	ARGB bpp64ArgbPixels[] = {
		0x00000000,
		0x00000000,
		0x00000000,
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp64ArgbPixels);
	verifyBitmapData (bitmap, bpp64ArgbData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
}

static void test_createBitmapFromScan064bppPARGB ()
{
	GpStatus status;
	GpBitmap *bitmap;
	
	// No scan - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat64bppPARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0x00000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat64bppPARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0x00000000,
		0x00000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
	
	// No scan - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat64bppPARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0x00000000, 0x00000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
	
	// No scan - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat64bppPARGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// Has scan0 - 1, 2.
	BYTE bpp64PArgbData[] = {
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 8, PixelFormat64bppPARGB, bpp64PArgbData, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	// It appears that GDI+ ignores the data.
	ARGB bpp64PArgbPixels[] = {
		0x00000000,
		0x00000000,
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp64PArgbPixels);
	verifyBitmapData (bitmap, bpp64PArgbData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
}

static void test_createBitmapFromScan048bppRGB ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat48bppRGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 1, 0, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat48bppRGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 2, 0, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat48bppRGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 2, 1, 0, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat48bppRGB, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 5, 3, 0, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// Has scan0 - 1, 2.
	BYTE bpp48RgbData[] = {
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 8, PixelFormat48bppRGB, bpp48RgbData, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	// It appears that GDI+ ignores the data.
	ARGB bpp48RgbPixels[] = {
		0xFF000000,
		0xFF000000
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp48RgbPixels);
	verifyBitmapData (bitmap, bpp48RgbData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
}

static void test_createBitmapFromScan032bppARGB ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0x00000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0x00000000,
		0x00000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one width.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0x00000000, 0x00000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - 1, 2.
	BYTE bpp32ArgbData[] = {
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0x80
	};
	ARGB bpp32ArgbPixels[] = {
		0xFF0000FF,
		0x8000FF00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppARGB, bpp32ArgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32ArgbPixels);
	verifyBitmapData (bitmap, bpp32ArgbData);
	
	// Holds a reference to the original data.
	ARGB bpp32ArgbPixelsModified[] = {
		0xFF000000,
		0x8000FF00
	};
	bpp32ArgbData[0] = 0x00;
	verifyPixels (bitmap, bpp32ArgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan032bppPARGB ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat32bppPARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0x00000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppPARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0x00000000,
		0x00000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one width.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat32bppPARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0x00000000, 0x00000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppPARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - 1, 2.
	BYTE bpp32PArgbData[] = {
		0xFF, 0xFF, 0xFF, 0xFF,
		0x2A, 0x33, 0x3A, 0x4E
	};
	ARGB bpp32PArgbPixels[] = {
		0xFFFFFFFF,
		0x4EBDA689
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppPARGB, bpp32PArgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32PArgbPixels);
	verifyBitmapData (bitmap, bpp32PArgbData);
	
	// Holds a reference to the original data.
	ARGB bpp32PArgbPixelsModified[] = {
		0xFFFFFF00,
		0x4EBDA689,
		0x00FF0000
	};
	bpp32PArgbData[0] = 0x00;
	verifyPixels (bitmap, bpp32PArgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan032bppRGB ()
{
	GpStatus status;
	GpBitmap *bitmap;
	
	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppRGB, 1, 1, 0, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
	// FIXME: should all be 0: https://github.com/mono/libgdiplus/issues/448.
	BYTE bpp32alpha = WINDOWS_GDIPLUS ? 0x00 : 0xFF;
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, bpp32alpha
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppRGB, 1, 2, 0, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, bpp32alpha,
		0x00, 0x00, 0x00, bpp32alpha
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one width.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppRGB, 2, 1, 0, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppRGB, 5, 3, 0, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha,
		0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha,
		0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - 1, 2.
	BYTE bpp32RgbData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00
	};
	ARGB bpp32RgbPixels[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFFFF0000
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp32RgbPixels);
	verifyBitmapData (bitmap, bpp32RgbData);
	
	// Holds a reference to the original data.
	ARGB bpp32RgbPixelsModified[] = {
		0xFF000000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp32RgbData[0] = 0x00;
	verifyPixels (bitmap, bpp32RgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan032bppCMYK ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat32bppCMYK, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 1, 0, 0, TRUE);
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppCMYK, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 2, 0, 0, TRUE);
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat32bppCMYK, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 2, 1, 0, 0, TRUE);
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppCMYK, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 5, 3, 0, 0, TRUE);
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
	
	// Has scan0 - 1, 2.
	BYTE bpp32CMYKData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppCMYK, bpp32CMYKData, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 2, 0, 0, TRUE);
	verifyBitmapData (bitmap, bpp32CMYKData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
}

static void test_createBitmapFromScan024bppRGB ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat24bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 1, 1, 0, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
#endif
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, oneWidthOneHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat24bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 1, 2, 0, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
#endif
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat24bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 2, 1, 0, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
#endif
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, largeWidthOneHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat24bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 5, 3, 0, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
#endif
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - 1, 2.
	BYTE bpp24RgbData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00
	};
	ARGB bpp24RgbPixels[] = {
		0xFF0000FF,
		0xFF00FF00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat24bppRGB, bpp24RgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp24RgbPixels);
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, bpp24RgbData);
#endif
	
	// Holds a reference to the original data.
	ARGB bpp24RgbPixelsModified[] = {
		0xFF000000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp24RgbData[0] = 0x00;
	verifyPixels (bitmap, bpp24RgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan016bppRGB555 ()
{
	GpStatus status;
	GpBitmap *bitmap;
	
	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat16bppRGB555, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 1, 1, 0, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
#endif
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, oneWidthOneHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppRGB555, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 1, 2, 0, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	};
#endif
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat16bppRGB555, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 2, 1, 0, 0, TRUE);
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - large width, large height.
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppRGB555, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 5, 3, 0, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// Has scan0 - 1, 2.
	BYTE bpp16rgb555Data[] = {
		0x1F, 0x00, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00
	};
	ARGB bpp16rgb555Pixels[] = {
		0xFF0000FF,
		0xFF00FF00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppRGB555, bpp16rgb555Data, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp16rgb555Pixels);
	verifyBitmapData (bitmap, bpp16rgb555Data);

	// Holds a reference to the original data.
	ARGB bpp16rgb555PixelsModified[] = {
		0xFF0000FF,
		0xFF003900
	};
	bpp16rgb555Data[5] = 0x00;
	verifyPixels (bitmap, bpp16rgb555PixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan016bppRGB565 ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat16bppRGB565, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB565, 1, 1, 0, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
#endif
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, oneWidthOneHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppRGB565, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB565, 1, 2, 0, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	};
#endif
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat16bppRGB565, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB565, 2, 1, 0, 0, TRUE);
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - large width, large height.
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppRGB565, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB565, 5, 3, 0, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// Has scan0 - 1, 2.
	BYTE bpp16rgb565Data[] = {
		0x1F, 0x00, 0x00, 0x00,
		0xE0, 0x07, 0x00, 0x00
	};
	ARGB bpp16rgb565Pixels[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFFFF0000
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppRGB565, bpp16rgb565Data, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB565, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp16rgb565Pixels);
	verifyBitmapData (bitmap, bpp16rgb565Data);
	
	// Holds a reference to the original data.
	ARGB bpp16rgb565PixelsModified[] = {
		0xFF0000FF,
		0xFF001C00
	};
	bpp16rgb565Data[5] = 0x00;
	verifyPixels (bitmap, bpp16rgb565PixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan016bppARGB1555 ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat16bppARGB1555, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0x00000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppARGB1555, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0x00000000,
		0x00000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
	
	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat16bppARGB1555, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0x00000000, 0x00000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
	
	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppARGB1555, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0x0000000, 0x0000000, 0x0000000, 0x00000000, 0x00000000,
		0x0000000, 0x0000000, 0x0000000, 0x00000000, 0x00000000,
		0x0000000, 0x0000000, 0x0000000, 0x00000000, 0x00000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// Has scan0 - 1, 2.
	BYTE bpp16argb1555Data[] = {
		0x1F, 0x80, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppARGB1555, bpp16argb1555Data, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	ARGB bpp16argb555Pixels[] = {
		0xFF0000FF,
		0x0000FF00
	};
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp16argb555Pixels);
	verifyBitmapData (bitmap, bpp16argb1555Data);

	// Holds a reference to the original data.
	ARGB bpp16argb1555PixelsModified[] = {
		0xFF0000FF,
		0x00003900
	};
	bpp16argb1555Data[5] = 0x00;
	verifyPixels (bitmap, bpp16argb1555PixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
}

static void test_createBitmapFromScan016bppGrayScale ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat16bppGrayScale, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 1, 0, 0, TRUE);
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - one wdith, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppGrayScale, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 2, 0, 0, TRUE);
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large wdith, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat16bppGrayScale, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 2, 1, 0, 0, TRUE);
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif

	// No scan0 - large wdith, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppGrayScale, NULL, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 5, 3, 0, 0, TRUE);
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
	
	// Has scan0 - 1, 2.
	BYTE bpp16grayscaleData[] = {
		0x1F, 0x80, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppGrayScale, bpp16grayscaleData, &bitmap);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 2, 0, 0, TRUE);
	verifyBitmapData (bitmap, bpp16grayscaleData);
	GdipDisposeImage ((GpImage *) bitmap);
#endif
}

static void test_createBitmapFromScan08bppIndexed ()
{
	GpStatus status;
	GpBitmap *bitmap;
	
	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat8bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat8bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat8bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat8bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - 1, 2.
	BYTE bpp8indexedData[] = {
		0x0C, 0x00, 0x00, 0x00,
		0x0A, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat8bppIndexed, bpp8indexedData, &bitmap);
	ARGB bpp8indexedPixels[] = {
		0xFF0000FF,
		0xFF00FF00
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp8indexedPixels);
	verifyBitmapData (bitmap, bpp8indexedData);
	
	// Holds a reference to the original data.
	ARGB bpp8indexedPixelsModified[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp8indexedData[0] = 0x09;
	verifyPixels (bitmap, bpp8indexedPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan04bppIndexed ()
{
	GpStatus status;
	GpBitmap *bitmap;
	
	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat4bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat4bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat4bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat4bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - 1, 2.
	BYTE bpp4indexedData[] = {
		0xC0, 0x00, 0x00, 0x00,
		0xA0, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat4bppIndexed, bpp4indexedData, &bitmap);
	ARGB bpp4indexedPixels[] = {
		0xFF0000FF,
		0xFF00FF00
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp4indexedPixels);
	
	// Holds a reference to the original data.
	ARGB bpp4indexedPixelsModified[] = {
		0xFFFF0000,
		0xFF00FF00
	};
	bpp4indexedData[0] = 0x90;
	verifyPixels (bitmap, bpp4indexedPixelsModified);
	verifyBitmapData (bitmap, bpp4indexedData);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan01bppIndexed ()
{
	GpStatus status;
	GpBitmap *bitmap;

	// No scan0 - one width, one height.
	status = GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthOneHeightPixels[] = {
		0xFF000000
	};
	BYTE oneWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthOneHeightPixels);
	verifyBitmapData (bitmap, oneWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - one width, large height.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	ARGB oneWidthLargeHeightPixels[] = {
		0xFF000000,
		0xFF000000
	};
	BYTE oneWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, oneWidthLargeHeightPixels);
	verifyBitmapData (bitmap, oneWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, one height.
	status = GdipCreateBitmapFromScan0 (2, 1, 0, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 2, 1, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthOneHeightPixels[] = {
		0xFF000000, 0xFF000000
	};
	BYTE largeWidthOneHeightData[] = {
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthOneHeightPixels);
	verifyBitmapData (bitmap, largeWidthOneHeightData);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - large width, large height.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 5, 3, ImageFlagsHasAlpha, 0, TRUE);
	ARGB largeWidthLargeHeightPixels[] = {
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
	};
	BYTE largeWidthLargeHeightData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyPixels (bitmap, largeWidthLargeHeightPixels);
	verifyBitmapData (bitmap, largeWidthLargeHeightData);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - 1, 2.
	BYTE bpp1indexedData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x80, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat1bppIndexed, bpp1indexedData, &bitmap);
	ARGB bpp1indexedPixels[] = {
		0xFF000000,
		0xFFFFFFFF
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp1indexedPixels);
	verifyBitmapData (bitmap, bpp1indexedData);

	// Holds a reference to the original data.
	ARGB bpp1indexedPixelsModified[] = {
		0xFF000000,
		0xFF000000
	};
	bpp1indexedData[4] = 0x00;
	verifyPixels (bitmap, bpp1indexedPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_createBitmapFromScan0 ()
{
	GpStatus status;
	GpBitmap *bitmap;
	BYTE bpp32RgbData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00
	};

	// Negative tests.
	status = GdipCreateBitmapFromScan0 (0, 2, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (-1, 2, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 0, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, -1, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromScan0 (1, 2, -1, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 3, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatIndexed, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatGDI, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatAlpha, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatPAlpha, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatExtended, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatCanonical, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatUndefined, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromScan0 (1, INT_MAX, 4, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateBitmapFromScan0 (1, 2, INT_MAX, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateBitmapFromScan0 (1, INT_MAX, INT_MAX, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat1bppIndexed, NULL, NULL);
	
	status = GdipCreateBitmapFromScan0 (1, 2, 0, -10, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);
}

static void test_createBitmapFromGraphics ()
{
	GpStatus status;
	GpBitmap *source;
	GpBitmap *sourceWithResolution;
	GpGraphics *graphics;
	GpGraphics *graphicsWithResolution;
	GpBitmap *bitmap;
	GpGraphics *bitmapGraphics;
	REAL dpiX;
	REAL dpiY;

	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat24bppRGB, NULL, &source);
	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat24bppRGB, NULL, &sourceWithResolution);
	GdipBitmapSetResolution (sourceWithResolution, 100, 124);

	GdipGetImageGraphicsContext ((GpImage *) source, &graphics);
	GdipGetImageGraphicsContext ((GpImage *) sourceWithResolution, &graphicsWithResolution);

	status = GdipCreateBitmapFromGraphics (24, 32, graphics, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 24, 32, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromGraphics (24, 32, graphicsWithResolution, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 24, 32, ImageFlagsHasAlpha, 0, TRUE);
	
	// Verify the DPI.
	GdipGetImageGraphicsContext ((GpImage *) bitmap, &bitmapGraphics);
	GdipGetDpiX (bitmapGraphics, &dpiX);
	GdipGetDpiY (bitmapGraphics, &dpiY);
	assertEqualFloat (dpiX, 100);
	assertEqualFloat (dpiY, 124);

	GdipDisposeImage ((GpImage *) bitmap);
	GdipDeleteGraphics (bitmapGraphics);

	// Negative tests.
	status = GdipCreateBitmapFromGraphics (0, 32, graphics, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromGraphics (-1, 32, graphics, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromGraphics (24, 0, graphics, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromGraphics (24, -1, graphics, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromGraphics (24, 32, NULL, &bitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateBitmapFromGraphics (24, 32, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage ((GpImage *) source);
	GdipDisposeImage ((GpImage *) sourceWithResolution);
	GdipDeleteGraphics (graphics);
	GdipDeleteGraphics (graphicsWithResolution);
}

static void test_bitmapLockBits64bppARGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26, 0x56, 0xC3, 0x00, 0xEB, 0xE8, 0x70, 0xDD, 0x50, 0x60, 0x3B, 0xB0, 0xA7,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6, 0x64, 0x40, 0xDA, 0x1E, 0xD4, 0xDF, 0xBD, 0xBB, 0x9B, 0xDC, 0xFA, 0x34,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E, 0x6D, 0x2C, 0xCA, 0x1E, 0xE5, 0xF1, 0x08, 0xD6, 0xC7, 0x82, 0xD0, 0x09
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 20, PixelFormat64bppARGB, scan0, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat64bppARGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat64bppARGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 20);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapLockBits64bppPARGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26, 0x56, 0xC3, 0x00, 0xEB, 0xE8, 0x70, 0xDD, 0x50, 0x60, 0x3B, 0xB0, 0xA7,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6, 0x64, 0x40, 0xDA, 0x1E, 0xD4, 0xDF, 0xBD, 0xBB, 0x9B, 0xDC, 0xFA, 0x34,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E, 0x6D, 0x2C, 0xCA, 0x1E, 0xE5, 0xF1, 0x08, 0xD6, 0xC7, 0x82, 0xD0, 0x09
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 20, PixelFormat64bppPARGB, scan0, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat64bppPARGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat64bppPARGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 20);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapLockBits48bppRGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26, 0x56, 0xC3, 0x00, 0xEB, 0xE8, 0x70, 0xDD, 0x50,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6, 0x64, 0x40, 0xDA, 0x1E, 0xD4, 0xDF, 0xBD, 0xBB,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E, 0x6D, 0x2C, 0xCA, 0x1E, 0xE5, 0xF1, 0x08, 0xD6
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 20, PixelFormat48bppRGB, scan0, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat48bppRGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat48bppRGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 20);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapLockBits32PARGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26, 0x56, 0xC3, 0x00, 0xEB,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6, 0x64, 0x40, 0xDA, 0x1E,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E, 0x6D, 0x2C, 0xCA, 0x1E
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));

	status = GdipCreateBitmapFromScan0 (3, 4, 16, PixelFormat32bppPARGB, scan0, &image);
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppPARGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat32bppPARGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 16);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits32bppARGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26, 0x56, 0xC3, 0x00, 0xEB,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6, 0x64, 0x40, 0xDA, 0x1E,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E, 0x6D, 0x2C, 0xCA, 0x1E
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 16, PixelFormat32bppARGB, scan0, &image);
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat32bppARGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 16);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits32bppRGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26, 0x56, 0xC3, 0x00, 0xEB,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6, 0x64, 0x40, 0xDA, 0x1E,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E, 0x6D, 0x2C, 0xCA, 0x1E
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 16, PixelFormat32bppRGB, scan0, &image);
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat32bppRGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 16);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits24bppRGB ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93, 0x01, 0x75, 0xE2, 0x26,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85, 0xA1, 0x75, 0xE8, 0xF6,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77, 0x18, 0x14, 0xDF, 0x9E
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 12, PixelFormat24bppRGB, scan0, &image);
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat24bppRGB, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat24bppRGB, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 12);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 3328);
	assert (data.Scan0);
	// Cairo uses 32bpp to represent 24bpp images so this is read as 32bpp.
	// https://github.com/mono/libgdiplus/issues/448.
#if defined(USE_WINDOWS_GDIPLUS)
	assert (data.Scan0 == scan0);
#endif
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));
#endif

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits16bppRGB555 ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 8, PixelFormat16bppRGB555, scan0, &image);
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat16bppRGB555, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat16bppRGB555, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 8);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits16bppRGB565 ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 8, PixelFormat16bppRGB565, scan0, &image);
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat16bppRGB565, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat16bppRGB565, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 8);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits16bppARGB1555 ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 8, PixelFormat16bppARGB1555, scan0, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat16bppARGB1555, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat16bppARGB1555, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 8);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapLockBits16bppGrayScale ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8, 0x28, 0x78, 0xE8, 0x93,
		0x15, 0x9E, 0xD6, 0xAA, 0xD4, 0x24, 0x6C, 0x85,
		0x08, 0x6A, 0x5A, 0xE8, 0x65, 0xB5, 0x8C, 0x77
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	status = GdipCreateBitmapFromScan0 (3, 4, 8, PixelFormat16bppGrayScale, scan0, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat16bppGrayScale, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat16bppGrayScale, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 8);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapLockBits8bppIndexed ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8,
		0x15, 0x9E, 0xD6, 0xAA,
		0x08, 0x6A, 0x5A, 0xE8
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	GdipCreateBitmapFromScan0 (3, 4, 4, PixelFormat8bppIndexed, scan0, &image);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat8bppIndexed, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 4);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits4bppIndexed ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8,
		0x15, 0x9E, 0xD6, 0xAA,
		0x08, 0x6A, 0x5A, 0xE8
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	GdipCreateBitmapFromScan0 (3, 4, 4, PixelFormat4bppIndexed, scan0, &image);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat4bppIndexed, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat4bppIndexed, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 4);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapLockBits1bppIndexed ()
{
	GpBitmap *image;
	GpStatus status;
	BYTE scan0[] = {
		0xFE, 0x96, 0x37, 0xE8,
		0x15, 0x9E, 0xD6, 0xAA,
		0x08, 0x6A, 0x5A, 0xE8
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	GdipCreateBitmapFromScan0 (3, 4, 4, PixelFormat1bppIndexed, scan0, &image);

	// No rect.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat1bppIndexed, &data);
	assertEqualInt (status, Ok);

	assertEqualInt (PixelFormat1bppIndexed, data.PixelFormat);
	assertEqualInt (data.Width, 3);
	assertEqualInt (data.Height, 4);
	assertEqualInt (data.Stride, 4);
	assertEqualInt ((int) data.Reserved, WINDOWS_GDIPLUS ? 0 : 1024);
	assert (data.Scan0);
	assert (data.Scan0 == scan0);
	assertEqualBytes ((BYTE *) data.Scan0, scan0, sizeof (scan0));

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel64bppARGB ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat64bppARGB, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0x10DF37F1);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0x00A1A2A3);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0x7E000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0x7EFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);
	
	BYTE expectedData64bppARGB[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x26, 0x1C, 0x39, 0x01, 0x9D, 0x17, 0x22, 0x02, 0xB8, 0x0B, 0x90, 0x0B, 0x68, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF, 0x0F, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0xEF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20
	};
	verifyBitmapData (image, expectedData64bppARGB);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapSetPixel64bppPARGB ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat64bppPARGB, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	
	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0x10DF36F1);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0x00000000);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0x7E000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0x7EFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);
	
	BYTE expectedData64bppPARGB[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xE0, 0x01, 0x14, 0x00, 0x92, 0x01, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF, 0x0F, 0xEF, 0x0F, 0xEF, 0x0F, 0xEF, 0x0F, 0xEF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20
	};
	verifyBitmapData (image, expectedData64bppPARGB);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapSetPixel48bppRGB ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif
	
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat48bppRGB, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	
	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFDF37F1);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFA1A2A3);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);
	
	BYTE expectedData48bppRGB[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x26, 0x1C, 0x39, 0x01, 0x9D, 0x17, 0xB8, 0x0B, 0x90, 0x0B, 0x68, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData48bppRGB);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapSetPixel32bppARGB ()
{
	GpStatus status;
	GpBitmap *image;
	ARGB pixel;

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppARGB, NULL, &image);
	assertEqualInt (status, Ok);
	
	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0x11DF37F1);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0x00A1A2A3);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0x7F000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0x7FFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);
	
	BYTE expectedData32bppARGB[] = {
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xF1, 0x37, 0xDF, 0x11, 0xA3, 0xA2, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF
	};
	verifyBitmapData (image, expectedData32bppARGB);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel32bppPARGB ()
{
	GpStatus status;
	GpBitmap *image;
	ARGB pixel;

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppPARGB, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0x11E13CF0);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualARGB (pixel, 0x00000000);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0x7F000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0x7FFEFEFE);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);

	BYTE expectedData32bppPARGB[] = {
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x04, 0x0F, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF
	};
	verifyBitmapData (image, expectedData32bppPARGB);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel32bppRGB ()
{
	GpStatus status;
	GpBitmap *image;
	ARGB pixel;

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppRGB, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFDF37F1);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualARGB (pixel, 0xFFA1A2A3);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);
	
	// FIXME: should all be 0: https://github.com/mono/libgdiplus/issues/448.
	BYTE bpp32alpha = WINDOWS_GDIPLUS ? 0x00 : 0xFF;
	BYTE expectedData32bppRGB[] = {
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha,
		0xF1, 0x37, 0xDF, WINDOWS_GDIPLUS ? 0x11 : 0xFF, 0xA3, 0xA2, 0xA1, bpp32alpha, 0x00, 0x00, 0x00, WINDOWS_GDIPLUS ? 0x7F : 0xFF, 0xFF, 0xFF, 0xFF, WINDOWS_GDIPLUS ? 0x7F : 0xFF, 0x00, 0x00, 0x00, bpp32alpha,
		0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0x00, 0x00, 0x00, bpp32alpha, 0xFF, 0x00, 0x00, 0xFF
	};
	verifyBitmapData (image, expectedData32bppRGB);

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel24bppRGB ()
{
	GpStatus status;
	GpBitmap *image;
	ARGB pixel;

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat24bppRGB, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFDF37F1);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFA1A2A3);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);
	
	// FIXME: reads uninitialized memory at the end: https://github.com/mono/libgdiplus/issues/451
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE expectedData24bppRGB[] = {
		0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xF1, 0x37, 0xDF, 0xA3, 0xA2, 0xA1, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData24bppRGB);
#endif

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel16bppRGB555 ()
{
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	GpStatus status;
	GpBitmap *image;
	ARGB pixel;

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppRGB555, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFDE31F7);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFA5A5A5);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);

	BYTE expectedData16bppRGB555[] = {
		0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xDE, 0x6C, 0x94, 0x52, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData16bppRGB555);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapSetPixel16bppRGB565 ()
{
	// FIXME: 16bpp scans are read as 32bpp: https://github.com/mono/libgdiplus/issues/461
#if defined(USE_WINDOWS_GDIPLUS)
	GpStatus status;
	GpBitmap *image;
	ARGB pixel;

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppRGB565, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFDE34F7);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFA5A2A5);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);

	BYTE expectedData16bppRGB565[] = {
		0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xBE, 0xD9, 0x14, 0xA5, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData16bppRGB565);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapSetPixel16bppARGB1555 ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppARGB1555, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0x00DE31F7);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0x00A5A5A5);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0x00000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0x00FFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);

	BYTE expectedData16bppARGB1555[] = {
		0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xDE, 0x6C, 0x94, 0x52, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData16bppARGB1555);

	GdipDisposeImage ((GpImage *) image);
#endif
}

static void test_bitmapSetPixel8bppIndexed ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat8bppIndexed, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	// FIXME: indexed formats not supported: https://github.com/mono/libgdiplus/issues/462
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, InvalidParameter);
#else
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFCC33FF);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFF999999);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);

	BYTE expectedData8bppIndexed[] = {
		0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xC3, 0xA9, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData8bppIndexed);
#endif

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel4bppIndexed ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat4bppIndexed, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	// FIXME: indexed formats not supported: https://github.com/mono/libgdiplus/issues/462
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, InvalidParameter);
#else
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFFFF0000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFF00FF);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFC0C0C0);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF0000FF);

	BYTE expectedData4bppIndexed[] = {
		0x90, 0x00, 0x00, 0x00,
		0xD8, 0x0F, 0x00, 0x00,
		0x00, 0x00, 0xC0, 0x00
	};
	verifyBitmapData (image, expectedData4bppIndexed);
#endif

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel1bppIndexed ()
{
	GpStatus status;
	GpBitmap *image;
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB pixel;
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat1bppIndexed, NULL, &image);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	// FIXME: indexed formats not supported: https://github.com/mono/libgdiplus/issues/462
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, InvalidParameter);
#else
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 0, 1, 0x11DF37F1);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 0, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 1, 1, 0x00A1A2A3);
	assertEqualInt (status, Ok);
	status = GdipBitmapGetPixel (image, 1, 1, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 2, 1, 0x7F000000);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 2, 1, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapSetPixel (image, 3, 1, 0x7FFFFFFF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 3, 1, &pixel);
	assertEqualARGB (pixel, 0xFFFFFFFF);

	status = GdipBitmapSetPixel (image, 4, 2, 0xFF0000FF);
	assertEqualInt (status, Ok);
	GdipBitmapGetPixel (image, 4, 2, &pixel);
	assertEqualARGB (pixel, 0xFF000000);

	BYTE expectedData1bppIndexed[] = {
		0x00, 0x00, 0x00, 0x00,
		0xD0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	verifyBitmapData (image, expectedData1bppIndexed);
#endif

	GdipDisposeImage ((GpImage *) image);
}

static void test_bitmapSetPixel ()
{
	GpStatus status;
	GpBitmap *image;
	GpBitmap *indexedImage;
	
	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppRGB, NULL, &image);
	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat8bppIndexed, NULL, &indexedImage);

	// Negative tests.
	status = GdipBitmapSetPixel (NULL, 0, 0, 0xFF000000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (image, -1, 1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (indexedImage, -1, 1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (image, 5, 1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (indexedImage, 5, 1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (image, 6, 1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (indexedImage, 6, 1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (image, 1, -1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (indexedImage, 1, -1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (image, 1, 3, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (indexedImage, 1, 3, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (image, 1, 4, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapSetPixel (indexedImage, 1, 4, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	// Locked.
	BitmapData data;
	memset (&data, 0, sizeof (data));

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, WrongState);

	// FIXME: indexed formats not supported: https://github.com/mono/libgdiplus/issues/462
	status = GdipBitmapSetPixel (indexedImage, 0, 0, 0xFFFF0000);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, InvalidParameter);
#else
	assertEqualInt (status, Ok);

	status = GdipBitmapGetPixel (indexedImage, 0, 0, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFFFF0000);
#endif

	status = GdipBitmapSetPixel (image, -1, 0, 0xFFFF0000);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapSetPixel (indexedImage, -1, 0, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapSetPixel (image, 5, 0, 0xFFFF0000);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapSetPixel (indexedImage, 5, 0, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapSetPixel (image,  6, 0, 0xFFFF0000);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapSetPixel (indexedImage,  6, 0, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapSetPixel (image, 0, -1, 0xFFFF0000);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapSetPixel (indexedImage, 0, -1, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapSetPixel (image, 0, 3, 0xFFFF0000);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapSetPixel (indexedImage, 0, 3, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapSetPixel (image, 0, 4, 0xFFFF0000);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapSetPixel (indexedImage, 0, 4, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	GdipBitmapUnlockBits (image, &data);
	GdipDisposeImage ((GpImage *) image);

	// Invalid type.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppCMYK, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage ((GpImage *) image);
#endif
	
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppGrayScale, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage ((GpImage *) image);
#endif
	
	GdipDisposeImage ((GpImage *) indexedImage);
}

static void test_bitmapGetPixel ()
{
	GpStatus status;
	GpBitmap *image;
	GpBitmap *indexedImage;
	ARGB pixel;

	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppRGB, NULL, &image);
	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat8bppIndexed, NULL, &indexedImage);

	// Negative tests.
	status = GdipBitmapGetPixel (NULL, 0, 0, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (image, -1, 1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, -1, 1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (image, 5, 1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, 5, 1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (image, 6, 1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, 6, 1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (image, 1, -1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, 1, -1, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (image, 1, 3, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, 1, 3, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (image, 1, 4, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, 1, 4, &pixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapGetPixel (indexedImage, 1, 4, &pixel);
	assertEqualInt (status, InvalidParameter);

	// Locked.
	BitmapData data;
	memset (&data, 0, sizeof (data));

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, Ok);

	status = GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualInt (status, WrongState);

	pixel = 0xCCCCCCCC;
	status = GdipBitmapGetPixel (indexedImage, 0, 0, &pixel);
	assertEqualInt (status, Ok);
	assertEqualARGB (pixel, 0xFF000000);
	
	status = GdipBitmapGetPixel (image, -1, 0, &pixel);
	assertEqualInt (status, WrongState);

	status = GdipBitmapGetPixel (indexedImage, -1, 0, &pixel);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapGetPixel (image, 5, 0, &pixel);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapGetPixel (indexedImage, 5, 0, &pixel);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapGetPixel (image,  6, 0, &pixel);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapGetPixel (indexedImage,  6, 0, &pixel);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapGetPixel (image, 0, -1, &pixel);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapGetPixel (indexedImage, 0, -1, &pixel);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapGetPixel (image, 0, 3, &pixel);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapGetPixel (indexedImage, 0, 3, &pixel);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapGetPixel (image, 0, 4, &pixel);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapGetPixel (indexedImage, 0, 4, &pixel);
	assertEqualInt (status, InvalidParameter);

	GdipBitmapUnlockBits (image, &data);
	GdipDisposeImage ((GpImage *) image);

	// Invalid type.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppCMYK, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);

	status = GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage ((GpImage *) image);
#endif
	
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppGrayScale, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, NotImplemented);
#else
	assertEqualInt (status, Ok);
	
	status = GdipBitmapGetPixel (image, 0, 0, &pixel);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage ((GpImage *) image);
#endif

	GdipDisposeImage ((GpImage *) indexedImage);
}

static void test_bitmapLockBits ()
{
	GpStatus status;
	GpBitmap *image;
	GpBitmap *indexedImage;
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	GdipCreateBitmapFromScan0 (3, 4, 0, PixelFormat32bppRGB, NULL, &image);
	GdipCreateBitmapFromScan0 (3, 4, 0, PixelFormat32bppRGB, NULL, &indexedImage);

	// Negative tests.
	status = GdipBitmapLockBits (NULL, NULL, 0, PixelFormat32bppARGB, &data);
	assertEqualInt (status, InvalidParameter);

	Rect negativeX = {-1, 0, 3, 4};
	status = GdipBitmapLockBits (image, &negativeX, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &negativeX, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect negativeY = {0, -1, 3, 4};
	status = GdipBitmapLockBits (image, &negativeY, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &negativeY, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect negativeWidth = {0, 0, -1, 4};
	status = GdipBitmapLockBits (image, &negativeWidth, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &negativeWidth, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect zeroWidth = {0, 0, 0, 4};
	status = GdipBitmapLockBits (image, &zeroWidth, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &zeroWidth, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect largeWidth = {0, 0, 4, 4};
	status = GdipBitmapLockBits (image, &largeWidth, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &largeWidth, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect negativeHeight = {0, 0, 3, -1};
	status = GdipBitmapLockBits (image, &negativeHeight, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &negativeHeight, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect zeroHeight = {0, 0, 3, 0};
	status = GdipBitmapLockBits (image, &zeroHeight, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &zeroHeight, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect largeHeight = {0, 0, 3, 5};
	status = GdipBitmapLockBits (image, &largeHeight, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &largeHeight, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppCMYK, &data);
	assertEqualInt (status, InvalidParameter);

	Rect invalidXWidth = {2, 0, 2, 4};
	status = GdipBitmapLockBits (image, &invalidXWidth, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &invalidXWidth, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	Rect invalidYHeight = {0, 2, 3, 3};
	status = GdipBitmapLockBits (image, &invalidYHeight, 0, PixelFormat32bppRGB, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (indexedImage, &invalidYHeight, 0, PixelFormat8bppIndexed, &data);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppCMYK, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatIndexed, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatGDI, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatAlpha, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatPAlpha, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatExtended, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatCanonical, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatUndefined, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, -10, &data);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, NULL);
	assertEqualInt (status, InvalidParameter);
	
	// Locked.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, &data);
	assertEqualInt (status, Ok);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, &data);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppCMYK, &data);
	assertEqualInt (status, WrongState);
	
	status = GdipBitmapLockBits (image, NULL, 0, -10, &data);
	assertEqualInt (status, WrongState);

	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Ok);

	GdipDisposeImage ((GpImage *) image);
	GdipDisposeImage ((GpImage *) indexedImage);
}

static void test_bitmapUnlockBits ()
{
	GpStatus status;
	GpBitmap *image;
	BYTE scan0[] = {
		0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04,
		0x7F, 0xFF, 0x00, 0x80, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x80, 0x01, 0x02, 0x03, 0x04,
		0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04,
	};
	BitmapData data;
	memset (&data, 0, sizeof (data));
	
	GdipCreateBitmapFromScan0 (3, 4, 16, PixelFormat32bppARGB, scan0, &image);

	// Negative tests.
	status = GdipBitmapUnlockBits (NULL, &data);
	assertEqualInt (status, InvalidParameter);

	status = GdipBitmapUnlockBits (image, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipBitmapUnlockBits (image, &data);
	assertEqualInt (status, Win32Error);

	GdipDisposeImage ((GpImage *) image);
}

static void test_readExifResolution ()
{
	REAL resolution;
	GpBitmap *bitmap;
	WCHAR *bitmapFile = createWchar ("test-exif.jpg");

	assertEqualInt (GdipCreateBitmapFromFile (bitmapFile, &bitmap), Ok);

	assertEqualInt (GdipGetImageVerticalResolution (bitmap, &resolution), Ok);
	assertEqualInt (resolution, 72);

	assertEqualInt (GdipGetImageHorizontalResolution (bitmap, &resolution), Ok);
	assertEqualInt (resolution, 72);

	GdipDisposeImage ((GpImage *) bitmap);
	freeWchar (bitmapFile);
}


int
main(int argc, char**argv)
{
	STARTUP;

	test_createBitmapFromStream ();
	test_createBitmapFromFile ();
	test_createBitmapFromStreamICM ();
	test_createBitmapFromFileICM ();
	test_createBitmapFromScan0 ();
	test_createBitmapFromScan064bppARGB ();
	test_createBitmapFromScan064bppPARGB ();
	test_createBitmapFromScan048bppRGB ();
	test_createBitmapFromScan032bppARGB ();
	test_createBitmapFromScan032bppPARGB ();
	test_createBitmapFromScan032bppRGB ();
	test_createBitmapFromScan032bppCMYK ();
	test_createBitmapFromScan024bppRGB ();
	test_createBitmapFromScan016bppRGB555 ();
	test_createBitmapFromScan016bppRGB565 ();
	test_createBitmapFromScan016bppARGB1555 ();
	test_createBitmapFromScan016bppGrayScale ();
	test_createBitmapFromScan08bppIndexed ();
	test_createBitmapFromScan04bppIndexed ();
	test_createBitmapFromScan01bppIndexed ();
	test_createBitmapFromGraphics ();
	test_bitmapLockBits64bppARGB ();
	test_bitmapLockBits64bppPARGB ();
	test_bitmapLockBits48bppRGB ();
	test_bitmapLockBits32PARGB ();
	test_bitmapLockBits32bppARGB ();
	test_bitmapLockBits32bppRGB ();
	test_bitmapLockBits24bppRGB ();
	test_bitmapLockBits16bppRGB555 ();
	test_bitmapLockBits16bppRGB565 ();
	test_bitmapLockBits16bppARGB1555 ();
	test_bitmapLockBits16bppGrayScale ();
	test_bitmapLockBits8bppIndexed ();
	test_bitmapLockBits4bppIndexed ();
	test_bitmapLockBits1bppIndexed ();
	test_bitmapSetPixel64bppARGB ();
	test_bitmapSetPixel64bppPARGB ();
	test_bitmapSetPixel48bppRGB ();
	test_bitmapSetPixel32bppPARGB ();
	test_bitmapSetPixel32bppARGB ();
	test_bitmapSetPixel32bppRGB ();
	test_bitmapSetPixel24bppRGB ();
	test_bitmapSetPixel16bppRGB555 ();
	test_bitmapSetPixel16bppRGB565 ();
	test_bitmapSetPixel16bppARGB1555 ();
	test_bitmapSetPixel8bppIndexed ();
	test_bitmapSetPixel4bppIndexed ();
	test_bitmapSetPixel1bppIndexed ();
	test_bitmapSetPixel ();
	test_bitmapGetPixel ();
	test_bitmapLockBits ();
	test_bitmapUnlockBits ();
	test_readExifResolution ();

	SHUTDOWN;
	return 0;
}
