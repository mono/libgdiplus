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

static void verifyPathGradientBrush (GpPathGradient *brush, REAL x, REAL y, REAL width, REAL height, ARGB centerColor, REAL centerX, REAL centerY, GpWrapMode expectedWrapMode)
{
    assert (brush && "Expected the brush to be initialized.");

    GpStatus status;
    GpBrushType brushType;
    GpRectF rect;
    GpWrapMode wrapMode;
    GpMatrix *brushTransform;
    ARGB color;
    GpPointF centerPoint;

    GdipCreateMatrix (&brushTransform);

    status = GdipGetBrushType (brush, &brushType);
    ASSERT_EQ (BrushTypePathGradient, brushType);

    status = GdipGetPathGradientRect (brush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (x, rect.X);
    ASSERT_FLOAT_EQ_NAN (y, rect.Y);
    ASSERT_FLOAT_EQ_NAN (width, rect.Width);
    ASSERT_FLOAT_EQ_NAN (height, rect.Height);

    status = GdipGetPathGradientCenterColor (brush, &color);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (centerColor, color);

    status = GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (centerX, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (centerY, centerPoint.Y);

    status = GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (expectedWrapMode, wrapMode);

    status = GdipGetPathGradientTransform (brush, brushTransform);
    ASSERT_EQ (Ok, status);
    verifyMatrix (brushTransform, 1, 0, 0, 1, 0, 0);

    GdipDeleteMatrix (brushTransform);
}

static GpPointF threePoints[3] =
{
    {3, 13},
    {1, 2},
    {5, 6}
};

TEST(PathGradientBrush, GdipCreatePathGradient)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPointF points[2] = {
        {1, 2},
        {-10, 11}
    };
    GpPointF points3[3] = {
        {3, 13},
        {1, 2},
        {5, 6}
    };
    GpPointF zeroWidthPoints[3] = {
        {1, 2},
        {1, 4},
        {1, 6}
    };
    GpPointF zeroHeightPoints[3] = {
        {2, 1},
        {4, 1},
        {6, 1}
    };

    // WrapModeClamp.
    status = GdipCreatePathGradient (points, 2, WrapModeClamp, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeClamp);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipXY.
    status = GdipCreatePathGradient (points, 2, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipY.
    status = GdipCreatePathGradient (points, 2, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeTileFlipY);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipX.
    status = GdipCreatePathGradient (points, 2, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTile.
    status = GdipCreatePathGradient (points3, 3, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xff000000, 3, 7, WrapModeTile);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 2, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 1, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 0, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, -1, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 2, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 3, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 0, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, -1, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 3, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 3, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 0, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, -1, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 3, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 1, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 0, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, -1, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 2, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    status = GdipCreatePathGradient (points, 2, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradient (points, 1, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradient (points, 0, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradient (points, -1, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradient (points, 2, (WrapMode)(WrapModeClamp + 1), NULL);
    ASSERT_EQ (InvalidParameter, status);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipCreatePathGradientI)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPoint points[2] = {
        {1, 2},
        {-10, 11}
    };
    GpPoint points3[3] = {
        {3, 13},
        {1, 2},
        {5, 6}
    };
    GpPoint zeroWidthPoints[3] = {
        {1, 2},
        {1, 4},
        {1, 6}
    };
    GpPoint zeroHeightPoints[3] = {
        {2, 1},
        {4, 1},
        {6, 1}
    };

    // WrapModeClamp.
    status = GdipCreatePathGradientI (points, 2, WrapModeClamp, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeClamp);
    GdipDeleteBrush (brush);

    // WrapModeTileFlipXY.
    status = GdipCreatePathGradientI (points, 2, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush (brush);

    // WrapModeTileFlipY.
    status = GdipCreatePathGradientI (points, 2, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeTileFlipY);
    GdipDeleteBrush (brush);

    // WrapModeTileFlipX.
    status = GdipCreatePathGradientI (points, 2, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush (brush);

    // WrapModeTile.
    status = GdipCreatePathGradientI (points3, 3, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xFF000000, 3, 7, WrapModeTile);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 2, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 1, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 0, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, -1, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 2, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 3, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 0, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, -1, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 3, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 3, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 0, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, -1, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 3, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 1, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 0, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, -1, WrapModeClamp, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    status = GdipCreatePathGradientI (points, 2, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradientI (points, 1, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradientI (points, 0, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradientI (points, -1, WrapModeClamp, NULL);
    ASSERT_EQ (OutOfMemory, status);

    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeClamp + 1), NULL);
    ASSERT_EQ (InvalidParameter, status);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipCreatePathGradientFromPath)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPath *linePath;
    GpPath *emptyPath;

    GdipCreatePath (FillModeWinding, &linePath);
    GdipAddPathRectangle (linePath, 10, 20, 30, 40);
    GdipCreatePath (FillModeWinding, &emptyPath);

    status = GdipCreatePathGradientFromPath (linePath, &brush);
    ASSERT_EQ (Ok, status);
    verifyPathGradientBrush (brush, 10, 20, 30, 40, 0xFFFFFFFF, 25, 40, WrapModeClamp);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientFromPath (NULL, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientFromPath (emptyPath, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    status = GdipCreatePathGradientFromPath (NULL, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradientFromPath (linePath, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreatePathGradientFromPath (emptyPath, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeletePath (linePath);
    GdipDeletePath (emptyPath);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientCenterColor)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    ARGB centerColor;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Negative tests.
    status = GdipGetPathGradientCenterColor (NULL, &centerColor);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientCenterColor (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientCenterColor)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    ARGB centerColor;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientCenterColor (brush, 0x01020304);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterColor (brush, &centerColor);
    ASSERT_EQ (0x01020304, centerColor);

    // Negative tests.
    status = GdipSetPathGradientCenterColor (NULL, 0x01020304);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientSurroundColorCount)
{
    STARTUP

    GpStatus status;
    GpPath *path;
    GpPathGradient *brush;
    INT count;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 3, 4);

    // From three points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, count);

    GdipDeleteBrush ((GpBrush *) brush);

    // From two points.
    GdipCreatePathGradient (threePoints, 2, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, count);

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (4, count);

    // Negative tests
    status = GdipGetPathGradientSurroundColorCount (NULL, &count);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientSurroundColorCount (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientSurroundColorsWithCount)
{
    STARTUP

    GpStatus status;
    GpPath *path;
    GpPathGradient *brush;
    ARGB colors[5] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
    INT count = 3;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 3, 4);

    // From points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);
    ASSERT_EQ (0xFFFFFFFF, colors[0]);
    ASSERT_EQ (0xFFFFFFFF, colors[1]);
    ASSERT_EQ (0xFFFFFFFF, colors[2]);
    ASSERT_EQ (0x00000000, colors[3]);
    ASSERT_EQ (0x00000000, colors[4]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;
    colors[4] = 0x00000000;

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    count = 100;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);
    ASSERT_EQ (0xFFFFFFFF, colors[0]);
    ASSERT_EQ (0xFFFFFFFF, colors[1]);
    ASSERT_EQ (0xFFFFFFFF, colors[2]);
    ASSERT_EQ (0xFFFFFFFF, colors[3]);
    ASSERT_EQ (0x00000000, colors[4]);

    // Negative tests
    status = GdipGetPathGradientSurroundColorsWithCount (NULL, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, NULL, &count);
    ASSERT_EQ (InvalidParameter, status);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, NULL);
    ASSERT_EQ (InvalidParameter, status);
#endif

    count = 2;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    count = 1;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    count = 0;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    count = -1;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientSurroundColorsWithCount)
{
    STARTUP

    GpStatus status;
    GpPath *path;
    GpPathGradient *brush;
    ARGB threeSurroundColors[3] = {0x0000001, 0x0000002, 0x0000003};
    ARGB threeEmptyColors[3] = {0x00000000, 0x00000000, 0x00000000};
    ARGB twoSameColors[2] = {0x0000001, 0x0000001};
    ARGB colors[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
    INT count = 3;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 3, 4);
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Three surround colors.
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, count);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (3, count);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, count);
    ASSERT_EQ (0x000000001, colors[0]);
    ASSERT_EQ (0x000000002, colors[1]);
    ASSERT_EQ (0x000000003, colors[2]);
    ASSERT_EQ (0x000000000, colors[3]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // Same.
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, count);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (3, count);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, count);
    ASSERT_EQ (0x000000001, colors[0]);
    ASSERT_EQ (0x000000002, colors[1]);
    ASSERT_EQ (0x000000003, colors[2]);
    ASSERT_EQ (0x000000000, colors[3]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // Two empty colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeEmptyColors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, count);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (3, count);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);
    ASSERT_EQ (0x000000000, colors[0]);
    ASSERT_EQ (0x000000000, colors[1]);
    ASSERT_EQ (0x000000000, colors[2]);
    ASSERT_EQ (0x000000000, colors[3]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // Two same colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, twoSameColors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, count);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (3, count);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);
    ASSERT_EQ (0x00000001, colors[0]);
    ASSERT_EQ (0x00000001, colors[1]);
    ASSERT_EQ (0x00000001, colors[2]);
    ASSERT_EQ (0x00000000, colors[3]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    GdipDeleteBrush ((GpBrush *) brush);

    // One surround color.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    count = 1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    ASSERT_EQ (Ok, status);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (3, count);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);
    ASSERT_EQ (0x00000001, colors[0]);
    ASSERT_EQ (0x00000001, colors[1]);
    ASSERT_EQ (0x00000001, colors[2]);
    ASSERT_EQ (0x00000000, colors[3]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    ASSERT_EQ (4, count);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, count);
    ASSERT_EQ (0x00000001, colors[0]);
    ASSERT_EQ (0x00000001, colors[1]);
    ASSERT_EQ (0x00000001, colors[2]);
    ASSERT_EQ (0x00000001, colors[3]);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientSurroundColorsWithCount (NULL, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, NULL, &count);
    ASSERT_EQ (InvalidParameter, status);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, NULL);
    ASSERT_EQ (InvalidParameter, status);
#endif

    count = 4;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    count = 0;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    count = -1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientPath)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPath *path;

    GdipCreatePath (FillModeAlternate, &path);
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPath (brush, path);
    ASSERT_EQ (NotImplemented, status);

    status = GdipGetPathGradientPath (NULL, path);
    ASSERT_EQ (NotImplemented, status);

    status = GdipGetPathGradientPath (brush, NULL);
    ASSERT_EQ (NotImplemented, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientPath)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPath *path;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreatePath (FillModeWinding, &path);

    status = GdipSetPathGradientPath (brush, path);
    ASSERT_EQ (NotImplemented, status);

    status = GdipSetPathGradientPath (NULL, path);
    ASSERT_EQ (NotImplemented, status);

    status = GdipSetPathGradientPath (brush, NULL);
    ASSERT_EQ (NotImplemented, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientCenterPoint)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPointF centerPoint;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (3, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (7, centerPoint.Y);

    status = GdipGetPathGradientCenterPoint (NULL, &centerPoint);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientCenterPoint (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientCenterPointI)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPoint centerPoint;
    GpPointF roundUpPoints[3] = {
        {1, 2},
        {8, 8},
        {17, 10}
    };

    // <= 0.5 is rounded down.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientCenterPointI (brush, &centerPoint);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, centerPoint.X);
    ASSERT_EQ (7, centerPoint.Y);
    GdipDeleteBrush ((GpBrush *) brush);

    // > 0.5 is rounded up.
    GdipCreatePathGradient (roundUpPoints, 3, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientCenterPointI (brush, &centerPoint);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (9, centerPoint.X);
    ASSERT_EQ (7, centerPoint.Y);

    // Negative tests.
    status = GdipGetPathGradientCenterPointI (NULL, &centerPoint);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientCenterPointI (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientCenterPoint)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPointF point = {1, 2};
    GpPointF outOfBoundsPoint = {100, 200};
    GpPointF centerPoint;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Set within the bounds of the brush.
    status = GdipSetPathGradientCenterPoint (brush, &point);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_FLOAT_EQ_NAN (1, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (2, centerPoint.Y);

    // Set same.
    status = GdipSetPathGradientCenterPoint (brush, &point);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_FLOAT_EQ_NAN (1, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (2, centerPoint.Y);

    // Set outside the bounds of the brush.
    status = GdipSetPathGradientCenterPoint (brush, &outOfBoundsPoint);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_FLOAT_EQ_NAN (100, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (200, centerPoint.Y);

    // Negative tests.
    status = GdipSetPathGradientCenterPoint (NULL, &point);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientCenterPoint (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientCenterPointI)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpPoint point = {1, 2};
    GpPoint outOfBoundsPoint = {100, 200};
    GpPointF centerPoint;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Set within the bounds of the brush.
    status = GdipSetPathGradientCenterPointI (brush, &point);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_FLOAT_EQ_NAN (1, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (2, centerPoint.Y);

    // Set same.
    status = GdipSetPathGradientCenterPointI (brush, &point);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_FLOAT_EQ_NAN (1, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (2, centerPoint.Y);

    // Set outside the bounds of the brush.
    status = GdipSetPathGradientCenterPointI (brush, &outOfBoundsPoint);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    ASSERT_FLOAT_EQ_NAN (100, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (200, centerPoint.Y);

    // Negative tests.
    status = GdipSetPathGradientCenterPointI (NULL, &point);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientCenterPointI (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientRect)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpRectF rect;
    GpPointF points[2] = {
        {1.5, 2.5},
        {5, 7}
    };

    GdipCreatePathGradient (points, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRect (brush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1.5, rect.X);
    ASSERT_FLOAT_EQ_NAN (2.5, rect.Y);
    ASSERT_FLOAT_EQ_NAN (3.5, rect.Width);
    ASSERT_FLOAT_EQ_NAN (4.5, rect.Height);

    // Negative tests.
    status = GdipGetPathGradientRect (NULL, &rect);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientRect (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientRectI)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpRect rect;
    GpPointF points1[2] = {
        {1.6f, 2.6f},
        {5.2f, 7.2f}
    };
    GpPointF points2[2] = {
        {1.5f, 2.5f},
        {5, 7}
    };
    GpPointF points3[2] = {
        {1.4f, 2.4f},
        {4.8f, 6.8f}
    };

    // > 0.5 is rounded up.
    GdipCreatePathGradient (points1, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, rect.X);
    ASSERT_EQ (3, rect.Y);
    ASSERT_EQ (4, rect.Width);
    ASSERT_EQ (5, rect.Height);

    GdipDeleteBrush ((GpBrush *) brush);

    // 0.5 is rounded up.
    GdipCreatePathGradient (points2, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, rect.X);
    ASSERT_EQ (3, rect.Y);
    ASSERT_EQ (4, rect.Width);
    ASSERT_EQ (5, rect.Height);

    GdipDeleteBrush ((GpBrush *) brush);

    // < 0.5 is rounded down.
    GdipCreatePathGradient (points3, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, rect.X);
    ASSERT_EQ (2, rect.Y);
    ASSERT_EQ (3, rect.Width);
    ASSERT_EQ (4, rect.Height);

    // Negative tests.
    status = GdipGetPathGradientRectI (NULL, &rect);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientRectI (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientPointCount)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    INT pointCount;
    GpPath *path;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 3, 4);

    // From points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPointCount (brush, &pointCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, pointCount);

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipGetPathGradientPointCount (brush, &pointCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (4, pointCount);

    // Negative tests.
    status = GdipGetPathGradientPointCount (NULL, &pointCount);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPointCount (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientGammaCorrection)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    BOOL useGammaCorrection;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Set true.
    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    ASSERT_TRUE (useGammaCorrection);

    // Set same.
    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    ASSERT_TRUE (useGammaCorrection);

    // Set false.
    status = GdipSetPathGradientGammaCorrection (brush, FALSE);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    ASSERT_FALSE (useGammaCorrection);

    // Negative tests.
    status = GdipSetPathGradientGammaCorrection (NULL, TRUE);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientGammaCorrection)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    BOOL useGammaCorrection;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    ASSERT_FALSE (useGammaCorrection);

    // Negative tests.
    status = GdipGetPathGradientGammaCorrection (NULL, &useGammaCorrection);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientGammaCorrection (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientBlendCount)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientBlendCount (brush, &blendCount);
    ASSERT_EQ (1, blendCount);

    // Negative tests.
    status = GdipGetPathGradientBlendCount (NULL, &blendCount);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientBlendCount (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

static void fill_array (REAL *array, INT count, REAL value)
{
    for (int i = 0; i < count; i++)
        array[i] = value;
}

TEST(PathGradientBrush, GdipGetPathGradientBlend)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    REAL blend[3];
    REAL positions[3];
    REAL threeBlends[3] = {1, 2, 3};
    REAL threePositions[3] = {0, 0.5, 1};

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Default blend - equal count.
    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (123, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    // Positions are meaningless for blends with a count of 1 so this parameter is ignored.
    ASSERT_FLOAT_EQ_NAN (123, positions[0]);
    ASSERT_FLOAT_EQ_NAN (123, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Default blend - larger count.
    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (123, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    // Positions are meaningless for blends with a count of 1 so this parameter is ignored.
    ASSERT_FLOAT_EQ_NAN (123, positions[0]);
    ASSERT_FLOAT_EQ_NAN (123, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Negative tests.
    GdipSetPathGradientBlend (brush, threeBlends, threePositions, 3);
    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    ASSERT_EQ (InsufficientBuffer, status);

    status = GdipGetPathGradientBlend (NULL, blend, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientBlend (brush, NULL, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientBlend (brush, blend, NULL, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientBlend (brush, blend, positions, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientBlend (brush, blend, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientBlend)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;

    REAL blend[3]= {1, 2, 3};
    REAL positions[3] = {0, 0.5f, 1.0f};
    REAL twoBlends[2] = {1, 2};
    REAL twoPositions[2] = {0, 1};
    REAL threeBlends[3] = {1, 2, 3};
    REAL threePositions[3] = {0, 0.5, 1};
    REAL oneBlend[1] = {10};
    REAL onePosition[1] = {-12};
    ARGB pathPresetBlend[3] = {1, 2, 3};
    REAL pathPresetPositions[3] = {0, 0.5f, 1.0f};
    INT presetBlendCount;

    REAL invalidPositions1[2] = {0.5, 1};
    REAL invalidPositions2[2] = {0, 0.5};

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Two blends - equal count.
    status = GdipSetPathGradientBlend (brush, twoBlends, twoPositions, 2);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (2, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    ASSERT_FLOAT_EQ_NAN (0, positions[0]);
    ASSERT_FLOAT_EQ_NAN (1, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Set same.
    status = GdipSetPathGradientBlend (brush, twoBlends, twoPositions, 2);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (2, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    ASSERT_FLOAT_EQ_NAN (0, positions[0]);
    ASSERT_FLOAT_EQ_NAN (1, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Three blends - equal count.
    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, 3);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 3);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (2, blend[1]);
    ASSERT_FLOAT_EQ_NAN (3, blend[2]);
    ASSERT_FLOAT_EQ_NAN (0, positions[0]);
    ASSERT_FLOAT_EQ_NAN (0.5, positions[1]);
    ASSERT_FLOAT_EQ_NAN (1, positions[2]);

    // One blend - equal count.
    status = GdipSetPathGradientBlend (brush, oneBlend, onePosition, 1);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (10, blend[0]);
    ASSERT_FLOAT_EQ_NAN (123, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    ASSERT_FLOAT_EQ_NAN (123, positions[0]);
    ASSERT_FLOAT_EQ_NAN (123, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Should clear the existing blend.
    status = GdipSetPathGradientPresetBlend (brush, pathPresetBlend, pathPresetPositions, 3);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, 3);
    ASSERT_EQ (Ok, status);

    status = GdipGetPathGradientPresetBlendCount (brush, &presetBlendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (0, presetBlendCount);

    // Negative tests.
    status = GdipSetPathGradientBlend (NULL, threeBlends, threePositions, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientBlend (brush, NULL, threePositions, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientBlend (brush, threeBlends, NULL, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientBlend (brush, twoBlends, invalidPositions1, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientBlend (brush, twoBlends, invalidPositions2, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientPresetBlendCount)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPresetBlendCount (brush, &blendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (0, blendCount);

    // Negative tests.
    status = GdipGetPathGradientPresetBlendCount (NULL, &blendCount);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlendCount (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientPresetBlend)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    ARGB blend[2];
    REAL positions[2];

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Negative tests.
    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 2);
    ASSERT_EQ (GenericError, status);

    status = GdipGetPathGradientPresetBlend (NULL, blend, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, NULL, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, blend, NULL, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, -1);
    ASSERT_EQ (OutOfMemory, status);

    status = GdipGetPathGradientPresetBlend (NULL, blend, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, NULL, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientPresetBlend (brush, blend, NULL, -1);
    ASSERT_EQ (OutOfMemory, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientPresetBlend)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;

    ARGB blend2[2] = {1, 0};
    REAL positions2[2] = {0, 1.0f};
    ARGB destBlend2[2];
    REAL destPositions2[2];

    ARGB blend3[3] = {1, 2, 3};
    REAL positions3[3] = {0, 0.5f, 1.0f};
    ARGB destBlend3[3];
    REAL destPositions3[3];

#if !defined(USE_WINDOWS_GDIPLUS)
    REAL destBlendReal[2];
    REAL destPositionsReal[2];
#endif

    REAL invalidPositions1[2] = {0.5, 1};
    REAL invalidPositions2[2] = {0, 0.5};

    REAL pathBlend[3] = {1, 2, 3};
    REAL pathPositions[3] = {0, 2, 1};
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Count of 2.
    status = GdipSetPathGradientPresetBlend (brush, blend2, positions2, 2);
    ASSERT_EQ (Ok, status);

    status = GdipGetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, destBlend2[0]);
    ASSERT_EQ (0, destBlend2[1]);
    ASSERT_FLOAT_EQ_NAN (0, destPositions2[0]);
    ASSERT_FLOAT_EQ_NAN (1, destPositions2[1]);

    // Same.
    status = GdipSetPathGradientPresetBlend (brush, blend2, positions2, 2);
    ASSERT_EQ (Ok, status);

    status = GdipGetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, destBlend2[0]);
    ASSERT_EQ (0, destBlend2[1]);
    ASSERT_FLOAT_EQ_NAN (0, destPositions2[0]);
    ASSERT_FLOAT_EQ_NAN (1, destPositions2[1]);

    // Count of 3.
    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 3);
    ASSERT_EQ (Ok, status);

    status = GdipGetPathGradientPresetBlend (brush, destBlend3, destPositions3, 3);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, destBlend3[0]);
    ASSERT_EQ (2, destBlend3[1]);
    ASSERT_EQ (3, destBlend3[2]);
    ASSERT_FLOAT_EQ_NAN (0, destPositions3[0]);
    ASSERT_FLOAT_EQ_NAN (0.5, destPositions3[1]);
    ASSERT_FLOAT_EQ_NAN (1, destPositions3[2]);

    // Should clear the existing blend.
    status = GdipSetPathGradientBlend (brush, pathBlend, pathPositions, 3);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    ASSERT_EQ (Ok, status);

    status = GdipGetPathGradientBlendCount (brush, &blendCount);
    ASSERT_EQ (Ok, status);
    // GDI+ has a bug where it sets the blend and positions to NULL but incorrectly
    // decrements blendCount, instead of setting it to zero.
    // This causes GDI+ to crash. This is a bug we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    ASSERT_EQ (0, blendCount);

    status = GdipGetPathGradientBlend(brush, destBlendReal, destPositionsReal, blendCount);
    ASSERT_EQ (InvalidParameter, status);
#endif

    // Negative tests.
    status = GdipSetPathGradientPresetBlend (NULL, blend3, positions3, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientPresetBlend (brush, NULL, positions3, 3);
    ASSERT_EQ (InvalidParameter, status);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetPathGradientPresetBlend (brush, blend3, NULL, 3);
    ASSERT_EQ (InvalidParameter, status);
#endif

    status = GdipSetPathGradientPresetBlend (brush, blend2, invalidPositions1, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientPresetBlend (brush, blend2, invalidPositions2, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientSigmaBlend)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientSigmaBlend (brush, 0, 0.5f);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, 1);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientSigmaBlend (brush, 1, 0);
    ASSERT_EQ (Ok, status);

    // Negative tests.
    status = GdipSetPathGradientSigmaBlend (NULL, 0.5f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientSigmaBlend (brush, -0.01f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientSigmaBlend (brush, 1.01f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, -0.01f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, 1.01f);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientLinearBlend)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientLinearBlend (brush, 0, 0.5f);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, 1);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientLinearBlend (brush, 1, 0);
    ASSERT_EQ (Ok, status);

    // Negative tests.
    status = GdipSetPathGradientLinearBlend (NULL, 0.5f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientLinearBlend (brush, -0.01f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientLinearBlend (brush, 1.01f, 0.5);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, -0.01f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, 1.01f);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientWrapMode)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpWrapMode wrapMode;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Negative tests.
    status = GdipGetPathGradientWrapMode (NULL, &wrapMode);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientWrapMode (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientWrapMode)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpWrapMode wrapMode;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // WrapModeTile.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTile);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTile, wrapMode);

    // Same.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTile);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTile, wrapMode);

    // WrapModeTileFlipX.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipX);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipX, wrapMode);

    // WrapModeTileFlipX.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipY);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipY, wrapMode);

    // WrapModeTileFlipXY.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipXY);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipXY, wrapMode);

    // WrapModeClamp.
    status = GdipSetPathGradientWrapMode (brush, WrapModeClamp);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeClamp, wrapMode);

    // Invalid WrapMode - nop.
    status = GdipSetPathGradientWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeClamp, wrapMode);

    // Negative tests.
    status = GdipSetPathGradientWrapMode (NULL, WrapModeTile);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *transform;
    ARGB blend[2] = {1, 0};
    REAL positions[2] = {0, 1.0f};

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix (&transform);

    // With no preset blend.
    status = GdipGetPathGradientTransform (brush, transform);
    ASSERT_EQ (Ok, status);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // With preset blend.
    GdipSetPathGradientPresetBlend (brush, blend, positions, 2);
    status = GdipGetPathGradientTransform (brush, transform);
    ASSERT_EQ (Ok, status);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipGetPathGradientTransform (NULL, transform);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientTransform (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *matrix;
    GpMatrix *nonInvertibleMatrix;
    GpMatrix *transform;
    ARGB blend[2] = {1, 0};
    REAL positions[2] = {0, 1.0f};

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    // With no preset blend.
    status = GdipSetPathGradientTransform (brush, matrix);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Same.
    status = GdipSetPathGradientTransform (brush, matrix);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // With preset blend.
    GdipSetPathGradientPresetBlend (brush, blend, positions, 2);
    status = GdipSetPathGradientTransform (brush, matrix);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 2, 3, 4, 5, 6, 7);

    // Negative tests.
    status = GdipSetPathGradientTransform (NULL, transform);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientTransform (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetPathGradientTransform (brush, nonInvertibleMatrix);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipResetPathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *matrix;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    GdipSetPathGradientTransform (brush, matrix);

    status = GdipResetPathGradientTransform (brush);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipResetPathGradientTransform (NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipMultiplyPathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *originalTransform;
    GpMatrix *matrix;
    GpMatrix *nonInvertibleMatrix;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix - nop.
    status = GdipMultiplyPathGradientTransform (brush, NULL, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - negative.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Invalid MatrixOrder - positive.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Negative tests.
    status = GdipMultiplyPathGradientTransform (NULL, matrix, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipMultiplyPathGradientTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipTranslatePathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *originalTransform;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipTranslatePathGradientTransform (brush, 5, 6, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipTranslatePathGradientTransform (brush, 5, 6, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslatePathGradientTransform (NULL, 1, 1, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipTranslatePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipTranslatePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipScalePathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *originalTransform;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipScalePathGradientTransform (brush, 0.5, 0.75, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipScalePathGradientTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScalePathGradientTransform (NULL, 1, 1, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipScalePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipScalePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, rotatePathGradientTransform)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *originalTransform;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipRotatePathGradientTransform (brush, 90, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipRotatePathGradientTransform (brush, 90, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotatePathGradientTransform (NULL, 90, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRotatePathGradientTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRotatePathGradientTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipGetPathGradientFocusScales)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    REAL xScale;
    REAL yScale;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (0, xScale);
    ASSERT_FLOAT_EQ_NAN (0, yScale);

    // Negative tests.
    status = GdipGetPathGradientFocusScales (NULL, &xScale, &yScale);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientFocusScales (NULL, &xScale, &yScale);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientFocusScales (brush, NULL, &yScale);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetPathGradientFocusScales (brush, &xScale, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipSetPathGradientFocusScales)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    REAL xScale;
    REAL yScale;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Positive.
    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    ASSERT_FLOAT_EQ_NAN (1, xScale);
    ASSERT_FLOAT_EQ_NAN (2, yScale);

    // Same.
    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    ASSERT_FLOAT_EQ_NAN (1, xScale);
    ASSERT_FLOAT_EQ_NAN (2, yScale);

    // Zero.
    status = GdipSetPathGradientFocusScales (brush, 0, 0);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    ASSERT_FLOAT_EQ_NAN (0, xScale);
    ASSERT_FLOAT_EQ_NAN (0, yScale);

    // Negative,
    status = GdipSetPathGradientFocusScales (brush, -1, -2);
    ASSERT_EQ (Ok, status);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    ASSERT_FLOAT_EQ_NAN (-1, xScale);
    ASSERT_FLOAT_EQ_NAN (-2, yScale);

    // Negative tests.
    status = GdipSetPathGradientFocusScales (NULL, 0, 0);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipCloneWithPoints)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpBrush *clonedBrush;

    GpBrushType brushType;
    GpWrapMode wrapMode;
    REAL xScale;
    REAL yScale;
    ARGB centerColor;
    GpPointF centerPoint;
    GpRectF rect;
    INT pointCount;
    REAL blend[2] = {1, 2};
    REAL positions[2] = {0, 1};
    INT blendCount;
    REAL blendResult[2];
    REAL positionsResult[2];
    BOOL useGammaCorrection;
    GpMatrix *transform;
    GpMatrix *matrix;
    INT surroundColorsCount = 3;
    ARGB surroundColors[3] = {1, 2, 3};
    ARGB surroundColorsResult[3];

    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);
    GdipCreateMatrix (&matrix);
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientCenterColor (brush, 3);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientTransform (brush, transform);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, surroundColors, &surroundColorsCount);
    ASSERT_EQ (Ok, status);

    status = GdipCloneBrush ((GpBrush *) brush, &clonedBrush);
    ASSERT_EQ (Ok, status);
    assert (clonedBrush && clonedBrush != brush);

    status = GdipGetBrushType (clonedBrush, &brushType);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (BrushTypePathGradient, brushType);

    status = GdipGetPathGradientWrapMode ((GpPathGradient *) clonedBrush, &wrapMode);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (WrapModeTileFlipX, wrapMode);

    status = GdipGetPathGradientFocusScales ((GpPathGradient *) clonedBrush, &xScale, &yScale);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, xScale);
    ASSERT_FLOAT_EQ_NAN (2, yScale);

    status = GdipGetPathGradientCenterColor ((GpPathGradient *) clonedBrush, &centerColor);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, centerColor);

    status = GdipGetPathGradientCenterPoint ((GpPathGradient *) clonedBrush, &centerPoint);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (3, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (7, centerPoint.Y);

    status = GdipGetPathGradientRect ((GpPathGradient *) clonedBrush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, rect.X);
    ASSERT_FLOAT_EQ_NAN (2, rect.Y);
    ASSERT_FLOAT_EQ_NAN (4, rect.Width);
    ASSERT_FLOAT_EQ_NAN (11, rect.Height);

    status = GdipGetPathGradientPointCount ((GpPathGradient *) clonedBrush, &pointCount);
    ASSERT_EQ (3, pointCount);

    status = GdipGetPathGradientBlendCount ((GpPathGradient *) clonedBrush, &blendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, blendCount);

    status = GdipGetPathGradientBlend ((GpPathGradient *) clonedBrush, blendResult, positionsResult, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blendResult[0]);
    ASSERT_FLOAT_EQ_NAN (2, blendResult[1]);
    ASSERT_FLOAT_EQ_NAN (0, positionsResult[0]);
    ASSERT_FLOAT_EQ_NAN (1, positionsResult[1]);

    status = GdipGetPathGradientGammaCorrection ((GpPathGradient *) clonedBrush, &useGammaCorrection);
    ASSERT_EQ (Ok, status);
    ASSERT_TRUE (useGammaCorrection);

    status = GdipGetPathGradientTransform ((GpPathGradient *) clonedBrush, matrix);
    ASSERT_EQ (Ok, status);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    status = GdipGetPathGradientSurroundColorCount (brush, &surroundColorsCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, surroundColorsCount);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, surroundColorsResult, &surroundColorsCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, surroundColorsResult[0]);
    ASSERT_EQ (2, surroundColorsResult[1]);
    ASSERT_EQ (3, surroundColorsResult[2]);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipCloneWithPath)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;
    GpBrush *clonedBrush;

    GpBrushType brushType;
    GpWrapMode wrapMode;
    REAL xScale;
    REAL yScale;
    ARGB centerColor;
    GpPointF centerPoint;
    GpRectF rect;
    INT pointCount;
    REAL blend[2] = {1, 2};
    REAL positions[2] = {0, 1};
    INT blendCount;
    REAL blendResult[2];
    REAL positionsResult[2];
    BOOL useGammaCorrection;
    GpMatrix *transform;
    GpMatrix *matrix;
    INT surroundColorsCount = 4;
    ARGB surroundColors[4] = {1, 2, 3, 4};
    ARGB surroundColorsResult[4];
    GpPath *path;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 4, 11);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);
    GdipCreateMatrix (&matrix);
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientCenterColor (brush, 3);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientTransform (brush, transform);
    ASSERT_EQ (Ok, status);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, surroundColors, &surroundColorsCount);
    ASSERT_EQ (Ok, status);

    status = GdipCloneBrush ((GpBrush *) brush, &clonedBrush);
    ASSERT_EQ (Ok, status);
    assert (clonedBrush && clonedBrush != brush);

    status = GdipGetBrushType (clonedBrush, &brushType);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (BrushTypePathGradient, brushType);

    status = GdipGetPathGradientWrapMode ((GpPathGradient *) clonedBrush, &wrapMode);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (WrapModeClamp, wrapMode);

    status = GdipGetPathGradientFocusScales ((GpPathGradient *) clonedBrush, &xScale, &yScale);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, xScale);
    ASSERT_FLOAT_EQ_NAN (2, yScale);

    status = GdipGetPathGradientCenterColor ((GpPathGradient *) clonedBrush, &centerColor);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, centerColor);

    status = GdipGetPathGradientCenterPoint ((GpPathGradient *) clonedBrush, &centerPoint);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (3, centerPoint.X);
    ASSERT_FLOAT_EQ_NAN (7.5, centerPoint.Y);

    status = GdipGetPathGradientRect ((GpPathGradient *) clonedBrush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, rect.X);
    ASSERT_FLOAT_EQ_NAN (2, rect.Y);
    ASSERT_FLOAT_EQ_NAN (4, rect.Width);
    ASSERT_FLOAT_EQ_NAN (11, rect.Height);

    status = GdipGetPathGradientPointCount ((GpPathGradient *) clonedBrush, &pointCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (4, pointCount);

    status = GdipGetPathGradientBlendCount ((GpPathGradient *) clonedBrush, &blendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, blendCount);

    status = GdipGetPathGradientBlend ((GpPathGradient *) clonedBrush, blendResult, positionsResult, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blendResult[0]);
    ASSERT_FLOAT_EQ_NAN (2, blendResult[1]);
    ASSERT_FLOAT_EQ_NAN (0, positionsResult[0]);
    ASSERT_FLOAT_EQ_NAN (1, positionsResult[1]);

    status = GdipGetPathGradientGammaCorrection ((GpPathGradient *) clonedBrush, &useGammaCorrection);
    ASSERT_EQ (Ok, status);
    ASSERT_TRUE (useGammaCorrection);

    status = GdipGetPathGradientTransform ((GpPathGradient *) clonedBrush, matrix);
    ASSERT_EQ (Ok, status);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    status = GdipGetPathGradientSurroundColorCount (brush, &surroundColorsCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (4, surroundColorsCount);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, surroundColorsResult, &surroundColorsCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, surroundColorsResult[0]);
    ASSERT_EQ (2, surroundColorsResult[1]);
    ASSERT_EQ (3, surroundColorsResult[2]);
    ASSERT_EQ (4, surroundColorsResult[3]);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeletePath (path);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);

    SHUTDOWN
}

TEST(PathGradientBrush, GdipDeleteBrush)
{
    STARTUP

    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    ASSERT_EQ (Ok, status);

    SHUTDOWN
}
