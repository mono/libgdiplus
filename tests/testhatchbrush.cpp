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

TEST(HatchBrush, GdipCloneBrush)
{
    STARTUP

    GpStatus status;
    GpBrush *brush;
    GpBrushType brushType;
    GpBrush *clone;

    GpHatchStyle hatchStyle;
    ARGB foregroundColor;
    ARGB backgroundColor;

    GdipCreateHatchBrush (HatchStyle10Percent, 0x00000001, 0x00000002, (GpHatch **)&brush);

    status = GdipCloneBrush (brush, &clone);
    ASSERT_EQ (Ok, status);
    assert (clone && brush != clone);

    GdipGetBrushType (clone, &brushType);
    ASSERT_EQ (BrushTypeHatchFill, brushType);

    GdipGetHatchStyle ((GpHatch *) clone, &hatchStyle);
    ASSERT_EQ (HatchStyle10Percent, hatchStyle);

    GdipGetHatchForegroundColor ((GpHatch *) clone, &foregroundColor);
    assertEqualARGB (0x00000001, foregroundColor);

    GdipGetHatchBackgroundColor ((GpHatch *) clone, &backgroundColor);
    assertEqualARGB (0x00000002, backgroundColor);

    GdipDeleteBrush (brush);
    GdipDeleteBrush (clone);

    SHUTDOWN
}

TEST(HatchBrush, GdipDeleteBrush)
{
    STARTUP

    GpStatus status;
    GpHatch *brush;

    GdipCreateHatchBrush (HatchStyle05Percent, 0x00000001, 0x00000002, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    ASSERT_EQ (Ok, status);

    SHUTDOWN
}

TEST(HatchBrush, GdipCreateHatchBrush)
{
    STARTUP

    GpStatus status;
    GpHatch *brush;
    GpBrushType brushType;

    status = GdipCreateHatchBrush (HatchStyleMin, 0x00000001, 0x00000002, &brush);
    ASSERT_EQ (Ok, status);
    assert (brush != NULL && "Expected the brush to be initialized.");

    status = GdipGetBrushType ((GpBrush *) brush, &brushType);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (BrushTypeHatchFill, brushType);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateHatchBrush (HatchStyleMax, 0x00000001, 0x00000002, &brush);
    ASSERT_EQ (Ok, status);
    assert (brush != NULL && "Expected the brush to be initialized.");

    GdipDeleteBrush ((GpBrush *)brush);

    // Negative tests.
    brush = (GpHatch *) 0xCC;
    status = GdipCreateHatchBrush (HatchStyle05Percent, 0x00000001, 0x00000002, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpHatch *) 0xCC);

    brush = (GpHatch *) 0xCC;
    status = GdipCreateHatchBrush ((HatchStyle)(HatchStyleMin - 1), 0x00000001, 0x00000002, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpHatch *) 0xCC);

    brush = (GpHatch *) 0xCC;
    status = GdipCreateHatchBrush ((HatchStyle)(HatchStyleMax + 1), 0x00000001, 0x00000002, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpHatch *) 0xCC);

    SHUTDOWN
}

TEST(HatchBrush, GdipGetHatchStyle)
{
    STARTUP

    GpStatus status;
    GpHatch *brush;
    GpHatchStyle hatchStyle;

    GdipCreateHatchBrush (HatchStyle05Percent, 0x00000001, 0x00000002, &brush);

    status = GdipGetHatchStyle (brush, &hatchStyle);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (HatchStyle05Percent, hatchStyle);

    // Negative tests.
    status = GdipGetHatchStyle (NULL, &hatchStyle);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetHatchStyle (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(HatchBrush, GdipGetHatchForegroundColor)
{
    STARTUP

    GpStatus status;
    GpHatch *brush;
    ARGB foregroundColor;

    GdipCreateHatchBrush (HatchStyle05Percent, 0x00000001, 0x00000002, &brush);

    status = GdipGetHatchForegroundColor (brush, &foregroundColor);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (0x00000001, foregroundColor);

    // Negative tests.
    status = GdipGetHatchForegroundColor (NULL, &foregroundColor);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetHatchForegroundColor (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(HatchBrush, GdipGetHatchBackgroundColor)
{
    STARTUP

    GpStatus status;
    GpHatch *brush;
    ARGB backgroundColor;

    GdipCreateHatchBrush (HatchStyle05Percent, 0x00000001, 0x00000002, &brush);

    status = GdipGetHatchBackgroundColor (brush, &backgroundColor);
    ASSERT_EQ (Ok, status);
    assertEqualARGB (0x00000002, backgroundColor);

    // Negative tests.
    status = GdipGetHatchBackgroundColor (NULL, &backgroundColor);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetHatchBackgroundColor (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}
