#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#include <windows.h>
#include <GdiPlus.h>
#else
#include <GdiPlusFlat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define C(func) assert(func == Ok)

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif


static void check_reverse_points(const GpPointF *p1, const GpPointF *p2, int count)
{
	p2 += count;
	while (count--) {
		p2--;
		assert (p1->X == p2->X);
		assert (p1->Y == p2->Y);
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
	assert (count == 0);

	// Reverse an empty path
	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == 0);

	// Path with one line
	C (GdipAddPathLine (path, 0, 10, 1, 11));
	C (GdipGetPointCount (path, &count));
	assert (count == 2);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[0] == PathPointTypeStart);
	assert (types[1] == PathPointTypeLine);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == PathPointTypeStart);
	assert (reverse_types[1] == PathPointTypeLine);

	GdipResetPath (path);

	// Closed path with one line
	C (GdipAddPathLine (path, 0, 10, 1, 11));
	C (GdipClosePathFigure (path));
	C (GdipGetPointCount (path, &count));
	assert (count == 2);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[0] == PathPointTypeStart);
	assert (types[1] == (PathPointTypeLine | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == PathPointTypeStart);
	assert (reverse_types[1] == (PathPointTypeLine | PathPointTypeCloseSubpath));

	GdipResetPath (path);

	// Rectangle
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipGetPointCount (path, &count));
	assert (count == 4);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[0] == PathPointTypeStart);
	assert (types[1] == PathPointTypeLine);
	assert (types[2] == PathPointTypeLine);
	assert (types[3] == (PathPointTypeLine | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == PathPointTypeStart);
	assert (reverse_types[1] == PathPointTypeLine);
	assert (reverse_types[2] == PathPointTypeLine);
	assert (reverse_types[3] == (PathPointTypeLine | PathPointTypeCloseSubpath));

	GdipResetPath (path);

	// Rectangle, marker
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipSetPathMarker (path));
	C (GdipGetPointCount (path, &count));
	assert (count == 4);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[0] == PathPointTypeStart);
	assert (types[1] == PathPointTypeLine);
	assert (types[2] == PathPointTypeLine);
	assert (types[3] == (PathPointTypeLine | PathPointTypeCloseSubpath | PathPointTypePathMarker));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == PathPointTypeStart);
	assert (reverse_types[1] == PathPointTypeLine);
	assert (reverse_types[2] == PathPointTypeLine);
	assert (reverse_types[3] == (PathPointTypeLine | PathPointTypeCloseSubpath));

	GdipResetPath (path);


	// Elipse and rectangle 
	C (GdipAddPathEllipse (path, 50, 51, 50, 100));
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipGetPointCount (path, &count));
	assert (count == 17);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[13] == PathPointTypeStart);
	assert (types[14] == PathPointTypeLine);
	assert (types[15] == PathPointTypeLine);
	assert (types[16] == (PathPointTypeLine | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == types[13]);
	assert (reverse_types[1] == (types[16] & ~PathPointTypeCloseSubpath));
	assert (reverse_types[2] == types[15]);
	assert (reverse_types[3] == (types[14] | PathPointTypeCloseSubpath));

	GdipResetPath (path);

	// Line, marker, bezier
	C (GdipAddPathLine (path, 0, 1, 2, 3));
	C (GdipSetPathMarker (path));
	C (GdipClosePathFigure (path));
	C (GdipAddPathBezier (path, 5, 6, 7, 8, 9, 10, 11, 12));
	C (GdipClosePathFigure (path));
	C (GdipGetPointCount (path, &count));
	assert (count == 6);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[0] == PathPointTypeStart);
	assert (types[1] == (PathPointTypeLine | PathPointTypePathMarker | PathPointTypeCloseSubpath));
	assert (types[2] == PathPointTypeStart);
	assert (types[3] == PathPointTypeBezier);
	assert (types[4] == PathPointTypeBezier);
	assert (types[5] == (PathPointTypeBezier | PathPointTypeCloseSubpath));

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == types[2]);
	assert (reverse_types[1] == types[4]);
	assert (reverse_types[2] == types[3]);
	assert (reverse_types[3] == (types[5] | PathPointTypePathMarker));
	assert (reverse_types[4] == types[0]);
	assert (reverse_types[5] == (types[1] & ~PathPointTypePathMarker));

	GdipResetPath (path);

	// Line, marker, line, bezier
	C (GdipAddPathLine (path, 0, 1, 2, 3));
	C (GdipSetPathMarker (path));
	C (GdipStartPathFigure (path));
	C (GdipAddPathLine (path, 20, 21, 22, 23));
	C (GdipAddPathBezier (path, 5, 6, 7, 8, 9, 10, 11, 12));
	C (GdipGetPointCount (path, &count));
	assert (count == 8);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	assert (types[0] == PathPointTypeStart);
	assert (types[1] == (PathPointTypeLine | PathPointTypePathMarker));
	assert (types[2] == PathPointTypeStart);
	assert (types[3] == PathPointTypeLine);
	assert (types[4] == PathPointTypeLine);
	assert (types[5] == PathPointTypeBezier);
	assert (types[6] == PathPointTypeBezier);
	assert (types[7] == PathPointTypeBezier);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	assert (reverse_count == count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	assert (reverse_types[0] == PathPointTypeStart);
	assert (reverse_types[1] == PathPointTypeBezier);
	assert (reverse_types[2] == PathPointTypeBezier);
	assert (reverse_types[3] == PathPointTypeBezier);
	assert (reverse_types[4] == PathPointTypeLine);
	assert (reverse_types[5] == (PathPointTypeLine | PathPointTypePathMarker));
	assert (reverse_types[6] == PathPointTypeStart);
	assert (reverse_types[7] == PathPointTypeLine);
	

	C (GdipDeletePath (path));
}


int
main(int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	test_gdip_reversepath();

	GdiplusShutdown(gdiplusToken);
	return 0;
}
