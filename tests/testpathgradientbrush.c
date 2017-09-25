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
#include <math.h>
#include <float.h>

static int floatsEqual(float v1, float v2)
{
    return fabs (v1 - v2) < 0.0001;
}

static void verifyMatrix (GpMatrix *matrix, REAL e1, REAL e2, REAL e3, REAL e4, REAL e5, REAL e6)
{
    float elements[6];
    GdipGetMatrixElements (matrix, elements);

    printf ("Expected: %f, %f, %f, %f, %f, %f\n", e1, e2, e3, e4, e5, e6);
    printf ("Actual:   %f, %f, %f, %f, %f, %f\n\n", elements[0], elements[1], elements[2], elements[3], elements[4], elements[5]);

    assert (floatsEqual(elements[0], e1));
    assert (floatsEqual(elements[1], e2));
    assert (floatsEqual(elements[2], e3));
    assert (floatsEqual(elements[3], e4));
    assert (floatsEqual(elements[4], e5));
    assert (floatsEqual(elements[5], e6));
}

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
    assert (brushType == BrushTypePathGradient);

    status = GdipGetPathGradientRect (brush, &rect);
    printf ("%f\n", rect.X);
    printf ("%f\n", rect.Y);
    printf ("%f\n", rect.Width);
    printf ("%f\n\n", rect.Height);
    assert (status == Ok);
    assert (rect.X == x);
    assert (rect.Y == y);
    assert (rect.Width == width);
    assert (rect.Height == height);

    status = GdipGetPathGradientCenterColor (brush, &color);
    assert (status == Ok);
    assert (color == centerColor);

    status = GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assert (status == Ok);
    printf ("%f\n", centerPoint.X);
    printf ("%f\n\n", centerPoint.Y);
    assert (centerPoint.X == centerX);
    assert (centerPoint.Y == centerY);

    status = GdipGetPathGradientWrapMode (brush, &wrapMode);
    assert (status == Ok);
    printf ("%d\n\n\n\n", wrapMode);
    assert (wrapMode == expectedWrapMode);

    status = GdipGetPathGradientTransform (brush, brushTransform);
    assert (status == Ok);
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
    assert (status == Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeClamp);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreatePathGradient (threePoints, 3, WrapModeTile, &brush);
    assert (status == Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xff000000, 3, 7, WrapModeTile);

    status = GdipCreatePathGradient (NULL, 2, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradient (points2, 1, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradient (points2, 0, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradient (points2, -1, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradient (points2, 2, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradient (points2, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradient (points2, 2, WrapModeClamp, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    verifyPathGradientBrush (brush, -10, 2, 11, 9, 0xff000000, -4.5, 6.5, WrapModeClamp);

    status = GdipCreatePathGradientI (points3, 3, WrapModeTile, &brush);
    assert (status == Ok);
    verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xff000000, 3, 7, WrapModeTile);

    status = GdipCreatePathGradientI (NULL, 2, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreatePathGradientI (points, 1, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientI (points, 0, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientI (points, -1, WrapModeClamp, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientI (points, 2, WrapModeClamp, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    status = GdipCreatePath (FillModeWinding, &emptyPath);
    assert (status == Ok);

    status = GdipCreatePathGradientFromPath (path, &brush);
    // FIXME: this fails in GDI+.
    // assert (status == Ok);
    // verifyPathGradientBrush (brush, 1, 2, 4, 11, 0xffffffff, 3, 7, WrapModeClamp);

    status = GdipCreatePathGradientFromPath (NULL, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientFromPath (emptyPath, &brush);
    assert (status == OutOfMemory);

    status = GdipCreatePathGradientFromPath (NULL, NULL);
    assert (status == InvalidParameter);

    status = GdipCreatePathGradientFromPath (path, NULL);
    assert (status == InvalidParameter);

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
    assert (status == InvalidParameter);

    status = GdipGetPathGradientCenterColor (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientCenterColor ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB centerColor;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientCenterColor (brush, 2);
    assert (status == Ok);

    GdipGetPathGradientCenterColor (brush, &centerColor);
    assert (centerColor == 2);

    status = GdipSetPathGradientCenterColor (NULL, 1);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (count == 3);
    GdipDeleteBrush ((GpBrush *) brush);

    // Two points.
    GdipCreatePathGradient (threePoints, 2, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientSurroundColorCount (brush, &count);
    assert (status == Ok);
    assert (count == 2);

    // Negative tests
    status = GdipGetPathGradientSurroundColorCount (NULL, &count);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientSurroundColorCount (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (count == 1);
    assert (colors[0] == 0xffffffff);
    assert (colors[1] == 0xffffffff);
    assert (colors[2] == 0xffffffff);

    // Negative tests
    status = GdipGetPathGradientSurroundColorsWithCount (NULL, colors, &count);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, NULL, &count);
    assert (status == InvalidParameter);

    // FIXME: this causes GDI+ to crash.
    // status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, NULL);
    // assert (status == InvalidParameter);

    count = 2;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

    count = 1;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

    count = 0;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

    count = -1;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (count == 3);

    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == Ok);
    assert (count == 3);
    assert (colors[0] == 1);
    assert (colors[1] == 2);
    assert (colors[2] == 3);

    // Two empty colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeEmptyColors, &count);
    assert (status == Ok);
    assert (count == 2);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == Ok);
    assert (count == 1);
    assert (colors[0] == 0);
    assert (colors[1] == 0);
    assert (colors[2] == 0);

    // Two same colors.
    count = 2;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, twoSameColors, &count);
    assert (status == Ok);
    assert (count == 2);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == Ok);
    assert (count == 1);
    assert (colors[0] == 1);
    assert (colors[1] == 1);
    assert (colors[2] == 1);

    // One surround color.
    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    count = 1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, threeSurroundColors, &count);
    assert (status == Ok);

    count = 3;
    status = GdipGetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == Ok);
    assert (count == 1);
    assert (colors[0] == 1);
    assert (colors[1] == 1);
    assert (colors[2] == 1);

    // Negative tests
    status = GdipSetPathGradientSurroundColorsWithCount (NULL, colors, &count);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientSurroundColorsWithCount (brush, NULL, &count);
    assert (status == InvalidParameter);

    // FIXME: this causes GDI+ to crash.
    // status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, NULL);
    // assert (status == InvalidParameter);

    count = 4;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

    count = 0;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

    count = -1;
    status = GdipSetPathGradientSurroundColorsWithCount (brush, colors, &count);
    assert (status == InvalidParameter);

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
    assert (status == NotImplemented);

    status = GdipGetPathGradientPath (NULL, path);
    assert (status == NotImplemented);

    status = GdipGetPathGradientPath (brush, NULL);
    assert (status == NotImplemented);

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
    assert (status == NotImplemented);

    status = GdipSetPathGradientPath (NULL, path);
    assert (status == NotImplemented);

    status = GdipSetPathGradientPath (brush, NULL);
    assert (status == NotImplemented);

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
    assert (status == Ok);
    assert (centerPoint.X == 3);
    assert (centerPoint.Y == 7);

    status = GdipGetPathGradientCenterPoint (NULL, &centerPoint);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientCenterPoint (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (centerPoint.X == 3);
    assert (centerPoint.Y == 7);
    GdipDeleteBrush ((GpBrush *) brush);

    // > 0.5 is rounded up.
    GdipCreatePathGradient (roundUpPoints, 3, WrapModeTileFlipXY, &brush);

    status = GdipGetPathGradientCenterPointI (brush, &centerPoint);
    assert (status == Ok);
    assert (centerPoint.X == 9);
    assert (centerPoint.Y == 7);

    // Negative tests.
    status = GdipGetPathGradientCenterPointI (NULL, &centerPoint);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientCenterPointI (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assert (centerPoint.X == 1);
    assert (centerPoint.Y == 2);

    // Set outside the bounds of the brush.
    status = GdipSetPathGradientCenterPoint (brush, &outOfBoundsPoint);
    assert (status == Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assert (centerPoint.X == 100);
    assert (centerPoint.Y == 200);

    // Negative tests.
    status = GdipSetPathGradientCenterPoint (NULL, &point);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientCenterPoint (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assert (centerPoint.X == 1);
    assert (centerPoint.Y == 2);

    // Set outside the bounds of the brush.
    status = GdipSetPathGradientCenterPointI (brush, &outOfBoundsPoint);
    assert (status == Ok);

    GdipGetPathGradientCenterPoint (brush, &centerPoint);
    assert (centerPoint.X == 100);
    assert (centerPoint.Y == 200);

    // Negative tests.
    status = GdipSetPathGradientCenterPointI (NULL, &point);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientCenterPointI (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (rect.X == 1.5);
    assert (rect.Y == 2.5);
    assert (rect.Width == 3.5);
    assert (rect.Height == 4.5);

    status = GdipGetPathGradientRect (NULL, &rect);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientRect (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (rect.X == 2);
    assert (rect.Y == 3);
    assert (rect.Width == 4);
    assert (rect.Height == 5);

    GdipDeleteBrush ((GpBrush *) brush);

    // 0.5 is rounded up.
    GdipCreatePathGradient (points2, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    assert (status == Ok);
    assert (rect.X == 2);
    assert (rect.Y == 3);
    assert (rect.Width == 4);
    assert (rect.Height == 5);

    GdipDeleteBrush ((GpBrush *) brush);

    // < 0.5 is rounded down.
    GdipCreatePathGradient (points3, 2, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientRectI (brush, &rect);
    assert (status == Ok);
    assert (rect.X == 1);
    assert (rect.Y == 2);
    assert (rect.Width == 3);
    assert (rect.Height == 4);

    // Negative tests.
    status = GdipGetPathGradientRectI (NULL, &rect);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientRectI (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientPointCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT pointCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPointCount (brush, &pointCount);
    assert (status == Ok);
    assert (pointCount == 3);

    status = GdipGetPathGradientPointCount (NULL, &pointCount);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPointCount (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientGammaCorrection ()
{
    GpStatus status;
    GpPathGradient *brush;
    BOOL useGammaCorrection;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientGammaCorrection (brush, TRUE);
    assert (status == Ok);

    GdipGetPathGradientGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    status = GdipSetPathGradientGammaCorrection (NULL, TRUE);
    assert (status == InvalidParameter);

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
    assert (status == InvalidParameter);

    status = GdipGetPathGradientGammaCorrection (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientBlendCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientBlendCount (brush, &blendCount);
    assert (blendCount == 1);

    status = GdipGetPathGradientBlendCount (NULL, &blendCount);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientBlendCount (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (floatsEqual(blend[0], 1));
    // FIXME: it appears that Windows 10+ versions of GDI+ don't copy anything to positions.
    // assert (floatsEqual(positions[0], 0));

    status = GdipGetPathGradientBlend (brush, blend, positions, 2);
    assert (status == Ok);
    assert (floatsEqual(blend[0], 1));
    // FIXME: it appears that Windows 10+ versions of GDI+ don't copy anything to positions.
    // assert (floatsEqual(positions[0], 0));

    status = GdipSetPathGradientBlend (brush, largeBlend, largePositions, 3);
    assert (status == Ok);

    status = GdipGetPathGradientBlend (brush, blend, positions, 1);
    assert (status == InsufficientBuffer);

    status = GdipGetPathGradientBlend (NULL, blend, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientBlend (brush, NULL, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientBlend (brush, blend, NULL, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientBlend (brush, blend, positions, 0);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientBlend (brush, blend, positions, -1);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    status = GdipGetPathGradientBlend (brush, destBlend1, destPositions1, 1);
    assert (status == Ok);
    assert (destBlend1[0] == 3);
    // FIXME: it appears GDI+ ignores the position value if there is a single element.
    // assert (floatsEqual(destPositions1[0], 0));

    // Count of 2.
    status = GdipSetPathGradientBlend (brush, blend2, positions2, 2);
    assert (status == Ok);

    status = GdipGetPathGradientBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);
    assert (destBlend2[0] == -1);
    assert (destBlend2[1] == 0);
    assert (destPositions2[0] == 0);
    assert (destPositions2[1] == 1);

    // Count of 3.
    status = GdipSetPathGradientBlend (brush, blend3, positions3, 3);
    assert (status == Ok);

    status = GdipGetPathGradientBlend (brush, destBlend3, destPositions3, 3);
    assert (status == Ok);
    assert (destBlend3[0] == 1);
    assert (destBlend3[1] == 2);
    assert (destBlend3[2] == 3);
    assert (destPositions3[0] == 0);
    assert (destPositions3[1] == 0.5);
    assert (destPositions3[2] == 1);

    // Should clear the existing blend.
    status = GdipSetPathGradientPresetBlend (brush, pathPresetBlend, pathPresetPositions, 3);
    assert (status == Ok);

    status = GdipSetPathGradientBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);

    status = GdipGetPathGradientPresetBlendCount (brush, &presetBlendCount);
    assert (status == Ok);
    // FIXME: this returns 0 with GDI+.
    // assert (presetBlendCount == 0);

    // Negative tests.
    status = GdipSetPathGradientBlend (NULL, blend3, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientBlend (brush, NULL, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend3, NULL, 3);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend2, invalidPositions1, 2);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend2, invalidPositions2, 2);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend3, positions3, 0);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientBlend (brush, blend3, positions3, -1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientPresetBlendCount ()
{
    GpStatus status;
    GpPathGradient *brush;
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientPresetBlendCount (brush, &blendCount);
    assert (status == Ok);
    // FIXME: this returns 0 with GDI+.
    // assert (blendCount == 0);

    status = GdipGetPathGradientPresetBlendCount (NULL, &blendCount);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlendCount (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientPresetBlend ()
{
    GpStatus status;
    GpPathGradient *brush;
    ARGB blend[1];
    REAL positions[1];

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // FIXME: GDI+ has no preset colors by default.
    // status = GdipGetPathGradientPresetBlend (brush, blend, positions, 2);
    // assert (status == GenericError);

    status = GdipGetPathGradientPresetBlend (NULL, blend, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, NULL, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, NULL, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, 0);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, positions, -1);
    assert (status == OutOfMemory);

    status = GdipGetPathGradientPresetBlend (NULL, blend, positions, -1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, NULL, positions, -1);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientPresetBlend (brush, blend, NULL, -1);
    assert (status == OutOfMemory);

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

    REAL invalidPositions1[2] = {0.5, 1};
    REAL invalidPositions2[2] = {0, 0.5};

    REAL pathBlend[3] = {1, 2, 3};
    REAL pathPositions[3] = {0, 2, 1};
    INT blendCount;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    // Count of 2.
    status = GdipSetPathGradientPresetBlend (brush, blend2, positions2, 2);
    assert (status == Ok);

    status = GdipGetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);
    assert (destBlend2[0] == 1);
    assert (destBlend2[1] == 0);
    assert (destPositions2[0] == 0);
    assert (destPositions2[1] == 1);

    // Count of 3.
    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 3);
    assert (status == Ok);

    status = GdipGetPathGradientPresetBlend (brush, destBlend3, destPositions3, 3);
    assert (status == Ok);
    assert (destBlend3[0] == 1);
    assert (destBlend3[1] == 2);
    assert (destBlend3[2] == 3);
    assert (destPositions3[0] == 0);
    assert (destPositions3[1] == 0.5);
    assert (destPositions3[2] == 1);

    // Should clear the existing blend.
    status = GdipSetPathGradientBlend (brush, pathBlend, pathPositions, 3);
    assert (status == Ok);

    status = GdipSetPathGradientPresetBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);

    status = GdipGetPathGradientBlendCount (brush, &blendCount);
    assert (status == Ok);
    // FIXME: this returns 2 with GDI+.
    // assert (blendCount == 2);

    // Negative tests.
    status = GdipSetPathGradientPresetBlend (NULL, blend3, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, NULL, positions3, 3);
    assert (status == InvalidParameter);

    // FIXME: this caused GDI+ to crash.
    // status = GdipSetPathGradientPresetBlend (brush, blend3, NULL, 3);
    // assert (status == InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend2, invalidPositions1, 2);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend2, invalidPositions2, 2);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 1);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, 0);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientPresetBlend (brush, blend3, positions3, -1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientSigmaBlend ()
{
    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientSigmaBlend (brush, 0, 0.5f);
    assert (status == Ok);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, 1);
    assert (status == Ok);

    status = GdipSetPathGradientSigmaBlend (brush, 1, 0);
    assert (status == Ok);

    status = GdipSetPathGradientSigmaBlend (NULL, 0.5f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, -0.01f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, 1.01f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, -0.01f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientSigmaBlend (brush, 0.5f, 1.01f);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientLinearBlend ()
{
    GpStatus status;
    GpPathGradient *brush;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientLinearBlend (brush, 0, 0.5f);
    assert (status == Ok);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, 1);
    assert (status == Ok);

    status = GdipSetPathGradientLinearBlend (brush, 1, 0);
    assert (status == Ok);

    status = GdipSetPathGradientLinearBlend (NULL, 0.5f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, -0.01f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, 1.01f, 0.5);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, -0.01f);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientLinearBlend (brush, 0.5f, 1.01f);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getPathGradientWrapMode ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpWrapMode wrapMode;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipGetPathGradientWrapMode (NULL, &wrapMode);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientWrapMode (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setPathGradientWrapMode ()
{
    GpStatus status;
    GpPathGradient *brush;
    GpWrapMode wrapMode;

    GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &brush);

    status = GdipSetPathGradientWrapMode (brush, WrapModeTileFlipY);
    assert (status == Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    status = GdipSetPathGradientWrapMode (brush, (WrapMode)(WrapModeTile - 1));
    assert (status == Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    status = GdipSetPathGradientWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assert (status == Ok);

    GdipGetPathGradientWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    status = GdipSetPathGradientWrapMode (NULL, WrapModeTile);
    assert (status == InvalidParameter);

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
    assert (status == InvalidParameter);

    status = GdipGetPathGradientTransform (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetPathGradientTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipSetPathGradientTransform (brush, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    status = GdipResetPathGradientTransform (NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix - nop.
    status = GdipMultiplyPathGradientTransform (brush, NULL, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - this produces garbage data.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipMultiplyPathGradientTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == Ok);

    status = GdipMultiplyPathGradientTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == Ok);

    // Negative tests.
    status = GdipMultiplyPathGradientTransform (NULL, matrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipMultiplyPathGradientTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipTranslatePathGradientTransform (brush, 5, 6, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslatePathGradientTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipTranslatePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipTranslatePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipScalePathGradientTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScalePathGradientTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipScalePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipScalePathGradientTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetPathGradientTransform (brush, originalTransform);

    status = GdipRotatePathGradientTransform (brush, 90, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPathGradientTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotatePathGradientTransform (NULL, 90, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipRotatePathGradientTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipRotatePathGradientTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (xScale == 0);
    assert (yScale == 0);

    status = GdipGetPathGradientFocusScales (NULL, &xScale, &yScale);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientFocusScales (NULL, &xScale, &yScale);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientFocusScales (brush, NULL, &yScale);
    assert (status == InvalidParameter);

    status = GdipGetPathGradientFocusScales (brush, &xScale, NULL);
    assert (status == InvalidParameter);

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
    assert (status == Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assert (xScale == 1);
    assert (yScale == 2);

    status = GdipSetPathGradientFocusScales (brush, -1, -2);
    assert (status == Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assert (xScale == -1);
    assert (yScale == -2);

    status = GdipSetPathGradientFocusScales (brush, 0, 0);
    assert (status == Ok);

    GdipGetPathGradientFocusScales (brush, &xScale, &yScale);
    assert (xScale == 0);
    assert (yScale == 0);

    status = GdipSetPathGradientFocusScales (NULL, 0, 0);
    assert (status == InvalidParameter);

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
    assert (status == Ok);
    assert (clonedBrush && clonedBrush != brush);

    GdipGetBrushType (clonedBrush, &brushType);
    assert (brushType == BrushTypePathGradient);

    GdipGetPathGradientWrapMode ((GpPathGradient *) clonedBrush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipX);

    GdipGetPathGradientFocusScales ((GpPathGradient *) clonedBrush, &xScale, &yScale);
    assert (xScale == 1);
    assert (yScale == 2);

    GdipGetPathGradientCenterColor ((GpPathGradient *) clonedBrush, &centerColor);
    assert (centerColor == 3);

    GdipGetPathGradientCenterPoint ((GpPathGradient *) clonedBrush, &centerPoint);
    assert (centerPoint.X == 3);
    assert (centerPoint.Y == 7);

    GdipGetPathGradientRect ((GpPathGradient *) clonedBrush, &rect);
    assert (rect.X == 1);
    assert (rect.Y == 2);
    assert (rect.Width == 4);
    assert (rect.Height == 11);

    GdipGetPathGradientPointCount ((GpPathGradient *) clonedBrush, &pointCount);
    assert (pointCount == 3);

    GdipGetPathGradientBlendCount ((GpPathGradient *) clonedBrush, &blendCount);
    assert (blendCount == 2);

    GdipGetPathGradientBlend ((GpPathGradient *) clonedBrush, blendResult, positionsResult, 2);
    assert (blendResult[0] == 1);
    assert (blendResult[1] == 2);
    assert (positionsResult[0] == 0);
    assert (positionsResult[1] == 1);

    GdipGetPathGradientGammaCorrection ((GpPathGradient *) clonedBrush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    GdipGetPathGradientTransform ((GpPathGradient *) clonedBrush, matrix);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    GdipGetPathGradientSurroundColorsWithCount (brush, surroundColorsResult, &surroundColorsCount);
    assert (surroundColorsResult[0] == 1);
    assert (surroundColorsResult[1] == 2);
    assert (surroundColorsResult[2] == 3);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);
}

int
main (int argc, char**argv)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

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

    GdiplusShutdown (gdiplusToken);
    return 0;
}
