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

static void verifyLineGradientBrush (GpLineGradient *brush, REAL x, REAL y, REAL width, REAL height, ARGB startColor, ARGB endColor, GpWrapMode expectedWrapMode, REAL e1, REAL e2, REAL e3, REAL e4, REAL e5, REAL e6)
{
    assert (brush && "Expected the brush to be initialized.");

    GpStatus status;
    GpBrushType brushType;
    GpRectF rect;
    GpWrapMode wrapMode;
    GpMatrix *brushTransform;
    ARGB colors[2];

    GdipCreateMatrix (&brushTransform);

    status = GdipGetBrushType (brush, &brushType);
    assert (brushType == BrushTypeLinearGradient);

    status = GdipGetLineRect (brush, &rect);
    assert (status == Ok);
    assert (rect.X == x);
    assert (rect.Y == y);
    assert (rect.Width == width);
    assert (rect.Height == height);

    status = GdipGetLineColors (brush, colors);
    assert (status == Ok);
    assert (colors[0] == startColor);
    assert (colors[1] == endColor);

    status = GdipGetLineWrapMode (brush, &wrapMode);
    assert (status == Ok);
    assert (wrapMode == expectedWrapMode);

    status = GdipGetLineTransform (brush, brushTransform);
    assert (status == Ok);
    verifyMatrix (brushTransform, e1, e2, e3, e4, e5, e6);

    GdipDeleteMatrix (brushTransform);
}

static void test_createLineBrush ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpPointF point1 = { 1, 5 };
    GpPointF point2 = { 2, 3 };

    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTile, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrush (&point2, &point1, 10, 11, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.8f, -0.4f, 6.2f, 2.6f);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrush (&point1, &point2, 10, 11, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);

    status = GdipCreateLineBrush (NULL, &point2, 10, 11, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrush (&point1, NULL, 10, 11, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTile, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createLineBrushI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpPoint point1 = { 1, 5 };
    GpPoint point2 = { 2, 3 };

    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTile, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushI (&point2, &point1, 10, 11, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.8f, -0.4f, 6.2f, 2.6f);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);

    status = GdipCreateLineBrushI (NULL, &point2, 10, 11, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushI (&point1, NULL, 10, 11, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTile, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createLineBrushFromRect ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRectF rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRect (&rect2, 10, 11, LinearGradientModeBackwardDiagonal, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeForwardDiagonal, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeVertical, WrapModeTileFlipX, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);

    status = GdipCreateLineBrushFromRect (NULL, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    GpRectF rect3 = { 1, 3, 0, 1 };
    status = GdipCreateLineBrushFromRect (&rect3, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    GpRectF rect4 = { 1, 3, 1, 0 };
    status = GdipCreateLineBrushFromRect (&rect4, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeHorizontal - 1), WrapModeTile, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateLineBrushFromRect (NULL, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, NULL);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createLineBrushFromRectI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRect rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRect rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectI (&rect2, 10, 11, LinearGradientModeBackwardDiagonal, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, LinearGradientModeForwardDiagonal, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, LinearGradientModeVertical, WrapModeTileFlipX, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);

    status = GdipCreateLineBrushFromRectI (NULL, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    GpRect rect3 = { 1, 3, 0, 1 };
    status = GdipCreateLineBrushFromRectI (&rect3, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    GpRect rect4 = { 1, 3, 1, 0 };
    status = GdipCreateLineBrushFromRectI (&rect4, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeHorizontal - 1), WrapModeTile, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateLineBrushFromRectI (NULL, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, NULL);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectI (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createLineBrushFromRectWithAngle ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 0, TRUE, WrapModeTile, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRectF rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectWithAngle (&rect2, 10, 11, 135, TRUE, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect2, 10, 11, -225, FALSE, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1.500000, 1.500000, -0.750000, -0.750000, 2.750000, 2.750000);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 405, TRUE, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 45, FALSE, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1.5, 1.5, -0.75, 0.75, 2.25, -1.25);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeTileFlipX, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);

    status = GdipCreateLineBrushFromRectWithAngle (NULL, 10, 11, 90, TRUE, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    GpRectF rect3 = { 1, 3, 0, 1 };
    status = GdipCreateLineBrushFromRectWithAngle (&rect3, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    GpRectF rect4 = { 1, 3, 1, 0 };
    status = GdipCreateLineBrushFromRectWithAngle (&rect4, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeTile, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_createLineBrushFromRectWithAngleI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRect rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 0, TRUE, WrapModeTile, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRect rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectWithAngleI (&rect2, 10, 11, 135, TRUE, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect2, 10, 11, -225, FALSE, (WrapMode)(WrapModeTile - 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1.500000, 1.500000, -0.750000, -0.750000, 2.750000, 2.750000);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 405, TRUE, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 45, FALSE, (WrapMode)(WrapModeClamp + 1), &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1.5, 1.5, -0.75, 0.75, 2.25, -1.25);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeTileFlipX, &brush);
    assert (status == Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);

    status = GdipCreateLineBrushFromRectWithAngleI (NULL, 10, 11, 90, TRUE, WrapModeTile, &brush);
    assert (status == InvalidParameter);

    GpRect rect3 = { 1, 3, 0, 1 };
    status = GdipCreateLineBrushFromRectWithAngleI (&rect3, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    GpRect rect4 = { 1, 3, 1, 0 };
    status = GdipCreateLineBrushFromRectWithAngleI (&rect4, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    // FIXME: should be OutOfMemory to match GDI+ but needs updates in Mono System.Drawing
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeTile, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineColors ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB colors[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineColors (brush, 1, 2);
    assert (status == Ok);

    GdipGetLineColors (brush, colors);
    assert (colors[0] == 1);
    assert (colors[1] == 2);

    status = GdipSetLineColors (NULL, 1, 2);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineColors ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB colors[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineColors (NULL, colors);
    assert (status == InvalidParameter);

    status = GdipGetLineColors (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineRect ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpRectF lineRect;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRect (NULL, &lineRect);
    assert (status == InvalidParameter);

    status = GdipGetLineRect (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineRectI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpRect lineRect;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    assert (rect.X == 1);
    assert (rect.Y == 3);
    assert (rect.Width == 1);
    assert (rect.Height == 2);

    status = GdipGetLineRectI (NULL, &lineRect);
    assert (status == InvalidParameter);

    status = GdipGetLineRectI (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineGammaCorrection ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    BOOL useGammaCorrection;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineGammaCorrection (brush, TRUE);
    assert (status == Ok);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    status = GdipSetLineGammaCorrection (NULL, TRUE);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineGammaCorrection ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    BOOL useGammaCorrection;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    assert (status == Ok);
    assert (useGammaCorrection == FALSE);

    status = GdipGetLineGammaCorrection (NULL, &useGammaCorrection);
    assert (status == InvalidParameter);

    status = GdipGetLineGammaCorrection (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineBlendCount ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    INT blendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineBlendCount (brush, &blendCount);
    assert (status == Ok);
    assert (blendCount == 1);

    status = GdipGetLineBlendCount (NULL, &blendCount);
    assert (status == InvalidParameter);

    status = GdipGetLineBlendCount (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    REAL blend[2];
    REAL positions[2];
    REAL largeBlend[3] = { 1, 2, 3 };
    REAL largePositions[3] = { 0, 0.5, 1 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineBlend (brush, blend, positions, 1);
    assert (status == Ok);
    assert (floatsEqual (blend[0], 1));
    // FIXME: it appears that Windows 10+ versions of GDI+ don't copy anything to positions.
    // assert (floatsEqual (positions[0], 0));

    status = GdipGetLineBlend (brush, blend, positions, 2);
    assert (status == Ok);
    assert (floatsEqual (blend[0], 1.0));
    // FIXME: it appears that Windows 10+ versions of GDI+ don't copy anything to positions.
    // assert (floatsEqual (positions[0], 0));

    status = GdipSetLineBlend (brush, largeBlend, largePositions, 3);
    assert (status == Ok);

    status = GdipGetLineBlend (brush, blend, positions, 1);
    assert (status == InsufficientBuffer);

    status = GdipGetLineBlend (NULL, blend, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetLineBlend (brush, NULL, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetLineBlend (brush, blend, NULL, 1);
    assert (status == InvalidParameter);

    status = GdipGetLineBlend (brush, blend, positions, 0);
    assert (status == InvalidParameter);

    status = GdipGetLineBlend (brush, blend, positions, -1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    REAL blend1[1] = { 3 };
    REAL positions1[1] = { -12 };
    REAL destBlend1[1];
    REAL destPositions1[1];

    REAL blend2[2] = { -1, 0 };
    REAL positions2[2] = { 0, 1.0f };
    REAL destBlend2[2];
    REAL destPositions2[2];

    REAL blend3[3] = { 1, 2, 3 };
    REAL positions3[3] = { 0, 0.5f, 1.0f };
    REAL destBlend3[3];
    REAL destPositions3[3];

    REAL invalidPositions1[2] = { 0.5, 1 };
    REAL invalidPositions2[2] = { 0, 0.5 };

    ARGB linePresetBlend[3] = { 1, 2, 3 };
    REAL linePresetPositions[3] = { 0, 0.5f, 1.0f };
    INT presetBlendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Count of 1.
    status = GdipSetLineBlend (brush, blend1, positions1, 1);
    assert (status == Ok);

    status = GdipGetLineBlend (brush, destBlend1, destPositions1, 1);
    assert (status == Ok);
    assert (destBlend1[0] == 3);
    // FIXME: it appears GDI+ ignores the position value if there is a single element.
    // assert (floatsEqual (destPositions1[0], 0));

    // Count of 2.
    status = GdipSetLineBlend (brush, blend2, positions2, 2);
    assert (status == Ok);

    status = GdipGetLineBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);
    assert (destBlend2[0] == -1);
    assert (destBlend2[1] == 0);
    assert (destPositions2[0] == 0);
    assert (destPositions2[1] == 1);

    // Count of 3.
    status = GdipSetLineBlend (brush, blend3, positions3, 3);
    assert (status == Ok);

    status = GdipGetLineBlend (brush, destBlend3, destPositions3, 3);
    assert (status == Ok);
    assert (destBlend3[0] == 1);
    assert (destBlend3[1] == 2);
    assert (destBlend3[2] == 3);
    assert (destPositions3[0] == 0);
    assert (destPositions3[1] == 0.5);
    assert (destPositions3[2] == 1);

    // Should clear the existing blend.
    status = GdipSetLinePresetBlend (brush, linePresetBlend, linePresetPositions, 3);
    assert (status == Ok);

    status = GdipSetLineBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);

    status = GdipGetLinePresetBlendCount (brush, &presetBlendCount);
    assert (status == Ok);
    assert (presetBlendCount == 0);

    // Negative tests.
    status = GdipSetLineBlend (NULL, blend3, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetLineBlend (brush, NULL, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetLineBlend (brush, blend3, NULL, 3);
    assert (status == InvalidParameter);

    status = GdipSetLineBlend (brush, blend2, invalidPositions1, 2);
    assert (status == InvalidParameter);

    status = GdipSetLineBlend (brush, blend3, positions3, 0);
    assert (status == InvalidParameter);

    status = GdipSetLineBlend (brush, blend3, positions3, -1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLinePresetBlendCount ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    INT blendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLinePresetBlendCount (brush, &blendCount);
    assert (status == Ok);
    assert (blendCount == 0);

    status = GdipGetLinePresetBlendCount (NULL, &blendCount);
    assert (status == InvalidParameter);

    status = GdipGetLinePresetBlendCount (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLinePresetBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB blend[2];
    REAL positions[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLinePresetBlend (brush, blend, positions, 2);
    assert (status == GenericError);

    status = GdipGetLinePresetBlend (NULL, blend, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetLinePresetBlend (brush, NULL, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, NULL, 1);
    assert (status == InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, positions, 1);
    assert (status == InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, positions, 0);
    assert (status == InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, positions, -1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLinePresetBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    ARGB blend2[2] = { 1, 0 };
    REAL positions2[2] = { 0, 1.0f };
    ARGB destBlend2[2];
    REAL destPositions2[2];

    ARGB blend3[3] = { 1, 2, 3 };
    REAL positions3[3] = { 0, 0.5f, 1.0f };
    ARGB destBlend3[3];
    REAL destPositions3[3];

    REAL invalidPositions1[2] = { 0.5, 1 };
    REAL invalidPositions2[2] = { 0, 0.5 };

    REAL lineBlend[3] = { 1, 2, 3 };
    REAL linePositions[3] = { 0, 2, 1 };
    INT blendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Count of 2.
    status = GdipSetLinePresetBlend (brush, blend2, positions2, 2);
    assert (status == Ok);

    status = GdipGetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);
    assert (destBlend2[0] == 1);
    assert (destBlend2[1] == 0);
    assert (destPositions2[0] == 0);
    assert (destPositions2[1] == 1);

    // Count of 3.
    status = GdipSetLinePresetBlend (brush, blend3, positions3, 3);
    assert (status == Ok);

    status = GdipGetLinePresetBlend (brush, destBlend3, destPositions3, 3);
    assert (status == Ok);
    assert (destBlend3[0] == 1);
    assert (destBlend3[1] == 2);
    assert (destBlend3[2] == 3);
    assert (destPositions3[0] == 0);
    assert (destPositions3[1] == 0.5);
    assert (destPositions3[2] == 1);

    // Should clear the existing blend.
    status = GdipSetLineBlend (brush, lineBlend, linePositions, 3);
    assert (status == Ok);

    status = GdipSetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    assert (status == Ok);

    status = GdipGetLineBlendCount (brush, &blendCount);
    assert (status == Ok);
    // FIXME: this returns 2 with GDI+.
    // assert (blendCount == 2);

    // Negative tests.
    status = GdipSetLinePresetBlend (NULL, blend3, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetLinePresetBlend (brush, NULL, positions3, 3);
    assert (status == InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, NULL, 3);
    assert (status == InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend2, invalidPositions1, 2);
    assert (status == InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, 1);
    assert (status == InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, 0);
    assert (status == InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, -1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineSigmaBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineSigmaBlend (brush, 0, 0.5f);
    assert (status == Ok);

    status = GdipSetLineSigmaBlend (brush, 0.5f, 1);
    assert (status == Ok);

    status = GdipSetLineSigmaBlend (brush, 1, 0);
    assert (status == Ok);

    status = GdipSetLineSigmaBlend (NULL, 0.5f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, -0.01f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, 1.01f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, 0.5f, -0.01f);
    assert (status == InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, 0.5f, 1.01f);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineLinearBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineLinearBlend (brush, 0, 0.5f);
    assert (status == Ok);

    status = GdipSetLineLinearBlend (brush, 0.5f, 1);
    assert (status == Ok);

    status = GdipSetLineLinearBlend (brush, 1, 0);
    assert (status == Ok);

    status = GdipSetLineLinearBlend (NULL, 0.5f, 0.5f);
    assert (status == InvalidParameter);

    status = GdipSetLineLinearBlend (brush, -0.01f, 0.5);
    assert (status == InvalidParameter);

    status = GdipSetLineLinearBlend (brush, 1.01f, 0.5);
    assert (status == InvalidParameter);

    status = GdipSetLineLinearBlend (brush, 0.5f, -0.01f);
    assert (status == InvalidParameter);

    status = GdipSetLineLinearBlend (brush, 0.5f, 1.01f);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineWrapMode ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpWrapMode wrapMode;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    status = GdipSetLineWrapMode (brush, WrapModeTileFlipY);
    assert (status == Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    status = GdipSetLineWrapMode (brush, (WrapMode)(WrapModeTile - 1));
    assert (status == Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    status = GdipSetLineWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assert (status == Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    status = GdipSetLineWrapMode (NULL, WrapModeTile);
    assert (status == InvalidParameter);

    status = GdipSetLineWrapMode (brush, WrapModeClamp);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineWrapMode ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpWrapMode wrapMode;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    status = GdipGetLineWrapMode (NULL, &wrapMode);
    assert (status == InvalidParameter);

    status = GdipGetLineWrapMode (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix (&transform);

    status = GdipGetLineTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipGetLineTransform (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (transform);
}

static void test_setLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *matrix;
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    status = GdipSetLineTransform (brush, matrix);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetLineTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipSetLineTransform (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (transform);
}

static void test_resetLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *matrix;
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    GdipSetLineTransform (brush, matrix);

    status = GdipResetLineTransform (brush);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    status = GdipResetLineTransform (NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
}

static void test_multiplyLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *originalTransform;
    GpMatrix *matrix;
    GpMatrix *nonInvertibleMatrix;
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix - nop.
    status = GdipMultiplyLineTransform (brush, NULL, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - this produces garbage data.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == Ok);

    status = GdipMultiplyLineTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == Ok);

    // Negative tests.
    status = GdipMultiplyLineTransform (NULL, matrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipMultiplyLineTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);
}

static void test_translateLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *originalTransform;
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipTranslateLineTransform (brush, 5, 6, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipTranslateLineTransform (brush, 5, 6, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslateLineTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipTranslateLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipTranslateLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_scaleLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *originalTransform;
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipScaleLineTransform (brush, 0.5, 0.75, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipScaleLineTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScaleLineTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipScaleLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipScaleLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_rotateLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *originalTransform;
    GpMatrix *transform;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipRotateLineTransform (brush, 90, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipRotateLineTransform (brush, 90, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotateLineTransform (NULL, 90, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipRotateLineTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipRotateLineTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_clone ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpBrush *clonedBrush;

    GpBrushType brushType;
    GpWrapMode wrapMode;
    ARGB colors[2];
    REAL blend[2] = { 1, 2 };
    REAL positions[2] = { 0, 1 };
    INT blendCount;
    REAL blendResult[2];
    REAL positionsResult[2];
    GpMatrix *transform;
    GpMatrix *matrix;
    BOOL useGammaCorrection;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    GdipSetLineBlend (brush, blend, positions, 2);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &transform);
    GdipCreateMatrix (&matrix);
    GdipSetLineTransform (brush, transform);
    GdipSetLineGammaCorrection (brush, TRUE);

    status = GdipCloneBrush ((GpBrush *) brush, &clonedBrush);
    assert (status == Ok);
    assert (clonedBrush && clonedBrush != brush);

    GdipGetBrushType (clonedBrush, &brushType);
    assert (brushType == BrushTypeLinearGradient);

    GdipGetLineWrapMode ((GpLineGradient *) clonedBrush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipY);

    GdipGetLineColors (brush, colors);
    assert (colors[0] == 10);
    assert (colors[1] == 11);

    GdipGetLineRect ((GpLineGradient *) clonedBrush, &rect);
    assert (rect.X == 1);
    assert (rect.Y == 3);
    assert (rect.Width == 1);
    assert (rect.Height == 2);

    GdipGetLineBlendCount ((GpLineGradient *) clonedBrush, &blendCount);
    assert (blendCount == 2);

    GdipGetLineBlend ((GpLineGradient *) clonedBrush, blendResult, positionsResult, 2);
    assert (blendResult[0] == 1);
    assert (blendResult[1] == 2);
    assert (positionsResult[0] == 0);
    assert (positionsResult[1] == 1);

    GdipGetLineTransform ((GpLineGradient *) clonedBrush, matrix);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    GdipGetLineGammaCorrection ((GpLineGradient *) clonedBrush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

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

    test_createLineBrush ();
    test_createLineBrushI ();
    test_createLineBrushFromRect ();
    test_createLineBrushFromRectI ();
    test_createLineBrushFromRectWithAngle ();
    test_createLineBrushFromRectWithAngleI ();
    test_setLineColors ();
    test_getLineColors ();
    test_getLineRect ();
    test_getLineRectI ();
    test_setLineGammaCorrection ();
    test_getLineGammaCorrection ();
    test_getLineBlendCount ();
    test_getLineBlend ();
    test_setLineBlend ();
    test_getLinePresetBlendCount ();
    test_getLinePresetBlend ();
    test_setLinePresetBlend ();
    test_setLineSigmaBlend ();
    test_setLineLinearBlend ();
    test_setLineWrapMode ();
    test_getLineWrapMode ();
    test_getLineTransform ();
    test_setLineTransform ();
    test_resetLineTransform ();
    test_multiplyLineTransform ();
    test_translateLineTransform ();
    test_scaleLineTransform ();
    test_rotateLineTransform ();
    test_clone ();

    GdiplusShutdown (gdiplusToken);
    return 0;
}
