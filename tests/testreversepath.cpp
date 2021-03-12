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

#define C(func) ASSERT_EQ (Ok, func)

static void check_reverse_points(const GpPointF *p1, const GpPointF *p2, int count)
{
	p2 += count;
	while (count--) {
		p2--;
		ASSERT_FLOAT_EQ_NAN (p2->X, p1->X);
		ASSERT_FLOAT_EQ_NAN (p2->Y, p1->Y);
		p1++;
	}
}

TEST(ReversePath, GdipReversepath)
{
	STARTUP

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
	ASSERT_EQ (0, count);

	// Reverse an empty path
	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (0, reverse_count);

	// Path with one line
	C (GdipAddPathLine (path, 0, 10, 1, 11));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (2, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[0]);
	ASSERT_EQ (PathPointTypeLine, types[1]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (PathPointTypeStart, reverse_types[0]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[1]);

	GdipResetPath (path);

	// Closed path with one line
	C (GdipAddPathLine (path, 0, 10, 1, 11));
	C (GdipClosePathFigure (path));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (2, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[0]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath), types[1]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (PathPointTypeStart, reverse_types[0]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath), reverse_types[1]);

	GdipResetPath (path);

	// Rectangle
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (4, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[0]);
	ASSERT_EQ (PathPointTypeLine, types[1]);
	ASSERT_EQ (PathPointTypeLine, types[2]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath), types[3]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (PathPointTypeStart, reverse_types[0]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[1]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[2]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath), reverse_types[3]);

	GdipResetPath (path);

	// Rectangle, marker
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipSetPathMarker (path));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (4, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[0]);
	ASSERT_EQ (PathPointTypeLine, types[1]);
	ASSERT_EQ (PathPointTypeLine, types[2]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath | PathPointTypePathMarker), types[3]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (PathPointTypeStart, reverse_types[0]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[1]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[2]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath), reverse_types[3]);

	GdipResetPath (path);


	// Elipse and rectangle
	C (GdipAddPathEllipse (path, 50, 51, 50, 100));
	C (GdipAddPathRectangle (path, 200, 201, 60, 61));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (17, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[13]);
	ASSERT_EQ (PathPointTypeLine, types[14]);
	ASSERT_EQ (PathPointTypeLine, types[15]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypeCloseSubpath), types[16]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (types[13], reverse_types[0]);
	ASSERT_EQ ((types[16] & ~PathPointTypeCloseSubpath), reverse_types[1]);
	ASSERT_EQ (types[15], reverse_types[2]);
	ASSERT_EQ ((types[14] | PathPointTypeCloseSubpath), reverse_types[3]);

	GdipResetPath (path);

	// Line, marker, bezier
	C (GdipAddPathLine (path, 0, 1, 2, 3));
	C (GdipSetPathMarker (path));
	C (GdipClosePathFigure (path));
	C (GdipAddPathBezier (path, 5, 6, 7, 8, 9, 10, 11, 12));
	C (GdipClosePathFigure (path));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (6, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[0]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypePathMarker | PathPointTypeCloseSubpath), types[1]);
	ASSERT_EQ (PathPointTypeStart, types[2]);
	ASSERT_EQ (PathPointTypeBezier, types[3]);
	ASSERT_EQ (PathPointTypeBezier, types[4]);
	ASSERT_EQ ((PathPointTypeBezier | PathPointTypeCloseSubpath), types[5]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (types[2], reverse_types[0]);
	ASSERT_EQ (types[4], reverse_types[1]);
	ASSERT_EQ (types[3], reverse_types[2]);
	ASSERT_EQ ((types[5] | PathPointTypePathMarker), reverse_types[3]);
	ASSERT_EQ (types[0], reverse_types[4]);
	ASSERT_EQ ((types[1] & ~PathPointTypePathMarker), reverse_types[5]);

	GdipResetPath (path);

	// Line, marker, line, bezier
	C (GdipAddPathLine (path, 0, 1, 2, 3));
	C (GdipSetPathMarker (path));
	C (GdipStartPathFigure (path));
	C (GdipAddPathLine (path, 20, 21, 22, 23));
	C (GdipAddPathBezier (path, 5, 6, 7, 8, 9, 10, 11, 12));
	C (GdipGetPointCount (path, &count));
	ASSERT_EQ (8, count);
	C (GdipGetPathTypes (path, types, count));
	C (GdipGetPathPoints (path, points, count));
	ASSERT_EQ (PathPointTypeStart, types[0]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypePathMarker), types[1]);
	ASSERT_EQ (PathPointTypeStart, types[2]);
	ASSERT_EQ (PathPointTypeLine, types[3]);
	ASSERT_EQ (PathPointTypeLine, types[4]);
	ASSERT_EQ (PathPointTypeBezier, types[5]);
	ASSERT_EQ (PathPointTypeBezier, types[6]);
	ASSERT_EQ (PathPointTypeBezier, types[7]);

	C (GdipReversePath (path));
	C (GdipGetPointCount (path, &reverse_count));
	ASSERT_EQ (count, reverse_count);
	C (GdipGetPathTypes (path, reverse_types, count));
	C (GdipGetPathPoints (path, reverse_points, count));
	check_reverse_points (points, reverse_points, count);
	ASSERT_EQ (PathPointTypeStart, reverse_types[0]);
	ASSERT_EQ (PathPointTypeBezier, reverse_types[1]);
	ASSERT_EQ (PathPointTypeBezier, reverse_types[2]);
	ASSERT_EQ (PathPointTypeBezier, reverse_types[3]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[4]);
	ASSERT_EQ ((PathPointTypeLine | PathPointTypePathMarker), reverse_types[5]);
	ASSERT_EQ (PathPointTypeStart, reverse_types[6]);
	ASSERT_EQ (PathPointTypeLine, reverse_types[7]);

	C (GdipDeletePath (path));

	SHUTDOWN
}
