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

static void test_createPathIter ()
{
	GpStatus status;
	GpPath *path;
	GpPathIterator *iterator;
	GdipCreatePath (FillModeAlternate, &path);

	// No path.
	status = GdipCreatePathIter (&iterator, NULL);
	assertEqualInt (status, Ok);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	assertEqualInt (status, Ok);
	GdipDeletePathIter (iterator);

	GdipDeletePath (path);
}

static void test_deletePathIter ()
{
	GpStatus status;
	GpPath *path;
	GpPathIterator *iterator;
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreatePathIter (&iterator, path);

	status = GdipDeletePathIter (iterator);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeletePathIter (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (path);
}

static void test_pathIterCopyData ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPathIterator *iterator;
	INT resultCount;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	GdipCreatePath (FillModeAlternate, &multiPath);
	PointF multiPoints[] = {
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
		{21, 6}
	};
	BYTE multiTypes[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (multiPoints, multiTypes, sizeof(multiTypes), FillModeAlternate, &multiPath);
	PointF points[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
	};
	BYTE types[] = {
		0, 0, 0, 0, 0
	};

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, -1, 0);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, -1);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 1, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	// Empty path.
	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, -1, 0);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, -1);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 1, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	// Path.
	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	PointF expectedPoints1[] = {
		{1, 2},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints1, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes1[] = {
		0, 0, 0, 0, 0
	};
	assertEqualBytes (types, expectedTypes1, sizeof (types));

	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 2);
	PointF expectedPoints2[] = {
		{1, 2},
		{4, 2},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints2, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes2[] = {
		PathPointTypeStart, PathPointTypeLine, 0, 0, 0
	};
	assertEqualBytes (types, expectedTypes2, sizeof (types));

	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	PointF expectedPoints3[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints3, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes3[] = {
		PathPointTypeStart, PathPointTypeLine, PathPointTypeLine, PathPointTypeLine | PathPointTypeCloseSubpath, 0
	};
	assertEqualBytes (types, expectedTypes3, sizeof (types));
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, 4);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, -1, 0);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 0, -1);
	assertEqualInt (status, Ok);
	assertEqualInt(0, resultCount);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterCopyData (iterator, &resultCount, points, types, 1, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	// Copy then enumerate - path.
	GdipCreatePathIter (&iterator, path);
	PointF points2[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
	};
	BYTE types2[] = {
		0, 0, 0, 0, 0
	};
	status = GdipPathIterCopyData (iterator, &resultCount, points2, types2, 0, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	assertEqualPointsF(points2, expectedPoints1, sizeof (points2) / sizeof (PointF));
	assertEqualBytes (types2, expectedTypes1, sizeof (types2));

	status = GdipPathIterEnumerate (iterator, &resultCount, points2, types2, 2);
	assertEqualInt (resultCount, 2);
	PointF expectedPoints4[] = {
		{1, 2},
		{4, 2},
		{0, 0},
		{0, 0},
		{0, 0},
	};
	assertEqualPointsF(points2, expectedPoints4, sizeof (points2) / sizeof (PointF));
	BYTE expectedTypes4[] = {
		PathPointTypeStart, PathPointTypeLine, 0, 0, 0
	};
	assertEqualBytes (types2, expectedTypes4, sizeof (types2));

	GdipDeletePathIter (iterator);

	// Copy then enumerate - multi path.
	GdipCreatePathIter (&iterator, multiPath);
	status = GdipPathIterCopyData (iterator, &resultCount, points2, types2, 0, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	PointF expectedPoints5[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{0, 0},
	};
	assertEqualPointsF(points2, expectedPoints5, sizeof (points2) / sizeof (PointF));
	BYTE expectedTypes5[] = {
		PathPointTypeStart, PathPointTypeLine, PathPointTypeLine, PathPointTypeLine | PathPointTypeCloseSubpath, 0
	};
	assertEqualBytes (types2, expectedTypes5, sizeof (types2));

	status = GdipPathIterEnumerate (iterator, &resultCount, points2, types2, 2);
	assertEqualInt (resultCount, 2);
	PointF expectedPoints6[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{0, 0},
	};
	assertEqualPointsF(points2, expectedPoints6, sizeof (points2) / sizeof (PointF));
	BYTE expectedTypes6[] = {
		PathPointTypeStart, PathPointTypeLine, PathPointTypeLine, PathPointTypeLine | PathPointTypeCloseSubpath, 0
	};
	assertEqualBytes (types2, expectedTypes6, sizeof (types2));

	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	status = GdipPathIterCopyData (NULL, &resultCount, points, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, NULL, points, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, NULL, points, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, NULL, points, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, &resultCount, NULL, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, &resultCount, NULL, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, &resultCount, NULL, types, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, &resultCount, points, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, &resultCount, points, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterCopyData (iterator, &resultCount, points, NULL, 0, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
}
static void test_pathIterEnumerate ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPathIterator *iterator;
	INT resultCount;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	PointF points[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
	};
	BYTE types[] = {
		0, 0, 0, 0, 0
	};

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	// Empty path.
	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	// Path.
	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 0);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	PointF expectedPoints1[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints1, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes1[] = {
		0, 0, 0, 0, 0
	};
	assertEqualBytes (types, expectedTypes1, sizeof (types));

	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	PointF expectedPoints2[] = {
		{1, 2},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints2, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes2[] = {
		PathPointTypeStart, 0, 0, 0, 0
	};
	assertEqualBytes (types, expectedTypes2, sizeof (types));

	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 3);
	PointF expectedPoints3[] = {
		{4, 2},
		{4, 6},
		{1, 6},
		{0, 0},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints3, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes3[] = {
		PathPointTypeLine, PathPointTypeLine, PathPointTypeLine | PathPointTypeCloseSubpath, 0, 0
	};
	assertEqualBytes (types, expectedTypes3, sizeof (types));

	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualPointsF(points, expectedPoints3, sizeof (points) / sizeof (PointF));
	assertEqualBytes (types, expectedTypes3, sizeof (types));
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 4);
	assertEqualInt (status, Ok);
	assertEqualInt(4, resultCount);
	PointF expectedPoints4[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints4, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes4[] = {
		PathPointTypeStart, PathPointTypeLine, PathPointTypeLine, PathPointTypeLine | PathPointTypeCloseSubpath, 0
	};
	assertEqualBytes (types, expectedTypes4, sizeof (types));
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 5);
	assertEqualInt (status, Ok);
	assertEqualInt(4, resultCount);
	assertEqualPointsF(points, expectedPoints4, sizeof (points) / sizeof (PointF));
	assertEqualBytes (types, expectedTypes4, sizeof (types));
	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (NULL, &resultCount, points, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, NULL, points, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, NULL, points, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, NULL, points, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, NULL, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, NULL, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, NULL, types, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, points, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, points, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, points, NULL, 0);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, points, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, points, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterEnumerate (iterator, &resultCount, points, NULL, -1);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
}

static void test_pathIterGetCount ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPathIterator *iterator;
	INT count;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterGetCount (iterator, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterGetCount (iterator, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	status = GdipPathIterGetCount (iterator, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 4);
	GdipDeletePathIter (iterator);

	// Negative tests.
	status = GdipPathIterGetCount (NULL, &count);
	assertEqualInt (status, InvalidParameter);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterGetCount (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterGetCount (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterGetCount (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
}

static void test_pathIterGetSubpathCount ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPathIterator *iterator;
	INT subpathCount;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	GdipCreatePath (FillModeAlternate, &multiPath);
	PointF points[] = {
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
		{21, 6}
	};
	BYTE types[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (points, types, sizeof(types), FillModeAlternate, &multiPath);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterGetSubpathCount (iterator, &subpathCount);
	assertEqualInt (status, Ok);
	assertEqualInt (subpathCount, 0);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterGetSubpathCount (iterator, &subpathCount);
	assertEqualInt (status, Ok);
	assertEqualInt (subpathCount, 0);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	status = GdipPathIterGetSubpathCount (iterator, &subpathCount);
	assertEqualInt (status, Ok);
	assertEqualInt (subpathCount, 1);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, multiPath);
	status = GdipPathIterGetSubpathCount (iterator, &subpathCount);
	assertEqualInt (status, Ok);
	assertEqualInt (subpathCount, 2);
	GdipDeletePathIter (iterator);

	// Negative tests.
	status = GdipPathIterGetSubpathCount (NULL, &subpathCount);
	assertEqualInt (status, InvalidParameter);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterGetSubpathCount (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterGetSubpathCount (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterGetSubpathCount (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
}

static void test_pathIterHasCurve ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *bezierPath;
	GpPath *openBezierPath;
	GpPathIterator *iterator;
	BOOL curve;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
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
		PathPointTypeBezier | PathPointTypeDashMode,
		PathPointTypeBezier | PathPointTypeDashMode,
		PathPointTypeBezier | PathPointTypeCloseSubpath,
		PathPointTypeStart,
		PathPointTypeBezier | PathPointTypeDashMode,
		PathPointTypeBezier | PathPointTypeDashMode,
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
	GdipCreatePath2 (bezierPoints, bezierTypes, sizeof (bezierTypes), FillModeWinding, &bezierPath);
	GdipCreatePath2 (bezierPoints, openBezierTypes, sizeof (openBezierTypes), FillModeWinding, &openBezierPath);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterHasCurve (iterator, &curve);
	assertEqualInt (status, Ok);
	assertEqualInt (curve, FALSE);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterHasCurve (iterator, &curve);
	assertEqualInt (status, Ok);
	assertEqualInt (curve, FALSE);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	status = GdipPathIterHasCurve (iterator, &curve);
	assertEqualInt (status, Ok);
	assertEqualInt (curve, FALSE);
	GdipDeletePathIter (iterator);

	status = GdipCreatePathIter (&iterator, bezierPath);
	status = GdipPathIterHasCurve (iterator, &curve);
	assertEqualInt (status, Ok);
	assertEqualInt (curve, TRUE);
	GdipDeletePathIter (iterator);

	status = GdipCreatePathIter (&iterator, openBezierPath);
	status = GdipPathIterHasCurve (iterator, &curve);
	assertEqualInt (status, Ok);
	assertEqualInt (curve, TRUE);
	GdipDeletePathIter (iterator);

	// Negative tests.
	status = GdipPathIterHasCurve (NULL, &curve);
	assertEqualInt (status, InvalidParameter);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterHasCurve (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterHasCurve (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterHasCurve (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (bezierPath);
	GdipDeletePath (openBezierPath);
}

static void test_pathIterIsValid ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPathIterator *iterator;
	BOOL isValid;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterIsValid (iterator, &isValid);
	assertEqualInt (status, Ok);
	assertEqualInt (isValid, TRUE);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterIsValid (iterator, &isValid);
	assertEqualInt (status, Ok);
	assertEqualInt (isValid, TRUE);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	status = GdipPathIterIsValid (iterator, &isValid);
	assertEqualInt (status, Ok);
	assertEqualInt (isValid, TRUE);
	GdipDeletePathIter (iterator);

	// Negative tests.
	status = GdipPathIterIsValid (NULL, &isValid);
	assertEqualInt (status, InvalidParameter);

	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterIsValid (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterIsValid (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterIsValid (iterator, NULL);
	assertEqualInt (status, InvalidParameter);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
}

static void test_pathIterNextMarker ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPathIterator *iterator;
	INT resultCount;
	INT startIndex;
	INT endIndex;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreatePath (FillModeAlternate, &multiPath);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	PointF points[] = {
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
		{21, 6}
	};
	BYTE types[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypePathMarker,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (points, types, sizeof(types), FillModeAlternate, &multiPath);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 3);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, multiPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 5);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 4);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 2);
	assertEqualInt (startIndex, 5);
	assertEqualInt (endIndex, 6);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	assertEqualInt (startIndex, 7);
	assertEqualInt (endIndex, 7);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (NULL, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, NULL, &startIndex, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, NULL, &startIndex, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, NULL, &startIndex, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, NULL, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, NULL, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, NULL, &endIndex);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
}

static void test_pathIterNextMarkerPath ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPathIterator *iterator;
	GpPath *targetPath;
	INT resultCount;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreatePath (FillModeAlternate, &multiPath);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	PointF points[] = {
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
		{21, 6}
	};
	BYTE types[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypePathMarker,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (points, types, sizeof(types), FillModeAlternate, &multiPath);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	PointF targetPathPoints1[] = {
		{2, 3},
		{6, 3},
		{6, 8},
		{2, 8}
	};
	BYTE targetPathTypes1[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	PointF targetPathPoints2[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6}
	};
	BYTE targetPathTypes2[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	verifyPath (targetPath, FillModeWinding, 1, 2, 3, 4, targetPathPoints2, targetPathTypes2, sizeof (targetPathTypes2));
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, multiPath);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 5);
	// Unclear why GDI+ doesn't copy any data here.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
#endif
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 2);
	PointF targetPathPoints3[] = {
		{14, 2},
		{14, 6}
	};
	BYTE targetPathTypes3[] = {
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypePathMarker
	};
	verifyPath (targetPath, FillModeWinding, 14, 2, 0, 4, targetPathPoints3, targetPathTypes3, sizeof (targetPathTypes3));
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	PointF targetPathPoints4[] = {
		{11, 6}
	};
	BYTE targetPathTypes4[] = {
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	verifyPath (targetPath, FillModeWinding, 0, 0, 0, 0	, targetPathPoints4, targetPathTypes4, sizeof (targetPathTypes4));
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, targetPath);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	status = GdipPathIterNextMarkerPath (NULL, &resultCount, path);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	status = GdipPathIterNextMarkerPath (NULL, &resultCount, path);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, NULL);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, NULL);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	status = GdipPathIterNextMarkerPath (iterator, &resultCount, NULL);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
	GdipDeletePath (targetPath);
}

static void test_pathIterNextSubpath ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPath *openPath;
	GpPathIterator *iterator;
	INT resultCount;
	INT startIndex;
	INT endIndex;
	BOOL isClosed;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreatePath (FillModeAlternate, &multiPath);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	PointF points[] = {
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
		{21, 6}
	};
	BYTE types[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (points, types, sizeof(types), FillModeAlternate, &multiPath);
	PointF openPoints[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6}
	};
	BYTE openTypes[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	GdipCreatePath2 (openPoints, openTypes, sizeof(openTypes), FillModeAlternate, &openPath);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 3);
	assertEqualInt (isClosed, TRUE);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 0);
	assertEqualInt (isClosed, TRUE);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, multiPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 3);
	assertEqualInt (isClosed, TRUE);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;

	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	assertEqualInt (startIndex, 4);
	assertEqualInt (endIndex, 7);
	assertEqualInt (isClosed, TRUE);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 0);
	assertEqualInt (isClosed, TRUE);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, openPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 3);
	assertEqualInt (isClosed, FALSE);

	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 0);
	assertEqualInt (isClosed, TRUE);
	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (NULL, &resultCount, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, NULL, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, NULL, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, NULL, &startIndex, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, NULL, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, NULL, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, NULL, &endIndex, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, NULL, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, NULL, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, NULL, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpath (iterator, &resultCount, &startIndex, &endIndex, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (startIndex, 0xCC);
	assertEqualInt (endIndex, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
	GdipDeletePath (openPath);
}

static void test_pathIterNextSubpathPath ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPath *openPath;
	GpPathIterator *iterator;
	GpPath *targetPath;
	INT resultCount;
	BOOL isClosed;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreatePath (FillModeAlternate, &multiPath);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	PointF points[] = {
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
		{21, 6}
	};
	BYTE types[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (points, types, sizeof(types), FillModeAlternate, &multiPath);
	PointF openPoints[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6}
	};
	BYTE openTypes[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	GdipCreatePath2 (openPoints, openTypes, sizeof(openTypes), FillModeAlternate, &openPath);

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	PointF targetPathPoints1[] = {
		{2, 3},
		{6, 3},
		{6, 8},
		{2, 8}
	};
	BYTE targetPathTypes1[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Empty path.
	status = GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	PointF targetPathPoints2[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6}
	};
	BYTE targetPathTypes2[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	verifyPath (targetPath, FillModeWinding, 1, 2, 3, 4, targetPathPoints2, targetPathTypes2, sizeof (targetPathTypes2));
	assertEqualInt (isClosed, TRUE);
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (isClosed, TRUE);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, multiPath);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	verifyPath (targetPath, FillModeWinding, 1, 2, 3, 4, targetPathPoints2, targetPathTypes2, sizeof (targetPathTypes2));
	assertEqualInt (isClosed, TRUE);
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	PointF targetPathPoints3[] = {
		{11, 2},
		{14, 2},
		{14, 6},
		{11, 6}
	};
	BYTE targetPathTypes3[] = {
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	verifyPath (targetPath, FillModeWinding, 11, 2, 3, 4, targetPathPoints3, targetPathTypes3, sizeof (targetPathTypes3));
	assertEqualInt (isClosed, TRUE);
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	assertEqualInt (isClosed, TRUE);
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Path.
	status = GdipCreatePathIter (&iterator, openPath);
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 4);
	PointF targetPathPoints4[] = {
		{1, 2},
		{4, 2},
		{4, 6},
		{1, 6}
	};
	BYTE targetPathTypes4[] = {
		PathPointTypeStart,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine
	};
	verifyPath (targetPath, FillModeWinding, 1, 2, 3, 4, targetPathPoints4, targetPathTypes4, sizeof (targetPathTypes4));
	assertEqualInt (isClosed, FALSE);
	GdipDeletePath (targetPath);

	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	verifyPath (targetPath, FillModeWinding, 2, 3, 4, 5, targetPathPoints1, targetPathTypes1, sizeof (targetPathTypes1));
	assertEqualInt (isClosed, TRUE);
	GdipDeletePath (targetPath);
	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	GdipCreatePath (FillModeWinding, &targetPath);
	GdipAddPathRectangle (targetPath, 2, 3, 4, 5);
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (NULL, &resultCount, path, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, NULL, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, NULL, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, NULL, &isClosed);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 0);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	// Negative tests.
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (NULL, &resultCount, path, &isClosed);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, NULL);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, emptyPath);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipCreatePathIter (&iterator, path);
	resultCount = 0xCC;
	isClosed = 0xCC;
	status = GdipPathIterNextSubpathPath (iterator, &resultCount, targetPath, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (resultCount, 0xCC);
	assertEqualInt (isClosed, 0xCC);
	GdipDeletePathIter (iterator);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
	GdipDeletePath (openPath);
	GdipDeletePath (targetPath);
}

static void test_pathIterRewind ()
{
	GpStatus status;
	GpPath *emptyPath;
	GpPath *path;
	GpPath *multiPath;
	GpPathIterator *iterator;
	INT resultCount;
	INT startIndex;
	INT endIndex;
	GdipCreatePath (FillModeAlternate, &emptyPath);
	GdipCreatePath (FillModeAlternate, &path);
	GdipAddPathRectangle (path, 1, 2, 3, 4);
	PointF multiPoints[] = {
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
		{21, 6}
	};
	BYTE multiTypes[] = {
		PathPointTypeStart | PathPointTypeDashMode,
		PathPointTypeLine,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypeCloseSubpath,
		PathPointTypeStart | PathPointTypePathMarker,
		PathPointTypeLine,
		PathPointTypeLine | PathPointTypePathMarker,
		PathPointTypeLine | PathPointTypeCloseSubpath
	};
	GdipCreatePath2 (multiPoints, multiTypes, sizeof(multiTypes), FillModeAlternate, &multiPath);
	PointF points[] = {
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0},
	};
	BYTE types[] = {
		0, 0, 0, 0, 0
	};

	// Null.
	GdipCreatePathIter (&iterator, NULL);
	status = GdipPathIterRewind (iterator);
	assertEqualInt (status, Ok);
	GdipDeletePathIter (iterator);

	// Empty path.
	GdipCreatePathIter (&iterator, emptyPath);
	status = GdipPathIterRewind (iterator);
	assertEqualInt (status, Ok);
	GdipDeletePathIter (iterator);

	// Path.
	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterRewind (iterator);
	assertEqualInt (status, Ok);
	GdipDeletePathIter (iterator);

	// Enumerated.
	GdipCreatePathIter (&iterator, path);
	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	PointF expectedPoints[] = {
		{1, 2},
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};
	assertEqualPointsF(points, expectedPoints, sizeof (points) / sizeof (PointF));
	BYTE expectedTypes[] = {
		PathPointTypeStart, 0, 0, 0, 0
	};
	assertEqualBytes (types, expectedTypes, sizeof (types));

	status = GdipPathIterRewind (iterator);
	assertEqualInt (status, Ok);

	status = GdipPathIterEnumerate (iterator, &resultCount, points, types, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 1);
	assertEqualPointsF(points, expectedPoints, sizeof (points) / sizeof (PointF));
	assertEqualBytes (types, expectedTypes, sizeof (types));
	GdipDeletePathIter (iterator);

	// Marker.
	status = GdipCreatePathIter (&iterator, multiPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 5);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 4);

	status = GdipPathIterRewind (iterator);
	assertEqualInt (status, Ok);

	status = GdipCreatePathIter (&iterator, multiPath);
	resultCount = 0xCC;
	startIndex = 0xCC;
	endIndex = 0xCC;
	status = GdipPathIterNextMarker (iterator, &resultCount, &startIndex, &endIndex);
	assertEqualInt (status, Ok);
	assertEqualInt (resultCount, 5);
	assertEqualInt (startIndex, 0);
	assertEqualInt (endIndex, 4);
	GdipDeletePathIter (iterator);

	// Negative tests.
	status = GdipPathIterRewind (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (emptyPath);
	GdipDeletePath (path);
	GdipDeletePath (multiPath);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_createPathIter ();
	test_deletePathIter ();
	test_pathIterCopyData ();
	test_pathIterEnumerate ();
	test_pathIterGetCount ();
	test_pathIterGetSubpathCount ();
	test_pathIterHasCurve ();
	test_pathIterIsValid ();
	test_pathIterNextMarker ();
	test_pathIterNextMarkerPath ();
	test_pathIterNextSubpath ();
	test_pathIterNextSubpathPath ();
	test_pathIterRewind ();

	SHUTDOWN;
	return 0;
}
