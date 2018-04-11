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

GpImage *image;
GpGraphics *graphics;

#define verifyRegion(region, expectedX, expectedY, expectedWidth, expectedHeight, expectedIsEmpty, expectedIsInfinite) \
{ \
	GpStatus status; \
	GpRectF bounds; \
	BOOL isEmpty; \
	BOOL isInfinite; \
\
	status = GdipGetRegionBounds (region, graphics, &bounds); \
	assertEqualInt (status, Ok); \
	assertEqualFloat (bounds.X, expectedX); \
	assertEqualFloat (bounds.Y, expectedY); \
	assertEqualFloat (bounds.Width, expectedWidth); \
	assertEqualFloat (bounds.Height, expectedHeight); \
\
	status = GdipIsEmptyRegion (region, graphics, &isEmpty); \
	assertEqualInt (status, Ok); \
	assert (isEmpty == (BOOL) expectedIsEmpty); \
\
	status = GdipIsInfiniteRegion (region, graphics, &isInfinite); \
	assertEqualInt (status, Ok); \
	assert (isInfinite == (BOOL) expectedIsInfinite); \
}

static void test_createRegion ()
{
	GpStatus status;
	GpRegion *region;

	status = GdipCreateRegion (&region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegion (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createRegionRect ()
{
	GpStatus status;
	GpRectF normalRect = {1, 2, 3, 4};
	GpRectF negativeWidthRect = {1, 2, -3, 4};
	GpRectF negativeHeightRect = {1, 2, 3, -4};
	GpRectF zeroRect = {0, 0, 0, 0};
	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GpRegion *region;

	// Normal.
	status = GdipCreateRegionRect (&normalRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative width.
	status = GdipCreateRegionRect (&negativeWidthRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative height.
	status = GdipCreateRegionRect (&negativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Zero.
	status = GdipCreateRegionRect (&zeroRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Infinite.
	status = GdipCreateRegionRect (&infiniteRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegionRect (NULL, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRect (&normalRect, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createRegionRectI ()
{
	GpStatus status;
	GpRect normalRect = {1, 2, 3, 4};
	GpRect negativeWidthRect = {1, 2, -3, 4};
	GpRect negativeHeightRect = {1, 2, 3, -4};
	GpRect zeroRect = {0, 0, 0, 0};
	GpRect infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	GpRegion *region;

	// Normal.
	status = GdipCreateRegionRectI (&normalRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative width.
	status = GdipCreateRegionRectI (&negativeWidthRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Negative height.
	status = GdipCreateRegionRectI (&negativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Zero.
	status = GdipCreateRegionRectI (&zeroRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Infinite.
	status = GdipCreateRegionRectI (&infiniteRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegionRectI (NULL, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRectI (&normalRect, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createRegionPath ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;

	// Normal path.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);

	status = GdipCreateRegionPath (path, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
	GdipDeleteRegion (region);
	GdipDeletePath (path);

	// Empty path.
	GdipCreatePath (FillModeWinding, &path);

	status = GdipCreateRegionPath (path, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	GdipDeleteRegion (region);
	GdipDeletePath (path);

	// Infinite path - rectangle.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f);

	status = GdipCreateRegionPath (path, &region);
	assertEqualInt (status, Ok);
	// FIXME: libgdiplus does not identify this as an infinite region.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
#else
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, FALSE);
#endif
	GdipDeleteRegion (region);
	GdipDeletePath (path);

	// Infinite path - polygon.
	GdipCreatePath (FillModeWinding, &path);
	PointF polygonPoints[] = {
		{-4194304, -4194304},
		{-4194304, 4194304},
		{4194304, 4194304},
		{4194304, -4194304},
		{-4194304, -4194304}
	};
	GdipAddPathPolygon (path, polygonPoints, 5);

	status = GdipCreateRegionPath (path, &region);
	assertEqualInt (status, Ok);
	// FIXME: libgdiplus does not identify this as an infinite region.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
#else
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, FALSE);
#endif
	GdipDeleteRegion (region);
	GdipDeletePath (path);

	// Empty path - curve.
	GdipCreatePath (FillModeWinding, &path);
	PointF emptyCurvePoints[] = {
		{10, 10},
		{20, 20}
	};
	status = GdipAddPathCurve (path, emptyCurvePoints, 2);

	status = GdipCreateRegionPath (path, &region);
	assertEqualInt (status, Ok);
	// FIXME: libgdiplus does not identify this as an empty infinite region.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyRegion (region, 10, 10, 10, 10, TRUE, FALSE);
#else
	verifyRegion (region, 10, 10, 10, 10, FALSE, FALSE);
#endif
	GdipDeleteRegion (region);
	GdipDeletePath (path);

	// Not infinite path - curve.
	GdipCreatePath (FillModeWinding, &path);
	PointF infiniteCurvePoints[] = {
		{-4194304.0f, -4194304.0f},
		{4194304.0f, 4194304.0f}
	};
	status = GdipAddPathCurve (path, infiniteCurvePoints, 2);

	status = GdipCreateRegionPath (path, &region);
	assertEqualInt (status, Ok);
	// FIXME: GetRegionBounds fails with OutOfMemory.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, TRUE, FALSE);
#endif
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegionPath (NULL, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionPath (path, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (path);
}

static void test_getRegionScansCount ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpMatrix *matrix;
	UINT count;

	GdipCreateMatrix (&matrix);

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipGetRegionScansCount (region, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	status = GdipGetRegionScansCount (region, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	
	status = GdipGetRegionScansCount (region, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);
	
	status = GdipGetRegionScansCount (region, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Negative tests.
	status = GdipGetRegionScansCount (NULL, &count, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionScansCount (region, NULL, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionScansCount (region, &count, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteRegion (region);
}

static void test_getRegionScans ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpMatrix *matrix;
	GpRectF scans[1];
	INT count;

	GdipCreateMatrix (&matrix);

	// Rect region - non null rects.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 10);
	assertEqualFloat (scans[0].Y, 20);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);

	// Rect region - null rects.
	count = 0xFF;
	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);

	// Path region - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (scans[0].X, 10);
	assertEqualFloat (scans[0].Y, 20);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
#endif
	assertEqualInt (count, 1);

	// Path region - null rects.
	count = 0xFF;
	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Empty region - non null rects.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);

	// Empty region - null rects.
	count = 0xFF;
	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);

	// Infinite region - non null rects.
	GdipCreateRegion (&region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, -4194304);
	assertEqualFloat (scans[0].Y, -4194304);
	assertEqualFloat (scans[0].Width, 8388608);
	assertEqualFloat (scans[0].Height, 8388608);
	assertEqualInt (count, 1);

	// Infinite region - null rects.
	count = 0xFF;
	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Negative tests.
	status = GdipGetRegionScans (NULL, scans, &count, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionScans (region, scans, NULL, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionScans (region, scans, &count, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteRegion (region);
}

#if defined(USE_WINDOWS_GDIPLUS)
static void test_getRegionScansI ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpMatrix *matrix;
	GpRect scans[1];
	INT count;

	GdipCreateMatrix (&matrix);

	// Rect region - non null rects.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 10);
	assertEqualFloat (scans[0].Y, 20);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);

	// Rect region - null rects.
	count = 0xFF;
	status = GdipGetRegionScansI (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);

	// Path region - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (scans[0].X, 10);
	assertEqualFloat (scans[0].Y, 20);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
#endif
	assertEqualInt (count, 1);

	// Path region - null rects.
	count = 0xFF;
	status = GdipGetRegionScansI (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Empty region - non null rects.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);

	// Empty region - null rects.
	count = 0xFF;
	status = GdipGetRegionScansI (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);

	// Infinite region - non null rects.
	GdipCreateRegion (&region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, -4194304);
	assertEqualFloat (scans[0].Y, -4194304);
	assertEqualFloat (scans[0].Width, 8388608);
	assertEqualFloat (scans[0].Height, 8388608);
	assertEqualInt (count, 1);

	// Infinite region - null rects.
	count = 0xFF;
	status = GdipGetRegionScansI (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Negative tests.
	status = GdipGetRegionScansI (NULL, scans, &count, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionScansI (region, scans, NULL, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionScansI (region, scans, &count, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteRegion (region);
}
#endif

int
main (int argc, char**argv)
{
	STARTUP;

	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppRGB, NULL, (GpBitmap **) &image);
	GdipGetImageGraphicsContext (image, &graphics);

	test_createRegion ();
	test_createRegionRect ();
	test_createRegionRectI ();
	test_createRegionPath ();
	test_getRegionScansCount ();
	test_getRegionScans ();
	// FIXME: implement GdipGetRegionScansI.
#if defined(USE_WINDOWS_GDIPLUS)
	test_getRegionScansI ();
#endif

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN;
	return 0;
}
