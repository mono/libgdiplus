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

static GpGraphics *getImageGraphics (GpImage **image)
{
	GpStatus status;
	WCHAR *filePath;
	GpGraphics *graphics;

	filePath = createWchar ("test.bmp");
	status = GdipLoadImageFromFile (filePath, image);
	assert (status == Ok);

	freeWchar (filePath);

	status = GdipGetImageGraphicsContext (*image, &graphics);
	assert (status == Ok);

	return graphics;
}

#if defined(USE_WINDOWS_GDIPLUS)
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

TEST(Graphics, GdipCreateFromHDC)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	graphicsOriginal = getImageGraphics (&image);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	ASSERT_EQ (Ok, status);

	status = GdipGetDC (graphicsOriginal, &hdc);
	ASSERT_EQ (Ok, status);

	status = GdipCreateFromHDC (hdc, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintSystemDefault, textRenderingHint);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixelGridFit);
	ASSERT_EQ (Ok, status);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintClearTypeGridFit, textRenderingHint);

#if defined(USE_WINDOWS_GDIPLUS)
	{
		HWND hwnd;
		HDC hdc;
		INT x;
		INT y;

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

		// Window DC.
		hdc = GetDC (hwnd);

		SetViewportOrgEx (hdc, 10, 20, NULL);
		SetWindowOrgEx (hdc, 30, 40, NULL);

		status = GdipCreateFromHDC (hdc, &graphics);
		ASSERT_EQ (Ok, status);

		GdipGetRenderingOrigin (graphics, &x, &y);
		ASSERT_EQ (x, 10);
		ASSERT_EQ (y, 20);

		ReleaseDC (hwnd, hdc);
		DestroyWindow (hwnd);
	}
#endif

	// Negative tests.
	status = GdipCreateFromHDC (0, &graphics);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipCreateFromHDC (0, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, GdipCreateFromHDC2)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphicsOriginal;
	HDC hdc;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	graphicsOriginal = getImageGraphics (&image);

	status = GdipSetTextRenderingHint (graphicsOriginal, TextRenderingHintClearTypeGridFit);
	ASSERT_EQ (Ok, status);

	status = GdipGetDC (graphicsOriginal, &hdc);
	ASSERT_EQ (Ok, status);

	status = GdipCreateFromHDC2 (hdc, NULL, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics != NULL);

	// The graphics from the HDC should not have the same values as the original graphics.
	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintSystemDefault, textRenderingHint);

	// Modifying the graphics from the HDC should not modify the original graphics.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixelGridFit);
	ASSERT_EQ (Ok, status);

	GdipReleaseDC (graphicsOriginal, hdc);

	status = GdipGetTextRenderingHint (graphicsOriginal, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintClearTypeGridFit, textRenderingHint);

#if defined(USE_WINDOWS_GDIPLUS)
	{
		HWND hwnd;
		HDC hdc;
		INT x;
		INT y;

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

		// Window DC.
		hdc = GetDC (hwnd);

		SetViewportOrgEx (hdc, 10, 20, NULL);
		SetWindowOrgEx (hdc, 30, 40, NULL);

		status = GdipCreateFromHDC2 (hdc, NULL, &graphics);
		ASSERT_EQ (Ok, status);

		GdipGetRenderingOrigin (graphics, &x, &y);
		ASSERT_EQ (x, 10);
		ASSERT_EQ (y, 20);

		ReleaseDC (hwnd, hdc);
		DestroyWindow (hwnd);
	}
#endif

	// Negative tests.
	status = GdipCreateFromHDC2 (0, NULL, &graphics);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipCreateFromHDC2 (0, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphicsOriginal);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, GdipCreateFromHWND)
{
	STARTUP

	GpStatus status;
	GpGraphics *graphics;
#if defined(USE_WINDOWS_GDIPLUS)
	GpRectF bounds;
#endif

	status = GdipCreateFromHWND (0, NULL);
	ASSERT_EQ (InvalidParameter, status);

// Libgdiplus does not implement GdipCreateFromHwnd.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWND (0, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics != NULL);

	GdipDeleteGraphics (graphics);

	status = GdipCreateFromHWND (hwnd, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics != NULL);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (484, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (61, bounds.Height);

	GdipDeleteGraphics (graphics);
#else
	status = GdipCreateFromHWND (0, &graphics);
	ASSERT_EQ (NotImplemented, status);
#endif

SHUTDOWN
}

TEST(Graphics, GdipCreateFromHWNDICM)
{
	STARTUP

	GpStatus status;
	GpGraphics *graphics;

	status = GdipCreateFromHWNDICM (0, NULL);
	ASSERT_EQ (InvalidParameter, status);

// Libgdiplus does not implement GdipCreateFromHwndICM.
#if defined(USE_WINDOWS_GDIPLUS)
	// HWND of zero means the current screen.
	status = GdipCreateFromHWNDICM (0, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics != NULL);

	status = GdipCreateFromHWNDICM (hwnd, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics != NULL);

	GdipDeleteGraphics (graphics);
#else
  status = GdipCreateFromHWNDICM (0, &graphics);
	ASSERT_EQ (NotImplemented, status);
#endif

SHUTDOWN
}

TEST(Graphics, GdipGetDC)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	HDC hdc;

	graphics = getImageGraphics (&image);

	// Negative tests.
	status = GdipGetDC (NULL, &hdc);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetDC (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetDC (graphics, &hdc);
	ASSERT_EQ (Ok, status);
	assert (hdc != 0);

	status = GdipGetDC (graphics, &hdc);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetDC (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipReleaseDC (NULL, hdc);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipReleaseDC (NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipReleaseDC (graphics, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipReleaseDC (graphics, hdc);
	ASSERT_EQ (Ok, status);

	status = GdipReleaseDC (graphics, hdc);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, CompositingMode)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingMode mode;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetCompositingMode(graphics, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingModeSourceOver, mode);

	// CompositingModeSourceOver.
	status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingMode(graphics, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingModeSourceOver, mode);

	// CompositingModeSourceOver.
	status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingMode(graphics, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingModeSourceCopy, mode);

	// Invalid CompositingMode.
	status = GdipSetCompositingMode (graphics, (CompositingMode) (CompositingModeSourceCopy + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingMode(graphics, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((CompositingMode) (CompositingModeSourceCopy + 1), mode);

	// Invalid CompositingMode.
	status = GdipSetCompositingMode (graphics, (CompositingMode) (CompositingModeSourceOver - 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingMode(graphics, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((CompositingMode) (CompositingModeSourceOver - 1), mode);

	// Negative tests.
	status = GdipSetCompositingMode (NULL, CompositingModeSourceCopy);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCompositingMode (NULL, &mode);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCompositingMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingMode (graphics, &mode);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetCompositingMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, CompositingQuality)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	CompositingQuality quality;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityDefault, quality);

	// CompositingQualityInvalid.
	status = GdipSetCompositingQuality (graphics, CompositingQualityInvalid);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityInvalid, quality);

	// CompositingQualityDefault.
	status = GdipSetCompositingQuality (graphics, CompositingQualityDefault);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityDefault, quality);

	// CompositingQualityHighSpeed.
	status = GdipSetCompositingQuality (graphics, CompositingQualityHighSpeed);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityHighSpeed, quality);

	// CompositingQualityHighQuality.
	status = GdipSetCompositingQuality (graphics, CompositingQualityHighQuality);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityHighQuality, quality);

	// CompositingQualityGammaCorrected.
	status = GdipSetCompositingQuality (graphics, CompositingQualityGammaCorrected);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityGammaCorrected, quality);

	// CompositingQualityAssumeLinear.
	status = GdipSetCompositingQuality (graphics, CompositingQualityAssumeLinear);
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CompositingQualityAssumeLinear, quality);

	// Invalid CompositingQuality.
	status = GdipSetCompositingQuality (graphics, (CompositingQuality) (CompositingQualityAssumeLinear + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((CompositingQuality) (CompositingQualityAssumeLinear + 1), quality);

	// Invalid CompositingQuality.
	status = GdipSetCompositingQuality (graphics, (CompositingQuality) (CompositingQualityInvalid - 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((CompositingQuality) (CompositingQualityInvalid - 1), quality);

	// Negative tests.
	status = GdipGetCompositingQuality(NULL, &quality);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCompositingQuality (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCompositingQuality (NULL, CompositingQualityAssumeLinear);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetCompositingQuality (graphics, &quality);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetCompositingQuality (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCompositingQuality(graphics, CompositingQualityAssumeLinear);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, RenderingOrigin)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	int x;
	int y;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetRenderingOrigin (graphics, &x, &y);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, x);
	ASSERT_EQ (0, y);

	// Positive.
	status = GdipSetRenderingOrigin (graphics, 1, 2);
	ASSERT_EQ (Ok, status);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, x);
	ASSERT_EQ (2, y);

	// Zero.
	status = GdipSetRenderingOrigin (graphics, 0, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, x);
	ASSERT_EQ (0, y);

	// Negative.
	status = GdipSetRenderingOrigin (graphics, -1, -2);
	ASSERT_EQ (Ok, status);

	status = GdipGetRenderingOrigin (graphics, &x, &y);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (-1, x);
	ASSERT_EQ (-2, y);

	// Negative tests.
	status = GdipGetRenderingOrigin (NULL, &x, &y);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetRenderingOrigin (graphics, NULL, &y);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetRenderingOrigin (graphics, &x, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetRenderingOrigin(NULL, 0, 0);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetRenderingOrigin(graphics, &x, &y);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetRenderingOrigin(graphics, NULL, &y);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetRenderingOrigin(graphics, &x, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetRenderingOrigin (graphics, 1, 2);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, TextRenderingHint)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	TextRenderingHint textRenderingHint;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintSystemDefault, textRenderingHint);

	// TextRenderingHintSystemDefault.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSystemDefault);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintSystemDefault, textRenderingHint);

	// TextRenderingHintSingleBitPerPixelGridFit.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixelGridFit);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintSingleBitPerPixelGridFit, textRenderingHint);

	// TextRenderingHintSingleBitPerPixel.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixel);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintSingleBitPerPixel, textRenderingHint);

	// TextRenderingHintAntiAliasGridFit.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintAntiAliasGridFit);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintAntiAliasGridFit, textRenderingHint);

	// TextRenderingHintAntiAlias.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintAntiAlias);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintAntiAlias, textRenderingHint);

	// TextRenderingHintClearTypeGridFit.
	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextRenderingHint (graphics, &textRenderingHint);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (TextRenderingHintClearTypeGridFit, textRenderingHint);

	// Negative tests.
	status = GdipGetTextRenderingHint (NULL, &textRenderingHint);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetTextRenderingHint (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextRenderingHint (NULL, TextRenderingHintAntiAlias);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintClearTypeGridFit + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextRenderingHint(graphics, &textRenderingHint);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetTextRenderingHint(graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextRenderingHint (graphics, TextRenderingHintClearTypeGridFit);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetTextRenderingHint (graphics, (TextRenderingHint)(TextRenderingHintClearTypeGridFit + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, TextContrast)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	UINT textContrast;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetTextContrast (graphics, &textContrast);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (4, textContrast);

	// Large.
	status = GdipSetTextContrast (graphics, 12);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextContrast (graphics, &textContrast);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (12, textContrast);

	// Zero.
	status = GdipSetTextContrast (graphics, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetTextContrast (graphics, &textContrast);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, textContrast);

	// Negative tests.
	status = GdipGetTextContrast (NULL, &textContrast);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetTextContrast (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextContrast(NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextContrast (graphics, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextContrast (graphics, 13);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetTextContrast (graphics, &textContrast);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetTextContrast (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetTextContrast (graphics, 1);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetTextContrast (graphics, 13);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, SmoothingMode)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	SmoothingMode smoothingMode;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (SmoothingModeNone, smoothingMode);

	// Default -> None
	status = GdipSetSmoothingMode (graphics, SmoothingModeDefault);
	ASSERT_EQ (Ok, status);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (SmoothingModeNone, smoothingMode);

	// HighSpeed -> None
	status = GdipSetSmoothingMode(graphics, SmoothingModeHighSpeed);
	ASSERT_EQ (Ok, status);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (SmoothingModeNone, smoothingMode);

	// HighQuality -> AntiAlias
	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	ASSERT_EQ (Ok, status);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (SmoothingModeAntiAlias, smoothingMode);

	// SmoothingModeNone.
	status = GdipSetSmoothingMode(graphics, SmoothingModeNone);
	ASSERT_EQ (Ok, status);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (SmoothingModeNone, smoothingMode);

	// SmoothingModeAntiAlias.
	status = GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
	ASSERT_EQ (Ok, status);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (SmoothingModeAntiAlias, smoothingMode);

	// SmoothingModeAntiAlias8x8.
	status = GdipSetSmoothingMode(graphics, (SmoothingMode) (SmoothingModeAntiAlias + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetSmoothingMode(graphics, &smoothingMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((SmoothingMode) (SmoothingModeAntiAlias + 1), smoothingMode);

	// Negative tests.
	status = GdipGetSmoothingMode (NULL, &smoothingMode);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetSmoothingMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetSmoothingMode (NULL, SmoothingModeAntiAlias);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode) (SmoothingModeAntiAlias + 2));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetSmoothingMode (graphics, &smoothingMode);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetSmoothingMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetSmoothingMode (graphics, SmoothingModeHighQuality);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, SmoothingModeInvalid);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode) (SmoothingModeInvalid - 1));
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetSmoothingMode (graphics, (SmoothingMode) (SmoothingModeAntiAlias + 2));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, PixelOffsetMode)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	PixelOffsetMode pixelOffsetMode;

	graphics = getImageGraphics (&image);

	// Default get.
	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PixelOffsetModeDefault, pixelOffsetMode);

	// PixelOffsetModeDefault.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	ASSERT_EQ (Ok, status);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PixelOffsetModeDefault, pixelOffsetMode);

	// PixelOffsetModeHighSpeed.
	status = GdipSetPixelOffsetMode(graphics, PixelOffsetModeHighSpeed);
	ASSERT_EQ (Ok, status);

	status = GdipGetPixelOffsetMode(graphics, &pixelOffsetMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PixelOffsetModeHighSpeed, pixelOffsetMode);

	// PixelOffsetModeHighQuality.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeHighQuality);
	ASSERT_EQ (Ok, status);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PixelOffsetModeHighQuality, pixelOffsetMode);

	// PixelOffsetModeNone.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeNone);
	ASSERT_EQ (Ok, status);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PixelOffsetModeNone, pixelOffsetMode);

	// PixelOffsetModeHalf.
	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeHalf);
	ASSERT_EQ (Ok, status);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PixelOffsetModeHalf, pixelOffsetMode);

	// Negative tests.
	status = GdipGetPixelOffsetMode (NULL, &pixelOffsetMode);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPixelOffsetMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPixelOffsetMode(NULL, PixelOffsetModeDefault);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeInvalid);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeInvalid - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeHalf + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetPixelOffsetMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeDefault);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeInvalid - 1));
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, PixelOffsetModeInvalid);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPixelOffsetMode (graphics, (PixelOffsetMode)(PixelOffsetModeHalf + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, InterpolationMode)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	InterpolationMode interpolationMode;

	graphics = getImageGraphics (&image);

	// Default get
	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeBilinear, interpolationMode);

	// Default -> Bilinear
	status = GdipSetInterpolationMode (graphics, InterpolationModeDefault);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeBilinear, interpolationMode);

	// LowQuality -> NearestNeighbor
	status = GdipSetInterpolationMode(graphics, InterpolationModeLowQuality);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeBilinear, interpolationMode);

	// HighQuality -> HighQualityBicubic
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQuality);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeHighQualityBicubic, interpolationMode);

	// InterpolationModeBilinear.
	status = GdipSetInterpolationMode (graphics, InterpolationModeBilinear);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeBilinear, interpolationMode);

	// InterpolationModeBicubic.
	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeBicubic, interpolationMode);

	// InterpolationModeNearestNeighbor.
	status = GdipSetInterpolationMode (graphics, InterpolationModeNearestNeighbor);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeNearestNeighbor, interpolationMode);

	// InterpolationModeHighQualityBilinear.
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQualityBilinear);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeHighQualityBilinear, interpolationMode);

	// InterpolationModeHighQualityBicubic.
	status = GdipSetInterpolationMode (graphics, InterpolationModeHighQualityBicubic);
	ASSERT_EQ (Ok, status);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (InterpolationModeHighQualityBicubic, interpolationMode);

	// Negative tests.
	status = GdipGetInterpolationMode (NULL, &interpolationMode);
	ASSERT_EQ (InvalidParameter, status);

// This causes an access violation in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetInterpolationMode (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipSetInterpolationMode (NULL, InterpolationModeBicubic);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeInvalid - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeHighQualityBicubic + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetInterpolationMode (graphics, &interpolationMode);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetInterpolationMode (graphics, NULL);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, InterpolationModeInvalid);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeInvalid - 1));
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetInterpolationMode (graphics, (InterpolationMode)(InterpolationModeHighQualityBicubic + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, Transform)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *matrix;
	GpMatrix *setMatrix;
	GpMatrix *nonInvertibleMatrix;

	graphics = getImageGraphics (&image);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, &matrix);
	ASSERT_EQ (Ok, status);

	status = GdipCreateMatrix2 (146, 66, 158, 104, 42, 150, &setMatrix);
	ASSERT_EQ (Ok, status);

	status = GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
	ASSERT_EQ (Ok, status);

	// Default get.
	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);

	status = GdipSetWorldTransform (graphics, setMatrix);
	ASSERT_EQ (Ok, status);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);

	BOOL result;
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	ASSERT_TRUE (result);

	// Negative tests.
	status = GdipGetWorldTransform (NULL, matrix);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetWorldTransform (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetWorldTransform (NULL, matrix);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetWorldTransform (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetWorldTransform (graphics, nonInvertibleMatrix);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetWorldTransform (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetWorldTransform (graphics, matrix);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetWorldTransform (graphics, NULL);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetWorldTransform (graphics, nonInvertibleMatrix);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (setMatrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, PageUnit)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	Unit pageUnit;

	graphics = getImageGraphics (&image);

	// UnitDisplay.
	status = GdipSetPageUnit (graphics, UnitDisplay);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitDisplay, pageUnit);

	// UnitPixel.
	status = GdipSetPageUnit (graphics, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitPixel, pageUnit);

	// UnitPoint.
	status = GdipSetPageUnit (graphics, UnitPoint);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitPoint, pageUnit);

	// UnitInch.
	status = GdipSetPageUnit (graphics, UnitInch);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitInch, pageUnit);

	// UnitDocument.
	status = GdipSetPageUnit (graphics, UnitDocument);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitDocument, pageUnit);

	// UnitMillimeter.
	status = GdipSetPageUnit (graphics, UnitMillimeter);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitMillimeter, pageUnit);

	// Negative tests.
	status = GdipGetPageUnit (NULL, &pageUnit);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPageUnit (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageUnit (NULL, UnitDisplay);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageUnit (graphics, (Unit)(UnitWorld - 1));
	ASSERT_EQ (InvalidParameter, status);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPageUnit (graphics, (Unit)(UnitMillimeter + 1));
	ASSERT_EQ (InvalidParameter, status);
#else
	status = GdipSetPageUnit (graphics, (Unit)(UnitCairoPoint + 1));
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipSetPageUnit (graphics, UnitWorld);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageUnit (graphics, &pageUnit);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetPageUnit (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageUnit (graphics, UnitDisplay);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPageUnit (graphics, UnitWorld);
	ASSERT_EQ (status, ObjectBusy);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPageUnit (graphics, (Unit)(UnitMillimeter + 1));
	ASSERT_EQ (status, ObjectBusy);
#else
	status = GdipSetPageUnit (graphics, (Unit)(UnitCairoPoint + 1));
	ASSERT_EQ (status, ObjectBusy);
#endif

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, PageScale)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL pageScale;

	graphics = getImageGraphics (&image);

	status = GdipSetPageScale (graphics, 1);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageScale (graphics, &pageScale);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, pageScale);

	status = GdipSetPageScale (graphics, (REAL) 1000000032);
	ASSERT_EQ (Ok, status);

	status = GdipGetPageScale (graphics, &pageScale);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN ((REAL) 1000000032, pageScale);

	// Negative tests.
	status = GdipGetPageScale (NULL, &pageScale);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPageScale (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageScale (NULL, UnitDisplay);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageScale (graphics, -INFINITY);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageScale (graphics, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageScale (graphics, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageScale (graphics, FLT_MAX);
	ASSERT_EQ (InvalidParameter, status);

	// 1000000032 appears to be the max value accepted by GDI+.
	status = GdipSetPageScale (graphics, (REAL) 1000000033);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetPageScale (graphics, &pageScale);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetPageScale (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPageScale (graphics, 1);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPageScale (graphics, 1);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPageScale (graphics, 0);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetPageScale (graphics, (REAL) 1000000033);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, DpiX)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL dpiX;

	graphics = getImageGraphics (&image);

	status = GdipGetDpiX (graphics, &dpiX);
	ASSERT_EQ (Ok, status);
	assert (dpiX > 0);

	// Negative tests.
	status = GdipGetDpiX (NULL, &dpiX);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetDpiX (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC (graphics, &hdc);

	status = GdipGetDpiX (graphics, &dpiX);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetDpiX (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC(graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, DpiY)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	REAL dpiY;

	graphics = getImageGraphics (&image);

	status = GdipGetDpiY (graphics, &dpiY);
	ASSERT_EQ (Ok, status);
	assert (dpiY > 0);

	// Negative tests.
	status = GdipGetDpiY (NULL, &dpiY);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetDpiY (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	GdipGetDC(graphics, &hdc);

	status = GdipGetDpiY (graphics, &dpiY);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetDpiY (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, GdipFlush)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	graphics = getImageGraphics (&image);

	status = GdipFlush (graphics, FlushIntentionFlush);
	ASSERT_EQ (Ok, status);

	status = GdipFlush (graphics, FlushIntentionSync);
	ASSERT_EQ (Ok, status);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionFlush - 1));
	ASSERT_EQ (Ok, status);

	status = GdipFlush (graphics, (FlushIntention)(FlushIntentionSync + 1));
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipFlush (NULL, FlushIntentionFlush);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipFlush (graphics, FlushIntentionSync);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, GdipDelete)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	graphics = getImageGraphics (&image);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipDeleteGraphics (graphics);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	status = GdipDeleteGraphics (graphics);
	ASSERT_EQ (Ok, status);

	status = GdipDeleteGraphics (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, GdipRotateWorldTransform)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetVisibleClipBounds (graphics, &visibleClipBounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, visibleClipBounds.X);
	ASSERT_FLOAT_EQ_NAN (-32, visibleClipBounds.Y);
	ASSERT_FLOAT_EQ_NAN (32, visibleClipBounds.Width);
	ASSERT_FLOAT_EQ_NAN (32, visibleClipBounds.Height);

	// Negative tests.
	status = GdipRotateWorldTransform (NULL, 0, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipRotateWorldTransform (graphics, 0, (MatrixOrder) (MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipRotateWorldTransform (graphics, 0, MatrixOrderAppend);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipRotateWorldTransform (graphics, 0, (MatrixOrder) (MatrixOrderAppend + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteRegion (clip);
	GdipDeleteMatrix (transform);
	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Graphics, GdipResetClip)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipResetClip (graphics);
	ASSERT_EQ (Ok, status);

	GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Height);

	// Negative tests.
	status = GdipResetClip (NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipResetClip (graphics);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, GdipGetClip)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Height);

	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Empty clip - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (Ok, status);

	GdipGetRegionBounds (clip, graphics, &bounds);
	ASSERT_FLOAT_EQ_NAN (-49, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-22, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (120, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (50, bounds.Height);

	// Negative tests.
	status = GdipGetClip (NULL, clip);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetClip (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClip (graphics, clip);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetClip (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
	GdipDeleteRegion (clip);

	SHUTDOWN
}

TEST(Graphics, GdipGetClipBounds)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-4194304, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Height);

	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_FLOAT_EQ_NAN (-4194294, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-4194284, bounds.Y);
#endif
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (8388608, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_FLOAT_EQ_NAN (-14680063, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-6291458, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (29360128, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (12582912, bounds.Height);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Empty clip - translate transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-2, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip rect - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Empty clip rect - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip rect - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-2, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-49, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-22, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (120, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (50, bounds.Height);

	// Negative tests.
	status = GdipGetClipBounds (NULL, &bounds);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetClipBounds (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetClipBounds (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
	GdipDeleteRegion (emptyRegion);

	SHUTDOWN
}

TEST(Graphics, GdipGetClipBoundsI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (-4194304, bounds.X);
	ASSERT_EQ (-4194304, bounds.Y);
	ASSERT_EQ (8388608, bounds.Width);
	ASSERT_EQ (8388608, bounds.Height);

	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (-4194294, bounds.X);
	ASSERT_EQ (-4194284, bounds.Y);
#endif
	ASSERT_EQ (8388608, bounds.Width);
	ASSERT_EQ (8388608, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/437
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (-14680063, bounds.X);
	ASSERT_EQ (-6291458, bounds.Y);
	ASSERT_EQ (29360128, bounds.Width);
	ASSERT_EQ (12582912, bounds.Height);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, bounds.X);
	ASSERT_EQ (0, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	// Empty clip - translate transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, bounds.X);
	ASSERT_EQ (20, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRegion (graphics, emptyRegion, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, bounds.X);
	ASSERT_EQ (-2, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip rect - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, bounds.X);
	ASSERT_EQ (0, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	// Empty clip rect - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, bounds.X);
	ASSERT_EQ (20, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip rect - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, bounds.X);
	ASSERT_EQ (-2, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, bounds.X);
	ASSERT_EQ (20, bounds.Y);
	ASSERT_EQ (30, bounds.Width);
	ASSERT_EQ (40, bounds.Height);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (20, bounds.X);
	ASSERT_EQ (40, bounds.Y);
	ASSERT_EQ (30, bounds.Width);
	ASSERT_EQ (40, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (-49, bounds.X);
	ASSERT_EQ (-22, bounds.Y);
	ASSERT_EQ (120, bounds.Width);
	ASSERT_EQ (50, bounds.Height);

	// Negative tests.
	status = GdipGetClipBoundsI (NULL, &bounds);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetClipBoundsI (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetClipBoundsI (graphics, &bounds);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetClipBoundsI (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);
	GdipDeleteRegion (emptyRegion);

	SHUTDOWN
}

TEST(Graphics, GdipGetVisibleClipBounds)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *transform;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);

	// Default get - no transform.
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (100, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (68, bounds.Height);

	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (100, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (68, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_FLOAT_EQ_NAN (-199, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-36, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (302, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (134, bounds.Height);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Empty clip - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-2, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_FLOAT_EQ_NAN (-49, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-22, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (120, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (50, bounds.Height);
#endif

	GdipResetWorldTransform (graphics);

	// Negative tests.
	status = GdipGetVisibleClipBounds (NULL, &bounds);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetVisibleClipBounds (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetVisibleClipBounds (graphics, &bounds);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetVisibleClipBounds (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Graphics, GdipGetVisibleClipBoundsI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpMatrix *transform;
	GpRect bounds;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);

	// Default get - no transform
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, bounds.X);
	ASSERT_EQ (0, bounds.Y);
	ASSERT_EQ (100, bounds.Width);
	ASSERT_EQ (68, bounds.Height);

	// Default get - translate transform.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, bounds.X);
	ASSERT_EQ (20, bounds.Y);
	ASSERT_EQ (100, bounds.Width);
	ASSERT_EQ (68, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Default get - complex transform.
	GdipSetWorldTransform (graphics, transform);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (-199, bounds.X);
	ASSERT_EQ (-36, bounds.Y);
	ASSERT_EQ (302, bounds.Width);
	ASSERT_EQ (134, bounds.Height);
#endif

	GdipResetWorldTransform (graphics);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, bounds.X);
	ASSERT_EQ (0, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	// Empty clip - translate transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, bounds.X);
	ASSERT_EQ (20, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Empty clip - complex transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, bounds.X);
	ASSERT_EQ (-2, bounds.Y);
	ASSERT_EQ (0, bounds.Width);
	ASSERT_EQ (0, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, bounds.X);
	ASSERT_EQ (20, bounds.Y);
	ASSERT_EQ (30, bounds.Width);
	ASSERT_EQ (40, bounds.Height);

	// Custom clip - translate transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (20, bounds.X);
	ASSERT_EQ (40, bounds.Y);
	ASSERT_EQ (30, bounds.Width);
	ASSERT_EQ (40, bounds.Height);

	GdipResetWorldTransform (graphics);

	// Custom clip - complex transform.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipSetWorldTransform (graphics, transform);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: should be transformed: https://github.com/mono/libgdiplus/issues/309
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_EQ (-49, bounds.X);
	ASSERT_EQ (-22, bounds.Y);
	ASSERT_EQ (120, bounds.Width);
	ASSERT_EQ (50, bounds.Height);
#endif

	// Negative tests.
	status = GdipGetVisibleClipBoundsI (NULL, &bounds);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetVisibleClipBoundsI (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipGetVisibleClipBoundsI (graphics, &bounds);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipGetVisibleClipBoundsI (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Graphics, GdipIsClipEmpty)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	BOOL isEmpty;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEmpty);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Empty width clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 0, 40, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Empty height clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 0, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEmpty);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEmpty);

	// Negative tests.
	status = GdipIsClipEmpty (NULL, &isEmpty);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsClipEmpty (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipIsClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipIsClipEmpty (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, GdipIsVisibleClipEmpty)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	BOOL isEmpty;

	graphics = getImageGraphics (&image);

	// Default clip.
	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEmpty);

	// Empty clip - no transform.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Empty width clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 0, 40, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Empty height clip - no transform.
	GdipSetClipRect (graphics, 10, 20, 30, 0, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Empty clip - transformed.
	GdipSetClipRect (graphics, 0, 0, 0, 0, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEmpty);

	// Custom clip - no transform.
	GdipResetWorldTransform (graphics);
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEmpty);

	// Custom clip - transformed.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEmpty);

	// Negative tests.
	status = GdipIsVisibleClipEmpty (NULL, &isEmpty);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsVisibleClipEmpty (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipIsVisibleClipEmpty (graphics, &isEmpty);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipIsVisibleClipEmpty (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, GdipSetClipGraphics)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpImage *otherImage;
	GpGraphics *graphics;
	GpGraphics *otherGraphics;
	GpRegion *clip;
	GpRectF bounds;

	graphics = getImageGraphics (&image);
	otherGraphics = getImageGraphics (&otherImage);
	GdipCreateRegion (&clip);

	// No transform.
	GdipSetClipRect (otherGraphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Source graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME this is incorrect: https://github.com/mono/libgdiplus/issues/308
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_FLOAT_EQ_NAN (20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);
#endif

	// Target graphics transformed.
	GdipResetWorldTransform (graphics);
	GdipTranslateWorldTransform (otherGraphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipSetClipGraphics (NULL, otherGraphics, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipGraphics (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipGraphics (graphics, otherGraphics, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipGraphics (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipGraphics (graphics, NULL, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	status = GdipGetDC (otherGraphics, &hdc);

	status = GdipSetClipGraphics (graphics, otherGraphics, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipGraphics (NULL, otherGraphics, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipGraphics (graphics, otherGraphics, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (otherGraphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteGraphics (otherGraphics);
	GdipDisposeImage (image);
	GdipDisposeImage (otherImage);
	GdipDeleteRegion (clip);

	SHUTDOWN
}

TEST(Graphics, GdipSetClipHrgn)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipHrgn (graphics, hrgn, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	// FIXME: no transformation applied.
#if defined(USE_WINDOWS_GDIPLUS)
	ASSERT_FLOAT_EQ_NAN (20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);
#endif

	// Negative tests.
	status = GdipSetClipHrgn (NULL, hrgn, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipHrgn (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipHrgn (graphics, hrgn, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipHrgn (graphics, hrgn, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipHrgn (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipHrgn (graphics, hrgn, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteRegion (region);
	GdipDeleteRegion ((GpRegion *)hrgn);

	SHUTDOWN
}

TEST(Graphics, GdipSetClipRect)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Positive width, zero height.
	status = GdipSetClipRect (graphics, 10, 20, 30, 0, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Positive width, negative height.
	status = GdipSetClipRect (graphics, 10, 20, 30, -40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Zero width, positive height.
	status = GdipSetClipRect (graphics, 10, 20, 0, 40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Zero width, zero height.
	status = GdipSetClipRect (graphics, 10, 20, 0, 0, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Zero width, negative height.
	status = GdipSetClipRect (graphics, 10, 20, 0, -40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Negative width, positive height.
	status = GdipSetClipRect (graphics, 10, 20, -30, 40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative width, zero height.
	status = GdipSetClipRect (graphics, 10, 20, -30, 0, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Negative width, negative height.
	status = GdipSetClipRect (graphics, 10, 20, -30, -40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipSetClipRect (NULL, 10, 20, 30, 40, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipRect (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteRegion (region);
	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Graphics, GdipSetClipRectI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Positive width, zero height.
	status = GdipSetClipRectI (graphics, 10, 20, 30, 0, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Positive width, negative height.
	status = GdipSetClipRectI (graphics, 10, 20, 30, -40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Zero width, positive height.
	status = GdipSetClipRectI (graphics, 10, 20, 0, 40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Zero width, zero height.
	status = GdipSetClipRectI (graphics, 10, 20, 0, 0, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Zero width, negative height.
	status = GdipSetClipRectI (graphics, 10, 20, 0, -40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Negative width, positive height.
	status = GdipSetClipRectI (graphics, 10, 20, -30, 40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative width, zero height.
	status = GdipSetClipRectI (graphics, 10, 20, -30, 0, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Height);

	// Negative width, negative height.
	status = GdipSetClipRectI (graphics, 10, 20, -30, -40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClip (graphics, region);
	ASSERT_EQ (Ok, status);

	status = GdipGetRegionScansCount (region, &scansCount, matrix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, scansCount);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.X);
	ASSERT_FLOAT_EQ_NAN (-20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipSetClipRectI (NULL, 10, 20, 30, 40, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipRectI (graphics, 10, 20, 30, 40, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeleteRegion (region);
	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Graphics, GdipSetClipRegion)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipRegion (graphics, region, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipSetClipRegion (NULL, region, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipRegion (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipRegion (graphics, region, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipRegion (graphics, region, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipRegion (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipRegion (graphics, region, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDeleteRegion (region);
	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, GdipSetClipPath)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Target graphics transformed.
	GdipTranslateWorldTransform (graphics, -10, -20, MatrixOrderAppend);

	status = GdipSetClipPath (graphics, path, CombineModeReplace);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipSetClipPath (NULL, path, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipPath (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetClipPath (graphics, path, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipSetClipPath (graphics, path, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipPath (graphics, NULL, CombineModeReplace);
	ASSERT_EQ (status, ObjectBusy);

	status = GdipSetClipPath (graphics, path, (CombineMode) (CombineModeComplement + 1));
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);
	GdipDeletePath (path);

	SHUTDOWN
}

TEST(Graphics, GdipTranslateClip)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// Non-infinite.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipTranslateClip (graphics, 0, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative.
	status = GdipTranslateClip (graphics, -10, -20);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Positive.
	status = GdipTranslateClip (graphics, 10, 20);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipTranslateClip (NULL, 10, 20);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipTranslateClip (graphics, 10, 20);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, GdipTranslateClipI)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpRectF bounds;

	graphics = getImageGraphics (&image);

	// Non-infinite.
	GdipSetClipRect (graphics, 10, 20, 30, 40, CombineModeReplace);

	status = GdipTranslateClipI (graphics, 0, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative.
	status = GdipTranslateClipI (graphics, -10, -20);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, bounds.X);
	ASSERT_FLOAT_EQ_NAN (0, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Positive.
	status = GdipTranslateClipI (graphics, 10, 20);
	ASSERT_EQ (Ok, status);

	status = GdipGetClipBounds (graphics, &bounds);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, bounds.X);
	ASSERT_FLOAT_EQ_NAN (20, bounds.Y);
	ASSERT_FLOAT_EQ_NAN (30, bounds.Width);
	ASSERT_FLOAT_EQ_NAN (40, bounds.Height);

	// Negative tests.
	status = GdipTranslateClipI (NULL, 10, 20);
	ASSERT_EQ (InvalidParameter, status);

	HDC hdc;
	status = GdipGetDC (graphics, &hdc);

	status = GdipTranslateClipI (graphics, 10, 20);
	ASSERT_EQ (status, ObjectBusy);

	GdipReleaseDC (graphics, hdc);

	GdipDeleteGraphics (graphics);
	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Graphics, region_mas)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	status = GdipGetImageGraphicsContext (bitmap, &graphics);
	ASSERT_EQ (Ok, status);
	GdipGraphicsClear (graphics, 0xFF808080);

	status = GdipCreatePath (FillModeAlternate, &rectPath);
	ASSERT_EQ (Ok, status);
	GdipAddPathRectangleI (rectPath, 50, 50, 300, 300);

	status = GdipCreatePath (FillModeAlternate, &polyPath);
	ASSERT_EQ (Ok, status);
	GdipAddPathPolygonI (polyPath, polyPoints, sizeof(polyPoints) / sizeof(polyPoints[0]));

	status = GdipCreateRegion (&region);
	ASSERT_EQ (Ok, status);
	GdipSetEmpty (region);
	GdipCombineRegionPath (region, rectPath, CombineModeUnion);
	GdipCombineRegionPath (region, polyPath, CombineModeExclude);

	status = GdipCreateSolidFill (0xFF00FF00, &brush);
	ASSERT_EQ (Ok, status);

	GdipFillRegion (graphics, brush, region);

	status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
	ASSERT_EQ (Ok, status);
	assertEqualARGB (color, 0xFF808080);
	status = GdipBitmapGetPixel (bitmap, 50, 50, &color);
	ASSERT_EQ (Ok, status);
	assertEqualARGB (color, 0xFF00FF00);
	status = GdipBitmapGetPixel (bitmap, 200, 200, &color);
	ASSERT_EQ (Ok, status);
	assertEqualARGB (color, 0xFF808080);

	GdipDeleteGraphics (graphics);
	GdipDeletePath (rectPath);
	GdipDeletePath (polyPath);
	GdipDeleteRegion (region);
	GdipDeleteBrush (brush);
	GdipDisposeImage (bitmap);

	SHUTDOWN
}

TEST(Graphics, premultiplication)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;
	GpBitmap *bitmapBackground;
	GpGraphics *graphicsBackground;

	BYTE bpp32ArgbData[] = { 0xFF, 0xFF, 0xFF, 0x80 };
	ARGB bpp32ArgbPixels[] = { 0x80FFFFFF };
	ARGB bpp32RgbPixelsPre[] = { 0xFF000000 };
	ARGB bpp32RgbPixels[] = { 0xFF808080 };

	status = GdipCreateBitmapFromScan0 (1, 1, 4, PixelFormat32bppARGB, bpp32ArgbData, &bitmap);
	ASSERT_EQ (Ok, status);
	verifyBitmap (bitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 1, ImageFlagsHasAlpha, 0, TRUE);
	verifyPixels (bitmap, bpp32ArgbPixels);
	status = GdipCreateBitmapFromScan0 (1, 1, 4, PixelFormat32bppRGB, NULL, &bitmapBackground);
	ASSERT_EQ (Ok, status);
	status = GdipBitmapSetPixel (bitmapBackground, 0, 0, 0);
	ASSERT_EQ (Ok, status);
	verifyPixels (bitmapBackground, bpp32RgbPixelsPre);
	GdipGetImageGraphicsContext (bitmapBackground, &graphicsBackground);
	status = GdipDrawImage (graphicsBackground, (GpImage *)bitmap, 0, 0);
	ASSERT_EQ (Ok, status);
	GdipDeleteGraphics (graphicsBackground);
	verifyPixels (bitmapBackground, bpp32RgbPixels);
	GdipDisposeImage ((GpImage *) bitmapBackground);
	GdipDisposeImage ((GpImage *) bitmap);

	SHUTDOWN
}

TEST(Graphics, world_transform_in_container)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GraphicsContainer state;
	GpMatrix *matrix;
	GpMatrix *setMatrix;

	graphics = getImageGraphics (&image);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, &matrix);
	ASSERT_EQ (Ok, status);

	status = GdipCreateMatrix2 (2, 0, 0, 2, 10, 20, &setMatrix);
	ASSERT_EQ (Ok, status);

	status = GdipSetWorldTransform (graphics, setMatrix);
	ASSERT_EQ (Ok, status);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);
	BOOL result;
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	ASSERT_EQ (1, result);

	status = GdipBeginContainer2 (graphics, &state);
	ASSERT_EQ (Ok, status);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);
	GdipIsMatrixIdentity (matrix, &result);
	ASSERT_EQ (1, result);

	status = GdipSetWorldTransform (graphics, setMatrix);
	ASSERT_EQ (Ok, status);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	ASSERT_EQ (1, result);

	status = GdipResetWorldTransform (graphics);
	ASSERT_EQ (Ok, status);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);
	GdipIsMatrixIdentity (matrix, &result);
	ASSERT_EQ (1, result);

	status = GdipEndContainer (graphics, state);
	ASSERT_EQ (Ok, status);

	status = GdipGetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);
	GdipIsMatrixEqual (matrix, setMatrix, &result);
	ASSERT_EQ (1, result);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (setMatrix);
	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, world_transform_respects_page_unit_document)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;
	GpGraphics *graphics;
	GpMatrix *matrix;
	GpSolidFill *brush;

	// Test Graphics with UnitDocument and 72 dpi
	status = GdipCreateBitmapFromScan0 (612, 792, 0, PixelFormat32bppARGB, NULL, &bitmap);
	ASSERT_EQ (Ok, status);

	status = GdipBitmapSetResolution (bitmap, 72, 72);
	ASSERT_EQ (Ok, status);

	status = GdipGetImageGraphicsContext (bitmap, &graphics);
	ASSERT_EQ (Ok, status);

	status = GdipSetPageUnit (graphics, UnitDocument);
	ASSERT_EQ (Ok, status);

	const ARGB FillColor = 0xFF000000;
	status = GdipCreateSolidFill (FillColor, &brush);
	ASSERT_EQ (Ok, status);

	REAL rectX = 360;
	REAL rectY = 111;
	GdipCreateMatrix2 (1, 0, 0, 1, rectX, rectY, &matrix);
	status = GdipSetWorldTransform (graphics, matrix);
	ASSERT_EQ (Ok, status);

	REAL rectWidth = 74;
	REAL rectHeight = 72;
	status = GdipFillRectangle (graphics, brush, 0, 0, rectWidth, rectHeight);
	ASSERT_EQ (Ok, status);

#if !defined(USE_WINDOWS_GDIPLUS)
	REAL points[] = {
		rectX + 2,
		rectY + 2,
		rectX + rectWidth / 2,
		rectY + rectHeight / 2,
		rectX + rectWidth - 1,
		rectY + rectHeight - 1
	};
	for (int i = 0; i < ARRAY_SIZE (points); i += 2) {
		ARGB color;
		status = GdipBitmapGetPixel (bitmap, (int)(points[i] * 72 / 300.0f), (int)(points[i + 1] * 72 / 300.0f), &color);
		ASSERT_EQ (Ok, status);
		assertEqualARGB (color, FillColor);
	}
#endif

	GdipDisposeImage ((GpImage *)bitmap);
	GdipDeleteGraphics (graphics);
	GdipDeleteBrush (brush);
	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Graphics, world_transform_respects_page_unit_point)
{
	STARTUP

	GpStatus status;
	GpBitmap *bitmap;
	GpGraphics *graphics;
	GpSolidFill *brush;
	const float TargetResolution = 300;
	double scaleFactor = TargetResolution / 72;

	// Test Graphics with UnitPoint and 72 dpi
	status = GdipCreateBitmapFromScan0 ((int)(612 * scaleFactor), (int)(792 * scaleFactor), 0, PixelFormat32bppARGB, NULL, &bitmap);
	ASSERT_EQ (Ok, status);

	status = GdipBitmapSetResolution (bitmap, TargetResolution, TargetResolution);
	ASSERT_EQ (Ok, status);

	status = GdipGetImageGraphicsContext (bitmap, &graphics);
	ASSERT_EQ (Ok, status);

	status = GdipSetPageUnit (graphics, UnitPoint);
	ASSERT_EQ (Ok, status);

	const ARGB FillColor = 0xFF000000;
	status = GdipCreateSolidFill (FillColor, &brush);
	ASSERT_EQ (Ok, status);

	REAL rectX = 360;
	REAL rectY = 111;
	REAL rectWidth = 74;
	REAL rectHeight = 72;
	status = GdipTranslateWorldTransform (graphics, rectX, rectY, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);

	status = GdipFillRectangle (graphics, brush, 0, 0, rectWidth, rectHeight);
	ASSERT_EQ (Ok, status);

	REAL points[] = {
		rectX,
		rectY,
		rectX + rectWidth / 2,
		rectY + rectHeight / 2,
		rectX + rectWidth - 1,
		rectY + rectHeight - 1
	};
	for (int i = 0; i < ARRAY_SIZE (points); i += 2) {
		ARGB color;
		status = GdipBitmapGetPixel (bitmap,(int) points[i], (int) points[i + 1], &color);
		ASSERT_EQ (Ok, status);
		assertEqualARGB (color, 0);

		status = GdipBitmapGetPixel (bitmap, (int)(points[i] * scaleFactor) + 1, (int)(points[i + 1] * scaleFactor) + 1, &color);
		ASSERT_EQ (Ok, status);
		assertEqualARGB (color, FillColor);
	}

	GdipDisposeImage ((GpImage *)bitmap);
	GdipDeleteGraphics (graphics);
	GdipDeleteBrush (brush);

	SHUTDOWN
}

TEST(Graphics, GdipSaveGraphics)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GraphicsState state1;
	GraphicsState state2;

	graphics = getImageGraphics (&image);

	status = GdipSaveGraphics (graphics, &state1);
	ASSERT_EQ (Ok, status);
	assert (state1);

	status = GdipSaveGraphics (graphics, &state2);
	ASSERT_EQ (Ok, status);
	assert (state2 && state1 != state2);

	// Negative tests.
	status = GdipSaveGraphics (NULL, &state1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSaveGraphics (graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN
}

TEST(Graphics, GdipRestoreGraphics)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GraphicsState state;
	RectF clipBounds1 = { 1, 2, 3, 4 };
	RectF clipBounds2 = { 2, 3, 4, 5 };
	CompositingMode compositingMode;
	CompositingQuality compositingQuality;
	InterpolationMode interpolationMode;
	REAL pageScale;
	Unit pageUnit;
	PixelOffsetMode pixelOffsetMode;
	INT renderingOriginX;
	INT renderingOriginY;
	SmoothingMode smoothingMode;
	UINT textContrast;
	TextRenderingHint textRenderingHint;
	GpMatrix *worldTransform1;
	GpMatrix *worldTransform2;
	GpMatrix *worldTransform;

	graphics = getImageGraphics (&image);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &worldTransform1);
	GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &worldTransform2);
	GdipCreateMatrix (&worldTransform);

	status = GdipSaveGraphics (graphics, &state);
	ASSERT_EQ (Ok, status);

	status = GdipRestoreGraphics (graphics, state);
	ASSERT_EQ (Ok, status);

	status = GdipRestoreGraphics (graphics, state);
	ASSERT_EQ (Ok, status);

	status = GdipSaveGraphics (graphics, &state);
	ASSERT_EQ (Ok, status);

	status = GdipRestoreGraphics (graphics, 0);
	ASSERT_EQ (Ok, status);

	status = GdipRestoreGraphics (graphics, -1);
	ASSERT_EQ (Ok, status);

	status = GdipRestoreGraphics (graphics, 512);
	ASSERT_EQ (Ok, status);

	status = GdipRestoreGraphics (graphics, 513);
	ASSERT_EQ (Ok, status);

	// Setup save.
	GdipSetClipRect (graphics, 1, 2, 3, 4, CombineModeReplace);
	GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
	GdipSetCompositingQuality (graphics, CompositingQualityAssumeLinear);
	GdipSetInterpolationMode (graphics, InterpolationModeBicubic);
	GdipSetPageScale (graphics, 100);
	GdipSetPageUnit (graphics, UnitInch);
	GdipSetPixelOffsetMode (graphics, PixelOffsetModeHighQuality);
	GdipSetRenderingOrigin (graphics, 1, 2);
	GdipSetSmoothingMode (graphics, SmoothingModeAntiAlias);
	GdipSetTextContrast (graphics, 12);
	GdipSetTextRenderingHint (graphics, TextRenderingHintSingleBitPerPixel);
	GdipSetWorldTransform (graphics, worldTransform1);

	GdipGetClipBounds (graphics, &clipBounds1);

	status = GdipSaveGraphics (graphics, &state);
	ASSERT_EQ (Ok, status);

	// Change.
	GdipSetClipRect (graphics, 2, 3, 4, 5, CombineModeReplace);
	GdipSetCompositingMode (graphics, CompositingModeSourceOver);
	GdipSetCompositingQuality (graphics, CompositingQualityGammaCorrected);
	GdipSetInterpolationMode (graphics, InterpolationModeBilinear);
	GdipSetPageScale (graphics, 99);
	GdipSetPageUnit (graphics, UnitPoint);
	GdipSetPixelOffsetMode (graphics, PixelOffsetModeHighSpeed);
	GdipSetRenderingOrigin (graphics, 2, 3);
	GdipSetSmoothingMode (graphics, SmoothingModeNone);
	GdipSetTextContrast (graphics, 11);
	GdipSetTextRenderingHint (graphics, TextRenderingHintAntiAlias);
	GdipSetWorldTransform (graphics, worldTransform2);

	GdipGetCompositingMode (graphics, &compositingMode);
	GdipGetCompositingQuality (graphics, &compositingQuality);
	GdipGetInterpolationMode (graphics, &interpolationMode);
	GdipGetPageScale (graphics, &pageScale);
	GdipGetPageUnit (graphics, &pageUnit);
	GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	GdipGetRenderingOrigin (graphics, &renderingOriginX, &renderingOriginY);
	GdipGetSmoothingMode (graphics, &smoothingMode);
	GdipGetTextContrast (graphics, &textContrast);
	GdipGetTextRenderingHint (graphics, &textRenderingHint);
	GdipGetWorldTransform (graphics, worldTransform);
	ASSERT_EQ (CompositingModeSourceOver, compositingMode);
	ASSERT_EQ (CompositingQualityGammaCorrected, compositingQuality);
	ASSERT_EQ (InterpolationModeBilinear, interpolationMode);
	ASSERT_EQ (UnitPoint, pageUnit);
	ASSERT_FLOAT_EQ_NAN (99, pageScale);
	ASSERT_EQ (PixelOffsetModeHighSpeed, pixelOffsetMode);
	ASSERT_EQ (2, renderingOriginX);
	ASSERT_EQ (3, renderingOriginY);
	ASSERT_EQ (SmoothingModeNone, smoothingMode);
	ASSERT_EQ (11, textContrast);
	ASSERT_EQ (TextRenderingHintAntiAlias, textRenderingHint);
	verifyMatrix (worldTransform, 2, 3, 4, 5, 6, 7);

	// Restore.
	status = GdipRestoreGraphics (graphics, state);
	ASSERT_EQ (Ok, status);

	GdipGetClipBounds (graphics, &clipBounds2);
	GdipGetCompositingMode (graphics, &compositingMode);
	GdipGetCompositingQuality (graphics, &compositingQuality);
	GdipGetInterpolationMode (graphics, &interpolationMode);
	GdipGetPageScale (graphics, &pageScale);
	GdipGetPageUnit (graphics, &pageUnit);
	GdipGetPixelOffsetMode (graphics, &pixelOffsetMode);
	GdipGetRenderingOrigin (graphics, &renderingOriginX, &renderingOriginY);
	GdipGetSmoothingMode (graphics, &smoothingMode);
	GdipGetTextContrast (graphics, &textContrast);
	GdipGetTextRenderingHint (graphics, &textRenderingHint);
	GdipGetWorldTransform (graphics, worldTransform);
	assertEqualRect (clipBounds2, clipBounds1);
	ASSERT_EQ (CompositingModeSourceCopy, compositingMode);
	ASSERT_EQ (CompositingQualityAssumeLinear, compositingQuality);
	ASSERT_EQ (InterpolationModeBicubic, interpolationMode);
	ASSERT_EQ (UnitInch, pageUnit);
	ASSERT_FLOAT_EQ_NAN (100, pageScale);
	ASSERT_EQ (PixelOffsetModeHighQuality, pixelOffsetMode);
	ASSERT_EQ (1, renderingOriginX);
	ASSERT_EQ (2, renderingOriginY);
	ASSERT_EQ (SmoothingModeAntiAlias, smoothingMode);
	ASSERT_EQ (12, textContrast);
	ASSERT_EQ (TextRenderingHintSingleBitPerPixel, textRenderingHint);
	verifyMatrix (worldTransform, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipRestoreGraphics (NULL, state);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
	GdipDeleteMatrix (worldTransform1);
	GdipDeleteMatrix (worldTransform2);
	GdipDeleteMatrix (worldTransform);

	SHUTDOWN
}
