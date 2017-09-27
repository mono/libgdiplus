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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "testhelpers.h"

static void test_createFromHDC()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphicsFromHdc;
	TextRenderingHint textRenderingHint;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphicsOriginal);
	assert(status == Ok);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	assert (status == Ok);

	status = GdipGetDC (graphicsOriginal, &hdc);
	assert (status == Ok);

	status = GdipCreateFromHDC (0, &graphicsFromHdc);
	assert (status == OutOfMemory);

	status = GdipCreateFromHDC (0, NULL);
	assert (status == InvalidParameter);

	status = GdipCreateFromHDC (hdc, &graphicsFromHdc);
	assert (status == Ok);
	assert(graphicsFromHdc != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphicsFromHdc, &textRenderingHint);
	assert(status == Ok);
	assert(textRenderingHint == TextRenderingHintSystemDefault);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphicsFromHdc, TextRenderingHintSingleBitPerPixelGridFit);
	assert (status == Ok);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	assert(status == Ok);
	assert(textRenderingHint == TextRenderingHintClearTypeGridFit);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphicsFromHdc);
}

static void test_createFromHDC2()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphicsFromHdc;
	TextRenderingHint textRenderingHint;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphicsOriginal);
	assert(status == Ok);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	assert (status == Ok);

	status = GdipGetDC (graphicsOriginal, &hdc);
	assert (status == Ok);

	status = GdipCreateFromHDC2 (0, NULL, &graphicsFromHdc);
	assert (status == OutOfMemory);

	status = GdipCreateFromHDC2 (0, NULL, NULL);
	assert (status == InvalidParameter);

	status = GdipCreateFromHDC2 (hdc, NULL, &graphicsFromHdc);
	assert (status == Ok);
	assert (graphicsFromHdc != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphicsFromHdc, &textRenderingHint);
	assert (status == Ok);
	assert (textRenderingHint == TextRenderingHintSystemDefault);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphicsFromHdc, TextRenderingHintSingleBitPerPixelGridFit);
	assert (status == Ok);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	assert (status == Ok);
	assert (textRenderingHint == TextRenderingHintClearTypeGridFit);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphicsFromHdc);
}

static void test_createFromHWND()
{
	GpStatus status;
	GpGraphics *graphics;

	status = GdipCreateFromHWND (0, NULL);
	assert (status == InvalidParameter);

// Libgdiplus does not implement GdipCreateFromHwnd.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWND (0, &graphics);
	assert (status == Ok);
	assert (graphics != NULL);
#else
  status = GdipCreateFromHWND (0, &graphics);
	assert (status == NotImplemented);
#endif
}

static void test_createFromHWNDICM()
{
	GpStatus status;
	GpGraphics *graphics;

	status = GdipCreateFromHWNDICM (0, NULL);
	assert (status == InvalidParameter);

// Libgdiplus does not implement GdipCreateFromHwndICM.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWNDICM (0, &graphics);
	assert (status == Ok);
	assert (graphics != NULL);
#else
  status = GdipCreateFromHWNDICM (0, &graphics);
	assert (status == NotImplemented);
#endif
}

static void test_hdc ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	HDC hdc;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipGetDC (NULL, &hdc);
	assert (status == InvalidParameter);

	status = GdipGetDC (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipGetDC (graphics, &hdc);
	assert (status == Ok);
	assert(hdc != 0);

	status = GdipGetDC (graphics, &hdc);
	assert (status == ObjectBusy);

	status = GdipReleaseDC (NULL, hdc);
	assert (status == InvalidParameter);

	status = GdipReleaseDC (NULL, 0);
	assert (status == InvalidParameter);

	status = GdipReleaseDC (graphics, 0);
	assert (status == InvalidParameter);

	status = GdipReleaseDC (graphics, hdc);
	assert (status == Ok);

	status = GdipReleaseDC (graphics, hdc);
	assert (status == InvalidParameter);	

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_compositingMode ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingMode mode;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipSetCompositingMode (NULL, CompositingModeSourceCopy);
	assert (status == InvalidParameter);

	status = GdipGetCompositingMode (NULL, &mode);
	assert (status == InvalidParameter);

	status = GdipGetCompositingMode (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetCompositingMode (graphics, (CompositingMode)-1);
	assert (status == Ok);

	status = GdipGetCompositingMode(graphics, &mode);
	assert (status == Ok);
	assert (mode == (CompositingMode)-1);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingMode (graphics, &mode);
	assert (status == ObjectBusy);

	status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_compositingQuality ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingQuality quality;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipGetCompositingQuality(NULL, &quality);
	assert (status == InvalidParameter);

	status = GdipGetCompositingQuality (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetCompositingQuality (NULL, CompositingQualityAssumeLinear);
	assert (status == InvalidParameter);

	status = GdipSetCompositingQuality (graphics, (CompositingQuality)-1);
	assert (status == Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assert (status == Ok);
	assert (quality == (CompositingQuality)-1);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingQuality (graphics, &quality);
	assert (status == ObjectBusy);

	status = GdipSetCompositingQuality(graphics, CompositingQualityAssumeLinear);
	assert(status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_renderingOrigin ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	int x;
	int y;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipGetRenderingOrigin (NULL, &x, &y);
	assert (status == InvalidParameter);

	status = GdipGetRenderingOrigin (graphics, NULL, &y);
	assert (status == InvalidParameter);

	status = GdipGetRenderingOrigin (graphics, &x, NULL);
	assert (status == InvalidParameter);

	status = GdipSetRenderingOrigin(NULL, 0, 0);
	assert(status == InvalidParameter);

	status = GdipSetRenderingOrigin (graphics, 1, 2);
	assert (status == Ok);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	assert (status == Ok);
	assert (x == 1);
	assert (y == 2);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetRenderingOrigin(graphics, &x, &y);
	assert (status == ObjectBusy);

	status = GdipSetRenderingOrigin (graphics, 1, 2);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_textRenderingHint ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipGetTextRenderingHint (NULL, &textRenderingHint);
	assert (status == InvalidParameter);

	status = GdipGetTextRenderingHint (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetTextRenderingHint (NULL, TextRenderingHintAntiAlias);
	assert (status == InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintSystemDefault - 1));
	assert (status == InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintClearTypeGridFit + 1));
	assert (status == InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assert (status == Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assert (status == Ok);
	assert (textRenderingHint == TextRenderingHintClearTypeGridFit);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextRenderingHint(graphics, &textRenderingHint);
	assert (status == ObjectBusy);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_textContrast ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	UINT textContrast;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipGetTextContrast (NULL, &textContrast);
	assert (status == InvalidParameter);

	status = GdipGetTextContrast (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetTextContrast(NULL, 1);
	assert(status == InvalidParameter);

	status = GdipSetTextContrast (graphics, -1);
	assert (status == InvalidParameter);

	status = GdipSetTextContrast (graphics, 13);
	assert (status == InvalidParameter);

	status = GdipSetTextContrast (graphics, 12);
	assert (status == Ok);

	status = GdipGetTextContrast (graphics, &textContrast);
	assert (status == Ok);
	assert (textContrast == 12);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextContrast (graphics, &textContrast);
	assert (status == ObjectBusy);

	status = GdipSetTextContrast (graphics, 1);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_smoothingMode ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	SmoothingMode smoothingMode;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipGetSmoothingMode (NULL, &smoothingMode);
	assert (status == InvalidParameter);

	status = GdipGetSmoothingMode (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetSmoothingMode (NULL, SmoothingModeAntiAlias);
	assert (status == InvalidParameter);

	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	assert (status == InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)(SmoothingModeInvalid - 1));
	assert (status == InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)(SmoothingModeAntiAlias + 1));
	assert (status == InvalidParameter);

	// HighQuality -> AntiAlias
	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assert (status == Ok);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assert (status == Ok);
	assert (smoothingMode == SmoothingModeAntiAlias);

	// Default -> None
	status = GdipSetSmoothingMode (graphics, SmoothingModeDefault);
	assert (status == Ok);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assert (status == Ok);
	assert (smoothingMode == SmoothingModeNone);

	// HighSpeed -> None
	status = GdipSetSmoothingMode(graphics, SmoothingModeHighSpeed);
	assert(status == Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assert(status == Ok);
	assert(smoothingMode == SmoothingModeNone);

	// Other -> Other
	status = GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
	assert(status == Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assert(status == Ok);
	assert(smoothingMode == SmoothingModeAntiAlias);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assert (status == ObjectBusy);

	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pixelOffsetMode ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	PixelOffsetMode pixelOffsetMode;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

	status = GdipGetPixelOffsetMode (NULL, &pixelOffsetMode);
	assert (status == InvalidParameter);

	status = GdipGetPixelOffsetMode (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetPixelOffsetMode(NULL, PixelOffsetModeDefault);
	assert(status == InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeInvalid);
	assert (status == InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeInvalid - 1));
	assert (status == InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeHalf + 1));
	assert (status == InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeHighQuality);
	assert (status == Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assert (status == Ok);
	assert (pixelOffsetMode == PixelOffsetModeHighQuality);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assert (status == Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assert (status == Ok);
	assert (pixelOffsetMode == PixelOffsetModeDefault);

	status = GdipSetPixelOffsetMode(graphics, PixelOffsetModeHighSpeed);
	assert(status == Ok);

	status = GdipGetPixelOffsetMode(graphics, &pixelOffsetMode);
	assert(status == Ok);
	assert(pixelOffsetMode == PixelOffsetModeHighSpeed);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assert (status == ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_interpolationMode ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	InterpolationMode interpolationMode;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);
	
	status = GdipGetInterpolationMode (NULL, &interpolationMode);
	assert (status == InvalidParameter);

// This causes an access violation in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetInterpolationMode(graphics, NULL);
	assert (status == InvalidParameter);
#endif

	status = GdipSetInterpolationMode(NULL, InterpolationModeBicubic);
	assert(status == InvalidParameter);

	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	assert (status == InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeInvalid - 1));
	assert (status == InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeHighQualityBicubic + 1));
	assert (status == InvalidParameter);

	// Default -> Bilinear
	status = GdipSetInterpolationMode (graphics, InterpolationModeDefault);
	assert (status == Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == Ok);
	assert (interpolationMode == InterpolationModeBilinear);

	// HighQuality -> HighQualityBicubic
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQuality);
	assert (status == Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == Ok);
	assert (interpolationMode == InterpolationModeHighQualityBicubic);

	// LowQuality -> NearestNeighbor
	status = GdipSetInterpolationMode(graphics, InterpolationModeLowQuality);
	assert (status == Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == Ok);
	assert (interpolationMode == InterpolationModeBilinear);

	// Other -> Other
	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assert (status == Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == Ok);
	assert (interpolationMode == InterpolationModeBicubic);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_transform ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *matrix;
	GpMatrix *setMatrix;
	GpMatrix *nonInvertibleMatrix;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, &matrix);
	assert (status == Ok);

	status = GdipCreateMatrix2 (146, 66, 158, 104, 42, 150, &setMatrix);
	assert (status == Ok);

	status = GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
	assert (status == Ok);

	status = GdipGetWorldTransform (NULL, matrix);
	assert (status == InvalidParameter);

	status = GdipGetWorldTransform (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetWorldTransform (NULL, matrix);
	assert (status == InvalidParameter);

	status = GdipSetWorldTransform (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetWorldTransform (graphics, nonInvertibleMatrix);
	assert (status == InvalidParameter);

	status = GdipSetWorldTransform (graphics, setMatrix);
	assert (status == Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assert (status == Ok);
	BOOL result;
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	assert (result == 1);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetWorldTransform (graphics, matrix);
	assert (status == ObjectBusy);

	status = GdipSetWorldTransform (graphics, matrix);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (setMatrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pageUnit ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	Unit pageUnit;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipGetPageUnit (NULL, &pageUnit);
	assert (status == InvalidParameter);

	status = GdipGetPageUnit (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetPageUnit (NULL, UnitDisplay);
	assert (status == InvalidParameter);

	status = GdipSetPageUnit (graphics, (Unit)(UnitWorld - 1));
	assert (status == InvalidParameter);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPageUnit (graphics, (Unit)(UnitMillimeter + 1));
	assert (status == InvalidParameter);
#else
	status = GdipSetPageUnit (graphics, (Unit)(UnitCairoPoint + 1));
	assert (status == InvalidParameter);
#endif

	status = GdipSetPageUnit (graphics, UnitWorld);
	assert (status == InvalidParameter);

	status = GdipSetPageUnit (graphics, UnitMillimeter);
	assert (status == Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assert (status == Ok);
	assert (pageUnit == UnitMillimeter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assert (status == ObjectBusy);

	status = GdipSetPageUnit (graphics, UnitDisplay);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pageScale ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL pageScale;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipGetPageScale (NULL, &pageScale);
	assert (status == InvalidParameter);

	status = GdipGetPageScale (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipSetPageScale (NULL, UnitDisplay);
	assert (status == InvalidParameter);

	status = GdipSetPageScale (graphics, -INFINITY);
	assert (status == InvalidParameter);

	status = GdipSetPageScale (graphics, -1);
	assert (status == InvalidParameter);

	status = GdipSetPageScale (graphics, 0);
	assert (status == InvalidParameter);

	status = GdipSetPageScale (graphics, FLT_MAX);
	assert (status == InvalidParameter);

	// 1000000032 appears to be the max value accepted by GDI+.
	status = GdipSetPageScale (graphics, (float)1000000033);
	assert (status == InvalidParameter);

	status = GdipSetPageScale (graphics, 1);
	assert (status == Ok);

	status = GdipGetPageScale (graphics, &pageScale);
	assert (status == Ok);
	assert (pageScale == 1);

	status = GdipSetPageScale (graphics, (float)1000000032);
	assert (status == Ok);

	status = GdipGetPageScale (graphics, &pageScale);
	assert (status == Ok);
	assert (pageScale == 1000000032);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageScale (graphics, &pageScale);
	assert (status == ObjectBusy);

	status = GdipSetPageScale (graphics, 1);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}
static void test_dpiX ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	float dpiX;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipGetDpiX (NULL, &dpiX);
	assert (status == InvalidParameter);

	status = GdipGetDpiX (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipGetDpiX (graphics, &dpiX);
	assert (status == Ok);
	assert (dpiX > 0);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetDpiX (graphics, &dpiX);
	assert (status == ObjectBusy);

	GdipReleaseDC(graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_dpiY ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL dpiY;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipGetDpiY (NULL, &dpiY);
	assert (status == InvalidParameter);

	status = GdipGetDpiY (graphics, NULL);
	assert (status == InvalidParameter);

	status = GdipGetDpiY (graphics, &dpiY);
	assert (status == Ok);
	assert (dpiY > 0);

	HDC hdc;
	GdipGetDC(graphics, &hdc);

	status = GdipGetDpiY (graphics, &dpiY);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_flush ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert (status == Ok);

	status = GdipFlush (NULL, FlushIntentionFlush);
	assert (status == InvalidParameter);

	status = GdipFlush (graphics, FlushIntentionFlush);
	assert (status == Ok);

	status = GdipFlush (graphics, FlushIntentionSync);
	assert (status == Ok);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionFlush - 1));
	assert (status == Ok);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionSync + 1));
	assert (status == Ok);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFlush (graphics, FlushIntentionSync);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_delete ()
{
	WCHAR *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	filePath = createWchar ("test.bmp");
	GdipLoadImageFromFile (filePath, &image);
	GdipGetImageGraphicsContext (image, &graphics);
	
	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDeleteGraphics (graphics);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	status = GdipDeleteGraphics (graphics);
	assert (status == Ok);
	
	status = GdipDeleteGraphics (NULL);
	assert (status == InvalidParameter);

	GdipDisposeImage (image);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

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

	GdiplusShutdown (gdiplusToken);
	return 0;
}
