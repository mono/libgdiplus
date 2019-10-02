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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

static void test_clone ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;
    GpBrush *clone;
    ARGB solidColor;

    status = GdipCreateSolidFill (0x010203040, &brush);
    assertEqualInt (status, Ok);

    status = GdipCloneBrush ((GpBrush *) brush, &clone);
    assertEqualInt (status, Ok);
    assert (clone && brush != clone);

    status = GdipGetBrushType (clone, &brushType);
    assertEqualInt (status, Ok);
    assertEqualInt (brushType, BrushTypeSolidColor);

    status = GdipGetSolidFillColor ((GpSolidFill *) clone, &solidColor);
    assertEqualInt (status, Ok);
    assertEqualARGB (solidColor, 0x010203040);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush (clone);
}

static void test_delete ()
{
    GpStatus status;
    GpSolidFill *brush;

    status = GdipCreateSolidFill (0x010203040, &brush);
    assertEqualInt (status, Ok);

    status =  GdipDeleteBrush ((GpBrush *) brush);
    assertEqualInt (status, Ok);
}

static void test_createSolidFill ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;
    ARGB color;

    status = GdipCreateSolidFill (0x010203040, &brush);
    assertEqualInt (status, Ok);
    assert (brush && "Expected the brush to be initialized.");

    status = GdipGetBrushType ((GpBrush *) brush, &brushType);
    assertEqualInt (status, Ok);
    assertEqualInt (brushType, BrushTypeSolidColor);
    
    status = GdipGetSolidFillColor (brush, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x010203040);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    status = GdipCreateSolidFill (0x010203040, NULL);
    assertEqualInt (status, InvalidParameter);
}

static void test_getSolidFillColor ()
{
    GpStatus status;
    GpSolidFill *brush;
    ARGB color;

    status = GdipCreateSolidFill (0xFF000000, &brush);
    assertEqualInt (status, Ok);

    // Negative tests.
    status = GdipGetSolidFillColor (NULL, &color);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetSolidFillColor (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setSolidFillColor ()
{
    GpStatus status;
    GpSolidFill *brush;
    ARGB color;

    status = GdipCreateSolidFill (0xFF000000, &brush);
    assertEqualInt (status, Ok);

    // Set new.
    status = GdipSetSolidFillColor (brush, 0x010203040);
    assertEqualInt (status, Ok);

    status = GdipGetSolidFillColor (brush, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x010203040);

    // Set same.
    status = GdipSetSolidFillColor (brush, 0x010203040);
    assertEqualInt (status, Ok);

    status = GdipGetSolidFillColor (brush, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x010203040);

    // Negative tests.
    status = GdipSetSolidFillColor (NULL, 0x010203040);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_fill ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBitmap *bitmap;
    GpGraphics *graphics;
    ARGB color;

    status = GdipCreateSolidFill (0xDDA03040, &brush);
    assertEqualInt (status, Ok);

	// PixelFormat32bppARGB.
    status = GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppARGB, NULL, &bitmap);
    assertEqualInt (status, Ok);

    status = GdipGetImageGraphicsContext ((GpImage *) bitmap, &graphics);
    assertEqualInt (status, Ok);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - CompositingModeSourceOver.
    status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - unchanged, CompositingModeSourceOver.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - changed, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - zero alpha, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x00000000);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    assertEqualInt (status, Ok);

    status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - unchanged, CompositingModeSourceCopy.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - changed, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - zero alpha, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x00000000);
	
    GdipDeleteGraphics (graphics);
    GdipDisposeImage ((GpImage *) bitmap);
	
	// PixelFormat32bppPARGB.
    status = GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppPARGB, NULL, &bitmap);
    assertEqualInt (status, Ok);

    status = GdipGetImageGraphicsContext ((GpImage *) bitmap, &graphics);
    assertEqualInt (status, Ok);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);
	
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    assertEqualInt (status, Ok);

    // Fill with brush - CompositingModeSourceOver.
    status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - unchanged, CompositingModeSourceOver.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - changed, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - zero alpha, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x00000000);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    assertEqualInt (status, Ok);

    status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - unchanged, CompositingModeSourceCopy.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - changed, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - zero alpha, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0x00000000);
	
    GdipDeleteGraphics (graphics);
    GdipDisposeImage ((GpImage *) bitmap);

	// PixelFormat32bppRGB.
    status = GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppRGB, NULL, &bitmap);
    assertEqualInt (status, Ok);

    status = GdipGetImageGraphicsContext ((GpImage *) bitmap, &graphics);
    assertEqualInt (status, Ok);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);
	
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    assertEqualInt (status, Ok);

    // Fill with brush - CompositingModeSourceOver.
    status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - unchanged, CompositingModeSourceOver.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - changed, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF7D8893);
#else
    assertEqualARGB (color, 0xFF7D8893);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - zero alpha, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xFF000000);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    assertEqualInt (status, Ok);

    status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - unchanged, CompositingModeSourceCopy.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - changed, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xFF7D8893);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    assertEqualInt (status, Ok);

    // Fill with brush - zero alpha, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    assertEqualInt (status, Ok);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    assertEqualInt (status, Ok);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    assertEqualInt (status, Ok);
    assertEqualARGB (color, 0xFF000000);
	
    GdipDeleteGraphics (graphics);
    GdipDisposeImage ((GpImage *) bitmap);

    // Negative tests.
    status = GdipSetSolidFillColor (NULL, 0x010203040);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

int
main (int argc, char**argv)
{
    STARTUP;

    test_clone ();
    test_delete ();
    test_createSolidFill ();
    test_getSolidFillColor ();
    test_setSolidFillColor ();
    test_fill ();

    SHUTDOWN;
    return 0;
}
