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
    assertEqualInt (brushType, BrushTypePathGradient);

    status = GdipGetPathGradientRect (brush, &rect);
    assertEqualInt (status, Ok);
    assertEqualFloat (rect.X, x);
    assertEqualFloat (rect.Y, y);
    assertEqualFloat (rect.Width, width);
    assertEqualFloat (rect.Height, height);

    status = GdipGetPathGradientCenterColor (brush, &color);
    assertEqualInt (status, Ok);
    assertEqualInt (color, centerColor);

    status = GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualInt (status, Ok);
    assertEqualFloat (centerPoint.X, centerX);
    assertEqualFloat (centerPoint.Y, centerY);

    status = GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (status, Ok);
    assertEqualInt (wrapMode, expectedWrapMode);

    status = GdipGetPathGradientTransform (brush, brushTransform);
    assertEqualInt (status, Ok);
    verifyMatrix (brushTransform, 1, 0, 0, 1, 0, 0);

    GdipDeleteMatrix (brushTransform);
}

static GpPointF threePoints[3] =
{
    {3, 13},
    {1, 2},
    {5, 6}
};

static void test_createPathGradient ()
{
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
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeClamp);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipXY.
    status = GdipCreatePathGradient (points, 2, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipY.
    status = GdipCreatePathGradient (points, 2, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeTileFlipY);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipX.
    status = GdipCreatePathGradient (points, 2, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTile.
    status = GdipCreatePathGradient (points3, 3, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xff000000, 3, 7, WrapModeTile);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 2, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 0, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, -1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (NULL, 2, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 3, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 0, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, -1, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroWidthPoints, 3, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 3, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 0, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, -1, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (zeroHeightPoints, 3, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 0, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, -1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 2, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradient (points, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    status = GdipCreatePathGradient (points, 2, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradient (points, 1, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradient (points, 0, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradient (points, -1, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradient (points, 2, (WrapMode)(WrapModeClamp + 1), NULL);
    assertEqualInt (status, InvalidParameter);
}

static void test_createPathGradientI ()
{
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
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeClamp);
    GdipDeleteBrush (brush);

    // WrapModeTileFlipXY.
    status = GdipCreatePathGradientI (points, 2, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush (brush);

    // WrapModeTileFlipY.
    status = GdipCreatePathGradientI (points, 2, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeTileFlipY);
    GdipDeleteBrush (brush);

    // WrapModeTileFlipX.
    status = GdipCreatePathGradientI (points, 2, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xFF000000, -4.5, 6.5, WrapModeTileFlipX);
    GdipDeleteBrush (brush);

    // WrapModeTile.
    status = GdipCreatePathGradientI (points3, 3, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xFF000000, 3, 7, WrapModeTile);

    // Negative tests.
    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 2, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 1, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 0, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, -1, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (NULL, 2, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 3, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 0, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, -1, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroWidthPoints, 3, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 3, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 0, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, -1, WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 3, (WrapMode)(WrapModeClamp + 1), NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (zeroHeightPoints, 3, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 0, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, -1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == (GpPathGradient *) 0xCC);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    status = GdipCreatePathGradientI (points, 2, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientI (points, 1, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientI (points, 0, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientI (points, -1, WrapModeClamp, NULL);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeClamp + 1), NULL);
    assertEqualInt (status, InvalidParameter);
}

static void test_createPathGradientFromPath ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPath *linePath;
    GpPath *emptyPath;

    GdipCreatePath (FillModeWinding, &linePath);
    GdipAddPathRectangle (linePath, 10, 20, 30, 40);
    GdipCreatePath (FillModeWinding, &emptyPath);

    status = GdipCreatePathGradientFromPath (linePath, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, 10, 20, 30, 40, 0xFFFFFFFF, 25, 40, WrapModeClamp);

    // Negative tests.
    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientFromPath (NULL, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpPathGradient *) 0xCC;
    status = GdipCreatePathGradientFromPath (emptyPath, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    status = GdipCreatePathGradientFromPath (NULL, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientFromPath (linePath, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientFromPath (emptyPath, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (linePath);
    GdipDeletePath (emptyPath);
}

static void test_getPathGradientCenterColor ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB centerColor;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Negative tests.
    status = GdipGetPathGradientCenterColor (NULL, &centerColor);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientCenterColor (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientCenterColor ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB centerColor;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientCenterColor (brush, 0x01020304);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterColor (brush, &centerColor);
    assertEqualInt (centerColor, 0x01020304);

    // Negative tests.
    status = GdipSetPathGradientCenterColor (NULL, 0x01020304);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientSurroundColorCount ()
{
    GpStatus status;
    GpPath *path;
    GpPathGradient *brush;
    INT count;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 3, 4);

    // From three points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 3);

    GdipDeleteBrush ((GpBrush *) brush);

    // From two points.
    GdipCreatePathGradient (threePoints, 2, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 4);

    // Negative tests
    status = GdipGetPathGradientSurroundColorCount (NULL, &count);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientSurroundColorCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);
}

static void test_getPathGradientSurroundColorsWithCount ()
{
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
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 0xFFFFFFFF);
    assertEqualInt (colors[1], 0xFFFFFFFF);
    assertEqualInt (colors[2], 0xFFFFFFFF);
    assertEqualInt (colors[3], 0x00000000);
    assertEqualInt (colors[4], 0x00000000);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;
    colors[4] = 0x00000000;

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    count = 100;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 0xFFFFFFFF);
    assertEqualInt (colors[1], 0xFFFFFFFF);
    assertEqualInt (colors[2], 0xFFFFFFFF);
    assertEqualInt (colors[3], 0xFFFFFFFF);
    assertEqualInt (colors[4], 0x00000000);

    // Negative tests
    status = GdipGetPathGradientSurroundColorsWithCount (NULL, colors, &count);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, NULL, &count);
    assertEqualInt (status, InvalidParameter);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, NULL);
    assertEqualInt (status, InvalidParameter);
#endif

    count = 2;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    count = 1;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    count = 0;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    count = -1;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);
}

static void test_setPathGradientSurroundColorsWithCount ()
{
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
    assertEqualInt (status, Ok);
    assertEqualInt (count, 3);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (count, 3);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 3);
    assertEqualInt (colors[0], 0x000000001);
    assertEqualInt (colors[1], 0x000000002);
    assertEqualInt (colors[2], 0x000000003);
    assertEqualInt (colors[3], 0x000000000);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // Same.
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 3);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (count, 3);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 3);
    assertEqualInt (colors[0], 0x000000001);
    assertEqualInt (colors[1], 0x000000002);
    assertEqualInt (colors[2], 0x000000003);
    assertEqualInt (colors[3], 0x000000000);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // Two empty colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeEmptyColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 2);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (count, 3);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 0x000000000);
    assertEqualInt (colors[1], 0x000000000);
    assertEqualInt (colors[2], 0x000000000);
    assertEqualInt (colors[3], 0x000000000);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // Two same colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, twoSameColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 2);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (count, 3);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 0x00000001);
    assertEqualInt (colors[1], 0x00000001);
    assertEqualInt (colors[2], 0x00000001);
    assertEqualInt (colors[3], 0x00000000);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    // One surround color.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    count = 1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    assertEqualInt (status, Ok);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (count, 3);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 0x00000001);
    assertEqualInt (colors[1], 0x00000001);
    assertEqualInt (colors[2], 0x00000001);
    assertEqualInt (colors[3], 0x00000000);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);

    count = 0xFF;
    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (count, 4);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 0x00000001);
    assertEqualInt (colors[1], 0x00000001);
    assertEqualInt (colors[2], 0x00000001);
    assertEqualInt (colors[3], 0x00000001);

    colors[0] = 0x00000000;
    colors[1] = 0x00000000;
    colors[2] = 0x00000000;
    colors[3] = 0x00000000;

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientSurroundColorsWithCount (NULL, colors, &count);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, NULL, &count);
    assertEqualInt (status, InvalidParameter);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, NULL);
    assertEqualInt (status, InvalidParameter);
#endif

    count = 4;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    count = 0;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    count = -1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);
}

static void test_getPathGradientPath ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPath *path;

    GdipCreatePath (FillModeAlternate, &path);
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPath (brush, path);
    assertEqualInt (status, NotImplemented);

    status = GdipGetPathGradientPath (NULL, path);
    assertEqualInt (status, NotImplemented);

    status = GdipGetPathGradientPath (brush, NULL);
    assertEqualInt (status, NotImplemented);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);
}

static void test_setPathGradientPath ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPath *path;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreatePath (FillModeWinding, &path);

    status = GdipSetPathGradientPath (brush, path);
    assertEqualInt (status, NotImplemented);

    status = GdipSetPathGradientPath (NULL, path);
    assertEqualInt (status, NotImplemented);

    status = GdipSetPathGradientPath (brush, NULL);
    assertEqualInt (status, NotImplemented);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);
}

static void test_getPathGradientCenterPoint ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPointF centerPoint;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualInt (status, Ok);
    assertEqualFloat (centerPoint.X, 3);
    assertEqualFloat (centerPoint.Y, 7);

    status = GdipGetPathGradientCenterPoint (NULL, &centerPoint);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientCenterPoint (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientCenterPointI ()
{
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
    assertEqualInt (status, Ok);
    assertEqualInt (centerPoint.X, 3);
    assertEqualInt (centerPoint.Y, 7);
    GdipDeleteBrush ((GpBrush *) brush);

    // > 0.5 is rounded up.
    GdipCreatePathGradient (roundUpPoints, 3, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientCenterPointI (brush, &centerPoint);
    assertEqualInt (status, Ok);
    assertEqualInt (centerPoint.X, 9);
    assertEqualInt (centerPoint.Y, 7);

    // Negative tests.
    status = GdipGetPathGradientCenterPointI (NULL, &centerPoint);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientCenterPointI (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientCenterPoint ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPointF point = {1, 2};
    GpPointF outOfBoundsPoint = {100, 200};
    GpPointF centerPoint;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Set within the bounds of the brush.
    status = GdipSetPathGradientCenterPoint (brush, &point);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualFloat (centerPoint.X, 1);
    assertEqualFloat (centerPoint.Y, 2);

    // Set same.
    status = GdipSetPathGradientCenterPoint (brush, &point);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualFloat (centerPoint.X, 1);
    assertEqualFloat (centerPoint.Y, 2);

    // Set outside the bounds of the brush.
    status = GdipSetPathGradientCenterPoint (brush, &outOfBoundsPoint);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualFloat (centerPoint.X, 100);
    assertEqualFloat (centerPoint.Y, 200);

    // Negative tests.
    status = GdipSetPathGradientCenterPoint (NULL, &point);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientCenterPoint (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientCenterPointI ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPoint point = {1, 2};
    GpPoint outOfBoundsPoint = {100, 200};
    GpPointF centerPoint;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Set within the bounds of the brush.
    status = GdipSetPathGradientCenterPointI (brush, &point);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualFloat (centerPoint.X, 1);
    assertEqualFloat (centerPoint.Y, 2);

    // Set same.
    status = GdipSetPathGradientCenterPointI (brush, &point);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualFloat (centerPoint.X, 1);
    assertEqualFloat (centerPoint.Y, 2);

    // Set outside the bounds of the brush.
    status = GdipSetPathGradientCenterPointI (brush, &outOfBoundsPoint);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assertEqualFloat (centerPoint.X, 100);
    assertEqualFloat (centerPoint.Y, 200);

    // Negative tests.
    status = GdipSetPathGradientCenterPointI (NULL, &point);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientCenterPointI (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientRect ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpRectF rect;
    GpPointF points[2] = {
        {1.5, 2.5},
        {5, 7}
    };

    GdipCreatePathGradient (points, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRect (brush, &rect);
    assertEqualInt (status, Ok);
    assertEqualFloat (rect.X, 1.5);
    assertEqualFloat (rect.Y, 2.5);
    assertEqualFloat (rect.Width, 3.5);
    assertEqualFloat (rect.Height, 4.5);

    // Negative tests.
    status = GdipGetPathGradientRect (NULL, &rect);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientRect (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientRectI ()
{
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
    assertEqualInt (status, Ok);
    assertEqualInt (rect.X, 2);
    assertEqualInt (rect.Y, 3);
    assertEqualInt (rect.Width, 4);
    assertEqualInt (rect.Height, 5);

    GdipDeleteBrush ((GpBrush *) brush);

    // 0.5 is rounded up.
    GdipCreatePathGradient (points2, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    assertEqualInt (status, Ok);
    assertEqualInt (rect.X, 2);
    assertEqualInt (rect.Y, 3);
    assertEqualInt (rect.Width, 4);
    assertEqualInt (rect.Height, 5);

    GdipDeleteBrush ((GpBrush *) brush);

    // < 0.5 is rounded down.
    GdipCreatePathGradient (points3, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    assertEqualInt (status, Ok);
    assertEqualInt (rect.X, 1);
    assertEqualInt (rect.Y, 2);
    assertEqualInt (rect.Width, 3);
    assertEqualInt (rect.Height, 4);

    // Negative tests.
    status = GdipGetPathGradientRectI (NULL, &rect);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientRectI (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientPointCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT pointCount;
    GpPath *path;

    GdipCreatePath (FillModeWinding, &path);
    GdipAddPathRectangle (path, 1, 2, 3, 4);

    // From points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPointCount (brush, &pointCount);
    assertEqualInt (status, Ok);
    assertEqualInt (pointCount, 3);

    GdipDeleteBrush ((GpBrush *) brush);

    // From path.
    GdipCreatePathGradientFromPath (path, &brush);

    status = GdipGetPathGradientPointCount (brush, &pointCount);
    assertEqualInt (status, Ok);
    assertEqualInt (pointCount, 4);

    // Negative tests.
    status = GdipGetPathGradientPointCount (NULL, &pointCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPointCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeletePath (path);
}

static void test_setPathGradientGammaCorrection ()
{
    GpStatus status;
    GpPathGradient *brush;
    BOOL useGammaCorrection;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Set true.
    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    // Set same.
    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    // Set false.
    status = GdipSetPathGradientGammaCorrection (brush, FALSE);
    assertEqualInt (status, Ok);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == FALSE);

    // Negative tests.
    status = GdipSetPathGradientGammaCorrection (NULL, TRUE);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientGammaCorrection ()
{
    GpStatus status;
    GpPathGradient *brush;
    BOOL useGammaCorrection;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == FALSE);

    // Negative tests.
    status = GdipGetPathGradientGammaCorrection (NULL, &useGammaCorrection);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientGammaCorrection (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientBlendCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientBlendCount (brush, &blendCount);
    assertEqualInt (blendCount, 1);

    // Negative tests.
    status = GdipGetPathGradientBlendCount (NULL, &blendCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientBlendCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void fill_array (REAL *array, INT count, REAL value)
{
    for (int i = 0; i < count; i++)
        array[i] = value;
}

static void test_getPathGradientBlend ()
{
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
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 123);
    assertEqualFloat (blend[2], 123);
    // Positions are meaningless for blends with a count of 1 so this parameter is ignored.
    assertEqualFloat (positions[0], 123);
    assertEqualFloat (positions[1], 123);
    assertEqualFloat (positions[2], 123);

    // Default blend - larger count.
    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 123);
    assertEqualFloat (blend[2], 123);
    // Positions are meaningless for blends with a count of 1 so this parameter is ignored.
    assertEqualFloat (positions[0], 123);
    assertEqualFloat (positions[1], 123);
    assertEqualFloat (positions[2], 123);

    // Negative tests.
    GdipSetPathGradientBlend (brush, threeBlends, threePositions, 3);
    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    assertEqualInt (status, InsufficientBuffer);

    status = GdipGetPathGradientBlend (NULL, blend, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientBlend (brush, NULL, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientBlend (brush, blend, NULL, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientBlend (brush, blend, positions, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientBlend (brush, blend, positions, -1);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientBlend ()
{
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
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 2);
    assertEqualFloat (blend[2], 123);
    assertEqualFloat (positions[0], 0);
    assertEqualFloat (positions[1], 1);
    assertEqualFloat (positions[2], 123);

    // Set same.
    status = GdipSetPathGradientBlend (brush, twoBlends, twoPositions, 2);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 2);
    assertEqualFloat (blend[2], 123);
    assertEqualFloat (positions[0], 0);
    assertEqualFloat (positions[1], 1);
    assertEqualFloat (positions[2], 123);

    // Three blends - equal count.
    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, 3);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 3);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 2);
    assertEqualFloat (blend[2], 3);
    assertEqualFloat (positions[0], 0);
    assertEqualFloat (positions[1], 0.5);
    assertEqualFloat (positions[2], 1);

    // One blend - equal count.
    status = GdipSetPathGradientBlend (brush, oneBlend, onePosition, 1);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 10);
    assertEqualFloat (blend[1], 123);
    assertEqualFloat (blend[2], 123);
    assertEqualFloat (positions[0], 123);
    assertEqualFloat (positions[1], 123);
    assertEqualFloat (positions[2], 123);

    // Should clear the existing blend.
    status = GdipSetPathGradientPresetBlend (brush, pathPresetBlend, pathPresetPositions, 3);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, 3);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientPresetBlendCount (brush, &presetBlendCount);
    assertEqualInt (status, Ok);
    assertEqualInt (presetBlendCount, 0);

    // Negative tests.
    status = GdipSetPathGradientBlend (NULL, threeBlends, threePositions, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, NULL, threePositions, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, threeBlends, NULL, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, twoBlends, invalidPositions1, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, twoBlends, invalidPositions2, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, threeBlends, threePositions, -1);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientPresetBlendCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPresetBlendCount (brush, &blendCount);
    assertEqualInt (status, Ok);
    assertEqualInt (blendCount, 0);

    // Negative tests.
    status = GdipGetPathGradientPresetBlendCount (NULL, &blendCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlendCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientPresetBlend ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB blend[2];
    REAL positions[2];

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Negative tests.
    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 2);
    assertEqualInt (status, GenericError);

    status = GdipGetPathGradientPresetBlend (NULL, blend, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, NULL, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, NULL, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, -1);
    assertEqualInt (status, OutOfMemory);

    status = GdipGetPathGradientPresetBlend (NULL, blend, positions, -1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, NULL, positions, -1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, NULL, -1);
    assertEqualInt (status, OutOfMemory);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientPresetBlend ()
{
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
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);
    assertEqualInt (destBlend2[0], 1);
    assertEqualInt (destBlend2[1], 0);
    assertEqualFloat (destPositions2[0], 0);
    assertEqualFloat (destPositions2[1], 1);

    // Same.
    status = GdipSetPathGradientPresetBlend (brush, blend2, positions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);
    assertEqualInt (destBlend2[0], 1);
    assertEqualInt (destBlend2[1], 0);
    assertEqualFloat (destPositions2[0], 0);
    assertEqualFloat (destPositions2[1], 1);

    // Count of 3.
    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 3);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientPresetBlend (brush, destBlend3, destPositions3, 3);
    assertEqualInt (status, Ok);
    assertEqualInt (destBlend3[0], 1);
    assertEqualInt (destBlend3[1], 2);
    assertEqualInt (destBlend3[2], 3);
    assertEqualFloat (destPositions3[0], 0);
    assertEqualFloat (destPositions3[1], 0.5);
    assertEqualFloat (destPositions3[2], 1);

    // Should clear the existing blend.
    status = GdipSetPathGradientBlend (brush, pathBlend, pathPositions, 3);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientBlendCount (brush, &blendCount);
    assertEqualInt (status, Ok);
    // GDI+ has a bug where it sets the blend and positions to NULL but incorrectly
    // decrements blendCount, instead of setting it to zero.
    // This causes GDI+ to crash. This is a bug we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    assertEqualInt (blendCount, 0);

    status = GdipGetPathGradientBlend(brush, destBlendReal, destPositionsReal, blendCount);
    assertEqualInt(status, InvalidParameter);
#endif

    // Negative tests.
    status = GdipSetPathGradientPresetBlend (NULL, blend3, positions3, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, NULL, positions3, 3);
    assertEqualInt (status, InvalidParameter);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetPathGradientPresetBlend (brush, blend3, NULL, 3);
    assertEqualInt (status, InvalidParameter);
#endif

    status = GdipSetPathGradientPresetBlend (brush, blend2, invalidPositions1, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend2, invalidPositions2, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, -1);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientSigmaBlend ()
{
    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientSigmaBlend (brush, 0, 0.5f);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, 1);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientSigmaBlend (brush, 1, 0);
    assertEqualInt (status, Ok);

    // Negative tests.
    status = GdipSetPathGradientSigmaBlend (NULL, 0.5f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, -0.01f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, 1.01f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, -0.01f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, 1.01f);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientLinearBlend ()
{
    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientLinearBlend (brush, 0, 0.5f);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, 1);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientLinearBlend (brush, 1, 0);
    assertEqualInt (status, Ok);

    // Negative tests.
    status = GdipSetPathGradientLinearBlend (NULL, 0.5f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, -0.01f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, 1.01f, 0.5);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, -0.01f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, 1.01f);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientWrapMode ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpWrapMode wrapMode;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Negative tests.
    status = GdipGetPathGradientWrapMode (NULL, &wrapMode);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientWrapMode (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientWrapMode ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpWrapMode wrapMode;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // WrapModeTile.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTile);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTile);

    // Same.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTile);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTile);

    // WrapModeTileFlipX.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipX);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipX);

    // WrapModeTileFlipX.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipY);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    // WrapModeTileFlipXY.
    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipXY);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipXY);

    // WrapModeClamp.
    status = GdipSetPathGradientWrapMode (brush, WrapModeClamp);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeClamp);

    // Invalid WrapMode - nop.
    status = GdipSetPathGradientWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeClamp);

    // Negative tests.
    status = GdipSetPathGradientWrapMode (NULL, WrapModeTile);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientTransform ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *transform;
    ARGB blend[2] = {1, 0};
    REAL positions[2] = {0, 1.0f};

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix (&transform);

    // With no preset blend.
    status = GdipGetPathGradientTransform (brush, transform);
    assertEqualInt (status, Ok);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // With preset blend.
    GdipSetPathGradientPresetBlend (brush, blend, positions, 2);
    status = GdipGetPathGradientTransform (brush, transform);
    assertEqualInt (status, Ok);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipGetPathGradientTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientTransform (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (transform);
}

static void test_setPathGradientTransform ()
{
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
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Same.
    status = GdipSetPathGradientTransform (brush, matrix);
    assertEqualInt (status, Ok);

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
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 2, 3, 4, 5, 6, 7);

    // Negative tests.
    status = GdipSetPathGradientTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientTransform (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientTransform (brush, nonInvertibleMatrix);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (transform);
}

static void test_resetPathGradientTransform ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *matrix;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    GdipSetPathGradientTransform (brush, matrix);

    status = GdipResetPathGradientTransform (brush);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipResetPathGradientTransform (NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
}

static void test_multiplyPathGradientTransform ()
{
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
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix - nop.
    status = GdipMultiplyPathGradientTransform (brush, NULL, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - negative.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Invalid MatrixOrder - positive.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Negative tests.
    status = GdipMultiplyPathGradientTransform (NULL, matrix, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipMultiplyPathGradientTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);
}

static void test_translatePathGradientTransform ()
{
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
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipTranslatePathGradientTransform (brush, 5, 6, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslatePathGradientTransform (NULL, 1, 1, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipTranslatePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipTranslatePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_scalePathGradientTransform ()
{
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
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipScalePathGradientTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScalePathGradientTransform (NULL, 1, 1, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipScalePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipScalePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_rotatePathGradientTransform ()
{
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
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipRotatePathGradientTransform (brush, 90, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotatePathGradientTransform (NULL, 90, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipRotatePathGradientTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipRotatePathGradientTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_getPathGradientFocusScales ()
{
    GpStatus status;
    GpPathGradient *brush;
    REAL xScale;
    REAL yScale;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualInt (status, Ok);
    assertEqualFloat (xScale, 0);
    assertEqualFloat (yScale, 0);

    // Negative tests.
    status = GdipGetPathGradientFocusScales (NULL, &xScale, &yScale);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientFocusScales (NULL, &xScale, &yScale);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientFocusScales (brush, NULL, &yScale);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientFocusScales (brush, &xScale, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientFocusScales ()
{
    GpStatus status;
    GpPathGradient *brush;
    REAL xScale;
    REAL yScale;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Positive.
    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, 1);
    assertEqualFloat (yScale, 2);

    // Same.
    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, 1);
    assertEqualFloat (yScale, 2);

    // Zero.
    status = GdipSetPathGradientFocusScales (brush, 0, 0);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, 0);
    assertEqualFloat (yScale, 0);

    // Negative,
    status = GdipSetPathGradientFocusScales (brush, -1, -2);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, -1);
    assertEqualFloat (yScale, -2);

    // Negative tests.
    status = GdipSetPathGradientFocusScales (NULL, 0, 0);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_cloneWithPoints ()
{
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
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientCenterColor (brush, 3);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientTransform (brush, transform);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, surroundColors, &surroundColorsCount);
    assertEqualInt (status, Ok);

    status = GdipCloneBrush ((GpBrush *) brush, &clonedBrush);
    assertEqualInt (status, Ok);
    assert (clonedBrush && clonedBrush != brush);

    status = GdipGetBrushType (clonedBrush, &brushType);
    assertEqualInt (status, Ok);
    assertEqualInt (brushType, BrushTypePathGradient);

    status = GdipGetPathGradientWrapMode ((GpPathGradient *) clonedBrush, &wrapMode);
    assertEqualInt (status, Ok);
    assertEqualInt (wrapMode, WrapModeTileFlipX);

    status = GdipGetPathGradientFocusScales ((GpPathGradient *) clonedBrush, &xScale, &yScale);
    assertEqualInt (status, Ok);
    assertEqualFloat (xScale, 1);
    assertEqualFloat (yScale, 2);

    status = GdipGetPathGradientCenterColor ((GpPathGradient *) clonedBrush, &centerColor);
    assertEqualInt (status, Ok);
    assertEqualInt (centerColor, 3);

    status = GdipGetPathGradientCenterPoint ((GpPathGradient *) clonedBrush, &centerPoint);
    assertEqualInt (status, Ok);
    assertEqualFloat (centerPoint.X, 3);
    assertEqualFloat (centerPoint.Y, 7);

    status = GdipGetPathGradientRect ((GpPathGradient *) clonedBrush, &rect);
    assertEqualInt (status, Ok);
    assertEqualFloat (rect.X, 1);
    assertEqualFloat (rect.Y, 2);
    assertEqualFloat (rect.Width, 4);
    assertEqualFloat (rect.Height, 11);

    status = GdipGetPathGradientPointCount ((GpPathGradient *) clonedBrush, &pointCount);
    assertEqualInt (pointCount, 3);

    status = GdipGetPathGradientBlendCount ((GpPathGradient *) clonedBrush, &blendCount);
    assertEqualInt (status, Ok);
    assertEqualInt (blendCount, 2);

    status = GdipGetPathGradientBlend ((GpPathGradient *) clonedBrush, blendResult, positionsResult, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blendResult[0], 1);
    assertEqualFloat (blendResult[1], 2);
    assertEqualFloat (positionsResult[0], 0);
    assertEqualFloat (positionsResult[1], 1);

    status = GdipGetPathGradientGammaCorrection ((GpPathGradient *) clonedBrush, &useGammaCorrection);
    assertEqualInt (status, Ok);
    assert (useGammaCorrection == TRUE);

    status = GdipGetPathGradientTransform ((GpPathGradient *) clonedBrush, matrix);
    assertEqualInt (status, Ok);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    status = GdipGetPathGradientSurroundColorCount (brush, &surroundColorsCount);
    assertEqualInt (status, Ok);
    assertEqualInt (surroundColorsCount, 3);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, surroundColorsResult, &surroundColorsCount);
    assertEqualInt (status, Ok);
    assertEqualInt (surroundColorsResult[0], 1);
    assertEqualInt (surroundColorsResult[1], 2);
    assertEqualInt (surroundColorsResult[2], 3);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);
}

static void test_cloneWithPath ()
{
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
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientCenterColor (brush, 3);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientTransform (brush, transform);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, surroundColors, &surroundColorsCount);
    assertEqualInt (status, Ok);

    status = GdipCloneBrush ((GpBrush *) brush, &clonedBrush);
    assertEqualInt (status, Ok);
    assert (clonedBrush && clonedBrush != brush);

    status = GdipGetBrushType (clonedBrush, &brushType);
    assertEqualInt (status, Ok);
    assertEqualInt (brushType, BrushTypePathGradient);

    status = GdipGetPathGradientWrapMode ((GpPathGradient *) clonedBrush, &wrapMode);
    assertEqualInt (status, Ok);
    assertEqualInt (wrapMode, WrapModeClamp);

    status = GdipGetPathGradientFocusScales ((GpPathGradient *) clonedBrush, &xScale, &yScale);
    assertEqualInt (status, Ok);
    assertEqualFloat (xScale, 1);
    assertEqualFloat (yScale, 2);

    status = GdipGetPathGradientCenterColor ((GpPathGradient *) clonedBrush, &centerColor);
    assertEqualInt (status, Ok);
    assertEqualInt (centerColor, 3);

    status = GdipGetPathGradientCenterPoint ((GpPathGradient *) clonedBrush, &centerPoint);
    assertEqualInt (status, Ok);
    assertEqualFloat (centerPoint.X, 3);
    assertEqualFloat (centerPoint.Y, 7.5);

    status = GdipGetPathGradientRect ((GpPathGradient *) clonedBrush, &rect);
    assertEqualInt (status, Ok);
    assertEqualFloat (rect.X, 1);
    assertEqualFloat (rect.Y, 2);
    assertEqualFloat (rect.Width, 4);
    assertEqualFloat (rect.Height, 11);

    status = GdipGetPathGradientPointCount ((GpPathGradient *) clonedBrush, &pointCount);
    assertEqualInt (status, Ok);
    assertEqualInt (pointCount, 4);

    status = GdipGetPathGradientBlendCount ((GpPathGradient *) clonedBrush, &blendCount);
    assertEqualInt (status, Ok);
    assertEqualInt (blendCount, 2);

    status = GdipGetPathGradientBlend ((GpPathGradient *) clonedBrush, blendResult, positionsResult, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blendResult[0], 1);
    assertEqualFloat (blendResult[1], 2);
    assertEqualFloat (positionsResult[0], 0);
    assertEqualFloat (positionsResult[1], 1);

    status = GdipGetPathGradientGammaCorrection ((GpPathGradient *) clonedBrush, &useGammaCorrection);
    assertEqualInt (status, Ok);
    assert (useGammaCorrection == TRUE);

    status = GdipGetPathGradientTransform ((GpPathGradient *) clonedBrush, matrix);
    assertEqualInt (status, Ok);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    status = GdipGetPathGradientSurroundColorCount (brush, &surroundColorsCount);
    assertEqualInt (status, Ok);
    assertEqualInt (surroundColorsCount, 4);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, surroundColorsResult, &surroundColorsCount);
    assertEqualInt (status, Ok);
    assertEqualInt (surroundColorsResult[0], 1);
    assertEqualInt (surroundColorsResult[1], 2);
    assertEqualInt (surroundColorsResult[2], 3);
    assertEqualInt (surroundColorsResult[3], 4);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);
}

static void test_delete ()
{
    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    assertEqualInt (status, Ok);
}

int
main (int argc, char**argv)
{
    STARTUP;

    test_createPathGradient ();
    test_createPathGradientI ();
    test_createPathGradientFromPath ();
    test_getPathGradientCenterColor ();
    test_setPathGradientCenterColor ();
    test_getPathGradientSurroundColorCount ();
    test_getPathGradientSurroundColorsWithCount ();
    test_setPathGradientSurroundColorsWithCount ();
    test_getPathGradientTransform ();
    test_getPathGradientPath ();
    test_setPathGradientPath ();
    test_getPathGradientCenterPoint ();
    test_getPathGradientCenterPointI ();
    test_setPathGradientCenterPoint ();
    test_setPathGradientCenterPointI ();
    test_getPathGradientRect ();
    test_getPathGradientRectI ();
    test_getPathGradientPointCount ();
    test_setPathGradientGammaCorrection ();
    test_getPathGradientGammaCorrection ();
    test_getPathGradientBlendCount ();
    test_getPathGradientBlend ();
    test_setPathGradientBlend ();
    test_getPathGradientPresetBlendCount ();
    test_getPathGradientPresetBlend ();
    test_setPathGradientPresetBlend ();
    test_setPathGradientSigmaBlend ();
    test_setPathGradientLinearBlend ();
    test_getPathGradientWrapMode ();
    test_setPathGradientWrapMode ();
    test_setPathGradientTransform ();
    test_resetPathGradientTransform ();
    test_multiplyPathGradientTransform ();
    test_translatePathGradientTransform ();
    test_scalePathGradientTransform ();
    test_rotatePathGradientTransform ();
    test_getPathGradientFocusScales ();
    test_setPathGradientFocusScales ();
    test_cloneWithPoints ();
    test_cloneWithPath ();
    test_delete ();

    SHUTDOWN;
    return 0;
}
