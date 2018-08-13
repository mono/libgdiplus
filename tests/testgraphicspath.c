#ifdef _WIN32
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

#include "testhelpers.h"
#if !defined(USE_WINDOWS_GDIPLUS)
#include "../config.h"
#endif

static void verifyPathImpl (GpPath *path, FillMode expectedFillMode, float expectedX, float expectedY, float expectedWidth, float expectedHeight, const PointF *expectedPoints, const BYTE *expectedTypes, INT expectedCount, const char *message, const char *file, const char *function, int line)
{
	GpStatus status;
	FillMode fillMode;
	RectF bounds;
	RectF expectedBounds = {expectedX, expectedY, expectedWidth, expectedHeight};
	GpPathData pathData;
	pathData.Count = 16;
	pathData.Points = (PointF *) malloc (sizeof (PointF) * 16);
	pathData.Types = (BYTE *) malloc (sizeof (BYTE) * 16);


	status = GdipGetPathFillMode (path, &fillMode);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (fillMode, expectedFillMode, message, file, function, line);

	status = GdipGetPathWorldBounds (path, &bounds, NULL, NULL);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualRectImpl (bounds, expectedBounds, message, file, function, line);

	status = GdipGetPathData (path, &pathData);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (pathData.Count, expectedCount, message, file, function, line);
	assertEqualPointsFImpl (pathData.Points, expectedPoints, expectedCount, file, function, line);
	for (int i = 0; i < pathData.Count; i++)
	{
		char iChar = i + '0';
		assertEqualIntImpl (pathData.Types[i], expectedTypes[i], &iChar, file, function, line);
	}

#if !defined(USE_WINDOWS_GDIPLUS)
	free (pathData.Points);
	free (pathData.Types);
#endif
}

#define verifyPath(path, expectedFillMode, expectedX, expectedY, expectedWidth, expectedHeight, expectedPoints, expectedTypes, expectedCount) \
	verifyPathImpl (path, expectedFillMode, expectedX, expectedY, expectedWidth, expectedHeight, expectedPoints, expectedTypes, expectedCount, NULL, __FILE__, __func__, __LINE__)

// A debug helper that simply saves the path out to an image for viewing and comparison with GDI+.
ATTRIBUTE_USED static void dumpPath (GpPath *path)
{
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;
	GpSolidFill *brush;
	WCHAR *fileName = createWchar ("test-image.bmp");

	GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppARGB, NULL, (GpBitmap **) &image);
	GdipGetImageGraphicsContext (image, &graphics);
	GdipCreateSolidFill (0xFF0000FF, &brush);

	GdipFillPath (graphics, brush, path);
	status = GdipSaveImageToFile (image, fileName, &bmpEncoderClsid, NULL);
	assertEqualInt (status, Ok);

	int systemResult = system ("test-image.bmp");
	(void) systemResult;

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
	GdipDeleteBrush ((GpBrush *) brush);
	freeWchar (fileName);
}

static void test_createPath ()
{
	GpStatus status;
	GpPath *path;

	// FillModeAlternate.
	status = GdipCreatePath (FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// FillModeWinding.
	status = GdipCreatePath (FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid FillMode - positive.
	status = GdipCreatePath ((FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, Ok);
	verifyPath (path, (FillMode)(FillModeWinding + 1), 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid FillMode - negative.
	status = GdipCreatePath ((FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, Ok);
	verifyPath (path, (FillMode)(FillModeAlternate - 1), 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Negative tests.
	status = GdipCreatePath (FillModeWinding, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createPath2 ()
{
	GpStatus status;
	GpPath *path;
	
	PointF rectPoints[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{11, 2},
		{14, 2},
		{14, 6},
		{11, 6},
		{21, 2},
		{24, 2},
		{24, 6},
		{21, 6},
	};
	BYTE rectTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE openRectTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	BYTE multiOpenAndClosedRectTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	PointF bezierPoints[] = {
		{1, 2},
		{2, 4},
		{3, 6},
		{4, 8},
		{11, 2},
		{12, 4},
		{13, 6},
		{14, 8},
		{21, 2},
		{22, 4},
		{23, 6},
		{24, 8},
	};
	BYTE bezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE openBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier
	};
	BYTE multiOpenAndClosedBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier
	};
	PointF rectBezierPoints[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{11, 2},
		{12, 4},
		{13, 6},
		{14, 8},
		{21, 2},
		{24, 2},
		{24, 6},
		{21, 6},
		{31, 2},
		{32, 4},
		{33, 6},
		{34, 8}
	};
	BYTE rectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE openRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier
	};
	BYTE multiRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE multiOpenRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	BYTE multiOpenAndClosedRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE allStartTypes[] = {
		PathPointTypeStart,
		PathPointTypeStart,
		PathPointTypeStart,
		PathPointTypeStart | PathPointTypeCloseSubpath
	};
	BYTE invalidMidType[] = {
		PathPointTypeStart,
		(PathPointType)4,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE invalidEndType[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		(PathPointType)4 | PathPointTypeCloseSubpath
	};
	BYTE invalidPostEndType[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		(PathPointType)4
	};
	BYTE invalidBezierType2[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		(PathPointType)4,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE invalidBezierType3[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		(PathPointType)4 | PathPointTypeCloseSubpath
	};
	BYTE midStartType1[] = {
		PathPointTypeStart,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine
	};
	BYTE midStartType2[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeStart
	};
	BYTE invalidStartType[] = {
		(PathPointType)4,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE endStartType[] = {
		PathPointTypeStart,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart
	};
	BYTE openEndStartType[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeStart
	};

	// Closed lines.
	status = GdipCreatePath2 (rectPoints, rectTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 4, rectPoints, rectTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, rectTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 4, rectPoints, rectTypes, 4);
	GdipDeletePath (path);

	// Open lines.
	status = GdipCreatePath2 (rectPoints, rectTypes, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 0, rectPoints, rectTypes, 2);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, rectTypes, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 0, rectPoints, rectTypes, 2);
	GdipDeletePath (path);
	
	// Multiple closed lines.
	status = GdipCreatePath2 (rectPoints, rectTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 4, rectPoints, rectTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 4, rectPoints, rectTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open lines.
	status = GdipCreatePath2 (rectPoints, openRectTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 4, rectPoints, openRectTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, openRectTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 4, rectPoints, openRectTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open and closed lines.
	status = GdipCreatePath2 (rectPoints, multiOpenAndClosedRectTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 4, rectPoints, multiOpenAndClosedRectTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, multiOpenAndClosedRectTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 4, rectPoints, multiOpenAndClosedRectTypes, 12);
	GdipDeletePath (path);

	// Closed beziers.
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 6, bezierPoints, bezierTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2 (bezierPoints, bezierTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 6, bezierPoints, bezierTypes, 4);
	GdipDeletePath (path);

	// Open beziers.
	status = GdipCreatePath2 (bezierPoints, openBezierTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 6, bezierPoints, openBezierTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2 (bezierPoints, openBezierTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 6, bezierPoints, openBezierTypes, 4);
	GdipDeletePath (path);
	
	// Multiple closed beziers.
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 6, bezierPoints, bezierTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 6, bezierPoints, bezierTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open beziers.
	status = GdipCreatePath2 (bezierPoints, openBezierTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 6, bezierPoints, openBezierTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, openBezierTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 6, bezierPoints, openBezierTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open and closed beziers.
	status = GdipCreatePath2 (bezierPoints, multiOpenAndClosedBezierTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 6, bezierPoints, multiOpenAndClosedBezierTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, multiOpenAndClosedBezierTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 6, bezierPoints, multiOpenAndClosedBezierTypes, 12);
	GdipDeletePath (path);
	
	// Closed lines and rects.
	status = GdipCreatePath2 (rectBezierPoints, rectBezierTypes, 7, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 12, 4, rectBezierPoints, rectBezierTypes, 7);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectBezierPoints, rectBezierTypes, 7, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 12, 4, rectBezierPoints, rectBezierTypes, 7);
	GdipDeletePath (path);
	
	// Open lines and rects.
	status = GdipCreatePath2 (rectBezierPoints, openRectBezierTypes, 7, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 12, 4, rectBezierPoints, openRectBezierTypes, 7);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectBezierPoints, openRectBezierTypes, 7, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 12, 4, rectBezierPoints, openRectBezierTypes, 7);
	GdipDeletePath (path);
	
	// Multiple closed lines and rects.
	status = GdipCreatePath2 (rectBezierPoints, multiRectBezierTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 6, rectBezierPoints, multiRectBezierTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectBezierPoints, multiRectBezierTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 6, rectBezierPoints, multiRectBezierTypes, 12);
	GdipDeletePath (path);
	
	// Multiple open lines and rects.
	status = GdipCreatePath2 (rectBezierPoints, multiOpenRectBezierTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 6, rectBezierPoints, multiOpenRectBezierTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectBezierPoints, multiOpenRectBezierTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 6, rectBezierPoints, multiOpenRectBezierTypes, 12);
	GdipDeletePath (path);
	
	// Multiple open and closed lines and rects.
	status = GdipCreatePath2 (rectBezierPoints, multiOpenAndClosedRectBezierTypes, 16, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 33, 6, rectBezierPoints, multiOpenAndClosedRectBezierTypes, 16);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectBezierPoints, multiOpenAndClosedRectBezierTypes, 16, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 33, 6, rectBezierPoints, multiOpenAndClosedRectBezierTypes, 16);
	GdipDeletePath (path);

	// Only start.
	status = GdipCreatePath2 (rectPoints, rectTypes, 1, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPoints, rectTypes, 1);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, rectTypes, 1, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPoints, rectTypes, 1);
	GdipDeletePath (path);

	// Invalid start type multiple - normalizes.
	status = GdipCreatePath2 (rectPoints, invalidStartType, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 4, rectPoints, rectTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, invalidStartType, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 4, rectPoints, rectTypes, 4);
	GdipDeletePath (path);

	// Invalid start type single - normalizes.
	status = GdipCreatePath2 (rectPoints, invalidStartType, 1, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	BYTE zeroType[] = {0};
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPoints, zeroType, 1);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, invalidStartType, 1, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPoints, zeroType, 1);
	GdipDeletePath (path);

	// End start type closed - sets to empty.
	status = GdipCreatePath2 (rectPoints, endStartType, 3, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, endStartType, 3, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	// End start type open - sets to empty.
	status = GdipCreatePath2 (rectPoints, openEndStartType, 3, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, openEndStartType, 3, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid mid type - sets to empty.
	status = GdipCreatePath2 (rectPoints, invalidMidType, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, invalidMidType, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid end type - sets to empty.
	status = GdipCreatePath2 (rectPoints, invalidEndType, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, invalidEndType, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid post end type - sets to empty.
	status = GdipCreatePath2 (rectPoints, invalidPostEndType, 5, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, invalidPostEndType, 5, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Missing 2 bezier points - sets to empty.
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Missing 1 bezier point - sets to empty.
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 3, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, bezierTypes, 3, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid second bezier point - sets to empty.
	status = GdipCreatePath2 (bezierPoints, invalidBezierType2, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, invalidBezierType2, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid third bezier point - sets to empty.
	status = GdipCreatePath2 (bezierPoints, invalidBezierType3, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (bezierPoints, invalidBezierType3, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	// Only start - sets to empty.
	status = GdipCreatePath2 (rectPoints, allStartTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPoints, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, allStartTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPoints, NULL, 0);
	GdipDeletePath (path);
	
	// Only start open - sets to empty.
	status = GdipCreatePath2 (rectPoints, midStartType1, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPoints, NULL, 0);
	GdipDeletePath (path);

	status = GdipCreatePath2 (rectPoints, midStartType2, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPoints, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, midStartType1, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPoints, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2 (rectPoints, midStartType2, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPoints, NULL, 0);
	GdipDeletePath (path);

	// Negative tests.
	status = GdipCreatePath2 (NULL, rectTypes, 2, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (NULL, rectTypes, 0, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (NULL, rectTypes, -1, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (NULL, rectTypes, 2, (FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (NULL, rectTypes, 2, (FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (NULL, NULL, 0, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, NULL, 2, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, NULL, 0, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, NULL, -1, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, NULL, 2, (FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, NULL, 2, (FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 0, FillModeAlternate, &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, -1, FillModeAlternate, &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 2, (FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 2, (FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 2, FillModeAlternate, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, -1, FillModeAlternate, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 2, (FillMode)(FillModeWinding + 1), NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2 (rectPoints, rectTypes, 2, (FillMode)(FillModeAlternate - 1), NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createPath2I ()
{
	GpStatus status;
	GpPath *path;
	
	Point rectPoints[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{11, 2},
		{14, 2},
		{14, 6},
		{11, 6},
		{21, 2},
		{24, 2},
		{24, 6},
		{21, 6},
	};
	PointF rectPointsF[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{11, 2},
		{14, 2},
		{14, 6},
		{11, 6},
		{21, 2},
		{24, 2},
		{24, 6},
		{21, 6},
	};
	BYTE rectTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE openRectTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	BYTE multiOpenAndClosedRectTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	Point bezierPoints[] = {
		{1, 2},
		{2, 4},
		{3, 6},
		{4, 8},
		{11, 2},
		{12, 4},
		{13, 6},
		{14, 8},
		{21, 2},
		{22, 4},
		{23, 6},
		{24, 8},
	};
	PointF bezierPointsF[] = {
		{1, 2},
		{2, 4},
		{3, 6},
		{4, 8},
		{11, 2},
		{12, 4},
		{13, 6},
		{14, 8},
		{21, 2},
		{22, 4},
		{23, 6},
		{24, 8},
	};
	BYTE bezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE openBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier
	};
	BYTE multiOpenAndClosedBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier
	};
	Point rectBezierPoints[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{11, 2},
		{12, 4},
		{13, 6},
		{14, 8},
		{21, 2},
		{24, 2},
		{24, 6},
		{21, 6},
		{31, 2},
		{32, 4},
		{33, 6},
		{34, 8}
	};
	PointF rectBezierPointsF[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{11, 2},
		{12, 4},
		{13, 6},
		{14, 8},
		{21, 2},
		{24, 2},
		{24, 6},
		{21, 6},
		{31, 2},
		{32, 4},
		{33, 6},
		{34, 8}
	};
	BYTE rectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE openRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier
	};
	BYTE multiRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE multiOpenRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	BYTE multiOpenAndClosedRectBezierTypes[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE allStartTypes[] = {
		PathPointTypeStart,
		PathPointTypeStart,
		PathPointTypeStart,
		PathPointTypeStart | PathPointTypeCloseSubpath
	};
	BYTE invalidMidType[] = {
		PathPointTypeStart,
		(PathPointType)4,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE invalidEndType[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		(PathPointType)4 | PathPointTypeCloseSubpath
	};
	BYTE invalidPostEndType[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		(PathPointType)4
	};
	BYTE invalidBezierType2[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		(PathPointType)4,
		PathPointTypeBezier | PathPointTypeCloseSubpath
	};
	BYTE invalidBezierType3[] = {
		PathPointTypeStart,
		PathPointTypeBezier,
		PathPointTypeBezier,
		(PathPointType)4 | PathPointTypeCloseSubpath
	};
	BYTE midStartType1[] = {
		PathPointTypeStart,
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine
	};
	BYTE midStartType2[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeStart,
		PathPointTypeStart
	};
	BYTE invalidStartType[] = {
		(PathPointType)4,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	BYTE endStartType[] = {
		PathPointTypeStart,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart
	};
	BYTE openEndStartType[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeStart
	};

	// Closed lines.
	status = GdipCreatePath2I (rectPoints, rectTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 4, rectPointsF, rectTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, rectTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 4, rectPointsF, rectTypes, 4);
	GdipDeletePath (path);

	// Open lines.
	status = GdipCreatePath2I (rectPoints, rectTypes, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 0, rectPointsF, rectTypes, 2);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, rectTypes, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 0, rectPointsF, rectTypes, 2);
	GdipDeletePath (path);
	
	// Multiple closed lines.
	status = GdipCreatePath2I (rectPoints, rectTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 4, rectPointsF, rectTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 4, rectPointsF, rectTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open lines.
	status = GdipCreatePath2I (rectPoints, openRectTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 4, rectPointsF, openRectTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, openRectTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 4, rectPointsF, openRectTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open and closed lines.
	status = GdipCreatePath2I (rectPoints, multiOpenAndClosedRectTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 4, rectPointsF, multiOpenAndClosedRectTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, multiOpenAndClosedRectTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 4, rectPointsF, multiOpenAndClosedRectTypes, 12);
	GdipDeletePath (path);

	// Closed beziers.
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 6, bezierPointsF, bezierTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2I (bezierPoints, bezierTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 6, bezierPointsF, bezierTypes, 4);
	GdipDeletePath (path);

	// Open beziers.
	status = GdipCreatePath2I (bezierPoints, openBezierTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 6, bezierPointsF, openBezierTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2I (bezierPoints, openBezierTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 6, bezierPointsF, openBezierTypes, 4);
	GdipDeletePath (path);
	
	// Multiple closed beziers.
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 6, bezierPointsF, bezierTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 6, bezierPointsF, bezierTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open beziers.
	status = GdipCreatePath2I (bezierPoints, openBezierTypes, 8, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 13, 6, bezierPointsF, openBezierTypes, 8);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, openBezierTypes, 8, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 13, 6, bezierPointsF, openBezierTypes, 8);
	GdipDeletePath (path);
	
	// Multiple open and closed beziers.
	status = GdipCreatePath2I (bezierPoints, multiOpenAndClosedBezierTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 6, bezierPointsF, multiOpenAndClosedBezierTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, multiOpenAndClosedBezierTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 6, bezierPointsF, multiOpenAndClosedBezierTypes, 12);
	GdipDeletePath (path);
	
	// Closed lines and rects.
	status = GdipCreatePath2I (rectBezierPoints, rectBezierTypes, 7, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 12, 4, rectBezierPointsF, rectBezierTypes, 7);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectBezierPoints, rectBezierTypes, 7, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 12, 4, rectBezierPointsF, rectBezierTypes, 7);
	GdipDeletePath (path);
	
	// Open lines and rects.
	status = GdipCreatePath2I (rectBezierPoints, openRectBezierTypes, 7, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 12, 4, rectBezierPointsF, openRectBezierTypes, 7);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectBezierPoints, openRectBezierTypes, 7, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 12, 4, rectBezierPointsF, openRectBezierTypes, 7);
	GdipDeletePath (path);
	
	// Multiple closed lines and rects.
	status = GdipCreatePath2I (rectBezierPoints, multiRectBezierTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 6, rectBezierPointsF, multiRectBezierTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectBezierPoints, multiRectBezierTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 6, rectBezierPointsF, multiRectBezierTypes, 12);
	GdipDeletePath (path);
	
	// Multiple open lines and rects.
	status = GdipCreatePath2I (rectBezierPoints, multiOpenRectBezierTypes, 12, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 23, 6, rectBezierPointsF, multiOpenRectBezierTypes, 12);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectBezierPoints, multiOpenRectBezierTypes, 12, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 23, 6, rectBezierPointsF, multiOpenRectBezierTypes, 12);
	GdipDeletePath (path);
	
	// Multiple open and closed lines and rects.
	status = GdipCreatePath2I (rectBezierPoints, multiOpenAndClosedRectBezierTypes, 16, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 33, 6, rectBezierPointsF, multiOpenAndClosedRectBezierTypes, 16);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectBezierPoints, multiOpenAndClosedRectBezierTypes, 16, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 33, 6, rectBezierPointsF, multiOpenAndClosedRectBezierTypes, 16);
	GdipDeletePath (path);

	// Only start.
	status = GdipCreatePath2I (rectPoints, rectTypes, 1, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPointsF, rectTypes, 1);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, rectTypes, 1, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPointsF, rectTypes, 1);
	GdipDeletePath (path);

	// Invalid start type multiple - normalizes.
	status = GdipCreatePath2I (rectPoints, invalidStartType, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 1, 2, 3, 4, rectPointsF, rectTypes, 4);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, invalidStartType, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 1, 2, 3, 4, rectPointsF, rectTypes, 4);
	GdipDeletePath (path);

	// Invalid start type single - normalizes.
	status = GdipCreatePath2I (rectPoints, invalidStartType, 1, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	BYTE zeroType[] = {0};
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPointsF, zeroType, 1);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, invalidStartType, 1, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPointsF, zeroType, 1);
	GdipDeletePath (path);
	
	// End start type closed - sets to empty.
	status = GdipCreatePath2I (rectPoints, endStartType, 3, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, endStartType, 3, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	// End start type open - sets to empty.
	status = GdipCreatePath2I (rectPoints, openEndStartType, 3, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, openEndStartType, 3, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid mid type - sets to empty.
	status = GdipCreatePath2I (rectPoints, invalidMidType, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, invalidMidType, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid end type - sets to empty.
	status = GdipCreatePath2I (rectPoints, invalidEndType, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, invalidEndType, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid post end type - sets to empty.
	status = GdipCreatePath2I (rectPoints, invalidPostEndType, 5, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, invalidPostEndType, 5, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Missing 2 bezier points - sets to empty.
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Missing 1 bezier point - sets to empty.
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 3, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, bezierTypes, 3, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid second bezier point - sets to empty.
	status = GdipCreatePath2I (bezierPoints, invalidBezierType2, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, invalidBezierType2, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);

	// Invalid third bezier point - sets to empty.
	status = GdipCreatePath2I (bezierPoints, invalidBezierType3, 2, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (bezierPoints, invalidBezierType3, 2, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, NULL, NULL, 0);
	GdipDeletePath (path);
	
	// Only start - sets to empty.
	status = GdipCreatePath2I (rectPoints, allStartTypes, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPointsF, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, allStartTypes, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPointsF, NULL, 0);
	GdipDeletePath (path);
	
	// Only start open - sets to empty.
	status = GdipCreatePath2I (rectPoints, midStartType1, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPointsF, NULL, 0);
	GdipDeletePath (path);

	status = GdipCreatePath2I (rectPoints, midStartType2, 4, FillModeAlternate, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeAlternate, 0, 0, 0, 0, rectPointsF, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, midStartType1, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPointsF, NULL, 0);
	GdipDeletePath (path);
	
	status = GdipCreatePath2I (rectPoints, midStartType2, 4, FillModeWinding, &path);
	assertEqualInt (status, Ok);
	verifyPath (path, FillModeWinding, 0, 0, 0, 0, rectPointsF, NULL, 0);
	GdipDeletePath (path);

	// Negative tests.
	status = GdipCreatePath2I (NULL, rectTypes, 2, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (NULL, rectTypes, 0, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (NULL, rectTypes, -1, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (NULL, rectTypes, 2, (FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (NULL, rectTypes, 2, (FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (NULL, NULL, 0, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, NULL, 2, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, NULL, 0, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, NULL, -1, FillModeAlternate, &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, NULL, 2, (FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, NULL, 2, (FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 0, FillModeAlternate, &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, -1, FillModeAlternate, &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 2, (FillMode)(FillModeWinding + 1), &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 2, (FillMode)(FillModeAlternate - 1), &path);
	assertEqualInt (status, OutOfMemory);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 2, FillModeAlternate, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, -1, FillModeAlternate, NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 2, (FillMode)(FillModeWinding + 1), NULL);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipCreatePath2I (rectPoints, rectTypes, 2, (FillMode)(FillModeAlternate - 1), NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_addPathString ()
{
	GpStatus status;
	GpPath *path;
	const WCHAR string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
	const WCHAR emptyString[] = {'\0'};
	GpFontFamily *family;
	GpStringFormat *format;
	RectF layoutRect = {10, 20, 236, 226};

	GdipGetGenericFontFamilySerif (&family);
	GdipCreateStringFormat (0, 0, &format);

	// Valid size.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, 5, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Smaller size.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, 3, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Minus one length - not empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, -1, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Minus one length - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, emptyString, -1, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero length - not empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, 0, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero length - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, emptyString, 0, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// No such style.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, 5, family, -1, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero emSize - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, emptyString, 0, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative emSize - non-empty string.
	RectF largeLayoutRect = {100, 200, 236, 226};
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, 5, family, 0, -72, &largeLayoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative emSize - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, emptyString, 0, family, 0, -72, &largeLayoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero layout rect width.
	GdipCreatePath (FillModeAlternate, &path);

	RectF zeroWidth = {0, 0, 0, 100};
	status = GdipAddPathString (path, string, 5, family, 0, 72, &zeroWidth, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero layout rect height.
	GdipCreatePath (FillModeAlternate, &path);

	RectF zeroHeight = {0, 0, 100, 0};
	status = GdipAddPathString (path, string, 5, family, 0, 72, &zeroHeight, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative layout rect width.
	GdipCreatePath (FillModeAlternate, &path);

	RectF negativeWidth = {0, 0, -100, 100};
	status = GdipAddPathString (path, string, 5, family, 0, 72, &negativeWidth, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative layout rect height.
	GdipCreatePath (FillModeAlternate, &path);

	RectF negativeHeight = {0, 0, 100, -100};
	status = GdipAddPathString (path, string, 5, family, -1, 72, &negativeHeight, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Null format.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathString (path, string, 5, family, 0, 72, &layoutRect, NULL);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipAddPathString (NULL, string, 5, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, NULL, 5, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, -2, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, 5, NULL, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, 5, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, GenericError);

	status = GdipAddPathString (NULL, string, 5, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, NULL, 5, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, -2, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, 5, NULL, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, 5, family, 0, 0, NULL, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathString (path, string, 5, family, 0, 72, NULL, format);
	assertEqualInt (status, InvalidParameter);

  GdipDeletePath (path);
  
#if defined USE_PANGO_RENDERING || defined(USE_WINDOWS_GDIPLUS)
  const WCHAR longString[] = {'H', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e', ',', ' ', 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'l', 'o', 'n', 'g', ' ', 's', 't', 'r', 'i', 'n', 'g', '.', '\0'};
  const int fontSize = 20;
  GpImage *image;
  GpGraphics *graphics;
  GpFont *font;
  RectF longLayoutRect = {30, 40, 700, 100};
  RectF rect1, rect2;

  // Set up Graphics stuff to use MeasureString (copied from testtext.c)
  status = GdipCreateFont (family, fontSize, FontStyleRegular, UnitPixel, &font);
  assertEqualInt (status, Ok);
  status = GdipCreateBitmapFromScan0 (2000, 100, 0, PixelFormat32bppRGB, NULL, (GpBitmap **) &image);
  assertEqualInt (status, Ok);
  status = GdipGetImageGraphicsContext (image, &graphics);
  assertEqualInt (status, Ok);
  
  // Check the path bounds
  GdipSetStringFormatAlign (format, StringAlignmentFar);
  GdipSetStringFormatLineAlign (format, StringAlignmentFar);
  GdipCreatePath (FillModeAlternate, &path);
  status = GdipAddPathString (path, longString, -1, family, 0, fontSize, &longLayoutRect, format);
  assertEqualInt (status, Ok);
  status = GdipGetPathWorldBounds (path, &rect1, NULL, NULL);
  assertEqualInt (status, Ok);
  status = GdipMeasureString (graphics, longString, -1, font, &longLayoutRect, format, &rect2, NULL, NULL);
  assertEqualInt (status, Ok);
  assertSimilarFloat (rect1.X, rect2.X, 10.0);
  assertSimilarFloat (rect1.Y, rect2.Y, 10.0);
  assertSimilarFloat (rect1.X + rect1.Width, rect2.X + rect2.Width, 5.0);
  assertSimilarFloat (rect1.Y + rect1.Height, rect2.Y + rect2.Height, 5.0);
  GdipDeletePath (path);
  
  // Dispose the Graphics stuff
  GdipDeleteGraphics (graphics);
  GdipDeleteFont (font);
  GdipDisposeImage (image);  
#endif
  
  GdipDeleteFontFamily (family);
  GdipDeleteStringFormat (format);
}

static void test_addPathStringI ()
{
	GpStatus status;
	GpPath *path;
	const WCHAR string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
	const WCHAR emptyString[] = {'\0'};
	GpFontFamily *family;
	GpStringFormat *format;
	Rect layoutRect = {10, 20, 236, 226};

	GdipGetGenericFontFamilySerif (&family);
	GdipCreateStringFormat (0, 0, &format);

	// Valid size.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, string, 5, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Minus one length - not empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, string, -1, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	// Minus one length - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, emptyString, -1, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero length - not empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, string, 0, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero length - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, emptyString, 0, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// No such style.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, string, 5, family, -1, 72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero emSize - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, emptyString, 0, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative emSize - non-empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, string, 5, family, 0, -72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative emSize - empty string.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, emptyString, 0, family, 0, -72, &layoutRect, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero layout rect width.
	GdipCreatePath (FillModeAlternate, &path);

	Rect zeroWidth = {0, 0, 0, 100};
	status = GdipAddPathStringI (path, string, 5, family, 0, 72, &zeroWidth, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Zero layout rect height.
	GdipCreatePath (FillModeAlternate, &path);

	Rect zeroHeight = {0, 0, 100, 0};
	status = GdipAddPathStringI (path, string, 5, family, 0, 72, &zeroHeight, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative layout rect width.
	GdipCreatePath (FillModeAlternate, &path);

	Rect negativeWidth = {0, 0, -100, 100};
	status = GdipAddPathStringI (path, string, 5, family, 0, 72, &negativeWidth, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Negative layout rect height.
	GdipCreatePath (FillModeAlternate, &path);

	Rect negativeHeight = {0, 0, 100, -100};
	status = GdipAddPathStringI (path, string, 5, family, -1, 72, &negativeHeight, format);
	assertEqualInt (status, Ok);

	GdipDeletePath (path);

	// Null format.
	GdipCreatePath (FillModeAlternate, &path);

	status = GdipAddPathStringI (path, string, 5, family, 0, 72, &layoutRect, NULL);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipAddPathStringI (NULL, string, 5, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, NULL, 5, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, -2, family, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, 5, NULL, 0, 72, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, 5, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, GenericError);

	status = GdipAddPathStringI (NULL, string, 5, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, NULL, 5, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, -2, family, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, 5, NULL, 0, 0, &layoutRect, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, 5, family, 0, 0, NULL, format);
	assertEqualInt (status, InvalidParameter);

	status = GdipAddPathStringI (path, string, 5, family, 0, 72, NULL, format);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (path);
	GdipDeleteFontFamily (family);
	GdipDeleteStringFormat (format);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_createPath ();
	test_createPath2 ();
	test_createPath2I ();
	test_addPathString ();
	test_addPathStringI ();

	SHUTDOWN;
	return 0;
}
