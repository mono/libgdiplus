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
    assertEqualInt (brushType, BrushTypeLinearGradient);

    status = GdipGetLineRect (brush, &rect);
    assertEqualInt (status, Ok);
    assertEqualFloat (rect.X, x);
    assertEqualFloat (rect.Y, y);
    assertEqualFloat (rect.Width, width);
    assertEqualFloat (rect.Height, height);

    status = GdipGetLineColors (brush, colors);
    assertEqualInt (status, Ok);
    assertEqualInt (colors[0], startColor);
    assertEqualInt (colors[1], endColor);

    status = GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (status, Ok);
    assertEqualInt (wrapMode, expectedWrapMode);

    status = GdipGetLineTransform (brush, brushTransform);
    assertEqualInt (status, Ok);
    verifyMatrix (brushTransform, e1, e2, e3, e4, e5, e6);

    GdipDeleteMatrix (brushTransform);
}

static void test_createLineBrush ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpPointF point1 = {1, 5};
    GpPointF point2 = {2, 3};
    GpPointF topLeft = {1, 2};
    GpPointF topCenter = {3, 2};
    GpPointF topRight = {5, 2};
    GpPointF middleLeft = {1, 4};
    GpPointF middleCenter = {3, 4};
    GpPointF middleRight = {5, 4};
    GpPointF bottomLeft = {1, 6};
    GpPointF bottomCenter = {3, 6};
    GpPointF bottomRight = {5, 6};

    // WrapModeTile.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipX.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipY.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipXY
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipXY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // Invalid WrapMode.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrush (&middleCenter, &middleLeft, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 2, 2, 10, 11, WrapModeTile, -1, 0, 0, -1, 4, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrush (&middleCenter, &topLeft, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 2, 2, 2, 10, 11, WrapModeTile, -1, -1, 1, -1, 1, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrush (&middleCenter, &topCenter, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 2, 2, 2, 2, 10, 11, WrapModeTile, 0, -1, 1, 0, 0, 6);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrush (&middleCenter, &topRight, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 3, 2, 2, 2, 10, 11, WrapModeTile, 1, -1, 1, 1, -3, 4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrush (&middleCenter, &middleRight, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 3, 3, 2, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrush (&middleCenter, &bottomRight, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 3, 4, 2, 2, 10, 11, WrapModeTile, 1, 1, -1, 1, 5, -4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrush (&middleCenter, &bottomCenter, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 2, 4, 2, 2, 10, 11, WrapModeTile, -0, 1, -1, -0, 8, 2);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrush (&middleCenter, &bottomLeft, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 4, 2, 2, 10, 11, WrapModeTile, -1, 1, -1, -1, 9, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (NULL, &point2, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, NULL, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
}

static void test_createLineBrushI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpPoint point1 = {1, 5};
    GpPoint point2 = {2, 3};
    GpPoint topLeft = {1, 2};
    GpPoint topCenter = {3, 2};
    GpPoint topRight = {5, 2};
    GpPoint middleLeft = {1, 4};
    GpPoint middleCenter = {3, 4};
    GpPoint middleRight = {5, 4};
    GpPoint bottomLeft = {1, 6};
    GpPoint bottomCenter = {3, 6};
    GpPoint bottomRight = {5, 6};

    // WrapModeTile.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipX.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipY.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipXY
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipXY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // Invalid WrapMode.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &middleLeft, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 2, 2, 10, 11, WrapModeTile, -1, 0, 0, -1, 4, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &topLeft, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 2, 2, 2, 10, 11, WrapModeTile, -1, -1, 1, -1, 1, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &topCenter, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 2, 2, 2, 2, 10, 11, WrapModeTile, 0, -1, 1, 0, 0, 6);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &topRight, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 3, 2, 2, 2, 10, 11, WrapModeTile, 1, -1, 1, 1, -3, 4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &middleRight, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 3, 3, 2, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &bottomRight, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 3, 4, 2, 2, 10, 11, WrapModeTile, 1, 1, -1, 1, 5, -4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &bottomCenter, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 2, 4, 2, 2, 10, 11, WrapModeTile, -0, 1, -1, -0, 8, 2);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &bottomLeft, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 4, 2, 2, 10, 11, WrapModeTile, -1, 1, -1, -1, 9, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (NULL, &point2, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (&point1, NULL, 10, 11, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
}

static void test_createLineBrushFromRect ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRectF rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRect (&rect2, 10, 11, LinearGradientModeBackwardDiagonal, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeForwardDiagonal, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeVertical, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (NULL, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);

    GpRectF rect3 = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect3, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);

    GpRectF rect4 = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect4, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeHorizontal - 1), WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (NULL, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
}

static void test_createLineBrushFromRectI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRect normalRect = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRect negativeRect = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectI (&negativeRect, 10, 11, LinearGradientModeBackwardDiagonal, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeForwardDiagonal, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeVertical, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (NULL, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);

    GpRect zeroWidth = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&zeroWidth, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);

    GpRect zeroHeight = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&zeroHeight, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, (LinearGradientMode)(LinearGradientModeHorizontal - 1), WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (NULL, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeHorizontal, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
}

static void test_createLineBrushFromRectWithAngle ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 0, TRUE, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRectF rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectWithAngle (&rect2, 10, 11, 135, TRUE, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect2, 10, 11, -225, FALSE, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1.500000, 1.500000, -0.750000, -0.750000, 2.750000, 2.750000);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 405, TRUE, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 45, FALSE, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1.5, 1.5, -0.75, 0.75, 2.25, -1.25);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (NULL, 10, 11, 90, TRUE, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);

    GpRectF rect3 = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect3, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);

    GpRectF rect4 = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect4, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
}

static void test_createLineBrushFromRectWithAngleI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRect rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 0, TRUE, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRect rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectWithAngleI (&rect2, 10, 11, 135, TRUE, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect2, 10, 11, -225, FALSE, (WrapMode)(WrapModeTile - 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1.500000, 1.500000, -0.750000, -0.750000, 2.750000, 2.750000);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 405, TRUE, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 45, FALSE, (WrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1.5, 1.5, -0.75, 0.75, 2.25, -1.25);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (NULL, 10, 11, 90, TRUE, WrapModeTile, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);

    GpRect rect3 = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect3, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);

    GpRect rect4 = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect4, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, OutOfMemory);
	assert (brush == NULL);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
	
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeTile, NULL);
    assertEqualInt (status, InvalidParameter);
	assert (brush == (GpLineGradient *) 0xCC);
}

static void test_setLineColors ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB colors[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineColors (brush, 1, 2);
    assertEqualInt (status, Ok);

    GdipGetLineColors (brush, colors);
    assertEqualInt (colors[0], 1);
    assertEqualInt (colors[1], 2);

    // Negative tests.
    status = GdipSetLineColors (NULL, 1, 2);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineColors ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB colors[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetLineColors (NULL, colors);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineColors (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineRect ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpRectF lineRect;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetLineRect (NULL, &lineRect);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineRect (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineRectI ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRect lineRect;

    // Whole numbers.
    GpRectF wholeNumbers = { 1, 3, 1, 2 };
    GdipCreateLineBrushFromRect (&wholeNumbers, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    assertEqualInt (status, Ok);
    assertEqualInt (lineRect.X, 1);
    assertEqualInt (lineRect.Y, 3);
    assertEqualInt (lineRect.Width, 1);
    assertEqualInt (lineRect.Height, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // > 0.5 is rounded up.
    GpRectF greaterThanPointFive = { 1.6f, 3.6f, 1.6f, 2.6f };
    GdipCreateLineBrushFromRect (&greaterThanPointFive, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    assertEqualInt (status, Ok);
    assertEqualInt (lineRect.X, 2);
    assertEqualInt (lineRect.Y, 4);
    assertEqualInt (lineRect.Width, 2);
    assertEqualInt (lineRect.Height, 3);

    GdipDeleteBrush ((GpBrush *) brush);

    // 0.5 is rounded up.
    GpRectF pointFive = { 1.5f, 3.5f, 1.5f, 2.5f };
    GdipCreateLineBrushFromRect (&pointFive, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    assertEqualInt (status, Ok);
    assertEqualInt (lineRect.X, 2);
    assertEqualInt (lineRect.Y, 4);
    assertEqualInt (lineRect.Width, 2);
    assertEqualInt (lineRect.Height, 3);

    GdipDeleteBrush ((GpBrush *) brush);

    // < 0.5 is rounded down.
    GpRectF lessThanPointFive = { 1.1f, 3.1f, 1.1f, 2.1f };
    GdipCreateLineBrushFromRect (&lessThanPointFive, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    assertEqualInt (status, Ok);
    assertEqualInt (lineRect.X, 1);
    assertEqualInt (lineRect.Y, 3);
    assertEqualInt (lineRect.Width, 1);
    assertEqualInt (lineRect.Height, 2);

    // Negative tests.
    status = GdipGetLineRectI (NULL, &lineRect);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineRectI (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineGammaCorrection ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    BOOL useGammaCorrection;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

	// True.
    status = GdipSetLineGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);
	
	// Same.
    status = GdipSetLineGammaCorrection (brush, TRUE);
    assertEqualInt (status, Ok);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);
	
	// False.
    status = GdipSetLineGammaCorrection (brush, FALSE);
    assertEqualInt (status, Ok);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    assert (useGammaCorrection == FALSE);

    // Negative tests.
    status = GdipSetLineGammaCorrection (NULL, TRUE);
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);
    assert (useGammaCorrection == FALSE);

    // Negative tests.
    status = GdipGetLineGammaCorrection (NULL, &useGammaCorrection);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineGammaCorrection (brush, NULL);
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);
    assertEqualInt (blendCount, 1);

    // Negative tests.
    status = GdipGetLineBlendCount (NULL, &blendCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineBlendCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void fill_array (REAL *array, INT count, REAL value)
{
    for (int i = 0; i < count; i++)
        array[i] = value;
}

static void test_getLineBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    REAL blend[3];
    REAL positions[3];
    GpRectF rect = { 1, 3, 1, 2 };
    REAL threeBlends[3] = {1, 2, 3};
    REAL threePositions[3] = {0, 0.5, 1};

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Default blend - equal count.
    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 1);
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
    status = GdipGetLineBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 123);
    assertEqualFloat (blend[2], 123);
    // Positions are meaningless for blends with a count of 1 so this parameter is ignored.
    assertEqualFloat (positions[0], 123);
    assertEqualFloat (positions[1], 123);
    assertEqualFloat (positions[2], 123);

    // Negative tests.
    GdipSetLineBlend (brush, threeBlends, threePositions, 3);
    status = GdipGetLineBlend (brush, blend, positions, 1);
    assertEqualInt (status, InsufficientBuffer);

    status = GdipGetLineBlend (NULL, blend, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineBlend (brush, NULL, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineBlend (brush, blend, NULL, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineBlend (brush, blend, positions, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineBlend (brush, blend, positions, -1);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

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

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Two blends - equal count.
    status = GdipSetLineBlend (brush, twoBlends, twoPositions, 2);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 2);
    assertEqualFloat (blend[2], 123);
    assertEqualFloat (positions[0], 0);
    assertEqualFloat (positions[1], 1);
    assertEqualFloat (positions[2], 123);
	
    // Same.
    status = GdipSetLineBlend (brush, twoBlends, twoPositions, 2);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 2);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 2);
    assertEqualFloat (blend[2], 123);
    assertEqualFloat (positions[0], 0);
    assertEqualFloat (positions[1], 1);
    assertEqualFloat (positions[2], 123);

    // Three blends - equal count.
    status = GdipSetLineBlend (brush, threeBlends, threePositions, 3);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 3);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 1);
    assertEqualFloat (blend[1], 2);
    assertEqualFloat (blend[2], 3);
    assertEqualFloat (positions[0], 0);
    assertEqualFloat (positions[1], 0.5);
    assertEqualFloat (positions[2], 1);

    // One blend - equal count.
    status = GdipSetLineBlend (brush, oneBlend, onePosition, 1);
    assertEqualInt (status, Ok);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 1);
    assertEqualInt (status, Ok);
    assertEqualFloat (blend[0], 10);
    assertEqualFloat (blend[1], 123);
    assertEqualFloat (blend[2], 123);
    assertEqualFloat (positions[0], 123);
    assertEqualFloat (positions[1], 123);
    assertEqualFloat (positions[2], 123);

    // Should clear the existing blend.
    status = GdipSetLinePresetBlend (brush, pathPresetBlend, pathPresetPositions, 3);
    assertEqualInt (status, Ok);

    status = GdipSetLineBlend (brush, threeBlends, threePositions, 3);
    assertEqualInt (status, Ok);

    status = GdipGetLinePresetBlendCount (brush, &presetBlendCount);
    assertEqualInt (status, Ok);
    assertEqualInt (presetBlendCount, 0);

    // Negative tests.
    status = GdipSetLineBlend (NULL, threeBlends, threePositions, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineBlend (brush, NULL, threePositions, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineBlend (brush, threeBlends, NULL, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineBlend (brush, twoBlends, invalidPositions1, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineBlend (brush, twoBlends, invalidPositions2, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineBlend (brush, threeBlends, threePositions, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineBlend (brush, threeBlends, threePositions, -1);
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);
    assertEqualInt (blendCount, 0);

	// Negative tests.
    status = GdipGetLinePresetBlendCount (NULL, &blendCount);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlendCount (brush, NULL);
    assertEqualInt (status, InvalidParameter);

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

    // Negative tests.
    status = GdipGetLinePresetBlend (brush, blend, positions, 2);
    assertEqualInt (status, GenericError);

    status = GdipGetLinePresetBlend (NULL, blend, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, NULL, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, NULL, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, positions, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, positions, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, positions, -1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (NULL, blend, positions, -1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, NULL, positions, -1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLinePresetBlend (brush, blend, NULL, -1);
    assertEqualInt (status, InvalidParameter);

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

#if !defined(USE_WINDOWS_GDIPLUS)
    REAL destBlendReal[2];
    REAL destPositionsReal[2];
#endif

    REAL invalidPositions1[2] = { 0.5, 1 };
    REAL invalidPositions2[2] = { 0, 0.5 };

    REAL lineBlend[3] = { 1, 2, 3 };
    REAL linePositions[3] = { 0, 2, 1 };
    INT blendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Count of 2.
    status = GdipSetLinePresetBlend (brush, blend2, positions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);
    assertEqualInt (destBlend2[0], 1);
    assertEqualInt (destBlend2[1], 0);
    assertEqualFloat (destPositions2[0], 0);
    assertEqualFloat (destPositions2[1], 1);
	
    // Same.
    status = GdipSetLinePresetBlend (brush, blend2, positions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);
    assertEqualInt (destBlend2[0], 1);
    assertEqualInt (destBlend2[1], 0);
    assertEqualFloat (destPositions2[0], 0);
    assertEqualFloat (destPositions2[1], 1);

    // Count of 3.
    status = GdipSetLinePresetBlend (brush, blend3, positions3, 3);
    assertEqualInt (status, Ok);

    status = GdipGetLinePresetBlend (brush, destBlend3, destPositions3, 3);
    assertEqualInt (status, Ok);
    assertEqualInt (destBlend3[0], 1);
    assertEqualInt (destBlend3[1], 2);
    assertEqualInt (destBlend3[2], 3);
    assertEqualFloat (destPositions3[0], 0);
    assertEqualFloat (destPositions3[1], 0.5);
    assertEqualFloat (destPositions3[2], 1);

    // Should clear the existing blend.
    status = GdipSetLineBlend (brush, lineBlend, linePositions, 3);
    assertEqualInt (status, Ok);

    status = GdipSetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    assertEqualInt (status, Ok);

    status = GdipGetLineBlendCount (brush, &blendCount);
    assertEqualInt (status, Ok);
    // GDI+ has a bug where it sets the blend and positions to NULL but incorrectly
    // decrements blendCount, instead of setting it to zero.
    // This causes GDI+ to crash. This is a bug we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    assertEqualInt (blendCount, 0);

    status = GdipGetLineBlend(brush, destBlendReal, destPositionsReal, blendCount);
    assertEqualInt(status, InvalidParameter);
#endif

    // Negative tests.
    status = GdipSetLinePresetBlend (NULL, blend3, positions3, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, NULL, positions3, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, NULL, 3);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend2, invalidPositions1, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend2, invalidPositions2, 2);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, 1);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, 0);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, -1);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineSigmaBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineSigmaBlend (brush, 0, 0.5f);
    assertEqualInt (status, Ok);

    status = GdipSetLineSigmaBlend (brush, 0.5f, 1);
    assertEqualInt (status, Ok);

    status = GdipSetLineSigmaBlend (brush, 1, 0);
    assertEqualInt (status, Ok);

    // Negative tests.
    status = GdipSetLineSigmaBlend (NULL, 0.5f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, -0.01f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, 1.01f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, 0.5f, -0.01f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineSigmaBlend (brush, 0.5f, 1.01f);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineLinearBlend ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineLinearBlend (brush, 0, 0.5f);
    assertEqualInt (status, Ok);

    status = GdipSetLineLinearBlend (brush, 0.5f, 1);
    assertEqualInt (status, Ok);

    status = GdipSetLineLinearBlend (brush, 1, 0);
    assertEqualInt (status, Ok);

    // Negative tests.
    status = GdipSetLineLinearBlend (NULL, 0.5f, 0.5f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineLinearBlend (brush, -0.01f, 0.5);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineLinearBlend (brush, 1.01f, 0.5);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineLinearBlend (brush, 0.5f, -0.01f);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineLinearBlend (brush, 0.5f, 1.01f);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setLineWrapMode ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpWrapMode wrapMode;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    // WrapModeTile.
    status = GdipSetLineWrapMode (brush, WrapModeTile);
    assertEqualInt (status, Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTile);
	
    // Same.
    status = GdipSetLineWrapMode (brush, WrapModeTile);
    assertEqualInt (status, Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTile);

    // WrapModeTileFlipX.
    status = GdipSetLineWrapMode (brush, WrapModeTileFlipX);
    assertEqualInt (status, Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipX);

    // WrapModeTileFlipY.
    status = GdipSetLineWrapMode (brush, WrapModeTileFlipY);
    assertEqualInt (status, Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    // WrapModeTileFlipXY.
    status = GdipSetLineWrapMode (brush, WrapModeTileFlipXY);
    assertEqualInt (status, Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipXY);

    // Invalid WrapMode - nop.
    status = GdipSetLineWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assertEqualInt (status, Ok);

    GdipGetLineWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipXY);

    // Negative tests.
    status = GdipSetLineWrapMode (NULL, WrapModeTile);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineWrapMode (brush, WrapModeClamp);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineWrapMode ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpWrapMode wrapMode;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    // Negative tests.
    status = GdipGetLineWrapMode (NULL, &wrapMode);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineWrapMode (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *transform;
    ARGB blend[2] = {1, 0};
    REAL positions[2] = {0, 1.0f};

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix (&transform);
	
    // With no preset blend.
    status = GdipGetLineTransform (brush, transform);
    assertEqualInt (status, Ok);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // With preset blend.
    GdipSetLinePresetBlend (brush, blend, positions, 2);
    status = GdipGetLineTransform (brush, transform);
    assertEqualInt (status, Ok);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipGetLineTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetLineTransform (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (transform);
}

static void test_setLineTransform ()
{
    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpMatrix *matrix;
    GpMatrix *nonInvertibleMatrix;
    GpMatrix *transform;
    ARGB blend[2] = {1, 0};
    REAL positions[2] = {0, 1.0f};

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

	// With no preset blend.
    status = GdipSetLineTransform (brush, matrix);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);
	
	// Same.
    status = GdipSetLineTransform (brush, matrix);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);
	
    // With preset blend.
    GdipSetLinePresetBlend (brush, blend, positions, 2);
    status = GdipSetLineTransform (brush, matrix);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 2, 3, 4, 5, 6, 7);

    // Negative tests.
    status = GdipSetLineTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineTransform (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetLineTransform (brush, nonInvertibleMatrix);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
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
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipResetLineTransform (NULL);
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix - nop.
    status = GdipMultiplyLineTransform (brush, NULL, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - negative.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Invalid MatrixOrder - positive.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Negative tests.
    status = GdipMultiplyLineTransform (NULL, matrix, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipMultiplyLineTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipTranslateLineTransform (brush, 5, 6, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslateLineTransform (NULL, 1, 1, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipTranslateLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipTranslateLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipScaleLineTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScaleLineTransform (NULL, 1, 1, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipScaleLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipScaleLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipRotateLineTransform (brush, 90, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotateLineTransform (NULL, 90, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipRotateLineTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipRotateLineTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

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
    assertEqualInt (status, Ok);
    assert (clonedBrush && clonedBrush != brush);

    GdipGetBrushType (clonedBrush, &brushType);
    assertEqualInt (brushType, BrushTypeLinearGradient);

    GdipGetLineWrapMode ((GpLineGradient *) clonedBrush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    GdipGetLineColors (brush, colors);
    assertEqualInt (colors[0], 10);
    assertEqualInt (colors[1], 11);

    GdipGetLineRect ((GpLineGradient *) clonedBrush, &rect);
    assertEqualFloat (rect.X, 1);
    assertEqualFloat (rect.Y, 3);
    assertEqualFloat (rect.Width, 1);
    assertEqualFloat (rect.Height, 2);

    GdipGetLineBlendCount ((GpLineGradient *) clonedBrush, &blendCount);
    assertEqualInt (blendCount, 2);

    GdipGetLineBlend ((GpLineGradient *) clonedBrush, blendResult, positionsResult, 2);
    assertEqualFloat (blendResult[0], 1);
    assertEqualFloat (blendResult[1], 2);
    assertEqualFloat (positionsResult[0], 0);
    assertEqualFloat (positionsResult[1], 1);

    GdipGetLineTransform ((GpLineGradient *) clonedBrush, matrix);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    GdipGetLineGammaCorrection ((GpLineGradient *) clonedBrush, &useGammaCorrection);
    assert (useGammaCorrection == TRUE);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);
}

static void test_delete ()
{
    GpStatus status;
    GpRectF rect = { 1, 3, 1, 2 };
    GpLineGradient *brush;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

	status = GdipDeleteBrush ((GpBrush *) brush);
	assertEqualInt (status, Ok);
}

int
main (int argc, char**argv)
{
	STARTUP;

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
	test_delete ();

    SHUTDOWN;
    return 0;
}
