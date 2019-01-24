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

static void test_createRegion ()
{
	GpStatus status;
	GpRegion *region;

	status = GdipCreateRegion (&region);
	assertEqualInt (status, Ok);
	verifyInfiniteRegion (region);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegion (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createRegionRect ()
{
	GpStatus status;
	GpRectF normalRect = {1, 2, 3, 4};
	GpRegion *region;

	// Positive width, positive height.
	status = GdipCreateRegionRect (&normalRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	verifyRegionScans (region, &normalRect, sizeof (normalRect));
	GdipDeleteRegion (region);
	
	// > Infinite width, > infinite height.
	GpRectF greaterThanInfiniteGreaterThanInfiniteHeightRect = {1, 2, 8388609.0f, 8388609.0f};
	status = GdipCreateRegionRect (&greaterThanInfiniteGreaterThanInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388609.0f, 8388609.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// > Infinite width, infinite height.
	GpRectF greaterThanInfiniteInfiniteHeightRect = {1, 2, 8388609.0f, 8388608.0f};
	status = GdipCreateRegionRect (&greaterThanInfiniteInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388609.0f, 8388608.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// > Infinite width, positive height.
	GpRectF greaterThanInfinitePositiveHeightRect = {1, 2, 8388609.0f, 4};
	status = GdipCreateRegionRect (&greaterThanInfinitePositiveHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388609.0f, 4, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// > Infinite width, zero height.
	GpRectF greaterThanInfiniteZeroHeightRect = {1, 2, 8388609.0f, 0};
	status = GdipCreateRegionRect (&greaterThanInfiniteZeroHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388609.0f, 0, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);
	
	// > Infinite width, negative height.
	GpRectF greaterThanInfiniteNegativeHeightRect = {1, 2, 8388609.0f, -4};
	status = GdipCreateRegionRect (&greaterThanInfiniteNegativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388609.0f, -4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);
	
	// Infinite width, > infinite height.
	GpRectF infiniteWidthGreaterThanInfiniteHeightRect = {1, 2, 8388608.0f, 8388609.0f};
	status = GdipCreateRegionRect (&infiniteWidthGreaterThanInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388608.0f, 8388609.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// Infinite width, infinite height.
	GpRectF infiniteWidthInfiniteHeightRect = {1, 2, 8388608.0f, 8388608.0f};
	status = GdipCreateRegionRect (&infiniteWidthInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388608.0f, 8388608.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// Infinite width, positive height.
	GpRectF infiniteWidthPositiveHeightRect = {1, 2, 8388608.0f, 4};
	status = GdipCreateRegionRect (&infiniteWidthPositiveHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388608.0f, 4, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// Infinite width, zero height.
	GpRectF infiniteWidthZeroHeightRect = {1, 2, 8388608.0f, 0};
	status = GdipCreateRegionRect (&infiniteWidthZeroHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388608.0f, 0, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);
	
	// Infinite width, negative height.
	GpRectF infiniteWidthNegativeHeightRect = {1, 2, 8388608.0f, -4};
	status = GdipCreateRegionRect (&infiniteWidthNegativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 8388608.0f, -4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);
	
	// Positive width, > infinite height.
	GpRectF positiveWidthGreaterThanInfiniteHeightRect = {1, 2, 3, 8388609.0f};
	status = GdipCreateRegionRect (&positiveWidthGreaterThanInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 8388609.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);
	
	// Positive width, infinite height.
	GpRectF positiveWidthInfiniteHeightRect = {1, 2, 3, 8388608.0f};
	status = GdipCreateRegionRect (&positiveWidthInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 8388608.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);

	// Positive width, zero height.
	GpRectF positiveWidthZeroHeightRect = {1, 2, 3, 0};
	status = GdipCreateRegionRect (&positiveWidthZeroHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 0, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Positive width, negative height.
	GpRectF positiveWidthNegativeHeightRect = {1, 2, 3, -4};
	status = GdipCreateRegionRect (&positiveWidthNegativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Zero width, > infinite height.
	GpRectF zeroWidthGreaterThanInfiniteHeightRect = {1, 2, 0, 8388609.0f};
	status = GdipCreateRegionRect (&zeroWidthGreaterThanInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, 8388609.0f, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Zero width, infinite height.
	GpRectF zeroWidthInfiniteHeightRect = {1, 2, 0, 8388608.0f};
	status = GdipCreateRegionRect (&zeroWidthInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, 8388608.0f, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Zero width, positive height.
	GpRectF zeroWidthPositiveHeightRect = {1, 2, 0, 4};
	status = GdipCreateRegionRect (&zeroWidthPositiveHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, 4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Zero width, zero height.
	GpRectF zeroWidthZeroHeightRect = {1, 2, 0, 0};
	status = GdipCreateRegionRect (&zeroWidthZeroHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, 0, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Zero width, Negative height.
	GpRectF zeroWidthNegativeHeightRect = {1, 2, 0, -4};
	status = GdipCreateRegionRect (&zeroWidthNegativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, -4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Negative width, > infinite height.
	GpRectF negativeWidthGreaterThanInfiniteHeightRect = {1, 2, -3, 8388609.0f};
	status = GdipCreateRegionRect (&negativeWidthGreaterThanInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 8388609.0f, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Negative width, infinite height.
	GpRectF negativeWidthInfiniteHeightRect = {1, 2, -3, 8388608.0f};
	status = GdipCreateRegionRect (&negativeWidthInfiniteHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 8388608.0f, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Negative width, positive height.
	GpRectF negativeWidthPositiveHeightRect = {1, 2, -3, 4};
	status = GdipCreateRegionRect (&negativeWidthPositiveHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Negative width, zero height.
	GpRectF negativeWidthZeroHeightRect = {1, 2, -3, 0};
	status = GdipCreateRegionRect (&negativeWidthZeroHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 0, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Negative width, negative height.
	GpRectF negativeWidthNegativeHeightRect = {1, 2, -3, -4};
	status = GdipCreateRegionRect (&negativeWidthNegativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, -4, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
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
	verifyRegion (region, 1, 2, -3, 4, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Negative height.
	status = GdipCreateRegionRectI (&negativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Zero.
	status = GdipCreateRegionRectI (&zeroRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	verifyRegionScans (region, emptyScans, sizeof (emptyScans));
	GdipDeleteRegion (region);

	// Infinite.
	status = GdipCreateRegionRectI (&infiniteRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	verifyRegionScans (region, infiniteScans, sizeof (infiniteScans));
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
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
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
	verifyRegion (region, 10, 10, 10, 10, TRUE, FALSE);
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
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipCreateRegionPath (NULL, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionPath (path, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (path);
}

static void test_createRegionRgnData ()
{
	GpStatus status;
	GpRegion *region;
	BYTE infiniteRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x0C, 0x00, 0x00, 0x00,
		/* Checksum */      0x9B, 0x34, 0x22, 0xA3,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */ 0x03, 0x00, 0x00, 0x10
	};
	BYTE emptyRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x0C, 0x00, 0x00, 0x00,
		/* Checksum */      0xFE, 0x53, 0x9E, 0x1B,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */ 0x02, 0x00, 0x00, 0x10
	};
	BYTE rectMagicNumber2[] = {
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic Number */  0x02, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x00, 0x00, 0x00, 0x10,
		/* X */             0x00, 0x00, 0x80, 0x3F,
		/* Y */             0x00, 0x00, 0x00, 0x40,
		/* Width */         0x00, 0x00, 0x40, 0x40,
		/* Height */        0x00, 0x00, 0x80, 0x40
	};
	BYTE rectMagicNumber1[] = {
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xD0, 0x97, 0x65, 0xEE,
		/* Magic Number */  0x01, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x00, 0x00, 0x00, 0x10,
		/* X */             0x00, 0x00, 0x80, 0x3F,
		/* Y */             0x00, 0x00, 0x00, 0x40,
		/* Width */         0x00, 0x00, 0x40, 0x40,
		/* Height */        0x00, 0x00, 0x80, 0x40
	};
	BYTE rectEmpty[] = {
		/* Size */          0x0C, 0x00, 0x00, 0x00,
		/* Checksum */      0x0E, 0x81, 0x00, 0x6C,
		/* Magic Number */  0x01, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x02, 0x00, 0x00, 0x10
	};
	BYTE rectInfinite[] = {
		/* Size */          0x0C, 0x00, 0x00, 0x00,
		/* Checksum */      0x6B, 0xE6, 0xBC, 0xD4,
		/* Magic Number */  0x01, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x03, 0x00, 0x00, 0x10
	};
	BYTE zeroWidthRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xD2, 0xC2, 0x10, 0xBB,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x00, 0x00,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};
	BYTE zeroHeightRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x2C, 0x49, 0xE4, 0xA1,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x00, 0x00
	};
	BYTE zeroWidthAndHeightRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x09, 0x1B, 0x4F, 0xF6,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x00, 0x00,
		/* Height */ 0x00, 0x00, 0x00, 0x00
	};
	BYTE negativeWidthRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x65, 0x20, 0x5D, 0x5D,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0xC0,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};
	BYTE negativeHeightRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xD7, 0x13, 0x03, 0x01,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x80, 0xC0
	};
	BYTE invalidChecksum[] = {
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x00, 0x00, 0x00, 0x00,
		/* Magic Number */  0x02, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x00, 0x00, 0x00, 0x10,
		/* X */             0x00, 0x00, 0x80, 0x3F,
		/* Y */             0x00, 0x00, 0x00, 0x40,
		/* Width */         0x00, 0x00, 0x40, 0x40,
		/* Height */        0x00, 0x00, 0x80, 0x40
	};
	BYTE zeroSize[] = {
		/* Size */          0x00, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic Number */  0x02, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x00, 0x00, 0x00, 0x10,
		/* X */             0x00, 0x00, 0x80, 0x3F,
		/* Y */             0x00, 0x00, 0x00, 0x40,
		/* Width */         0x00, 0x00, 0x40, 0x40,
		/* Height */        0x00, 0x00, 0x80, 0x40
	};
	BYTE smallSize[] = {
		/* Size */          0x18, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic Number */  0x02, 0x10, 0xC0, 0xDB,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x00, 0x00, 0x00, 0x10,
		/* X */             0x00, 0x00, 0x80, 0x3F,
		/* Y */             0x00, 0x00, 0x00, 0x40,
		/* Width */         0x00, 0x00, 0x40, 0x40,
		/* Height */        0x00, 0x00, 0x80, 0x40
	};
	BYTE invalidMagicNumber[] = {
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x6A, 0x4E, 0xC2, 0x0F,
		/* Magic Number */  0xFF, 0xFF, 0xFF, 0xFF,
		/* Combining ops */ 0x00, 0x00, 0x00, 0x00,
		/* Rect */          0x00, 0x00, 0x00, 0x10,
		/* X */             0x00, 0x00, 0x80, 0x3F,
		/* Y */             0x00, 0x00, 0x00, 0x40,
		/* Width */         0x00, 0x00, 0x40, 0x40,
		/* Height */        0x00, 0x00, 0x80, 0x40
	};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE pathData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x30, 0x00, 0x00, 0x00,
		/* Checksum */      0xCA, 0x7C, 0x8B, 0x34,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */      0x01, 0x00, 0x00, 0x10,
		/* Size */      0x20, 0x00, 0x00, 0x00,
		/* Magic */     0x02, 0x10, 0xC0, 0xDB,
		/* Count */     0x04, 0x00, 0x00, 0x00,
		/* Flags */     0x00, 0x60, 0x00, 0x00,
		/* (1, 2) */    0x01, 0x00, 0x02, 0x00,
		/* (4, 2) */    0x04, 0x00, 0x02, 0x00,
		/* (4, 6) */    0x04, 0x00, 0x06, 0x00,
		/* (1, 6) */    0x01, 0x00, 0x06, 0x00,
		/* Start */     0x00,
		/* LineTo */    0x01,
		/* LineTo */    0x01,
		/* CloseLine */ 0x81,
	};
#endif

	// Infinite.
	status = GdipCreateRegionRgnData (infiniteRegionData, sizeof (infiniteRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	verifyRegionScans(region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);

	// Empty.
	status = GdipCreateRegionRgnData (emptyRegionData, sizeof (emptyRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);

	// Rect.
	status = GdipCreateRegionRgnData (rectMagicNumber2, sizeof (rectMagicNumber2), &region);
	assertEqualInt (status, Ok);
	GpRectF expectedRect = { 1, 2, 3, 4 };
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	verifyRegionScans(region, &expectedRect, sizeof (expectedRect));
	GdipDeleteRegion (region);

	status = GdipCreateRegionRgnData (rectMagicNumber1, sizeof (rectMagicNumber1), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	verifyRegionScans(region, &expectedRect, sizeof (expectedRect));
	GdipDeleteRegion (region);

	status = GdipCreateRegionRgnData (rectEmpty, sizeof (rectEmpty), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);

	status = GdipCreateRegionRgnData (rectInfinite, sizeof (rectInfinite), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	verifyRegionScans(region, infiniteScans, sizeof (infiniteScans));
	GdipDeleteRegion (region);

	status = GdipCreateRegionRgnData (zeroWidthRectRegionData, sizeof (zeroWidthRectRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, 4, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);
	
	status = GdipCreateRegionRgnData (zeroHeightRectRegionData, sizeof (zeroHeightRectRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 0, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);
	
	status = GdipCreateRegionRgnData (zeroWidthAndHeightRectRegionData, sizeof (zeroWidthAndHeightRectRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 0, 0, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);

	status = GdipCreateRegionRgnData (negativeWidthRectRegionData, sizeof (negativeWidthRectRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, -3, 4, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);

	status = GdipCreateRegionRgnData (negativeHeightRectRegionData, sizeof (negativeHeightRectRegionData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, TRUE, FALSE);
	verifyRegionScans(region, emptyScans, 0);
	GdipDeleteRegion (region);

	// Path.
	// FIXME: match GDI+ parsing.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateRegionRgnData (pathData, sizeof (pathData), &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, 4, FALSE, FALSE);
	verifyRegionScans(region, &expectedRect, sizeof (expectedRect));
	GdipDeleteRegion (region);
#endif

	// Negative tests.
	status = GdipCreateRegionRgnData (NULL, 12, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (rectMagicNumber2, -1, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (rectMagicNumber2, 0, &region);
	assertEqualInt (status, GenericError);

	status = GdipCreateRegionRgnData (NULL, 0, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (rectMagicNumber2, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (rectMagicNumber2, 7, &region);
	assertEqualInt (status, GenericError);

	status = GdipCreateRegionRgnData (NULL, 7, &region);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (rectMagicNumber2, 7, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (rectMagicNumber2, 12, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateRegionRgnData (invalidChecksum, sizeof (invalidChecksum), &region);
	assertEqualInt (status, GenericError);

	status = GdipCreateRegionRgnData (zeroSize, sizeof (zeroSize), &region);
	assertEqualInt (status, GenericError);

	status = GdipCreateRegionRgnData (smallSize, sizeof (smallSize), &region);
	assertEqualInt (status, GenericError);

	status = GdipCreateRegionRgnData (smallSize, sizeof (invalidMagicNumber), &region);
	assertEqualInt (status, GenericError);
}

static void test_getRegionData ()
{
	GpRegion *region;
	GpPath *path;
	GpStatus status;
	BYTE buffer[1024];
	UINT sizeFilled;
	
	// Infinite region.
	GdipCreateRegion (&region);
	BYTE infiniteRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x0C, 0x00, 0x00, 0x00,
		/* Checksum */      0x9B, 0x34, 0x22, 0xA3,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */ 0x03, 0x00, 0x00, 0x10
	};
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);

	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	BYTE emptyRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x0C, 0x00, 0x00, 0x00,
		/* Checksum */      0xFE, 0x53, 0x9E, 0x1B,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */ 0x02, 0x00, 0x00, 0x10
	};
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	
	// Infinite Rect.
	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GdipCreateRegionRect (&infiniteRect, &region);
	BYTE infiniteRectRegionData[] = {
		/* --RegionHeader-- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x19, 0x83, 0xAD, 0x2A,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0xCA,
		/* Y */      0x00, 0x00, 0x80, 0xCA,
		/* Width */  0x00, 0x00, 0x00, 0x4B,
		/* Height */ 0x00, 0x00, 0x00, 0x4B
	};
	verifyRegionData (region, infiniteRectRegionData);
	GdipDeleteRegion (region);
	
	// Empty Rect.
	GpRectF emptyRect = {0, 0, 0, 0};
	GdipCreateRegionRect (&emptyRect, &region);
	BYTE emptyRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xF6, 0xCF, 0xA7, 0x8B,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x00, 0x00,
		/* Y */      0x00, 0x00, 0x00, 0x00,
		/* Width */  0x00, 0x00, 0x00, 0x00,
		/* Height */ 0x00, 0x00, 0x00, 0x00
	};
	verifyRegionData (region, emptyRectRegionData);
	GdipDeleteRegion (region);

	// Rect.
	GpRectF rect = {1, 2, 3, 4};
	GdipCreateRegionRect (&rect, &region);
	BYTE nonEmptyRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};
	verifyRegionData (region, nonEmptyRectRegionData);
	GdipDeleteRegion (region);
	
	// Zero Width Rect.
	GpRectF zeroWidthRect = {1, 2, 0, 4};
	GdipCreateRegionRect (&zeroWidthRect, &region);
	BYTE zeroWidthRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xD2, 0xC2, 0x10, 0xBB,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x00, 0x00,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};
	verifyRegionData (region, zeroWidthRectRegionData);
	GdipDeleteRegion (region);
	
	// Zero Height Rect.
	GpRectF zeroHeightRect = {1, 2, 3, 0};
	GdipCreateRegionRect (&zeroHeightRect, &region);
	BYTE zeroHeightRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x2C, 0x49, 0xE4, 0xA1,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x00, 0x00
	};
	verifyRegionData (region, zeroHeightRectRegionData);
	GdipDeleteRegion (region);
	
	// Zero Width and Height Rect.
	GpRectF zeroWidthAndHeightRect = {1, 2, 0, 0};
	GdipCreateRegionRect (&zeroWidthAndHeightRect, &region);
	BYTE zeroWidthAndHeightRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x09, 0x1B, 0x4F, 0xF6,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x00, 0x00,
		/* Height */ 0x00, 0x00, 0x00, 0x00
	};
	verifyRegionData (region, zeroWidthAndHeightRectRegionData);
	GdipDeleteRegion (region);

	// Negative Width Rect.
	GpRectF negativeWidthRect = {1, 2, -3, 4};
	GdipCreateRegionRect (&negativeWidthRect, &region);
	BYTE negativeWidthRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0x65, 0x20, 0x5D, 0x5D,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0xC0,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};
	verifyRegionData (region, negativeWidthRectRegionData);
	GdipDeleteRegion (region);
	
	// Negative Height Rect.
	GpRectF negativeHeightRect = {1, 2, 3, -4};
	GdipCreateRegionRect (&negativeHeightRect, &region);
	BYTE negativeHeightRectRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xD7, 0x13, 0x03, 0x01,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x80, 0xC0
	};
	verifyRegionData (region, negativeHeightRectRegionData);
	GdipDeleteRegion (region);

	// Path.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	GdipCreateRegionPath (path, &region);
	// FIXME: match GDI+ serializing.
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE pathRegionData[] = {
		/* -- RegionHeader -- */
		/* Size */          0x30, 0x00, 0x00, 0x00,
		/* Checksum */      0xCA, 0x7C, 0x8B, 0x34,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */      0x01, 0x00, 0x00, 0x10,
		/* Size */      0x20, 0x00, 0x00, 0x00,
		/* Magic */     0x02, 0x10, 0xC0, 0xDB,
		/* Count */     0x04, 0x00, 0x00, 0x00,
		/* Flags */     0x00, 0x60, 0x00, 0x00,
		/* (1, 2) */    0x01, 0x00, 0x02, 0x00,
		/* (4, 2) */    0x04, 0x00, 0x02, 0x00,
		/* (4, 6) */    0x04, 0x00, 0x06, 0x00,
		/* (1, 6) */    0x01, 0x00, 0x06, 0x00,
		/* Start */     0x00,
		/* LineTo */    0x01,
		/* LineTo */    0x01,
		/* CloseLine */ 0x81,
	};
	verifyRegionData (region, pathRegionData);
#endif
	GdipDeletePath (path);

	// Negative tests.
	status = GdipGetRegionData (NULL, buffer, sizeof(buffer), &sizeFilled);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionData (region, NULL, sizeof(buffer), &sizeFilled);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipGetRegionData (region, buffer, 0, &sizeFilled);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipGetRegionData (region, buffer, 1, &sizeFilled);
	assertEqualInt (status, InsufficientBuffer);
	
	status = GdipGetRegionData (region, buffer, 2, &sizeFilled);
	assertEqualInt (status, InsufficientBuffer);
	
	status = GdipGetRegionData (region, buffer, sizeof (buffer), NULL);
	assertEqualInt (status, Ok);
	
	GdipDeleteRegion (region);
}

static void test_getRegionDataReplace ()
{
	GpRegion *region;
	GpRegion *other;
	GpPath *path;
	GpRectF rect1 = { 1, 2, 3, 4 };
	GpRectF rect2 = { 1, 2, 3, 4 };

	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	
	BYTE rectRegionData[] = {
		/* --RegionHeader-- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};

	// Infinite Region + Infinite Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Infinite Region + Empty Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Infinite Region + Rect Region.
	GdipCreateRegion (&region);
	GdipCreateRegionRect (&rect1, &other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Infinite Region + Rect.
	GdipCreateRegion (&region);
	GdipCombineRegionRect (region, &rect1, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	
	// Empty Region + Infinite Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (region);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Empty Region + Empty Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (region);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Empty Region + Rect Region.
	GdipCreateRegion (&region);
	GdipCreateRegionRect (&rect1, &other);
	GdipSetEmpty (region);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Empty Region + Rect.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	GdipCombineRegionRect (region, &rect1, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	
	// Rect Region + Infinite Region.
	GdipCreateRegionRect (&rect1, &region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Rect Region + Empty Region.
	GdipCreateRegionRect (&rect1, &region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Rect Region + Rect Region.
	GdipCreateRegionRect (&rect2, &region);
	GdipCreateRegionRect (&rect1, &other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Rect Region + Rect.
	GdipCreateRegionRect (&rect2, &region);
	GdipCombineRegionRect (region, &rect1, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	
	// Path Region + Infinite Region.
	GdipCreateRegionPath (path, &region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Path Region + Empty Region.
	GdipCreateRegionPath (path, &region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Path Region + Rect Region.
	GdipCreateRegionPath (path, &region);
	GdipCreateRegionRect (&rect1, &other);
	GdipCombineRegionRegion (region, other, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Path Region + Rect.
	GdipCreateRegionPath (path, &region);
	GdipCombineRegionRect (region, &rect1, CombineModeReplace);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);

	GdipDeletePath (path);
}

static void test_getRegionDataUnion ()
{
	GpRegion *region;
	GpRegion *other;
	GpPath *path;
	GpRectF rect1 = { 1, 2, 3, 4 };

	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	
	BYTE rectRegionData[] = {
		/* --RegionHeader-- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};

	// Infinite Region + Infinite Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Infinite Region + Empty Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Infinite Region + Rect Region.
	GdipCreateRegion (&region);
	GdipCreateRegionRect (&rect1, &other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Infinite Region + Rect.
	GdipCreateRegion (&region);
	GdipCombineRegionRect (region, &rect1, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	
	// Infinite Region + Path.
	GdipCreateRegion (&region);
	GdipCombineRegionPath (region, path, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	
	// Empty Region + Infinite Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (region);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Empty Region + Empty Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (region);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Empty Region + Rect Region.
	GdipCreateRegion (&region);
	GdipCreateRegionRect (&rect1, &other);
	GdipSetEmpty (region);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Empty Region + Rect.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	GdipCombineRegionRect (region, &rect1, CombineModeUnion);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	
	// Rect Region + Infinite Region.
	GdipCreateRegionRect (&rect1, &region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Rect Region + Empty Region.
	GdipCreateRegionRect (&rect1, &region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Path Region + Infinite Region.
	GdipCreateRegionPath (path, &region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeUnion);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	GdipDeletePath (path);
}

static void test_getRegionDataComplement ()
{
	GpRegion *region;
	GpRegion *other;
	GpPath *path;
	GpRectF rect1 = { 1, 2, 3, 4 };

	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	
	BYTE rectRegionData[] = {
		/* --RegionHeader-- */
		/* Size */          0x1C, 0x00, 0x00, 0x00,
		/* Checksum */      0xF7, 0x90, 0xBB, 0xEC,
		/* Magic */         0x02, 0x10, 0xC0, 0xDB,
		/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
		/* -- Entry -- */
		/* Type */   0x00, 0x00, 0x00, 0x10,
		/* X */      0x00, 0x00, 0x80, 0x3F,
		/* Y */      0x00, 0x00, 0x00, 0x40,
		/* Width */  0x00, 0x00, 0x40, 0x40,
		/* Height */ 0x00, 0x00, 0x80, 0x40
	};

	// Infinite Region + Infinite Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Infinite Region + Empty Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Infinite Region + Rect Region.
	GdipCreateRegion (&region);
	GdipCreateRegionRect (&rect1, &other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Infinite Region + Path Region.
	GdipCreateRegion (&region);
	GdipCreateRegionPath (path, &other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Infinite Region + Rect.
	GdipCreateRegion (&region);
	GdipCombineRegionRect (region, &rect1, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	
	// Infinite Region + Path.
	GdipCreateRegion (&region);
	GdipCombineRegionPath (region, path, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	
	// Empty Region + Infinite Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (region);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, infiniteRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Empty Region + Empty Region.
	GdipCreateRegion (&region);
	GdipCreateRegion (&other);
	GdipSetEmpty (region);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Empty Region + Rect Region.
	GdipCreateRegion (&region);
	GdipCreateRegionRect (&rect1, &other);
	GdipSetEmpty (region);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	// Empty Region + Rect.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);
	GdipCombineRegionRect (region, &rect1, CombineModeComplement);
	verifyRegionData (region, rectRegionData);
	GdipDeleteRegion (region);
	
	// Rect Region + Empty Region.
	GdipCreateRegionRect (&rect1, &region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);
	
	// Path Region + Empty Region.
	GdipCreateRegionPath (path, &region);
	GdipCreateRegion (&other);
	GdipSetEmpty (other);
	GdipCombineRegionRegion (region, other, CombineModeComplement);
	verifyRegionData (region, emptyRegionData);
	GdipDeleteRegion (region);
	GdipDeleteRegion (other);

	GdipDeletePath (path);
}

static void test_getRegionDataSize()
{
	GpStatus status;
	GpRegion *region;
	UINT bufferSize;

	GdipCreateRegion (&region);

	// Negative tests.
	status = GdipGetRegionDataSize (NULL, &bufferSize);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipGetRegionDataSize (region, NULL);
	assertEqualInt (status, InvalidParameter);
	
	GdipDeleteRegion (region);
}

static void test_cloneRegion ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpRegion *clone;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipCloneRegion (region, &clone);
	assertEqualInt (status, Ok);
	verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	status = GdipCloneRegion (region, &clone);
	assertEqualInt (status, Ok);
	verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
	GdipDeleteRegion (region);
	GdipDeletePath (path);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipCloneRegion (region, &clone);
	assertEqualInt (status, Ok);
	verifyRegion (clone, 0, 0, 0, 0, TRUE, FALSE);
	GdipDeleteRegion (region);
	GdipDeleteRegion (clone);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipCloneRegion (region, &clone);
	assertEqualInt (status, Ok);
	verifyRegion (clone, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	GdipDeleteRegion (region);
	GdipDeleteRegion (clone);

	// Negative tests.
	status = GdipCloneRegion (NULL, &clone);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneRegion (region, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_deleteRegion ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipDeleteRegion (region);
	assertEqualInt (status, Ok);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	status = GdipDeleteRegion (region);
	assertEqualInt (status, Ok);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipDeleteRegion (region);
	assertEqualInt (status, Ok);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipDeleteRegion (region);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeleteRegion (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_setInfinite ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipSetInfinite (region);
	assertEqualInt (status, Ok);
	verifyInfiniteRegion (region);
	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	status = GdipSetInfinite (region);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipSetInfinite (region);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipSetInfinite (region);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipSetInfinite (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_setEmpty ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipSetEmpty (region);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	status = GdipSetEmpty (region);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipSetEmpty (region);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipSetEmpty (region);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);
	assertEqualInt (status, Ok);
	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipSetEmpty (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_getRegionBounds ()
{
	GpStatus status;
	GpRegion *region;
	GpRectF rect;

	GdipCreateRegion (&region);

	// Negative tests.
	status = GdipGetRegionBounds (NULL, graphics, &rect);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionBounds (region, NULL, &rect);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetRegionBounds (region, graphics, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_isEmptyRegion ()
{
	GpStatus status;
	GpRegion *region;
	BOOL isEmpty;

	GdipCreateRegion (&region);

	// Negative tests.
	status = GdipIsEmptyRegion (NULL, graphics, &isEmpty);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsEmptyRegion (region, NULL, &isEmpty);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsEmptyRegion (region, graphics, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_isEqualRegion ()
{
	GpStatus status;
	GpRegion *infiniteRegion1;
	GpRegion *infiniteRegion2;
	GpRegion *emptyRegion1;
	GpRegion *emptyRegion2;
	GpRegion *infiniteRectRegion1;
	GpRegion *infiniteRectRegion2;
	GpRegion *emptyRectRegion1;
	GpRegion *emptyRectRegion2;
	GpRegion *zeroWidthRectRegion1;
	GpRegion *zeroWidthRectRegion2;
	GpRegion *zeroHeightRectRegion1;
	GpRegion *zeroHeightRectRegion2;
	GpRegion *negativeWidthRectRegion1;
	GpRegion *negativeWidthRectRegion2;
	GpRegion *negativeHeightRectRegion1;
	GpRegion *negativeHeightRectRegion2;
	GpRegion *rectRegion1;
	GpRegion *rectRegion2;
	GpRegion *rectRegion3;
	GpRegion *rectRegion4;
	GpRegion *rectRegion5;
	GpRegion *rectRegion6;
	GpRegion *multiRectRegion1;
	GpRegion *multiRectRegion2;
	GpRegion *multiRectRegion3;
	GpRegion *multiRectRegion4;
	GpRegion *multiRectRegion5;
	GpRegion *multiRectRegion6;
	GpRegion *emptyPathRegion1;
	GpRegion *emptyPathRegion2;
	GpRegion *infinitePathRegion1;
	GpRegion *infinitePathRegion2;
	GpRegion *pathRegion1;
	GpRegion *pathRegion2;
	GpRegion *pathRegion3;
	GpRegion *pathRegion4;
	GpRegion *pathRegion5;
	GpRegion *pathRegion6;
	BOOL isEqual;

	GdipCreateRegion (&infiniteRegion1);

	GdipCreateRegion (&infiniteRegion2);
	GdipSetInfinite (infiniteRegion2);

	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GdipCreateRegionRect (&infiniteRect, &infiniteRectRegion1);
	
	GdipCreateRegionRect (&infiniteRect, &infiniteRectRegion2);

	GdipCreateRegion (&emptyRegion1);
	GdipSetEmpty (emptyRegion1);

	GdipCreateRegion (&emptyRegion2);
	GdipSetEmpty (emptyRegion2);

	GpRectF emptyRect = {0, 0, 0, 0};
	GdipCreateRegionRect (&emptyRect, &emptyRectRegion1);
	
	GdipCreateRegionRect (&emptyRect, &emptyRectRegion2);
	
	GpRectF zeroWidthRect = {1, 2, 0, 4};
	GdipCreateRegionRect (&zeroWidthRect, &zeroWidthRectRegion1);

	GdipCreateRegionRect (&zeroWidthRect, &zeroWidthRectRegion2);
	
	GpRectF zeroHeightRect = {1, 2, 3, 0};
	GdipCreateRegionRect (&zeroHeightRect, &zeroHeightRectRegion1);
	
	GdipCreateRegionRect (&zeroHeightRect, &zeroHeightRectRegion2);
	
	GpRectF negativeWidthRect = {1, 2, -1, 4};
	GdipCreateRegionRect (&negativeWidthRect, &negativeWidthRectRegion1);
	
	GdipCreateRegionRect (&negativeWidthRect, &negativeWidthRectRegion2);
	
	GpRectF negativeHeightRect = {1, 2, 3, -1};
	GdipCreateRegionRect (&negativeHeightRect, &negativeHeightRectRegion1);
	
	GdipCreateRegionRect (&negativeHeightRect, &negativeHeightRectRegion2);

	GpRectF rect1 = {1, 2, 3, 4};
	GdipCreateRegionRect (&rect1, &rectRegion1);

	GpRectF rect2 = {1, 2, 3, 4};
	GdipCreateRegionRect (&rect2, &rectRegion2);

	GpRectF rect3 = {2, 2, 3, 4};
	GdipCreateRegionRect (&rect3, &rectRegion3);

	GpRectF rect4 = {1, 3, 3, 4};
	GdipCreateRegionRect (&rect4, &rectRegion4);

	GpRectF rect5 = {1, 2, 4, 4};
	GdipCreateRegionRect (&rect5, &rectRegion5);

	GpRectF rect6 = {1, 2, 3, 5};
	GdipCreateRegionRect (&rect6, &rectRegion6);

	GpRectF multiRect1_1 = {1, 2, 3, 4};
	GpRectF multiRect1_2 = {6, 7, 8, 9};
	GdipCreateRegionRect (&multiRect1_1, &multiRectRegion1);
	GdipCombineRegionRect (multiRectRegion1, &multiRect1_2, CombineModeUnion);
	
	GpRectF multiRect2_1 = {1, 2, 3, 4};
	GpRectF multiRect2_2 = {6, 7, 8, 9};
	GdipCreateRegionRect (&multiRect2_1, &multiRectRegion2);
	GdipCombineRegionRect (multiRectRegion2, &multiRect2_2, CombineModeUnion);
	
	GpRectF multiRect3_1 = {1, 2, 3, 4};
	GpRectF multiRect3_2 = {7, 7, 8, 9};
	GdipCreateRegionRect (&multiRect3_1, &multiRectRegion3);
	GdipCombineRegionRect (multiRectRegion3, &multiRect3_2, CombineModeUnion);
	
	GpRectF multiRect4_1 = {1, 2, 3, 4};
	GpRectF multiRect4_2 = {6, 8, 8, 9};
	GdipCreateRegionRect (&multiRect4_1, &multiRectRegion4);
	GdipCombineRegionRect (multiRectRegion4, &multiRect4_2, CombineModeUnion);
	
	GpRectF multiRect5_1 = {1, 2, 3, 4};
	GpRectF multiRect5_2 = {6, 7, 9, 9};
	GdipCreateRegionRect (&multiRect5_1, &multiRectRegion5);
	GdipCombineRegionRect (multiRectRegion5, &multiRect5_2, CombineModeUnion);
	
	GpRectF multiRect6_1 = {1, 2, 3, 4};
	GpRectF multiRect6_2 = {6, 7, 8, 10};
	GdipCreateRegionRect (&multiRect6_1, &multiRectRegion6);
	GdipCombineRegionRect (multiRectRegion6, &multiRect6_2, CombineModeUnion);
	
	GpPath *emptyPath;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreateRegionPath (emptyPath, &emptyPathRegion1);

	GdipCreateRegionPath (emptyPath, &emptyPathRegion2);

	GpPath *infinitePath;
	GdipCreatePath (FillModeAlternate, &infinitePath);
	GdipAddPathRectangle (infinitePath, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f);
	GdipCreateRegionPath (infinitePath, &infinitePathRegion1);

	GdipCreateRegionPath (infinitePath, &infinitePathRegion2);
	
	GpPath *path1;
	GdipCreatePath (FillModeAlternate, &path1);
	GdipAddPathRectangle (path1, 1, 2, 3, 4);
	GdipCreateRegionPath (path1, &pathRegion1);
	
	GpPath *path2;
	GdipCreatePath (FillModeAlternate, &path2);
	GdipAddPathRectangle (path2, 1, 2, 3, 4);
	GdipCreateRegionPath (path2, &pathRegion2);
	
	GpPath *path3;
	GdipCreatePath (FillModeAlternate, &path3);
	GdipAddPathRectangle (path3, 2, 2, 3, 4);
	GdipCreateRegionPath (path3, &pathRegion3);
	
	GpPath *path4;
	GdipCreatePath (FillModeAlternate, &path4);
	GdipAddPathRectangle (path4, 1, 3, 3, 4);
	GdipCreateRegionPath (path4, &pathRegion4);
	
	GpPath *path5;
	GdipCreatePath (FillModeAlternate, &path5);
	GdipAddPathRectangle (path5, 1, 2, 4, 4);
	GdipCreateRegionPath (path5, &pathRegion5);
	
	GpPath *path6;
	GdipCreatePath (FillModeAlternate, &path6);
	GdipAddPathRectangle (path6, 1, 2, 3, 5);
	GdipCreateRegionPath (path6, &pathRegion6);

	// Infinite region.
	status = GdipIsEqualRegion (infiniteRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (infiniteRegion1, infiniteRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (infiniteRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infiniteRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infiniteRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (infiniteRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Empty region.
	status = GdipIsEqualRegion (emptyRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (emptyRegion1, emptyRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (emptyRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (emptyRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (emptyRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Rectangular region - infinite.
	status = GdipIsEqualRegion (infiniteRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infiniteRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (infiniteRectRegion1, infiniteRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infiniteRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infiniteRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infiniteRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Rectangular region - empty.
	status = GdipIsEqualRegion (emptyRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (emptyRectRegion1, emptyRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (emptyRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (emptyRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (emptyRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	// Rectangular region - zero width.
	status = GdipIsEqualRegion (zeroWidthRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (zeroWidthRectRegion1, emptyRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, zeroWidthRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (zeroWidthRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (zeroWidthRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (zeroWidthRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (zeroWidthRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	// Rectangular region - zero height.
	status = GdipIsEqualRegion (zeroHeightRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (zeroHeightRectRegion1, emptyRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, zeroHeightRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (zeroHeightRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (zeroHeightRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (zeroHeightRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (zeroHeightRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Rectangular region - negative width.
	status = GdipIsEqualRegion (negativeWidthRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (negativeWidthRectRegion1, emptyRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, negativeWidthRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (negativeWidthRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (negativeWidthRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (negativeWidthRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (negativeWidthRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Rectangular region - negative height.
	status = GdipIsEqualRegion (negativeHeightRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (negativeHeightRectRegion1, emptyRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, negativeWidthRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (negativeHeightRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (negativeHeightRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (negativeHeightRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (negativeHeightRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Rectangular region - normal.
	status = GdipIsEqualRegion (rectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (rectRegion1, rectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (rectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (rectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (rectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, rectRegion3, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, rectRegion4, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, rectRegion5, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, rectRegion6, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (rectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, pathRegion3, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, pathRegion4, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, pathRegion5, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, pathRegion6, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (rectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (rectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Rectangular region - multiple.
	status = GdipIsEqualRegion (multiRectRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (multiRectRegion1, multiRectRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (multiRectRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, multiRectRegion3, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, multiRectRegion4, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, multiRectRegion5, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, multiRectRegion6, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (multiRectRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	// Path region - infinite.
	status = GdipIsEqualRegion (infinitePathRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infinitePathRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infinitePathRegion1, infinitePathRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infinitePathRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (infinitePathRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (infinitePathRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Path region - empty.
	status = GdipIsEqualRegion (emptyPathRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, emptyPathRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, zeroWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, zeroHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, negativeWidthRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, negativeHeightRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (emptyPathRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyPathRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyPathRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyPathRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (emptyPathRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (emptyPathRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Path region - normal.
	status = GdipIsEqualRegion (pathRegion1, rectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (pathRegion1, pathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);

	status = GdipIsEqualRegion (pathRegion1, pathRegion2, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, TRUE);
	
	status = GdipIsEqualRegion (pathRegion1, infiniteRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (pathRegion1, emptyRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (pathRegion1, infiniteRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (pathRegion1, emptyRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, rectRegion3, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, rectRegion4, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, rectRegion5, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, rectRegion6, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (pathRegion1, multiRectRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (pathRegion1, emptyPathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	status = GdipIsEqualRegion (pathRegion1, infinitePathRegion1, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, pathRegion3, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, pathRegion4, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, pathRegion5, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);
	
	status = GdipIsEqualRegion (pathRegion1, pathRegion6, graphics, &isEqual);
	assertEqualInt (status, Ok);
	assertEqualInt (isEqual, FALSE);

	// Negative tests.
	status = GdipIsEqualRegion (NULL, infiniteRegion2, graphics, &isEqual);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsEqualRegion (infiniteRegion1, NULL, graphics, &isEqual);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsEqualRegion (infiniteRegion1, infiniteRegion2, NULL, &isEqual);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsEqualRegion (infiniteRegion1, infiniteRegion2, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteRegion (infiniteRegion1);
	GdipDeleteRegion (infiniteRegion2);
	GdipDeleteRegion (infiniteRectRegion1);
	GdipDeleteRegion (infiniteRectRegion2);
	GdipDeleteRegion (emptyRegion1);
	GdipDeleteRegion (emptyRegion2);
	GdipDeleteRegion (emptyRectRegion1);
	GdipDeleteRegion (emptyRectRegion2);
	GdipDeleteRegion (rectRegion1);
	GdipDeleteRegion (rectRegion2);
	GdipDeleteRegion (rectRegion3);
	GdipDeleteRegion (rectRegion4);
	GdipDeleteRegion (rectRegion5);
	GdipDeleteRegion (rectRegion6);
	GdipDeleteRegion (emptyPathRegion1);
	GdipDeleteRegion (emptyPathRegion2);
	GdipDeleteRegion (infinitePathRegion1);
	GdipDeleteRegion (infinitePathRegion2);
	GdipDeleteRegion (pathRegion1);
	GdipDeleteRegion (pathRegion2);
	GdipDeleteRegion (pathRegion3);
	GdipDeleteRegion (pathRegion4);
	GdipDeleteRegion (pathRegion5);
	GdipDeleteRegion (pathRegion6);
}

static void test_isInfiniteRegion ()
{
	GpStatus status;
	GpRegion *region;
	BOOL isInfinite;

	GdipCreateRegion (&region);

	// Negative tests.
	status = GdipIsInfiniteRegion (NULL, graphics, &isInfinite);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsInfiniteRegion (region, NULL, &isInfinite);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsInfiniteRegion (region, graphics, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_isVisibleRegionPoint ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	BOOL isVisible;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);

	status = GdipIsVisibleRegionPoint (region, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 39, 59, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 39, 60, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 40, 49, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 9, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 10, 19, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 10, 20, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 10, 19, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);

	status = GdipIsVisibleRegionPoint (region, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 39, 59, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 39, 60, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 40, 49, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 9, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 10, 19, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 10, 20, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 10, 19, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipIsVisibleRegionPoint (region, 0, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 0, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipIsVisibleRegionPoint (region, 0, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, -4194304, -4194304, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 4194303, 4194303, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, -4194305, -4194304, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, -4194304, -4194305, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 4194303, 4194304, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 4194304, 4194303, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 8388608, 8388608, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPoint (region, 0, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPoint (region, 8388608, 8388608, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Negative tests.
	status = GdipIsVisibleRegionPoint (NULL, 0, 0, graphics, &isVisible);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsVisibleRegionPoint (region, 0, 0, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteRegion (region);
}

static void test_isVisibleRegionPointI ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	BOOL isVisible;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);

	status = GdipIsVisibleRegionPointI (region, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 39, 59, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 39, 60, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 40, 49, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 9, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 10, 19, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 10, 20, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 10, 19, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);

	status = GdipIsVisibleRegionPointI (region, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 39, 59, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 39, 60, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 40, 49, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 9, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 10, 19, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipIsVisibleRegionPointI (region, 0, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);
	
	status = GdipIsVisibleRegionPointI (region, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 0, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipIsVisibleRegionPointI (region, 0, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, -4194304, -4194304, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 4194303, 4194303, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, -4194305, -4194304, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, -4194304, -4194305, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 4194303, 4194304, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 4194304, 4194303, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 8388608, 8388608, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionPointI (region, 0, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionPointI (region, 8388608, 8388608, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Negative tests.
	status = GdipIsVisibleRegionPointI (NULL, 0, 0, graphics, &isVisible);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsVisibleRegionPointI (region, 0, 0, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteRegion (region);
}

static void test_isVisibleRegionRect ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	BOOL isVisible;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 30, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 39, 59, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 31, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 30, 41, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 39, 60, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 40, 49, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 9, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 19, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 0, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 30, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 39, 59, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 31, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 30, 41, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 39, 60, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 40, 49, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 9, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 19, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 0, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 1, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipIsVisibleRegionRect (region, 0, 0, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 10, 20, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 0, 0, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 8388608, 8388608, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 4194303, 4194303, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 8388609, 8388608, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 8388608, 8388609, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, -4194305, -4194305, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, 4194303, 4194304, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 4194304, 4194303, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 4194303, 4194304, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, 4194304, 4194303, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 0, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 1, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRect (region, -4194304, -4194304, 1, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Negative tests.
	status = GdipIsVisibleRegionRect (NULL, 0, 0, 0, 0, graphics, &isVisible);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsVisibleRegionRect (region, 0, 0, 0, 0, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteRegion (region);
}

static void test_isVisibleRegionRectI ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	BOOL isVisible;

	// Rect region.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 30, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 39, 59, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 31, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 30, 41, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 39, 60, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 40, 49, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 9, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 19, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 0, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Path region.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 30, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 39, 59, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 31, 40, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 30, 41, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 9, 19, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 39, 60, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 40, 49, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 9, 20, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 19, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 0, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 1, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipIsVisibleRegionRectI (region, 0, 0, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 10, 20, 10, 20, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 0, 0, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 8388608, 8388608, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 4194303, 4194303, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 8388609, 8388608, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 8388608, 8388609, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, -4194305, -4194305, 2, 2, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, 4194303, 4194304, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 4194304, 4194303, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 4194303, 4194304, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, 4194304, 4194303, 1, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 0, 1, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 1, 0, graphics, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 1, 1, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, TRUE);

	status = GdipIsVisibleRegionRectI (region, -4194304, -4194304, 1, 0, NULL, &isVisible);
	assertEqualInt (status, Ok);
	assertEqualInt (isVisible, FALSE);

	// Negative tests.
	status = GdipIsVisibleRegionRectI (NULL, 0, 0, 0, 0, graphics, &isVisible);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsVisibleRegionRectI (region, 0, 0, 0, 0, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteRegion (region);
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
	GpRectF scans[2];
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

	// Rect region < 0.5 - non null rects.
	GpRectF lessThanPointFiveRect = {10.2f, 20.2f, 30.2f, 40.2f};
	GdipCreateRegionRect (&lessThanPointFiveRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 11);
	assertEqualFloat (scans[0].Y, 21);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Rect region == 0.5 - non null rects.
	GpRectF pointFiveRect = {10.5f, 20.5f, 30.5f, 40.5f};
	GdipCreateRegionRect (&pointFiveRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 11);
	assertEqualFloat (scans[0].Y, 21);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Rect region > 0.5 - non null rects.
	GpRectF greaterThanPointFiveRect = {10.6f, 20.6f, 30.6f, 40.6f};
	GdipCreateRegionRect (&greaterThanPointFiveRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 11);
	assertEqualFloat (scans[0].Y, 21);
	assertEqualFloat (scans[0].Width, 31);
	assertEqualFloat (scans[0].Height, 41);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Rect region zero width - non null rects.
	GpRectF zeroWidthRect = {10, 20, 0, 40};
	status = GdipCreateRegionRect (&zeroWidthRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region zero height - non null rects.
	GpRectF zeroHeightRect = {10, 20, 30, 0};
	GdipCreateRegionRect (&zeroHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region zero width and height - non null rects.
	GpRectF zeroWidthAndHeightRect = {10, 20, 0, 0};
	GdipCreateRegionRect (&zeroWidthAndHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region negative width - non null rects.
	GpRectF negativeWidthRect = {10, 20, -30, 40};
	status = GdipCreateRegionRect (&negativeWidthRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region negative height - non null rects.
	GpRectF negativeHeightRect = {10, 20, 30, -40};
	status = GdipCreateRegionRect (&negativeHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region negative width and height - non null rects.
	GpRectF negativeWidthAndHeightRect = {10, 20, 30, -40};
	status = GdipCreateRegionRect (&negativeWidthAndHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);

	// Path region - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 10);
	assertEqualFloat (scans[0].Y, 20);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);

	// Path region - null rects.
	count = 0xFF;
	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeletePath (path);
	GdipDeleteRegion (region);
	
	// Path region < 0.5 - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10.2f, 20.2f, 30.2f, 40.2f);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (scans[0].X, 11);
	assertEqualFloat (scans[0].Y, 21);
#endif
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);
	
	GdipDeletePath (path);
	GdipDeleteRegion (region);
	
	// Path region == 0.5 - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10.5f, 20.5f, 30.5f, 40.5f);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect: https://github.com/mono/libgdiplus/issues/430
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (scans[0].X, 11);
	assertEqualFloat (scans[0].Y, WINDOWS_GDIPLUS ? 21 : 20);
	assertEqualFloat (scans[0].Width, 30);
	assertEqualFloat (scans[0].Height, 40);
	assertEqualInt (count, 1);
#endif
	
	GdipDeletePath (path);
	GdipDeleteRegion (region);
	
	// Path region > 0.5 - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10.6f, 20.6f, 30.6f, 40.6f);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualFloat (scans[0].X, 11);
	assertEqualFloat (scans[0].Y, 21);
	// FIXME: these are incorrect.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (scans[0].Width, 31);
	assertEqualFloat (scans[0].Height, 41);
#endif
	assertEqualInt (count, 1);
	
	GdipDeletePath (path);
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

static void test_getRegionScansI ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpMatrix *matrix;
	GpRect scans[2];
	INT count;

	GdipCreateMatrix (&matrix);

	// Rect region - non null rects.
	GpRectF rect = {10, 20, 30, 40};
	GdipCreateRegionRect (&rect, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scans[0].X, 10);
	assertEqualInt (scans[0].Y, 20);
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
	assertEqualInt (count, 1);

	// Rect region - null rects.
	count = 0xFF;
	status = GdipGetRegionScansI (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);

	// Rect region < 0.5 - non null rects.
	GpRectF lessThanPointFiveRect = {10.2f, 20.2f, 30.2f, 40.2f};
	GdipCreateRegionRect (&lessThanPointFiveRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scans[0].X, 11);
	assertEqualInt (scans[0].Y, 21);
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Rect region == 0.5 - non null rects.
	GpRectF pointFiveRect = {10.5f, 20.5f, 30.5f, 40.5f};
	GdipCreateRegionRect (&pointFiveRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scans[0].X, 11);
	assertEqualInt (scans[0].Y, 21);
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Rect region > 0.5 - non null rects.
	GpRectF greaterThanPointFiveRect = {10.6f, 20.6f, 30.6f, 40.6f};
	GdipCreateRegionRect (&greaterThanPointFiveRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scans[0].X, 11);
	assertEqualInt (scans[0].Y, 21);
	assertEqualInt (scans[0].Width, 31);
	assertEqualInt (scans[0].Height, 41);
	assertEqualInt (count, 1);
	GdipDeleteRegion (region);
	
	// Rect region zero width - non null rects.
	GpRectF zeroWidthRect = {10, 20, 0, 40};
	status = GdipCreateRegionRect (&zeroWidthRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region zero height - non null rects.
	GpRectF zeroHeightRect = {10, 20, 30, 0};
	GdipCreateRegionRect (&zeroHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region zero width and height - non null rects.
	GpRectF zeroWidthAndHeightRect = {10, 20, 0, 0};
	GdipCreateRegionRect (&zeroWidthAndHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region negative width - non null rects.
	GpRectF negativeWidthRect = {10, 20, -30, 40};
	status = GdipCreateRegionRect (&negativeWidthRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region negative height - non null rects.
	GpRectF negativeHeightRect = {10, 20, 30, -40};
	status = GdipCreateRegionRect (&negativeHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);
	
	// Rect region negative width and height - non null rects.
	GpRectF negativeWidthAndHeightRect = {10, 20, 30, -40};
	status = GdipCreateRegionRect (&negativeWidthAndHeightRect, &region);
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeleteRegion (region);

	// Path region - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (scans[0].X, 10);
	assertEqualInt (scans[0].Y, 20);
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
	assertEqualInt (count, 1);

	// Path region - null rects.
	count = 0xFF;
	status = GdipGetRegionScansI (region, NULL, &count, matrix);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);
	GdipDeletePath (path);
	GdipDeleteRegion (region);
	
	// Path region < 0.5 - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10.2f, 20.2f, 30.2f, 40.2f);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (scans[0].X, 11);
	assertEqualInt (scans[0].Y, 21);
#endif
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
	assertEqualInt (count, 1);
	
	GdipDeletePath (path);
	GdipDeleteRegion (region);
	
	// Path region == 0.5 - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10.5f, 20.5f, 30.5f, 40.5f);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect: https://github.com/mono/libgdiplus/issues/430
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (scans[0].X, 11);
	assertEqualInt (scans[0].Y, 21);
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
	assertEqualInt (count, 1);
#endif
	
	GdipDeletePath (path);
	GdipDeleteRegion (region);
	
	// Path region > 0.5 - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10.6f, 20.6f, 30.6f, 40.6f);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect: https://github.com/mono/libgdiplus/issues/430
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (scans[0].X, 11);
	assertEqualInt (scans[0].Y, 21);
	assertEqualInt (scans[0].Width, 31);
	assertEqualInt (scans[0].Height, 41);
	assertEqualInt (count, 1);
#endif
	
	GdipDeletePath (path);
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
	assertEqualInt (scans[0].X, -4194304);
	assertEqualInt (scans[0].Y, -4194304);
	assertEqualInt (scans[0].Width, 8388608);
	assertEqualInt (scans[0].Height, 8388608);
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

static void verifyCombineRegionWithRegionImpl (GpRegion *region, GpRegion *region2, CombineMode mode, float x, float y, float width, float height, BOOL isEmpty, BOOL isInfinite, RectF * scans, INT scansCount, const char *file, const char *function, int line)
{
	GpStatus status = GdipCombineRegionRegion (region, region2, mode);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);

	verifyRegionImpl (region, x, y, width, height, isEmpty, isInfinite, file, function, line);
	verifyRegionScansImpl (region, scans, scansCount, file, function, line);
}

#define verifyCombineRegionWithRegion(region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
	verifyCombineRegionWithRegionImpl (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount, __FILE__, __func__, __LINE__)

static void verifyCombineRegionWithRectImpl (GpRegion *region, RectF *rect, CombineMode mode, float x, float y, float width, float height, BOOL isEmpty, BOOL isInfinite, RectF *scans, INT scansCount, const char *file, const char *function, int line)
{
	GpRegion *clone;
	GdipCloneRegion (region, &clone);

	/* First, test combining with a rect region. */
	GpRegion *region2;
	GdipCreateRegionRect (rect, &region2);
	verifyCombineRegionWithRegionImpl (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount, file, function, line);
	GdipDeleteRegion (region2);

	/* Second, test combining with an actual rect. */
	GdipCombineRegionRect (clone, rect, mode);
	verifyRegionImpl (clone, x, y, width, height, isEmpty, isInfinite, file, function, line);
	verifyRegionScansImpl (clone, scans, scansCount, file, function, line);
	GdipDeleteRegion (clone);
}

#define verifyCombineRegionWithRect(region, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
	verifyCombineRegionWithRectImpl (region, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount, __FILE__, __func__, __LINE__)

static void verifyCombineRegionWithPathImpl (GpRegion *region, GpPath *path, CombineMode mode, float x, float y, float width, float height, BOOL isEmpty, BOOL isInfinite, RectF *scans, INT scansCount, const char *file, const char *function, int line)
{
	GpRegion *clone;
	GdipCloneRegion (region, &clone);

	/* First, test combining with a path region. */
	GpRegion *region2;
	GdipCreateRegionPath (path, &region2);
	verifyCombineRegionWithRegionImpl (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount, file, function, line);
	GdipDeleteRegion (region2);

	/* Second, test combining with an actual path. */
	GdipCombineRegionPath (clone, path, mode);
	verifyRegionImpl (clone, x, y, width, height, isEmpty, isInfinite, file, function, line);
	verifyRegionScansImpl (clone, scans, scansCount, file, function, line);
	GdipDeleteRegion (clone);
}

#define verifyCombineRegionWithPath(region, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
	verifyCombineRegionWithPathImpl (region, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount, __FILE__, __func__, __LINE__)

#define verifyCombineInfiniteWithRegion(region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegion (&region); \
 \
	verifyCombineRegionWithRegion (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineInfiniteWithRect(rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegion (&region); \
 \
	verifyCombineRegionWithRect (region, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineInfiniteWithPath(path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegion (&region); \
 \
	verifyCombineRegionWithPath (region, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineEmptyWithRegion(region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegion (&region); \
	GdipSetEmpty (region); \
 \
 	verifyCombineRegionWithRegion (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineEmptyWithRect(rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegion (&region); \
	GdipSetEmpty (region); \
 \
 	verifyCombineRegionWithRect (region, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineEmptyWithPath(path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegion (&region); \
	GdipSetEmpty (region); \
 \
 	verifyCombineRegionWithPath (region, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineRectWithRegion(rect, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionRect (rect, &region); \
 \
 	verifyCombineRegionWithRegion (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineRectWithRect(rect, rect2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionRect (rect, &region); \
 \
 	verifyCombineRegionWithRect (region, rect2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineRects(rects, rectsCount, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionRect (rects, &region); \
	int _c = rectsCount / sizeof (GpRectF); \
 \
	for (int _i = 1; _i < _c - 1; _i++) \
		GdipCombineRegionRect (region, &rects[_i], mode); \
	verifyCombineRegionWithRect (region, &rects[_c - 1], mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombineRectWithPath(rect, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionRect (rect, &region); \
 \
 	verifyCombineRegionWithPath (region, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombinePathWithRegion(path, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionPath (path, &region); \
 \
 	verifyCombineRegionWithRegion (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombinePathWithRect(path, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionPath (path, &region); \
 \
 	verifyCombineRegionWithRect (region, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

#define verifyCombinePathWithPath(path, path2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *region; \
	GdipCreateRegionPath (path, &region); \
 \
 	verifyCombineRegionWithPath (region, path2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region); \
} \

static GpPath *createPathFromRect(RectF *rect)
{
	GpPath *path;
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, rect->X, rect->Y, rect->Width, rect->Height);
	return path;
}

static void test_combineReplace ()
{
	GpRegion *infiniteRegion;
	GpRegion *emptyRegion;
	GpRegion *rectRegion;
	GpRegion *pathRegion;

	GdipCreateRegion (&infiniteRegion);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	RectF rect1 = {10, 20, 30, 40};
	RectF rect2 = {20, 30, 40, 50};
	GdipCreateRegionRect (&rect1, &rectRegion);

	GpPath *path1 = createPathFromRect (&rect1);
	GpPath *path2 = createPathFromRect (&rect2);
	GdipCreateRegionPath (path1, &pathRegion);

	// Infinite + Infinite = Infinite.
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeReplace, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Empty = Empty.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeReplace, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Rect = Rect.
	verifyCombineInfiniteWithRect (&rect1, CombineModeReplace, 10, 20, 30, 40, FALSE, FALSE, &rect1, sizeof (rect1));

	// Infinite + Path = Path.
	verifyCombineInfiniteWithPath (path1, CombineModeReplace, 10, 20, 30, 40, FALSE, FALSE, &rect1, sizeof (rect1));

	// Empty + Infinite = Infinite.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeReplace, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeReplace, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect1, CombineModeReplace, 10, 20, 30, 40, FALSE, FALSE, &rect1, sizeof (rect1));

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path1, CombineModeReplace, 10, 20, 30, 40, FALSE, FALSE, &rect1, sizeof (rect1));

	// Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&rect1, infiniteRegion, CombineModeReplace, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Rect + Empty = Empty.
	verifyCombineRectWithRegion (&rect1, emptyRegion, CombineModeReplace, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Other Rect = Other Rect.
	verifyCombineRectWithRect (&rect1, &rect2, CombineModeReplace, 20, 30, 40, 50, FALSE, FALSE, &rect2, sizeof (rect2));

	// Rect + Path = Path.
	verifyCombineRectWithPath (&rect1, path2, CombineModeReplace, 20, 30, 40, 50, FALSE, FALSE, &rect2, sizeof (rect2));

	// Path + Infinite = Infinite.
	verifyCombinePathWithRegion (path1, infiniteRegion, CombineModeReplace, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Path + Empty = Empty.
	verifyCombinePathWithRegion (path1, emptyRegion, CombineModeReplace, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Rect = Rect.
	verifyCombinePathWithRect (path1, &rect2, CombineModeReplace, 20, 30, 40, 50, FALSE, FALSE, &rect2, sizeof (rect2));

	// Path + Other Path = Other Path.
	verifyCombinePathWithPath (path1, path2, CombineModeReplace, 20, 30, 40, 50, FALSE, FALSE, &rect2, sizeof (rect2));

	GdipDeleteRegion (infiniteRegion);
	GdipDeleteRegion (emptyRegion);
	GdipDeleteRegion (rectRegion);
	GdipDeleteRegion (pathRegion);
	GdipDeletePath (path1);
	GdipDeletePath (path2);
}

static void test_combineIntersect ()
{
	GpRegion *infiniteRegion;
	GpRegion *emptyRegion;
	GpRegion *rectRegion;
	GpRegion *pathRegion;

	GdipCreateRegion (&infiniteRegion);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	RectF rect = {10, 20, 30, 40};
	RectF superRect = {0, 10, 50, 60};
	RectF subRect = {15, 25, 20, 30};
	RectF intersectLeftRect = {0, 20, 30, 40};
	RectF intersectTopRect = {10, 10, 30, 40};
	RectF intersectRightRect = {20, 20, 30, 40};
	RectF intersectBottomRect = {10, 30, 30, 40};
	RectF intersectTopLeftRect = {0, 10, 30, 40};
	RectF intersectTopRightRect = {20, 10, 30, 40};
	RectF intersectBottomRightRect = {20, 30, 30, 40};
	RectF intersectBottomLeftRect = {0, 30, 30, 40};
	RectF touchingLeftRect = {-20, 20, 30, 40};
	RectF touchingTopRect = {10, -20, 30, 40};
	RectF touchingRightRect = {40, 20, 30, 40};
	RectF touchingBottomRect = {10, 60, 30, 40};
	RectF touchingTopLeftRect = {-20, -20, 30, 40};
	RectF touchingTopRightRect = {40, -20, 30, 40};
	RectF touchingBottomRightRect = {40, 60, 30, 40};
	RectF touchingBottomLeftRect = {-20, 60, 30, 40};
	RectF noIntersectLeftRect = {-21, 20, 30, 40};
	RectF noIntersectTopRect = {10, -21, 30, 40};
	RectF noIntersectRightRect = {41, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 61, 30, 40};
	RectF noIntersectTopLeftRect = {-21, -21, 30, 40};
	RectF noIntersectTopRightRect = {41, -21, 30, 40};
	RectF noIntersectBottomRightRect = {41, 61, 30, 40};
	RectF noIntersectBottomLeftRect = {-21, 61, 30, 40};
	RectF emptyRect = {0, 0, 0, 0};
	RectF infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	RectF negativeRect = {20, 30, -10, -10};
	GdipCreateRegionRect (&rect, &rectRegion);

	GpPath *path = createPathFromRect (&rect);
	GpPath *superPath = createPathFromRect (&superRect);
	GpPath *subPath = createPathFromRect (&subRect);
	GpPath *intersectLeftPath = createPathFromRect (&intersectLeftRect);
	GpPath *intersectTopPath = createPathFromRect (&intersectTopRect);
	GpPath *intersectRightPath = createPathFromRect (&intersectRightRect);
	GpPath *intersectBottomPath = createPathFromRect (&intersectBottomRect);
	GpPath *intersectTopLeftPath = createPathFromRect (&intersectTopLeftRect);
	GpPath *intersectTopRightPath = createPathFromRect (&intersectTopRightRect);
	GpPath *intersectBottomRightPath = createPathFromRect (&intersectBottomRightRect);
	GpPath *intersectBottomLeftPath = createPathFromRect (&intersectBottomLeftRect);
	GpPath *touchingLeftPath = createPathFromRect (&touchingLeftRect);
	GpPath *touchingTopPath = createPathFromRect (&touchingTopRect);
	GpPath *touchingRightPath = createPathFromRect (&touchingRightRect);
	GpPath *touchingBottomPath = createPathFromRect (&touchingBottomRect);
	GpPath *touchingTopLeftPath = createPathFromRect (&touchingTopLeftRect);
	GpPath *touchingTopRightPath = createPathFromRect (&touchingTopRightRect);
	GpPath *touchingBottomRightPath = createPathFromRect (&touchingBottomRightRect);
	GpPath *touchingBottomLeftPath = createPathFromRect (&touchingBottomLeftRect);
	GpPath *noIntersectLeftPath = createPathFromRect (&noIntersectLeftRect);
	GpPath *noIntersectTopPath = createPathFromRect (&noIntersectTopRect);
	GpPath *noIntersectRightPath = createPathFromRect (&noIntersectRightRect);
	GpPath *noIntersectBottomPath = createPathFromRect (&noIntersectBottomRect);
	GpPath *noIntersectTopLeftPath = createPathFromRect (&noIntersectTopLeftRect);
	GpPath *noIntersectTopRightPath = createPathFromRect (&noIntersectTopRightRect);
	GpPath *noIntersectBottomRightPath = createPathFromRect (&noIntersectBottomRightRect);
	GpPath *noIntersectBottomLeftPath = createPathFromRect (&noIntersectBottomLeftRect);
	GpPath *emptyPath = createPathFromRect (&emptyRect);
	GpPath *infinitePath = createPathFromRect (&infiniteRect);
	GpPath *negativePath = createPathFromRect (&negativeRect);

	GdipCreateRegionPath (path, &pathRegion);

	// Infinite + Infinite = Infinite.
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Empty = Empty.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
	
	// Infinite + Infinite Rect = Infinite.
	verifyCombineInfiniteWithRect (&infiniteRect, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Empty Rect = Empty.
	verifyCombineInfiniteWithRect (&emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Rect = Rect.
	verifyCombineInfiniteWithRect (&rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
	
	// Infinite + Negative Rect = Rect.
	{
		GpRegion *region;
		GdipCreateRegion (&region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeIntersect, 20, 30, -10, -10, TRUE, FALSE, emptyScans, 0);
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF negativeRectScan = {10, 20, 10, 10};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeIntersect);
		verifyRegion (clone, 10, 20, 10, 10, FALSE, FALSE);
		verifyRegionScans (clone, &negativeRectScan, sizeof (negativeRectScan));
		GdipDeleteRegion (clone);
	}

	// Infinite + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/411
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithPath (infinitePath, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Infinite + Empty Path = Empty.
	verifyCombineInfiniteWithPath (emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Path = Path.
	verifyCombineInfiniteWithPath (path, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty + Infinite = Empty.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite Rect = Empty.
	verifyCombineEmptyWithRect (&infiniteRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty Rect = Empty.
	verifyCombineEmptyWithRect (&emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
	
	// Empty + Infinite Rect = Empty.
	verifyCombineEmptyWithPath (infinitePath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty Rect = Empty.
	verifyCombineEmptyWithPath (emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&infiniteRect, infiniteRegion, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Empty = Empty.
	verifyCombineRectWithRegion (&infiniteRect, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Infinite Rect = Infinite.
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Rect = Rect.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
	
	// Infinite Rect + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/411
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&infiniteRect, infinitePath, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Infinite Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&infiniteRect, emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Path = Path.
	verifyCombineRectWithPath (&infiniteRect, path, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Rect + Infinite = Empty.
	verifyCombineRectWithRegion (&emptyRect, infiniteRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Empty = Empty.
	verifyCombineRectWithRegion (&emptyRect, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Path = Empty.
	verifyCombineRectWithPath (&emptyRect, infinitePath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&emptyRect, emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Path = Empty.
	verifyCombineRectWithPath (&emptyRect, path, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Infinite = Rect.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Empty = Empty.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Infinite Rect = Rect.
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Negative Rect = Empty.
	{
		GpRegion *region;
		GdipCreateRegionRect (&rect, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF negativeRectScan = {10, 20, 10, 10};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeIntersect);
		verifyRegion (clone, 10, 20, 10, 10, FALSE, FALSE);
		verifyRegionScans (clone, &negativeRectScan, sizeof (negativeRectScan));
		GdipDeleteRegion (clone);
	}

	// Rect + Equal Rect = Equal Rect.
	verifyCombineRectWithRect (&rect, &rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Super Rect = Rect.
	verifyCombineRectWithRect (&rect, &superRect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Sub Rect = Sub Rect.
	verifyCombineRectWithRect (&rect, &subRect, CombineModeIntersect, 15, 25, 20, 30, FALSE, FALSE, &subRect, sizeof (subRect));

	// Rect + Intersect Left = Calculation.
	RectF intersectLeftScan = {10, 20, 20, 40};
	verifyCombineRectWithRect (&rect, &intersectLeftRect, CombineModeIntersect, 10, 20, 20, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Calculation.
	RectF intersectTopScan = {10, 20, 30, 30};
	verifyCombineRectWithRect (&rect, &intersectTopRect, CombineModeIntersect, 10, 20, 30, 30, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Calculation.
	RectF intersectRightScan = {20, 20, 20, 40};
	verifyCombineRectWithRect (&rect, &intersectRightRect, CombineModeIntersect, 20, 20, 20, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Calculation.
	RectF intersectBottomScan = {10, 30, 30, 30};
	verifyCombineRectWithRect (&rect, &intersectBottomRect, CombineModeIntersect, 10, 30, 30, 30, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Calculation.
	RectF intersectTopLeftScan = {10, 20, 20, 30};
	verifyCombineRectWithRect (&rect, &intersectTopLeftRect, CombineModeIntersect, 10, 20, 20, 30, FALSE, FALSE, &intersectTopLeftScan, sizeof (intersectTopLeftScan));

	// Rect + Intersect Top Right = Calculation.
	RectF intersectTopRightScan = {20, 20, 20, 30};
	verifyCombineRectWithRect (&rect, &intersectTopRightRect, CombineModeIntersect, 20, 20, 20, 30, FALSE, FALSE, &intersectTopRightScan, sizeof (intersectTopRightScan));

	// Rect + Intersect Bottom Right = Calculation.
	RectF intersectBottomRightScan = {20, 30, 20, 30};
	verifyCombineRectWithRect (&rect, &intersectBottomRightRect, CombineModeIntersect, 20, 30, 20, 30, FALSE, FALSE, &intersectBottomRightScan, sizeof (intersectBottomRightScan));

	// Rect + Intersect Bottom Left = Calculation.
	RectF intersectBottomLeftScan = {10, 30, 20, 30};
	verifyCombineRectWithRect (&rect, &intersectBottomLeftRect, CombineModeIntersect, 10, 30, 20, 30, FALSE, FALSE, &intersectBottomLeftScan, sizeof (intersectBottomLeftScan));

	// Rect + Touching Left = Empty.
	verifyCombineRectWithRect (&rect, &touchingLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Top = Empty.
	verifyCombineRectWithRect (&rect, &touchingTopRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Right = Empty.
	verifyCombineRectWithRect (&rect, &touchingRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Bottom = Empty.
	verifyCombineRectWithRect (&rect, &touchingBottomRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Top Left = Empty.
	verifyCombineRectWithRect (&rect, &touchingTopLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Top Right = Empty.
	verifyCombineRectWithRect (&rect, &touchingTopRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Bottom Right = Empty.
	verifyCombineRectWithRect (&rect, &touchingBottomRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Bottom Left = Empty.
	verifyCombineRectWithRect (&rect, &touchingBottomLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Left = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Top = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Right = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Top Left = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Top Right = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom Right = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom Left = Empty.
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Infinite Path = Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Negative Path = Empty.
	verifyCombineRectWithPath (&rect, negativePath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Equal Path = Equal Path.
	verifyCombineRectWithPath (&rect, path, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Super Path = Path.
	verifyCombineRectWithPath (&rect, superPath, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Sub Path = Sub Path.
	verifyCombineRectWithPath (&rect, subPath, CombineModeIntersect, 15, 25, 20, 30, FALSE, FALSE, &subRect, sizeof (subRect));

	// Rect + Intersect Left = Calculation.
	verifyCombineRectWithPath (&rect, intersectLeftPath, CombineModeIntersect, 10, 20, 20, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Calculation.
	verifyCombineRectWithPath (&rect, intersectTopPath, CombineModeIntersect, 10, 20, 30, 30, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Calculation.
	verifyCombineRectWithPath (&rect, intersectRightPath, CombineModeIntersect, 20, 20, 20, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Calculation.
	verifyCombineRectWithPath (&rect, intersectBottomPath, CombineModeIntersect, 10, 30, 30, 30, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Calculation.
	verifyCombineRectWithPath (&rect, intersectTopLeftPath, CombineModeIntersect, 10, 20, 20, 30, FALSE, FALSE, &intersectTopLeftScan, sizeof (intersectTopLeftScan));

	// Rect + Intersect Top Right = Calculation.
	verifyCombineRectWithPath (&rect, intersectTopRightPath, CombineModeIntersect, 20, 20, 20, 30, FALSE, FALSE, &intersectTopRightScan, sizeof (intersectTopRightScan));

	// Rect + Intersect Bottom Right = Calculation.
	verifyCombineRectWithPath (&rect, intersectBottomRightPath, CombineModeIntersect, 20, 30, 20, 30, FALSE, FALSE, &intersectBottomRightScan, sizeof (intersectBottomRightScan));

	// Rect + Intersect Bottom Left = Calculation.
	verifyCombineRectWithPath (&rect, intersectBottomLeftPath, CombineModeIntersect, 10, 30, 20, 30, FALSE, FALSE, &intersectBottomLeftScan, sizeof (intersectBottomLeftScan));

	// Rect + Touching Left = Empty.
	verifyCombineRectWithPath (&rect, touchingLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Top = Empty.
	verifyCombineRectWithPath (&rect, touchingTopPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Right = Empty.
	verifyCombineRectWithPath (&rect, touchingRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Bottom = Empty.
	verifyCombineRectWithPath (&rect, touchingBottomPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Top Left = Empty.
	verifyCombineRectWithPath (&rect, touchingTopLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Top Right = Empty.
	verifyCombineRectWithPath (&rect, touchingTopRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Bottom Right = Empty.
	verifyCombineRectWithPath (&rect, touchingBottomRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Touching Bottom Left = Empty.
	verifyCombineRectWithPath (&rect, touchingBottomLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Left = Empty.
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Top = Empty.
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Right = Empty.
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom = Empty.
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Top Left = Empty.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Top Right = Empty.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom Right = Empty.
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom Left = Empty.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
	
	// Infinite Path + Infinite = Infinite.
	verifyCombinePathWithRegion (infinitePath, infiniteRegion, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Empty = Empty.
	verifyCombinePathWithRegion (infinitePath, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Infinite Rect = Infinite.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Empty Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Rect = Rect.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
	
	// Infinite Path + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/411
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (infinitePath, infinitePath, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Infinite Path + Empty Path = Empty.
	verifyCombinePathWithPath (infinitePath, emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Path = Path.
	verifyCombinePathWithPath (infinitePath, path, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Path + Infinite = Empty.
	verifyCombinePathWithRegion (emptyPath, infiniteRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Empty = Empty.
	verifyCombinePathWithRegion (emptyPath, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Empty Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Path = Empty.
	verifyCombinePathWithPath (emptyPath, infinitePath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Empty Path = Empty.
	verifyCombinePathWithPath (emptyPath, emptyPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Path = Empty.
	verifyCombinePathWithPath (emptyPath, path, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Infinite = Path.
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Empty = Empty.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Infinite Rect = Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Path + Empty Rect = Empty.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Negative Rect = Empty.
	{
		GpRegion *region;
		GdipCreateRegionPath (path, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF negativeRectScan = {10, 20, 10, 10};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeIntersect);
		verifyRegion (clone, 10, 20, 10, 10, FALSE, FALSE);
		verifyRegionScans (clone, &negativeRectScan, sizeof (negativeRectScan));
		GdipDeleteRegion (clone);
	}

	// Path + Equal Rect = Equal Rect.
	verifyCombinePathWithRect (path, &rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Super Rect = Rect.
	verifyCombinePathWithRect (path, &superRect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Sub Rect = Sub Rect.
	verifyCombinePathWithRect (path, &subRect, CombineModeIntersect, 15, 25, 20, 30, FALSE, FALSE, &subRect, sizeof (subRect));

	// Path + Intersect Left = Calculation.
	verifyCombinePathWithRect (path, &intersectLeftRect, CombineModeIntersect, 10, 20, 20, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Calculation.
	verifyCombinePathWithRect (path, &intersectTopRect, CombineModeIntersect, 10, 20, 30, 30, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Calculation.
	verifyCombinePathWithRect (path, &intersectRightRect, CombineModeIntersect, 20, 20, 20, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomRect, CombineModeIntersect, 10, 30, 30, 30, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Calculation.
	verifyCombinePathWithRect (path, &intersectTopLeftRect, CombineModeIntersect, 10, 20, 20, 30, FALSE, FALSE, &intersectTopLeftScan, sizeof (intersectTopLeftScan));

	// Path + Intersect Top Right = Calculation.
	verifyCombinePathWithRect (path, &intersectTopRightRect, CombineModeIntersect, 20, 20, 20, 30, FALSE, FALSE, &intersectTopRightScan, sizeof (intersectTopRightScan));

	// Path + Intersect Bottom Right = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomRightRect, CombineModeIntersect, 20, 30, 20, 30, FALSE, FALSE, &intersectBottomRightScan, sizeof (intersectBottomRightScan));

	// Path + Intersect Bottom Left = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomLeftRect, CombineModeIntersect, 10, 30, 20, 30, FALSE, FALSE, &intersectBottomLeftScan, sizeof (intersectBottomLeftScan));

	// Path + Touching Left = Empty.
	verifyCombinePathWithRect (path, &touchingLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Top = Empty.
	verifyCombinePathWithRect (path, &touchingTopRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Right = Empty.
	verifyCombinePathWithRect (path, &touchingRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Bottom = Empty.
	verifyCombinePathWithRect (path, &touchingBottomRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Top Left = Empty.
	verifyCombinePathWithRect (path, &touchingTopLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Top Right = Empty.
	verifyCombinePathWithRect (path, &touchingTopRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Bottom Right = Empty.
	verifyCombinePathWithRect (path, &touchingBottomRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Bottom Left = Empty.
	verifyCombinePathWithRect (path, &touchingBottomLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Left = Empty.
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Top = Empty.
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Right = Empty.
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom = Empty.
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Top Left = Empty.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Top Right = Empty.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Right = Empty.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Left = Empty.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Equal Path = Equal Path.
	verifyCombinePathWithPath (path, path, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Super Path = Path.
	verifyCombinePathWithPath (path, superPath, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Sub Path = Sub Path.
	verifyCombinePathWithPath (path, subPath, CombineModeIntersect, 15, 25, 20, 30, FALSE, FALSE, &subRect, sizeof (subRect));

	// Path + Intersect Left = Calculation.
	verifyCombinePathWithPath (path, intersectLeftPath, CombineModeIntersect, 10, 20, 20, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Calculation.
	verifyCombinePathWithPath (path, intersectTopPath, CombineModeIntersect, 10, 20, 30, 30, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Calculation.
	verifyCombinePathWithPath (path, intersectRightPath, CombineModeIntersect, 20, 20, 20, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Calculation.
	verifyCombinePathWithPath (path, intersectBottomPath, CombineModeIntersect, 10, 30, 30, 30, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Calculation.
	verifyCombinePathWithPath (path, intersectTopLeftPath, CombineModeIntersect, 10, 20, 20, 30, FALSE, FALSE, &intersectTopLeftScan, sizeof (intersectTopLeftScan));

	// Path + Intersect Top Right = Calculation.
	verifyCombinePathWithPath (path, intersectTopRightPath, CombineModeIntersect, 20, 20, 20, 30, FALSE, FALSE, &intersectTopRightScan, sizeof (intersectTopRightScan));

	// Path + Intersect Bottom Right = Calculation.
	verifyCombinePathWithPath (path, intersectBottomRightPath, CombineModeIntersect, 20, 30, 20, 30, FALSE, FALSE, &intersectBottomRightScan, sizeof (intersectBottomRightScan));

	// Path + Intersect Bottom Left = Calculation.
	verifyCombinePathWithPath (path, intersectBottomLeftPath, CombineModeIntersect, 10, 30, 20, 30, FALSE, FALSE, &intersectBottomLeftScan, sizeof (intersectBottomLeftScan));
	
	// Path + Touching Left = Empty.
	verifyCombinePathWithPath (path, touchingLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Top = Empty.
	verifyCombinePathWithPath (path, touchingTopPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Right = Empty.
	verifyCombinePathWithPath (path, touchingRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Bottom = Empty.
	verifyCombinePathWithPath (path, touchingBottomPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Top Left = Empty.
	verifyCombinePathWithPath (path, touchingTopLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Top Right = Empty.
	verifyCombinePathWithPath (path, touchingTopRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Bottom Right = Empty.
	verifyCombinePathWithPath (path, touchingBottomRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Touching Bottom Left = Empty.
	verifyCombinePathWithPath (path, touchingBottomLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Left = Empty.
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Top = Empty.
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Right = Empty.
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom = Empty.
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Top Left = Empty.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Top Right = Empty.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Right = Empty.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Left = Empty.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	GdipDeleteRegion (infiniteRegion);
	GdipDeleteRegion (emptyRegion);
	GdipDeleteRegion (rectRegion);
	GdipDeleteRegion (pathRegion);
	GdipDeletePath (path);
	GdipDeletePath (superPath);
	GdipDeletePath (subPath);
	GdipDeletePath (intersectLeftPath);
	GdipDeletePath (intersectTopPath);
	GdipDeletePath (intersectRightPath);
	GdipDeletePath (intersectBottomPath);
	GdipDeletePath (intersectTopLeftPath);
	GdipDeletePath (intersectTopRightPath);
	GdipDeletePath (intersectBottomRightPath);
	GdipDeletePath (intersectBottomLeftPath);
	GdipDeletePath (touchingLeftPath);
	GdipDeletePath (touchingTopPath);
	GdipDeletePath (touchingRightPath);
	GdipDeletePath (touchingBottomPath);
	GdipDeletePath (touchingTopLeftPath);
	GdipDeletePath (touchingTopRightPath);
	GdipDeletePath (touchingBottomRightPath);
	GdipDeletePath (touchingBottomLeftPath);
	GdipDeletePath (noIntersectLeftPath);
	GdipDeletePath (noIntersectTopPath);
	GdipDeletePath (noIntersectRightPath);
	GdipDeletePath (noIntersectBottomPath);
	GdipDeletePath (noIntersectTopLeftPath);
	GdipDeletePath (noIntersectTopRightPath);
	GdipDeletePath (noIntersectBottomRightPath);
	GdipDeletePath (noIntersectBottomLeftPath);
	GdipDeletePath (emptyPath);
	GdipDeletePath (infinitePath);
	GdipDeletePath (negativePath);
}

static void test_combineUnion ()
{
	GpRegion *infiniteRegion;
	GpRegion *emptyRegion;
	GpRegion *rectRegion;
	GpRegion *pathRegion;

	GdipCreateRegion (&infiniteRegion);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	RectF rect = {10, 20, 30, 40};
	RectF superRect = {0, 10, 50, 60};
	RectF subRect = {15, 25, 20, 30};
	RectF intersectLeftRect = {0, 20, 30, 40};
	RectF intersectTopRect = {10, 10, 30, 40};
	RectF intersectRightRect = {20, 20, 30, 40};
	RectF intersectBottomRect = {10, 30, 30, 40};
	RectF intersectTopLeftRect = {0, 10, 30, 40};
	RectF intersectTopRightRect = {20, 10, 30, 40};
	RectF intersectBottomRightRect = {20, 30, 30, 40};
	RectF intersectBottomLeftRect = {0, 30, 30, 40};
	RectF touchingLeftRect = {-20, 20, 30, 40};
	RectF touchingTopRect = {10, -20, 30, 40};
	RectF touchingRightRect = {40, 20, 30, 40};
	RectF touchingBottomRect = {10, 60, 30, 40};
	RectF touchingTopLeftRect = {-20, -20, 30, 40};
	RectF touchingTopRightRect = {40, -20, 30, 40};
	RectF touchingBottomRightRect = {40, 60, 30, 40};
	RectF touchingBottomLeftRect = {-20, 60, 30, 40};
	RectF noIntersectLeftRect = {-21, 20, 30, 40};
	RectF noIntersectTopRect = {10, -21, 30, 40};
	RectF noIntersectRightRect = {41, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 61, 30, 40};
	RectF noIntersectTopLeftRect = {-21, -21, 30, 40};
	RectF noIntersectTopRightRect = {41, -21, 30, 40};
	RectF noIntersectBottomRightRect = {41, 61, 30, 40};
	RectF noIntersectBottomLeftRect = {-21, 61, 30, 40};
	RectF intersectLeftNarrowRect = {-30, 30, 60, 10};
	RectF intersectTopNarrowRect = {20, -20, 10, 60};
	RectF intersectRightNarrowRect = {20, 30, 60, 10};
	RectF intersectBottomNarrowRect = {20, 40, 10, 60};
	RectF touchesTopIntersectBottomNarrowRect = {20, 20, 10, 60};
	RectF intersectRightTallerRect = {20, 20, 30, 60};
	RectF intersectRightShorterRect = {20, 20, 30, 20};
	RectF tallerRect = {10, 20, 30, 70};
	RectF tallerNarrowRect = {10, 20, 10, 70};
	RectF widerRect = {10, 20, 60, 40};
	RectF widerShorterRect = {10, 20, 60, 20};
	RectF largerRect = {10, 20, 60, 70};
	RectF crossingRect = {0, 30, 50, 20};
	RectF emptyRect = {0, 0, 0, 0};
	RectF infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	RectF negativeRect = {20, 30, -10, -10};
	GdipCreateRegionRect (&rect, &rectRegion);

	GpPath *path = createPathFromRect (&rect);
	GpPath *superPath = createPathFromRect (&superRect);
	GpPath *subPath = createPathFromRect (&subRect);
	GpPath *intersectLeftPath = createPathFromRect (&intersectLeftRect);
	GpPath *intersectTopPath = createPathFromRect (&intersectTopRect);
	GpPath *intersectRightPath = createPathFromRect (&intersectRightRect);
	GpPath *intersectBottomPath = createPathFromRect (&intersectBottomRect);
	GpPath *intersectTopLeftPath = createPathFromRect (&intersectTopLeftRect);
	GpPath *intersectTopRightPath = createPathFromRect (&intersectTopRightRect);
	GpPath *intersectBottomRightPath = createPathFromRect (&intersectBottomRightRect);
	GpPath *intersectBottomLeftPath = createPathFromRect (&intersectBottomLeftRect);
	GpPath *touchingLeftPath = createPathFromRect (&touchingLeftRect);
	GpPath *touchingTopPath = createPathFromRect (&touchingTopRect);
	GpPath *touchingRightPath = createPathFromRect (&touchingRightRect);
	GpPath *touchingBottomPath = createPathFromRect (&touchingBottomRect);
	GpPath *touchingTopLeftPath = createPathFromRect (&touchingTopLeftRect);
	GpPath *touchingTopRightPath = createPathFromRect (&touchingTopRightRect);
	GpPath *touchingBottomRightPath = createPathFromRect (&touchingBottomRightRect);
	GpPath *touchingBottomLeftPath = createPathFromRect (&touchingBottomLeftRect);
	GpPath *noIntersectLeftPath = createPathFromRect (&noIntersectLeftRect);
	GpPath *noIntersectTopPath = createPathFromRect (&noIntersectTopRect);
	GpPath *noIntersectRightPath = createPathFromRect (&noIntersectRightRect);
	GpPath *noIntersectBottomPath = createPathFromRect (&noIntersectBottomRect);
	GpPath *noIntersectTopLeftPath = createPathFromRect (&noIntersectTopLeftRect);
	GpPath *noIntersectTopRightPath = createPathFromRect (&noIntersectTopRightRect);
	GpPath *noIntersectBottomRightPath = createPathFromRect (&noIntersectBottomRightRect);
	GpPath *noIntersectBottomLeftPath = createPathFromRect (&noIntersectBottomLeftRect);
	GpPath *emptyPath = createPathFromRect (&emptyRect);
	GpPath *infinitePath = createPathFromRect (&infiniteRect);
	GpPath *negativePath = createPathFromRect (&negativeRect);

	GdipCreateRegionPath (path, &pathRegion);

	// Infinite + Infinite = Infinite
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Empty = Infinite.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Infinite Rect = Infinite.
	verifyCombineInfiniteWithRect (&infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Empty Rect = Infinite.
	verifyCombineInfiniteWithRect (&emptyRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Rect = Infinite.
	verifyCombineInfiniteWithRect (&rect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
	
	// Infinite + Negative Rect = Infinite.
	verifyCombineInfiniteWithRect (&negativeRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Infinite Path = Infinite.
	verifyCombineInfiniteWithPath (infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Empty Path = Infinite.
	verifyCombineInfiniteWithPath (emptyPath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Path = Infinite.
	verifyCombineInfiniteWithPath (path, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Infinite = Infinite.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite Rect = Infinite.
	verifyCombineEmptyWithRect (&infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty Rect = Empty.
	verifyCombineEmptyWithRect (&emptyRect, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty + Infinite Path = Infinite.
	// FIXME: this should have scans: https://github.com/mono/libgdiplus/issues/412
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineEmptyWithPath (infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty + Empty Path = Empty.
	verifyCombineEmptyWithPath (emptyPath, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
	
	// Infinite Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&infiniteRect, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Empty = Infinite.
	verifyCombineRectWithRegion (&infiniteRect, emptyRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Infinite Rect = Infinite.
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Empty Rect = Infinite.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Rect = Infinite.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Infinite Path = Infinite.
	verifyCombineRectWithPath (&infiniteRect, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Empty Path = Infinite.
	verifyCombineRectWithPath (&infiniteRect, emptyPath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Path = Infinite.
	verifyCombineRectWithPath (&infiniteRect, path, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
	
	// Empty Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&emptyRect, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Empty = Empty.
	verifyCombineRectWithRegion (&emptyRect, emptyRegion, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Rect = Infinite.
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Rect = Rect.
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Rect + Infinite Path = Infinite.
	// FIXME: this should have scans: https://github.com/mono/libgdiplus/issues/412
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&emptyRect, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&emptyRect, emptyPath, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Path = Path.
	verifyCombineRectWithPath (&emptyRect, path, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Rect + Empty = Rect.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Rect = Infinite.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Rect + Empty Rect = Rect.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Rect = Empty.
	verifyCombineRectWithRect (&rect, &negativeRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Equal Rect = Equal Rect.
	verifyCombineRectWithRect (&rect, &rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Super Rect = Super Rect.
	verifyCombineRectWithRect (&rect, &superRect, CombineModeUnion, 0, 10, 50, 60, FALSE, FALSE, &superRect, sizeof (superRect));

	// Rect + Sub Rect = Rect.
	verifyCombineRectWithRect (&rect, &subRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Intersect Left = Calculation.
	RectF intersectLeftScan = {0, 20, 40, 40};
	verifyCombineRectWithRect (&rect, &intersectLeftRect, CombineModeUnion, 0, 20, 40, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Calculation.
	RectF intersectTopScan = {10, 10, 30, 50};
	RectF intersectTopScansRect[] = {intersectTopScan};
	verifyCombineRectWithRect (&rect, &intersectTopRect, CombineModeUnion, 10, 10, 30, 50, FALSE, FALSE, intersectTopScansRect, sizeof (intersectTopScansRect));
	
	// Rect + Intersect Right = Calculation.
	RectF intersectRightScan = {10, 20, 40, 40};
	verifyCombineRectWithRect (&rect, &intersectRightRect, CombineModeUnion, 10, 20, 40, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Calculation.
	RectF intersectBottomScan = {10, 20, 30, 50};
	RectF intersectBottomScansRect[] = {intersectBottomScan};
	verifyCombineRectWithRect (&rect, &intersectBottomRect, CombineModeUnion, 10, 20, 30, 50, FALSE, FALSE, intersectBottomScansRect, sizeof (intersectBottomScansRect));

	// Rect + Intersect Top Left = Calculation.
	RectF intersectTopLeftScans[] = {
		{0, 10, 30, 10},
		{0, 20, 40, 30},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopLeftRect, CombineModeUnion, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Calculation.
	RectF intersectTopRightScans[] = {
		{20, 10, 30, 10},
		{10, 20, 40, 30},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopRightRect, CombineModeUnion, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Calculation.
	RectF intersectBottomRightScans[] = {
		{10, 20, 30, 10},
		{10, 30, 40, 30},
		{20, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomRightRect, CombineModeUnion, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Calculation.
	RectF intersectBottomLeftScans[] = {
		{10, 20, 30, 10},
		{0, 30, 40, 30},
		{0, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomLeftRect, CombineModeUnion, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Calculation.
	RectF touchingLeftScan = {-20, 20, 60, 40};
	verifyCombineRectWithRect (&rect, &touchingLeftRect, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &touchingLeftScan, sizeof (touchingLeftScan));

	// Rect + Touching Top = Calculation.
	RectF touchingTopScan = {10, -20, 30, 80};
	RectF touchingTopScansRect[] = {touchingTopScan};
	verifyCombineRectWithRect (&rect, &touchingTopRect, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, touchingTopScansRect, sizeof (touchingTopScansRect));

	// Rect + Touching Right = Calculation.
	RectF touchingRightScan = {10, 20, 60, 40};
	verifyCombineRectWithRect (&rect, &touchingRightRect, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &touchingRightScan, sizeof (touchingRightScan));

	// Rect + Touching Bottom = Calculation.
	RectF touchingBottomScan = {10, 20, 30, 80};
	RectF touchingBottomScansRect[] = {touchingBottomScan};
	verifyCombineRectWithRect (&rect, &touchingBottomRect, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, touchingBottomScansRect, sizeof (touchingBottomScansRect));

	// Rect + Touching Top Left = Both.
	RectF touchingTopLeftScans[] = {
		touchingTopLeftRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &touchingTopLeftRect, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Rect + Touching Top Right = Both.
	RectF touchingTopRightScans[] = {
		touchingTopRightRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &touchingTopRightRect, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Rect + Touching Bottom Right = Both.
	RectF touchingBottomRightScans[] = {
		rect,
		touchingBottomRightRect
	};
	verifyCombineRectWithRect (&rect, &touchingBottomRightRect, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Rect + Touching Bottom Left = Both.
	RectF touchingBottomLeftScans[] = {
		rect,
		touchingBottomLeftRect
	};
	verifyCombineRectWithRect (&rect, &touchingBottomLeftRect, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));
	
	// Rect + No Intersect Left = Both.
	RectF noIntersectLeftScans[] = {
		noIntersectLeftRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeUnion, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Rect + No Intersect Top = Both.
	RectF noIntersectTopScans[] = {
		noIntersectTopRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeUnion, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Rect + No Intersect Right = Both.
	RectF noIntersectRightScans[] = {
		rect,
		noIntersectRightRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeUnion, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Rect + No Intersect Bottom = Both.
	RectF noIntersectBottomScans[] = {
		rect,
		noIntersectBottomRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeUnion, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));
	
	// Rect + Intersect Left Narrow = Calculation.
	RectF intersectLeftNarrowScans[] = {
		{10, 20, 30, 10},
		{-30, 30, 70, 10},
		{10, 40, 30, 20},
	};
	verifyCombineRectWithRect (&rect, &intersectLeftNarrowRect, CombineModeUnion, -30, 20, 70, 40, FALSE, FALSE, intersectLeftNarrowScans, sizeof (intersectLeftNarrowScans));

	// RectF intersectTopNarrowRect = {20, -20, 10, 60};
	// Rect + Intersect Top Narrow = Calculation.
	RectF intersectTopNarrowScans[] = {
		{20, -20, 10, 40},
		{10, 20, 30, 40},
	};
	verifyCombineRectWithRect (&rect, &intersectTopNarrowRect, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, intersectTopNarrowScans, sizeof (intersectTopNarrowScans));

	// Rect + Intersect Right Narrow = Calculation.
	RectF intersectRightNarrowScans[] = {
		{10, 20, 30, 10},
		{10, 30, 70, 10},
		{10, 40, 30, 20},
	};
	verifyCombineRectWithRect (&rect, &intersectRightNarrowRect, CombineModeUnion, 10, 20, 70, 40, FALSE, FALSE, intersectRightNarrowScans, sizeof (intersectRightNarrowScans));

	// Rect + Intersect Bottom Narrow = Calculation.
	RectF intersectBottomNarrowScans[] = {
		{10, 20, 30, 40},
		{20, 60, 10, 40}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomNarrowRect, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, intersectBottomNarrowScans, sizeof (intersectBottomNarrowScans));

	// Rect + Touches Top Intersect Bottom Narrow = Calculation.
	RectF touchesTopIntersectBottomNarrowScans[] = {
		{10, 20, 30, 40},
		{20, 60, 10, 20}
	};
	verifyCombineRectWithRect (&rect, &touchesTopIntersectBottomNarrowRect, CombineModeUnion, 10, 20, 30, 60, FALSE, FALSE, touchesTopIntersectBottomNarrowScans, sizeof (touchesTopIntersectBottomNarrowScans));

	// Rect + Intersect Right Taller = Calculation.
	RectF intersectRightTallerScans[] = {
		{10, 20, 40, 40},
		{20, 60, 30, 20},
	};
	verifyCombineRectWithRect (&rect, &intersectRightTallerRect, CombineModeUnion, 10, 20, 40, 60, FALSE, FALSE, intersectRightTallerScans, sizeof (intersectRightTallerScans));

	// Rect + Intersect Right Shorter = Calculation.
	RectF intersectRightShorterScans[] = {
		{10, 20, 40, 20},
		{10, 40, 30, 20},
	};
	verifyCombineRectWithRect (&rect, &intersectRightShorterRect, CombineModeUnion, 10, 20, 40, 40, FALSE, FALSE, intersectRightShorterScans, sizeof (intersectRightShorterScans));

	// Rect + Taller = Calculation.
	RectF tallerScan = tallerRect;
	verifyCombineRectWithRect (&rect, &tallerRect, CombineModeUnion, 10, 20, 30, 70, FALSE, FALSE, &tallerScan, sizeof (tallerScan));

	// Rect + Taller Narrow = Calculation.
	RectF tallerNarrowScans[] = {
		{10, 20, 30, 40},
		{10, 60, 10, 30},
	};
	verifyCombineRectWithRect (&rect, &tallerNarrowRect, CombineModeUnion, 10, 20, 30, 70, FALSE, FALSE, tallerNarrowScans, sizeof (tallerNarrowScans));

	// Rect + Wider = Calculation.
	RectF widerScan = widerRect;
	verifyCombineRectWithRect (&rect, &widerRect, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &widerScan, sizeof (widerScan));

	// Rect + Wider Shorter = Calculation.
	RectF widerShorterScans[] = {
		{10, 20, 60, 20},
		{10, 40, 30, 20},
	};
	verifyCombineRectWithRect (&rect, &widerShorterRect, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, widerShorterScans, sizeof (widerShorterScans));

	// Rect + Larger = Calculation.
	RectF largerScan = largerRect;
	verifyCombineRectWithRect (&rect, &largerRect, CombineModeUnion, 10, 20, 60, 70, FALSE, FALSE, &largerScan, sizeof (largerScan));

	// Rect + Overlap Taller Narrow = Calculation.
	RectF crossingScans[] = {
		{10, 20, 30, 10},
		{0, 30, 50, 20},
		{10, 50, 30, 10},
	};
	verifyCombineRectWithRect (&rect, &crossingRect, CombineModeUnion, 0, 20, 50, 40, FALSE, FALSE, crossingScans, sizeof (crossingScans));

	// Ported from mono/external/corefx/src/System.Drawing.Common/tests/RegionTests.cs:1813
	RectF intersectThreeRects[] = {
		{20, 180, 40, 50},
		{50, 190, 40, 50},
		{70, 210, 30, 50}
	};
	RectF intersectThreeRectsScans[] = {
		{20, 180, 40, 10},
		{20, 190, 70, 20},
		{20, 210, 80, 20},
		{50, 230, 50, 10},
		{70, 240, 30, 20}
	};
	verifyCombineRects (intersectThreeRects, sizeof (intersectThreeRects), CombineModeUnion, 20, 180, 80, 80, FALSE, FALSE, intersectThreeRectsScans, sizeof (intersectThreeRectsScans));

	// Ported from mono/external/corefx/src/System.Drawing.Common/tests/RegionTests.cs:1831
	RectF intersectFourRects[] = {
		{20, 330, 40, 50},
		{50, 340, 40, 50},
		{70, 360, 30, 50},
		{80, 400, 30, 10}
	};
	RectF intersectFourRectsScans[] = {
		{20, 330, 40, 10},
		{20, 340, 70, 20},
		{20, 360, 80, 20},
		{50, 380, 50, 10},
		{70, 390, 30, 10},
		{70, 400, 40, 10}
	};
	verifyCombineRects (intersectFourRects, sizeof (intersectFourRects), CombineModeUnion, 20, 330, 90, 80, FALSE, FALSE, intersectFourRectsScans, sizeof (intersectFourRectsScans));

	// Ported from mono/external/corefx/src/System.Drawing.Common/tests/RegionTests.cs:1918
	// Has two regions separated by 0 pixels.
	RectF fourPartialIntersectRects[] = {
		{30, 30, 80, 80},
		{45, 45, 200, 200},
		{160, 260, 10, 10},
		{170, 260, 10, 10}
	};
	RectF fourPartialIntersectRectsScans[] = {
		{30, 30, 80, 15},
		{30, 45, 215, 65},
		{45, 110, 200, 135},
		{160, 260, 20, 10}
	};
	verifyCombineRects (fourPartialIntersectRects, sizeof (fourPartialIntersectRects), CombineModeUnion, 30, 30, 215, 240, FALSE, FALSE, fourPartialIntersectRectsScans, sizeof (fourPartialIntersectRectsScans));

	// Rect + No Intersect Top Left = Both.
	RectF noIntersectTopLeftScans[] = {
		noIntersectTopLeftRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeUnion, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	RectF noIntersectTopRightScans[] = {
		noIntersectTopRightRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeUnion, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	RectF noIntersectBottomRightScans[] = {
		rect,
		noIntersectBottomRightRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeUnion, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	RectF noIntersectBottomLeftScans[] = {
		rect,
		noIntersectBottomLeftRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeUnion, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// No Intersect Top Rect + No Intersect Bottom Right = Both.
	RectF noIntersectTopAndBottomRight[] = {
		touchingTopRect,
		touchingBottomRightRect
	};
	verifyCombineRectWithRect (&touchingTopRect, &touchingBottomRightRect, CombineModeUnion, 10, -20, 60, 120, FALSE, FALSE, noIntersectTopAndBottomRight, sizeof (noIntersectTopAndBottomRight));


	// Rect + Infinite Path = Infinite.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Path = Rect.
	verifyCombineRectWithPath (&rect, negativePath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Equal Path = Equal Path.
	verifyCombineRectWithPath (&rect, path, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Super Path = Super Path.
	verifyCombineRectWithPath (&rect, superPath, CombineModeUnion, 0, 10, 50, 60, FALSE, FALSE, &superRect, sizeof (superRect));

	// Rect + Sub Path = Path.
	verifyCombineRectWithPath (&rect, subPath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Intersect Left = Calculation.
	verifyCombineRectWithPath (&rect, intersectLeftPath, CombineModeUnion, 0, 20, 40, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Calculation.
	verifyCombineRectWithPath (&rect, intersectTopPath, CombineModeUnion, 10, 10, 30, 50, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Calculation.
	verifyCombineRectWithPath (&rect, intersectRightPath, CombineModeUnion, 10, 20, 40, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Calculation.
	verifyCombineRectWithPath (&rect, intersectBottomPath, CombineModeUnion, 10, 20, 30, 50, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Calculation.
	verifyCombineRectWithPath (&rect, intersectTopLeftPath, CombineModeUnion, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Calculation.
	verifyCombineRectWithPath (&rect, intersectTopRightPath, CombineModeUnion, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Calculation.
	verifyCombineRectWithPath (&rect, intersectBottomRightPath, CombineModeUnion, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Calculation.
	verifyCombineRectWithPath (&rect, intersectBottomLeftPath, CombineModeUnion, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Calculation.
	verifyCombineRectWithPath (&rect, touchingLeftPath, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &touchingLeftScan, sizeof (touchingLeftScan));

	// Rect + Touching Top = Calculation.
	verifyCombineRectWithPath (&rect, touchingTopPath, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, &touchingTopScan, sizeof (touchingTopScan));

	// Rect + Touching Right = Calculation.
	verifyCombineRectWithPath (&rect, touchingRightPath, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &touchingRightScan, sizeof (touchingRightScan));

	// Rect + Touching Bottom = Calculation.
	verifyCombineRectWithPath (&rect, touchingBottomPath, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, &touchingBottomScan, sizeof (touchingBottomScan));

	// Rect + Touching Top Left = Both.
	verifyCombineRectWithPath (&rect, touchingTopLeftPath, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Rect + Touching Top Right = Both.
	verifyCombineRectWithPath (&rect, touchingTopRightPath, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Rect + Touching Bottom Right = Both.
	verifyCombineRectWithPath (&rect, touchingBottomRightPath, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Rect + Touching Bottom Left = Both.
	verifyCombineRectWithPath (&rect, touchingBottomLeftPath, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));

	// Rect + No Intersect Left = Calculation.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeUnion, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));
#endif

	// Rect + No Intersect Top = Calculation.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeUnion, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));
#endif

	// Rect + No Intersect Right = Calculation.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeUnion, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));
#endif

	// Rect + No Intersect Bottom = Calculation.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeUnion, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));
#endif

	// Rect + No Intersect Top Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeUnion, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeUnion, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeUnion, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));
#endif

	// Rect + No Intersect Bottom Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeUnion, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// Infinite Path + Infinite = Infinite.
	verifyCombinePathWithRegion (infinitePath, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Empty = Infinite.
	verifyCombinePathWithRegion (infinitePath, emptyRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Infinite Rect = Infinite.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Empty Rect = Infinite.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Rect = Infinite.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
	
	// Infinite Path + Infinite Path = Infinite.
	verifyCombinePathWithPath (infinitePath, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Empty Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/412
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (infinitePath, emptyPath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Infinite Path + Path = Infinite.
	verifyCombinePathWithPath (infinitePath, path, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
	
	// Empty Path + Infinite = Infinite.
	verifyCombinePathWithRegion (emptyPath, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Empty = Empty.
	verifyCombinePathWithRegion (emptyPath, emptyRegion, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Rect = Infinite.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Empty Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Rect = Rect.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Path + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/412
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (emptyPath, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Path + Empty Path = Empty.
	verifyCombinePathWithPath (emptyPath, emptyPath, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Path = Path.
	verifyCombinePathWithPath (emptyPath, path, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite = Infinite.
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Path + Empty = Empty.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite Rect = Infinite.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Path + Empty Rect = Empty.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Rect = Rect.
	verifyCombinePathWithRect (path, &negativeRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Equal Rect = Equal Rect.
	verifyCombinePathWithRect (path, &rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Super Rect = Super Rect.
	verifyCombinePathWithRect (path, &superRect, CombineModeUnion, 0, 10, 50, 60, FALSE, FALSE, &superRect, sizeof (superRect));

	// Path + Sub Rect = Rect.
	verifyCombinePathWithRect (path, &subRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Intersect Left = Calculation.
	verifyCombinePathWithRect (path, &intersectLeftRect, CombineModeUnion, 0, 20, 40, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Calculation.
	verifyCombinePathWithRect (path, &intersectTopRect, CombineModeUnion, 10, 10, 30, 50, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));
	
	// Path + Intersect Right = Calculation.
	verifyCombinePathWithRect (path, &intersectRightRect, CombineModeUnion, 10, 20, 40, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomRect, CombineModeUnion, 10, 20, 30, 50, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Calculation.
	verifyCombinePathWithRect (path, &intersectTopLeftRect, CombineModeUnion, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Calculation.
	verifyCombinePathWithRect (path, &intersectTopRightRect, CombineModeUnion, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomRightRect, CombineModeUnion, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomLeftRect, CombineModeUnion, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Calculation.
	verifyCombinePathWithRect (path, &touchingLeftRect, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &touchingLeftScan, sizeof (touchingLeftScan));

	// Path + Touching Top = Calculation.
	verifyCombinePathWithRect (path, &touchingTopRect, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, &touchingTopScan, sizeof (touchingTopScan));

	// Path + Touching Right = Calculation.
	verifyCombinePathWithRect (path, &touchingRightRect, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &touchingRightScan, sizeof (touchingRightScan));

	// Path + Touching Bottom = Calculation.
	verifyCombinePathWithRect (path, &touchingBottomRect, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, &touchingBottomScan, sizeof (touchingBottomScan));

	// Path + Touching Top Left = Both.
	verifyCombinePathWithRect (path, &touchingTopLeftRect, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Path + Touching Top Right = Both.
	verifyCombinePathWithRect (path, &touchingTopRightRect, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Path + Touching Bottom Right = Both.
	verifyCombinePathWithRect (path, &touchingBottomRightRect, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Path + Touching Bottom Left = Both.
	verifyCombinePathWithRect (path, &touchingBottomLeftRect, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));

	// Path + No Intersect Left = Calculation.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeUnion, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));
#endif

	// Path + No Intersect Top = Calculation.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeUnion, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));
#endif

	// Path + No Intersect Right = Calculation.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeUnion, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));
#endif

	// Path + No Intersect Bottom = Calculation.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeUnion, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));
#endif

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeUnion, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeUnion, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeUnion, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeUnion, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// Path + Infinite Path = Infinite.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Path + Empty Path = Empty.
	verifyCombinePathWithPath (path, emptyPath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Path = Path.
	verifyCombinePathWithPath (path, negativePath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Equal Path = Equal Path.
	verifyCombinePathWithPath (path, path, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Super Path = Super Path.
	verifyCombinePathWithPath (path, superPath, CombineModeUnion, 0, 10, 50, 60, FALSE, FALSE, &superRect, sizeof (superRect));

	// Path + Sub Path = Path.
	verifyCombinePathWithPath (path, subPath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Intersect Left = Calculation.
	verifyCombinePathWithPath (path, intersectLeftPath, CombineModeUnion, 0, 20, 40, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Calculation.
	verifyCombinePathWithPath (path, intersectTopPath, CombineModeUnion, 10, 10, 30, 50, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Calculation.
	verifyCombinePathWithPath (path, intersectRightPath, CombineModeUnion, 10, 20, 40, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Calculation.
	verifyCombinePathWithPath (path, intersectBottomPath, CombineModeUnion, 10, 20, 30, 50, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Calculation.
	verifyCombinePathWithPath (path, intersectTopLeftPath, CombineModeUnion, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Calculation.
	verifyCombinePathWithPath (path, intersectTopRightPath, CombineModeUnion, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Calculation.
	verifyCombinePathWithPath (path, intersectBottomRightPath, CombineModeUnion, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Calculation.
	verifyCombinePathWithPath (path, intersectBottomLeftPath, CombineModeUnion, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Calculation.
	verifyCombinePathWithPath (path, touchingLeftPath, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &touchingLeftScan, sizeof (touchingLeftScan));

	// Path + Touching Top = Calculation.
	verifyCombinePathWithPath (path, touchingTopPath, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, &touchingTopScan, sizeof (touchingTopScan));

	// Path + Touching Right = Calculation.
	verifyCombinePathWithPath (path, touchingRightPath, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &touchingRightScan, sizeof (touchingRightScan));

	// Path + Touching Bottom = Calculation.
	verifyCombinePathWithPath (path, touchingBottomPath, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, &touchingBottomScan, sizeof (touchingBottomScan));

	// Path + Touching Top Left = Both.
	verifyCombinePathWithPath (path, touchingTopLeftPath, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Path + Touching Top Right = Both.
	verifyCombinePathWithPath (path, touchingTopRightPath, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Path + Touching Bottom Right = Both.
	verifyCombinePathWithPath (path, touchingBottomRightPath, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Path + Touching Bottom Left = Both.
	verifyCombinePathWithPath (path, touchingBottomLeftPath, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));
	
	// Path + No Intersect Left = Calculation.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeUnion, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));
#endif

	// Path + No Intersect Top = Calculation.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeUnion, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));
#endif

	// Path + No Intersect Right = Calculation.
	// FIXME: scans are wrong: https://github.com/mono/libgdiplus/issues/413
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeUnion, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));
#endif

	// Path + No Intersect Bottom = Calculation.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeUnion, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));
#endif

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeUnion, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeUnion, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeUnion, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeUnion, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	GdipDeleteRegion (infiniteRegion);
	GdipDeleteRegion (emptyRegion);
	GdipDeleteRegion (rectRegion);
	GdipDeleteRegion (pathRegion);
	GdipDeletePath (path);
	GdipDeletePath (superPath);
	GdipDeletePath (subPath);
	GdipDeletePath (intersectLeftPath);
	GdipDeletePath (intersectTopPath);
	GdipDeletePath (intersectRightPath);
	GdipDeletePath (intersectBottomPath);
	GdipDeletePath (intersectTopLeftPath);
	GdipDeletePath (intersectTopRightPath);
	GdipDeletePath (intersectBottomRightPath);
	GdipDeletePath (intersectBottomLeftPath);
	GdipDeletePath (touchingLeftPath);
	GdipDeletePath (touchingTopPath);
	GdipDeletePath (touchingRightPath);
	GdipDeletePath (touchingBottomPath);
	GdipDeletePath (touchingTopLeftPath);
	GdipDeletePath (touchingTopRightPath);
	GdipDeletePath (touchingBottomRightPath);
	GdipDeletePath (touchingBottomLeftPath);
	GdipDeletePath (noIntersectLeftPath);
	GdipDeletePath (noIntersectTopPath);
	GdipDeletePath (noIntersectRightPath);
	GdipDeletePath (noIntersectBottomPath);
	GdipDeletePath (noIntersectTopLeftPath);
	GdipDeletePath (noIntersectTopRightPath);
	GdipDeletePath (noIntersectBottomRightPath);
	GdipDeletePath (noIntersectBottomLeftPath);
	GdipDeletePath (emptyPath);
	GdipDeletePath (infinitePath);
	GdipDeletePath (negativePath);
}

static void test_combineXor ()
{
	GpRegion *infiniteRegion;
	GpRegion *emptyRegion;
	GpRegion *rectRegion;
	GpRegion *pathRegion;

	GdipCreateRegion (&infiniteRegion);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	RectF rect = {10, 20, 30, 40};
	RectF superRect = {0, 10, 50, 60};
	RectF subRect = {15, 25, 20, 30};
	RectF intersectLeftRect = {0, 20, 30, 40};
	RectF intersectTopRect = {10, 10, 30, 40};
	RectF intersectRightRect = {20, 20, 30, 40};
	RectF intersectBottomRect = {10, 30, 30, 40};
	RectF intersectTopLeftRect = {0, 10, 30, 40};
	RectF intersectTopRightRect = {20, 10, 30, 40};
	RectF intersectBottomRightRect = {20, 30, 30, 40};
	RectF intersectBottomLeftRect = {0, 30, 30, 40};
	RectF touchingLeftRect = {-20, 20, 30, 40};
	RectF touchingTopRect = {10, -20, 30, 40};
	RectF touchingRightRect = {40, 20, 30, 40};
	RectF touchingBottomRect = {10, 60, 30, 40};
	RectF touchingTopLeftRect = {-20, -20, 30, 40};
	RectF touchingTopRightRect = {40, -20, 30, 40};
	RectF touchingBottomRightRect = {40, 60, 30, 40};
	RectF touchingBottomLeftRect = {-20, 60, 30, 40};
	RectF noIntersectLeftRect = {-21, 20, 30, 40};
	RectF noIntersectTopRect = {10, -21, 30, 40};
	RectF noIntersectRightRect = {41, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 61, 30, 40};
	RectF noIntersectTopLeftRect = {-21, -21, 30, 40};
	RectF noIntersectTopRightRect = {41, -21, 30, 40};
	RectF noIntersectBottomRightRect = {41, 61, 30, 40};
	RectF noIntersectBottomLeftRect = {-21, 61, 30, 40};
	RectF emptyRect = {0, 0, 0, 0};
	RectF infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	RectF negativeRect = {20, 30, -10, -10};
	GdipCreateRegionRect (&rect, &rectRegion);

	GpPath *path = createPathFromRect (&rect);
	GpPath *superPath = createPathFromRect (&superRect);
	GpPath *subPath = createPathFromRect (&subRect);
	GpPath *intersectLeftPath = createPathFromRect (&intersectLeftRect);
	GpPath *intersectTopPath = createPathFromRect (&intersectTopRect);
	GpPath *intersectRightPath = createPathFromRect (&intersectRightRect);
	GpPath *intersectBottomPath = createPathFromRect (&intersectBottomRect);
	GpPath *intersectTopLeftPath = createPathFromRect (&intersectTopLeftRect);
	GpPath *intersectTopRightPath = createPathFromRect (&intersectTopRightRect);
	GpPath *intersectBottomRightPath = createPathFromRect (&intersectBottomRightRect);
	GpPath *intersectBottomLeftPath = createPathFromRect (&intersectBottomLeftRect);
	GpPath *touchingLeftPath = createPathFromRect (&touchingLeftRect);
	GpPath *touchingTopPath = createPathFromRect (&touchingTopRect);
	GpPath *touchingRightPath = createPathFromRect (&touchingRightRect);
	GpPath *touchingBottomPath = createPathFromRect (&touchingBottomRect);
	GpPath *touchingTopLeftPath = createPathFromRect (&touchingTopLeftRect);
	GpPath *touchingTopRightPath = createPathFromRect (&touchingTopRightRect);
	GpPath *touchingBottomRightPath = createPathFromRect (&touchingBottomRightRect);
	GpPath *touchingBottomLeftPath = createPathFromRect (&touchingBottomLeftRect);
	GpPath *noIntersectLeftPath = createPathFromRect (&noIntersectLeftRect);
	GpPath *noIntersectTopPath = createPathFromRect (&noIntersectTopRect);
	GpPath *noIntersectRightPath = createPathFromRect (&noIntersectRightRect);
	GpPath *noIntersectBottomPath = createPathFromRect (&noIntersectBottomRect);
	GpPath *noIntersectTopLeftPath = createPathFromRect (&noIntersectTopLeftRect);
	GpPath *noIntersectTopRightPath = createPathFromRect (&noIntersectTopRightRect);
	GpPath *noIntersectBottomRightPath = createPathFromRect (&noIntersectBottomRightRect);
	GpPath *noIntersectBottomLeftPath = createPathFromRect (&noIntersectBottomLeftRect);
	GpPath *emptyPath = createPathFromRect (&emptyRect);
	GpPath *infinitePath = createPathFromRect (&infiniteRect);
	GpPath *negativePath = createPathFromRect (&negativeRect);

	GdipCreateRegionPath (path, &pathRegion);

	// Infinite + Infinite = Empty
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Empty = Infinite.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Infinite Rect = Empty
	// FIXME: should be empty: https://github.com/mono/libgdiplus/issues/342
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithRect (&infiniteRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite + Empty Rect = Infinite.
	verifyCombineInfiniteWithRect (&emptyRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Rect = Not Rect.
	RectF infiniteWithRectScans[] = {
		{-4194304, -4194304, 8388608, 4194324},
		{-4194304, 20, 4194314, 40},
		{40, 20, 4194264, 40},
		{-4194304, 60, 8388608, 4194244}
	};
	verifyCombineInfiniteWithRect (&rect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
	
	// Infinite + Negative Rect = Infinite.
	{
		GpRegion *region;
		GdipCreateRegion (&region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF infiniteWithNegativeRectScans[] = {
			{-4194304, -4194304, 8388608, 4194324},
			{-4194304, 20, 4194314, 10},
			{20, 20, 4194284, 10},
			{-4194304, 30, 8388608, 4194274}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeXor);
		verifyRegion (clone, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE);
		verifyRegionScans (clone, infiniteWithNegativeRectScans, sizeof (infiniteWithNegativeRectScans));
		GdipDeleteRegion (clone);
	}
	
	// Infinite + Infinite Path = Empty
	// FIXME: shoud be infinite: https://github.com/mono/libgdiplus/issues/342
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithPath (infinitePath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite + Empty Path = Infinite.
	verifyCombineInfiniteWithPath (emptyPath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithPath (path, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty + Infinite = Infinite.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite Rect = Infinite.
	verifyCombineEmptyWithRect (&infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty Rect = Empty.
	verifyCombineEmptyWithRect (&emptyRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty + Infinite Path = Infinite.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineEmptyWithPath (infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty + Empty Path = Empty.
	verifyCombineEmptyWithPath (emptyPath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
	
	// Infinite Rect + Infinite = Empty.
	verifyCombineRectWithRegion (&infiniteRect, infiniteRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Empty = Infinite Rect.
	verifyCombineRectWithRegion (&infiniteRect, emptyRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Infinite Rect = Empty.
	// FIXME: this should be empty: https://github.com/mono/libgdiplus/issues/342
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite Rect + Empty Rect = Infinite.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Rect = Not Rect.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Infinite Rect + Infinite Path = Empty.
	// FIXME: this should be empty: https://github.com/mono/libgdiplus/issues/342
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&infiniteRect, infinitePath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite Rect + Empty Path = Infinite.
	verifyCombineRectWithPath (&infiniteRect, emptyPath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&infiniteRect, path, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&emptyRect, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Empty = Empty.
	verifyCombineRectWithRegion (&emptyRect, emptyRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Rect = Infinite.
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Rect = Rect.
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Rect + Infinite Path = Infinite.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&emptyRect, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&emptyRect, emptyPath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Path = Path.
	verifyCombineRectWithPath (&emptyRect, path, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite = Not Rect.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Rect + Empty = Rect.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Rect = Not Rect.
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Rect + Empty Rect = Rect.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Rect = Rect.
	{
		GpRegion *region;
		GdipCreateRegionRect (&rect, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		RectF negativeRectScan = {10, 20, 30, 40};
		verifyCombineRegionWithRegion (region, region2, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF rectWithNegativeRectScans[] = {
			{20, 20, 20, 10},
			{10, 30, 30, 30}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeXor);
		verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
		verifyRegionScans (clone, rectWithNegativeRectScans, sizeof (rectWithNegativeRectScans));
		GdipDeleteRegion (clone);
	}

	// Rect + Equal Rect = Empty Rect.
	verifyCombineRectWithRect (&rect, &rect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Super Rect = Not Rect and Super Rect.
	RectF superScans[] = {
		{0, 10, 50, 10},
		{0, 20, 10, 40},
		{40, 20, 10, 40},
		{0, 60, 50, 10}
	};
	verifyCombineRectWithRect (&rect, &superRect, CombineModeXor, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));

	// Rect + Sub Rect = Not Sub Rect and Rect.
	RectF subScans[] = {
		{10, 20, 30, 5},
		{10, 25, 5, 30},
		{35, 25, 5, 30},
		{10, 55, 30, 5}
	};
	verifyCombineRectWithRect (&rect, &subRect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));

	// Rect + Intersect Left = Not (Rect and Intersect Left)
	RectF intersectLeftScans[] = {
		{0, 20, 10, 40},
		{30, 20, 10, 40}
	};
	verifyCombineRectWithRect (&rect, &intersectLeftRect, CombineModeXor, 0, 20, 40, 40, FALSE, FALSE, intersectLeftScans, sizeof (intersectLeftScans));

	// Rect + Intersect Top = Not (Rect and Intersect Top).
	RectF intersectTopScans[] = {
		{10, 10, 30, 10},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopRect, CombineModeXor, 10, 10, 30, 50, FALSE, FALSE, intersectTopScans, sizeof (intersectTopScans));

	// Rect + Intersect Right = Not (Rect and Intersect Right).
	RectF intersectRightScans[] = {
		{10, 20, 10, 40},
		{40, 20, 10, 40}
	};
	verifyCombineRectWithRect (&rect, &intersectRightRect, CombineModeXor, 10, 20, 40, 40, FALSE, FALSE, intersectRightScans, sizeof (intersectRightScans));

	// Rect + Intersect Bottom = Not (Rect and Intersect Bottom).
	RectF intersectBottomScans[] = {
		{10, 20, 30, 10},
		{10, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomRect, CombineModeXor, 10, 20, 30, 50, FALSE, FALSE, intersectBottomScans, sizeof (intersectBottomScans));

	// Rect + Intersect Top Left = Not (Rect and Intersect Left).
	RectF intersectTopLeftScans[] = {
		{0, 10, 30, 10},
		{0, 20, 10, 30},
		{30, 20, 10, 30},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopLeftRect, CombineModeXor, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Calculation.
	RectF intersectTopRightScans[] = {
		{20, 10, 30, 10},
		{10, 20, 10, 30},
		{40, 20, 10, 30},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopRightRect, CombineModeXor, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Calculation.
	RectF intersectBottomRightScans[] = {
		{10, 20, 30, 10},
		{10, 30, 10, 30},
		{40, 30, 10, 30},
		{20, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomRightRect, CombineModeXor, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Calculation.
	RectF intersectBottomLeftScans[] = {
		{10, 20, 30, 10},
		{0, 30, 10, 30},
		{30, 30, 10, 30},
		{0, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomLeftRect, CombineModeXor, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF touchingLeftScans[] = {{-20, 20, 60, 40}};
#else
	RectF touchingLeftScans[] = {
		rect,
		touchingLeftRect
	};
#endif
	verifyCombineRectWithRect (&rect, &touchingLeftRect, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, touchingLeftScans, sizeof (touchingLeftScans));

	// Rect + Touching Top = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF touchingTopScans[] = {{10, -20, 30, 80}};
#else
	RectF touchingTopScans[] = {
		rect,
		touchingTopRect
	};
#endif
	verifyCombineRectWithRect (&rect, &touchingTopRect, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, touchingTopScans, sizeof (touchingTopScans));

	// Rect + Touching Right = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF touchingRightScans[] = {{10, 20, 60, 40}};
#else
	RectF touchingRightScans[] = {
		rect,
		touchingRightRect
	};
#endif
	verifyCombineRectWithRect (&rect, &touchingRightRect, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, touchingRightScans, sizeof (touchingRightScans));

	// Rect + Touching Bottom = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF touchingBottomScans[] = {{10, 20, 30, 80}};
#else
	RectF touchingBottomScans[] = {{10, 20, 30, 40}, {10, 60, 30, 40}};
#endif
	verifyCombineRectWithRect (&rect, &touchingBottomRect, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, touchingBottomScans, sizeof (touchingBottomScans));

	// Rect + Touching Top Left = Both.
	// FIXME: different ordering: https://github.com/mono/libgdiplus/issues/346
#if defined(USE_WINDOWS_GDIPLUS)
	RectF touchingTopLeftScans[] = {
		touchingTopLeftRect,
		rect
	};
#else
	RectF touchingTopLeftScans[] = {
		rect,
		touchingTopLeftRect
	};
#endif
	verifyCombineRectWithRect (&rect, &touchingTopLeftRect, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Rect + Touching Top Right = Both.
	// FIXME: different ordering: https://github.com/mono/libgdiplus/issues/346
#if defined(USE_WINDOWS_GDIPLUS)
	RectF touchingTopRightScans[] = {
		touchingTopRightRect,
		rect
	};
#else
	RectF touchingTopRightScans[] = {
		rect,
		touchingTopRightRect
	};
#endif
	verifyCombineRectWithRect (&rect, &touchingTopRightRect, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Rect + Touching Bottom Right = Both.
	RectF touchingBottomRightScans[] = {
		rect,
		touchingBottomRightRect
	};
	verifyCombineRectWithRect (&rect, &touchingBottomRightRect, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Rect + Touching Bottom Left = Both.
	RectF touchingBottomLeftScans[] = {
		rect,
		touchingBottomLeftRect
	};
	verifyCombineRectWithRect (&rect, &touchingBottomLeftRect, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));

	// Rect + No Intersect Left = Both.
	RectF noIntersectLeftScans[] = {
#if defined(USE_WINDOWS_GDIPLUS)
		noIntersectLeftRect,
		rect
#else
		rect,
		noIntersectLeftRect
#endif
	};
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeXor, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Rect + No Intersect Top = Both.
	RectF noIntersectTopScans[] = {
#if defined(USE_WINDOWS_GDIPLUS)
		noIntersectTopRect,
		rect
#else
		rect,
		noIntersectTopRect
#endif
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeXor, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Rect + No Intersect Right = Both.
	RectF noIntersectRightScans[] = {
		rect,
		noIntersectRightRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeXor, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Rect + No Intersect Bottom = Both.
	RectF noIntersectBottomScans[] = {
		rect,
		noIntersectBottomRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeXor, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Rect + No Intersect Top Left = Both.
	RectF noIntersectTopLeftScans[] = {
#if defined(USE_WINDOWS_GDIPLUS)
		noIntersectTopLeftRect,
		rect
#else
		rect,
		noIntersectTopLeftRect
#endif
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeXor, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	RectF noIntersectTopRightScans[] = {
#if defined(USE_WINDOWS_GDIPLUS)
		noIntersectTopRightRect,
		rect
#else
		rect,
		noIntersectTopRightRect
#endif
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeXor, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	RectF noIntersectBottomRightScans[] = {
		rect,
		noIntersectBottomRightRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeXor, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	RectF noIntersectBottomLeftScans[] = {
		rect,
		noIntersectBottomLeftRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeXor, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// Rect + Infinite Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Rect + Empty Path = Path.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Path = Rect.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Equal Path = Empty.
	verifyCombineRectWithPath (&rect, path, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/347.
#if defined(USE_WINDOWS_GDIPLUS)
	// Rect + Super Rect = Not Rect and Super Rect.
	verifyCombineRectWithPath (&rect, superPath, CombineModeXor, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));

	// Rect + Sub Rect = Not Sub Rect and Rect.
	verifyCombineRectWithPath (&rect, subPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));

	// Rect + Intersect Left = Not (Rect and Intersect Left).
	verifyCombineRectWithPath (&rect, intersectLeftPath, CombineModeXor, 0, 20, 40, 40, FALSE, FALSE, intersectLeftScans, sizeof (intersectLeftScans));

	// Rect + Intersect Top = Not (Rect and Intersect Top).
	verifyCombineRectWithPath (&rect, intersectTopPath, CombineModeXor, 10, 10, 30, 50, FALSE, FALSE, intersectTopScans, sizeof (intersectTopScans));

	// Rect + Intersect Right = Not (Rect and Intersect Right).
	verifyCombineRectWithPath (&rect, intersectRightPath, CombineModeXor, 10, 20, 40, 40, FALSE, FALSE, intersectRightScans, sizeof (intersectRightScans));

	// Rect + Intersect Bottom = Not (Rect and Intersect Bottom).
	verifyCombineRectWithPath (&rect, intersectBottomPath, CombineModeXor, 10, 20, 30, 50, FALSE, FALSE, intersectBottomScans, sizeof (intersectBottomScans));

	// Rect + Intersect Top Left = Not (Rect and Intersect Top Left).
	verifyCombineRectWithPath (&rect, intersectTopLeftPath, CombineModeXor, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Not (Rect and Intersect Top Right).
	verifyCombineRectWithPath (&rect, intersectTopRightPath, CombineModeXor, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Not (Rect and Intersect Bottom Right).
	verifyCombineRectWithPath (&rect, intersectBottomRightPath, CombineModeXor, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Not (Rect and Intersect Bottom Left).
	verifyCombineRectWithPath (&rect, intersectBottomLeftPath, CombineModeXor, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Both.
	verifyCombineRectWithPath (&rect, touchingLeftPath, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, touchingLeftScans, sizeof (touchingLeftScans));

	// Rect + Touching Top = Both.
	verifyCombineRectWithPath (&rect, touchingTopPath, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, touchingTopScans, sizeof (touchingTopScans));

	// Rect + Touching Right = Both.
	verifyCombineRectWithPath (&rect, touchingRightPath, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, touchingRightScans, sizeof (touchingRightScans));

	// Rect + Touching Bottom = Both.
	verifyCombineRectWithPath (&rect, touchingBottomPath, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, touchingBottomScans, sizeof (touchingBottomScans));

	// Rect + Touching Top Left = Both.
	verifyCombineRectWithPath (&rect, touchingTopLeftPath, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Rect + Touching Top Right = Both.
	verifyCombineRectWithPath (&rect, touchingTopRightPath, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Rect + Touching Bottom Right = Both.
	verifyCombineRectWithPath (&rect, touchingBottomRightPath, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Rect + Touching Bottom Left = Both.
	verifyCombineRectWithPath (&rect, touchingBottomLeftPath, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));

	// Rect + No Intersect Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeXor, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Rect + No Intersect Top = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeXor, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Rect + No Intersect Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeXor, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Rect + No Intersect Bottom = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeXor, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Rect + No Intersect Top Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeXor, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeXor, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeXor, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeXor, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
#endif

	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	// Infinite Path + Infinite = Empty.
	verifyCombinePathWithRegion (infinitePath, infiniteRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty = Infinite.
	verifyCombinePathWithRegion (infinitePath, emptyRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Infinite Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Rect = Infinite.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Rect = Not Path.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Infinite Path + Infinite Path = Empty.
	verifyCombinePathWithPath (infinitePath, infinitePath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Path = Infinite.
	verifyCombinePathWithPath (infinitePath, emptyPath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Path = Not Path.
	verifyCombinePathWithPath (infinitePath, path, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty Path + Infinite = Infinite.
	verifyCombinePathWithRegion (emptyPath, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Empty = Empty.
	verifyCombinePathWithRegion (emptyPath, emptyRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Rect = Infinite.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Empty Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Rect = Rect.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Path + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/416
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (emptyPath, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Path + Empty Path = Empty.
	verifyCombinePathWithPath (emptyPath, emptyPath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Path = Path.
	verifyCombinePathWithPath (emptyPath, path, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Path + Empty = Rect.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite Rect = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Path + Empty Rect = Path.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Rect = Path.
	{
		GpRegion *region;
		GdipCreateRegionPath (path, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		RectF negativeRectScan = {10, 20, 30, 40};
		verifyCombineRegionWithRegion (region, region2, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF rectWithNegativePathScans[] = {
			{20, 20, 20, 10},
			{10, 30, 30, 30}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeXor);
		verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
		verifyRegionScans (clone, rectWithNegativePathScans, sizeof (rectWithNegativePathScans));
		GdipDeleteRegion (clone);
	}

	// Path + Equal Rect = Empty.
	// FIXME: should be empty: https://github.com/mono/libgdiplus/issues/348
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &rect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/347
#if defined(USE_WINDOWS_GDIPLUS)
	// Path + Super Rect = Not Path and Super Rect.
	verifyCombinePathWithRect (path, &superRect, CombineModeXor, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));

	// Path + Sub Rect = Not Sub Rect and Rect.
	verifyCombinePathWithRect (path, &subRect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));

	// Path + Intersect Left = Not (Rect and Intersect Left).
	verifyCombinePathWithRect (path, &intersectLeftRect, CombineModeXor, 0, 20, 40, 40, FALSE, FALSE, intersectLeftScans, sizeof (intersectLeftScans));

	// Path + Intersect Top = Not (Rect and Intersect Top).
	verifyCombinePathWithRect (path, &intersectTopRect, CombineModeXor, 10, 10, 30, 50, FALSE, FALSE, intersectTopScans, sizeof (intersectTopScans));

	// Path + Intersect Right = Not (Rect and Intersect Right).
	verifyCombinePathWithRect (path, &intersectRightRect, CombineModeXor, 10, 20, 40, 40, FALSE, FALSE, intersectRightScans, sizeof (intersectRightScans));

	// Path + Intersect Bottom = Not (Rect and Intersect Bottom).
	verifyCombinePathWithRect (path, &intersectBottomRect, CombineModeXor, 10, 20, 30, 50, FALSE, FALSE, intersectBottomScans, sizeof (intersectBottomScans));

	// Path + Intersect Top Left = Not (Rect and Intersect Left).
	verifyCombinePathWithRect (path, &intersectTopLeftRect, CombineModeXor, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Calculation.
	verifyCombinePathWithRect (path, &intersectTopRightRect, CombineModeXor, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomRightRect, CombineModeXor, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Calculation.
	verifyCombinePathWithRect (path, &intersectBottomLeftRect, CombineModeXor, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Both.
	verifyCombinePathWithRect (path, &touchingLeftRect, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, touchingLeftScans, sizeof (touchingLeftScans));

	// Path + Touching Top = Both.
	verifyCombinePathWithRect (path, &touchingTopRect, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, touchingTopScans, sizeof (touchingTopScans));

	// Path + Touching Right = Both.
	verifyCombinePathWithRect (path, &touchingRightRect, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, touchingRightScans, sizeof (touchingRightScans));

	// Path + Touching Bottom = Both.
	verifyCombinePathWithRect (path, &touchingBottomRect, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, touchingBottomScans, sizeof (touchingBottomScans));

	// Path + Touching Top Left = Both.
	verifyCombinePathWithRect (path, &touchingTopLeftRect, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Path + Touching Top Right = Both.
	verifyCombinePathWithRect (path, &touchingTopRightRect, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Path + Touching Bottom Right = Both.
	verifyCombinePathWithRect (path, &touchingBottomRightRect, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Path + Touching Bottom Left = Both.
	verifyCombinePathWithRect (path, &touchingBottomLeftRect, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));
	
	// Path + No Intersect Left = Both.
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeXor, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Path + No Intersect Top = Both.
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeXor, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Path + No Intersect Right = Both.
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeXor, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Path + No Intersect Bottom = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeXor, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeXor, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeXor, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeXor, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeXor, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
#endif

	// Path + Infinite Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Path + Empty Path = Path.
	verifyCombinePathWithPath (path, emptyPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Path = Path.
	verifyCombinePathWithPath (path, negativePath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Equal Path = Empty.
	verifyCombinePathWithPath (path, path, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/347.
#if defined(USE_WINDOWS_GDIPLUS)
	// Path + Super Rect = Not Path and Super Rect.
	verifyCombinePathWithPath (path, superPath, CombineModeXor, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));

	// Path + Sub Rect = Not Sub Rect and Rect.
	verifyCombinePathWithPath (path, subPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));

	// Path + Intersect Left = Not (Rect and Intersect Left).
	verifyCombinePathWithPath (path, intersectLeftPath, CombineModeXor, 0, 20, 40, 40, FALSE, FALSE, intersectLeftScans, sizeof (intersectLeftScans));

	// Path + Intersect Top = Not (Rect and Intersect Top).
	verifyCombinePathWithPath (path, intersectTopPath, CombineModeXor, 10, 10, 30, 50, FALSE, FALSE, intersectTopScans, sizeof (intersectTopScans));

	// Path + Intersect Right = Not (Rect and Intersect Right).
	verifyCombinePathWithPath (path, intersectRightPath, CombineModeXor, 10, 20, 40, 40, FALSE, FALSE, intersectRightScans, sizeof (intersectRightScans));

	// Path + Intersect Bottom = Not (Rect and Intersect Bottom).
	verifyCombinePathWithPath (path, intersectBottomPath, CombineModeXor, 10, 20, 30, 50, FALSE, FALSE, intersectBottomScans, sizeof (intersectBottomScans));

	// Path + Intersect Top Left = Not (Rect and Intersect Top Left).
	verifyCombinePathWithPath (path, intersectTopLeftPath, CombineModeXor, 0, 10, 40, 50, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Not (Rect and Intersect Top Right).
	verifyCombinePathWithPath (path, intersectTopRightPath, CombineModeXor, 10, 10, 40, 50, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Not (Rect and Intersect Bottom Right).
	verifyCombinePathWithPath (path, intersectBottomRightPath, CombineModeXor, 10, 20, 40, 50, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Not (Rect and Intersect Bottom Left).
	verifyCombinePathWithPath (path, intersectBottomLeftPath, CombineModeXor, 0, 20, 40, 50, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Both.
	verifyCombinePathWithPath (path, touchingLeftPath, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, touchingLeftScans, sizeof (touchingLeftScans));

	// Path + Touching Top = Both.
	verifyCombinePathWithPath (path, touchingTopPath, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, touchingTopScans, sizeof (touchingTopScans));

	// Path + Touching Right = Both.
	verifyCombinePathWithPath (path, touchingRightPath, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, touchingRightScans, sizeof (touchingRightScans));

	// Path + Touching Bottom = Both.
	verifyCombinePathWithPath (path, touchingBottomPath, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, touchingBottomScans, sizeof (touchingBottomScans));

	// Path + Touching Top Left = Both.
	verifyCombinePathWithPath (path, touchingTopLeftPath, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, touchingTopLeftScans, sizeof (touchingTopLeftScans));

	// Path + Touching Top Right = Both.
	verifyCombinePathWithPath (path, touchingTopRightPath, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, touchingTopRightScans, sizeof (touchingTopRightScans));

	// Path + Touching Bottom Right = Both.
	verifyCombinePathWithPath (path, touchingBottomRightPath, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, touchingBottomRightScans, sizeof (touchingBottomRightScans));

	// Path + Touching Bottom Left = Both.
	verifyCombinePathWithPath (path, touchingBottomLeftPath, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, touchingBottomLeftScans, sizeof (touchingBottomLeftScans));

	// Path + No Intersect Left = Both.
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeXor, -21, 20, 61, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Path + No Intersect Top = Both.
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeXor, 10, -21, 30, 81, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Path + No Intersect Right = Both.
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeXor, 10, 20, 61, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Path + No Intersect Bottom = Both.
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeXor, 10, 20, 30, 81, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeXor, -21, -21, 61, 81, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeXor, 10, -21, 61, 81, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeXor, 10, 20, 61, 81, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeXor, -21, 20, 61, 81, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
#endif

	GdipDeleteRegion (infiniteRegion);
	GdipDeleteRegion (emptyRegion);
	GdipDeleteRegion (rectRegion);
	GdipDeleteRegion (pathRegion);
	GdipDeletePath (path);
	GdipDeletePath (superPath);
	GdipDeletePath (subPath);
	GdipDeletePath (intersectLeftPath);
	GdipDeletePath (intersectTopPath);
	GdipDeletePath (intersectRightPath);
	GdipDeletePath (intersectBottomPath);
	GdipDeletePath (intersectTopLeftPath);
	GdipDeletePath (intersectTopRightPath);
	GdipDeletePath (intersectBottomRightPath);
	GdipDeletePath (intersectBottomLeftPath);
	GdipDeletePath (touchingLeftPath);
	GdipDeletePath (touchingTopPath);
	GdipDeletePath (touchingRightPath);
	GdipDeletePath (touchingBottomPath);
	GdipDeletePath (touchingTopLeftPath);
	GdipDeletePath (touchingTopRightPath);
	GdipDeletePath (touchingBottomRightPath);
	GdipDeletePath (touchingBottomLeftPath);
	GdipDeletePath (noIntersectLeftPath);
	GdipDeletePath (noIntersectTopPath);
	GdipDeletePath (noIntersectRightPath);
	GdipDeletePath (noIntersectBottomPath);
	GdipDeletePath (noIntersectTopLeftPath);
	GdipDeletePath (noIntersectTopRightPath);
	GdipDeletePath (noIntersectBottomRightPath);
	GdipDeletePath (noIntersectBottomLeftPath);
	GdipDeletePath (emptyPath);
	GdipDeletePath (infinitePath);
	GdipDeletePath (negativePath);
}

static void test_combineExclude ()
{
	GpRegion *infiniteRegion;
	GpRegion *emptyRegion;
	GpRegion *rectRegion;
	GpRegion *pathRegion;

	GdipCreateRegion (&infiniteRegion);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	RectF rect = {10, 20, 30, 40};
	RectF superRect = {0, 10, 50, 60};
	RectF subRect = {15, 25, 20, 30};
	RectF intersectLeftRect = {0, 20, 30, 40};
	RectF intersectTopRect = {10, 10, 30, 40};
	RectF intersectRightRect = {20, 20, 30, 40};
	RectF intersectBottomRect = {10, 30, 30, 40};
	RectF intersectTopLeftRect = {0, 10, 30, 40};
	RectF intersectTopRightRect = {20, 10, 30, 40};
	RectF intersectBottomRightRect = {20, 30, 30, 40};
	RectF intersectBottomLeftRect = {0, 30, 30, 40};
	RectF touchingLeftRect = {-20, 20, 30, 40};
	RectF touchingTopRect = {10, -20, 30, 40};
	RectF touchingRightRect = {40, 20, 30, 40};
	RectF touchingBottomRect = {10, 60, 30, 40};
	RectF touchingTopLeftRect = {-20, -20, 30, 40};
	RectF touchingTopRightRect = {40, -20, 30, 40};
	RectF touchingBottomRightRect = {40, 60, 30, 40};
	RectF touchingBottomLeftRect = {-20, 60, 30, 40};
	RectF noIntersectLeftRect = {-21, 20, 30, 40};
	RectF noIntersectTopRect = {10, -21, 30, 40};
	RectF noIntersectRightRect = {41, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 61, 30, 40};
	RectF noIntersectTopLeftRect = {-21, -21, 30, 40};
	RectF noIntersectTopRightRect = {41, -21, 30, 40};
	RectF noIntersectBottomRightRect = {41, 61, 30, 40};
	RectF noIntersectBottomLeftRect = {-21, 61, 30, 40};
	RectF emptyRect = {0, 0, 0, 0};
	RectF infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	RectF negativeRect = {20, 30, -10, -10};
	GdipCreateRegionRect (&rect, &rectRegion);

	GpPath *path = createPathFromRect (&rect);
	GpPath *superPath = createPathFromRect (&superRect);
	GpPath *subPath = createPathFromRect (&subRect);
	GpPath *intersectLeftPath = createPathFromRect (&intersectLeftRect);
	GpPath *intersectTopPath = createPathFromRect (&intersectTopRect);
	GpPath *intersectRightPath = createPathFromRect (&intersectRightRect);
	GpPath *intersectBottomPath = createPathFromRect (&intersectBottomRect);
	GpPath *intersectTopLeftPath = createPathFromRect (&intersectTopLeftRect);
	GpPath *intersectTopRightPath = createPathFromRect (&intersectTopRightRect);
	GpPath *intersectBottomRightPath = createPathFromRect (&intersectBottomRightRect);
	GpPath *intersectBottomLeftPath = createPathFromRect (&intersectBottomLeftRect);
	GpPath *touchingLeftPath = createPathFromRect (&touchingLeftRect);
	GpPath *touchingTopPath = createPathFromRect (&touchingTopRect);
	GpPath *touchingRightPath = createPathFromRect (&touchingRightRect);
	GpPath *touchingBottomPath = createPathFromRect (&touchingBottomRect);
	GpPath *touchingTopLeftPath = createPathFromRect (&touchingTopLeftRect);
	GpPath *touchingTopRightPath = createPathFromRect (&touchingTopRightRect);
	GpPath *touchingBottomRightPath = createPathFromRect (&touchingBottomRightRect);
	GpPath *touchingBottomLeftPath = createPathFromRect (&touchingBottomLeftRect);
	GpPath *noIntersectLeftPath = createPathFromRect (&noIntersectLeftRect);
	GpPath *noIntersectTopPath = createPathFromRect (&noIntersectTopRect);
	GpPath *noIntersectRightPath = createPathFromRect (&noIntersectRightRect);
	GpPath *noIntersectBottomPath = createPathFromRect (&noIntersectBottomRect);
	GpPath *noIntersectTopLeftPath = createPathFromRect (&noIntersectTopLeftRect);
	GpPath *noIntersectTopRightPath = createPathFromRect (&noIntersectTopRightRect);
	GpPath *noIntersectBottomRightPath = createPathFromRect (&noIntersectBottomRightRect);
	GpPath *noIntersectBottomLeftPath = createPathFromRect (&noIntersectBottomLeftRect);
	GpPath *emptyPath = createPathFromRect (&emptyRect);
	GpPath *infinitePath = createPathFromRect (&infiniteRect);
	GpPath *negativePath = createPathFromRect (&negativeRect);

	GdipCreateRegionPath (path, &pathRegion);

	// Infinite + Infinite = Empty
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Empty = Infinite.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Infinite Rect = Empty
	verifyCombineInfiniteWithRect (&infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Empty Rect = Infinite.
	verifyCombineInfiniteWithRect (&emptyRect, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Rect = Not Rect.
	RectF infiniteWithRectScans[] = {
		{-4194304, -4194304, 8388608, 4194324},
		{-4194304, 20, 4194314, 40},
		{40, 20, 4194264, 40},
		{-4194304, 60, 8388608, 4194244}
	};
	verifyCombineInfiniteWithRect (&rect, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
	
	// Infinite + Negative Rect = Infinite.
	{
		GpRegion *region;
		GdipCreateRegion (&region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF infiniteWithNegativeRectScans[] = {
			{-4194304, -4194304, 8388608, 4194324},
			{-4194304, 20, 4194314, 10},
			{20, 20, 4194284, 10},
			{-4194304, 30, 8388608, 4194274}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeExclude);
		verifyRegion (clone, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE);
		verifyRegionScans (clone, infiniteWithNegativeRectScans, sizeof (infiniteWithNegativeRectScans));
		GdipDeleteRegion (clone);
	}
	
	// Infinite + Infinite Path = Empty
	// FIXME: should be empty: https://github.com/mono/libgdiplus/issues/352
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithPath (infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite + Empty Path = Infinite.
	verifyCombineInfiniteWithPath (emptyPath, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Path = Not Path.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/349
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithPath (path, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty + Infinite = Empty.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite Rect = Empty.
	verifyCombineEmptyWithRect (&infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty Rect = Empty.
	verifyCombineEmptyWithRect (&emptyRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite Path = Empty.
	verifyCombineEmptyWithPath (infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty Path = Empty.
	verifyCombineEmptyWithPath (emptyPath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Infinite = Empty.
	verifyCombineRectWithRegion (&infiniteRect, infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Empty = Infinite.
	verifyCombineRectWithRegion (&infiniteRect, emptyRegion, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Infinite Rect = Empty.
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Empty Rect = Infinite.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Rect = Not Rect.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Infinite Rect + Negative Rect = Infinite.
	{
		GpRegion *region;
		GdipCreateRegionRect (&infiniteRect, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF infiniteWithNegativeRectScans[] = {
			{-4194304, -4194304, 8388608, 4194324},
			{-4194304, 20, 4194314, 10},
			{20, 20, 4194284, 10},
			{-4194304, 30, 8388608, 4194274}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeExclude);
		verifyRegion (clone, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE);
		verifyRegionScans (clone, infiniteWithNegativeRectScans, sizeof (infiniteWithNegativeRectScans));
		GdipDeleteRegion (clone);
	}
	
	// Infinite Rect + Infinite Path = Empty.
	// FIXME: should be empty: https://github.com/mono/libgdiplus/issues/352
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&infiniteRect, infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite Rect + Empty Path = Infinite.
	verifyCombineRectWithPath (&infiniteRect, emptyPath, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Path = Not Path.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&infiniteRect, path, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty Rect + Infinite = Empty.
	verifyCombineRectWithRegion (&emptyRect, infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Empty = Empty.
	verifyCombineRectWithRegion (&emptyRect, emptyRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Path = Empty.
	verifyCombineRectWithPath (&emptyRect, infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&emptyRect, emptyPath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Path = Empty.
	verifyCombineRectWithPath (&emptyRect, path, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Infinite = Empty.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Empty = Rect.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Rect = Empty.
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Empty Rect = Rect.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Rect = Rect.
	{
		GpRegion *region;
		GdipCreateRegionRect (&rect, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF rectWithNegativeRectScans[] = {
			{20, 20, 20, 10},
			{10, 30, 30, 30}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeXor);
		verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
		verifyRegionScans (clone, rectWithNegativeRectScans, sizeof (rectWithNegativeRectScans));
		GdipDeleteRegion (clone);
	}

	// Rect + Equal Rect = Empty.
	verifyCombineRectWithRect (&rect, &rect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Super Rect = Empty.
	verifyCombineRectWithRect (&rect, &superRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Sub Rect = Not Sub Rect and Rect.
	RectF subScans[] = {
		{10, 20, 30, 5},
		{10, 25, 5, 30},
		{35, 25, 5, 30},
		{10, 55, 30, 5}
	};
	verifyCombineRectWithRect (&rect, &subRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));

	// Rect + Intersect Left = Rect and Not Intersect Left.
	RectF intersectLeftScan = {30, 20, 10, 40};
	verifyCombineRectWithRect (&rect, &intersectLeftRect, CombineModeExclude, 30, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Rect and Not Intersect Top.
	RectF intersectTopScan = {10, 50, 30, 10};
	verifyCombineRectWithRect (&rect, &intersectTopRect, CombineModeExclude, 10, 50, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Rect and Not Intersect Right.
	RectF intersectRightScan = {10, 20, 10, 40};
	verifyCombineRectWithRect (&rect, &intersectRightRect, CombineModeExclude, 10, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Rect and Not Intersect Bottom.
	RectF intersectBottomScan = {10, 20, 30, 10};
	verifyCombineRectWithRect (&rect, &intersectBottomRect, CombineModeExclude, 10, 20, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Rect and Not Intersect Top Left.
	RectF intersectTopLeftScans[] = {
		{30, 20, 10, 30},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Rect and Not Intersect Top Right.
	RectF intersectTopRightScans[] = {
		{10, 20, 10, 30},
		{10, 50, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectTopRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Rect and Not Intersect Bottom Right.
	RectF intersectBottomRightScans[] = {
		{10, 20, 30, 10},
		{10, 30, 10, 30}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Rect and Not Intersect Bottom Left.
	RectF intersectBottomLeftScans[] = {
		{10, 20, 30, 10},
		{30, 30, 10, 30}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Rect.
	verifyCombineRectWithRect (&rect, &touchingLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Top = Rect.
	verifyCombineRectWithRect (&rect, &touchingTopRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Right = Rect.
	verifyCombineRectWithRect (&rect, &touchingRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Bottom = Rect.
	verifyCombineRectWithRect (&rect, &touchingBottomRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Top Left = Rect.
	verifyCombineRectWithRect (&rect, &touchingTopLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Top Right = Rect.
	// FIXME: should not be empty: https://github.com/mono/libgdiplus/issues/351
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&rect, &touchingTopRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Rect + Touching Bottom Right = Rect.
	verifyCombineRectWithRect (&rect, &touchingBottomRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Bottom Left = Rect.
	verifyCombineRectWithRect (&rect, &touchingBottomLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Left = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Top = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Right = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Bottom = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Top Left = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Top Right = Rect.
	// FIXME: incorrectly empty: 
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Rect + No Intersect Bottom Right = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Bottom Left = Rect.
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Path = Empty.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Rect + Empty Path = Rect.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Path = Rect.
	{
		GpRegion *region;
		GdipCreateRegionRect (&rect, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a path region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual path. */
		RectF rectWithNegativePathScans[] = {
			{20, 20, 20, 10},
			{10, 30, 30, 30}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeExclude);
		verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
		verifyRegionScans (clone, rectWithNegativePathScans, sizeof (rectWithNegativePathScans));
		GdipDeleteRegion (clone);
	}

	// Rect + Equal Path = Empty.
	verifyCombineRectWithPath (&rect, path, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Super Path = Empty.
	verifyCombineRectWithPath (&rect, superPath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Sub Path = Rect and Not Sub Path.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/354
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, subPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));
#endif

	// Rect + Intersect Left = Rect and Not Intersect Left.
	verifyCombineRectWithPath (&rect, intersectLeftPath, CombineModeExclude, 30, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Rect and Not Intersect Top.
	verifyCombineRectWithPath (&rect, intersectTopPath, CombineModeExclude, 10, 50, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Rect and Not Intersect Right.
	verifyCombineRectWithPath (&rect, intersectRightPath, CombineModeExclude, 10, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Rect and Not Intersect Bottom.
	verifyCombineRectWithPath (&rect, intersectBottomPath, CombineModeExclude, 10, 20, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Rect and Not Intersect Top Left.
	verifyCombineRectWithPath (&rect, intersectTopLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Rect and Not Intersect Top Right.
	verifyCombineRectWithPath (&rect, intersectTopRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Rect and Not Intersect Bottom Right.
	verifyCombineRectWithPath (&rect, intersectBottomRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Rect and Not Intersect Bottom Left.
	verifyCombineRectWithPath (&rect, intersectBottomLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Rect.
	verifyCombineRectWithPath (&rect, touchingLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Top = Rect.
	verifyCombineRectWithPath (&rect, touchingTopPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Right = Rect.
	verifyCombineRectWithPath (&rect, touchingRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Bottom = Rect.
	verifyCombineRectWithPath (&rect, touchingBottomPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Top Left = Rect.
	verifyCombineRectWithPath (&rect, touchingTopLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Top Right = Rect.
	verifyCombineRectWithPath (&rect, touchingTopRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Bottom Right = Rect.
	verifyCombineRectWithPath (&rect, touchingBottomRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Touching Bottom Left = Rect.
	verifyCombineRectWithPath (&rect, touchingBottomLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Left = Rect.
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Top = Rect.
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Right = Rect.
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Bottom = Rect.
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Top Left = Rect.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Top Right = Rect.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Bottom Right = Rect.
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + No Intersect Bottom Left = Rect.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	// Infinite Path + Infinite = Empty.
	verifyCombinePathWithRegion (infinitePath, infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty = Infinite.
	verifyCombinePathWithRegion (infinitePath, emptyRegion, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Infinite Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Rect = Infinite.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Rect = Not Rect.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Infinite Path + Infinite Path = Empty.
	verifyCombinePathWithPath (infinitePath, infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Path = Infinite.
	verifyCombinePathWithPath (infinitePath, emptyPath, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Path = Not Path.
	verifyCombinePathWithPath (infinitePath, path, CombineModeExclude, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty Path + Infinite = Empty.
	verifyCombinePathWithRegion (emptyPath, infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Empty = Empty.
	verifyCombinePathWithRegion (emptyPath, emptyRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Empty Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Path = Empty.
	verifyCombinePathWithPath (emptyPath, infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Empty Path = Empty.
	verifyCombinePathWithPath (emptyPath, emptyPath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Path = Empty.
	verifyCombinePathWithPath (emptyPath, path, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Infinite = Empty.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Path + Empty = Rect.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite Rect = Empty.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Path + Empty Rect = Path.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Rect = Path.
	{
		GpRegion *region;
		GdipCreateRegionPath (path, &region);
		GpRegion *clone;
		GdipCloneRegion (region, &clone);

		/* First, test combining with a rect region. */
		GpRegion *region2;
		GdipCreateRegionRect (&negativeRect, &region2);
		verifyCombineRegionWithRegion (region, region2, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
		GdipDeleteRegion (region2);

		/* Second, test combining with an actual rect. */
		RectF rectWithNegativeRectScans[] = {
			{20, 20, 20, 10},
			{10, 30, 30, 30}
		};
		GdipCombineRegionRect (clone, &negativeRect, CombineModeExclude);
		verifyRegion (clone, 10, 20, 30, 40, FALSE, FALSE);
		verifyRegionScans (clone, rectWithNegativeRectScans, sizeof (rectWithNegativeRectScans));
		GdipDeleteRegion (clone);
	}

	// Path + Equal Rect = Empty.
	verifyCombinePathWithRect (path, &rect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Super Rect = Empty.
	verifyCombinePathWithRect (path, &superRect, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Sub Rect = Path and Not Sub Rect.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/354
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &subRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));
#endif

	// Path + Intersect Left = Path and Not Intersect Left.
	verifyCombinePathWithRect (path, &intersectLeftRect, CombineModeExclude, 30, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Path and Not Intersect Top.
	verifyCombinePathWithRect (path, &intersectTopRect, CombineModeExclude, 10, 50, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Path and Not Intersect Right.
	verifyCombinePathWithRect (path, &intersectRightRect, CombineModeExclude, 10, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Path and Not Intersect Bottom.
	verifyCombinePathWithRect (path, &intersectBottomRect, CombineModeExclude, 10, 20, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Path and Not Intersect Top Left.
	verifyCombinePathWithRect (path, &intersectTopLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Path and Not Intersect Top Right.
	verifyCombinePathWithRect (path, &intersectTopRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Path and Not Intersect Bottom Right.
	verifyCombinePathWithRect (path, &intersectBottomRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Path and Not BottomLeft..
	verifyCombinePathWithRect (path, &intersectBottomLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Path.
	verifyCombinePathWithRect (path, &touchingLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Top = Path.
	verifyCombinePathWithRect (path, &touchingTopRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Right = Path.
	verifyCombinePathWithRect (path, &touchingRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Bottom = Path.
	verifyCombinePathWithRect (path, &touchingBottomRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Top Left = Path.
	verifyCombinePathWithRect (path, &touchingTopLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Top Right = Path.
	verifyCombinePathWithRect (path, &touchingTopRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Bottom Right = Path.
	verifyCombinePathWithRect (path, &touchingBottomRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Bottom Left = Path.
	verifyCombinePathWithRect (path, &touchingBottomLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Left = Path.
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Top = Path.
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Right = Path.
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Bottom = Path.
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Top Left = Path.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Top Right = Path.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Bottom Right = Path.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Bottom Left = Path.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite Path = Empty.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, infinitePath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Path + Empty Path = Path.
	verifyCombinePathWithPath (path, emptyPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Path = Path.
	verifyCombinePathWithPath (path, negativePath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Equal Path = Empty.
	verifyCombinePathWithPath (path, path, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Super Path = Empty.
	verifyCombinePathWithPath (path, superPath, CombineModeExclude, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Sub Path = Path and Not Sub Path.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/354
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, subPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, subScans, sizeof (subScans));
#endif

	// Path + Intersect Left = Path and Not Intersect Left.
	verifyCombinePathWithPath (path, intersectLeftPath, CombineModeExclude, 30, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Path and Not Intersect Top.
	verifyCombinePathWithPath (path, intersectTopPath, CombineModeExclude, 10, 50, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Path and Not Intersect Right.
	verifyCombinePathWithPath (path, intersectRightPath, CombineModeExclude, 10, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Path and Not Intersect Bottom.
	verifyCombinePathWithPath (path, intersectBottomPath, CombineModeExclude, 10, 20, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Path and Not Intersect Top Left.
	verifyCombinePathWithPath (path, intersectTopLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Path and Not Intersect Top Right.
	verifyCombinePathWithPath (path, intersectTopRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Path and Not Intersect Bottom Right.
	verifyCombinePathWithPath (path, intersectBottomRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Path and Not Intersect Bottom Left.
	verifyCombinePathWithPath (path, intersectBottomLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Path.
	verifyCombinePathWithPath (path, touchingLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Top = Path.
	verifyCombinePathWithPath (path, touchingTopPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Right = Path.
	verifyCombinePathWithPath (path, touchingRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Bottom = Path.
	verifyCombinePathWithPath (path, touchingBottomPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Top Left = Path.
	verifyCombinePathWithPath (path, touchingTopLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Top Right = Path.
	verifyCombinePathWithPath (path, touchingTopRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Bottom Right = Path.
	verifyCombinePathWithPath (path, touchingBottomRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Touching Bottom Left = Path.
	verifyCombinePathWithPath (path, touchingBottomLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Left = Path.
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Top = Path.
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Right = Path.
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Bottom = Path.
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Top Left = Path.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Top Right = Path.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Bottom Right = Path.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + No Intersect Bottom Left = Path.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeExclude, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	GdipDeleteRegion (infiniteRegion);
	GdipDeleteRegion (emptyRegion);
	GdipDeleteRegion (rectRegion);
	GdipDeleteRegion (pathRegion);
	GdipDeletePath (path);
	GdipDeletePath (superPath);
	GdipDeletePath (subPath);
	GdipDeletePath (intersectLeftPath);
	GdipDeletePath (intersectTopPath);
	GdipDeletePath (intersectRightPath);
	GdipDeletePath (intersectBottomPath);
	GdipDeletePath (intersectTopLeftPath);
	GdipDeletePath (intersectTopRightPath);
	GdipDeletePath (intersectBottomRightPath);
	GdipDeletePath (intersectBottomLeftPath);
	GdipDeletePath (touchingLeftPath);
	GdipDeletePath (touchingTopPath);
	GdipDeletePath (touchingRightPath);
	GdipDeletePath (touchingBottomPath);
	GdipDeletePath (touchingTopLeftPath);
	GdipDeletePath (touchingTopRightPath);
	GdipDeletePath (touchingBottomRightPath);
	GdipDeletePath (touchingBottomLeftPath);
	GdipDeletePath (noIntersectLeftPath);
	GdipDeletePath (noIntersectTopPath);
	GdipDeletePath (noIntersectRightPath);
	GdipDeletePath (noIntersectBottomPath);
	GdipDeletePath (noIntersectTopLeftPath);
	GdipDeletePath (noIntersectTopRightPath);
	GdipDeletePath (noIntersectBottomRightPath);
	GdipDeletePath (noIntersectBottomLeftPath);
	GdipDeletePath (emptyPath);
	GdipDeletePath (infinitePath);
	GdipDeletePath (negativePath);
}

static void test_combineComplement ()
{
	GpRegion *infiniteRegion;
	GpRegion *emptyRegion;
	GpRegion *rectRegion;
	GpRegion *pathRegion;

	GdipCreateRegion (&infiniteRegion);

	GdipCreateRegion (&emptyRegion);
	GdipSetEmpty (emptyRegion);

	RectF rect = {10, 20, 30, 40};
	RectF superRect = {0, 10, 50, 60};
	RectF subRect = {15, 25, 20, 30};
	RectF intersectLeftRect = {0, 20, 30, 40};
	RectF intersectTopRect = {10, 10, 30, 40};
	RectF intersectRightRect = {20, 20, 30, 40};
	RectF intersectBottomRect = {10, 30, 30, 40};
	RectF intersectTopLeftRect = {0, 10, 30, 40};
	RectF intersectTopRightRect = {20, 10, 30, 40};
	RectF intersectBottomRightRect = {20, 30, 30, 40};
	RectF intersectBottomLeftRect = {0, 30, 30, 40};
	RectF touchingLeftRect = {-20, 20, 30, 40};
	RectF touchingTopRect = {10, -20, 30, 40};
	RectF touchingRightRect = {40, 20, 30, 40};
	RectF touchingBottomRect = {10, 60, 30, 40};
	RectF touchingTopLeftRect = {-20, -20, 30, 40};
	RectF touchingTopRightRect = {40, -20, 30, 40};
	RectF touchingBottomRightRect = {40, 60, 30, 40};
	RectF touchingBottomLeftRect = {-20, 60, 30, 40};
	RectF noIntersectLeftRect = {-21, 20, 30, 40};
	RectF noIntersectTopRect = {10, -21, 30, 40};
	RectF noIntersectRightRect = {41, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 61, 30, 40};
	RectF noIntersectTopLeftRect = {-21, -21, 30, 40};
	RectF noIntersectTopRightRect = {41, -21, 30, 40};
	RectF noIntersectBottomRightRect = {41, 61, 30, 40};
	RectF noIntersectBottomLeftRect = {-21, 61, 30, 40};
	RectF emptyRect = {0, 0, 0, 0};
	RectF infiniteRect = {-4194304, -4194304, 8388608, 8388608};
	RectF negativeRect = {20, 30, -10, -10};
	GdipCreateRegionRect (&rect, &rectRegion);

	GpPath *path = createPathFromRect (&rect);
	GpPath *superPath = createPathFromRect (&superRect);
	GpPath *subPath = createPathFromRect (&subRect);
	GpPath *intersectLeftPath = createPathFromRect (&intersectLeftRect);
	GpPath *intersectTopPath = createPathFromRect (&intersectTopRect);
	GpPath *intersectRightPath = createPathFromRect (&intersectRightRect);
	GpPath *intersectBottomPath = createPathFromRect (&intersectBottomRect);
	GpPath *intersectTopLeftPath = createPathFromRect (&intersectTopLeftRect);
	GpPath *intersectTopRightPath = createPathFromRect (&intersectTopRightRect);
	GpPath *intersectBottomRightPath = createPathFromRect (&intersectBottomRightRect);
	GpPath *intersectBottomLeftPath = createPathFromRect (&intersectBottomLeftRect);
	GpPath *touchingLeftPath = createPathFromRect (&touchingLeftRect);
	GpPath *touchingTopPath = createPathFromRect (&touchingTopRect);
	GpPath *touchingRightPath = createPathFromRect (&touchingRightRect);
	GpPath *touchingBottomPath = createPathFromRect (&touchingBottomRect);
	GpPath *touchingTopLeftPath = createPathFromRect (&touchingTopLeftRect);
	GpPath *touchingTopRightPath = createPathFromRect (&touchingTopRightRect);
	GpPath *touchingBottomRightPath = createPathFromRect (&touchingBottomRightRect);
	GpPath *touchingBottomLeftPath = createPathFromRect (&touchingBottomLeftRect);
	GpPath *noIntersectLeftPath = createPathFromRect (&noIntersectLeftRect);
	GpPath *noIntersectTopPath = createPathFromRect (&noIntersectTopRect);
	GpPath *noIntersectRightPath = createPathFromRect (&noIntersectRightRect);
	GpPath *noIntersectBottomPath = createPathFromRect (&noIntersectBottomRect);
	GpPath *noIntersectTopLeftPath = createPathFromRect (&noIntersectTopLeftRect);
	GpPath *noIntersectTopRightPath = createPathFromRect (&noIntersectTopRightRect);
	GpPath *noIntersectBottomRightPath = createPathFromRect (&noIntersectBottomRightRect);
	GpPath *noIntersectBottomLeftPath = createPathFromRect (&noIntersectBottomLeftRect);
	GpPath *emptyPath = createPathFromRect (&emptyRect);
	GpPath *infinitePath = createPathFromRect (&infiniteRect);
	GpPath *negativePath = createPathFromRect (&negativeRect);

	GdipCreateRegionPath (path, &pathRegion);

	// Infinite + Infinite = Empty
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Empty = Empty.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Infinite Rect = Empty
	verifyCombineInfiniteWithRect (&infiniteRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Empty Rect = Empty.
	verifyCombineInfiniteWithRect (&emptyRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Rect = Empty.
	verifyCombineInfiniteWithRect (&rect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Negative Rect = Empty.
	verifyCombineInfiniteWithRect (&negativeRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Infinite Path = Empty
	verifyCombineInfiniteWithPath (infinitePath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Empty Path = Empty.
	verifyCombineInfiniteWithPath (emptyPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Path = Empty
	verifyCombineInfiniteWithPath (path, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite = Empty.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Infinite Rect = Empty.
	verifyCombineEmptyWithRect (&infiniteRect, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty Rect = Empty.
	verifyCombineEmptyWithRect (&emptyRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeComplement, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
	
	// Empty + Infinite Path = Empty.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/414
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineEmptyWithPath (infinitePath, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty + Empty Path = Empty.
	verifyCombineEmptyWithPath (emptyPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeComplement, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Infinite Rect + Infinite = Empty.
	verifyCombineRectWithRegion (&infiniteRect, infiniteRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Empty = Empty.
	verifyCombineRectWithRegion (&infiniteRect, emptyRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Infinite Rect = Empty.
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Rect = Empty.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Infinite Path = Empty.
	verifyCombineRectWithPath (&infiniteRect, infinitePath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&infiniteRect, emptyPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Path = Empty.
	verifyCombineRectWithPath (&infiniteRect, path, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
	
	// Empty Rect + Infinite = Infinite.
	verifyCombineRectWithRegion (&emptyRect, infiniteRegion, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Empty = Empty.
	verifyCombineRectWithRegion (&emptyRect, emptyRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Infinite Rect = Infinite.
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Rect = Rect.
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeComplement, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Rect + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/414
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&emptyRect, infinitePath, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&emptyRect, emptyPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Path = Path.
	verifyCombineRectWithPath (&emptyRect, path, CombineModeComplement, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite = Empty.
	RectF rectWithInfiniteScans[] = {
		{-4194304, -4194304, 8388608, 4194324},
		{-4194304, 20, 4194314, 40},
		{40, 20, 4194264, 40},
		{-4194304, 60, 8388608, 4194244}
	};
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, rectWithInfiniteScans, sizeof (rectWithInfiniteScans));

	// Rect + Empty = Empty.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeComplement, 0, 0,0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Infinite Rect = Empty.
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, rectWithInfiniteScans, sizeof (rectWithInfiniteScans));

	// Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeComplement, 0, 0,0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Negative Rect = Empty.
	verifyCombineRectWithRect (&rect, &negativeRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Equal Rect = Empty.
	verifyCombineRectWithRect (&rect, &rect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Super Rect = Not Rect and Super Rect.
	RectF superScans[] = {
		{0, 10, 50, 10},
		{0, 20, 10, 40},
		{40, 20, 10, 40},
		{0, 60, 50, 10}
	};
	verifyCombineRectWithRect (&rect, &superRect, CombineModeComplement, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));

	// Rect + Sub Rect = Empty
	verifyCombineRectWithRect (&rect, &subRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Intersect Left = Not Rect and Intersect Left.
	RectF intersectLeftScan = {0, 20, 10, 40};
	verifyCombineRectWithRect (&rect, &intersectLeftRect, CombineModeComplement, 0, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Not Rect and Intersect Top.
	RectF intersectTopScan = {10, 10, 30, 10};
	verifyCombineRectWithRect (&rect, &intersectTopRect, CombineModeComplement, 10, 10, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Not Rect and Intersect Right.
	RectF intersectRightScan = {40, 20, 10, 40};
	verifyCombineRectWithRect (&rect, &intersectRightRect, CombineModeComplement, 40, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Not Rect and Intersect Bottom.
	RectF intersectBottomScan = {10, 60, 30, 10};
	verifyCombineRectWithRect (&rect, &intersectBottomRect, CombineModeComplement, 10, 60, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Not Rect and Intersect Top Left.
	RectF intersectTopLeftScans[] = {
		{0, 10, 30, 10},
		{0, 20, 10, 30}
	};
	verifyCombineRectWithRect (&rect, &intersectTopLeftRect, CombineModeComplement, 0, 10, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Not Rect and Intersect Top Right.
	RectF intersectTopRightScans[] = {
		{20, 10, 30, 10},
		{40, 20, 10, 30}
	};
	verifyCombineRectWithRect (&rect, &intersectTopRightRect, CombineModeComplement, 20, 10, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Not Rect and Intersect Bottom Right.
	RectF intersectBottomRightScans[] = {
		{40, 30, 10, 30},
		{20, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomRightRect, CombineModeComplement, 20, 30, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Not Rect and Intersect Bottom Left.
	RectF intersectBottomLeftScans[] = {
		{0, 30, 10, 30},
		{0, 60, 30, 10}
	};
	verifyCombineRectWithRect (&rect, &intersectBottomLeftRect, CombineModeComplement, 0, 30, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Touching Left.
	verifyCombineRectWithRect (&rect, &touchingLeftRect, CombineModeComplement, -20, 20, 30, 40, FALSE, FALSE, &touchingLeftRect, sizeof (touchingLeftRect));

	// Rect + Touching Top = Touching Top.
	verifyCombineRectWithRect (&rect, &touchingTopRect, CombineModeComplement, 10, -20, 30, 40, FALSE, FALSE, &touchingTopRect, sizeof (touchingTopRect));

	// Rect + Touching Right = Touching Right.
	verifyCombineRectWithRect (&rect, &touchingRightRect, CombineModeComplement, 40, 20, 30, 40, FALSE, FALSE, &touchingRightRect, sizeof (touchingRightRect));

	// Rect + Touching Bottom = Touching Bottom.
	verifyCombineRectWithRect (&rect, &touchingBottomRect, CombineModeComplement, 10, 60, 30, 40, FALSE, FALSE, &touchingBottomRect, sizeof (touchingBottomRect));

	// Rect + Touching Top Left = Touching Top Left.
	verifyCombineRectWithRect (&rect, &touchingTopLeftRect, CombineModeComplement, -20, -20, 30, 40, FALSE, FALSE, &touchingTopLeftRect, sizeof (touchingTopLeftRect));

	// Rect + Touching Top Right = Touching Top Right.
	verifyCombineRectWithRect (&rect, &touchingTopRightRect, CombineModeComplement, 40, -20, 30, 40, FALSE, FALSE, &touchingTopRightRect, sizeof (touchingTopRightRect));

	// Rect + Touching Bottom Right = Touching Bottom Right.
	verifyCombineRectWithRect (&rect, &touchingBottomRightRect, CombineModeComplement, 40, 60, 30, 40, FALSE, FALSE, &touchingBottomRightRect, sizeof (touchingBottomRightRect));

	// Rect + Touching Bottom Left = Touching Bottom Left.
	// FIXME: should be touchingBottomLeftRect: https://github.com/mono/libgdiplus/issues/357
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&rect, &touchingBottomLeftRect, CombineModeComplement, -20, 60, 30, 40, FALSE, FALSE, &touchingBottomLeftRect, sizeof (touchingBottomLeftRect));
#endif

	// Rect + No Intersect Left = No Intersect Left.
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeComplement, -21, 20, 30, 40, FALSE, FALSE, &noIntersectLeftRect, sizeof (noIntersectLeftRect));

	// Rect + No Intersect Top = No Intersect Top.
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeComplement, 10, -21, 30, 40, FALSE, FALSE, &noIntersectTopRect, sizeof (noIntersectTopRect));

	// Rect + No Intersect Right = No Intersect Right.
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeComplement, 41, 20, 30, 40, FALSE, FALSE, &noIntersectRightRect, sizeof (noIntersectRightRect));

	// Rect + No Intersect Bottom = No Intersect Bottom.
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeComplement, 10, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRect, sizeof (noIntersectBottomRect));

	// Rect + No Intersect Top Left = No Intersect Top Left.
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeComplement, -21, -21, 30, 40, FALSE, FALSE, &noIntersectTopLeftRect, sizeof (noIntersectTopLeftRect));

	// Rect + No Intersect Top Right = No Intersect Top Right.
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeComplement, 41, -21, 30, 40, FALSE, FALSE, &noIntersectTopRightRect, sizeof (noIntersectTopRightRect));

	// Rect + No Intersect Bottom Right = No Intersect Bottom Right.
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeComplement, 41, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRightRect, sizeof (noIntersectBottomRightRect));

	// Rect + No Intersect Bottom Left = No Intersect Bottom Left.
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeComplement, -21, 61, 30, 40, FALSE, FALSE, &noIntersectBottomLeftRect, sizeof (noIntersectBottomLeftRect));

  	// Rect + Infinite Path = Infinite.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, rectWithInfiniteScans, sizeof (rectWithInfiniteScans));
#endif

	// Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeComplement, 0, 0,0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Negative Path = Empty.
	verifyCombineRectWithPath (&rect, negativePath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Equal Path = Empty.
	verifyCombineRectWithPath (&rect, path, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Super Path = Not Rect and Super Path.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/358
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, superPath, CombineModeComplement, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));
#endif

	// Rect + Sub Path = Empty
	verifyCombineRectWithPath (&rect, subPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Intersect Left = Not Rect and Intersect Left.
	verifyCombineRectWithPath (&rect, intersectLeftPath, CombineModeComplement, 0, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Rect + Intersect Top = Not Rect and Intersect Top.
	verifyCombineRectWithPath (&rect, intersectTopPath, CombineModeComplement, 10, 10, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Rect + Intersect Right = Not Rect and Intersect Right.
	verifyCombineRectWithPath (&rect, intersectRightPath, CombineModeComplement, 40, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Not Rect and Intersect Bottom.
	verifyCombineRectWithPath (&rect, intersectBottomPath, CombineModeComplement, 10, 60, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Rect + Intersect Top Left = Not Rect and Intersect Top Left.
	verifyCombineRectWithPath (&rect, intersectTopLeftPath, CombineModeComplement, 0, 10, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Rect + Intersect Top Right = Not Rect and Intersect Top Right.
	verifyCombineRectWithPath (&rect, intersectTopRightPath, CombineModeComplement, 20, 10, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Right = Not Rect and Intersect Bottom Right.
	verifyCombineRectWithPath (&rect, intersectBottomRightPath, CombineModeComplement, 20, 30, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Rect + Intersect Bottom Left = Not Rect and Intersect Bottom Left.
	verifyCombineRectWithPath (&rect, intersectBottomLeftPath, CombineModeComplement, 0, 30, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Rect + Touching Left = Touching Left.
	verifyCombineRectWithPath (&rect, touchingLeftPath, CombineModeComplement, -20, 20, 30, 40, FALSE, FALSE, &touchingLeftRect, sizeof (touchingLeftRect));

	// Rect + Touching Top = Touching Top.
	verifyCombineRectWithPath (&rect, touchingTopPath, CombineModeComplement, 10, -20, 30, 40, FALSE, FALSE, &touchingTopRect, sizeof (touchingTopRect));

	// Rect + Touching Right = Touching Right.
	verifyCombineRectWithPath (&rect, touchingRightPath, CombineModeComplement, 40, 20, 30, 40, FALSE, FALSE, &touchingRightRect, sizeof (touchingRightRect));

	// Rect + Touching Bottom = Touching Bottom.
	verifyCombineRectWithPath (&rect, touchingBottomPath, CombineModeComplement, 10, 60, 30, 40, FALSE, FALSE, &touchingBottomRect, sizeof (touchingBottomRect));

	// Rect + Touching Top Left = Touching Top Left.
	verifyCombineRectWithPath (&rect, touchingTopLeftPath, CombineModeComplement, -20, -20, 30, 40, FALSE, FALSE, &touchingTopLeftRect, sizeof (touchingTopLeftRect));

	// Rect + Touching Top Right = Touching Top Right.
	verifyCombineRectWithPath (&rect, touchingTopRightPath, CombineModeComplement, 40, -20, 30, 40, FALSE, FALSE, &touchingTopRightRect, sizeof (touchingTopRightRect));

	// Rect + Touching Bottom Right = Touching Bottom Right.
	verifyCombineRectWithPath (&rect, touchingBottomRightPath, CombineModeComplement, 40, 60, 30, 40, FALSE, FALSE, &touchingBottomRightRect, sizeof (touchingBottomRightRect));

	// Rect + Touching Bottom Left = Touching Bottom Left.
	verifyCombineRectWithPath (&rect, touchingBottomLeftPath, CombineModeComplement, -20, 60, 30, 40, FALSE, FALSE, &touchingBottomLeftRect, sizeof (touchingBottomLeftRect));
		
	// Rect + No Intersect Left = No Intersect Left.
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeComplement, -21, 20, 30, 40, FALSE, FALSE, &noIntersectLeftRect, sizeof (noIntersectLeftRect));

	// Rect + No Intersect Top = No Intersect Top.
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeComplement, 10, -21, 30, 40, FALSE, FALSE, &noIntersectTopRect, sizeof (noIntersectTopRect));

	// Rect + No Intersect Right = No Intersect Right.
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeComplement, 41, 20, 30, 40, FALSE, FALSE, &noIntersectRightRect, sizeof (noIntersectRightRect));

	// Rect + No Intersect Bottom = No Intersect Bottom.
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeComplement, 10, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRect, sizeof (noIntersectBottomRect));

	// Rect + No Intersect Top Left = No Intersect Top Left.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeComplement, -21, -21, 30, 40, FALSE, FALSE, &noIntersectTopLeftRect, sizeof (noIntersectTopLeftRect));

	// Rect + No Intersect Top Right = No Intersect Top Right.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeComplement, 41, -21, 30, 40, FALSE, FALSE, &noIntersectTopRightRect, sizeof (noIntersectTopRightRect));

	// Rect + No Intersect Bottom Right = No Intersect Bottom Right.
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeComplement, 41, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRightRect, sizeof (noIntersectBottomRightRect));

	// Rect + No Intersect Bottom Left = No Intersect Bottom Left.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeComplement, -21, 61, 30, 40, FALSE, FALSE, &noIntersectBottomLeftRect, sizeof (noIntersectBottomLeftRect));
		
	// Path + Infinite = Infinite.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, rectWithInfiniteScans, sizeof (rectWithInfiniteScans));
#endif

	// Path + Empty = Rect.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeComplement, 0, 0,0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Infinite = Empty.
	verifyCombinePathWithRegion (infinitePath, infiniteRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty = Empty.
	verifyCombinePathWithRegion (infinitePath, emptyRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Infinite Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Infinite Path = Empty.
	verifyCombinePathWithPath (infinitePath, infinitePath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Path = Empty.
	verifyCombinePathWithPath (infinitePath, emptyPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Path = Empty.
	verifyCombinePathWithPath (infinitePath, path, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite = Infinite.
	verifyCombinePathWithRegion (emptyPath, infiniteRegion, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Empty = Empty.
	verifyCombinePathWithRegion (emptyPath, emptyRegion, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Infinite Rect = Infinite.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Empty Rect = Empty.
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Rect = Rect.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeComplement, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Path + Infinite Path = Infinite.
	// FIXME: should have scans: https://github.com/mono/libgdiplus/issues/414
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (emptyPath, infinitePath, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Path + Empty Path = Empty.
	verifyCombinePathWithPath (emptyPath, emptyPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Path + Path = Path.
	verifyCombinePathWithPath (emptyPath, path, CombineModeComplement, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

  // Path + Infinite Rect = Empty.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, rectWithInfiniteScans, sizeof (rectWithInfiniteScans));
#endif

	// Path + Empty Rect = Empty.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeComplement, 0, 0,0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Negative Rect = Empty.
	verifyCombinePathWithRect (path, &negativeRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Equal Rect = Empty.
	verifyCombinePathWithRect (path, &rect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Super Rect = Not Rect and Super Rect.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/358
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &superRect, CombineModeComplement, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));
#endif

	// Path + Sub Rect = Empty.
	verifyCombinePathWithRect (path, &subRect, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Intersect Left = Not Path and Intersect Left.
	verifyCombinePathWithRect (path, &intersectLeftRect, CombineModeComplement, 0, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Not Path and Intersect Top.
	verifyCombinePathWithRect (path, &intersectTopRect, CombineModeComplement, 10, 10, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Not Path and Intersect Right.
	verifyCombinePathWithRect (path, &intersectRightRect, CombineModeComplement, 40, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Not Path and Intersect Bottom.
	verifyCombinePathWithRect (path, &intersectBottomRect, CombineModeComplement, 10, 60, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Not Path and Intersect Top Left.
	verifyCombinePathWithRect (path, &intersectTopLeftRect, CombineModeComplement, 0, 10, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Not Path and Intersect Top Right.
	verifyCombinePathWithRect (path, &intersectTopRightRect, CombineModeComplement, 20, 10, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Not Path and Intersect Bottom Right.
	verifyCombinePathWithRect (path, &intersectBottomRightRect, CombineModeComplement, 20, 30, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Not Path and Intersect Bottom Left.
	verifyCombinePathWithRect (path, &intersectBottomLeftRect, CombineModeComplement, 0, 30, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Touching Left.
	verifyCombinePathWithRect (path, &touchingLeftRect, CombineModeComplement, -20, 20, 30, 40, FALSE, FALSE, &touchingLeftRect, sizeof (touchingLeftRect));

	// Path + Touching Top = Touching Top.
	verifyCombinePathWithRect (path, &touchingTopRect, CombineModeComplement, 10, -20, 30, 40, FALSE, FALSE, &touchingTopRect, sizeof (touchingTopRect));

	// Path + Touching Right = Touching Right.
	verifyCombinePathWithRect (path, &touchingRightRect, CombineModeComplement, 40, 20, 30, 40, FALSE, FALSE, &touchingRightRect, sizeof (touchingRightRect));

	// Path + Touching Bottom = Touching Bottom.
	verifyCombinePathWithRect (path, &touchingBottomRect, CombineModeComplement, 10, 60, 30, 40, FALSE, FALSE, &touchingBottomRect, sizeof (touchingBottomRect));

	// Path + Touching Top Left = Touching Top Left.
	verifyCombinePathWithRect (path, &touchingTopLeftRect, CombineModeComplement, -20, -20, 30, 40, FALSE, FALSE, &touchingTopLeftRect, sizeof (touchingTopLeftRect));

	// Path + Touching Top Right = Touching Top Right.
	verifyCombinePathWithRect (path, &touchingTopRightRect, CombineModeComplement, 40, -20, 30, 40, FALSE, FALSE, &touchingTopRightRect, sizeof (touchingTopRightRect));

	// Path + Touching Bottom Right = Touching Bottom Right.
	verifyCombinePathWithRect (path, &touchingBottomRightRect, CombineModeComplement, 40, 60, 30, 40, FALSE, FALSE, &touchingBottomRightRect, sizeof (touchingBottomRightRect));

	// Path + Touching Bottom Left = Touching Bottom Left.
	verifyCombinePathWithRect (path, &touchingBottomLeftRect, CombineModeComplement, -20, 60, 30, 40, FALSE, FALSE, &touchingBottomLeftRect, sizeof (touchingBottomLeftRect));

	// Path + No Intersect Left = No Intersect Left.
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeComplement, -21, 20, 30, 40, FALSE, FALSE, &noIntersectLeftRect, sizeof (noIntersectLeftRect));

	// Path + No Intersect Top = No Intersect Top.
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeComplement, 10, -21, 30, 40, FALSE, FALSE, &noIntersectTopRect, sizeof (noIntersectTopRect));

	// Path + No Intersect Right = No Intersect Right.
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeComplement, 41, 20, 30, 40, FALSE, FALSE, &noIntersectRightRect, sizeof (noIntersectRightRect));

	// Path + No Intersect Bottom = No Intersect Bottom.
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeComplement, 10, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRect, sizeof (noIntersectBottomRect));

	// Path + No Intersect Top Left = No Intersect Top Left.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeComplement, -21, -21, 30, 40, FALSE, FALSE, &noIntersectTopLeftRect, sizeof (noIntersectTopLeftRect));

	// Path + No Intersect Top Right = No Intersect Top Right.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeComplement, 41, -21, 30, 40, FALSE, FALSE, &noIntersectTopRightRect, sizeof (noIntersectTopRightRect));

	// Path + No Intersect Bottom Right = No Intersect Bottom Right.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeComplement, 41, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRightRect, sizeof (noIntersectBottomRightRect));

	// Path + No Intersect Bottom Left = No Intersect Bottom Left.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeComplement, -21, 61, 30, 40, FALSE, FALSE, &noIntersectBottomLeftRect, sizeof (noIntersectBottomLeftRect));

  // Path + Infinite Path = Calculate.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, infinitePath, CombineModeComplement, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, rectWithInfiniteScans, sizeof (rectWithInfiniteScans));
#endif

	// Path + Empty Path = Empty.
	verifyCombinePathWithPath (path, emptyPath, CombineModeComplement, 0, 0,0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Negative Path = Empty.
	verifyCombinePathWithPath (path, negativePath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Equal Path = Empty.
	verifyCombinePathWithPath (path, path, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Super Path = Not Path and Super Path.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/358
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, superPath, CombineModeComplement, 0, 10, 50, 60, FALSE, FALSE, superScans, sizeof (superScans));
#endif

	// Path + Sub Path = Empty
	verifyCombinePathWithPath (path, subPath, CombineModeComplement, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Path + Intersect Left = Not Path and Intersect Left.
	verifyCombinePathWithPath (path, intersectLeftPath, CombineModeComplement, 0, 20, 10, 40, FALSE, FALSE, &intersectLeftScan, sizeof (intersectLeftScan));

	// Path + Intersect Top = Not Path and Intersect Top.
	verifyCombinePathWithPath (path, intersectTopPath, CombineModeComplement, 10, 10, 30, 10, FALSE, FALSE, &intersectTopScan, sizeof (intersectTopScan));

	// Path + Intersect Right = Not Path and Intersect Right.
	verifyCombinePathWithPath (path, intersectRightPath, CombineModeComplement, 40, 20, 10, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Path + Intersect Bottom = Not Path and Intersect Bottom.
	verifyCombinePathWithPath (path, intersectBottomPath, CombineModeComplement, 10, 60, 30, 10, FALSE, FALSE, &intersectBottomScan, sizeof (intersectBottomScan));

	// Path + Intersect Top Left = Not Path and Intersect Top Left.
	verifyCombinePathWithPath (path, intersectTopLeftPath, CombineModeComplement, 0, 10, 30, 40, FALSE, FALSE, intersectTopLeftScans, sizeof (intersectTopLeftScans));

	// Path + Intersect Top Right = Not Path and Intersect Top Right.
	verifyCombinePathWithPath (path, intersectTopRightPath, CombineModeComplement, 20, 10, 30, 40, FALSE, FALSE, intersectTopRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Right = Not Path and Intersect Bottom Right.
	verifyCombinePathWithPath (path, intersectBottomRightPath, CombineModeComplement, 20, 30, 30, 40, FALSE, FALSE, intersectBottomRightScans, sizeof (intersectTopRightScans));

	// Path + Intersect Bottom Left = Not Path and Intersect Bottom Left.
	verifyCombinePathWithPath (path, intersectBottomLeftPath, CombineModeComplement, 0, 30, 30, 40, FALSE, FALSE, intersectBottomLeftScans, sizeof (intersectBottomLeftScans));

	// Path + Touching Left = Touching Left.
	verifyCombinePathWithPath (path, touchingLeftPath, CombineModeComplement, -20, 20, 30, 40, FALSE, FALSE, &touchingLeftRect, sizeof (touchingLeftRect));

	// Path + Touching Top = Touching Top.
	verifyCombinePathWithPath (path, touchingTopPath, CombineModeComplement, 10, -20, 30, 40, FALSE, FALSE, &touchingTopRect, sizeof (touchingTopRect));

	// Path + Touching Right = Touching Right.
	verifyCombinePathWithPath (path, touchingRightPath, CombineModeComplement, 40, 20, 30, 40, FALSE, FALSE, &touchingRightRect, sizeof (touchingRightRect));

	// Path + Touching Bottom = Touching Bottom.
	verifyCombinePathWithPath (path, touchingBottomPath, CombineModeComplement, 10, 60, 30, 40, FALSE, FALSE, &touchingBottomRect, sizeof (touchingBottomRect));

	// Path + Touching Top Left = Touching Top Left.
	verifyCombinePathWithPath (path, touchingTopLeftPath, CombineModeComplement, -20, -20, 30, 40, FALSE, FALSE, &touchingTopLeftRect, sizeof (touchingTopLeftRect));

	// Path + Touching Top Right = Touching Top Right.
	verifyCombinePathWithPath (path, touchingTopRightPath, CombineModeComplement, 40, -20, 30, 40, FALSE, FALSE, &touchingTopRightRect, sizeof (touchingTopRightRect));

	// Path + Touching Bottom Right = Touching Bottom Right.
	verifyCombinePathWithPath (path, touchingBottomRightPath, CombineModeComplement, 40, 60, 30, 40, FALSE, FALSE, &touchingBottomRightRect, sizeof (touchingBottomRightRect));

	// Path + Touching Bottom Left = Touching Bottom Left.
	verifyCombinePathWithPath (path, touchingBottomLeftPath, CombineModeComplement, -20, 60, 30, 40, FALSE, FALSE, &touchingBottomLeftRect, sizeof (touchingBottomLeftRect));

	// Path + No Intersect Left = No Intersect Left.
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeComplement, -21, 20, 30, 40, FALSE, FALSE, &noIntersectLeftRect, sizeof (noIntersectLeftRect));

	// Path + No Intersect Top = No Intersect Top.
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeComplement, 10, -21, 30, 40, FALSE, FALSE, &noIntersectTopRect, sizeof (noIntersectTopRect));

	// Path + No Intersect Right = No Intersect Right.
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeComplement, 41, 20, 30, 40, FALSE, FALSE, &noIntersectRightRect, sizeof (noIntersectRightRect));

	// Path + No Intersect Bottom = No Intersect Bottom.
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeComplement, 10, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRect, sizeof (noIntersectBottomRect));

	// Path + No Intersect Top Left = No Intersect Top Left.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeComplement, -21, -21, 30, 40, FALSE, FALSE, &noIntersectTopLeftRect, sizeof (noIntersectTopLeftRect));

	// Path + No Intersect Top Right = No Intersect Top Right.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeComplement, 41, -21, 30, 40, FALSE, FALSE, &noIntersectTopRightRect, sizeof (noIntersectTopRightRect));

	// Path + No Intersect Bottom Right = No Intersect Bottom Right.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeComplement, 41, 61, 30, 40, FALSE, FALSE, &noIntersectBottomRightRect, sizeof (noIntersectBottomRightRect));

	// Path + No Intersect Bottom Left = No Intersect Bottom Left.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeComplement, -21, 61, 30, 40, FALSE, FALSE, &noIntersectBottomLeftRect, sizeof (noIntersectBottomLeftRect));

	GdipDeleteRegion (infiniteRegion);
	GdipDeleteRegion (emptyRegion);
	GdipDeleteRegion (rectRegion);
	GdipDeleteRegion (pathRegion);
	GdipDeletePath (path);
	GdipDeletePath (superPath);
	GdipDeletePath (subPath);
	GdipDeletePath (intersectLeftPath);
	GdipDeletePath (intersectTopPath);
	GdipDeletePath (intersectRightPath);
	GdipDeletePath (intersectBottomPath);
	GdipDeletePath (intersectTopLeftPath);
	GdipDeletePath (intersectTopRightPath);
	GdipDeletePath (intersectBottomRightPath);
	GdipDeletePath (intersectBottomLeftPath);
	GdipDeletePath (touchingLeftPath);
	GdipDeletePath (touchingTopPath);
	GdipDeletePath (touchingRightPath);
	GdipDeletePath (touchingBottomPath);
	GdipDeletePath (touchingTopLeftPath);
	GdipDeletePath (touchingTopRightPath);
	GdipDeletePath (touchingBottomRightPath);
	GdipDeletePath (touchingBottomLeftPath);
	GdipDeletePath (noIntersectLeftPath);
	GdipDeletePath (noIntersectTopPath);
	GdipDeletePath (noIntersectRightPath);
	GdipDeletePath (noIntersectBottomPath);
	GdipDeletePath (noIntersectTopLeftPath);
	GdipDeletePath (noIntersectTopRightPath);
	GdipDeletePath (noIntersectBottomRightPath);
	GdipDeletePath (noIntersectBottomLeftPath);
	GdipDeletePath (emptyPath);
	GdipDeletePath (infinitePath);
	GdipDeletePath (negativePath);
}

static void test_translateRegion ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GpRectF emptyRect = {0, 0, 0, 0};
	GpRectF rect = {10, 20, 30, 40};
	
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	
	// Infinite rect region - zero, zero
	GdipCreateRegionRect (&infiniteRect, &region);
	
	status = GdipTranslateRegion (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - positive, zero.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegion (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194294, -4194304, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - zero, positive.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegion (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304, -4194284, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - positive, positive.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegion (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194294, -4194284, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - negative, negative.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegion (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194324, -4194344, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);
	
	// Empty rect region - zero, zero
	GdipCreateRegionRect (&emptyRect, &region);
	
	status = GdipTranslateRegion (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Empty rect region - positive, zero.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegion (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 0, 0, 0, TRUE, FALSE);
	
	GdipDeleteRegion (region);

	// Empty rect region - zero, positive.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegion (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 20, 0, 0, TRUE, FALSE);
	
	GdipDeleteRegion (region);

	// Empty rect region - positive, positive.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegion (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 0, 0, TRUE, FALSE);
	
	GdipDeleteRegion (region);

	// Empty rect region - negative, negative.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegion (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, -20, -40, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Rect region.
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipTranslateRegion (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
	
	status = GdipTranslateRegion (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 20, 30, 40, FALSE, FALSE);
	
	status = GdipTranslateRegion (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 40, 30, 40, FALSE, FALSE);

	status = GdipTranslateRegion (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 30, 60, 30, 40, FALSE, FALSE);

	status = GdipTranslateRegion (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region.
	GdipCreateRegionPath (path, &region);
	
	status = GdipTranslateRegion (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
	
	status = GdipTranslateRegion (region, 10, 0);
	assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
	// FIXME: translating a path region should not affect the bounds.
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
#endif
	
	status = GdipTranslateRegion (region, 0, 20);
	assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
	// FIXME: translating a path region should not affect the bounds.
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
#endif

	status = GdipTranslateRegion (region, 10, 20);
	assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
	// FIXME: translating a path region should not affect the bounds.
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
#endif

	status = GdipTranslateRegion (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipTranslateRegion (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegion (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegion (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegion (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegion (region, -10, -20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);
	
	status = GdipTranslateRegion (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	
	status = GdipTranslateRegion (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	
	status = GdipTranslateRegion (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	status = GdipTranslateRegion (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	status = GdipTranslateRegion (region, -10, -20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipTranslateRegion (NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);
	
	GdipDeletePath (path);
}

static void test_translateRegionI ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GpRectF emptyRect = {0, 0, 0, 0};
	GpRectF rect = {10, 20, 30, 40};
	
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	
	// Infinite rect region - zero, zero
	GdipCreateRegionRect (&infiniteRect, &region);
	
	status = GdipTranslateRegionI (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - positive, zero.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegionI (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194294, -4194304, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - zero, positive.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegionI (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304, -4194284, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - positive, positive.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegionI (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194294, -4194284, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - negative, negative.
	GdipCreateRegionRect (&infiniteRect, &region);
	status = GdipTranslateRegionI (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194324, -4194344, 8388608, 8388608, FALSE, TRUE);

	GdipDeleteRegion (region);
	
	// Empty rect region - zero, zero
	GdipCreateRegionRect (&emptyRect, &region);
	
	status = GdipTranslateRegionI (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Empty rect region - positive, zero.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegionI (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 0, 0, 0, TRUE, FALSE);
	
	GdipDeleteRegion (region);

	// Empty rect region - zero, positive.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegionI (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 20, 0, 0, TRUE, FALSE);
	
	GdipDeleteRegion (region);

	// Empty rect region - positive, positive.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegionI (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 0, 0, TRUE, FALSE);
	
	GdipDeleteRegion (region);

	// Empty rect region - negative, negative.
	GdipCreateRegionRect (&emptyRect, &region);
	status = GdipTranslateRegionI (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, -20, -40, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Rect region.
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipTranslateRegionI (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
	
	status = GdipTranslateRegionI (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 20, 30, 40, FALSE, FALSE);
	
	status = GdipTranslateRegionI (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 40, 30, 40, FALSE, FALSE);

	status = GdipTranslateRegionI (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 30, 60, 30, 40, FALSE, FALSE);

	status = GdipTranslateRegionI (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region.
	GdipCreateRegionPath (path, &region);
	
	status = GdipTranslateRegionI (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
	
	status = GdipTranslateRegionI (region, 10, 0);
	assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
	// FIXME: translating a path region should not affect the bounds.
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
#endif
	
	status = GdipTranslateRegionI (region, 0, 20);
	assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
	// FIXME: translating a path region should not affect the bounds.
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
#endif

	status = GdipTranslateRegionI (region, 10, 20);
	assertEqualInt (status, Ok);
#if defined(USE_WINDOWS_GDIPLUS)
	// FIXME: translating a path region should not affect the bounds.
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
#endif

	status = GdipTranslateRegionI (region, -20, -40);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Empty region.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipTranslateRegionI (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegionI (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegionI (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegionI (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	status = GdipTranslateRegionI (region, -10, -20);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Infinite region.
	GdipCreateRegion (&region);
	
	status = GdipTranslateRegionI (region, 0, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	
	status = GdipTranslateRegionI (region, 10, 0);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);
	
	status = GdipTranslateRegionI (region, 0, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	status = GdipTranslateRegionI (region, 10, 20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	status = GdipTranslateRegionI (region, -10, -20);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipTranslateRegionI (NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);
	
	GdipDeletePath (path);
}

static void test_transformRegion ()
{
	GpStatus status;
	GpPath *path;
	GpRegion *region;
	GpMatrix *emptyMatrix;
	GpMatrix *translateXMatrix;
	GpMatrix *translateXScaleXMatrix;
	GpMatrix *translateXScaleYMatrix;
	GpMatrix *translateXScaleXYMatrix;
	GpMatrix *translateYMatrix;
	GpMatrix *translateYScaleXMatrix;
	GpMatrix *translateYScaleYMatrix;
	GpMatrix *translateYScaleXYMatrix;
	GpMatrix *translateXYMatrix;
	GpMatrix *translateXYScaleXMatrix;
	GpMatrix *translateXYScaleYMatrix;
	GpMatrix *translateXYScaleXYMatrix;
	GpMatrix *scaleXMatrix;
	GpMatrix *scaleYMatrix;
	GpMatrix *scaleXYMatrix;
	GpMatrix *matrix;
	GpRectF infiniteRect = {-4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f};
	GpRectF emptyRect = {0, 0, 0, 0};
	GpRectF negativeWidthAndHeightRect = {10, 20, -30, -40};
	GpRectF rect = {10, 20, 30, 40};
	
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);

	GdipCreateMatrix (&emptyMatrix);

	GdipCreateMatrix (&translateXMatrix);
	GdipCreateMatrix (&translateYMatrix);
	GdipCreateMatrix (&translateXYMatrix);

	GdipCreateMatrix (&scaleXMatrix);
	GdipScaleMatrix (scaleXMatrix, 20, 1, MatrixOrderAppend);
	GdipCreateMatrix (&scaleYMatrix);
	GdipScaleMatrix (scaleYMatrix, 1, 20, MatrixOrderAppend);
	GdipCreateMatrix (&scaleXYMatrix);
	GdipScaleMatrix (scaleXYMatrix, 20, 20, MatrixOrderAppend);

	GdipTranslateMatrix (translateXMatrix, 10, 0, MatrixOrderAppend);
	GdipCreateMatrix (&translateXScaleXMatrix);
	GdipTranslateMatrix (translateXScaleXMatrix, 10, 0, MatrixOrderAppend);
	GdipScaleMatrix (translateXScaleXMatrix, 20, 1, MatrixOrderAppend);
	GdipCreateMatrix (&translateXScaleYMatrix);
	GdipTranslateMatrix (translateXScaleYMatrix, 10, 0, MatrixOrderAppend);
	GdipScaleMatrix (translateXScaleYMatrix, 1, 20, MatrixOrderAppend);
	GdipCreateMatrix (&translateXScaleXYMatrix);
	GdipTranslateMatrix (translateXScaleXYMatrix, 10, 0, MatrixOrderAppend);
	GdipScaleMatrix (translateXScaleXYMatrix, 20, 20, MatrixOrderAppend);
	
	GdipTranslateMatrix (translateYMatrix, 0, 10, MatrixOrderAppend);
	GdipCreateMatrix (&translateYScaleXMatrix);
	GdipTranslateMatrix (translateYScaleXMatrix, 0, 10, MatrixOrderAppend);
	GdipScaleMatrix (translateYScaleXMatrix, 20, 1, MatrixOrderAppend);
	GdipCreateMatrix (&translateYScaleYMatrix);
	GdipTranslateMatrix (translateYScaleYMatrix, 0, 10, MatrixOrderAppend);
	GdipScaleMatrix (translateYScaleYMatrix, 1, 20, MatrixOrderAppend);
	GdipCreateMatrix (&translateYScaleXYMatrix);
	GdipTranslateMatrix (translateYScaleXYMatrix, 0, 10, MatrixOrderAppend);
	GdipScaleMatrix (translateYScaleXYMatrix, 20, 20, MatrixOrderAppend);
	
	GdipTranslateMatrix (translateXYMatrix, 10, 10, MatrixOrderAppend);
	GdipCreateMatrix (&translateXYScaleXMatrix);
	GdipTranslateMatrix (translateXYScaleXMatrix, 10, 10, MatrixOrderAppend);
	GdipScaleMatrix (translateXYScaleXMatrix, 20, 1, MatrixOrderAppend);
	GdipCreateMatrix (&translateXYScaleYMatrix);
	GdipTranslateMatrix (translateXYScaleYMatrix, 10, 10, MatrixOrderAppend);
	GdipScaleMatrix (translateXYScaleYMatrix, 1, 20, MatrixOrderAppend);
	GdipCreateMatrix (&translateXYScaleXYMatrix);
	GdipTranslateMatrix (translateXYScaleXYMatrix, 10, 10, MatrixOrderAppend);
	GdipScaleMatrix (translateXYScaleXYMatrix, 20, 20, MatrixOrderAppend);

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);

	// Infinite rect region - empty.
	GdipCreateRegionRect (&infiniteRect, &region);
	
	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Infinite rect region - translate transform.
	GdipCreateRegionRect (&infiniteRect, &region);
	
	status = GdipTransformRegion (region, translateXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194294.0f, -4194294.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Empty rect region - empty.
	GdipCreateRegionRect (&emptyRect, &region);
	
	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Empty rect region - translate transform.
	GdipCreateRegionRect (&emptyRect, &region);

	status = GdipTransformRegion (region, translateXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 10, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Empty rect region - complex transform.
	GdipCreateRegionRect (&emptyRect, &region);
	
	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Negative rect region - empty.
	GdipCreateRegionRect (&negativeWidthAndHeightRect, &region);
	
	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, -30, -40, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Negative rect region - translate transform.
	GdipCreateRegionRect (&negativeWidthAndHeightRect, &region);
	
	status = GdipTransformRegion (region, translateXYMatrix);
	assertEqualInt (status, Ok);
	// FIXME: libgdiplus doesn't normalize.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyRegion (region, -10, -10, 30, 40, FALSE, FALSE);
#endif

	GdipDeleteRegion (region);

	// Negative rect region - complex transform.
	GdipCreateRegionRect (&negativeWidthAndHeightRect, &region);
	
	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, -75, -114, 150, 220, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Rect region - empty transform.
	GdipCreateRegionRect (&rect, &region);
	
	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);
	
	GdipDeleteRegion (region);
	
	// Rect region - translate x transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 30, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x and y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 30, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - scale x transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, scaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 20, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - scale y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, scaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 400, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - scale x and y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, scaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 400, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x, scale x transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXScaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 20, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x, scale y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXScaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 400, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x, scale x and y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXScaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 400, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate y, scale x transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateYScaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 30, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate y, scale y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateYScaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 600, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate y, scale x and y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateYScaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 600, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x and y, scale x transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXYScaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 30, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x and y, scale y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXYScaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 600, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Rect region - translate x and y, scale x and y transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, translateXYScaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 600, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Rect region - complex transform.
	GdipCreateRegionRect (&rect, &region);

	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 75, 106, 150, 220, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - empty transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate x transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 20, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Path region - translate y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 30, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Path region - translate x and y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 30, 30, 40, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Path region - scale x transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, scaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 20, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - scale y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, scaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 400, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - scale x and y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, scaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 400, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate x, scale x transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXScaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 20, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate x, scale y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXScaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 400, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate x, scale x and y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXScaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 400, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate y, scale x transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateYScaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 30, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate y, scale y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateYScaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 10, 600, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate y, scale x and y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateYScaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 200, 600, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate x and y, scale x transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXYScaleXMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 30, 600, 40, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - translate x and y, scale y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXYScaleYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 20, 600, 30, 800, FALSE, FALSE);

	GdipDeleteRegion (region);
	
	// Path region - translate x and y, scale x and y transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, translateXYScaleXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 400, 600, 600, 800, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Path region - complex transform.
	GdipCreateRegionPath (path, &region);

	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 75, 106, 150, 220, FALSE, FALSE);

	GdipDeleteRegion (region);

	// Empty region - empty transform.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Empty region - translate transform.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipTransformRegion (region, translateXYMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Empty region - complex transform.
	GdipCreateRegion (&region);
	GdipSetEmpty (region);

	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, 0, 0, 0, 0, TRUE, FALSE);

	GdipDeleteRegion (region);

	// Infinite region - empty transform.
	GdipCreateRegion (&region);

	status = GdipTransformRegion (region, emptyMatrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);
	
	// Infinite region - translate transform.
	GdipCreateRegion (&region);

	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);
	
	// Infinite region - complex transform.
	GdipCreateRegion (&region);

	status = GdipTransformRegion (region, matrix);
	assertEqualInt (status, Ok);
	verifyRegion (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE);

	GdipDeleteRegion (region);

	// Negative tests.
	status = GdipTransformRegion (NULL, matrix);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformRegion (region, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (path);
	GdipDeleteMatrix (translateXMatrix);
	GdipDeleteMatrix (translateYMatrix);
	GdipDeleteMatrix (translateXYMatrix);
	GdipDeleteMatrix (scaleXMatrix);
	GdipDeleteMatrix (scaleYMatrix);
	GdipDeleteMatrix (scaleXYMatrix);
	GdipDeleteMatrix (translateXScaleXMatrix);
	GdipDeleteMatrix (translateXScaleYMatrix);
	GdipDeleteMatrix (translateXScaleXYMatrix);
	GdipDeleteMatrix (translateYScaleXMatrix);
	GdipDeleteMatrix (translateYScaleYMatrix);
	GdipDeleteMatrix (translateYScaleXYMatrix);
	GdipDeleteMatrix (translateXYScaleXMatrix);
	GdipDeleteMatrix (translateXYScaleYMatrix);
	GdipDeleteMatrix (translateXYScaleXYMatrix);
	GdipDeleteMatrix (emptyMatrix);
	GdipDeleteMatrix (matrix);
}

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
	test_createRegionRgnData ();
	test_getRegionData ();
	test_getRegionDataReplace ();
	test_getRegionDataUnion ();
	test_getRegionDataComplement ();
	test_getRegionDataSize ();
	test_cloneRegion ();
	test_deleteRegion ();
	test_setInfinite ();
	test_setEmpty ();
	test_getRegionBounds ();
	test_isEmptyRegion ();
	test_isEqualRegion ();
	test_isInfiniteRegion ();
	test_isVisibleRegionPoint ();
	test_isVisibleRegionPointI ();
	test_isVisibleRegionRect ();
	test_isVisibleRegionRectI ();
	test_getRegionScansCount ();
	test_getRegionScans ();
	test_getRegionScansI ();
	test_combineReplace ();
	test_combineIntersect ();
	test_combineUnion ();
	test_combineXor ();
	test_combineExclude ();
	test_combineComplement ();
	test_translateRegion ();
	test_translateRegionI ();
	test_transformRegion ();

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN;
	return 0;
}
