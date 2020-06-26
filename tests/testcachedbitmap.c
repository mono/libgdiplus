#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_GDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#else
#include <GdiPlusFlat.h>
#endif

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

static GpGraphics *getImageGraphics (GpImage **image)
{
	GpStatus status;
	WCHAR *filePath;
	GpGraphics *graphics;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, image);
	assertEqualInt (status, Ok);

	freeWchar (filePath);

	status = GdipGetImageGraphicsContext (*image, &graphics);
	assertEqualInt (status, Ok);

	return graphics;
}

static GpImage* getImage (const char* fileName) 
{
	GpStatus status;
	WCHAR *wFileName = wcharFromChar (fileName);
	GpImage *image;

	status = GdipLoadImageFromFile (wFileName, &image);
	assertEqualInt (status, Ok);

	freeWchar (wFileName);

	return image;
}

static void test_roundtrip ()
{
    UINT width;
    UINT height;
    GpBitmap *originalBitmap;
    GpBitmap *surface;
    GpGraphics *graphics;
    GpCachedBitmap *cached;

    originalBitmap = getImage("test.bmp");
    
    
    assertEqualInt(GdipGetImageWidth(originalBitmap, &width), Ok);
    assertEqualInt(GdipGetImageHeight(originalBitmap, &height), Ok);

    assertEqualInt(GdipCreateBitmapFromScan0(width, height, 0, PixelFormat32bppARGB, 0, &surface), Ok);
    assertEqualInt(GdipGetImageGraphicsContext(surface, &graphics), Ok);

    GpRect rect = {
        .X = 0,
        .Y = 0,
        .Width = width,
        .Height = height
    };


    assertEqualInt(GdipSetVisibleClip_linux(graphics, &rect), Ok);
    assertEqualInt(GdipCreateCachedBitmap(originalBitmap, graphics, &cached), Ok);
    assertEqualInt(GdipDrawCachedBitmap(graphics, cached, 0, 0), Ok);

	CLSID png_clsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	WCHAR *filePath = createWchar ("HELP.png");
	GdipSaveImageToFile (surface, filePath, &png_clsid, NULL);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            ARGB drawn, original;
            assertEqualInt(GdipBitmapGetPixel(originalBitmap, x, y, &original), Ok);

            assertEqualInt(GdipBitmapGetPixel(surface, x, y, &drawn), Ok);

            assertEqualInt(drawn, original);
        }
    }
}

static void test_create ()
{
    GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
    GpCachedBitmap* cachedBitmap;

	graphics = getImageGraphics (&image);

    assertEqualInt(GdipCreateCachedBitmap(image, graphics, &cachedBitmap), Ok);

    // Negative tests.
    image = getImage ("test.wmf");
    status = GdipCreateCachedBitmap(image, graphics, &cachedBitmap);
    assertEqualInt(status, InvalidParameter);

    assertEqualInt(GdipCreateCachedBitmap(image, graphics, NULL), InvalidParameter);
    assertEqualInt(GdipCreateCachedBitmap(image, NULL, &cachedBitmap), InvalidParameter);
    assertEqualInt(GdipCreateCachedBitmap(NULL, graphics, &cachedBitmap), InvalidParameter);

    GdipDeleteGraphics(graphics);
    GdipDisposeImage(image);
}

int
main (int argc, char**argv)
{
    STARTUP;

    test_create ();
    test_roundtrip ();

    SHUTDOWN;
    return 0;
}
