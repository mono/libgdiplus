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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "testhelpers.h"

static void createImageGraphics (INT width, INT height, GpImage **image, GpGraphics **graphics)
{
	GpStatus status;

	status = GdipCreateBitmapFromScan0 (width, height, 0, PixelFormat32bppARGB, NULL, (GpBitmap **) image);
	assertEqualInt (status, Ok);

	status = GdipGetImageGraphicsContext (*image, graphics);
	assertEqualInt (status, Ok);
}

static void test_drawArc ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, -700);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawArc (NULL, pen, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawArc (NULL, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, -1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawArcI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, -700);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawArcI (NULL, pen, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawArcI (NULL, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, -1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawBezier ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Normal.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezier (graphics, pen, 10, 20, 40, 30, 60, 50, 80, 70);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Straight line.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezier (graphics, pen, 10, 20, 30, 40, 50, 60, 70, 80);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezier (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawBezier (NULL, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBezier (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBezier (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBezier (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawBezierI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Normal.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezierI (graphics, pen, 10, 20, 40, 30, 60, 50, 80, 70);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Straight line.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezierI (graphics, pen, 10, 20, 30, 40, 50, 60, 70, 80);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezierI (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawBezierI (NULL, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBezierI (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBezierI (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBezierI (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawBeziers ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPointF normal7[] = {
		{10, 20},
		{40, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Straight line (4).
	GpPointF straight4[] = {
		{10, 20},
		{30, 40},
		{50, 60},
		{70, 80}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, straight4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4).
	GpPointF zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 2);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 1);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawBeziers (NULL, pen, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (NULL, pen, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (NULL, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (NULL, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (NULL, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (NULL, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, normal7, 8);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, normal7, 6);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, normal7, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBeziers (graphics, pen, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziers (graphics, pen, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziers (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziers (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziers (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziers (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziers (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawBeziersI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPoint normal7[] = {
		{10, 20},
		{40, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
		{200, 130}
	};

	// Normal (7).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Straight line (4).
	GpPoint straight4[] = {
		{10, 20},
		{30, 40},
		{50, 60},
		{70, 80}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, straight4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4).
	GpPoint zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 2);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 1);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawBeziersI (NULL, pen, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (NULL, pen, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (NULL, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (NULL, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (NULL, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (NULL, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, normal7, 8);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, normal7, 6);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, normal7, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBeziersI (graphics, pen, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziersI (graphics, pen, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziersI (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziersI (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziersI (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawBeziersI (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawCurve ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPointF normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 6);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4).
	GpPointF zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 2);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawCurve (NULL, pen, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (NULL, pen, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (NULL, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (NULL, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (NULL, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (NULL, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve (graphics, pen, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve (graphics, pen, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawCurveI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPoint normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 6);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4).
	GpPoint zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 2);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawCurveI (NULL, pen, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (NULL, pen, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (NULL, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (NULL, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (NULL, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (NULL, pen, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurveI (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, NULL, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurveI (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurveI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurveI (graphics, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurveI (graphics, pen, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurveI (graphics, pen, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurveI (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurveI (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurveI (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurveI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, NULL, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	// FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurveI (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurveI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurveI (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurveI (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurveI (graphics, pen, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_drawCurve2 ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPointF normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - large tension.
	GpPointF zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, -10);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawCurve2 (NULL, pen, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (NULL, pen, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, NULL, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawCurve2I ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPoint normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - large tension.
	GpPoint zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, -10);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawCurve2I (NULL, pen, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (NULL, pen, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (NULL, pen, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, NULL, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, NULL, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 1, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawCurve3 ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPointF normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 0, 6, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 1, 5, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 2, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 1, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 5, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 0, 5, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 1, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 2, 3, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 1, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 4, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 0, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 1, 3, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 2, 2, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 1, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 3, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 2, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - large tension.
	GpPointF zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 0, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 1, 2, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 2, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 1, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 2, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 2, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 1, 1, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, -10);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawCurve3 (NULL, pen, normal7, 4, 0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 4, 0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 3, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 3, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 2, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 2, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (NULL, pen, normal7, -1, 0, -2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4,  0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4,  0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 3,  0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 3,  0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 2,  0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 2,  0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, -1, 0, -2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 3, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 3, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 2, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 2, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, -1, 0, -2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, -1, 0, -2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, -2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, -2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, -1, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, -1, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 2, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 2, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 2, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 2, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 1, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 1, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 1, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 1, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 1, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 2, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawCurve3I ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPoint normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 0, 6, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 1, 5, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 2, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 1, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 5, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 0, 5, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 1, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 2, 3, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 1, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 4, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 0, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 1, 3, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 2, 2, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 1, 2, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 3, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 2, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - large tension.
	GpPoint zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 0, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 1, 2, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 2, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 1, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 2, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 2, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 1, 1, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 1, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, -10);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawCurve3I (NULL, pen, normal7, 4, 0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 4, 0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 3, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 3, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 2, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 2, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (NULL, pen, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (NULL, pen, normal7, -1, 0, -2, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4,  0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4,  0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 3,  0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 3,  0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 2,  0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 2,  0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, NULL, normal7, -1, 0, -2, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0);
	assertEqualInt (status, InvalidParameter);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 3, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 3, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 2, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 2, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurve3I (graphics, pen, NULL, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, -1, 0, -2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, NULL, -1, 0, -2, 0);
	assertEqualInt (status, OutOfMemory);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, -2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, -2, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, -1, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, -1, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 2, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 2, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 2, 0, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 2, 0, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 2, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 1, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 1, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 1, 0);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0);
	assertEqualInt (status, ObjectBusy);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 1, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 1, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 0, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 0, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 0, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 0, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 2, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawClosedCurve ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPointF normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 6);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4).
	GpPointF zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 3);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawClosedCurve (NULL, pen, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (NULL, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve (graphics, pen, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawClosedCurveI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPoint normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 6);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4).
	GpPoint zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 3);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawClosedCurveI (NULL, pen, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurveI (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	// FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurveI (graphics, pen, NULL, 4);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawClosedCurve2 ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPointF normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - large tension.
	GpPointF zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, -10);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawClosedCurve2I ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	GpPoint normal7[] = {
		{10, 20},
		{60, 30},
		{60, 50},
		{80, 70},
		{100, 90},
		{150, 110},
		{200, 130},
	};

	// Normal (7) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - large tension.
	GpPoint zero4[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, -10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 10);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, -10);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 3, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 3, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 1, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0);
	assertEqualInt (status, ObjectBusy);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0.5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0.5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0.5);
	assertEqualInt (status, OutOfMemory);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawEllipse ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 100, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 0, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 0, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, -100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, -100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, -100, -100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawEllipse (NULL, pen, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawEllipse (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawEllipse (graphics, pen, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawEllipse (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawEllipseI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 100, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 0, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 0, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, -100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, -100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, -100, -100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawEllipseI (NULL, pen, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawEllipseI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawEllipseI (graphics, pen, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawEllipseI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

const WCHAR bitmapFile[] = {'t', 'e', 's', 't', '.', 'b', 'm', 'p', 0};

#define GDIPLUS_MAXFLOAT 0x40000040
#define GDIPLUS_MINFLOAT -GDIPLUS_MAXFLOAT

// GDI+ float conversion differs from libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
#define GDIPLUS_MAXINT 0x4000007F
#define GDIPLUS_MININT -0x40000000
#else
#define GDIPLUS_MAXINT GDIPLUS_MAXFLOAT
#define GDIPLUS_MININT -GDIPLUS_MAXFLOAT
#endif

static void test_drawImage ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, 10, 20);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, -10, -20);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawImage (NULL, bitmapImage, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (graphics, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	assertEqualInt (status, ValueOverflow);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImage (graphics, bitmapImage, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImage (graphics, NULL, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImageI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, 10, 20);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, -1, -1);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawImageI (NULL, bitmapImage, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (NULL, bitmapImage, GDIPLUS_MAXINT + 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (NULL, bitmapImage, GDIPLUS_MININT - 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (NULL, bitmapImage, 0, GDIPLUS_MININT - 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (graphics, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (graphics, NULL, GDIPLUS_MAXINT + 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (graphics, NULL, GDIPLUS_MININT - 1, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (graphics, NULL, 0, GDIPLUS_MAXINT + 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (graphics, NULL, 0, GDIPLUS_MININT - 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1);
	assertEqualInt (status, ValueOverflow);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageI (graphics, bitmapImage, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageI (graphics, NULL, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImageRect ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 10, 20, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, -10, -20, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, 0, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, -150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, 150, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, 150, -100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 0, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 150, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0);
	assertEqualInt (status, ValueOverflow);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, NULL, 0, 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, NULL, 0, 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, NULL, 0, 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImageRectI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 10, 20, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, -10, -20, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, 0, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, -150, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, 150, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, 150, -100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 0, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 150, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, GDIPLUS_MAXINT + 10, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, GDIPLUS_MININT - 10, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (graphics, NULL, GDIPLUS_MAXINT + 1, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (graphics, NULL, GDIPLUS_MININT - 1, 0, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (graphics, NULL, 0, GDIPLUS_MAXINT + 1, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (graphics, NULL, 0, GDIPLUS_MININT - 1, 150, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 0);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 100);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 0);
	assertEqualInt (status, ValueOverflow);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, NULL, 0, 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, NULL, 0, 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, NULL, 0, 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 100);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImagePoints ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;
	PointF points[] = {
		{10, 20},
		{160, 10},
		{50, 120},
		{100, 150}
	};

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePoints (graphics, bitmapImage, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	PointF zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePoints (graphics, bitmapImage, zeroPoints, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	PointF negativePoints[] = {
		{100, 50},
		{10, 50},
		{10, 150}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePoints (graphics, bitmapImage, negativePoints, 3);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawImagePoints (NULL, bitmapImage, points, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 4);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, NULL, points, 5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, NULL, points, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, NULL, points, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImagePointsI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;
	Point points[] = {
		{10, 20},
		{160, 10},
		{50, 120},
		{100, 150},
		{100, 150}
	};

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	Point zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsI (graphics, bitmapImage, zeroPoints, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	Point negativePoints[] = {
		{100, 50},
		{10, 50},
		{10, 150}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsI (graphics, bitmapImage, negativePoints, 3);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 4);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, NULL, points, 5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, NULL, points, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, NULL, points, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 5);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 5);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImagePointRect ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 10, 20, 50, 20, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, -10, -20, -30, -40, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative source origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, -150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, -100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitPoint);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitInch);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitDocument);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitMillimeter);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 0,100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0,100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImagePointRectI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 10, 20, 50, 20, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, -10, -20, -30, -40, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, -150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, -100, UnitPixel);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitPoint);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitInch);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitDocument);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitMillimeter);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, NULL, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, NULL, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 0,100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 0,100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImageRectRect ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 200, 125, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 10, 20, 200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, -10, -20, 200, 125, -30, -40, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 200, 125, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 0, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, -200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 0, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, -125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Zero source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, -150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, -100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif	

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0,100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImageRectRectI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 200, 125, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 10, 20, 200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, -10, -20, 200, 125, -30, -40, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT, 200, 125, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 0, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, -200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 0, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, -125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Zero source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, -150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, -100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, GDIPLUS_MINFLOAT - 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, GDIPLUS_MININT - 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, GDIPLUS_MAXINT + 1, 0, 100, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif	

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImagePointsRect ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;
	PointF points[] = {
		{10, 20},
		{160, 10},
		{50, 120},
		{100, 150}
	};

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 10, 10, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	PointF zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, zeroPoints, 3, 10, 10, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	PointF negativePoints[] = {
		{100, 50},
		{10, 50},
		{10, 150}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, negativePoints, 3, 10, 10, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 10, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, -20, -30, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative source origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, -150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, -100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage,  points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage,  points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage,  points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0,100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawImagePointsRectI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;
	Point points[] = {
		{10, 20},
		{160, 10},
		{50, 120},
		{100, 150},
		{100, 150}
	};

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 10, 10, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	Point zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, zeroPoints, 3, 10, 10, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	Point negativePoints[] = {
		{100, 50},
		{10, 50},
		{10, 150}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, negativePoints, 3, 10, 10, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 10, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, -20, -30, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative source origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, -150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, -100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage,  points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage,  points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage,  points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, NotImplemented);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, Ok);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ValueOverflow);
#endif

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);
}

static void test_drawLine ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLine (graphics, pen, 10, 20, 30, 40);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLine (graphics, pen, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLine (graphics, pen, 10, 20, -10, -20);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawLine (NULL, pen, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLine (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLine (graphics, pen, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLine (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawLineI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLineI (graphics, pen, 10, 20, 30, 40);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLineI (graphics, pen, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLineI (graphics, pen, 10, 20, -10, -20);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawLineI (NULL, pen, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLineI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLineI (graphics, pen, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLineI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawLines ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	PointF points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLines (graphics, pen, points, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLines (graphics, pen, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLines (graphics, pen, points, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	PointF zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLines (graphics, pen, zeroPoints, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawLines (NULL, pen, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (NULL, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (NULL, pen, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawLines (NULL, pen, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, NULL, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, points, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLines (graphics, pen, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLines (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLines (graphics, NULL, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLines (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLines (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLines (graphics, pen, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawLinesI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	Point points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLinesI (graphics, pen, points, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLinesI (graphics, pen, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLinesI (graphics, pen, points, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	Point zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLinesI (graphics, pen, zeroPoints, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawLinesI (NULL, pen, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (NULL, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (NULL, pen, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawLinesI (NULL, pen, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipDrawLinesI (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, NULL, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawLinesI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipDrawLinesI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipDrawLinesI (graphics, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, points, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLinesI (graphics, pen, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLinesI (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLinesI (graphics, NULL, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLinesI (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawLinesI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipDrawLinesI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipDrawLinesI (graphics, pen, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawLinesI (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawLinesI (graphics, pen, points, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawPath ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	GpPath *emptyPath;
	GpPath *onePath;
	GpPath *twoPath;
	GpPath *threePath;
	GpPath *multiPath;
	GpPath *intersectPath;
	GpPath *alternatePath;
	GpPath *windingPath;
	GpPath *invalidPath;
	PointF points[] = {
		{1, 2},
		{4, 2},
		{4, 6},
	};
	BYTE types[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine
	};
	
	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);
	GdipCreatePath (FillModeWinding, &emptyPath);
	GdipCreatePath2 (points, types, 1, FillModeAlternate, &onePath);
	GdipCreatePath2 (points, types, 2, FillModeAlternate, &twoPath);
	GdipCreatePath2 (points, types, 3, FillModeAlternate, &threePath);
	GdipCreatePath (FillModeWinding, &multiPath);
	GdipAddPathRectangle (multiPath, 10, 20, 30, 40);
	GdipAddPathRectangle (multiPath, 60, 70, 80, 90);
	GdipCreatePath (FillModeWinding, &intersectPath);
	GdipAddPathRectangle (intersectPath, 10, 20, 30, 40);
	GdipAddPathRectangle (intersectPath, 20, 30, 40, 50);
	GdipCreatePath (FillModeAlternate, &alternatePath);
	GdipAddPathRectangle (alternatePath, 10, 20, 30, 40);
	GdipCreatePath (FillModeWinding, &windingPath);
	GdipAddPathRectangle (windingPath, 10, 20, 30, 40);
	GdipCreatePath ((FillMode) (FillModeWinding + 1), &invalidPath);
	GdipAddPathRectangle (invalidPath, 10, 20, 30, 40);

	// FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, alternatePath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, windingPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Invalid FillMode.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, invalidPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Empty path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, emptyPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// One path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, onePath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Two path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, twoPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Three path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, threePath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Multi path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, multiPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Intersect path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, intersectPath);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawPath (NULL, pen, windingPath);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPath (graphics, NULL, windingPath);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPath (graphics, pen, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);
	
	status = GdipDrawPath (graphics, NULL, windingPath);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPath (graphics, pen, NULL);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePath (emptyPath);
	GdipDeletePath (onePath);
	GdipDeletePath (twoPath);
	GdipDeletePath (threePath);
	GdipDeletePath (multiPath);
	GdipDeletePath (intersectPath);
	GdipDeletePath (alternatePath);
	GdipDeletePath (windingPath);
	GdipDeletePath (invalidPath);
	GdipDeletePen (pen);
}

static void test_drawPie ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, -700);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawPie (NULL, pen, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPie (NULL, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, -1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawPieI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, -700);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawPieI (NULL, pen, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPieI (NULL, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, -1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawPolygon ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	PointF points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygon (graphics, pen, points, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygon (graphics, pen, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygon (graphics, pen, points, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	PointF zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygon (graphics, pen, zeroPoints, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawPolygon (NULL, pen, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (NULL, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (NULL, pen, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPolygon (NULL, pen, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, NULL, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, points, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPolygon (graphics, pen, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygon (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygon (graphics, NULL, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygon (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygon (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygon (graphics, pen, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawPolygonI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	Point points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygonI (graphics, pen, points, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygonI (graphics, pen, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygonI (graphics, pen, points, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	Point zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygonI (graphics, pen, zeroPoints, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawPolygonI (NULL, pen, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (NULL, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (NULL, pen, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawPolygonI (NULL, pen, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawPolygonI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipDrawPolygonI (graphics, pen, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, points, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPolygonI (graphics, pen, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawPolygonI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipDrawPolygonI (graphics, pen, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawPolygonI (graphics, pen, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawPolygonI (graphics, pen, points, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawRectangle ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangle (graphics, pen, 10, 20, 30, 40);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangle (graphics, pen, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangle (graphics, pen, 10, 20, -10, -20);
	assertEqualInt (status, Ok);
	
	// Negative tests.
	status = GdipDrawRectangle (NULL, pen, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectangle (graphics, pen, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawRectangle (graphics, pen, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawRectangle (graphics, pen, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawRectangleI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangleI (graphics, pen, 10, 20, 30, 40);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangleI (graphics, pen, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangleI (graphics, pen, 10, 20, -10, -20);
	assertEqualInt (status, Ok);
	
	// Negative tests.
	status = GdipDrawRectangleI (NULL, pen, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectangleI (graphics, pen, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawRectangleI (graphics, pen, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipDrawRectangleI (graphics, pen, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawRectangles ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	RectF rectangles[] = {
		{10, 20, 30, 40},
		{30, 40, 0, 0},
		{100, 150, -20, -30},
		{200, 200, 40, 50}
	};

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 1);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	RectF zeroRectangles[] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, zeroRectangles, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawRectangles (NULL, pen, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (NULL, pen, rectangles, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawRectangles (NULL, pen, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectangles (graphics, pen, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

static void test_drawRectanglesI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;
	Rect rectangles[] = {
		{10, 20, 30, 40},
		{30, 40, 0, 0},
		{100, 150, -20, -30},
		{200, 200, 40, 50}
	};

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 1);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	Rect zeroRectangles[] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, zeroRectangles, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDrawRectanglesI (NULL, pen, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (NULL, pen, rectangles, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipDrawRectanglesI (NULL, pen, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectanglesI (graphics, pen, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_drawArc ();
	test_drawArcI ();
	test_drawBezier ();
	test_drawBezierI ();
	test_drawBeziers ();
	test_drawBeziersI ();
	test_drawCurve ();
	test_drawCurveI ();
	test_drawCurve2 ();
	test_drawCurve2I ();
	test_drawCurve3 ();
	test_drawCurve3I ();
	test_drawClosedCurve ();
	test_drawClosedCurveI ();
	test_drawClosedCurve2 ();
	test_drawClosedCurve2I ();
	test_drawEllipse ();
	test_drawEllipseI ();
	test_drawImage ();
	test_drawImageI ();
	test_drawImageRect ();
	test_drawImageRectI ();
	test_drawImagePoints ();
	test_drawImagePointsI ();
	test_drawImagePointRect ();
	test_drawImagePointRectI ();
	test_drawImageRectRect ();
	test_drawImageRectRectI ();
	test_drawImagePointsRect ();
	test_drawImagePointsRectI ();
	test_drawLine ();
	test_drawLineI ();
	test_drawLines ();
	test_drawLinesI ();
	test_drawPath ();
	test_drawPie ();
	test_drawPieI ();
	test_drawPolygon ();
	test_drawPolygonI ();
	test_drawRectangle ();
	test_drawRectangleI ();
	test_drawRectangles ();
	test_drawRectanglesI ();

	SHUTDOWN;
	return 0;
}
