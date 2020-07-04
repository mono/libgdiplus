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

TEST(Brush, GdipGetBrushType)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;

    GdipCreateSolidFill (1, &brush);

    // Negative tests.
    status = GdipGetBrushType (NULL, &brushType);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetBrushType ((GpBrush *) brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(Brush, GdipCloneBrush)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;
    GpBrush *clone;

    GdipCreateSolidFill (1, &brush);

    // Negative tests.
    status = GdipCloneBrush (NULL, &clone);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCloneBrush ((GpBrush *) brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(Brush, GdipDeleteBrush)
{
    STARTUP

    GpStatus status;
    GpSolidFill *brush;

    GdipCreateSolidFill (1, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    ASSERT_EQ (Ok, status);

    // Negative tests.
    status = GdipDeleteBrush (NULL);
    ASSERT_EQ (InvalidParameter, status);

    SHUTDOWN
}
