#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_GDIPLUS)
#define GDIPVER 0x0110
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

static void test_startup ()
{
	GpStatus status;
	ULONG_PTR gdiplusToken = 0;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	GdiplusStartupOutput gdiplusStartupOutput = {NULL, NULL};

	// Version 1 - no output.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	GdiplusShutdown (gdiplusToken);

	// Version 1 - output.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	assert (!gdiplusStartupOutput.NotificationHook && !gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);

	// Version 2 - no output.
	gdiplusStartupInput.GdiplusVersion = 2;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	GdiplusShutdown (gdiplusToken);

	// Version 2 - output.
	gdiplusStartupInput.GdiplusVersion = 2;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	assert (!gdiplusStartupOutput.NotificationHook && !gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);
	gdiplusStartupInput.GdiplusVersion = 1;

	// SuppressBackgroundThread is true.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	assert (gdiplusStartupOutput.NotificationHook && gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;

	// Startup multiple times.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);

	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);
	GdiplusShutdown (gdiplusToken);

	// Negative tests.
	status = GdiplusStartup (NULL, &gdiplusStartupInput, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdiplusStartup (&gdiplusToken, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	gdiplusStartupInput.GdiplusVersion = 0;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, UnsupportedGdiplusVersion);

	gdiplusStartupInput.GdiplusVersion = 3;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, UnsupportedGdiplusVersion);

	gdiplusStartupInput.GdiplusVersion = 1;
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, InvalidParameter);

	gdiplusStartupInput.GdiplusVersion = 3;
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_shutdown ()
{
	ULONG_PTR gdiplusToken = 0;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	// Valid token.
	GdiplusShutdown (gdiplusToken);

	// Valid token again.
	GdiplusShutdown (gdiplusToken);

	// Invalid token.
	GdiplusShutdown ((ULONG_PTR) 1);

	// Null token.
	GdiplusShutdown ((ULONG_PTR) 0);
}

static void test_notificationHook ()
{
	GpStatus status;
	ULONG_PTR gdiplusToken = 0;
	ULONG_PTR notificationToken;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartupOutput gdiplusStartupOutput = {NULL, NULL};

	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);

	status = GdiplusNotificationHook (&notificationToken);
	assertEqualInt (status, Ok);
	assert (notificationToken);
	GdiplusNotificationUnhook (notificationToken);

	// Negative tests.
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdiplusNotificationHook (NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	GdiplusShutdown (gdiplusToken);

	// SupressBackgroundThread is false.
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	status = GdiplusNotificationHook (&notificationToken);
	assertEqualInt (status, GenericError);

	status = GdiplusNotificationHook (NULL);
	assertEqualInt (status, GenericError);

	GdiplusShutdown (gdiplusToken);
}

static void test_notificationUnhook ()
{
	ULONG_PTR gdiplusToken = 0;
	ULONG_PTR notificationToken;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartupOutput gdiplusStartupOutput = {NULL, NULL};

	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	GdiplusNotificationHook (&notificationToken);

	// Valid token.
	GdiplusNotificationUnhook (notificationToken);

	// Valid token again.
	GdiplusNotificationUnhook (notificationToken);

	// Invalid token.
	GdiplusNotificationUnhook ((ULONG_PTR) 1);

	// Zero token.
	GdiplusNotificationUnhook ((ULONG_PTR) 0);

	GdiplusShutdown (gdiplusToken);

	// SupressBackgroundThread is false.
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	// Valid token.
	GdiplusNotificationUnhook (notificationToken);

	// Valid token again.
	GdiplusNotificationUnhook (notificationToken);

	// Invalid token.
	GdiplusNotificationUnhook ((ULONG_PTR) 1);

	// Zero token.
	GdiplusNotificationUnhook ((ULONG_PTR) 0);

	GdiplusShutdown (gdiplusToken);
}

static void test_alloc()
{
	void *data;

	data = GdipAlloc (0);
	GdipFree (data);

	data = GdipAlloc (1);
	GdipFree (data);
}

static void test_free()
{
	void *data;

	// Valid pointer.
	data = GdipAlloc (1);
	GdipFree (data);

	// Null pointer.
	GdipFree (NULL);
}

static void test_notInitialized ()
{
	GpStatus status;

	void *data = GdipAlloc (10);
	assert (!data);

	status = GdipCloneRegion (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateAdjustableArrowCap (0, 0, FALSE, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromFile (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromFileICM (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	// Libgdiplus does not implement GdipCreateBitmapFromGdiDib.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateBitmapFromGdiDib (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);
#endif

	status = GdipCreateBitmapFromGraphics (0, 0, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromHBITMAP (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromHICON (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromResource (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromScan0 (0, 0, 0, PixelFormat32bppARGB, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromStream (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateBitmapFromStreamICM (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	// Libgdiplus does not implement GdipCreateCachedBitmap.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateCachedBitmap (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);
#endif

	status = GdipCreateCustomLineCap (NULL, NULL, LineCapFlat, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFont (NULL, 0, 0, UnitPixel, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFontFamilyFromName (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipGetGenericFontFamilySansSerif (NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetGenericFontFamilySerif (NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetGenericFontFamilyMonospace (NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFontFromDC (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFontFromLogfontA (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFontFromLogfontW (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFromHDC (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFromHDC2 (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFromHWND (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateFromHWNDICM (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateHBITMAPFromBitmap (NULL, NULL, 0);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateHICONFromBitmap (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateHatchBrush (HatchStyle05Percent, 0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateImageAttributes (NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateLineBrush (NULL, NULL, 0, 0, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateLineBrushFromRect (NULL, 0, 0, LinearGradientModeVertical, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateLineBrushFromRectI (NULL, 0, 0, LinearGradientModeVertical, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateLineBrushFromRectWithAngle (NULL, 0, 0, 0, FALSE, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateLineBrushFromRectWithAngleI (NULL, 0, 0, 0, FALSE, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateLineBrushI (NULL, NULL, 0, 0, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMatrix (NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMatrix3 (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMatrix3I (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMetafileFromEmf (NULL, FALSE, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMetafileFromFile (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMetafileFromStream (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateMetafileFromWmf (NULL, FALSE, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	// Libgdiplus does not implement GdipCreateMetafileFromWmfFile.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateMetafileFromWmfFile (NULL, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);
#endif

	status = GdipCreatePath (FillModeAlternate, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePath2 (NULL, NULL, 0, FillModeAlternate, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePath2I (NULL, NULL, 0, FillModeAlternate, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePathGradient (NULL, 0, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePathGradientFromPath (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePathGradientI (NULL, 0, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePathIter (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePen1 (0, 0, UnitPixel, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreatePen2 (NULL, 0, UnitPixel, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateRegion (NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateRegionPath (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateRegionRect (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateRegionRectI (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateRegionRgnData (NULL, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateSolidFill (0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateStringFormat (0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipStringFormatGetGenericDefault (NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipStringFormatGetGenericTypographic (NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateTexture (NULL, WrapModeClamp, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateTextureIA (NULL, NULL, 0, 0, 0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipCreateTextureIAI (NULL, NULL, 0, 0, 0, 0, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipDeletePrivateFontCollection (NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipGetTextureImage (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipLoadImageFromStream (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipLoadImageFromStreamICM (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipLoadImageFromFile (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipLoadImageFromFileICM (NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipNewPrivateFontCollection (NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipRecordMetafile (NULL, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitMillimeter, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipRecordMetafileFileName (NULL, NULL, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitMillimeter, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipRecordMetafileStream (NULL, NULL, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitMillimeter, NULL, NULL);
	assertEqualInt (status, GdiplusNotInitialized);

	status = GdipSetPathGradientSurroundColorsWithCount (NULL, NULL, 0);
	assertEqualInt (status, GdiplusNotInitialized);
}

int
main (int argc, char**argv)
{
	test_startup ();
	test_shutdown ();
	test_notificationHook ();
	test_notificationUnhook ();
	test_alloc ();
	test_free ();
	test_notInitialized ();

	return 0;
}
