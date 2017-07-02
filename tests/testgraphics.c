#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#include <GdiPlusFlat.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>

static void test_hdc ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	HDC hdc;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext (image, &graphics);
	assert(status == Ok);

// This causes libgdiplus to crash.
#if 0
	status = GdipGetDC (NULL, &hdc);
	assert (status == InvalidParameter);

	status = GdipGetDC (graphics, NULL);
	assert (status == InvalidParameter);
#endif

	status = GdipGetDC (graphics, &hdc);
	assert (status == Ok);
	assert(hdc != 0);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	status = GdipGetDC (graphics, &hdc);
	assert (status == ObjectBusy);
#endif

	status = GdipReleaseDC (NULL, hdc);
	assert (status == InvalidParameter);

// This causes libgdiplus to crash.
#if 0
	status = GdipReleaseDC (NULL, 0);
	assert (status == InvalidParameter);
#endif

	status = GdipReleaseDC (graphics, 0);
	assert (status == InvalidParameter);

	status = GdipReleaseDC (graphics, hdc);
	assert (status == Ok);

// Libgdiplus does not validate that the graphics has an hdc before releasing.
#if 0
	status = GdipReleaseDC (graphics, hdc);
	assert (status == InvalidParameter);	
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_compositingMode ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingMode mode;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingMode (graphics, &mode);
	assert (status == ObjectBusy);

	status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_compositingQuality ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingQuality quality;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingQuality (graphics, &quality);
	assert (status == ObjectBusy);

	status = GdipSetCompositingQuality(graphics, CompositingQualityAssumeLinear);
	assert(status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_renderingOrigin ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	int x;
	int y;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetRenderingOrigin(graphics, &x, &y);
	assert (status == ObjectBusy);

	status = GdipSetRenderingOrigin (graphics, 1, 2);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_textRenderingHint ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus doesn't validate the TextRenderingHint.
#if 0
	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)-1);
	assert (status == InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)6);
	assert (status == InvalidParameter);

#endif
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assert (status == Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assert (status == Ok);
	assert (textRenderingHint == TextRenderingHintClearTypeGridFit);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextRenderingHint(graphics, &textRenderingHint);
	assert (status == ObjectBusy);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_textContrast ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	unsigned int textContrast;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextContrast (graphics, &textContrast);
	assert (status == ObjectBusy);

	status = GdipSetTextContrast (graphics, 1);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_smoothingMode ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	SmoothingMode smoothingMode;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not validate the SmoothingMode.
#if 0
	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	assert (status == InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)-2);
	assert (status == InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode)6);
	assert (status == InvalidParameter);
#endif

// Libgdiplus does not interpret quality/default/speed.
#if 0
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
#endif

	// Other -> Other
	status = GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
	assert(status == Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assert(status == Ok);
	assert(smoothingMode == SmoothingModeAntiAlias);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assert (status == ObjectBusy);

	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pixelOffsetMode ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	PixelOffsetMode pixelOffsetMode;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not validate PixelOffsetMode.
#if 0
	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)-2);
	assert (status == InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)6);
	assert (status == InvalidParameter);
#endif

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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assert (status == ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_interpolationMode ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	InterpolationMode interpolationMode;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
	status = GdipLoadImageFromFile (filePath, &image);
	assert (status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);
	
	status = GdipGetInterpolationMode (NULL, &interpolationMode);
	assert (status == InvalidParameter);

// This causes an access violation in GDI+.
#if 0
	status = GdipGetInterpolationMode(graphics, NULL);
	assert (status == InvalidParameter);
#endif

	status = GdipSetInterpolationMode(NULL, InterpolationModeBicubic);
	assert(status == InvalidParameter);

#if 0
	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	assert (status == InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)-2);
	assert (status == InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)8);
	assert (status == InvalidParameter);
#endif

// Libgdiplus does not interpret default/quality/speed.
#if 0
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
#endif

	// Other -> Other
	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assert (status == Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == Ok);
	assert (interpolationMode == InterpolationModeBicubic);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assert (status == ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_transform ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *matrix;
	GpMatrix *setMatrix;
	GpMatrix *nonInvertibleMatrix;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetWorldTransform (graphics, matrix);
	assert (status == ObjectBusy);

	status = GdipSetWorldTransform (graphics, matrix);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (setMatrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pageUnit ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	Unit pageUnit;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not validate the Unit.
#if 0
	status = GdipSetPageUnit (graphics, (Unit)-1);
	assert (status == InvalidParameter);

	status = GdipSetPageUnit (graphics, (Unit)7);
	assert (status == InvalidParameter);

	status = GdipSetPageUnit (graphics, UnitWorld);
	assert (status == InvalidParameter);
#endif

	status = GdipSetPageUnit (graphics, UnitMillimeter);
	assert (status == Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assert (status == Ok);
	assert (pageUnit == UnitMillimeter);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assert (status == ObjectBusy);

	status = GdipSetPageUnit (graphics, UnitDisplay);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_pageScale ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	float pageScale;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not validate the page scale.
#if 0
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
#endif

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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageScale (graphics, &pageScale);
	assert (status == ObjectBusy);

	status = GdipSetPageScale (graphics, 1);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}
static void test_dpiX ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	float dpiX;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetDpiX (graphics, &dpiX);
	assert (status == ObjectBusy);

	GdipReleaseDC(graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_dpiY ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	float dpiY;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	GdipGetDC(graphics, &hdc);

	status = GdipGetDpiY (graphics, &dpiY);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_flush ()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	filePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
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

	status = GdipFlush (graphics, (FlushIntention)-1);
	assert (status == Ok);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFlush (graphics, FlushIntention::FlushIntentionSync);
	assert (status == ObjectBusy);

	GdipReleaseDC (graphics, hdc);
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

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

	GdiplusShutdown (gdiplusToken);
	return 0;
}
