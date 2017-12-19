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

#ifdef WIN32
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
    GpPointF points2[2] =
    {
		{1, 2},
		{-10, 11}
    };

    status = GdipCreatePathGradient (points2, 2, WrapModeClamp, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeClamp);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreatePathGradient (threePoints, 3, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xff000000, 3, 7, WrapModeTile);

    status = GdipCreatePathGradient (NULL, 2, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradient (points2, 1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradient (points2, 0, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradient (points2, -1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradient (points2, 2, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradient (points2, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradient (points2, 2, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createPathGradientI ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpPoint points[2] =
    {
		{1, 2},
		{-10, 11}
    };
    GpPoint points3[3] =
    {
		{3, 13},
		{1, 2},
		{5, 6}
    };

    status = GdipCreatePathGradientI (points, 2, WrapModeClamp, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeClamp);

    status = GdipCreatePathGradientI (points3, 3, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xff000000, 3, 7, WrapModeTile);

    status = GdipCreatePathGradientI (NULL, 2, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientI (points, 1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientI (points, 0, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientI (points, -1, WrapModeClamp, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createPathGradientFromPath ()
{
    GpStatus status;
    GpPathGradient *brush;
    BYTE types[3] = {1, 2, 3};
    GpPath *path;
    GpPath *emptyPath;

    status = GdipCreatePath2 (threePoints, types, 3, FillModeAlternate, &path);
    assertEqualInt (status, Ok);

    status = GdipCreatePath (FillModeWinding, &emptyPath);
    assertEqualInt (status, Ok);

    status = GdipCreatePathGradientFromPath (path, &brush);
    // FIXME: this fails in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
    assertEqualInt (status, Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xffffffff, 3, 7, WrapModeClamp);
#endif

    status = GdipCreatePathGradientFromPath (NULL, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientFromPath (emptyPath, &brush);
    assertEqualInt (status, OutOfMemory);

    status = GdipCreatePathGradientFromPath (NULL, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreatePathGradientFromPath (path, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *)brush);
    GdipDeletePath (path);
    GdipDeletePath (emptyPath);
}

static void test_getPathGradientCenterColor ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB centerColor;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

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

    status = GdipSetPathGradientCenterColor (brush, 2);
    assertEqualInt (status, Ok);

    GdipGetPathGradientCenterColor (brush, &centerColor);
    assertEqualInt (centerColor, 2);

    status = GdipSetPathGradientCenterColor (NULL, 1);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientSurroundColorCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT count;

    // Three points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  3);
    GdipDeleteBrush ((GpBrush *) brush);

    // Two points.
    GdipCreatePathGradient (threePoints, 2, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  2);

    // Negative tests
    status = GdipGetPathGradientSurroundColorCount (NULL, &count);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientSurroundColorCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientSurroundColorsWithCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB colors[3] = {0, 0, 0};
    INT count = 3;

    // Three points.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  1);
    assertEqualInt (colors[0], 0xffffffff);
    assertEqualInt (colors[1], 0xffffffff);
    assertEqualInt (colors[2], 0xffffffff);

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
}

static void test_setPathGradientSurroundColorsWithCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB threeSurroundColors[3] = {1, 2, 3};
    ARGB threeEmptyColors[3] = {0, 0, 0};
    ARGB twoSameColors[2] = {1, 1};
    ARGB colors[3];
    INT count = 3;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Three surround colors.
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  3);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  3);
    assertEqualInt (colors[0], 1);
    assertEqualInt (colors[1], 2);
    assertEqualInt (colors[2], 3);

    // Two empty colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeEmptyColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  2);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  1);
    assertEqualInt (colors[0], 0);
    assertEqualInt (colors[1], 0);
    assertEqualInt (colors[2], 0);

    // Two same colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, twoSameColors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  2);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count,  1);
    assertEqualInt (colors[0], 1);
    assertEqualInt (colors[1], 1);
    assertEqualInt (colors[2], 1);

    // One surround color.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    count = 1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    assertEqualInt (status, Ok);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assertEqualInt (status, Ok);
    assertEqualInt (count, 1);
    assertEqualInt (colors[0], 1);
    assertEqualInt (colors[1], 1);
    assertEqualInt (colors[2], 1);

    // Negative tests
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
    GpPointF roundUpPoints[3] =
    {
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
    GpPointF points[2] =
    {
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
    GpPointF points1[2] =
    {
		{1.6f, 2.6f},
		{5.2f, 7.2f}
    };
    GpPointF points2[2] =
    {
		{1.5f, 2.5f},
		{5, 7}
    };
    GpPointF points3[2] =
    {
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

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPointCount (brush, &pointCount);
    assertEqualInt (status, Ok);
    assertEqualInt (pointCount, 3);

    status = GdipGetPathGradientPointCount (NULL, &pointCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientPointCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientGammaCorrection ()
{
    GpStatus status;
    GpPathGradient *brush;
    BOOL useGammaCorrection;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

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

    status = GdipGetPathGradientBlendCount (NULL, &blendCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetPathGradientBlendCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientBlend ()
{
    GpStatus status;
    GpPathGradient *brush;
    REAL blend[1];
    REAL positions[1];
    REAL largeBlend[3] = {1, 2, 3};
    REAL largePositions[3] = {0, 0.5, 1};

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    // It appears that Windows 10+ versions of GDI+ don't copy anything to positions.
    // This is a GDI+ bug that we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    assertEqualFloat (positions[0], 0);
#endif

    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    // It appears that Windows 10+ versions of GDI+ don't copy anything to positions.
    // This is a GDI+ bug that we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    assertEqualFloat (positions[0], 0);
#endif

    status = GdipSetPathGradientBlend (brush, largeBlend, largePositions, 3);
    assertEqualInt (status, Ok);

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

    REAL blend1[1] = {3};
    REAL positions1[1] = {-12};
    REAL destBlend1[1];
    REAL destPositions1[1];

    REAL blend2[2] = {-1, 0};
    REAL positions2[2] = {0, 1.0f};
    REAL destBlend2[2];
    REAL destPositions2[2];

    REAL blend3[3] = {1, 2, 3};
    REAL positions3[3] = {0, 0.5f, 1.0f};
    REAL destBlend3[3];
    REAL destPositions3[3];

    REAL invalidPositions1[2] = {0.5, 1};
    REAL invalidPositions2[2] = {0, 0.5};

    ARGB pathPresetBlend[3] = {1, 2, 3};
    REAL pathPresetPositions[3] = {0, 0.5f, 1.0f};
    INT presetBlendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Count of 1.
    status = GdipSetPathGradientBlend (brush, blend1, positions1, 1);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientBlend (brush, destBlend1, destPositions1, 1);
    assertEqualInt (status, Ok);
    assertEqualFloat (destBlend1[0], 3);
    // It appears GDI+ ignores the position value if there is a single element.
    // This is a GDI+ bug we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    assertEqualFloat (destPositions1[0], -12);
#endif

    // Count of 2.
    status = GdipSetPathGradientBlend (brush, blend2, positions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (destBlend2[0], -1);
    assertEqualFloat (destBlend2[1], 0);
    assertEqualFloat (destPositions2[0], 0);
    assertEqualFloat (destPositions2[1], 1);

    // Count of 3.
    status = GdipSetPathGradientBlend (brush, blend3, positions3, 3);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientBlend (brush, destBlend3, destPositions3, 3);
    assertEqualInt (status, Ok);
    assertEqualFloat (destBlend3[0], 1);
    assertEqualFloat (destBlend3[1], 2);
    assertEqualFloat (destBlend3[2], 3);
    assertEqualFloat (destPositions3[0], 0);
    assertEqualFloat (destPositions3[1], 0.5);
    assertEqualFloat (destPositions3[2], 1);

    // Should clear the existing blend.
    status = GdipSetPathGradientPresetBlend (brush, pathPresetBlend, pathPresetPositions, 3);
    assertEqualInt (status, Ok);

    status = GdipSetPathGradientBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetPathGradientPresetBlendCount (brush, &presetBlendCount);
    assertEqualInt (status, Ok);
    assertEqualInt (presetBlendCount, 0);

    // Negative tests.
    status = GdipSetPathGradientBlend (NULL, blend3, positions3, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, NULL, positions3, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend3, NULL, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend2, invalidPositions1, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend2, invalidPositions2, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend3, positions3, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend3, positions3, -1);
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

    REAL destBlendReal[2];
    REAL destPositionsReal[2];

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

    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipY);
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    status = GdipSetPathGradientWrapMode (brush, (WrapMode)(WrapModeTile - 1));
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    status = GdipSetPathGradientWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assertEqualInt (status, Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    status = GdipSetPathGradientWrapMode (NULL, WrapModeTile);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientTransform ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix (&transform);

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
    GpMatrix *transform;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    status = GdipSetPathGradientTransform (brush, matrix);
    assertEqualInt (status, Ok);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetPathGradientTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetPathGradientTransform (brush, NULL);
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

    status = GdipSetPathGradientFocusScales (brush, 1, 2);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, 1);
    assertEqualFloat (yScale, 2);

    status = GdipSetPathGradientFocusScales (brush, -1, -2);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, -1);
    assertEqualFloat (yScale, -2);

    status = GdipSetPathGradientFocusScales (brush, 0, 0);
    assertEqualInt (status, Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assertEqualFloat (xScale, 0);
    assertEqualFloat (yScale, 0);

    status = GdipSetPathGradientFocusScales (NULL, 0, 0);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_clone ()
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

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);
    GdipSetPathGradientFocusScales (brush, 1, 2);
    GdipSetPathGradientCenterColor (brush, 3);
    GdipSetPathGradientBlend (brush, blend, positions, 2);
    GdipSetPathGradientGammaCorrection (brush, TRUE);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);
    GdipCreateMatrix (&matrix);
    GdipSetPathGradientTransform (brush, transform);
    GdipSetPathGradientSurroundColorsWithCount (brush, surroundColors, &surroundColorsCount);

    status = GdipCloneBrush ((GpBrush *) brush, &clonedBrush);
    assertEqualInt (status, Ok);
    assert (clonedBrush && clonedBrush != brush);

    GdipGetBrushType (clonedBrush, &brushType);
    assertEqualInt (brushType, BrushTypePathGradient);

    GdipGetPathGradientWrapMode ((GpPathGradient *) clonedBrush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipX);

    GdipGetPathGradientFocusScales ((GpPathGradient *) clonedBrush, &xScale, &yScale);
    assertEqualFloat (xScale, 1);
    assertEqualFloat (yScale, 2);

    GdipGetPathGradientCenterColor ((GpPathGradient *) clonedBrush, &centerColor);
    assertEqualInt (centerColor, 3);

    GdipGetPathGradientCenterPoint ((GpPathGradient *) clonedBrush, &centerPoint);
    assertEqualFloat (centerPoint.X, 3);
    assertEqualFloat (centerPoint.Y, 7);

    GdipGetPathGradientRect ((GpPathGradient *) clonedBrush, &rect);
    assertEqualFloat (rect.X, 1);
    assertEqualFloat (rect.Y, 2);
    assertEqualFloat (rect.Width, 4);
    assertEqualFloat (rect.Height, 11);

    GdipGetPathGradientPointCount ((GpPathGradient *) clonedBrush, &pointCount);
    assertEqualInt (pointCount, 3);

    GdipGetPathGradientBlendCount ((GpPathGradient *) clonedBrush, &blendCount);
    assertEqualInt (blendCount, 2);

    GdipGetPathGradientBlend ((GpPathGradient *) clonedBrush, blendResult, positionsResult, 2);
    assertEqualFloat (blendResult[0], 1);
    assertEqualFloat (blendResult[1], 2);
    assertEqualFloat (positionsResult[0], 0);
    assertEqualFloat (positionsResult[1], 1);

    GdipGetPathGradientGammaCorrection ((GpPathGradient *) clonedBrush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    GdipGetPathGradientTransform ((GpPathGradient *) clonedBrush, matrix);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    GdipGetPathGradientSurroundColorsWithCount (brush, surroundColorsResult, &surroundColorsCount);
    assertEqualInt (surroundColorsResult[0], 1);
    assertEqualInt (surroundColorsResult[1], 2);
    assertEqualInt (surroundColorsResult[2], 3);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);
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
    test_clone ();

    SHUTDOWN;
    return 0;
}
