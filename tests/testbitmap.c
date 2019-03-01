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

static void test_createBitmapFromScan0 ()
{
	GpStatus status;
	GpBitmap *bitmap;
	ARGB emptyPixelsWithAlpha[] = {0x00000000, 0x00000000,0x00000000};
	ARGB emptyPixelsWithNoAlpha[] = {0xFF000000, 0xFF000000,0xFF000000};
	
	// No scan0 - PixelFormat64bppARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat64bppARGB, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif
	
	// Has scan0 - PixelFormat64bppARGB.
	BYTE bpp64ArgbData[] = {
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 8, PixelFormat64bppARGB, bpp64ArgbData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	// It appears that GDI+ ignores the data.
	ARGB bpp64ArgbPixels[] = {
		0x00000000,
		0x00000000,
		0x00000000,
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp64ArgbPixels);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// No scan0 - PixelFormat64bppPARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat64bppPARGB, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif
	
	// Has scan0 - PixelFormat64bppPARGB.
	BYTE bpp64PArgbData[] = {
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 8, PixelFormat64bppPARGB, bpp64PArgbData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	// It appears that GDI+ ignores the data.
	ARGB bpp64PArgbPixels[] = {
		0x00000000,
		0x00000000,
		0x00000000,
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp64PArgbPixels);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// No scan0 - PixelFormat48bppRGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat48bppRGB, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// Has scan0 - PixelFormat48bppRGB.
	BYTE bpp48RgbData[] = {
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 8, PixelFormat48bppRGB, bpp48RgbData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	// It appears that GDI+ ignores the data.
	ARGB bpp48RgbPixels[] = {
		0xFF000000,
		0xFF000000,
		0xFF000000,
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp48RgbPixels);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// No scan0 - PixelFormat32bppARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - PixelFormat32bppARGB.
	BYTE bpp32ArgbData[] = {
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0x80,
		0x00, 0x00, 0xFF, 0x00
	};
	ARGB bpp32ArgbPixels[] = {
		0xFF0000FF,
		0x8000FF00,
		0x00FF0000,
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppARGB, bpp32ArgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32ArgbPixels);
	
	// Holds a reference to the original data.
	ARGB bpp32ArgbPixelsModified[] = {
		0xFF000000,
		0x8000FF00,
		0x00FF0000
	};
	bpp32ArgbData[0] = 0x00;
	verifyPixels (bitmap, bpp32ArgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - PixelFormat32bppPARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppPARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - PixelFormat32bppPARGB.
	BYTE bpp32PArgbData[] = {
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0x80,
		0x00, 0x00, 0xFF, 0x00
	};
	ARGB bpp32PArgbPixels[] = {
		0xFF0000FF,
		0x8000FF00,
		0x00FF0000
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppPARGB, bpp32PArgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32PArgbPixels);
	
	// Holds a reference to the original data.
	ARGB bpp32PArgbPixelsModified[] = {
		0xFF000000,
		0x8000FF00,
		0x00FF0000
	};
	bpp32PArgbData[0] = 0x00;
	verifyPixels (bitmap, bpp32PArgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - PixelFormat32bppRGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppRGB, 1, 2, 0, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - PixelFormat32bppRGB.
	BYTE bpp32RgbData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00
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
	
	// Holds a reference to the original data.
	ARGB bpp32RgbPixelsModified[] = {
		0xFF000000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp32RgbData[0] = 0x00;
	verifyPixels (bitmap, bpp32RgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - PixelFormat32bppCMYK.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppCMYK, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif
	
	BYTE bpp32CMYKData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppCMYK, bpp32CMYKData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// No scan0 - PixelFormat24bppRGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat24bppRGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - PixelFormat24bppRGB.
	BYTE bpp24RgbData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00
	};
	ARGB bpp24RgbPixels[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFFFF0000
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat24bppRGB, bpp24RgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat24bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp24RgbPixels);
	
	// Holds a reference to the original data.
	ARGB bpp24RgbPixelsModified[] = {
		0xFF000000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp24RgbData[0] = 0x00;
	verifyPixels (bitmap, bpp24RgbPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - PixelFormat16bppRGB555.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppRGB555, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - PixelFormat16bppRGB555.
	BYTE bpp16rgb555Data[] = {
		0x1F, 0x00, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00,
		0x00, 0x7C, 0x00, 0x00
	};
	ARGB bpp16rgb555Pixels[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFFFF0000,
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppRGB555, bpp16rgb555Data, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB555, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp16rgb555Pixels);

	// Holds a reference to the original data.
	ARGB bpp16rgb555PixelsModified[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFF000000
	};
	bpp16rgb555Data[9] = 0x00;
	verifyPixels (bitmap, bpp16rgb555PixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);

	// No scan0 - PixelFormat16bppRGB565.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppRGB565, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppRGB565, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);

	// Has scan0 - PixelFormat16bppRGB565.
	BYTE bpp16rgb565Data[] = {
		0x1F, 0x00, 0x00, 0x00,
		0xE0, 0x07, 0x00, 0x00,
		0x00, 0xF8, 0x00, 0x00
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
	
	// Holds a reference to the original data.
	ARGB bpp16rgb565PixelsModified[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFF000000
	};
	bpp16rgb565Data[9] = 0x00;
	verifyPixels (bitmap, bpp16rgb565PixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - PixelFormat16bppARGB1555.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppARGB1555, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// Has scan0 - PixelFormat16bppARGB1555.
	BYTE bpp16argb555Data[] = {
		0x1F, 0x80, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00,
		0x00, 0x7C, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppARGB1555, bpp16argb555Data, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ARGB bpp16argb555Pixels[] = {
		0xFF0000FF,
		0x0000FF00,
		0x00FF0000
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp16argb555Pixels);

	// Holds a reference to the original data.
	ARGB bpp16argb555PixelsModified[] = {
		0xFF0000FF,
		0x0000FF00,
		0x00000000
	};
	bpp16argb555Data[9] = 0x00;
	verifyPixels (bitmap, bpp16argb555PixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif
	
	// No scan0 - PixelFormat16bppGrayScale.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppGrayScale, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif
	
	// Has scan0 - PixelFormat16bppGrayScale.
	BYTE bpp16grayscaleData[] = {
		0x1F, 0x80, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00,
		0x00, 0x7C, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppGrayScale, bpp16grayscaleData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	assertEqualInt (status, NotImplemented);
#endif

	// No scan0 - PixelFormat8bppIndexed.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat8bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - PixelFormat8bppIndexed.
	BYTE bpp8indexedData[] = {
		0x0C, 0x00, 0x00, 0x00,
		0x0A, 0x00, 0x00, 0x00,
		0x09, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat8bppIndexed, bpp8indexedData, &bitmap);
	ARGB bpp8indexedPixels[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFFFF0000
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat8bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp8indexedPixels);
	
	// Holds a reference to the original data.
	ARGB bpp8indexedPixelsModified[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp8indexedData[0] = 0x09;
	verifyPixels (bitmap, bpp8indexedPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - PixelFormat4bppIndexed.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat4bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - PixelFormat4bppIndexed.
	BYTE bpp4indexedData[] = {
		0xC0, 0x00, 0x00, 0x00,
		0xA0, 0x00, 0x00, 0x00,
		0x90, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat4bppIndexed, bpp4indexedData, &bitmap);
	ARGB bpp4indexedPixels[] = {
		0xFF0000FF,
		0xFF00FF00,
		0xFFFF0000
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat4bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp4indexedPixels);
	
	// Holds a reference to the original data.
	ARGB bpp4indexedPixelsModified[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFFFF0000
	};
	bpp4indexedData[0] = 0x90;
	verifyPixels (bitmap, bpp4indexedPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// No scan0 - PixelFormat1bppIndexed.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat1bppIndexed, NULL, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
	
	// Has scan0 - PixelFormat1bppIndexed.
	BYTE bpp1indexedData[] = {
		0x00, 0x00, 0x00, 0x00,
		0x80, 0x00, 0x00, 0x00,
		0x80, 0x00, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat1bppIndexed, bpp1indexedData, &bitmap);
	ARGB bpp1indexedPixels[] = {
		0xFF000000,
		0xFFFFFFFF,
		0xFFFFFFFF
	};
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat1bppIndexed, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp1indexedPixels);

	// Holds a reference to the original data.
	ARGB bpp1indexedPixelsModified[] = {
		0xFF000000,
		0xFFFFFFFF,
		0xFF000000
	};
	bpp1indexedData[8] = 0x00;
	verifyPixels (bitmap, bpp1indexedPixelsModified);
	GdipDisposeImage ((GpImage *) bitmap);

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

int
main(int argc, char**argv)
{
	STARTUP;

	test_createBitmapFromStream ();
	test_createBitmapFromFile ();
	test_createBitmapFromStreamICM ();
	test_createBitmapFromFileICM ();
	test_createBitmapFromScan0 ();
	test_createBitmapFromGraphics ();
	test_bitmapLockBits ();
	test_bitmapUnlockBits ();

	SHUTDOWN;
	return 0;
}
