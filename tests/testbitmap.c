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
	// FIXME: libgdiplus should convert metafiles into a thumbnail.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 770, 649, ImageFlagsHasAlpha, 0, TRUE);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	status = GdipCreateBitmapFromFile (enhancedMetafileFile, &bitmap);
	assertEqualInt (status, Ok);
	// FIXME: libgdiplus should convert metafiles into a thumbnail.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 100, 100, ImageFlagsHasAlpha, 0, TRUE);
#endif
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
	// FIXME: libgdiplus should convert metafiles into a thumbnail.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 770, 649, ImageFlagsHasAlpha, 0, TRUE);
#endif
	GdipDisposeImage ((GpImage *) bitmap);
	
	status = GdipCreateBitmapFromFileICM (enhancedMetafileFile, &bitmap);
	assertEqualInt (status, Ok);
	// FIXME: libgdiplus should convert metafiles into a thumbnail.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 100, 100, ImageFlagsHasAlpha, 0, TRUE);
#endif
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

int
main(int argc, char**argv)
{
	STARTUP;

	test_createBitmapFromStream ();
	test_createBitmapFromFile ();
	test_createBitmapFromStreamICM ();
	test_createBitmapFromFileICM ();

	SHUTDOWN;
	return 0;
}
