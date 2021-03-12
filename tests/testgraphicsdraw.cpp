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

static void createImageGraphics (INT width, INT height, GpImage **image, GpGraphics **graphics)
{
	GpStatus status;

	status = GdipCreateBitmapFromScan0 (width, height, 0, PixelFormat32bppARGB, NULL, (GpBitmap **) image);
	ASSERT_EQ (Ok, status);

	status = GdipGetImageGraphicsContext (*image, graphics);
	ASSERT_EQ (Ok, status);
}

TEST(Graphics, GdipDrawArc)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 90, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, 0, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArc (graphics, pen, 5, 5, 100, 100, -90, -700);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawArc (NULL, pen, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawArc (NULL, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, -1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArc (graphics, pen, 0, 0, 1, -1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawArcI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 90, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, 0, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawArcI (graphics, pen, 5, 5, 100, 100, -90, -700);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawArcI (NULL, pen, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawArcI (NULL, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, -1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawArcI (graphics, pen, 0, 0, 1, -1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawBezier)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Normal.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezier (graphics, pen, 10, 20, 40, 30, 60, 50, 80, 70);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Straight line.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezier (graphics, pen, 10, 20, 30, 40, 50, 60, 70, 80);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezier (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawBezier (NULL, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBezier (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBezier (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBezier (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawBezierI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Normal.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezierI (graphics, pen, 10, 20, 40, 30, 60, 50, 80, 70);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Straight line.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezierI (graphics, pen, 10, 20, 30, 40, 50, 60, 70, 80);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBezierI (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawBezierI (NULL, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBezierI (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBezierI (graphics, pen, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBezierI (graphics, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawBeziers)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 4);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 2);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziers (graphics, pen, normal7, 1);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawBeziers (NULL, pen, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (NULL, pen, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (NULL, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (NULL, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (NULL, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (NULL, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 8);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 6);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBeziers (graphics, pen, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziers (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziers (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawBeziersI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 4);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 2);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawBeziersI (graphics, pen, normal7, 1);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawBeziersI (NULL, pen, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (NULL, pen, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (NULL, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (NULL, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (NULL, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (NULL, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 8);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 6);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawBeziersI (graphics, pen, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawBeziersI (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawCurve)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 6);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 4);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve (graphics, pen, normal7, 2);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawCurve (NULL, pen, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (NULL, pen, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (NULL, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (NULL, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (NULL, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (NULL, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve (graphics, pen, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve (graphics, pen, normal7, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawCurveI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 6);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 4);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurveI (graphics, pen, normal7, 2);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawCurveI (NULL, pen, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (NULL, pen, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (NULL, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (NULL, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (NULL, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (NULL, pen, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurveI (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurveI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurveI (graphics, pen, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, NULL, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	// FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurveI (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurveI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurveI (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurveI (graphics, pen, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawCurve2)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 7, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 6, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 5, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 4, -10);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, zero4, 4, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 3, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2 (graphics, pen, normal7, 2, -10);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawCurve2 (NULL, pen, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (NULL, pen, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2 (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawCurve2I)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 7, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 6, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 5, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 4, -10);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, zero4, 4, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 3, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve2I (graphics, pen, normal7, 2, -10);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawCurve2I (NULL, pen, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (NULL, pen, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 1, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve2I (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawCurve3)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 1, 5, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 2, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 1, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 7, 5, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 0, 5, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 1, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 2, 3, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 1, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 6, 4, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 0, 4, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 1, 3, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 2, 2, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 1, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 5, 3, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 3, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 2, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 1, -10);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 1, 2, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 2, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 1, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, zero4, 4, 2, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 2, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 1, 1, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, -10);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawCurve3 (NULL, pen, normal7, 4, 0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 4, 0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 3, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 3, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 2, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 2, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (NULL, pen, normal7, -1, 0, -2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4,  0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4,  0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 3,  0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 3,  0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 2,  0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 2,  0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, -1, 0, -2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 3, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 3, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 2, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 2, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, -1, 0, -2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, -1, 0, -2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, -2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, -2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, -1, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, -1, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 2, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 2, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 2, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 2, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 1, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 1, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 3, 0, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 2, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, -1, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 4, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, -1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 0, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, NULL, normal7, 4, 2, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 4, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, NULL, 4, 2, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, -1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, -1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 1, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, -1, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 1, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 1, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 0, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, -1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, -1, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, -1, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 4, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, -1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 0, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 1, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3 (graphics, pen, normal7, 4, 2, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawCurve3I)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 1, 5, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 2, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 1, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 7, 5, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 0, 5, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 1, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 2, 3, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 1, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 6, 4, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 0, 4, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 1, 3, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 2, 2, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 1, 2, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 5, 3, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 3, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 2, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 1, -10);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 1, 2, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 2, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 1, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, zero4, 4, 2, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 2, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 1, 1, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 1, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, -10);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawCurve3I (NULL, pen, normal7, 4, 0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 4, 0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 3, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 3, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 2, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 2, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (NULL, pen, normal7, -1, 0, -2, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4,  0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4,  0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 3,  0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 3,  0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 2,  0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 2,  0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, -1, 0, -2, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 3, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 3, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 2, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 2, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurve3I (graphics, pen, NULL, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, -1, 0, -2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, -1, 0, -2, 0);
	ASSERT_EQ (OutOfMemory, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, -2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, -2, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, -1, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, -1, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 2, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 2, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 2, 0, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 2, 0, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 2, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 1, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 1, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 1, 0);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 3, 0, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 2, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, -1, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 4, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, -1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 0, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, NULL, normal7, 4, 2, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, -1, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 4, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 0, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, NULL, 4, 2, 2, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, -1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, -1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 1, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, -1, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 1, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 1, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 0, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -2, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 0, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, -1, 0, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 0, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 1, 0, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 0, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, -1, 0, 1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, -1, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 4, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, -1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 0, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 1, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawCurve3I (graphics, pen, normal7, 4, 2, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawClosedCurve)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 6);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 4);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve (graphics, pen, normal7, 3);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawClosedCurve (NULL, pen, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (NULL, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (NULL, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, NULL, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve (graphics, pen, normal7, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawClosedCurveI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 6);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 4);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurveI (graphics, pen, normal7, 3);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawClosedCurveI (NULL, pen, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (NULL, pen, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurveI (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, NULL, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	// FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurveI (graphics, pen, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawClosedCurveI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurveI (graphics, pen, normal7, -1);
	ASSERT_EQ (OutOfMemory, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawClosedCurve2)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 7, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 6, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 5, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, -10);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, zero4, 4, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, -10);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (NULL, pen, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2 (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawClosedCurve2I)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 7, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 6, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 5, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, -10);
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, zero4, 4, -10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 10);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, -10);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (NULL, pen, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 3, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 3, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 1, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 3, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, NULL, normal7, 4, 0);
	ASSERT_EQ (ObjectBusy, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 4, 0);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, pen, NULL, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 2, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0.5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0.5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0.5);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipDrawClosedCurve2I (graphics, pen, normal7, -1, 0);
	ASSERT_EQ (OutOfMemory, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawEllipse)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 100, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 100, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 100, -100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 0, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, 0, -100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, -100, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, -100, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipse (graphics, pen, 5, 5, -100, -100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawEllipse (NULL, pen, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawEllipse (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawEllipse (graphics, pen, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawEllipse (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawEllipseI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 100, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 100, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 100, -100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 0, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, 0, -100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, -100, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, -100, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawEllipseI (graphics, pen, 5, 5, -100, -100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawEllipseI (NULL, pen, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawEllipseI (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawEllipseI (graphics, pen, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawEllipseI (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
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

TEST(Graphics, GdipDrawImage)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, 10, 20);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, -10, -20);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImage (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawImage (NULL, bitmapImage, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (graphics, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	ASSERT_EQ (ValueOverflow, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImage (graphics, bitmapImage, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImage (graphics, NULL, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImage (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1));
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImage (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1));
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImageI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, 10, 20);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, -1, -1);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawImageI (NULL, bitmapImage, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (NULL, bitmapImage, GDIPLUS_MAXINT + 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (NULL, bitmapImage, GDIPLUS_MININT - 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (NULL, bitmapImage, 0, GDIPLUS_MININT - 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (graphics, NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (graphics, NULL, GDIPLUS_MAXINT + 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (graphics, NULL, GDIPLUS_MININT - 1, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (graphics, NULL, 0, GDIPLUS_MAXINT + 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (graphics, NULL, 0, GDIPLUS_MININT - 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1);
	ASSERT_EQ (ValueOverflow, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageI (graphics, bitmapImage, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageI (graphics, NULL, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImageRect)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 10, 20, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, -10, -20, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, 0, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, -150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, 150, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRect (graphics, bitmapImage, 75, 100, 150, -100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 0, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 150, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (NULL, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, NULL, 0, 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, NULL, 0, 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, NULL, 0, 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImageRectI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 10, 20, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, -10, -20, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, 0, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, -150, 100);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, 150, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectI (graphics, bitmapImage, 75, 100, 150, -100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 0, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 150, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, GDIPLUS_MAXINT + 10, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, GDIPLUS_MININT - 10, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (NULL, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (graphics, NULL, GDIPLUS_MAXINT + 1, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (graphics, NULL, GDIPLUS_MININT - 1, 0, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (graphics, NULL, 0, GDIPLUS_MAXINT + 1, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (graphics, NULL, 0, GDIPLUS_MININT - 1, 150, 100);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 100);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 0);
	ASSERT_EQ (ValueOverflow, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, NULL, 0, 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, NULL, 0, 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, NULL, 0, 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 100);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 150, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImagePoints)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawImagePoints (NULL, bitmapImage, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (NULL, bitmapImage, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 4);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePoints (graphics, bitmapImage, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImagePointsI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (NULL, bitmapImage, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 4);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 3);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 5);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 5);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 4);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 2);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsI (graphics, bitmapImage, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImagePointRect)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 10, 20, 50, 20, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, -10, -20, -30, -40, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative source origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, -150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, -100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitPoint);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitInch);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitDocument);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRect (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitMillimeter);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, NULL, 0, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 0,100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0,100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRect (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImagePointRectI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 10, 20, 50, 20, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, -10, -20, -30, -40, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, -150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, -100, UnitPixel);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitPoint);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitInch);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitDocument);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 75, 100, 50, 20, 150, 100, UnitMillimeter);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (NULL, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, NULL, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, NULL, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, NULL, 0, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 0,100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 0,100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitWorld);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, UnitDisplay);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointRectI (graphics, bitmapImage, 0, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1));
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImageRectRect)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 200, 125, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 10, 20, 200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, -10, -20, 200, 125, -30, -40, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 200, 125, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 0, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, -200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 0, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, -125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Zero source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, -150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, -100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRect (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif	

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);
	
	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 0, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0,100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRect (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImageRectRectI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpImage *bitmapImage;

	GdipLoadImageFromFile (bitmapFile, &bitmapImage);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 200, 125, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 10, 20, 200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, -10, -20, 200, 125, -30, -40, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative  origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT, GDIPLUS_MININT, 200, 125, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 0, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, -200, 125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 0, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative destination height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, -125, 50, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Zero source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, -150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative source height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, -100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 75, 100, 200, 125, 50, 20, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, GDIPLUS_MINFLOAT - 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (NULL, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, GDIPLUS_MININT - 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 0, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, GDIPLUS_MAXINT + 1, 0, 100, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif	

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);
	
	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 0, 125, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, NULL, 0, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MAXINT + 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, GDIPLUS_MININT - 1, 0, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MAXINT + 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, GDIPLUS_MININT - 1, 200, 125, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 0, 200, 125, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImageRectRectI (graphics, bitmapImage, 0, 200, 125, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImagePointsRect)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL) GDIPLUS_MAXFLOAT, (REAL) GDIPLUS_MAXFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 10, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, -20, -30, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative source origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL) GDIPLUS_MINFLOAT, (REAL) GDIPLUS_MINFLOAT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, -150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, -100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage,  points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage,  points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage,  points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, NULL, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, NULL, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 0,100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MAXFLOAT + 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, (REAL)(GDIPLUS_MINFLOAT - 1), 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRect (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawImagePointsRectI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT, GDIPLUS_MAXINT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 10, 20, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, -20, -30, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Max Negative source origin.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT, GDIPLUS_MININT, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative width.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, -150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative height.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, -100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitPoint.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPoint, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitInch.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitInch, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitDocument.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDocument, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// UnitMillimeter.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitMillimeter, NULL, NULL, NULL);
	// FIXME: not implemented in libgdiplus: https://github.com/mono/libgdiplus/issues/428
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage,  points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage,  points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage,  points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (NULL, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (NotImplemented, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// FIXME: libgdiplus does not validate this: https://github.com/mono/libgdiplus/issues/429
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ValueOverflow, status);
#endif

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, NULL, points, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, NULL, 3, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 5, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 4, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 2, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, -1, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 0, 0, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 0,100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MAXINT + 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, GDIPLUS_MININT - 1, 0, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MAXINT + 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 0, 100, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, GDIPLUS_MININT - 1, 150, 0, UnitPixel, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitWorld, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, UnitDisplay, NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawImagePointsRectI (graphics, bitmapImage, points, 3, 0, 0, 150, 100, (Unit) (UnitMillimeter + 1), NULL, NULL, NULL);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDisposeImage (bitmapImage);

	SHUTDOWN
}

TEST(Graphics, GdipDrawLine)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLine (graphics, pen, 10, 20, 30, 40);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLine (graphics, pen, 0, 0, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLine (graphics, pen, 10, 20, -10, -20);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawLine (NULL, pen, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLine (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLine (graphics, pen, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLine (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawLineI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLineI (graphics, pen, 10, 20, 30, 40);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLineI (graphics, pen, 0, 0, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLineI (graphics, pen, 10, 20, -10, -20);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawLineI (NULL, pen, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLineI (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLineI (graphics, pen, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLineI (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawLines)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLines (graphics, pen, points, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLines (graphics, pen, points, 2);
	ASSERT_EQ (Ok, status);
	
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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawLines (NULL, pen, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (NULL, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (NULL, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawLines (NULL, pen, points, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, NULL, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLines (graphics, pen, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLines (graphics, NULL, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLines (graphics, NULL, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLines (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLines (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLines (graphics, pen, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawLinesI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLinesI (graphics, pen, points, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawLinesI (graphics, pen, points, 2);
	ASSERT_EQ (Ok, status);
	
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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawLinesI (NULL, pen, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (NULL, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (NULL, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawLinesI (NULL, pen, points, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawLinesI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
#endif
	
	status = GdipDrawLinesI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	status = GdipDrawLinesI (graphics, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, points, -1);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawLinesI (graphics, pen, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, NULL, points, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawLinesI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
#endif
	
	status = GdipDrawLinesI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	status = GdipDrawLinesI (graphics, pen, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawLinesI (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawLinesI (graphics, pen, points, -1);
	ASSERT_EQ (OutOfMemory, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawPath)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, windingPath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Invalid FillMode.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, invalidPath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Empty path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, emptyPath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// One path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, onePath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Two path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, twoPath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Three path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, threePath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Multi path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, multiPath);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Intersect path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPath (graphics, pen, intersectPath);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawPath (NULL, pen, windingPath);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPath (graphics, NULL, windingPath);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPath (graphics, pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);
	
	status = GdipDrawPath (graphics, NULL, windingPath);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPath (graphics, pen, NULL);
	ASSERT_EQ (ObjectBusy, status);

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

	SHUTDOWN
}

TEST(Graphics, GdipDrawPie)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 90, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, 0, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPie (graphics, pen, 5, 5, 100, 100, -90, -700);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawPie (NULL, pen, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPie (NULL, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, -1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPie (graphics, pen, 0, 0, 1, -1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawPieI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 90, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, 0, -700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 700);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 135);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, -90);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, -360);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPieI (graphics, pen, 5, 5, 100, 100, -90, -700);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawPieI (NULL, pen, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPieI (NULL, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 0, 0, 135);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 0, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 0, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, -1, 1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, 0, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, -1, 0, 360);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawPieI (graphics, pen, 0, 0, 1, -1, 0, 0);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawPolygon)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygon (graphics, pen, points, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygon (graphics, pen, points, 2);
	ASSERT_EQ (Ok, status);
	
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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawPolygon (NULL, pen, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (NULL, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (NULL, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPolygon (NULL, pen, points, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPolygon (graphics, pen, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygon (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygon (graphics, pen, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawPolygonI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygonI (graphics, pen, points, 3);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawPolygonI (graphics, pen, points, 2);
	ASSERT_EQ (Ok, status);
	
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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawPolygonI (NULL, pen, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (NULL, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (NULL, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawPolygonI (NULL, pen, points, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawPolygonI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
#endif
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	status = GdipDrawPolygonI (graphics, pen, points, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, points, -1);
	ASSERT_EQ (OutOfMemory, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawPolygonI (graphics, pen, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, NULL, points, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipDrawPolygonI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
#endif
	
	status = GdipDrawPolygonI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);
	
	status = GdipDrawPolygonI (graphics, pen, points, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawPolygonI (graphics, pen, points, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawPolygonI (graphics, pen, points, -1);
	ASSERT_EQ (OutOfMemory, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawRectangle)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangle (graphics, pen, 10, 20, 30, 40);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangle (graphics, pen, 0, 0, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangle (graphics, pen, 10, 20, -10, -20);
	ASSERT_EQ (Ok, status);
	
	// Negative tests.
	status = GdipDrawRectangle (NULL, pen, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 0, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, -1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectangle (graphics, pen, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawRectangle (graphics, pen, 0, 0, 0, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, -1, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawRectangle (graphics, pen, 0, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 0, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, -1, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 1, -1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawRectangleI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpPen *pen;

	GdipCreatePen1 (0xFF00FF00, 1, UnitPixel, &pen);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangleI (graphics, pen, 10, 20, 30, 40);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangleI (graphics, pen, 0, 0, 0, 0);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangleI (graphics, pen, 10, 20, -10, -20);
	ASSERT_EQ (Ok, status);
	
	// Negative tests.
	status = GdipDrawRectangleI (NULL, pen, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 0, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, -1, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectangleI (graphics, pen, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawRectangleI (graphics, pen, 0, 0, 0, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, -1, 1);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipDrawRectangleI (graphics, pen, 0, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangle (graphics, NULL, 0, 0, 0, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, -1, 1);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, 0);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangleI (graphics, NULL, 0, 0, 1, -1);
	ASSERT_EQ (ObjectBusy, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawRectangles)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 4);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 2);
	ASSERT_EQ (Ok, status);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectangles (graphics, pen, rectangles, 1);
	ASSERT_EQ (Ok, status);
	
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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawRectangles (NULL, pen, rectangles, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (NULL, pen, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawRectangles (NULL, pen, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectangles (graphics, pen, rectangles, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, NULL, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectangles (graphics, pen, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Graphics, GdipDrawRectanglesI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 4);
	ASSERT_EQ (Ok, status);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 2);
	ASSERT_EQ (Ok, status);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 1);
	ASSERT_EQ (Ok, status);
	
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
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDrawRectanglesI (NULL, pen, rectangles, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (NULL, pen, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipDrawRectanglesI (NULL, pen, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDrawRectanglesI (graphics, pen, rectangles, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 3);
	ASSERT_EQ (ObjectBusy, status);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, NULL, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, 0);
	ASSERT_EQ (InvalidParameter, status);
	
	status = GdipDrawRectanglesI (graphics, pen, rectangles, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePen (pen);

	SHUTDOWN
}
