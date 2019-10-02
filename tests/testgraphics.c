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

#if defined(USE_WINDOWS_GDIPLUS)
	HWND hwnd;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
#endif

static void test_createFromHDC()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	graphicsOriginal = getImageGraphics (&image);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetDC (graphicsOriginal, &hdc);
	assertEqualInt (status, Ok);

	status = GdipCreateFromHDC (hdc, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSystemDefault);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixelGridFit);
	assertEqualInt (status, Ok);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintClearTypeGridFit);

#if defined(USE_WINDOWS_GDIPLUS)
	{
		HDC hdc;
		INT x;
		INT y;

		// Window DC.
		hdc = GetDC (hwnd);

		SetViewportOrgEx (hdc, 10, 20, NULL);
		SetWindowOrgEx (hdc, 30, 40, NULL);

		status = GdipCreateFromHDC (hdc, &graphics);
		assertEqualInt (status, Ok);
	
		GdipGetRenderingOrigin (graphics, &x, &y);
		assertEqualInt (10, x);
		assertEqualInt (20, y);

		ReleaseDC (hwnd, hdc);
	}
#endif

	// Negative tests.
	status = GdipCreateFromHDC (0, &graphics);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateFromHDC (0, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphics);
}

static void test_createFromHDC2()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	graphicsOriginal = getImageGraphics (&image);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetDC (graphicsOriginal, &hdc);
	assertEqualInt (status, Ok);

	status = GdipCreateFromHDC2 (hdc, NULL, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSystemDefault);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixelGridFit);
	assertEqualInt (status, Ok);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintClearTypeGridFit);
	
#if defined(USE_WINDOWS_GDIPLUS)
	{
		HDC hdc;
		INT x;
		INT y;

		// Window DC.
		hdc = GetDC (hwnd);

		SetViewportOrgEx (hdc, 10, 20, NULL);
		SetWindowOrgEx (hdc, 30, 40, NULL);

		status = GdipCreateFromHDC2 (hdc, NULL, &graphics);
		assertEqualInt (status, Ok);
	
		GdipGetRenderingOrigin (graphics, &x, &y);
		assertEqualInt (10, x);
		assertEqualInt (20, y);

		ReleaseDC (hwnd, hdc);
	}
#endif

	// Negative tests.
	status = GdipCreateFromHDC2 (0, NULL, &graphics);
	assertEqualInt (status, OutOfMemory);

	status = GdipCreateFromHDC2 (0, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphics);
}

static void test_createFromHWND()
{
	GpStatus status;
	GpGraphics *graphics;
#if defined(USE_WINDOWS_GDIPLUS)
	GpRectF bounds;
#endif

	status = GdipCreateFromHWND (0, NULL);
	assertEqualInt (status, InvalidParameter);

// Libgdiplus does not implement GdipCreateFromHwnd.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWND (0, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);

	GdipDeleteGraphics (graphics);
	
	status = GdipCreateFromHWND (hwnd, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 484);
	assertEqualFloat (bounds.Height, 61);

	GdipDeleteGraphics (graphics);
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
	
	status = GdipCreateFromHWNDICM (hwnd, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics != NULL);

	GdipDeleteGraphics (graphics);
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

	// Negative tests.
	status = GdipGetDC (NULL, &hdc);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDC (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDC (graphics, &hdc);
	assertEqualInt (status, Ok);
	assert (hdc != 0);

	status = GdipGetDC (graphics, &hdc);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetDC (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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
	
	// Default get.
	status = GdipGetCompositingMode(graphics, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, CompositingModeSourceOver);

	// CompositingModeSourceOver.
	status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingMode(graphics, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, CompositingModeSourceOver);

	// CompositingModeSourceOver.
	status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingMode(graphics, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, CompositingModeSourceCopy);

	// Invalid CompositingMode.
	status = GdipSetCompositingMode (graphics, (CompositingMode) (CompositingModeSourceCopy + 1));
	assertEqualInt (status, Ok);

	status = GdipGetCompositingMode(graphics, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (CompositingMode) (CompositingModeSourceCopy + 1));

	// Invalid CompositingMode.
	status = GdipSetCompositingMode (graphics, (CompositingMode) (CompositingModeSourceOver - 1));
	assertEqualInt (status, Ok);

	status = GdipGetCompositingMode(graphics, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (CompositingMode) (CompositingModeSourceOver - 1));

	// Negative tests.
	status = GdipSetCompositingMode (NULL, CompositingModeSourceCopy);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCompositingMode (NULL, &mode);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCompositingMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingMode (graphics, &mode);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetCompositingMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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
	
	// Default get.
	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityDefault);

	// CompositingQualityInvalid.
	status = GdipSetCompositingQuality (graphics, CompositingQualityInvalid);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityInvalid);

	// CompositingQualityDefault.
	status = GdipSetCompositingQuality (graphics, CompositingQualityDefault);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityDefault);

	// CompositingQualityHighSpeed.
	status = GdipSetCompositingQuality (graphics, CompositingQualityHighSpeed);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityHighSpeed);

	// CompositingQualityHighQuality.
	status = GdipSetCompositingQuality (graphics, CompositingQualityHighQuality);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityHighQuality);

	// CompositingQualityGammaCorrected.
	status = GdipSetCompositingQuality (graphics, CompositingQualityGammaCorrected);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityGammaCorrected);

	// CompositingQualityAssumeLinear.
	status = GdipSetCompositingQuality (graphics, CompositingQualityAssumeLinear);
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, CompositingQualityAssumeLinear);

	// Invalid CompositingQuality.
	status = GdipSetCompositingQuality (graphics, (CompositingQuality) (CompositingQualityAssumeLinear + 1));
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, (CompositingQuality) (CompositingQualityAssumeLinear + 1));

	// Invalid CompositingQuality.
	status = GdipSetCompositingQuality (graphics, (CompositingQuality) (CompositingQualityInvalid - 1));
	assertEqualInt (status, Ok);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, Ok);
	assertEqualInt (quality, (CompositingQuality) (CompositingQualityInvalid - 1));

	// Negative tests.
	status = GdipGetCompositingQuality(NULL, &quality);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCompositingQuality (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCompositingQuality (NULL, CompositingQualityAssumeLinear);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingQuality (graphics, &quality);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetCompositingQuality (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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
	
	// Default get.
	status = GdipGetRenderingOrigin (graphics, &x, &y);
	assertEqualInt (status, Ok);
	assertEqualInt (x, 0);
	assertEqualInt (y, 0);

	// Positive.
	status = GdipSetRenderingOrigin (graphics, 1, 2);
	assertEqualInt (status, Ok);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	assertEqualInt (status, Ok);
	assertEqualInt (x, 1);
	assertEqualInt (y, 2);

	// Zero.
	status = GdipSetRenderingOrigin (graphics, 0, 0);
	assertEqualInt (status, Ok);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	assertEqualInt (status, Ok);
	assertEqualInt (x, 0);
	assertEqualInt (y, 0);

	// Negative.
	status = GdipSetRenderingOrigin (graphics, -1, -2);
	assertEqualInt (status, Ok);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	assertEqualInt (status, Ok);
	assertEqualInt (x, -1);
	assertEqualInt (y, -2);

	// Negative tests.
	status = GdipGetRenderingOrigin (NULL, &x, &y);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRenderingOrigin (graphics, NULL, &y);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRenderingOrigin (graphics, &x, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetRenderingOrigin(NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetRenderingOrigin(graphics, &x, &y);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetRenderingOrigin(graphics, NULL, &y);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRenderingOrigin(graphics, &x, NULL);
	assertEqualInt (status, InvalidParameter);

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

	// Default get.
	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSystemDefault);

	// TextRenderingHintSystemDefault.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSystemDefault);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSystemDefault);

	// TextRenderingHintSingleBitPerPixelGridFit.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixelGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSingleBitPerPixelGridFit);

	// TextRenderingHintSingleBitPerPixel.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixel);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintSingleBitPerPixel);

	// TextRenderingHintAntiAliasGridFit.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintAntiAliasGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintAntiAliasGridFit);

	// TextRenderingHintAntiAlias.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintAntiAlias);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintAntiAlias);

	// TextRenderingHintClearTypeGridFit.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, Ok);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	assertEqualInt (status, Ok);
	assertEqualInt (textRenderingHint, TextRenderingHintClearTypeGridFit);

	// Negative tests.
	status = GdipGetTextRenderingHint (NULL, &textRenderingHint);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetTextRenderingHint (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (NULL, TextRenderingHintAntiAlias);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintClearTypeGridFit + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextRenderingHint(graphics, &textRenderingHint);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetTextRenderingHint(graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintClearTypeGridFit + 1));
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

	// Default get.
	status = GdipGetTextContrast (graphics, &textContrast);
	assertEqualInt (status, Ok);
	assertEqualInt (textContrast, 4);

	// Large.
	status = GdipSetTextContrast (graphics, 12);
	assertEqualInt (status, Ok);

	status = GdipGetTextContrast (graphics, &textContrast);
	assertEqualInt (status, Ok);
	assertEqualInt (textContrast, 12);

	// Zero.
	status = GdipSetTextContrast (graphics, 0);
	assertEqualInt (status, Ok);

	status = GdipGetTextContrast (graphics, &textContrast);
	assertEqualInt (status, Ok);
	assertEqualInt (textContrast, 0);

	// Negative tests.
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

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextContrast (graphics, &textContrast);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetTextContrast (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetTextContrast (graphics, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetTextContrast (graphics, 13);
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

	// Default get.
	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeNone);

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

	// HighQuality -> AntiAlias
	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeAntiAlias);

	// SmoothingModeNone.
	status = GdipSetSmoothingMode(graphics, SmoothingModeNone);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeNone);

	// SmoothingModeAntiAlias.
	status = GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, SmoothingModeAntiAlias);

	// SmoothingModeAntiAlias8x8.
	status = GdipSetSmoothingMode(graphics, (SmoothingMode) (SmoothingModeAntiAlias + 1));
	assertEqualInt (status, Ok);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	assertEqualInt (status, Ok);
	assertEqualInt (smoothingMode, (SmoothingMode) (SmoothingModeAntiAlias + 1));

	// Negative tests.
	status = GdipGetSmoothingMode (NULL, &smoothingMode);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetSmoothingMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (NULL, SmoothingModeAntiAlias);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode) (SmoothingModeAntiAlias + 2));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetSmoothingMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode) (SmoothingModeInvalid - 1));
	assertEqualInt (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode) (SmoothingModeAntiAlias + 2));
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

	// Default get.
	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeDefault);

	// PixelOffsetModeDefault.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeDefault);

	// PixelOffsetModeHighSpeed.
	status = GdipSetPixelOffsetMode(graphics, PixelOffsetModeHighSpeed);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode(graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeHighSpeed);

	// PixelOffsetModeHighQuality.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeHighQuality);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeHighQuality);

	// PixelOffsetModeNone.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeNone);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeNone);

	// PixelOffsetModeHalf.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeHalf);
	assertEqualInt (status, Ok);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelOffsetMode, PixelOffsetModeHalf);

	// Negative tests.
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

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetPixelOffsetMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeInvalid - 1));
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeInvalid);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeHalf + 1));
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

	// Default get
	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBilinear);

	// Default -> Bilinear
	status = GdipSetInterpolationMode (graphics, InterpolationModeDefault);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBilinear);

	// LowQuality -> NearestNeighbor
	status = GdipSetInterpolationMode(graphics, InterpolationModeLowQuality);
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

	// InterpolationModeBilinear.
	status = GdipSetInterpolationMode (graphics, InterpolationModeBilinear);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBilinear);

	// InterpolationModeBicubic.
	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeBicubic);

	// InterpolationModeNearestNeighbor.
	status = GdipSetInterpolationMode (graphics, InterpolationModeNearestNeighbor);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeNearestNeighbor);

	// InterpolationModeHighQualityBilinear.
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQualityBilinear);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeHighQualityBilinear);

	// InterpolationModeHighQualityBicubic.
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQualityBicubic);
	assertEqualInt (status, Ok);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, Ok);
	assertEqualInt (interpolationMode, InterpolationModeHighQualityBicubic);

	// Negative tests.
	status = GdipGetInterpolationMode (NULL, &interpolationMode);
	assertEqualInt (status, InvalidParameter);

// This causes an access violation in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetInterpolationMode (graphics, NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipSetInterpolationMode (NULL, InterpolationModeBicubic);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeInvalid - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeHighQualityBicubic + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetInterpolationMode (graphics, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeInvalid - 1));
	assertEqualInt (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeHighQualityBicubic + 1));
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
	
	// Default get.
	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);

	status = GdipSetWorldTransform (graphics, setMatrix);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);

	BOOL result;
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	assertEqualInt (result, 1);

	// Negative tests.
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

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetWorldTransform (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetWorldTransform (graphics, matrix);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetWorldTransform (graphics, NULL);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetWorldTransform (graphics, nonInvertibleMatrix);
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

	// UnitDisplay.
	status = GdipSetPageUnit (graphics, UnitDisplay);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitDisplay);

	// UnitPixel.
	status = GdipSetPageUnit (graphics, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitPixel);

	// UnitPoint.
	status = GdipSetPageUnit (graphics, UnitPoint);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitPoint);

	// UnitInch.
	status = GdipSetPageUnit (graphics, UnitInch);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitInch);

	// UnitDocument.
	status = GdipSetPageUnit (graphics, UnitDocument);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitDocument);

	// UnitMillimeter.
	status = GdipSetPageUnit (graphics, UnitMillimeter);
	assertEqualInt (status, Ok);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, Ok);
	assertEqualInt (pageUnit, UnitMillimeter);

	// Negative tests.
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

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageUnit (graphics, &pageUnit);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetPageUnit (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageUnit (graphics, UnitDisplay);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPageUnit (graphics, UnitWorld);
	assertEqualInt (status, ObjectBusy);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPageUnit (graphics, (Unit)(UnitMillimeter + 1));
	assertEqualInt (status, ObjectBusy);
#else
	status = GdipSetPageUnit (graphics, (Unit)(UnitCairoPoint + 1));
	assertEqualInt (status, ObjectBusy);
#endif

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

	status = GdipSetPageScale (graphics, 1);
	assertEqualInt (status, Ok);

	status = GdipGetPageScale (graphics, &pageScale);
	assertEqualInt (status, Ok);
	assertEqualFloat (pageScale, 1);

	status = GdipSetPageScale (graphics, (REAL) 1000000032);
	assertEqualInt (status, Ok);

	status = GdipGetPageScale (graphics, &pageScale);
	assertEqualInt (status, Ok);
	assertEqualFloat (pageScale, (REAL) 1000000032);

	// Negative tests.
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
	status = GdipSetPageScale (graphics, (REAL) 1000000033);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageScale (graphics, &pageScale);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetPageScale (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPageScale (graphics, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPageScale (graphics, 1);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPageScale (graphics, 0);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetPageScale (graphics, (REAL) 1000000033);
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

	status = GdipGetDpiX (graphics, &dpiX);
	assertEqualInt (status, Ok);
	assert (dpiX > 0);

	// Negative tests.
	status = GdipGetDpiX (NULL, &dpiX);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDpiX (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetDpiX (graphics, &dpiX);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetDpiX (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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

	status = GdipGetDpiY (graphics, &dpiY);
	assertEqualInt (status, Ok);
	assert (dpiY > 0);

	// Negative tests.
	status = GdipGetDpiY (NULL, &dpiY);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetDpiY (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	GdipGetDC(graphics, &hdc);

	status = GdipGetDpiY (graphics, &dpiY);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetDpiY (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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

	status = GdipFlush (graphics, FlushIntentionFlush);
	assertEqualInt (status, Ok);

	status = GdipFlush (graphics, FlushIntentionSync);
	assertEqualInt (status, Ok);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionFlush - 1));
	assertEqualInt (status, Ok);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionSync + 1));
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipFlush (NULL, FlushIntentionFlush);
	assertEqualInt (status, InvalidParameter);

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
{
	GpStatus status;
	GpMatrix *transform;
	GpMatrix *matrix;
	GpImage *image;
	GpGraphics *graphics;
	GpRegion *clip;
	GpRectF visibleClipBounds;

	GpRectF rect = {0, 0, 32, 32};
	GdipCreateRegionRect (&rect, &clip);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);
	GdipCreateMatrix (&matrix);

	// MatrixOrderPrepend.
	graphics = getImageGraphics (&image);

	GdipSetWorldTransform (graphics, transform);
	status = GdipRotateWorldTransform (graphics, 90, MatrixOrderPrepend);

	GdipGetWorldTransform (graphics, matrix);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// MatrixOrderAppend.
	graphics = getImageGraphics (&image);

	GdipSetWorldTransform (graphics, transform);
	status = GdipRotateWorldTransform (graphics, 90, MatrixOrderAppend);

	GdipGetWorldTransform (graphics, matrix);
	verifyMatrix (matrix, -2, 1, -4, 3, -6, 5);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	// With clip region.
	graphics = getImageGraphics (&image);
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
	
	status = GdipRotateWorldTransform (graphics, 0, (MatrixOrder) (MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipRotateWorldTransform (graphics, 0, MatrixOrderAppend);
	assertEqualInt (status, ObjectBusy);

	status = GdipRotateWorldTransform (graphics, 0, (MatrixOrder) (MatrixOrderAppend + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteRegion (clip);
	GdipDeleteMatrix (transform);
	GdipDeleteMatrix (matrix);
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
	GpMatrix *transform;
	GpRegion *clip;

	GpRectF bounds = {1, 2, 3, 4};
	GdipCreateRegionRect (&bounds, &clip);

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);

	// Default get - no transform.
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);
	
	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, -4194304);
	assertEqualFloat (bounds.Y, -4194304);
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);
	
	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, -4194304);
	assertEqualFloat (bounds.Y, -4194304);
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);
	
	GdipResetWorldTransform (graphics);
	
	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, -4194304);
	assertEqualFloat (bounds.Y, -4194304);
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);
	
	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Empty clip - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, Ok);

	GdipGetRegionBounds (clip, graphics, &bounds);
	assertEqualFloat (bounds.X, -49);
	assertEqualFloat (bounds.Y, -22);
	assertEqualFloat (bounds.Width, 120);
	assertEqualFloat (bounds.Height, 50);

	// Negative tests.
	status = GdipGetClip (NULL, clip);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetClip (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClip (graphics, clip);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetClip (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
}

static void test_getClipBounds ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *transform;
	GpRegion *emptyRegion;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	// Default get - no transform.
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -4194304);
	assertEqualFloat (bounds.Y, -4194304);
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);

	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (bounds.X, -4194294);
	assertEqualFloat (bounds.Y, -4194284);
#endif
	assertEqualFloat (bounds.Width, 8388608);
	assertEqualFloat (bounds.Height, 8388608);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (bounds.X, -14680063);
	assertEqualFloat (bounds.Y, -6291458);
	assertEqualFloat (bounds.Width, 29360128);
	assertEqualFloat (bounds.Height, 12582912);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Empty clip - translate transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 1);
	assertEqualFloat (bounds.Y, -2);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip rect - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Empty clip rect - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip rect - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 1);
	assertEqualFloat (bounds.Y, -2);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -49);
	assertEqualFloat (bounds.Y, -22);
	assertEqualFloat (bounds.Width, 120);
	assertEqualFloat (bounds.Height, 50);

	// Negative tests.
	status = GdipGetClipBounds (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetClipBounds (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetClipBounds (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
	GdipDeleteRegion (emptyRegion);
}

static void test_getClipBoundsI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *transform;
	GpRegion *emptyRegion;
	GpRect bounds;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);
	
	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	// Default get - no transform.
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, -4194304);
	assertEqualInt (bounds.Y, -4194304);
	assertEqualInt (bounds.Width, 8388608);
	assertEqualInt (bounds.Height, 8388608);
	
	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (bounds.X, -4194294);
	assertEqualInt (bounds.Y, -4194284);
#endif
	assertEqualInt (bounds.Width, 8388608);
	assertEqualInt (bounds.Height, 8388608);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (bounds.X, -14680063);
	assertEqualInt (bounds.Y, -6291458);
	assertEqualInt (bounds.Width, 29360128);
	assertEqualInt (bounds.Height, 12582912);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Empty clip - translate transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 1);
	assertEqualInt (bounds.Y, -2);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip rect - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Empty clip rect - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip rect - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 1);
	assertEqualInt (bounds.Y, -2);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 20);
	assertEqualInt (bounds.Y, 40);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, -49);
	assertEqualInt (bounds.Y, -22);
	assertEqualInt (bounds.Width, 120);
	assertEqualInt (bounds.Height, 50);

	// Negative tests.
	status = GdipGetClipBoundsI (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetClipBoundsI (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClipBoundsI (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetClipBoundsI (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
	GdipDeleteRegion (emptyRegion);
}

static void test_getVisibleClipBounds ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *transform;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);

	// Default get - no transform.
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 100);
	assertEqualFloat (bounds.Height, 68);
	
	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 100);
	assertEqualFloat (bounds.Height, 68);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (bounds.X, -199);
	assertEqualFloat (bounds.Y, -36);
	assertEqualFloat (bounds.Width, 302);
	assertEqualFloat (bounds.Height, 134);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Empty clip - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 1);
	assertEqualFloat (bounds.Y, -2);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 20);
	assertEqualFloat (bounds.Y, 40);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);
	
	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (bounds.X, -49);
	assertEqualFloat (bounds.Y, -22);
	assertEqualFloat (bounds.Width, 120);
	assertEqualFloat (bounds.Height, 50);
#endif

	GdipResetWorldTransform (graphics);

	// Negative tests.
	status = GdipGetVisibleClipBounds (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetVisibleClipBounds (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetVisibleClipBounds (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
}

static void test_getVisibleClipBoundsI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *transform;
	GpRect bounds;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);

	// Default get - no transform
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 100);
	assertEqualInt (bounds.Height, 68);
	
	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 100);
	assertEqualInt (bounds.Height, 68);

	GdipResetWorldTransform (graphics);
	
	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (bounds.X, -199);
	assertEqualInt (bounds.Y, -36);
	assertEqualInt (bounds.Width, 302);
	assertEqualInt (bounds.Height, 134);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 0);
	assertEqualInt (bounds.Y, 0);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);

	// Empty clip - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 1);
	assertEqualInt (bounds.Y, -2);
	assertEqualInt (bounds.Width, 0);
	assertEqualInt (bounds.Height, 0);
	
	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 10);
	assertEqualInt (bounds.Y, 20);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualInt (bounds.X, 20);
	assertEqualInt (bounds.Y, 40);
	assertEqualInt (bounds.Width, 30);
	assertEqualInt (bounds.Height, 40);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, Ok);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (bounds.X, -49);
	assertEqualInt (bounds.Y, -22);
	assertEqualInt (bounds.Width, 120);
	assertEqualInt (bounds.Height, 50);
#endif

	// Negative tests.
	status = GdipGetVisibleClipBoundsI (NULL, &bounds);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetVisibleClipBoundsI (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	assertEqualInt (status, ObjectBusy);

	status = GdipGetVisibleClipBoundsI (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
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

	status = GdipIsClipEmpty (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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

	status = GdipIsVisibleClipEmpty (graphics, NULL);
	assertEqualInt (status, InvalidParameter);

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

	status = GdipSetClipGraphics (graphics, otherGraphics, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipGraphics (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipGraphics (graphics, NULL, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	status = GdipGetDC (otherGraphics, &hdc);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipGraphics (NULL, otherGraphics, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetClipGraphics (graphics, otherGraphics, (CombineMode) (CombineModeComplement + 1));
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

	status = GdipSetClipHrgn (graphics, hrgn, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipHrgn (graphics, hrgn, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipHrgn (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipHrgn (graphics, hrgn, (CombineMode) (CombineModeComplement + 1));
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
	UINT scansCount;
	GpRegion *region;
	GpMatrix *matrix;

	graphics = getImageGraphics (&image);
	GdipCreateRegion (&region);
	GdipCreateMatrix (&matrix);

	// Positive width, positive height.
	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Positive width, zero height.
	status = GdipSetClipRect (graphics, 10, 20, 30, 0, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Positive width, negative height.
	status = GdipSetClipRect (graphics, 10, 20, 30, -40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 1);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, -20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);
	
	// Zero width, positive height.
	status = GdipSetClipRect (graphics, 10, 20, 0, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	// Zero width, zero height.
	status = GdipSetClipRect (graphics, 10, 20, 0, 0, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	// Zero width, negative height.
	status = GdipSetClipRect (graphics, 10, 20, 0, -40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	// Negative width, positive height.
	status = GdipSetClipRect (graphics, 10, 20, -30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 1);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -20);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative width, zero height.
	status = GdipSetClipRect (graphics, 10, 20, -30, 0, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Negative width, negative height.
	status = GdipSetClipRect (graphics, 10, 20, -30, -40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 1);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -20);
	assertEqualFloat (bounds.Y, -20);
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

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteRegion (region);
	GdipDeleteMatrix (matrix);
}

static void test_setClipRectI ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;
	UINT scansCount;
	GpRegion *region;
	GpMatrix *matrix;

	graphics = getImageGraphics (&image);
	GdipCreateRegion (&region);
	GdipCreateMatrix (&matrix);

	// Positive width, positive height.
	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Positive width, zero height.
	status = GdipSetClipRectI (graphics, 10, 20, 30, 0, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Positive width, negative height.
	status = GdipSetClipRectI (graphics, 10, 20, 30, -40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 1);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, -20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);
	
	// Zero width, positive height.
	status = GdipSetClipRectI (graphics, 10, 20, 0, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	// Zero width, zero height.
	status = GdipSetClipRectI (graphics, 10, 20, 0, 0, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	// Zero width, negative height.
	status = GdipSetClipRectI (graphics, 10, 20, 0, -40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);
	
	// Negative width, positive height.
	status = GdipSetClipRectI (graphics, 10, 20, -30, 40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 1);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -20);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative width, zero height.
	status = GdipSetClipRectI (graphics, 10, 20, -30, 0, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 0);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 0);
	assertEqualFloat (bounds.Y, 0);
	assertEqualFloat (bounds.Width, 0);
	assertEqualFloat (bounds.Height, 0);

	// Negative width, negative height.
	status = GdipSetClipRectI (graphics, 10, 20, -30, -40, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClip (graphics, region);
	assertEqualInt (status, Ok);
	
	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scansCount, 1);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, -20);
	assertEqualFloat (bounds.Y, -20);
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

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteRegion (region);
	GdipDeleteMatrix (matrix);
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

	status = GdipSetClipRegion (graphics, region, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRegion (graphics, region, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipRegion (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipRegion (graphics, region, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteRegion (region);
	GdipDisposeImage (image);
}

static void test_setClipPath ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;
	GpPath *path;

	graphics = getImageGraphics (&image);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);

	// No transform.
	status = GdipSetClipPath (graphics, path, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipPath (graphics, path, CombineModeReplace);
	assertEqualInt (status, Ok);

	status = GdipGetClipBounds (graphics, &bounds);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, 10);
	assertEqualFloat (bounds.Y, 20);
	assertEqualFloat (bounds.Width, 30);
	assertEqualFloat (bounds.Height, 40);

	// Negative tests.
	status = GdipSetClipPath (NULL, path, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipSetClipPath (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetClipPath (graphics, path, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, InvalidParameter);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipPath (graphics, path, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);
	
	status = GdipSetClipPath (graphics, NULL, CombineModeReplace);
	assertEqualInt (status, ObjectBusy);

	status = GdipSetClipPath (graphics, path, (CombineMode) (CombineModeComplement + 1));
	assertEqualInt (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePath (path);
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
	ARGB bpp32RgbPixelsPre[] = { 0xFF000000 };
	ARGB bpp32RgbPixels[] = { 0xFF808080 };

	status = GdipCreateBitmapFromScan0 (1, 1, 4, PixelFormat32bppARGB, bpp32ArgbData, &bitmap);
	assertEqualInt (status, Ok);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32ArgbPixels);
	status = GdipCreateBitmapFromScan0 (1, 1, 4, PixelFormat32bppRGB, NULL, &bitmapBackground);
	assertEqualInt (status, Ok);
	status = GdipBitmapSetPixel (bitmapBackground, 0, 0, 0);
	assertEqualInt (status, Ok);
	verifyPixels (bitmapBackground, bpp32RgbPixelsPre);
	GdipGetImageGraphicsContext (bitmapBackground, &graphicsBackground);
	status = GdipDrawImage (graphicsBackground, (GpImage *)bitmap, 0, 0);
	assertEqualInt (status, Ok);
	GdipDeleteGraphics (graphicsBackground);
	verifyPixels (bitmapBackground, bpp32RgbPixels);
	GdipDisposeImage ((GpImage *) bitmapBackground);
	GdipDisposeImage ((GpImage *) bitmap);
}

static void test_world_transform_in_container ()
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GraphicsContainer state;
	GpMatrix *matrix;
	GpMatrix *setMatrix;

	graphics = getImageGraphics (&image);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, &matrix);
	assertEqualInt (status, Ok);

	status = GdipCreateMatrix2 (2, 0, 0, 2, 10, 20, &setMatrix);
	assertEqualInt (status, Ok);

	status = GdipSetWorldTransform (graphics, setMatrix);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	BOOL result;
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	assertEqualInt (result, 1);

	status = GdipBeginContainer2 (graphics, &state);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	GdipIsMatrixIdentity (matrix, &result);
	assertEqualInt (result, 1);

	status = GdipSetWorldTransform (graphics, setMatrix);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	assertEqualInt (result, 1);

	status = GdipResetWorldTransform (graphics);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	GdipIsMatrixIdentity (matrix, &result);
	assertEqualInt (result, 1);

	status = GdipEndContainer (graphics, state);
	assertEqualInt (status, Ok);

	status = GdipGetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	assertEqualInt (result, 1);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (setMatrix);
	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

static void test_world_transform_respects_page_unit_document ()
{
	GpStatus status;
	GpBitmap *bitmap;
	GpGraphics *graphics;
	GpMatrix *matrix;
	GpSolidFill *brush;

	// Test Graphics with UnitDocument and 72 dpi
	status = GdipCreateBitmapFromScan0 (612, 792, 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetResolution (bitmap, 72, 72);
	assertEqualInt (status, Ok);

	status = GdipGetImageGraphicsContext (bitmap, &graphics);
	assertEqualInt (status, Ok);

	status = GdipSetPageUnit (graphics, UnitDocument);
	assertEqualInt (status, Ok);

	const ARGB FillColor = 0xFF000000;
	status = GdipCreateSolidFill (FillColor, &brush);
	assertEqualInt (status, Ok);

	int rectX = 360;
	int rectY = 111;
	GdipCreateMatrix2 (1, 0, 0, 1, rectX, rectY, &matrix);
	status = GdipSetWorldTransform (graphics, matrix);
	assertEqualInt (status, Ok);

	int rectWidth = 74;
	int rectHeight = 72;
	status = GdipFillRectangleI (graphics, brush, 0, 0, rectWidth, rectHeight);
	assertEqualInt (status, Ok);

	//CLSID png_clsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	//WCHAR *filePath = createWchar ("test_world_transform_respects_page_unit_document.png");
	//status = GdipSaveImageToFile (bitmap, filePath, &png_clsid, NULL);

	int points[] = {
		rectX + 2,
		rectY + 2,
		rectX + rectWidth / 2,
		rectY + rectHeight / 2,
		rectX + rectWidth - 1,
		rectY + rectHeight - 1
	};
	for (int i = 0; i < sizeof (points) / sizeof (points[0]); i += 2) {
		ARGB color;
		status = GdipBitmapGetPixel (bitmap, (int)(points[i] * 72 / 300.0f), (int)(points[i + 1] * 72 / 300.0f), &color);
		assertEqualInt (status, Ok);
		assertEqualInt (color, FillColor);
	}

	GdipDisposeImage ((GpImage *)bitmap);
	GdipDeleteGraphics (graphics);
	GdipDeleteBrush (brush);
}

static void test_world_transform_respects_page_unit_point ()
{
	GpStatus status;
	GpBitmap *bitmap;
	GpGraphics *graphics;
	GpSolidFill *brush;
	const float TargetResolution = 300;
	double scaleFactor = TargetResolution / 72;

	// Test Graphics with UnitPoint and 72 dpi
	status = GdipCreateBitmapFromScan0 ((int)(612 * scaleFactor), (int)(792 * scaleFactor), 0, PixelFormat32bppARGB, NULL, &bitmap);
	assertEqualInt (status, Ok);

	status = GdipBitmapSetResolution (bitmap, TargetResolution, TargetResolution);
	assertEqualInt (status, Ok);

	status = GdipGetImageGraphicsContext (bitmap, &graphics);
	assertEqualInt (status, Ok);

	status = GdipSetPageUnit (graphics, UnitPoint);
	assertEqualInt (status, Ok);

	const ARGB FillColor = 0xFF000000;
	status = GdipCreateSolidFill (FillColor, &brush);
	assertEqualInt (status, Ok);

	int rectX = 360;
	int rectY = 111;
	int rectWidth = 74;
	int rectHeight = 72;
	status = GdipTranslateWorldTransform (graphics, rectX, rectY, MatrixOrderPrepend);
	assertEqualInt (status, Ok);

	status = GdipFillRectangleI (graphics, brush, 0, 0, rectWidth, rectHeight);
	assertEqualInt (status, Ok);

	//CLSID png_clsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	//WCHAR *filePath = createWchar ("test_world_transform_respects_page_unit_point.png");
	//status = GdipSaveImageToFile (bitmap, filePath, &png_clsid, NULL);

	int points[] = {
		rectX,
		rectY,
		rectX + rectWidth / 2,
		rectY + rectHeight / 2,
		rectX + rectWidth - 1,
		rectY + rectHeight - 1
	};
	for (int i = 0; i < sizeof (points) / sizeof (points[0]); i += 2) {
		ARGB color;
		status = GdipBitmapGetPixel (bitmap, points[i], points[i + 1], &color);
		assertEqualInt (status, Ok);
		assertEqualInt (color, 0);

		status = GdipBitmapGetPixel (bitmap, (int)(points[i] * scaleFactor) + 1, (int)(points[i + 1] * scaleFactor) + 1, &color);
		assertEqualInt (status, Ok);
		assertEqualInt (color, FillColor);
	}

	GdipDisposeImage ((GpImage *)bitmap);
	GdipDeleteGraphics (graphics);
	GdipDeleteBrush (brush);
}

int
main (int argc, char**argv)
{
	STARTUP;
	
#if defined(USE_WINDOWS_GDIPLUS)
	WNDCLASS wc;
	HINSTANCE hInstance = GetModuleHandle (NULL);

	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"Window_Class";

	RegisterClass (&wc);
	hwnd = CreateWindow (L"Window_Class", L"Window_Title", WS_OVERLAPPEDWINDOW, 20, 30, 500, 100, NULL, NULL, hInstance, NULL);
#endif

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
	test_setClipPath ();
	test_translateClip ();
	test_translateClipI ();
	test_region_mask ();
	test_premultiplication ();
	test_world_transform_in_container ();
	test_world_transform_respects_page_unit_document ();
	test_world_transform_respects_page_unit_point ();

#if defined(USE_WINDOWS_GDIPLUS)
	DestroyWindow (hwnd);
#endif

	SHUTDOWN;
	return 0;
}
