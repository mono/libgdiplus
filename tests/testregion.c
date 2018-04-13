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

static void verifyRegionImpl(GpRegion *region, float expectedX, float expectedY, float expectedWidth, float expectedHeight, BOOL expectedIsEmpty, BOOL expectedIsInfinite, const char *file, const char *function, int line)
{
	GpStatus status;
	GpRectF bounds;
	BOOL isEmpty;
	BOOL isInfinite;
	RectF expectedBounds = {expectedX, expectedY, expectedWidth, expectedHeight};

	status = GdipGetRegionBounds (region, graphics, &bounds);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualRectImpl (bounds, expectedBounds, "Bounds", file, function, line);

	status = GdipIsEmptyRegion (region, graphics, &isEmpty);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualIntImpl (isEmpty, expectedIsEmpty, "IsEmpty", file, function, line);

	status = GdipIsInfiniteRegion (region, graphics, &isInfinite);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualIntImpl (isInfinite, expectedIsInfinite, "IsInfinite", file, function, line);
}

#define verifyRegion(region, expectedX, expectedY, expectedWidth, expectedHeight, expectedIsEmpty, expectedIsInfinite) \
	verifyRegionImpl (region, expectedX, expectedY, expectedWidth, expectedHeight, expectedIsEmpty, expectedIsInfinite, __FILE__, __func__, __LINE__)

static void verifyRegionScansImpl (GpRegion *region, RectF *expectedScans, INT expectedCount, const char *file, const char *function, int line)
{
	GpStatus status;
	GpMatrix *matrix;
	INT count;
	GpRectF *scans;
	GdipCreateMatrix (&matrix);

	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualIntImpl (count, expectedCount / sizeof (GpRectF), "ScansCount", file, function, line);
	scans = (GpRectF *) malloc (count * sizeof (GpRectF));

	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	for (int i = 0; i < count; i++)
	{
		printf("{%.0f, %.0f, %.0f, %.0f}", scans[i].X, scans[i].Y, scans[i].Width, scans[i].Height);
		if (i != count - 1)
			printf (",");

		printf("\n");
	}
	printf("\n");

	for (int i = 0; i < count; i++)
	{
		char iChar = i + '0';
		assertEqualRectImpl (scans[i], expectedScans[i], &iChar, file, function, line);
	}

	GdipDeleteMatrix (matrix);
}

#define verifyRegionScans(region, expectedScans, expectedCount) \
	verifyRegionScansImpl (region, expectedScans, expectedCount, __FILE__, __func__, __LINE__)

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
	verifyRegion (region, 1, 2, -3, 4, TRUE, FALSE);
	GdipDeleteRegion (region);

	// Negative height.
	status = GdipCreateRegionRect (&negativeHeightRect, &region);
	assertEqualInt (status, Ok);
	verifyRegion (region, 1, 2, 3, -4, TRUE, FALSE);
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

	// Path region - non null rects.
	GdipCreatePath (FillModeWinding, &path);
	GdipAddPathRectangle (path, 10, 20, 30, 40);
	GdipCreateRegionPath (path, &region);
	
	count = 0xFF;
	status = GdipGetRegionScansI (region, scans, &count, matrix);
	assertEqualInt (status, Ok);
	// FIXME: these are incorrect.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (scans[0].X, 10);
	assertEqualInt (scans[0].Y, 20);
	assertEqualInt (scans[0].Width, 30);
	assertEqualInt (scans[0].Height, 40);
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
#endif

static GpRectF infiniteScans[] = {
	{-4194304, -4194304, 8388608, 8388608}
};

static GpRectF *emptyScans = NULL;

#define verifyCombineRegionWithRegion(region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpStatus status = GdipCombineRegionRegion (region, region2, mode); \
	assertEqualInt (status, Ok); \
 \
	verifyRegion (region, x, y, width, height, isEmpty, isInfinite); \
	verifyRegionScans (region, scans, scansCount); \
 \
}

#define verifyCombineRegionWithRect(region, rect, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *clone; \
	GdipCloneRegion (region, &clone); \
 \
	/* First test combining with a rect region. */ \
	GpRegion *region2; \
	GdipCreateRegionRect (rect, &region2); \
	verifyCombineRegionWithRegion (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region2); \
 \
	/* First test combining with an actual rect. */ \
	GdipCombineRegionRect (clone, rect, mode); \
	verifyRegion (clone, x, y, width, height, isEmpty, isInfinite); \
	verifyRegionScans (clone, scans, scansCount); \
	GdipDeleteRegion (clone); \
}

#define verifyCombineRegionWithPath(region, path, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount) \
{ \
	GpRegion *clone; \
	GdipCloneRegion (region, &clone); \
 \
	/* First test combining with a path region. */ \
	GpRegion *region2; \
	GdipCreateRegionPath (path, &region2); \
	verifyCombineRegionWithRegion (region, region2, mode, x, y, width, height, isEmpty, isInfinite, scans, scansCount); \
	GdipDeleteRegion (region2); \
 \
	/* First test combining with an actual path. */ \
	GdipCombineRegionPath (clone, path, mode); \
	verifyRegion (clone, x, y, width, height, isEmpty, isInfinite); \
	verifyRegionScans (clone, scans, scansCount); \
	GdipDeleteRegion (clone); \
}

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
	RectF noIntersectLeftRect = {-20, 20, 30, 40};
	RectF noIntersectTopRect = {10, -20, 30, 40};
	RectF noIntersectRightRect = {40, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 60, 30, 40};
	RectF noIntersectTopLeftRect = {-20, -20, 30, 40};
	RectF noIntersectTopRightRect = {40, -20, 30, 40};
	RectF noIntersectBottomRightRect = {40, 60, 30, 40};
	RectF noIntersectBottomLeftRect = {-20, 60, 30, 40};
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

	// Infinite + Infinite = Empty.
	// FIXME: this incorrectly returns an empty region: https://github.com/mono/libgdiplus/issues/335
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif
	// Infinite + Empty = Empty.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite + Rect = Rect.
	verifyCombineInfiniteWithRect (&rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Infinite + Path = Path.
	verifyCombineInfiniteWithPath (path, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty + Infinite = Empty.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Infinite Rect = Infinite.
	// FIXME: this incorrectly returns an empty region: https://github.com/mono/libgdiplus/issues/335
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeIntersect, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Infinite Rect + Empty Rect = Empty Rect.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Rect + Rect = Rect.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty Rect + Infinite Rect = Empty.
	// FIXME: scans are incorrect: https://github.com/mono/libgdiplus/issues/337
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Rect + Empty Rect = Empty.
	// FIXME: scans are incorrect: https://github.com/mono/libgdiplus/issues/337
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Rect + Rect = Empty.
	// FIXME: scans are incorrect: https://github.com/mono/libgdiplus/issues/337
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Rect + Infinite = Rect.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Empty = Empty.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Infinite Rect = Rect.
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Rect + Negative Rect = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	RectF negativeRectScan = {10, 20, 10, 10};
	verifyCombineRectWithRect (&rect, &negativeRect, CombineModeIntersect, 10, 20, 10, 10, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + Negative Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, negativePath, CombineModeIntersect, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

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

	// Rect + No Intersect Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Top = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Top Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Top Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Rect + No Intersect Bottom Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + Infinite = Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Path + Empty = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + Infinite Rect = Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeIntersect, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Path + Empty Rect = Empty.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + Negative Rect = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &negativeRect, CombineModeIntersect, 10, 20, 10, 10, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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

	// Path + No Intersect Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Top = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Top Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	printf("1122\n");
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Top Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	printf("an\n");
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

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

	// Path + No Intersect Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Top = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Top Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Top Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Right = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

	// Path + No Intersect Bottom Left = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeIntersect, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

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
	RectF noIntersectLeftRect = {-20, 20, 30, 40};
	RectF noIntersectTopRect = {10, -20, 30, 40};
	RectF noIntersectRightRect = {40, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 60, 30, 40};
	RectF noIntersectTopLeftRect = {-20, -20, 30, 40};
	RectF noIntersectTopRightRect = {40, -20, 30, 40};
	RectF noIntersectBottomRightRect = {40, 60, 30, 40};
	RectF noIntersectBottomLeftRect = {-20, 60, 30, 40};
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

	// Infinite + Rect = Infinite.
	verifyCombineInfiniteWithRect (&rect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Path = Infinite.
	verifyCombineInfiniteWithPath (path, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Infinite = Infinite.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Infinite Rect + Infinite Rect = Infinite Rect.
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Empty Rect = Infinite Rect.
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Rect + Rect = Infinite Rect.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Rect + Infinite Rect = Infinite Rect.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Rect + Empty Rect = Empty.
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty Rect + Rect = Rect.
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite = Infinite.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Rect + Empty = Rect.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Rect = Rect.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Rect + Empty Rect = Rect.
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Rect = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	RectF negativeRectScan = {10, 20, 30, 40};
	verifyCombineRectWithRect (&rect, &negativeRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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
	// FIXME: should be combined into a single rect: https://github.com/mono/libgdiplus/issues/340
	RectF intersectTopScan = {10, 10, 30, 50};
#if defined(USE_WINDOWS_GDIPLUS)
	RectF intersectTopScansRect[] = {intersectTopScan};
#else
	RectF intersectTopScansRect[] = {{10, 10, 30, 40}, {10, 50, 30, 10}};
#endif
	verifyCombineRectWithRect (&rect, &intersectTopRect, CombineModeUnion, 10, 10, 30, 50, FALSE, FALSE, intersectTopScansRect, sizeof (intersectTopScansRect));
	
	// Rect + Intersect Right = Calculation.
	RectF intersectRightScan = {10, 20, 40, 40};
	verifyCombineRectWithRect (&rect, &intersectRightRect, CombineModeUnion, 10, 20, 40, 40, FALSE, FALSE, &intersectRightScan, sizeof (intersectRightScan));

	// Rect + Intersect Bottom = Calculation.
	// FIXME: should be combined into a single rect: https://github.com/mono/libgdiplus/issues/340
	RectF intersectBottomScan = {10, 20, 30, 50};
#if defined(USE_WINDOWS_GDIPLUS)
	RectF intersectBottomScansRect[] = {intersectBottomScan};
#else
	RectF intersectBottomScansRect[] = {{10, 20, 30, 40}, {10, 60, 30, 10}};
#endif
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

	// Rect + No Intersect Left = Calculation.
	RectF noIntersectLeftScan = {-20, 20, 60, 40};
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &noIntersectLeftScan, sizeof (noIntersectLeftScan));

	// Rect + No Intersect Top = Calculation.
	// FIXME: should be combined into a single rect: https://github.com/mono/libgdiplus/issues/340
	RectF noIntersectTopScan = {10, -20, 30, 80};
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectTopScansRect[] = {noIntersectTopScan};
#else
	RectF noIntersectTopScansRect[] = {{10, -20, 30, 40}, {10, 20, 30, 40}};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, noIntersectTopScansRect, sizeof (noIntersectTopScansRect));

	// Rect + No Intersect Right = Calculation.
	RectF noIntersectRightScan = {10, 20, 60, 40};
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &noIntersectRightScan, sizeof (noIntersectRightScan));

	// Rect + No Intersect Bottom = Calculation.
	// FIXME: should be combined into a single rect: https://github.com/mono/libgdiplus/issues/340
	RectF noIntersectBottomScan = {10, 20, 30, 80};
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectBottomScansRect[] = {noIntersectBottomScan};
#else
	RectF noIntersectBottomScansRect[] = {{10, 20, 30, 40}, {10, 60, 30, 40}};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, noIntersectBottomScansRect, sizeof (noIntersectBottomScansRect));

	// Rect + No Intersect Top Left = Both.
	RectF noIntersectTopLeftScans[] = {
		noIntersectTopLeftRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	RectF noIntersectTopRightScans[] = {
		noIntersectTopRightRect,
		rect
	};
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	RectF noIntersectBottomRightScans[] = {
		rect,
		noIntersectBottomRightRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	RectF noIntersectBottomLeftScans[] = {
		rect,
		noIntersectBottomLeftRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// Rect + Infinite Path = Path.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Rect + Empty Path = Empty.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, negativePath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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

	// Rect + No Intersect Left = Calculation.
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &noIntersectLeftScan, sizeof (noIntersectLeftScan));

	// Rect + No Intersect Top = Calculation.
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, &noIntersectTopScan, sizeof (noIntersectTopScan));

	// Rect + No Intersect Right = Calculation.
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &noIntersectRightScan, sizeof (noIntersectRightScan));

	// Rect + No Intersect Bottom = Calculation.
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, &noIntersectBottomScan, sizeof (noIntersectBottomScan));

	// Rect + No Intersect Top Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338.
#if defined(USE_WINDOWS_GDIPLUS)
	// Infinite Path + Infinite Rect = Infinite Rect.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Empty Rect = Infinite Rect.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Rect = Infinite Rect.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty Path + Infinite Rect = Infinite Rect.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Path + Empty Rect = Empty.
	// FIXME: this should be empty: https://github.com/mono/libgdiplus/issues/341
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeUnion, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Path + Rect = Rect.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite = Infinite.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Path + Empty = Empty.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite Rect = Rect.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/339
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &infiniteRect, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Path + Empty Rect = Empty.
	verifyCombinePathWithRect (path, &emptyRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Rect = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &negativeRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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

	// Path + No Intersect Left = Calculation.
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &noIntersectLeftScan, sizeof (noIntersectLeftScan));

	// Path + No Intersect Top = Calculation.
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, &noIntersectTopScan, sizeof (noIntersectTopScan));

	// Path + No Intersect Right = Calculation.
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &noIntersectRightScan, sizeof (noIntersectRightScan));

	// Path + No Intersect Bottom = Calculation.
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, &noIntersectBottomScan, sizeof (noIntersectBottomScan));

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

	// Path + Infinite Path = Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, infinitePath, CombineModeUnion, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Path + Empty Path = Empty.
	verifyCombinePathWithPath (path, emptyPath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, negativePath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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

	// Path + No Intersect Left = Calculation.
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeUnion, -20, 20, 60, 40, FALSE, FALSE, &noIntersectLeftScan, sizeof (noIntersectLeftScan));

	// Path + No Intersect Top = Calculation.
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeUnion, 10, -20, 30, 80, FALSE, FALSE, &noIntersectTopScan, sizeof (noIntersectTopScan));

	// Path + No Intersect Right = Calculation.
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeUnion, 10, 20, 60, 40, FALSE, FALSE, &noIntersectRightScan, sizeof (noIntersectRightScan));

	// Path + No Intersect Bottom = Calculation.
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeUnion, 10, 20, 30, 80, FALSE, FALSE, &noIntersectBottomScan, sizeof (noIntersectBottomScan));

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeUnion, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeUnion, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeUnion, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeUnion, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));

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
	RectF noIntersectLeftRect = {-20, 20, 30, 40};
	RectF noIntersectTopRect = {10, -20, 30, 40};
	RectF noIntersectRightRect = {40, 20, 30, 40};
	RectF noIntersectBottomRect = {10, 60, 30, 40};
	RectF noIntersectTopLeftRect = {-20, -20, 30, 40};
	RectF noIntersectTopRightRect = {40, -20, 30, 40};
	RectF noIntersectBottomRightRect = {40, 60, 30, 40};
	RectF noIntersectBottomLeftRect = {-20, 60, 30, 40};
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
	// FIXME: this should be empty: https://github.com/mono/libgdiplus/issues/342
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithRegion (infiniteRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif
	// Infinite + Empty = Infinite.
	verifyCombineInfiniteWithRegion (emptyRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite + Rect = Not Rect.
	RectF infiniteWithRectScans[] = {
		{-4194304, -4194304, 8388608, 4194324},
		{-4194304, 20, 4194314, 40},
		{40, 20, 4194264, 40},
		{-4194304, 60, 8388608, 4194244}
	};
	verifyCombineInfiniteWithRect (&rect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Infinite + Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineInfiniteWithPath (path, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Empty + Infinite = Infinite.
	verifyCombineEmptyWithRegion (infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Empty + Empty = Empty.
	verifyCombineEmptyWithRegion (emptyRegion, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Empty + Rect = Rect.
	verifyCombineEmptyWithRect (&rect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Empty + Path = Path.
	verifyCombineEmptyWithPath (path, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Infinite Rect + Infinite Rect = Empty.
	// FIXME: this should be empty: https://github.com/mono/libgdiplus/issues/342
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&infiniteRect, &infiniteRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite Rect + Empty Rect = Infinite Rect.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/343
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&infiniteRect, &emptyRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Infinite Rect + Rect = Not Rect.
	verifyCombineRectWithRect (&infiniteRect, &rect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Empty Rect + Infinite Rect = Infinite Rect.
	// FIXME: this should be infinite: https://github.com/mono/libgdiplus/issues/343
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Rect + Empty Rect = Empty.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &emptyRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Rect + Rect = Rect.
	// FIXME: incorrect bounds/scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&emptyRect, &rect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Rect + Infinite = Not Rect.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Rect + Empty = Rect.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Rect = Not Rect.
	verifyCombineRectWithRect (&rect, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Rect + Empty Rect = Rect.
	// FIXME: incorrect bounds/scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithRect (&rect, &emptyRect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Rect + Negative Rect = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	RectF negativeRectScan = {10, 20, 30, 40};
	verifyCombineRectWithRect (&rect, &negativeRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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

	// Rect + No Intersect Left = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectLeftScans[] = {{-20, 20, 60, 40}};
#else
	RectF noIntersectLeftScans[] = {
		rect,
		noIntersectLeftRect
	};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectLeftRect, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Rect + No Intersect Top = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectTopScans[] = {{10, -20, 30, 80}};
#else
	RectF noIntersectTopScans[] = {
		rect,
		noIntersectTopRect
	};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectTopRect, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Rect + No Intersect Right = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectRightScans[] = {{10, 20, 60, 40}};
#else
	RectF noIntersectRightScans[] = {
		rect,
		noIntersectRightRect
	};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectRightRect, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Rect + No Intersect Bottom = Both.
	// FIXME: combine adjacent regions: https://github.com/mono/libgdiplus/issues/345
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectBottomScans[] = {{10, 20, 30, 80}};
#else
	RectF noIntersectBottomScans[] = {{10, 20, 30, 40}, {10, 60, 30, 40}};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectBottomRect, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Rect + No Intersect Top Left = Both.
	// FIXME: different ordering: https://github.com/mono/libgdiplus/issues/346
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectTopLeftScans[] = {
		noIntersectTopLeftRect,
		rect
	};
#else
	RectF noIntersectTopLeftScans[] = {
		rect,
		noIntersectTopLeftRect
	};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectTopLeftRect, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	// FIXME: different ordering: https://github.com/mono/libgdiplus/issues/346
#if defined(USE_WINDOWS_GDIPLUS)
	RectF noIntersectTopRightScans[] = {
		noIntersectTopRightRect,
		rect
	};
#else
	RectF noIntersectTopRightScans[] = {
		rect,
		noIntersectTopRightRect
	};
#endif
	verifyCombineRectWithRect (&rect, &noIntersectTopRightRect, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	RectF noIntersectBottomRightScans[] = {
		rect,
		noIntersectBottomRightRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomRightRect, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	RectF noIntersectBottomLeftScans[] = {
		rect,
		noIntersectBottomLeftRect
	};
	verifyCombineRectWithRect (&rect, &noIntersectBottomLeftRect, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
	
	// Infinite Rect + Infinite Path = Empty.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&infiniteRect, infinitePath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Infinite Rect + Empty Path = Infinite Rect.
	verifyCombineRectWithPath (&infiniteRect, emptyPath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	// Infinite Rect + Path = Not Path.
	verifyCombineRectWithPath (&infiniteRect, path, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Empty Rect + Infinite Path = Infinite Rect.
	verifyCombineRectWithPath (&emptyRect, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Rect + Empty Path = Empty.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&emptyRect, emptyPath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Rect + Path = Path.
	verifyCombineRectWithPath (&emptyRect, path, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite = Not Rect.
	verifyCombineRectWithRegion (&rect, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Rect + Empty = Rect.
	verifyCombineRectWithRegion (&rect, emptyRegion, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Infinite Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Rect + Empty Path = Path.
	verifyCombineRectWithPath (&rect, emptyPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Rect + Negative Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombineRectWithPath (&rect, negativePath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

	// Rect + Equal Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombineRectWithPath (&rect, path, CombineModeXor, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

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

	// Rect + No Intersect Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectLeftPath, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Rect + No Intersect Top = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopPath, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Rect + No Intersect Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectRightPath, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Rect + No Intersect Bottom = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomPath, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Rect + No Intersect Top Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopLeftPath, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Rect + No Intersect Top Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectTopRightPath, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Rect + No Intersect Bottom Right = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomRightPath, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Rect + No Intersect Bottom Left = Both.
	verifyCombineRectWithPath (&rect, noIntersectBottomLeftPath, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
#endif

	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	// Infinite Path + Infinite Rect = Empty.
	verifyCombinePathWithRect (infinitePath, &infiniteRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Rect = Infinite Rect.
	verifyCombinePathWithRect (infinitePath, &emptyRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Rect = Not Path.
	verifyCombinePathWithRect (infinitePath, &rect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Empty Path + Infinite Rect = Infinite Rect.
	verifyCombinePathWithRect (emptyPath, &infiniteRect, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Path + Empty Rect = Empty.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (emptyPath, &emptyRect, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Path + Rect = Rect.
	verifyCombinePathWithRect (emptyPath, &rect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

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
	// FIXME: incorrect bounds/scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &emptyRect, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));
#endif

	// Path + Negative Rect = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRect (path, &negativeRect, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

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

	// Path + No Intersect Left = Both.
	verifyCombinePathWithRect (path, &noIntersectLeftRect, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Path + No Intersect Top = Both.
	verifyCombinePathWithRect (path, &noIntersectTopRect, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Path + No Intersect Right = Both.
	verifyCombinePathWithRect (path, &noIntersectRightRect, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Path + No Intersect Bottom = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomRect, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithRect (path, &noIntersectTopLeftRect, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithRect (path, &noIntersectTopRightRect, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomRightRect, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithRect (path, &noIntersectBottomLeftRect, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
#endif

	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	// Infinite Path + Infinite Path = Empty.
	verifyCombinePathWithPath (infinitePath, infinitePath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);

	// Infinite Path + Empty Path = Infinite Rect.
	verifyCombinePathWithPath (infinitePath, emptyPath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));

	// Infinite Path + Path = Not Path.
	verifyCombinePathWithPath (infinitePath, path, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));

	// Empty Path + Infinite Path = Infinite Rect.
	verifyCombinePathWithPath (emptyPath, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, TRUE, infiniteScans, sizeof (infiniteScans));
#endif

	// Empty Path + Empty Path = Empty.
	// FIXME: incorrect scans: https://github.com/mono/libgdiplus/issues/344
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (emptyPath, emptyPath, CombineModeXor, 0, 0, 0, 0, TRUE, FALSE, emptyScans, 0);
#endif

	// Empty Path + Path = Path.
	verifyCombinePathWithPath (emptyPath, path, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithRegion (path, infiniteRegion, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Path + Empty = Rect.
	verifyCombinePathWithRegion (path, emptyRegion, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Infinite Path = Not Path.
	// FIXME: this fails with OutOfMemory: https://github.com/mono/libgdiplus/issues/338
#if defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, infinitePath, CombineModeXor, -4194304, -4194304, 8388608, 8388608, FALSE, FALSE, infiniteWithRectScans, sizeof (infiniteWithRectScans));
#endif

	// Path + Empty Path = Path.
	verifyCombinePathWithPath (path, emptyPath, CombineModeXor, 10, 20, 30, 40, FALSE, FALSE, &rect, sizeof (rect));

	// Path + Negative Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
#if !defined(USE_WINDOWS_GDIPLUS)
	verifyCombinePathWithPath (path, negativePath, CombineModeUnion, 10, 20, 30, 40, FALSE, FALSE, &negativeRectScan, sizeof (negativeRectScan));
#endif

	// Path + Equal Path = Empty.
	// FIXME: this should set to empty: https://github.com/mono/libgdiplus/issues/336
	verifyCombinePathWithPath (path, path, CombineModeXor, 0, 0, 0, 0, WINDOWS_GDIPLUS, FALSE, emptyScans, 0);

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

	// Path + No Intersect Left = Both.
	verifyCombinePathWithPath (path, noIntersectLeftPath, CombineModeXor, -20, 20, 60, 40, FALSE, FALSE, noIntersectLeftScans, sizeof (noIntersectLeftScans));

	// Path + No Intersect Top = Both.
	verifyCombinePathWithPath (path, noIntersectTopPath, CombineModeXor, 10, -20, 30, 80, FALSE, FALSE, noIntersectTopScans, sizeof (noIntersectTopScans));

	// Path + No Intersect Right = Both.
	verifyCombinePathWithPath (path, noIntersectRightPath, CombineModeXor, 10, 20, 60, 40, FALSE, FALSE, noIntersectRightScans, sizeof (noIntersectRightScans));

	// Path + No Intersect Bottom = Both.
	verifyCombinePathWithPath (path, noIntersectBottomPath, CombineModeXor, 10, 20, 30, 80, FALSE, FALSE, noIntersectBottomScans, sizeof (noIntersectBottomScans));

	// Path + No Intersect Top Left = Both.
	verifyCombinePathWithPath (path, noIntersectTopLeftPath, CombineModeXor, -20, -20, 60, 80, FALSE, FALSE, noIntersectTopLeftScans, sizeof (noIntersectTopLeftScans));

	// Path + No Intersect Top Right = Both.
	verifyCombinePathWithPath (path, noIntersectTopRightPath, CombineModeXor, 10, -20, 60, 80, FALSE, FALSE, noIntersectTopRightScans, sizeof (noIntersectTopRightScans));

	// Path + No Intersect Bottom Right = Both.
	verifyCombinePathWithPath (path, noIntersectBottomRightPath, CombineModeXor, 10, 20, 60, 80, FALSE, FALSE, noIntersectBottomRightScans, sizeof (noIntersectBottomRightScans));

	// Path + No Intersect Bottom Left = Both.
	verifyCombinePathWithPath (path, noIntersectBottomLeftPath, CombineModeXor, -20, 20, 60, 80, FALSE, FALSE, noIntersectBottomLeftScans, sizeof (noIntersectBottomLeftScans));
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
	test_combineReplace ();
	test_combineIntersect ();
	test_combineUnion ();
	test_combineXor ();

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);

	SHUTDOWN;
	return 0;
}
