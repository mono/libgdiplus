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

#define C(func) assert (func == Ok)

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

static void test_roundtrip ()
{
	UINT width;
	UINT height;
	GpBitmap *originalBitmap;
	GpBitmap *surface;
	GpGraphics *graphics;
	GpCachedBitmap *cached;

	originalBitmap = getImage ("test.bmp");

	C (GdipGetImageWidth (originalBitmap, &width));
	C (GdipGetImageHeight (originalBitmap, &height));

	C (GdipCreateBitmapFromScan0 (width, height, 0, PixelFormat32bppARGB, 0, &surface));
	C (GdipGetImageGraphicsContext (surface, &graphics));

	GpRect rect = {
		.X = 0,
		.Y = 0,
		.Width = width,
		.Height = height
	};

	C (GdipSetVisibleClip_linux (graphics, &rect));
	C (GdipCreateCachedBitmap (originalBitmap, graphics, &cached));
	C (GdipDrawCachedBitmap (graphics, cached, 0, 0));

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			ARGB drawn, original;

			C (GdipBitmapGetPixel (originalBitmap, x, y, &original));
			C (GdipBitmapGetPixel (surface, x, y, &drawn));

			assertEqualInt (drawn, original);
		}
	}
}

static void test_create ()
{
	GpImage *image;
	GpGraphics *graphics;
	GpCachedBitmap* cachedBitmap;

	graphics = getImageGraphics (&image);

	C (GdipCreateCachedBitmap(image, graphics, &cachedBitmap));

	// Negative tests.
	image = getImage ("test.wmf");
	assertEqualInt (GdipCreateCachedBitmap(image, graphics, &cachedBitmap), InvalidParameter);

	assertEqualInt (GdipCreateCachedBitmap(image, graphics, NULL), InvalidParameter);
	assertEqualInt (GdipCreateCachedBitmap(image, NULL, &cachedBitmap), InvalidParameter);
	assertEqualInt (GdipCreateCachedBitmap(NULL, graphics, &cachedBitmap), InvalidParameter);

	C (GdipDeleteGraphics(graphics));
	C (GdipDisposeImage(image));
}

static void test_delete ()
{
	GpCachedBitmap *cachedBitmap;
	GpBitmap *image;
	GpGraphics *graphics = getImageGraphics ();

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
	test_delete ();

	SHUTDOWN;
	return 0;
}
