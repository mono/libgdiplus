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

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)(SmoothingModeAntiAlias + 1));
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

	SHUTDOWN;
	return 0;
}
