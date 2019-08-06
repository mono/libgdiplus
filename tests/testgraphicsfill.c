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

static void test_fillClosedCurve ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

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
	status = GdipFillClosedCurve (graphics, brush, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve (graphics, brush, normal7, 6);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve (graphics, brush, normal7, 5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve (graphics, brush, normal7, 4);
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
	status = GdipFillClosedCurve (graphics, brush, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve (graphics, brush, normal7, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve (graphics, brush, normal7, 2);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve (graphics, brush, normal7, 1);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillClosedCurve (NULL, brush, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (NULL, brush, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (NULL, brush, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (NULL, brush, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (NULL, brush, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (NULL, brush, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillClosedCurve (graphics, brush, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve (graphics, brush, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve (graphics, brush, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve (graphics, brush, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, NULL, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve (graphics, brush, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve (graphics, brush, normal7, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillClosedCurveI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

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
	status = GdipFillClosedCurveI (graphics, brush, normal7, 7);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurveI (graphics, brush, normal7, 6);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurveI (graphics, brush, normal7, 5);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurveI (graphics, brush, normal7, 4);
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
	status = GdipFillClosedCurveI (graphics, brush, zero4, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurveI (graphics, brush, normal7, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurveI (graphics, brush, normal7, 2);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1).
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurveI (graphics, brush, normal7, 1);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillClosedCurveI (NULL, brush, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (NULL, brush, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (NULL, brush, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (NULL, brush, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (NULL, brush, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (NULL, brush, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillClosedCurveI (graphics, brush, NULL, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillClosedCurveI (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 3);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 4);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, NULL, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	// FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillClosedCurveI (graphics, brush, NULL, 4);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillClosedCurveI (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 2);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurveI (graphics, brush, normal7, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurveI (graphics, brush, normal7, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillClosedCurve2 ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

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
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 7, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 7, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 7, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 7, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 7, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 7, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 6, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 6, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 6, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 6, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 6, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 6, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 5, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 5, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 5, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 5, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 5, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 5, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, -10, FillModeAlternate);
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
	status = GdipFillClosedCurve2 (graphics, brush, zero4, 4, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, zero4, 4, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, zero4, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, zero4, 4, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, zero4, 4, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillClosedCurve2 (NULL, brush, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 3, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 2, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, -1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, 0, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (NULL, brush, normal7, -1, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, NULL, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, NULL, -1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2 (graphics, brush, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillClosedCurve2I ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

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
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 7, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 7, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 7, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 7, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 7, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (7) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 7, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 6, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 6, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 6, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 6, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 6, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (6) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 6, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 5, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 5, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 5, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 5, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 5, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (5) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 5, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Normal (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, -10, FillModeAlternate);
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
	status = GdipFillClosedCurve2I (graphics, brush, zero4, 4, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, zero4, 4, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, zero4, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, zero4, 4, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// All zero (4) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, zero4, 4, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (3) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (2) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - large tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 10, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - one tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 1, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - normal tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0.5, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - zero tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Small (1) - negative tension.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, -10, FillModeAlternate);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillClosedCurve2I (NULL, brush, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (NULL, brush, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillClosedCurve2I (NULL, brush, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillClosedCurve2I (graphics, brush, NULL, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 3, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 2, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 1, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, -1, 0, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillClosedCurve2I (graphics, brush, NULL, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillClosedCurve2I (graphics, brush, normal7, -1, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 3, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, NULL, normal7, 4, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillClosedCurve2I (graphics, brush, NULL, 4, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 4, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (graphics, brush, NULL, -1, 0, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 2, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0.5, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 1, 0, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 0, 0.5, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, 0, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, -1, 0.5, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	status = GdipFillClosedCurve2I (graphics, brush, normal7, -1, 0, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillEllipse ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Positive, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, 100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, 100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, 100, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, 0, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, 0, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, -100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, -100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipse (graphics, brush, 5, 5, -100, -100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillEllipse (NULL, brush, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillEllipse (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillEllipse (graphics, brush, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillEllipse (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillEllipseI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Positive, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, 100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, 100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, 100, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, 0, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, 0, -100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, -100, 100);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, -100, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative, Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillEllipseI (graphics, brush, 5, 5, -100, -100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillEllipseI (NULL, brush, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillEllipseI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillEllipseI (graphics, brush, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillEllipseI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillPath ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
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

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);
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
	status = GdipFillPath (graphics, brush, alternatePath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, windingPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Invalid FillMode.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, invalidPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Empty path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, emptyPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// One path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, onePath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Two path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, twoPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Three path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, threePath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Multi path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, multiPath);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Intersect path.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPath (graphics, brush, intersectPath);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPath (NULL, brush, windingPath);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPath (graphics, NULL, windingPath);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPath (graphics, brush, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);
	
	status = GdipFillPath (graphics, NULL, windingPath);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPath (graphics, brush, NULL);
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
	GdipDeleteBrush (brush);
}

static void test_fillPie ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 90, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, 0, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPie (graphics, brush, 5, 5, 100, 100, -90, -700);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPie (NULL, brush, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPie (NULL, brush, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, NULL, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, 0, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, -1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, 1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, -1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPie (graphics, brush, 0, 0, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillPie (graphics, brush, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, NULL, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 0, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, -1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 1, 0, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 1, -1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPie (graphics, brush, 0, 0, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillPieI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Positive -> large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Positive -> Negative large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 90, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, 0, -700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, 700);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, 360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, 135);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, -90);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative 360.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, -360);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative -> Negative Large.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPieI (graphics, brush, 5, 5, 100, 100, -90, -700);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPieI (NULL, brush, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPieI (NULL, brush, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, NULL, 0, 0, 1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, 0, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, -1, 1, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, -1, 0, 0, 135);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPieI (graphics, brush, 0, 0, -1, 0, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, NULL, 0, 0, 1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, NULL, 0, 0, 1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 0, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 0, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, -1, 1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, -1, 1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, 0, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, 0, 0, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, -1, 0, 360);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillPieI (graphics, brush, 0, 0, 1, -1, 0, 0);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillPolygon ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
	PointF points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);
	
	// Four - FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 4, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Four - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 4, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Three - FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 3, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 3, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two - FillModeAlternate
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 2, FillModeAlternate);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 2, FillModeWinding);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One - FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 1, FillModeAlternate);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One - FillModeWinding
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, points, 1, FillModeWinding);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero - FillModeAlternate.
	PointF zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, zeroPoints, 4, FillModeAlternate);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon (graphics, brush, zeroPoints, 4, FillModeWinding);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPolygon (NULL, brush, points, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (NULL, brush, points, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (NULL, brush, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPolygon (NULL, brush, points, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (NULL, brush, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, NULL, points, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, NULL, points, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, NULL, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, NULL, points, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, NULL, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, -1, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillPolygon (graphics, brush, points, 3, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon (graphics, NULL, points, 3, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon (graphics, NULL, points, 1, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon (graphics, NULL, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, NULL, points, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, NULL, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, 1, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon (graphics, brush, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, -1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon (graphics, brush, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillPolygonI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
	Point points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Four - FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 4, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Four - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 4, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three - FillModeAlternate
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 3, FillModeAlternate);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 3, FillModeWinding);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two - FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 2, FillModeAlternate);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Two - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 2, FillModeWinding);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One - FillModeAlternate.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 1, FillModeAlternate);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, points, 1, FillModeWinding);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero - FillModeAlternate.
	Point zeroPoints[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, zeroPoints, 4, FillModeAlternate);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	
	// Zero - FillModeWinding.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygonI (graphics, brush, zeroPoints, 4, FillModeWinding);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPolygonI (NULL, brush, points, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (NULL, brush, points, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (NULL, brush, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPolygonI (NULL, brush, points, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygonI (NULL, brush, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, NULL, points, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, NULL, points, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, NULL, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, NULL, points, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygonI (graphics, NULL, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillPolygonI (graphics, brush, NULL, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, NULL, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipFillPolygonI (graphics, brush, NULL, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, NULL, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillPolygonI (graphics, brush, NULL, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillPolygonI (graphics, brush, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, points, 0, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, points, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygonI (graphics, brush, points, -1, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygonI (graphics, brush, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillPolygonI (graphics, brush, points, 3, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygonI (graphics, NULL, points, 3, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygonI (graphics, NULL, points, 1, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygonI (graphics, NULL, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, NULL, points, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillPolygonI (graphics, brush, NULL, 3, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, NULL, 1, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipFillPolygonI (graphics, brush, NULL, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, NULL, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygonI (graphics, brush, points, 1, FillModeAlternate);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygonI (graphics, brush, points, 0, FillModeAlternate);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygonI (graphics, brush, points, -1, FillModeAlternate);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygonI (graphics, brush, points, 4, (FillMode) (FillModeWinding + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillPolygon2 ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
	PointF points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2 (graphics, brush, points, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2 (graphics, brush, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2 (graphics, brush, points, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2 (graphics, brush, points, 2);
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
	status = GdipFillPolygon2 (graphics, brush, zeroPoints, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPolygon2 (NULL, brush, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (NULL, brush, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (NULL, brush, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPolygon2 (NULL, brush, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, NULL, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, points, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillPolygon2 (graphics, brush, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2 (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2 (graphics, NULL, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2 (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, NULL, points, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2 (graphics, brush, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2 (graphics, brush, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillPolygon2I ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
	Point points[] = {
		{10, 20},
		{30, 40},
		{100, 150},
		{200, 200}
	};

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2I (graphics, brush, points, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2I (graphics, brush, points, 3);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2I (graphics, brush, points, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillPolygon2I (graphics, brush, points, 2);
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
	status = GdipFillPolygon2I (graphics, brush, zeroPoints, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillPolygon2I (NULL, brush, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (NULL, brush, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (NULL, brush, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillPolygon2I (NULL, brush, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygon2I (graphics, NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, NULL, points, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillPolygon2I (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipFillPolygon2I (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, brush, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygon2I (graphics, brush, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, brush, points, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillPolygon2I (graphics, brush, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2I (graphics, NULL, points, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2I (graphics, NULL, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2I (graphics, NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillPolygon2I (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipFillPolygon2I (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, brush, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillPolygon2I (graphics, brush, points, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillPolygon2I (graphics, brush, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillPolygon2I (graphics, brush, points, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillRectangle ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangle (graphics, brush, 10, 20, 30, 40);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangle (graphics, brush, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangle (graphics, brush, 10, 20, -10, -20);
	assertEqualInt (status, Ok);
	
	// Negative tests.
	status = GdipFillRectangle (NULL, brush, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillRectangle (graphics, brush, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillRectangle (graphics, brush, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillRectangleI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Positive.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangleI (graphics, brush, 10, 20, 30, 40);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Zero.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangleI (graphics, brush, 0, 0, 0, 0);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Negative.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangleI (graphics, brush, 10, 20, -10, -20);
	assertEqualInt (status, Ok);
	
	// Negative tests.
	status = GdipFillRectangleI (NULL, brush, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillRectangleI (graphics, brush, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillRectangleI (graphics, brush, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipFillRectangleI (graphics, brush, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangle (graphics, NULL, 0, 0, 0, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, -1, 1);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 1, 0);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangleI (graphics, NULL, 0, 0, 1, -1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillRectangles ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
	RectF rectangles[] = {
		{10, 20, 30, 40},
		{30, 40, 0, 0},
		{100, 150, -20, -30},
		{200, 200, 40, 50}
	};

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangles (graphics, brush, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangles (graphics, brush, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangles (graphics, brush, rectangles, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectangles (graphics, brush, rectangles, 1);
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
	status = GdipFillRectangles (graphics, brush, zeroRectangles, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillRectangles (NULL, brush, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (NULL, brush, rectangles, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillRectangles (NULL, brush, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, NULL, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, NULL, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, rectangles, -1);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillRectangles (graphics, brush, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangles (graphics, NULL, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectangles (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, NULL, rectangles, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectangles (graphics, brush, rectangles, -1);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

static void test_fillRectanglesI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpBrush *brush;
	Rect rectangles[] = {
		{10, 20, 30, 40},
		{30, 40, 0, 0},
		{100, 150, -20, -30},
		{200, 200, 40, 50}
	};

	GdipCreateSolidFill (0xFF00FFFF, (GpSolidFill **) &brush);

	// Four.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectanglesI (graphics, brush, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Three.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectanglesI (graphics, brush, rectangles, 4);
	assertEqualInt (status, Ok);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// Two.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectanglesI (graphics, brush, rectangles, 2);
	assertEqualInt (status, Ok);
	
	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// One.
	createImageGraphics (256, 256, &image, &graphics);
	status = GdipFillRectanglesI (graphics, brush, rectangles, 1);
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
	status = GdipFillRectanglesI (graphics, brush, zeroRectangles, 4);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFillRectanglesI (NULL, brush, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (NULL, brush, rectangles, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipFillRectanglesI (NULL, brush, rectangles, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillRectanglesI (graphics, NULL, rectangles, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, NULL, rectangles, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillRectanglesI (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipFillRectanglesI (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, brush, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillRectanglesI (graphics, brush, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, brush, rectangles, -1);
	assertEqualInt (status, OutOfMemory);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFillRectanglesI (graphics, brush, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectanglesI (graphics, NULL, rectangles, 3);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipFillRectanglesI (graphics, NULL, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, NULL, rectangles, -1);
	assertEqualInt (status, OutOfMemory);
	
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipFillRectanglesI (graphics, brush, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, brush, NULL, 1);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipFillRectanglesI (graphics, brush, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, brush, NULL, -1);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipFillRectanglesI (graphics, brush, rectangles, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipFillRectanglesI (graphics, brush, rectangles, -1);
	assertEqualInt (status, OutOfMemory);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteBrush (brush);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_fillClosedCurve ();
	test_fillClosedCurveI ();
	test_fillClosedCurve2 ();
	test_fillClosedCurve2I ();
	test_fillEllipse ();
	test_fillEllipseI ();
	test_fillPath ();
	test_fillPie ();
	test_fillPieI ();
	test_fillPolygon ();
	test_fillPolygonI ();
	test_fillPolygon2 ();
	test_fillPolygon2I ();
	test_fillRectangle ();
	test_fillRectangleI ();
	test_fillRectangles ();
	test_fillRectanglesI ();

	SHUTDOWN;
	return 0;
}
