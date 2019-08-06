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

#define C(func) assert (func == Ok)

static void check_reverse_points(const GpPointF *p1, const GpPointF *p2, int count)
{
	p2 += count;
	while (count--) {
		p2--;
		assertEqualFloat (p1->X, p2->X);
		assertEqualFloat (p1->Y, p2->Y);
		p1++;
	}
}

static void
test_gdip_reversepath()
{
	GpPath *path;
	int count;
	int reverse_count;
	BYTE types[100];
	GpPointF points[100];
	BYTE reverse_types[100];
	GpPointF reverse_points[100];

	// Create a path
	C (GdipCreatePath (FillModeAlternate, &path));

	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  0);

	// Reverse an empty path
	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, 0);

	// Path with one line
	C (GdipAddPathLine (path, 0, 10, 1, 11));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  2);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[0], PathPointTypeStart);
	assertEqualInt (types[1], PathPointTypeLine);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], PathPointTypeStart);
	assertEqualInt (reverse_types[1], PathPointTypeLine);

	GdipResetPath (path);

	// Closed path with one line
	C (GdipAddPathLine (path, 0, 10, 1, 11));
	C (GdipClosePathFigure (path));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  2);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[0], PathPointTypeStart);
	assertEqualInt (types[1], (PathPointTypeLine | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], PathPointTypeStart);
	assertEqualInt (reverse_types[1], (PathPointTypeLine | PathPointTypeCloseSubpath));

	GdipResetPath (path);

	// Rectangle
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  4);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[0], PathPointTypeStart);
	assertEqualInt (types[1], PathPointTypeLine);
	assertEqualInt (types[2], PathPointTypeLine);
	assertEqualInt (types[3], (PathPointTypeLine | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], PathPointTypeStart);
	assertEqualInt (reverse_types[1], PathPointTypeLine);
	assertEqualInt (reverse_types[2], PathPointTypeLine);
	assertEqualInt (reverse_types[3], (PathPointTypeLine | PathPointTypeCloseSubpath));

	GdipResetPath (path);

	// Rectangle, marker
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipSetPathMarker (path));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  4);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[0], PathPointTypeStart);
	assertEqualInt (types[1], PathPointTypeLine);
	assertEqualInt (types[2], PathPointTypeLine);
	assertEqualInt (types[3], (PathPointTypeLine | PathPointTypeCloseSubpath | PathPointTypePathMarker));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], PathPointTypeStart);
	assertEqualInt (reverse_types[1], PathPointTypeLine);
	assertEqualInt (reverse_types[2], PathPointTypeLine);
	assertEqualInt (reverse_types[3], (PathPointTypeLine | PathPointTypeCloseSubpath));

	GdipResetPath (path);


	// Elipse and rectangle
	C (GdipAddPathEllipse (path, 50, 51, 50, 100));
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  17);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[13], PathPointTypeStart);
	assertEqualInt (types[14], PathPointTypeLine);
	assertEqualInt (types[15], PathPointTypeLine);
	assertEqualInt (types[16], (PathPointTypeLine | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], types[13]);
	assertEqualInt (reverse_types[1], (types[16] & ~PathPointTypeCloseSubpath));
	assertEqualInt (reverse_types[2], types[15]);
	assertEqualInt (reverse_types[3], (types[14] | PathPointTypeCloseSubpath));

	GdipResetPath (path);

	// Line, marker, bezier
	C (GdipAddPathLine (path, 0, 1, 2, 3));
	C (GdipSetPathMarker (path));
	C (GdipClosePathFigure (path));
	C (GdipAddPathBezier (path, 5, 6, 7, 8, 9, 10, 11, 12));
	C (GdipClosePathFigure (path));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  6);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[0], PathPointTypeStart);
	assertEqualInt (types[1], (PathPointTypeLine | PathPointTypePathMarker | PathPointTypeCloseSubpath));
	assertEqualInt (types[2], PathPointTypeStart);
	assertEqualInt (types[3], PathPointTypeBezier);
	assertEqualInt (types[4], PathPointTypeBezier);
	assertEqualInt (types[5], (PathPointTypeBezier | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], types[2]);
	assertEqualInt (reverse_types[1], types[4]);
	assertEqualInt (reverse_types[2], types[3]);
	assertEqualInt (reverse_types[3], (types[5] | PathPointTypePathMarker));
	assertEqualInt (reverse_types[4], types[0]);
	assertEqualInt (reverse_types[5], (types[1] & ~PathPointTypePathMarker));

	GdipResetPath (path);

	// Line, marker, line, bezier
	C (GdipAddPathLine (path, 0, 1, 2, 3));
	C (GdipSetPathMarker (path));
	C (GdipStartPathFigure (path));
	C (GdipAddPathLine (path, 20, 21, 22, 23));
	C (GdipAddPathBezier (path, 5, 6, 7, 8, 9, 10, 11, 12));
	C (GdipGetPointCount (path, &count));
	assertEqualInt (count,  8);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assertEqualInt (types[0], PathPointTypeStart);
	assertEqualInt (types[1], (PathPointTypeLine | PathPointTypePathMarker));
	assertEqualInt (types[2], PathPointTypeStart);
	assertEqualInt (types[3], PathPointTypeLine);
	assertEqualInt (types[4], PathPointTypeLine);
	assertEqualInt (types[5], PathPointTypeBezier);
	assertEqualInt (types[6], PathPointTypeBezier);
	assertEqualInt (types[7], PathPointTypeBezier);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assertEqualInt (reverse_count, count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assertEqualInt (reverse_types[0], PathPointTypeStart);
	assertEqualInt (reverse_types[1], PathPointTypeBezier);
	assertEqualInt (reverse_types[2], PathPointTypeBezier);
	assertEqualInt (reverse_types[3], PathPointTypeBezier);
	assertEqualInt (reverse_types[4], PathPointTypeLine);
	assertEqualInt (reverse_types[5], (PathPointTypeLine | PathPointTypePathMarker));
	assertEqualInt (reverse_types[6], PathPointTypeStart);
	assertEqualInt (reverse_types[7], PathPointTypeLine);

	C (GdipDeletePath (path));
}

int
main(int argc, char**argv)
{
	STARTUP;

	test_gdip_reversepath();

	SHUTDOWN;
	return 0;
}
