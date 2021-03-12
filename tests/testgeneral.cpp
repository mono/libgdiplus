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

TEST(General, GdiplusStartup)
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
	ASSERT_EQ (Ok, status);
	assert (gdiplusToken);
	GdiplusShutdown (gdiplusToken);

	// Version 1 - output.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	ASSERT_EQ (Ok, status);
	assert (gdiplusToken);
	assert (!gdiplusStartupOutput.NotificationHook && !gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);

	// Version 2 - no output.
	gdiplusStartupInput.GdiplusVersion = 2;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (Ok, status);
	assert (gdiplusToken);
	GdiplusShutdown (gdiplusToken);

	// Version 2 - output.
	gdiplusStartupInput.GdiplusVersion = 2;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	ASSERT_EQ (Ok, status);
	assert (gdiplusToken);
	assert (!gdiplusStartupOutput.NotificationHook && !gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);
	gdiplusStartupInput.GdiplusVersion = 1;

	// SuppressBackgroundThread is true.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	ASSERT_EQ (Ok, status);
	assert (gdiplusToken);
	assert (gdiplusStartupOutput.NotificationHook && gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;

	// Startup multiple times.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (Ok, status);

	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (Ok, status);
	GdiplusShutdown (gdiplusToken);

	// Negative tests.
	status = GdiplusStartup (NULL, &gdiplusStartupInput, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdiplusStartup (&gdiplusToken, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	gdiplusStartupInput.GdiplusVersion = 0;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (UnsupportedGdiplusVersion, status);

	gdiplusStartupInput.GdiplusVersion = 3;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (UnsupportedGdiplusVersion, status);

	gdiplusStartupInput.GdiplusVersion = 1;
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (InvalidParameter, status);

	gdiplusStartupInput.GdiplusVersion = 3;
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	ASSERT_EQ (InvalidParameter, status);
}

TEST(General, GdiplusShutdown)
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

TEST(General, GdiplusNotificationHook)
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
	ASSERT_EQ (Ok, status);
	assert (notificationToken);
	GdiplusNotificationUnhook (notificationToken);

	// Negative tests.
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdiplusNotificationHook (NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdiplusShutdown (gdiplusToken);

	// SupressBackgroundThread is false.
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	status = GdiplusNotificationHook (&notificationToken);
	ASSERT_EQ (GenericError, status);

	status = GdiplusNotificationHook (NULL);
	ASSERT_EQ (GenericError, status);

	GdiplusShutdown (gdiplusToken);
}

TEST(General, GdiplusNotificationUnhook)
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

TEST(General, GdipAlloc)
{
	void *data;

	data = GdipAlloc (0);
	GdipFree (data);

	data = GdipAlloc (1);
	GdipFree (data);
}

TEST(General, GdipFree)
{
	void *data;

	// Valid pointer.
	data = GdipAlloc (1);
	GdipFree (data);

	// Null pointer.
	GdipFree (NULL);
}

TEST(General, notInitialized)
{
	GpStatus status;

	void *data = GdipAlloc (10);
	assert (!data);

	status = GdipCloneRegion (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateAdjustableArrowCap (0, 0, FALSE, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromFile (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromFileICM (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	// Libgdiplus does not implement GdipCreateBitmapFromGdiDib.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateBitmapFromGdiDib (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);
#endif

	status = GdipCreateBitmapFromGraphics (0, 0, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromHBITMAP (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromHICON (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromResource (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromScan0 (0, 0, 0, PixelFormat32bppARGB, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromStream (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateBitmapFromStreamICM (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	// Libgdiplus does not implement GdipCreateCachedBitmap.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateCachedBitmap (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);
#endif

	status = GdipCreateCustomLineCap (NULL, NULL, LineCapFlat, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFont (NULL, 0, 0, UnitPixel, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFontFamilyFromName (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipGetGenericFontFamilySansSerif (NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetGenericFontFamilySerif (NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetGenericFontFamilyMonospace (NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFontFromDC (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFontFromLogfontA (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFontFromLogfontW (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFromHDC (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFromHDC2 (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFromHWND (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateFromHWNDICM (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateHBITMAPFromBitmap (NULL, NULL, 0);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateHICONFromBitmap (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateHatchBrush (HatchStyle05Percent, 0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateImageAttributes (NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateLineBrush (NULL, NULL, 0, 0, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateLineBrushFromRect (NULL, 0, 0, LinearGradientModeVertical, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateLineBrushFromRectI (NULL, 0, 0, LinearGradientModeVertical, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateLineBrushFromRectWithAngle (NULL, 0, 0, 0, FALSE, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateLineBrushFromRectWithAngleI (NULL, 0, 0, 0, FALSE, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateLineBrushI (NULL, NULL, 0, 0, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMatrix (NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMatrix2 (0, 0, 0, 0, 0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMatrix3 (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMatrix3I (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMetafileFromEmf (NULL, FALSE, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMetafileFromFile (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMetafileFromStream (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateMetafileFromWmf (NULL, FALSE, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	// Libgdiplus does not implement GdipCreateMetafileFromWmfFile.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateMetafileFromWmfFile (NULL, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);
#endif

	status = GdipCreatePath (FillModeAlternate, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePath2 (NULL, NULL, 0, FillModeAlternate, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePath2I (NULL, NULL, 0, FillModeAlternate, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePathGradient (NULL, 0, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePathGradientFromPath (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePathGradientI (NULL, 0, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePathIter (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePen1 (0, 0, UnitPixel, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreatePen2 (NULL, 0, UnitPixel, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateRegion (NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateRegionPath (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateRegionRect (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateRegionRectI (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateRegionRgnData (NULL, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateSolidFill (0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateStringFormat (0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipStringFormatGetGenericDefault (NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipStringFormatGetGenericTypographic (NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateTexture (NULL, WrapModeClamp, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateTextureIA (NULL, NULL, 0, 0, 0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipCreateTextureIAI (NULL, NULL, 0, 0, 0, 0, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipDeletePrivateFontCollection (NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipGetTextureImage (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipLoadImageFromStream (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipLoadImageFromStreamICM (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipLoadImageFromFile (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipLoadImageFromFileICM (NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipNewPrivateFontCollection (NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipRecordMetafile (NULL, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitMillimeter, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipRecordMetafileFileName (NULL, NULL, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitMillimeter, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipRecordMetafileStream (NULL, NULL, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitMillimeter, NULL, NULL);
	ASSERT_EQ (GdiplusNotInitialized, status);

	status = GdipSetPathGradientSurroundColorsWithCount (NULL, NULL, 0);
	ASSERT_EQ (GdiplusNotInitialized, status);
}
