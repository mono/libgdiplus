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

static void test_createFromHDC()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphicsFromHdc;
	TextRenderingHint textRenderingHint;

	graphicsOriginal = getImageGraphics (&image);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetDC (graphicsOriginal, &hdc);
	assertEqualInt (status, Ok);

	status = GdipCreateFromHDC (0, &graphicsFromHdc);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateFromHDC (0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFromHDC (hdc, &graphicsFromHdc);
	assertEqualInt (status, Ok);
	assert (graphicsFromHdc != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphicsFromHdc, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSystemDefault);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphicsFromHdc, TextRenderingHintSingleBitPerPixelGridFit);
	assertEqualInt (status, Ok);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintClearTypeGridFit);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphicsFromHdc);
}

static void test_createFromHDC2()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphicsFromHdc;
	TextRenderingHint textRenderingHint;

	graphicsOriginal = getImageGraphics (&image);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetDC (graphicsOriginal, &hdc);
	assertEqualInt (status, Ok);

	status = GdipCreateFromHDC2 (0, NULL, &graphicsFromHdc);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateFromHDC2 (0, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFromHDC2 (hdc, NULL, &graphicsFromHdc);
	assertEqualInt (status, Ok);
	assert (graphicsFromHdc != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphicsFromHdc, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSystemDefault);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphicsFromHdc, TextRenderingHintSingleBitPerPixelGridFit);
	assertEqualInt (status, Ok);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintClearTypeGridFit);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphicsFromHdc);
}

static void test_createFromHWND()
{
	GpStatus status;
	GpGraphics *graphics;

	status = GdipCreateFromHWND (0, NULL);
	assertEqualInt (status, InvalidParameter);

// Libgdiplus does not implement GdipCreateFromHwnd.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWND (0, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);
#else
  status = GdipCreateFromHWND (0, &graphics);
	assertEqualInt (status, NotImplemented);
#endif
}

static void test_createFromHWNDICM()
{
	GpStatus status;
	GpGraphics *graphics;

	status = GdipCreateFromHWNDICM (0, NULL);
	assertEqualInt (status, InvalidParameter);

// Libgdiplus does not implement GdipCreateFromHwndICM.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWNDICM (0, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);
#else
  status = GdipCreateFromHWNDICM (0, &graphics);
	assertEqualInt (status, NotImplemented);
#endif
}

static void test_hdc ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	HDC hdc;

	graphics = getImageGraphics (&image);

	status = GdipGetDC (NULL, &hdc);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDC (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDC (graphics, &hdc);
	assertEqualInt (status, Ok);
	assert (hdc != 0);

	status = GdipGetDC (graphics, &hdc);
	assertEqualInt (status, ObjectBusy);

	status = GdipReleaseDC (NULL, hdc);
	assertEqualInt (status, InvalidParameter);

	status = GdipReleaseDC (NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipReleaseDC (graphics, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipReleaseDC (graphics, hdc);
	assertEqualInt (status, Ok);

	status = GdipReleaseDC (graphics, hdc);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_compositingMode ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingMode mode;

	graphics = getImageGraphics (&image);

	status = GdipSetCompositingMode (NULL, CompositingModeSourceCopy);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCompositingMode (NULL, &mode);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCompositingMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCompositingMode (graphics, (CompositingMode)-1);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingMode(graphics, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (CompositingMode)-1);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingMode (graphics, &mode);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_compositingQuality ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingQuality quality;

	graphics = getImageGraphics (&image);

	status = GdipGetCompositingQuality(NULL, &quality);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCompositingQuality (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCompositingQuality (NULL, CompositingQualityAssumeLinear);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCompositingQuality (graphics, (CompositingQuality)-1);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, (CompositingQuality)-1);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetCompositingQuality(graphics, CompositingQualityAssumeLinear);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_renderingOrigin ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	int x;
	int y;

	graphics = getImageGraphics (&image);

	status = GdipGetRenderingOrigin (NULL, &x, &y);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRenderingOrigin (graphics, NULL, &y);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRenderingOrigin (graphics, &x, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetRenderingOrigin(NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetRenderingOrigin (graphics, 1, 2);
	assertEqualInt (status, Ok);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	assertEqualInt (status, Ok);
	assertEqualInt (x, 1);
	assertEqualInt (y, 2);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetRenderingOrigin(graphics, &x, &y);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetRenderingOrigin (graphics, 1, 2);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_textRenderingHint ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	graphics = getImageGraphics (&image);

	status = GdipGetTextRenderingHint (NULL, &textRenderingHint);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetTextRenderingHint (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (NULL, TextRenderingHintAntiAlias);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintSystemDefault - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintClearTypeGridFit + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintClearTypeGridFit);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextRenderingHint(graphics, &textRenderingHint);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_textContrast ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	UINT textContrast;

	graphics = getImageGraphics (&image);

	status = GdipGetTextContrast (NULL, &textContrast);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetTextContrast (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextContrast(NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextContrast (graphics, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextContrast (graphics, 13);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextContrast (graphics, 12);
	assertEqualInt (status, Ok);

	status = GdipGetTextContrast (graphics, &textContrast);
	assertEqualInt (status, Ok);
	assertEqualInt (textContrast, 12);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextContrast (graphics, &textContrast);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetTextContrast (graphics, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_smoothingMode ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	SmoothingMode smoothingMode;

	graphics = getImageGraphics (&image);

	status = GdipGetSmoothingMode (NULL, &smoothingMode);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetSmoothingMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (NULL, SmoothingModeAntiAlias);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)(SmoothingModeInvalid - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)6);
	assertEqualInt (status, InvalidParameter);

	// HighQuality -> AntiAlias
	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeAntiAlias);

	// Default -> None
	status = GdipSetSmoothingMode (graphics, SmoothingModeDefault);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeNone);

	// HighSpeed -> None
	status = GdipSetSmoothingMode(graphics, SmoothingModeHighSpeed);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeNone);

	// Other -> Other
	status = GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeAntiAlias);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pixelOffsetMode ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	PixelOffsetMode pixelOffsetMode;

	graphics = getImageGraphics (&image);

	status = GdipGetPixelOffsetMode (NULL, &pixelOffsetMode);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPixelOffsetMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPixelOffsetMode(NULL, PixelOffsetModeDefault);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeInvalid);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeInvalid - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeHalf + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeHighQuality);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeHighQuality);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeDefault);

	status = GdipSetPixelOffsetMode(graphics, PixelOffsetModeHighSpeed);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode(graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeHighSpeed);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_interpolationMode ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	InterpolationMode interpolationMode;

	graphics = getImageGraphics (&image);

	status = GdipGetInterpolationMode (NULL, &interpolationMode);
	assertEqualInt (status, InvalidParameter);

// This causes an access violation in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetInterpolationMode(graphics, NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipSetInterpolationMode(NULL, InterpolationModeBicubic);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeInvalid - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeHighQualityBicubic + 1));
	assertEqualInt (status, InvalidParameter);

	// Default -> Bilinear
	status = GdipSetInterpolationMode (graphics, InterpolationModeDefault);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBilinear);

	// HighQuality -> HighQualityBicubic
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQuality);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeHighQualityBicubic);

	// LowQuality -> NearestNeighbor
	status = GdipSetInterpolationMode(graphics, InterpolationModeLowQuality);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBilinear);

	// Other -> Other
	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBicubic);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_transform ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *matrix;
	GpMatrix *setMatrix;
	GpMatrix *nonInvertibleMatrix;

	graphics = getImageGraphics (&image);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, &matrix);
	assertEqualInt (status, Ok);

	status = GdipCreateMatrix2 (146, 66, 158, 104, 42, 150, &setMatrix);
	assertEqualInt (status, Ok);

	status = GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (NULL, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetWorldTransform (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetWorldTransform (NULL, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetWorldTransform (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetWorldTransform (graphics, nonInvertibleMatrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetWorldTransform (graphics, setMatrix);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	BOOL result;
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	assertEqualInt (result, 1);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetWorldTransform (graphics, matrix);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (setMatrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pageUnit ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	Unit pageUnit;

	graphics = getImageGraphics (&image);

	status = GdipGetPageUnit (NULL, &pageUnit);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPageUnit (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageUnit (NULL, UnitDisplay);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageUnit (graphics, (Unit)(UnitWorld - 1));
	assertEqualInt (status, InvalidParameter);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPageUnit (graphics, (Unit)(UnitMillimeter + 1));
	assertEqualInt (status, InvalidParameter);
#else
	status = GdipSetPageUnit (graphics, (Unit)(UnitCairoPoint + 1));
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipSetPageUnit (graphics, UnitWorld);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageUnit (graphics, UnitMillimeter);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitMillimeter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPageUnit (graphics, UnitDisplay);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pageScale ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL pageScale;

	graphics = getImageGraphics (&image);

	status = GdipGetPageScale (NULL, &pageScale);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPageScale (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (NULL, UnitDisplay);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (graphics, -INFINITY);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (graphics, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (graphics, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (graphics, FLT_MAX);
	assertEqualInt (status, InvalidParameter);

	// 1000000032 appears to be the max value accepted by GDI+.
	status = GdipSetPageScale (graphics, (float)1000000033);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (graphics, 1);
	assertEqualInt (status, Ok);

	status = GdipGetPageScale (graphics, &pageScale);
	assertEqualInt (status, Ok);
	assertEqualFloat (pageScale, 1);

	status = GdipSetPageScale (graphics, (REAL) 1000000032);
	assertEqualInt (status, Ok);

	status = GdipGetPageScale (graphics, &pageScale);
	assertEqualInt (status, Ok);
	assertEqualFloat (pageScale, 1000000032);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageScale (graphics, &pageScale);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPageScale (graphics, 1);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_dpiX ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL dpiX;

	graphics = getImageGraphics (&image);

	status = GdipGetDpiX (NULL, &dpiX);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDpiX (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDpiX (graphics, &dpiX);
	assertEqualInt (status, Ok);
	assert (dpiX > 0);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetDpiX (graphics, &dpiX);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC(graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_dpiY ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL dpiY;

	graphics = getImageGraphics (&image);

	status = GdipGetDpiY (NULL, &dpiY);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDpiY (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDpiY (graphics, &dpiY);
	assertEqualInt (status, Ok);
	assert (dpiY > 0);

	HDC hdc;
	GdipGetDC(graphics, &hdc);

	status = GdipGetDpiY (graphics, &dpiY);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_flush ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	graphics = getImageGraphics (&image);

	status = GdipFlush (NULL, FlushIntentionFlush);
	assertEqualInt (status, InvalidParameter);

	status = GdipFlush (graphics, FlushIntentionFlush);
	assertEqualInt (status, Ok);

	status = GdipFlush (graphics, FlushIntentionSync);
	assertEqualInt (status, Ok);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionFlush - 1));
	assertEqualInt (status, Ok);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionSync + 1));
	assertEqualInt (status, Ok);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFlush (graphics, FlushIntentionSync);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_delete ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	graphics = getImageGraphics (&image);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDeleteGraphics (graphics);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	status = GdipDeleteGraphics (graphics);
	assertEqualInt (status, Ok);

	status = GdipDeleteGraphics (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_rotateWorldTransform ()
{	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRegion *clip;
	GpRectF visibleClipBounds;

	graphics = getImageGraphics (&image);
	GpRectF rect = {0, 0, 32, 32};
	GdipCreateRegionRect (&rect, &clip);

	// With clip region.
	GdipSetClipRegion (graphics, clip, CombineModeReplace);

	status = GdipRotateWorldTransform (graphics, 90, MatrixOrderPrepend);
	assertEqualInt (status, Ok);

	status = GdipGetVisibleClipBounds (graphics, &visibleClipBounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (0, visibleClipBounds.X);
	assertEqualFloat (-32, visibleClipBounds.Y);
	assertEqualFloat (32, visibleClipBounds.Width);
	assertEqualFloat (32, visibleClipBounds.Height);

	// Negative tests.
	status = GdipRotateWorldTransform (NULL, 0, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipRotateWorldTransform (graphics, 0, MatrixOrderAppend);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteRegion (clip);
	GdipDisposeImage (image);
}

static void test_resetClip ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipResetClip (graphics);
	assertEqualInt (status, Ok);

	GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -4194304);
	assertEqualFloat (bounds.Y, -4194304);
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);

	// Negative tests.
	status = GdipResetClip (NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipResetClip (graphics);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_getClip ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRegion *clip;

	GpRectF bounds = {1, 2, 3, 4};
	GdipCreateRegionRect (&bounds, &clip);

	graphics = getImageGraphics (&image);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	// No transform.
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipGetClip (NULL, clip);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetClip (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_getClipBounds ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -4194304);
	assertEqualFloat (bounds.Y, -4194304);
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipGetClipBounds (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetClipBounds (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_getClipBoundsI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRect bounds;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, -4194304);
	assertEqualInt (bounds.Y, -4194304);
	assertEqualInt (bounds.Width, 8388608);
	assertEqualInt (bounds.Height, 8388608);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 20);
	assertEqualInt (bounds.Y, 40);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	// Negative tests.
	status = GdipGetClipBoundsI (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetClipBoundsI (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_getVisibleClipBounds ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 100);
	assertEqualFloat (bounds.Height, 68);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Custom clip - rotated.
	GpRegion *clip;
	GpRectF rect = {0, 0, 32, 32};
	GdipCreateRegionRect (&rect, &clip);

	GdipResetWorldTransform (graphics);
	GdipSetClipRegion (graphics, clip, CombineModeReplace);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 32);
	assertEqualFloat (bounds.Height, 32);

	// Negative tests.
	status = GdipGetVisibleClipBounds (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetVisibleClipBounds (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_getVisibleClipBoundsI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRect bounds;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 100);
	assertEqualInt (bounds.Height, 68);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 20);
	assertEqualInt (bounds.Y, 40);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	// Custom clip - rotated.
	GpRegion *clip;
	GpRectF rect = {0, 0, 32, 32};
	GdipCreateRegionRect (&rect, &clip);

	GdipResetWorldTransform (graphics);
	GdipSetClipRegion (graphics, clip, CombineModeReplace);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 32);
	assertEqualInt (bounds.Height, 32);

	// Negative tests.
	status = GdipGetVisibleClipBoundsI (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetVisibleClipBoundsI (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_isClipEmpty ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	BOOL isEmpty;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, FALSE);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Empty width clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 0, 40, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Empty height clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 0, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, FALSE);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, FALSE);

	// Negative tests.
	status = GdipIsClipEmpty (NULL, &isEmpty);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsClipEmpty (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_isVisibleClipEmpty ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	BOOL isEmpty;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, FALSE);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Empty width clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 0, 40, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Empty height clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 0, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, TRUE);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, FALSE);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, Ok);
	assertEqualInt (isEmpty, FALSE);

	// Negative tests.
	status = GdipIsVisibleClipEmpty (NULL, &isEmpty);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsVisibleClipEmpty (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_setClipGraphics ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpGraphics *otherGraphics;
	GpRegion *clip;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	otherGraphics = getImageGraphics (&image);
	GdipCreateRegion (&clip);

	// No transform.
	GdipSetClipRect (otherGraphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);	

	// Source graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME this is incorrect: https://github.com/mono/libgdiplus/issues/308
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);
#endif

	// Target graphics transformed.
	GdipResetWorldTransform (graphics);
	GdipTranslateWorldTransform (otherGraphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipSetClipGraphics (NULL, otherGraphics, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetClipGraphics (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	status = GdipGetDC (otherGraphics, &hdc);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (otherGraphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteGraphics (otherGraphics);
	GdipDisposeImage (image);
}

static void test_setClipHrgn ()
{
	GpStatus status;
	GpImage *image;
	GpRegion *region;
	GpGraphics *graphics;
	HRGN hrgn;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	GdipGetRegionHRgn (region, graphics, &hrgn);

	// No transform.
	status = GdipSetClipHrgn (graphics, hrgn, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipHrgn (graphics, hrgn, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: no transformation applied.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);
#endif

	// Negative tests.
	status = GdipSetClipHrgn (NULL, hrgn, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetClipHrgn (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipHrgn (graphics, hrgn, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteRegion (region);
}

static void test_setClipRect ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// No transform.
	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipSetClipRect (NULL, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_setClipRectI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// No transform.
	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipSetClipRectI (NULL, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_setClipRegion ()
{
	GpStatus status;
	GpImage *image;
	GpRegion *region;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);

	// No transform.
	status = GdipSetClipRegion (graphics, region, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipRegion (graphics, region, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipSetClipRegion (NULL, region, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetClipRegion (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRegion (graphics, region, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteRegion (region);
	GdipDisposeImage (image);
}

static void test_translateClip ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// Non-infinite.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipTranslateClip (graphics, 0, 0);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative.
	status = GdipTranslateClip (graphics, -10, -20);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Positive.
	status = GdipTranslateClip (graphics, 10, 20);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipTranslateClip (NULL, 10, 20);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipTranslateClip (graphics, 10, 20);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_translateClipI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// Non-infinite.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipTranslateClipI (graphics, 0, 0);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative.
	status = GdipTranslateClipI (graphics, -10, -20);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Positive.
	status = GdipTranslateClipI (graphics, 10, 20);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipTranslateClipI (NULL, 10, 20);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipTranslateClipI (graphics, 10, 20);
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
}

static void test_region_mask()
{
	ARGB color;
	GpStatus status;
	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	GpPath* rectPath = NULL;
	GpPath* polyPath = NULL;
	GpRegion* region = NULL;
	GpSolidFill* brush = NULL;
	static GpPoint polyPoints[] = { { 100, 100 }, { 200,  75 }, { 300, 100 }, { 325, 200 }, { 300, 300 }, { 200, 325 }, { 100, 300 }, {  75, 200 } };

	status = GdipCreateBitmapFromScan0 (400, 400, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);
	status = GdipGetImageGraphicsContext (bitmap, &graphics);
	assertEqualInt (status, Ok);
	GdipGraphicsClear (graphics, 0xFF808080);

	status = GdipCreatePath (FillModeAlternate, &rectPath);
	assertEqualInt (status, Ok);
	GdipAddPathRectangleI (rectPath, 50, 50, 300, 300);

	status = GdipCreatePath (FillModeAlternate, &polyPath);
	assertEqualInt (status, Ok);
	GdipAddPathPolygonI (polyPath, polyPoints, sizeof(polyPoints) / sizeof(polyPoints[0]));

	status = GdipCreateRegion (&region);
	assertEqualInt (status, Ok);
	GdipSetEmpty (region);
	GdipCombineRegionPath (region, rectPath, CombineModeUnion);
	GdipCombineRegionPath (region, polyPath, CombineModeExclude);

	status = GdipCreateSolidFill (0xFF00FF00, &brush);
	assertEqualInt (status, Ok);

	GdipFillRegion (graphics, brush, region);

	status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
	assertEqualInt(status, Ok);
	assertEqualInt(color, 0xFF808080);
	status = GdipBitmapGetPixel (bitmap, 50, 50, &color);
	assertEqualInt(status, Ok);
	assertEqualInt(color, 0xFF00FF00);
	status = GdipBitmapGetPixel (bitmap, 200, 200, &color);
	assertEqualInt(status, Ok);
	assertEqualInt(color, 0xFF808080);

	//GdipSaveImageToFile (bitmap, (const WCHAR*)createWchar( "test-image.png" ), &png_clsid, NULL);

	GdipDeleteGraphics (graphics);
	GdipDeletePath (rectPath);
	GdipDeletePath (polyPath);
	GdipDeleteRegion (region);
	GdipDeleteBrush (brush);
	GdipDisposeImage (bitmap);
}

static void test_premultiplication ()
{
	GpStatus status;
	GpBitmap *bitmap;
	GpBitmap *bitmapBackground;
	GpGraphics *graphicsBackground;

	BYTE bpp32ArgbData[] = { 0xFF, 0xFF, 0xFF, 0x80 };
	ARGB bpp32ArgbPixels[] = { 0x80FFFFFF };
	ARGB bpp32RgbPixels[] = { 0xFF808080 };

	status = GdipCreateBitmapFromScan0 (1, 1, 4, PixelFormat32bppARGB, bpp32ArgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32ArgbPixels);
	status = GdipCreateBitmapFromScan0 (1, 1, 4, PixelFormat32bppRGB, NULL, &bitmapBackground);
	assertEqualInt (status, Ok);
	status = GdipBitmapSetPixel (bitmapBackground, 0, 0, 0);
	assertEqualInt (status, Ok);
	GdipGetImageGraphicsContext (bitmapBackground, &graphicsBackground);
	status = GdipDrawImage (graphicsBackground, (GpImage *)bitmap, 0, 0);
	assertEqualInt (status, Ok);
	GdipDeleteGraphics (graphicsBackground);
	verifyPixels (bitmapBackground, bpp32RgbPixels);
	GdipDisposeImage ((GpImage *) bitmapBackground);
	GdipDisposeImage ((GpImage *) bitmap);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_createFromHDC ();
	test_createFromHDC2 ();
	test_createFromHWND ();
	test_createFromHWNDICM ();
	test_hdc ();
	test_compositingMode ();
	test_compositingQuality ();
	test_renderingOrigin ();
	test_textRenderingHint ();
	test_textContrast ();
	test_smoothingMode ();
	test_pixelOffsetMode ();
	test_interpolationMode ();
	test_transform ();
	test_pageUnit ();
	test_pageScale ();
	test_dpiX ();
	test_dpiY ();
	test_flush ();
	test_delete ();
	test_rotateWorldTransform ();
	test_resetClip ();
	test_getClip ();
	test_getClipBounds ();
	test_getClipBoundsI ();
	test_getVisibleClipBounds ();
	test_getVisibleClipBoundsI ();
	test_isClipEmpty ();
	test_isVisibleClipEmpty ();
	test_setClipGraphics ();
	test_setClipHrgn ();
	test_setClipRect ();
	test_setClipRectI ();
	test_setClipRegion ();
	test_translateClip ();
	test_translateClipI ();
	test_region_mask ();
	test_premultiplication ();

	SHUTDOWN;
	return 0;
}
