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

TEST(SolidBrush, GdipCloneBrush)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;
    GpBrush *clone;
    ARGB solidColor;

    status = GdipCreateSolidFill (0x010203040, &brush);
    ASSERT_EQ (Ok, status);

    status = GdipCloneBrush ((GpBrush *) brush, &clone);
    ASSERT_EQ (Ok, status);
    assert (clone && brush != clone);

    status = GdipGetBrushType (clone, &brushType);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (BrushTypeSolidColor, brushType);

    status = GdipGetSolidFillColor ((GpSolidFill *) clone, &solidColor);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (solidColor, 0x010203040);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush (clone);

    SHUTDOWN
}

TEST(SolidBrush, GdipDeleteBrush)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;

    status = GdipCreateSolidFill (0x010203040, &brush);
    ASSERT_EQ (Ok, status);

    status =  GdipDeleteBrush ((GpBrush *) brush);
    ASSERT_EQ (Ok, status);

    SHUTDOWN
}

TEST(SolidBrush, GdipCreateSolidFill)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;
    ARGB color;

    status = GdipCreateSolidFill (0x010203040, &brush);
    ASSERT_EQ (Ok, status);
    assert (brush && "Expected the brush to be initialized.");

    status = GdipGetBrushType ((GpBrush *) brush, &brushType);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (BrushTypeSolidColor, brushType);
    
    status = GdipGetSolidFillColor (brush, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x010203040);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    status = GdipCreateSolidFill (0x010203040, NULL);
    ASSERT_EQ (InvalidParameter, status);

    SHUTDOWN
}

TEST(SolidBrush, GdipGetSolidFillColor)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    ARGB color;

    status = GdipCreateSolidFill (0xFF000000, &brush);
    ASSERT_EQ (Ok, status);

    // Negative tests.
    status = GdipGetSolidFillColor (NULL, &color);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetSolidFillColor (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(SolidBrush, GdipSetSolidFillColor)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    ARGB color;

    status = GdipCreateSolidFill (0xFF000000, &brush);
    ASSERT_EQ (Ok, status);

    // Set new.
    status = GdipSetSolidFillColor (brush, 0x010203040);
    ASSERT_EQ (Ok, status);

    status = GdipGetSolidFillColor (brush, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x010203040);

    // Set same.
    status = GdipSetSolidFillColor (brush, 0x010203040);
    ASSERT_EQ (Ok, status);

    status = GdipGetSolidFillColor (brush, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x010203040);

    // Negative tests.
    status = GdipSetSolidFillColor (NULL, 0x010203040);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(SolidBrush, fill)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    GpBitmap *bitmap;
    GpGraphics *graphics;
    ARGB color;

    status = GdipCreateSolidFill (0xDDA03040, &brush);
    ASSERT_EQ (Ok, status);

	// PixelFormat32bppARGB.
    status = GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppARGB, NULL, &bitmap);
    ASSERT_EQ (Ok, status);

    status = GdipGetImageGraphicsContext ((GpImage *) bitmap, &graphics);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - CompositingModeSourceOver.
    status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - unchanged, CompositingModeSourceOver.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - changed, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - zero alpha, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x00000000);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    ASSERT_EQ (Ok, status);

    status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - unchanged, CompositingModeSourceCopy.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - changed, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - zero alpha, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x00000000);
	
    GdipDeleteGraphics (graphics);
    GdipDisposeImage ((GpImage *) bitmap);
	
	// PixelFormat32bppPARGB.
    status = GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppPARGB, NULL, &bitmap);
    ASSERT_EQ (Ok, status);

    status = GdipGetImageGraphicsContext ((GpImage *) bitmap, &graphics);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);
	
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    ASSERT_EQ (Ok, status);

    // Fill with brush - CompositingModeSourceOver.
    status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - unchanged, CompositingModeSourceOver.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - changed, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - zero alpha, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x00000000);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    ASSERT_EQ (Ok, status);

    status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - unchanged, CompositingModeSourceCopy.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xDDA0303F);
#else
    assertEqualARGB (color, 0xDDA02F3F);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - changed, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xAABBCCDC);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - zero alpha, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0x00000000);
	
    GdipDeleteGraphics (graphics);
    GdipDisposeImage ((GpImage *) bitmap);

	// PixelFormat32bppRGB.
    status = GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppRGB, NULL, &bitmap);
    ASSERT_EQ (Ok, status);

    status = GdipGetImageGraphicsContext ((GpImage *) bitmap, &graphics);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);
	
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    ASSERT_EQ (Ok, status);

    // Fill with brush - CompositingModeSourceOver.
    status = GdipSetCompositingMode (graphics, CompositingModeSourceOver);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - unchanged, CompositingModeSourceOver.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - changed, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF7D8893);
#else
    assertEqualARGB (color, 0xFF7D8893);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - zero alpha, CompositingModeSourceOver.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xFF000000);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xDDA03040);
    ASSERT_EQ (Ok, status);

    status = GdipSetCompositingMode (graphics, CompositingModeSourceCopy);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - unchanged, CompositingModeSourceCopy.
    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualARGB (color, 0xFF8B2A37);
#else
    assertEqualARGB (color, 0xFF8B2937);
#endif

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - changed, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0xAABBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xFF7D8893);

    status = GdipBitmapSetPixel (bitmap, 0, 0, 0x00000000);
    ASSERT_EQ (Ok, status);

    // Fill with brush - zero alpha, CompositingModeSourceCopy.
    status = GdipSetSolidFillColor (brush, 0x00BBCCDD);
    ASSERT_EQ (Ok, status);

    status = GdipFillRectangle (graphics, brush, 0, 0, 1, 1);
    ASSERT_EQ (Ok, status);

    status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (color, 0xFF000000);
	
    GdipDeleteGraphics (graphics);
    GdipDisposeImage ((GpImage *) bitmap);

    // Negative tests.
    status = GdipSetSolidFillColor (NULL, 0x010203040);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}
