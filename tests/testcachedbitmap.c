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

#define C(func) assertEqualInt (func, Ok)

static GpGraphics *getImageGraphics (GpImage **image)
{
	WCHAR *filePath;
	GpGraphics *graphics;

	filePath = createWchar ("test.bmp");
	C (GdipLoadImageFromFile (filePath, image));

	freeWchar (filePath);

	C (GdipGetImageGraphicsContext (*image, &graphics));

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

static void createBitmaps (GpBitmap **originalBitmap, GpBitmap **blankBitmap, GpGraphics **blankGraphics, UINT *width, UINT *height)
{
	*originalBitmap = getImage ("test.bmp");
	C (GdipGetImageWidth (*originalBitmap, width));
	C (GdipGetImageHeight (*originalBitmap, height));

	C (GdipCreateBitmapFromScan0 (*width, *height, 0, PixelFormat32bppARGB, 0, blankBitmap));
	C (GdipGetImageGraphicsContext (*blankBitmap, blankGraphics));
}

static void compareEqual (GpBitmap *expected, GpBitmap *actual, UINT width, UINT height)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			ARGB expectedColor, actualColor;

			C (GdipBitmapGetPixel (expected, x, y, &expectedColor));
			C (GdipBitmapGetPixel (actual, x, y, &actualColor));

			assertEqualInt (expectedColor, actualColor);
		}
	}
}

CLSID png_clsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };

static void test_clip ()
{
	UINT width;
	UINT height;
	GpBitmap *originalBitmap;
	GpBitmap *surface;
	GpGraphics *graphics;
	GpCachedBitmap *cached;

	createBitmaps(&originalBitmap, &surface, &graphics, &width, &height);
	
	C (GdipSetClipRect (graphics, 20, 20, 20, 20, CombineModeReplace));

	C (GdipCreateCachedBitmap (originalBitmap, graphics, &cached));
	C (GdipDrawCachedBitmap (graphics, cached, 0, 0));

	GpImage *expected = getImage("test-clipped.png");
	compareEqual(expected, surface, width, height);
}

static void test_matrix ()
{
	UINT width;
	UINT height;
	GpBitmap *originalBitmap;
	GpBitmap *surface;
	GpGraphics *graphics;
	GpCachedBitmap *cached;

	createBitmaps(&originalBitmap, &surface, &graphics, &width, &height);

	C (GdipTranslateWorldTransform(graphics, 30, 30, MatrixOrderAppend));

	C (GdipCreateCachedBitmap (originalBitmap, graphics, &cached));
	C (GdipDrawCachedBitmap (graphics, cached, 0, 0));

	GpImage *expected = getImage("test-translated.png");
	compareEqual(expected, surface, width, height);

	// Negative tests.
	C (GdipScaleWorldTransform(graphics, 30, 30, MatrixOrderAppend));
	assertEqualInt (GdipDrawCachedBitmap(graphics, cached, 0, 0), WrongState);
	C (GdipRotateWorldTransform(graphics, 30, MatrixOrderAppend));
	assertEqualInt (GdipDrawCachedBitmap(graphics, cached, 0, 0), WrongState);
}


static void test_roundtrip ()
{
	UINT width;
	UINT height;
	GpBitmap *originalBitmap;
	GpBitmap *surface;
	GpGraphics *graphics;
	GpCachedBitmap *cached;

	createBitmaps(&originalBitmap, &surface, &graphics, &width, &height);

	C (GdipCreateCachedBitmap (originalBitmap, graphics, &cached));
	C (GdipDrawCachedBitmap (graphics, cached, 0, 0));

	compareEqual(originalBitmap, surface, width, height);
}

static void test_create ()
{
	GpImage *image;
	GpGraphics *graphics;
	GpCachedBitmap* cachedBitmap;

	graphics = getImageGraphics (&image);

	C (GdipCreateCachedBitmap (image, graphics, &cachedBitmap));

	// Negative tests.
	image = getImage ("test.wmf");
	assertEqualInt (GdipCreateCachedBitmap (image, graphics, &cachedBitmap), InvalidParameter);

	assertEqualInt (GdipCreateCachedBitmap (image, graphics, NULL), InvalidParameter);
	assertEqualInt (GdipCreateCachedBitmap (image, NULL, &cachedBitmap), InvalidParameter);
	assertEqualInt (GdipCreateCachedBitmap (NULL, graphics, &cachedBitmap), InvalidParameter);

	C (GdipDeleteGraphics(graphics));
	C (GdipDisposeImage(image));
}

static void test_delete ()
{
	GpImage *image;
	GpGraphics *graphics;
	GpCachedBitmap* cachedBitmap;

	graphics = getImageGraphics (&image);

	C (GdipCreateCachedBitmap(image, graphics, &cachedBitmap));
	C (GdipDeleteCachedBitmap(cachedBitmap));

	// Negative tests.
	assertEqualInt (GdipDeleteCachedBitmap (NULL), InvalidParameter);
}

int main (int argc, char**argv)
{
	STARTUP;

	test_create ();
	test_roundtrip ();
	test_clip ();
	test_matrix ();
	test_delete ();

	SHUTDOWN;
	return 0;
}
