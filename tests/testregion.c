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

GpImage *image;
GpGraphics *graphics;

#define verifyRegion(region, expectedX, expectedY, expectedWidth, expectedHeight, expectedIsEmpty, expectedIsInfinite) \
{ \
	GpStatus status; \
	GpRectF bounds; \
	BOOL isEmpty; \
	BOOL isInfinite; \
\
	status = GdipGetRegionBounds (region, graphics, &bounds); \
	assertEqualInt (status, Ok); \
	assertEqualFloat (bounds.X, expectedX); \
	assertEqualFloat (bounds.Y, expectedY); \
	assertEqualFloat (bounds.Width, expectedWidth); \
	assertEqualFloat (bounds.Height, expectedHeight); \
\
	status = GdipIsEmptyRegion (region, graphics, &isEmpty); \
	assertEqualInt (status, Ok); \
	assert (isEmpty == (BOOL) expectedIsEmpty); \
\
	status = GdipIsInfiniteRegion (region, graphics, &isInfinite); \
	assertEqualInt (status, Ok); \
	assert (isInfinite == (BOOL) expectedIsInfinite); \
}

static void test_createRegion ()
{
	GpStatus status;
	GpRegion *region;

	status = GdipCreateRegion (&region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegion (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createRegionRect ()
{
	GpStatus status;
	GpRectF normalRect = {1, 2, 3, 4};
	GpRectF negativeWidthRect = {1, 2, -3, 4};
	GpRectF negativeHeightRect = {1, 2, 3, -4};
	GpRectF zeroRect = {0, 0, 0, 0};
	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GpRegion *region;

	// Normal.
	status = GdipCreateRegionRect (&normalRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative width.
	status = GdipCreateRegionRect (&negativeWidthRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative height.
	status = GdipCreateRegionRect (&negativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Zero.
	status = GdipCreateRegionRect (&zeroRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Infinite.
	status = GdipCreateRegionRect (&infiniteRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegionRect (NULL, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRect (&normalRect, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createRegionRectI ()
{
	GpStatus status;
	GpRect normalRect = {1, 2, 3, 4};
	GpRect negativeWidthRect = {1, 2, -3, 4};
	GpRect negativeHeightRect = {1, 2, 3, -4};
	GpRect zeroRect = {0, 0, 0, 0};
	GpRect infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	GpRegion *region;

	// Normal.
	status = GdipCreateRegionRectI (&normalRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative width.
	status = GdipCreateRegionRectI (&negativeWidthRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative height.
	status = GdipCreateRegionRectI (&negativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Zero.
	status = GdipCreateRegionRectI (&zeroRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Infinite.
	status = GdipCreateRegionRectI (&infiniteRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegionRectI (NULL, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRectI (&normalRect, NULL);
	assertEqualInt (status, InvalidParameter);
}

int
main (int argc, char**argv)
{
	STARTUP;

	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppRGB, NULL, (GpBitmap **) &image);
	GdipGetImageGraphicsContext (image, &graphics);

	test_createRegion ();
	test_createRegionRect ();
	test_createRegionRectI ();

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN;
	return 0;
}
