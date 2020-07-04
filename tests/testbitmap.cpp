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

#include <gtest/gtest.h>
#include "testhelpers.h"

TEST(Bitmap, GdipCreateBitmapFromStream)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;

	// Negative tests.
	status = GdipCreateBitmapFromStream (NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipCreateBitmapFromStream (&temp, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromStream (&temp, &bitmap);
	ASSERT_EQ (NotImplemented, status);
#endif

	SHUTDOWN
}

TEST(Bitmap, GdipCreateBitmapFromFile)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;
	WCHAR *bitmapFile = createWchar ("test.png");
	WCHAR *metafileFile = createWchar ("test.wmf");
	WCHAR *enhancedMetafileFile = createWchar ("test.emf");
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	status = GdipCreateBitmapFromFile (bitmapFile, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap ((GpImage *) bitmap, pngRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromFile (metafileFile, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 770, 649, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromFile (enhancedMetafileFile, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 100, 100, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	// Negative tests.
	status = GdipCreateBitmapFromFile (NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromFile (noSuchFile, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromFile (invalidFile, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromFile (noSuchFile, NULL);
	ASSERT_EQ (InvalidParameter, status);

	freeWchar (bitmapFile);
	freeWchar (metafileFile);
	freeWchar (enhancedMetafileFile);
	freeWchar (noSuchFile);
	freeWchar (invalidFile);

	SHUTDOWN
}

TEST(Bitmap, GdipCreateBitmapFromStreamICM)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;

	// Negative tests.
	status = GdipCreateBitmapFromStreamICM (NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipCreateBitmapFromStreamICM (&temp, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromStreamICM (&temp, &bitmap);
	ASSERT_EQ (NotImplemented, status);
#endif

	SHUTDOWN
}

TEST(Bitmap, GdipCreateBitmapFromFileICM)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;
	WCHAR *bitmapFile = createWchar ("test.png");
	WCHAR *metafileFile = createWchar ("test.wmf");
	WCHAR *enhancedMetafileFile = createWchar ("test.emf");
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	status = GdipCreateBitmapFromFileICM (bitmapFile, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap ((GpImage *) bitmap, pngRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromFileICM (metafileFile, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 770, 649, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromFileICM (enhancedMetafileFile, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap ((GpImage *) bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 100, 100, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	// Negative tests.
	status = GdipCreateBitmapFromFileICM (NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromFileICM (noSuchFile, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromFileICM (invalidFile, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromFileICM (noSuchFile, NULL);
	ASSERT_EQ (InvalidParameter, status);

	freeWchar (bitmapFile);
	freeWchar (metafileFile);
	freeWchar (enhancedMetafileFile);
	freeWchar (noSuchFile);
	freeWchar (invalidFile);

	SHUTDOWN
}

TEST(Bitmap, GdipCreateBitmapFromScan0)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;
	ARGB emptyPixelsWithAlpha[] = {0x00000000, 0x00000000,0x00000000};
	ARGB emptyPixelsWithNoAlpha[] = {0xFF000000, 0xFF000000,0xFF000000};

	// No scan0 - PixelFormat64bppARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat64bppARGB, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
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
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp64ArgbPixels);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	// No scan0 - PixelFormat64bppPARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat64bppPARGB, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
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
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat64bppPARGB, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp64PArgbPixels);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	// No scan0 - PixelFormat48bppRGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat48bppRGB, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithNoAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
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
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat48bppRGB, 1, 2, 0, 0, TRUE);
	verifyPixels (bitmap, bpp48RgbPixels);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	// No scan0 - PixelFormat32bppARGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppARGB, NULL, &bitmap);
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	BYTE bpp32CMYKData[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat32bppCMYK, bpp32CMYKData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppCMYK, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	// No scan0 - PixelFormat24bppRGB.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat24bppRGB, NULL, &bitmap);
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppARGB1555, 1, 2, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, emptyPixelsWithAlpha);
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (NotImplemented, status);
#endif

	// No scan0 - PixelFormat16bppGrayScale.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat16bppGrayScale, NULL, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	// Has scan0 - PixelFormat16bppGrayScale.
	BYTE bpp16grayscaleData[] = {
		0x1F, 0x80, 0x00, 0x00,
		0xE0, 0x03, 0x00, 0x00,
		0x00, 0x7C, 0x00, 0x00
	};
	status = GdipCreateBitmapFromScan0 (1, 2, 4, PixelFormat16bppGrayScale, bpp16grayscaleData, &bitmap);
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat16bppGrayScale, 1, 2, 0, 0, TRUE);
	// Don't verify the image pixel data as a bug in GDI+ means that we read uninitialized memory.
	GdipDisposeImage ((GpImage *) bitmap);
#else
	ASSERT_EQ (NotImplemented, status);
#endif

	// No scan0 - PixelFormat8bppIndexed.
	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat8bppIndexed, NULL, &bitmap);
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (Ok, status);
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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (-1, 2, 0, PixelFormat32bppRGB, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 0, 0, PixelFormat32bppRGB, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, -1, 0, PixelFormat32bppRGB, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, -1, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 3, PixelFormat32bppRGB, bpp32RgbData, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatIndexed, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatGDI, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatAlpha, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatPAlpha, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatExtended, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatCanonical, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormatUndefined, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromScan0 (1, INT_MAX, 4, PixelFormat1bppIndexed, NULL, &bitmap);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipCreateBitmapFromScan0 (1, 2, INT_MAX, PixelFormat1bppIndexed, NULL, &bitmap);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipCreateBitmapFromScan0 (1, INT_MAX, INT_MAX, PixelFormat1bppIndexed, NULL, &bitmap);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipCreateBitmapFromScan0 (1, 2, 0, PixelFormat1bppIndexed, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(Bitmap, GdipCreateBitmapFromGraphics)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 24, 32, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage ((GpImage *) bitmap);

	status = GdipCreateBitmapFromGraphics (24, 32, graphicsWithResolution, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 24, 32, ImageFlagsHasAlpha, 0, TRUE);

	// Verify the DPI.
	GdipGetImageGraphicsContext ((GpImage *) bitmap, &bitmapGraphics);
	GdipGetDpiX (bitmapGraphics, &dpiX);
	GdipGetDpiY (bitmapGraphics, &dpiY);
	ASSERT_FLOAT_EQ_NAN (100, dpiX);
	ASSERT_FLOAT_EQ_NAN (124, dpiY);

	GdipDisposeImage ((GpImage *) bitmap);
	GdipDeleteGraphics (bitmapGraphics);

	// Negative tests.
	status = GdipCreateBitmapFromGraphics (0, 32, graphics, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromGraphics (-1, 32, graphics, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromGraphics (24, 0, graphics, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromGraphics (24, -1, graphics, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromGraphics (24, 32, NULL, &bitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateBitmapFromGraphics (24, 32, graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage ((GpImage *) source);
	GdipDisposeImage ((GpImage *) sourceWithResolution);
	GdipDeleteGraphics (graphics);
	GdipDeleteGraphics (graphicsWithResolution);

	SHUTDOWN
}

TEST(Bitmap, GdipBitmapSetPixel)
{
	STARTUP

	GpStatus status;
	GpBitmap *image;
	GpBitmap *indexedImage;

	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppRGB, NULL, &image);
	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat8bppIndexed, NULL, &indexedImage);

	// Negative tests.
	status = GdipBitmapSetPixel (NULL, 0, 0, 0xFF000000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, -1, 1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (indexedImage, -1, 1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 5, 1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (indexedImage, 5, 1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 6, 1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (indexedImage, 6, 1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 1, -1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (indexedImage, 1, -1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 1, 3, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (indexedImage, 1, 3, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 1, 4, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (indexedImage, 1, 4, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	// Locked.
	BitmapData data;
	memset (&data, 0, sizeof (data));

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (Ok, status);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	// FIXME: indexed formats not supported: https://github.com/mono/libgdiplus/issues/462
	status = GdipBitmapSetPixel (indexedImage, 0, 0, 0xFFFF0000);
#if !defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (InvalidParameter, status);
#else
	ASSERT_EQ (Ok, status);

	status = GdipBitmapGetPixel (indexedImage, 0, 0, &pixel);
	ASSERT_EQ (Ok, status);
	assertEqualARGB (pixel, 0xFFFF0000);
#endif

	status = GdipBitmapSetPixel (image, -1, 0, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapSetPixel (indexedImage, -1, 0, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 5, 0, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapSetPixel (indexedImage, 5, 0, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image,  6, 0, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapSetPixel (indexedImage,  6, 0, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 0, -1, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapSetPixel (indexedImage, 0, -1, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 0, 3, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapSetPixel (indexedImage, 0, 3, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapSetPixel (image, 0, 4, 0xFFFF0000);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapSetPixel (indexedImage, 0, 4, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	GdipBitmapUnlockBits (image, &data);
	GdipDisposeImage ((GpImage *) image);

	// Invalid type.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppCMYK, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (NotImplemented, status);
#else

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage ((GpImage *) image);
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppGrayScale, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (NotImplemented, status);
#else
	ASSERT_EQ (Ok, status);

	status = GdipBitmapSetPixel (image, 0, 0, 0xFFFF0000);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage ((GpImage *) image);
#endif

	GdipDisposeImage ((GpImage *) indexedImage);

	SHUTDOWN
}

TEST(Bitmap, GdipBitmapGetPixel)
{
	STARTUP

	GpStatus status;
	GpBitmap *image;
	GpBitmap *indexedImage;
	ARGB pixel;

	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppRGB, NULL, &image);
	GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat8bppIndexed, NULL, &indexedImage);

	// Negative tests.
	status = GdipBitmapGetPixel (NULL, 0, 0, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, -1, 1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, -1, 1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 5, 1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, 5, 1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 6, 1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, 6, 1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 1, -1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, 1, -1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 1, 3, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, 1, 3, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 1, 4, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, 1, 4, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (indexedImage, 1, 4, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	// Locked.
	BitmapData data;
	memset (&data, 0, sizeof (data));

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (Ok, status);

	status = GdipBitmapGetPixel (image, 0, 0, &pixel);
	ASSERT_EQ (WrongState, status);

	pixel = 0xCCCCCCCC;
	status = GdipBitmapGetPixel (indexedImage, 0, 0, &pixel);
	ASSERT_EQ (Ok, status);
	assertEqualARGB (pixel, 0xFF000000);

	status = GdipBitmapGetPixel (image, -1, 0, &pixel);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapGetPixel (indexedImage, -1, 0, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 5, 0, &pixel);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapGetPixel (indexedImage, 5, 0, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image,  6, 0, &pixel);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapGetPixel (indexedImage,  6, 0, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 0, -1, &pixel);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapGetPixel (indexedImage, 0, -1, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 0, 3, &pixel);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapGetPixel (indexedImage, 0, 3, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapGetPixel (image, 0, 4, &pixel);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapGetPixel (indexedImage, 0, 4, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	GdipBitmapUnlockBits (image, &data);
	GdipDisposeImage ((GpImage *) image);

	// Invalid type.
	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat32bppCMYK, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (NotImplemented, status);
#else
	ASSERT_EQ (Ok, status);

	status = GdipBitmapGetPixel (image, 0, 0, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage ((GpImage *) image);
#endif

	status = GdipCreateBitmapFromScan0 (5, 3, 0, PixelFormat16bppGrayScale, NULL, &image);
#if !defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (NotImplemented, status);
#else
	ASSERT_EQ (Ok, status);

	status = GdipBitmapGetPixel (image, 0, 0, &pixel);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage ((GpImage *) image);
#endif

	GdipDisposeImage ((GpImage *) indexedImage);

	SHUTDOWN
}

TEST(Bitmap, GdipBitmapLockBits)
{
	STARTUP

	GpStatus status;
	GpBitmap *image;
	GpBitmap *indexedImage;
	BitmapData data;
	memset (&data, 0, sizeof (data));

	GdipCreateBitmapFromScan0 (3, 4, 0, PixelFormat32bppRGB, NULL, &image);
	GdipCreateBitmapFromScan0 (3, 4, 0, PixelFormat32bppRGB, NULL, &indexedImage);

	// Negative tests.
	status = GdipBitmapLockBits (NULL, NULL, 0, PixelFormat32bppARGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect negativeX = {-1, 0, 3, 4};
	status = GdipBitmapLockBits (image, &negativeX, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &negativeX, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect negativeY = {0, -1, 3, 4};
	status = GdipBitmapLockBits (image, &negativeY, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &negativeY, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect negativeWidth = {0, 0, -1, 4};
	status = GdipBitmapLockBits (image, &negativeWidth, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &negativeWidth, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect zeroWidth = {0, 0, 0, 4};
	status = GdipBitmapLockBits (image, &zeroWidth, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &zeroWidth, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect largeWidth = {0, 0, 4, 4};
	status = GdipBitmapLockBits (image, &largeWidth, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &largeWidth, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect negativeHeight = {0, 0, 3, -1};
	status = GdipBitmapLockBits (image, &negativeHeight, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &negativeHeight, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect zeroHeight = {0, 0, 3, 0};
	status = GdipBitmapLockBits (image, &zeroHeight, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &zeroHeight, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect largeHeight = {0, 0, 3, 5};
	status = GdipBitmapLockBits (image, &largeHeight, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &largeHeight, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppCMYK, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect invalidXWidth = {2, 0, 2, 4};
	status = GdipBitmapLockBits (image, &invalidXWidth, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &invalidXWidth, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	Rect invalidYHeight = {0, 2, 3, 3};
	status = GdipBitmapLockBits (image, &invalidYHeight, 0, PixelFormat32bppRGB, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (indexedImage, &invalidYHeight, 0, PixelFormat8bppIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppCMYK, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatIndexed, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatGDI, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatAlpha, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatPAlpha, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatExtended, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatCanonical, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormatUndefined, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, -10, &data);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, NULL);
	ASSERT_EQ (InvalidParameter, status);

	// Locked.
	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, &data);
	ASSERT_EQ (Ok, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, &data);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppARGB, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapLockBits (image, NULL, 0, PixelFormat32bppCMYK, &data);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapLockBits (image, NULL, 0, -10, &data);
	ASSERT_EQ (WrongState, status);

	status = GdipBitmapUnlockBits (image, &data);
	ASSERT_EQ (Ok, status);

	GdipDisposeImage ((GpImage *) image);
	GdipDisposeImage ((GpImage *) indexedImage);

	SHUTDOWN
}

TEST(Bitmap, GdipBitmapUnlockBits)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapUnlockBits (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipBitmapUnlockBits (image, &data);
	ASSERT_EQ (Win32Error, status);

	GdipDisposeImage ((GpImage *) image);

	SHUTDOWN
}

TEST(Bitmap, readExifResolution)
{
	STARTUP

	GpStatus status;
	REAL resolution;
	GpBitmap *bitmap;
	WCHAR *bitmapFile = createWchar ("test-exif.jpg");

	status = GdipCreateBitmapFromFile (bitmapFile, &bitmap);
	ASSERT_EQ (Ok, status);

	status = GdipGetImageVerticalResolution (bitmap, &resolution);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (72, resolution);

	status = GdipGetImageHorizontalResolution (bitmap, &resolution);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (72, resolution);

	GdipDisposeImage ((GpImage *) bitmap);
	freeWchar (bitmapFile);

	SHUTDOWN
}
