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
    ASSERT_EQ (BrushTypeLinearGradient, brushType);

    status = GdipGetLineRect (brush, &rect);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (x, rect.X);
    ASSERT_FLOAT_EQ_NAN (y, rect.Y);
    ASSERT_FLOAT_EQ_NAN (width, rect.Width);
    ASSERT_FLOAT_EQ_NAN (height, rect.Height);

    status = GdipGetLineColors (brush, colors);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (startColor, colors[0]);
    ASSERT_EQ (endColor, colors[1]);

    status = GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (expectedWrapMode, wrapMode);

    status = GdipGetLineTransform (brush, brushTransform);
    ASSERT_EQ (Ok, status);
    verifyMatrix (brushTransform, e1, e2, e3, e4, e5, e6);

    GdipDeleteMatrix (brushTransform);
}

TEST(LinearGradientBrush, GdipCreateLineBrush)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipX.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipY.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipXY
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipXY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // Invalid WrapMode.
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrush (&middleCenter, &middleLeft, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 2, 2, 10, 11, WrapModeTile, -1, 0, 0, -1, 4, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrush (&middleCenter, &topLeft, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 2, 2, 2, 10, 11, WrapModeTile, -1, -1, 1, -1, 1, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrush (&middleCenter, &topCenter, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 2, 2, 2, 2, 10, 11, WrapModeTile, 0, -1, 1, 0, 0, 6);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrush (&middleCenter, &topRight, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 3, 2, 2, 2, 10, 11, WrapModeTile, 1, -1, 1, 1, -3, 4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrush (&middleCenter, &middleRight, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 3, 3, 2, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrush (&middleCenter, &bottomRight, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 3, 4, 2, 2, 10, 11, WrapModeTile, 1, 1, -1, 1, 5, -4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrush (&middleCenter, &bottomCenter, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 2, 4, 2, 2, 10, 11, WrapModeTile, -0, 1, -1, -0, 8, 2);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrush (&middleCenter, &bottomLeft, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 4, 2, 2, 10, 11, WrapModeTile, -1, 1, -1, -1, 9, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (NULL, &point2, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, NULL, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, &point1, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (!brush);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrush (&point1, &point2, 10, 11, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipCreateLineBrushI)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipX.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipY.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // WrapModeTileFlipXY
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipXY, 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // Invalid WrapMode.
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, -2, 0.8f, 0.4f, -3.2f, 5.4f);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &middleLeft, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 2, 2, 10, 11, WrapModeTile, -1, 0, 0, -1, 4, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &topLeft, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 2, 2, 2, 10, 11, WrapModeTile, -1, -1, 1, -1, 1, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &topCenter, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 2, 2, 2, 2, 10, 11, WrapModeTile, 0, -1, 1, 0, 0, 6);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y > point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &topRight, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 3, 2, 2, 2, 10, 11, WrapModeTile, 1, -1, 1, 1, -3, 4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y == point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &middleRight, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 3, 3, 2, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X < point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &bottomRight, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 3, 4, 2, 2, 10, 11, WrapModeTile, 1, 1, -1, 1, 5, -4);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X == point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &bottomCenter, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 2, 4, 2, 2, 10, 11, WrapModeTile, -0, 1, -1, -0, 8, 2);
    GdipDeleteBrush ((GpBrush *) brush);

    // point1.X > point2.X, point1.Y < point2.Y
    status = GdipCreateLineBrushI (&middleCenter, &bottomLeft, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 4, 2, 2, 10, 11, WrapModeTile, -1, 1, -1, -1, 9, 8);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (NULL, &point2, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (&point1, NULL, 10, 11, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushI (&point1, &point2, 10, 11, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipCreateLineBrushFromRect)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRectF rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRect (&rect2, 10, 11, LinearGradientModeBackwardDiagonal, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeForwardDiagonal, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeVertical, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (NULL, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    GpRectF rect3 = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect3, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    GpRectF rect4 = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect4, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeHorizontal - 1), WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (NULL, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRect (&rect1, 10, 11, LinearGradientModeHorizontal, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipCreateLineBrushFromRectI)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRect normalRect = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRect negativeRect = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectI (&negativeRect, 10, 11, LinearGradientModeBackwardDiagonal, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeForwardDiagonal, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeVertical, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (NULL, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    GpRect zeroWidth = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&zeroWidth, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    GpRect zeroHeight = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&zeroHeight, 10, 11, LinearGradientModeBackwardDiagonal, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, (LinearGradientMode)(LinearGradientModeHorizontal - 1), WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (NULL, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, (LinearGradientMode)(LinearGradientModeBackwardDiagonal + 1), WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeHorizontal, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectI (&normalRect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipCreateLineBrushFromRectWithAngle)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 0, TRUE, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRectF rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectWithAngle (&rect2, 10, 11, 135, TRUE, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect2, 10, 11, -225, FALSE, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1.500000, 1.500000, -0.750000, -0.750000, 2.750000, 2.750000);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 405, TRUE, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 45, FALSE, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1.5, 1.5, -0.75, 0.75, 2.25, -1.25);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (NULL, 10, 11, 90, TRUE, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    GpRectF rect3 = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect3, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    GpRectF rect4 = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect4, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngle (&rect1, 10, 11, 90, TRUE, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipCreateLineBrushFromRectWithAngleI)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRect rect1 = { 1, 3, 1, 2 };

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 0, TRUE, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTile, 1, 0, 0, 1, 0, 0);
    GdipDeleteBrush ((GpBrush *) brush);

    GpRect rect2 = { 1, 3, -1, -2 };
    status = GdipCreateLineBrushFromRectWithAngleI (&rect2, 10, 11, 135, TRUE, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1, 2, -0.5, -1, 2, 3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect2, 10, 11, -225, FALSE, (WrapMode)(WrapModeTile - 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, -1, -2, 10, 11, (WrapMode)(WrapModeTile - 1), -1.500000, 1.500000, -0.750000, -0.750000, 2.750000, 2.750000);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 405, TRUE, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1, 2, -0.5, 1, 2, -3);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 45, FALSE, (WrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, (WrapMode)(WrapModeClamp + 1), 1.5, 1.5, -0.75, 0.75, 2.25, -1.25);
    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyLineGradientBrush (brush, 1, 3, 1, 2, 10, 11, WrapModeTileFlipX, 0, 2, -0.5, 0, 3.5, 1);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (NULL, 10, 11, 90, TRUE, WrapModeTile, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    GpRect rect3 = { 1, 3, 0, 1 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect3, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    GpRect rect4 = { 1, 3, 1, 0 };
    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect4, 10, 11, 90, TRUE, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    brush = (GpLineGradient *) 0xCC;
    status = GdipCreateLineBrushFromRectWithAngleI (&rect1, 10, 11, 90, TRUE, WrapModeTile, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpLineGradient *) 0xCC);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineColors)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB colors[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineColors (brush, 1, 2);
    ASSERT_EQ (Ok, status);

    GdipGetLineColors (brush, colors);
    ASSERT_EQ (1, colors[0]);
    ASSERT_EQ (2, colors[1]);

    // Negative tests.
    status = GdipSetLineColors (NULL, 1, 2);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineColors)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB colors[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetLineColors (NULL, colors);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineColors (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineRect)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpRectF lineRect;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetLineRect (NULL, &lineRect);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineRect (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineRectI)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRect lineRect;

    // Whole numbers.
    GpRectF wholeNumbers = { 1, 3, 1, 2 };
    GdipCreateLineBrushFromRect (&wholeNumbers, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, lineRect.X);
    ASSERT_EQ (3, lineRect.Y);
    ASSERT_EQ (1, lineRect.Width);
    ASSERT_EQ (2, lineRect.Height);

    GdipDeleteBrush ((GpBrush *) brush);

    // > 0.5 is rounded up.
    GpRectF greaterThanPointFive = { 1.6f, 3.6f, 1.6f, 2.6f };
    GdipCreateLineBrushFromRect (&greaterThanPointFive, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, lineRect.X);
    ASSERT_EQ (4, lineRect.Y);
    ASSERT_EQ (2, lineRect.Width);
    ASSERT_EQ (3, lineRect.Height);

    GdipDeleteBrush ((GpBrush *) brush);

    // 0.5 is rounded up.
    GpRectF pointFive = { 1.5f, 3.5f, 1.5f, 2.5f };
    GdipCreateLineBrushFromRect (&pointFive, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, lineRect.X);
    ASSERT_EQ (4, lineRect.Y);
    ASSERT_EQ (2, lineRect.Width);
    ASSERT_EQ (3, lineRect.Height);

    GdipDeleteBrush ((GpBrush *) brush);

    // < 0.5 is rounded down.
    GpRectF lessThanPointFive = { 1.1f, 3.1f, 1.1f, 2.1f };
    GdipCreateLineBrushFromRect (&lessThanPointFive, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineRectI (brush, &lineRect);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, lineRect.X);
    ASSERT_EQ (3, lineRect.Y);
    ASSERT_EQ (1, lineRect.Width);
    ASSERT_EQ (2, lineRect.Height);

    // Negative tests.
    status = GdipGetLineRectI (NULL, &lineRect);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineRectI (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineGammaCorrection)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    BOOL useGammaCorrection;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // True.
    status = GdipSetLineGammaCorrection (brush, TRUE);
    ASSERT_EQ (Ok, status);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    ASSERT_TRUE (useGammaCorrection);

    // Same.
    status = GdipSetLineGammaCorrection (brush, TRUE);
    ASSERT_EQ (Ok, status);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    ASSERT_TRUE (useGammaCorrection);

    // False.
    status = GdipSetLineGammaCorrection (brush, FALSE);
    ASSERT_EQ (Ok, status);

    GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    ASSERT_FALSE (useGammaCorrection);

    // Negative tests.
    status = GdipSetLineGammaCorrection (NULL, TRUE);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineGammaCorrection)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    BOOL useGammaCorrection;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineGammaCorrection (brush, &useGammaCorrection);
    ASSERT_EQ (Ok, status);
    ASSERT_FALSE (useGammaCorrection);

    // Negative tests.
    status = GdipGetLineGammaCorrection (NULL, &useGammaCorrection);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineGammaCorrection (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineBlendCount)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    INT blendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLineBlendCount (brush, &blendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, blendCount);

    // Negative tests.
    status = GdipGetLineBlendCount (NULL, &blendCount);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineBlendCount (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

static void fill_array (REAL *array, INT count, REAL value)
{
    for (int i = 0; i < count; i++)
        array[i] = value;
}

TEST(LinearGradientBrush, GdipGetLineBlend)
{
    STARTUP

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
    status = GdipGetLineBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (123, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    // Positions are meaningless for blends with a count of 1 so this parameter is ignored.
    ASSERT_FLOAT_EQ_NAN (123, positions[0]);
    ASSERT_FLOAT_EQ_NAN (123, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Negative tests.
    GdipSetLineBlend (brush, threeBlends, threePositions, 3);
    status = GdipGetLineBlend (brush, blend, positions, 1);
    ASSERT_EQ (InsufficientBuffer, status);

    status = GdipGetLineBlend (NULL, blend, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineBlend (brush, NULL, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineBlend (brush, blend, NULL, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineBlend (brush, blend, positions, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineBlend (brush, blend, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineBlend)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (2, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    ASSERT_FLOAT_EQ_NAN (0, positions[0]);
    ASSERT_FLOAT_EQ_NAN (1, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Same.
    status = GdipSetLineBlend (brush, twoBlends, twoPositions, 2);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (2, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    ASSERT_FLOAT_EQ_NAN (0, positions[0]);
    ASSERT_FLOAT_EQ_NAN (1, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Three blends - equal count.
    status = GdipSetLineBlend (brush, threeBlends, threePositions, 3);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 3);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (1, blend[0]);
    ASSERT_FLOAT_EQ_NAN (2, blend[1]);
    ASSERT_FLOAT_EQ_NAN (3, blend[2]);
    ASSERT_FLOAT_EQ_NAN (0, positions[0]);
    ASSERT_FLOAT_EQ_NAN (0.5, positions[1]);
    ASSERT_FLOAT_EQ_NAN (1, positions[2]);

    // One blend - equal count.
    status = GdipSetLineBlend (brush, oneBlend, onePosition, 1);
    ASSERT_EQ (Ok, status);

    fill_array (blend, ARRAY_SIZE (blend), 123);
    fill_array (positions, ARRAY_SIZE (positions), 123);
    status = GdipGetLineBlend (brush, blend, positions, 1);
    ASSERT_EQ (Ok, status);
    ASSERT_FLOAT_EQ_NAN (10, blend[0]);
    ASSERT_FLOAT_EQ_NAN (123, blend[1]);
    ASSERT_FLOAT_EQ_NAN (123, blend[2]);
    ASSERT_FLOAT_EQ_NAN (123, positions[0]);
    ASSERT_FLOAT_EQ_NAN (123, positions[1]);
    ASSERT_FLOAT_EQ_NAN (123, positions[2]);

    // Should clear the existing blend.
    status = GdipSetLinePresetBlend (brush, pathPresetBlend, pathPresetPositions, 3);
    ASSERT_EQ (Ok, status);

    status = GdipSetLineBlend (brush, threeBlends, threePositions, 3);
    ASSERT_EQ (Ok, status);

    status = GdipGetLinePresetBlendCount (brush, &presetBlendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (0, presetBlendCount);

    // Negative tests.
    status = GdipSetLineBlend (NULL, threeBlends, threePositions, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineBlend (brush, NULL, threePositions, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineBlend (brush, threeBlends, NULL, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineBlend (brush, twoBlends, invalidPositions1, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineBlend (brush, twoBlends, invalidPositions2, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineBlend (brush, threeBlends, threePositions, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineBlend (brush, threeBlends, threePositions, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLinePresetBlendCount)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    INT blendCount;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipGetLinePresetBlendCount (brush, &blendCount);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (0, blendCount);

    // Negative tests.
    status = GdipGetLinePresetBlendCount (NULL, &blendCount);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlendCount (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLinePresetBlend)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    ARGB blend[2];
    REAL positions[2];

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetLinePresetBlend (brush, blend, positions, 2);
    ASSERT_EQ (GenericError, status);

    status = GdipGetLinePresetBlend (NULL, blend, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, NULL, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, blend, NULL, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, blend, positions, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, blend, positions, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, blend, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (NULL, blend, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, NULL, positions, -1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLinePresetBlend (brush, blend, NULL, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLinePresetBlend)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    status = GdipGetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, destBlend2[0]);
    ASSERT_EQ (0, destBlend2[1]);
    ASSERT_FLOAT_EQ_NAN (0, destPositions2[0]);
    ASSERT_FLOAT_EQ_NAN (1, destPositions2[1]);

    // Same.
    status = GdipSetLinePresetBlend (brush, blend2, positions2, 2);
    ASSERT_EQ (Ok, status);

    status = GdipGetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, destBlend2[0]);
    ASSERT_EQ (0, destBlend2[1]);
    ASSERT_FLOAT_EQ_NAN (0, destPositions2[0]);
    ASSERT_FLOAT_EQ_NAN (1, destPositions2[1]);

    // Count of 3.
    status = GdipSetLinePresetBlend (brush, blend3, positions3, 3);
    ASSERT_EQ (Ok, status);

    status = GdipGetLinePresetBlend (brush, destBlend3, destPositions3, 3);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (1, destBlend3[0]);
    ASSERT_EQ (2, destBlend3[1]);
    ASSERT_EQ (3, destBlend3[2]);
    ASSERT_FLOAT_EQ_NAN (0, destPositions3[0]);
    ASSERT_FLOAT_EQ_NAN (0.5, destPositions3[1]);
    ASSERT_FLOAT_EQ_NAN (1, destPositions3[2]);

    // Should clear the existing blend.
    status = GdipSetLineBlend (brush, lineBlend, linePositions, 3);
    ASSERT_EQ (Ok, status);

    status = GdipSetLinePresetBlend (brush, destBlend2, destPositions2, 2);
    ASSERT_EQ (Ok, status);

    status = GdipGetLineBlendCount (brush, &blendCount);
    ASSERT_EQ (Ok, status);
    // GDI+ has a bug where it sets the blend and positions to NULL but incorrectly
    // decrements blendCount, instead of setting it to zero.
    // This causes GDI+ to crash. This is a bug we don't want to replicate.
#if !defined(USE_WINDOWS_GDIPLUS)
    ASSERT_EQ (0, blendCount);

    status = GdipGetLineBlend(brush, destBlendReal, destPositionsReal, blendCount);
    ASSERT_EQ (InvalidParameter, status);
#endif

    // Negative tests.
    status = GdipSetLinePresetBlend (NULL, blend3, positions3, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, NULL, positions3, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, blend3, NULL, 3);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, blend2, invalidPositions1, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, blend2, invalidPositions2, 2);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, 1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, 0);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLinePresetBlend (brush, blend3, positions3, -1);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineSigmaBlend)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineSigmaBlend (brush, 0, 0.5f);
    ASSERT_EQ (Ok, status);

    status = GdipSetLineSigmaBlend (brush, 0.5f, 1);
    ASSERT_EQ (Ok, status);

    status = GdipSetLineSigmaBlend (brush, 1, 0);
    ASSERT_EQ (Ok, status);

    // Negative tests.
    status = GdipSetLineSigmaBlend (NULL, 0.5f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineSigmaBlend (brush, -0.01f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineSigmaBlend (brush, 1.01f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineSigmaBlend (brush, 0.5f, -0.01f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineSigmaBlend (brush, 0.5f, 1.01f);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineLinearBlend)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTile, &brush);

    status = GdipSetLineLinearBlend (brush, 0, 0.5f);
    ASSERT_EQ (Ok, status);

    status = GdipSetLineLinearBlend (brush, 0.5f, 1);
    ASSERT_EQ (Ok, status);

    status = GdipSetLineLinearBlend (brush, 1, 0);
    ASSERT_EQ (Ok, status);

    // Negative tests.
    status = GdipSetLineLinearBlend (NULL, 0.5f, 0.5f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineLinearBlend (brush, -0.01f, 0.5);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineLinearBlend (brush, 1.01f, 0.5);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineLinearBlend (brush, 0.5f, -0.01f);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineLinearBlend (brush, 0.5f, 1.01f);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineWrapMode)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpWrapMode wrapMode;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    // WrapModeTile.
    status = GdipSetLineWrapMode (brush, WrapModeTile);
    ASSERT_EQ (Ok, status);

    GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTile, wrapMode);

    // Same.
    status = GdipSetLineWrapMode (brush, WrapModeTile);
    ASSERT_EQ (Ok, status);

    GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTile, wrapMode);

    // WrapModeTileFlipX.
    status = GdipSetLineWrapMode (brush, WrapModeTileFlipX);
    ASSERT_EQ (Ok, status);

    GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipX, wrapMode);

    // WrapModeTileFlipY.
    status = GdipSetLineWrapMode (brush, WrapModeTileFlipY);
    ASSERT_EQ (Ok, status);

    GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipY, wrapMode);

    // WrapModeTileFlipXY.
    status = GdipSetLineWrapMode (brush, WrapModeTileFlipXY);
    ASSERT_EQ (Ok, status);

    GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipXY, wrapMode);

    // Invalid WrapMode - nop.
    status = GdipSetLineWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    ASSERT_EQ (Ok, status);

    GdipGetLineWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipXY, wrapMode);

    // Negative tests.
    status = GdipSetLineWrapMode (NULL, WrapModeTile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineWrapMode (brush, WrapModeClamp);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineWrapMode)
{
    STARTUP

    GpStatus status;
    GpLineGradient *brush;
    GpRectF rect = { 1, 3, 1, 2 };
    GpWrapMode wrapMode;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    // Negative tests.
    status = GdipGetLineWrapMode (NULL, &wrapMode);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineWrapMode (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipGetLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // With preset blend.
    GdipSetLinePresetBlend (brush, blend, positions, 2);
    status = GdipGetLineTransform (brush, transform);
    ASSERT_EQ (Ok, status);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipGetLineTransform (NULL, transform);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetLineTransform (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipSetLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Same.
    status = GdipSetLineTransform (brush, matrix);
    ASSERT_EQ (Ok, status);

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 2, 3, 4, 5, 6, 7);

    // Negative tests.
    status = GdipSetLineTransform (NULL, transform);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineTransform (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetLineTransform (brush, nonInvertibleMatrix);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipResetLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipResetLineTransform (NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipMultiplyLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix - nop.
    status = GdipMultiplyLineTransform (brush, NULL, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - negative.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Invalid MatrixOrder - positive.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipMultiplyLineTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Negative tests.
    status = GdipMultiplyLineTransform (NULL, matrix, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipMultiplyLineTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipTranslateLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipTranslateLineTransform (brush, 5, 6, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslateLineTransform (NULL, 1, 1, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipTranslateLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipTranslateLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipScaleLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipScaleLineTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScaleLineTransform (NULL, 1, 1, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipScaleLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipScaleLineTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipRotateLineTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetLineTransform (brush, originalTransform);

    status = GdipRotateLineTransform (brush, 90, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetLineTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotateLineTransform (NULL, 90, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRotateLineTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRotateLineTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipCloneBrush)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    assert (clonedBrush && clonedBrush != brush);

    GdipGetBrushType (clonedBrush, &brushType);
    ASSERT_EQ (BrushTypeLinearGradient, brushType);

    GdipGetLineWrapMode ((GpLineGradient *) clonedBrush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipY, wrapMode);

    GdipGetLineColors (brush, colors);
    ASSERT_EQ (10, colors[0]);
    ASSERT_EQ (11, colors[1]);

    GdipGetLineRect ((GpLineGradient *) clonedBrush, &rect);
    ASSERT_FLOAT_EQ_NAN (1, rect.X);
    ASSERT_FLOAT_EQ_NAN (3, rect.Y);
    ASSERT_FLOAT_EQ_NAN (1, rect.Width);
    ASSERT_FLOAT_EQ_NAN (2, rect.Height);

    GdipGetLineBlendCount ((GpLineGradient *) clonedBrush, &blendCount);
    ASSERT_EQ (2, blendCount);

    GdipGetLineBlend ((GpLineGradient *) clonedBrush, blendResult, positionsResult, 2);
    ASSERT_FLOAT_EQ_NAN (blendResult[0], 1);
    ASSERT_FLOAT_EQ_NAN (blendResult[1], 2);
    ASSERT_FLOAT_EQ_NAN (0, positionsResult[0]);
    ASSERT_FLOAT_EQ_NAN (1, positionsResult[1]);

    GdipGetLineTransform ((GpLineGradient *) clonedBrush, matrix);
    verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

    GdipGetLineGammaCorrection ((GpLineGradient *) clonedBrush, &useGammaCorrection);
    ASSERT_TRUE (useGammaCorrection);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush ((GpBrush *) clonedBrush);
    GdipDeleteMatrix (transform);
    GdipDeleteMatrix (matrix);

    SHUTDOWN
}

TEST(LinearGradientBrush, GdipDeleteBrush)
{
    STARTUP

    GpStatus status;
    GpRectF rect = { 1, 3, 1, 2 };
    GpLineGradient *brush;

    GdipCreateLineBrushFromRect (&rect, 10, 11, LinearGradientModeHorizontal, WrapModeTileFlipY, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    ASSERT_EQ (Ok, status);

    SHUTDOWN
}
