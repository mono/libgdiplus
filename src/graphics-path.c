/*
 * graphics-path.c
 *
 * Copyright (C) 2003-2006, Novell Inc. (http://www.novell.com)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *      Duncan Mak (duncan@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *
 * Additional copyrights:
 *	gdip_point_in_polygon is based on INPOLY.C (http://www.visibone.com/inpoly/inpoly.c.txt)
 * 	by Bob Stein (stein@visibone.com) & Craig Yap (craig@cse.fau.edu)
 *	Copyright (c) 1995-1996 Galacticomm, Inc.  Freeware source code.
 */
 
#include <math.h>
#include "gdip.h"
#include "graphics-path.h"
#include "matrix.h"

static GArray *
array_to_g_array (const GpPointF *pt, int length)
{
        GArray *p = g_array_sized_new (FALSE, TRUE, sizeof (GpPointF), length);
        g_array_append_vals (p, pt, length);
        return p;
}

static GpPointF *
g_array_to_array (GArray *p)
{
        int length = p->len;
        GpPointF *pts = (GpPointF *) GdipAlloc (sizeof (GpPointF) * length);
	if (!pts)
		return NULL;

        memcpy (pts, p->data, p->len * sizeof (GpPointF));        
        
        return pts;
}

static byte *
g_byte_array_to_array (GByteArray *p)
{
        int length = p->len;
        byte *types = (byte *) GdipAlloc (sizeof (byte) * length);
	if (!types)
		return NULL;

        memcpy (types, p->data, p->len * sizeof (byte));
        
        return types;
}

static GByteArray *
array_to_g_byte_array (const byte *types, int count)
{
        GByteArray *p = g_byte_array_sized_new (count);
        g_byte_array_append (p, types, count);
        return p;
}

/* return TRUE if the specified path has (at least one) curves, FALSE otherwise */
static BOOL
gdip_path_has_curve (GpPath *path)
{
	int i;

	if (!path)
		return FALSE;

	for (i = 0; i < path->count; i++) {
		if (g_array_index (path->types, byte, i) == PathPointTypeBezier)
			return TRUE;
	}

	return FALSE;
}

/*
 * Return the correct point type when adding a new shape to the path.
 */
static GpPathPointType
gdip_get_first_point_type (GpPath *path)
{
	GpPathPointType type;

	/* check for a new figure flag or an empty path */ 
	if (path->start_new_fig || (path->count == 0))
		return PathPointTypeStart;

	/* check if the previous point is a closure */
	type = g_array_index (path->types, byte, path->count - 1);
	if (type & PathPointTypeCloseSubpath)
		return PathPointTypeStart;
	else
		return PathPointTypeLine;
}

static void
append (GpPath *path, float x, float y, GpPathPointType type)
{
        byte t = (byte) type;
	GpPointF pt;

	pt.X = x; pt.Y = y;

	if (path->start_new_fig)
		t = PathPointTypeStart;
	/* if we closed a subpath, then start new figure and append */
	else if (path->count > 0) {
		type = g_array_index (path->types, byte, path->count - 1);
		if (type & PathPointTypeCloseSubpath)
			t = PathPointTypeStart;
        }

	g_array_append_val (path->points, pt);
	g_byte_array_append (path->types, &t, 1);
	path->count++;

	path->start_new_fig = FALSE;
}

static void
append_point (GpPath *path, GpPointF pt, GpPathPointType type)
{
        append (path, pt.X, pt.Y, type);
}

static void
append_bezier (GpPath *path, float x1, float y1, float x2, float y2, float x3, float y3)
{
        append (path, x1, y1, PathPointTypeBezier3);
        append (path, x2, y2, PathPointTypeBezier3);
        append (path, x3, y3, PathPointTypeBezier3);
}

static void
append_curve (GpPath *path, const GpPointF *points, GpPointF *tangents, int offset, int length, _CurveType type)
{
	int i;
	GpPathPointType ptype = ((type == CURVE_CLOSE) || (path->count == 0)) ? PathPointTypeStart : PathPointTypeLine;

	append_point (path, points [offset], ptype);
	for (i = offset; i < offset + length; i++) {
		int j = i + 1;

		double x1 = points [i].X + tangents [i].X;
		double y1 = points [i].Y + tangents [i].Y;

		double x2 = points [j].X - tangents [j].X;
		double y2 = points [j].Y - tangents [j].Y;

		double x3 = points [j].X;
		double y3 = points [j].Y;

		append_bezier (path, x1, y1, x2, y2, x3, y3);
        }

        if (type == CURVE_CLOSE) {
		/* complete (close) the curve using the first point */
		double x1 = points [i].X + tangents [i].X;
		double y1 = points [i].Y + tangents [i].Y;

		double x2 = points [0].X - tangents [0].X;
		double y2 = points [0].Y - tangents [0].Y;

		double x3 = points [0].X;
		double y3 = points [0].Y;

		append_bezier (path, x1, y1, x2, y2, x3, y3);
		GdipClosePathFigure (path);
	}
}

GpStatus
GdipCreatePath (GpFillMode fillMode, GpPath **path)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);

	*path = (GpPath *) GdipAlloc (sizeof (GpPath));
	if (!*path)
		return OutOfMemory;

	(*path)->fill_mode = fillMode;
	(*path)->points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
	(*path)->types = g_byte_array_new ();
	(*path)->count = 0;
	(*path)->start_new_fig = TRUE;

	return Ok;
}

GpStatus
GdipCreatePath2 (const GpPointF *points, const byte *types,
                int count, GpFillMode fillMode, GpPath **path)
{
	GArray *pts;
	GByteArray *t;

	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
	g_return_val_if_fail (types != NULL, InvalidParameter);

	/* FIXME: Check whether path types are valid before adding them. MS does
	 * some checking and does not use the points passed in, if they look
	 * invalid. Some cases are like last type being PathPointTypeStart or
	 * PathPointTypeCloseSubpath etc.
	 */
	pts = array_to_g_array (points, count);
	if (!pts)
		return OutOfMemory;

	t = array_to_g_byte_array (types, count);
        
        *path = (GpPath *) GdipAlloc (sizeof (GpPath));
	if (!*path)
		return OutOfMemory;

        (*path)->fill_mode = fillMode;
        (*path)->count = count;
        (*path)->points = pts;
        (*path)->types = t;
        
        return Ok;
}

GpStatus
GdipCreatePath2I (const GpPoint *points, const byte *types,
                int count, GpFillMode fillMode, GpPath **path)
{
	GpPointF *pt;
        GpStatus s;

	g_return_val_if_fail (points != NULL, InvalidParameter);

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;
	
	s = GdipCreatePath2 (pt, types, count, fillMode, path);

        GdipFree (pt);

        return s;
}

GpStatus
GdipClonePath (GpPath *path, GpPath **clonePath)
{
	int i;
	byte type;
	GpPointF point;

	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (clonePath != NULL, InvalidParameter);

        *clonePath = (GpPath *) GdipAlloc (sizeof (GpPath));
	if (!*clonePath)
		return OutOfMemory;

        (*clonePath)->fill_mode = path->fill_mode;
        (*clonePath)->count = path->count;
        (*clonePath)->points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
        (*clonePath)->types = g_byte_array_new ();
	for (i = 0; i < path->count; i++) {
		point = g_array_index (path->points, GpPointF, i);
		type = g_array_index (path->types, byte, i);
		g_array_append_val ((*clonePath)->points, point);
		g_byte_array_append ((*clonePath)->types, &type, 1);
	}

	(*clonePath)->start_new_fig = path->start_new_fig;

        return Ok;
}

GpStatus
GdipDeletePath (GpPath *path)
{
	if (path == NULL)
		return InvalidParameter;

	if (path->points != NULL)
		g_array_free (path->points, TRUE);
	path->points = NULL;

	if (path->types != NULL)
		g_byte_array_free (path->types, TRUE);
	path->types = NULL;

	GdipFree (path);
	return Ok;
}

GpStatus
GdipResetPath (GpPath *path)
{
	if (path == NULL)
		return InvalidParameter;

	if (path->points != NULL)
		g_array_free (path->points, TRUE);

	if (path->types != NULL)
		g_byte_array_free (path->types, TRUE);

	path->count = 0;
	path->points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
	path->types = g_byte_array_new ();
	path->fill_mode = FillModeAlternate;
	path->start_new_fig = TRUE;

	return Ok;
}

GpStatus
GdipGetPointCount (GDIPCONST GpPath *path, int *count)
{
	if (!path || !count)
		return InvalidParameter;

	*count = path->count;
	return Ok;
}

GpStatus
GdipGetPathTypes (GpPath *path, byte *types, int count)
{
	int i;

	if (!path || !types || (count < 1))
		return InvalidParameter;

        for (i = 0; i < count; i++)
                types [i] = path->types->data [i];
        
        return Ok;
}

GpStatus
GdipGetPathPoints (GDIPCONST GpPath *path, GpPointF *points, int count)
{
	int i;

	if (!path ||!points || (count < 1))
		return InvalidParameter;

        for (i = 0; i < count; i++) {
		GpPointF point = g_array_index (path->points, GpPointF, i);
                points [i].X = point.X;
                points [i].Y = point.Y;
	}

        return Ok;
}

GpStatus
GdipGetPathPointsI (GDIPCONST GpPath *path, GpPoint *points, int count)
{
	int i;
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
	if (count < 1)
		return InvalidParameter;

        for (i = 0; i < count; i++) {
                GpPoint point = g_array_index (path->points, GpPoint, i);
                points [i].X = (int) point.X;
                points [i].Y = (int) point.Y; 
        }

        return Ok;
}

GpStatus
GdipGetPathFillMode (GpPath *path, GpFillMode *fillMode)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (fillMode != NULL, InvalidParameter);

        *fillMode = path->fill_mode;
        
        return Ok;
}

GpStatus
GdipSetPathFillMode (GpPath *path, GpFillMode fillMode)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);

        path->fill_mode = fillMode;
        
        return Ok;
}

GpStatus
GdipGetPathData (GDIPCONST GpPath *path, GpPathData *pathData)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (pathData != NULL, InvalidParameter);

        pathData->Count = path->count;
        pathData->Points = g_array_to_array (path->points);
	if (!pathData->Points)
		return OutOfMemory;

        pathData->Types = g_byte_array_to_array (path->types);
        
        return Ok;
}

GpStatus
GdipStartPathFigure (GpPath *path)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);

	path->start_new_fig = TRUE;

	return Ok;
}

GpStatus
GdipClosePathFigure (GpPath *path)
{
	byte current;
	g_return_val_if_fail (path != NULL, InvalidParameter);

	if (path->count > 0) {
		current = g_array_index (path->types, byte, path->count - 1);
		g_byte_array_remove_index (path->types, path->count - 1);
		current |= PathPointTypeCloseSubpath;
		g_byte_array_append (path->types, &current, 1);
	}
	path->start_new_fig = TRUE;

	return Ok;
}

GpStatus
GdipClosePathFigures (GpPath *path)
{
	int index = 0;
	byte currentType;
	byte lastType;
	GByteArray *oldTypes;

	g_return_val_if_fail (path != NULL, InvalidParameter);

	/* first point is not closed */
	if (path->count <= 1)
		return Ok;

	oldTypes = path->types;
	path->types = g_byte_array_new ();

	lastType = g_array_index (oldTypes, byte, index);
	index++;

	for (index = 1; index < path->count; index++) {
		currentType = g_array_index (oldTypes, byte, index);
		/* we dont close on the first point */
		if ((currentType == PathPointTypeStart) && (index > 1)) {
			lastType |= PathPointTypeCloseSubpath;
			g_byte_array_append (path->types, &lastType, 1);
		}
		else
			g_byte_array_append (path->types, &lastType, 1);

		lastType = currentType;
	}

	/* close at the end */
	lastType |= PathPointTypeCloseSubpath;
	g_byte_array_append (path->types, &lastType, 1);

	path->start_new_fig = TRUE;

	g_byte_array_free (oldTypes, TRUE);

        return Ok;
}

GpStatus
GdipSetPathMarker (GpPath *path)
{
	byte current;
	g_return_val_if_fail (path != NULL, InvalidParameter);

	if (path->count == 0)
		return Ok;

	current = g_array_index (path->types, byte, path->count - 1);

        g_byte_array_remove_index (path->types, path->count - 1);

        current |= PathPointTypePathMarker;

        g_byte_array_append (path->types, &current, 1);

        return Ok;
}

GpStatus
GdipClearPathMarkers (GpPath *path)
{
        int i;
	byte current;
	GByteArray *cleared;

	g_return_val_if_fail (path != NULL, InvalidParameter);

	/* shortcut to avoid allocations */
	if (path->count == 0)
		return Ok;

	cleared = g_byte_array_new ();

        for (i = 0; i < path->count; i++) {
                current = g_array_index (path->types, byte, i);

                /* take out the marker if there is one */
                if (current & PathPointTypePathMarker)
                        current &= ~PathPointTypePathMarker;
                
                g_byte_array_append (cleared, &current, 1);                        
        }

        /* replace the existing with the cleared array */
	g_byte_array_free (path->types, TRUE);
        path->types = cleared;

        return Ok;
}

GpStatus
GdipReversePath (GpPath *path)
{
	int length, i;
	GByteArray *types;
	GArray *points;

	g_return_val_if_fail (path != NULL, InvalidParameter);

	length = path->count;
	/* shortcut to avoid allocations */
	if (length <= 1)
		return Ok;

	types = g_byte_array_sized_new (length);
	points = g_array_sized_new (FALSE, TRUE, sizeof (GpPointF), length);

        for (i = length; i > 0; i--) {
                byte t = g_array_index (path->types, byte, i);
                GpPointF pt = g_array_index (path->points, GpPointF, i);
                
                g_byte_array_append (types, &t, 1);
                g_array_append_val (points, pt);
        }
        path->points = points;
        path->types = types;
        
        return Ok;
}

GpStatus
GdipGetPathLastPoint (GpPath *path, GpPointF *lastPoint)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (lastPoint != NULL, InvalidParameter);
	g_return_val_if_fail (path->count > 0, InvalidParameter);

	*lastPoint = g_array_index (path->points, GpPointF, path->count - 1);
	return Ok;
}

GpStatus
GdipAddPathLine (GpPath *path, float x1, float y1, float x2, float y2)
{
	if (!path)
		return InvalidParameter;

	append (path, x1, y1, PathPointTypeLine);
	append (path, x2, y2, PathPointTypeLine);

	return Ok;
}

GpStatus
GdipAddPathLine2 (GpPath *path, const GpPointF *points, int count)
{
	int i;
	GpPointF *tmp;

	if (!path || !points || (count < 0))
		return InvalidParameter;

	tmp = (GpPointF *) points;

        for (i = 0; i < count; i++, tmp++)
                append_point (path, *tmp, PathPointTypeLine);
        
        return Ok;
}

static void
append_arc (GpPath *path, bool start, float x, float y, float width, float height, float startAngle, float endAngle)
{
        float rx = width / 2;
        float ry = height / 2;
        
        /* center */
        float cx = x + rx;
        float cy = y + ry;

        /* angles in radians */        
        float alpha = startAngle * PI / 180;
        float beta = endAngle * PI / 180;

        /* adjust angles for ellipses */
	alpha = atan2 (rx * sin (alpha), ry * cos (alpha));
	beta = atan2 (rx * sin (beta), ry * cos (beta));

	if (fabs (beta - alpha) > M_PI){
		if (beta > alpha)
			beta -= 2 * PI;
		else
			alpha -= 2 * PI;
	}

	float delta = beta - alpha;
        float bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta / 2);

        double sin_alpha = sin (alpha);
        double sin_beta = sin (beta);
        double cos_alpha = cos (alpha);
        double cos_beta = cos (beta);

        /* starting point */
        double sx = cx + rx * cos_alpha;
        double sy = cy + ry * sin_alpha;
	
        /* move to the starting point if we're not continuing a curve */
        if (start)
                append (path, sx, sy, PathPointTypeLine);

        append_bezier (path, 
                cx + rx * (cos_alpha - bcp * sin_alpha),
                cy + ry * (sin_alpha + bcp * cos_alpha),
                cx + rx * (cos_beta  + bcp * sin_beta),
                cy + ry * (sin_beta  - bcp * cos_beta),
                cx + rx *  cos_beta,
                cy + ry *  sin_beta);
}

static void
append_arcs (GpPath *path, float x, float y, float width, float height, float startAngle, float sweepAngle)
{
	int i;
	float drawn = 0;
	float endAngle = startAngle + sweepAngle;
	int sign = (endAngle > 0) ? 1 : -1;
	int increment = sign * 90;
	bool enough = FALSE;

	if (fabs (sweepAngle) >= 360) {
		GdipAddPathEllipse (path, x, y, width, height);
		return;
	}

	/* i is the number of sub-arcs drawn, each sub-arc can be at most 90 degrees.*/
	/* there can be no more then 4 subarcs, ie. 90 + 90 + 90 + (something less than 90) */
	for (i = 0; i < 4; i++) {
		float current = startAngle + drawn;

		if (enough)
			return;
		
		float additional;
		if (fabs (current + increment) < fabs (endAngle))
			additional = increment;  /* add the default increment */
		else {
			additional = endAngle - current; /* otherwise, add the remainder */
			enough = TRUE;
		}

		append_arc (path,
			    (i == 0) ? TRUE : FALSE,  /* only move to the starting pt in the 1st iteration */
			    x, y, width, height,      /* bounding rectangle */
			    current, current + additional);
		drawn += additional;
	}
}

GpStatus
GdipAddPathArc (GpPath *path, float x, float y, 
                float width, float height, float startAngle, float sweepAngle)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);

	/* draw the arcs */
	append_arcs (path, x, y, width, height, startAngle, sweepAngle);

        return Ok;
}

GpStatus
GdipAddPathBezier (GpPath *path, 
                   float x1, float y1, float x2, float y2, 
                   float x3, float y3, float x4, float y4)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);

        append (path, x1, y1, PathPointTypeLine);
        append_bezier (path, x2, y2, x3, y3, x4, y4);
        
        return Ok;
}

GpStatus
GdipAddPathBeziers (GpPath *path, const GpPointF *points, int count)
{
	int i;
	GpPointF *tmp = (GpPointF *) points;

	if (!path || !points)
		return InvalidParameter;

	/* first bezier requires 4 points, other 3 more points */
	if ((count < 4) || ((count % 3) != 1))
		return InvalidParameter;
        
        append_point (path, *tmp, PathPointTypeLine);
        tmp++;

        for (i = 1; i < count; i++, tmp++)
                append_point (path, *tmp, PathPointTypeBezier3);

        return Ok;
}

GpStatus
GdipAddPathCurve (GpPath *path, const GpPointF *points, int count)
{
        return GdipAddPathCurve2 (path, points, count, 0.5);
}

GpStatus
GdipAddPathCurve2 (GpPath *path, const GpPointF *points, int count, float tension)
{
        GpPointF *tangents;

	/* special case, here we support a curve with 2 points */
	if (!path || !points || (count < 2))
		return InvalidParameter;

	tangents = gdip_open_curve_tangents (CURVE_MIN_TERMS, points, count - 1, tension);
	if (!tangents)
		return OutOfMemory;

	append_curve (path, points, tangents, 0, count - 1, CURVE_OPEN);

	GdipFree (tangents);

       	return Ok;
}

GpStatus
GdipAddPathCurve3 (GpPath *path, const GpPointF *points, int count, 
        int offset, int numberOfSegments, float tension)
{
        GpPointF *tangents;
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	if (numberOfSegments < 1)
		return InvalidParameter;

	/* we need 3 points for the first curve, 2 more for each curves */
	/* and it's possible to use a point prior to the offset (to calculate) */
	if ((offset == 0) && (numberOfSegments == 1) && (count < 3))
		return InvalidParameter;
	else if (numberOfSegments >= count - offset)
		return InvalidParameter;

        tangents = gdip_open_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

        append_curve (path, points, tangents, offset, numberOfSegments, CURVE_OPEN);

        GdipFree (tangents);

        return Ok;
}

GpStatus
GdipAddPathClosedCurve (GpPath *path, const GpPointF *points, int count)
{
        return GdipAddPathClosedCurve2 (path, points, count, 0.5);
}

GpStatus
GdipAddPathClosedCurve2 (GpPath *path, const GpPointF *points, int count, float tension)
{
        GpPointF *tangents;
	g_return_val_if_fail (points != NULL, InvalidParameter);
	if (count < 3)
		return InvalidParameter;

        tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

        append_curve (path, points, tangents, 0, count - 1, CURVE_CLOSE);

	/* close the path */
	GdipClosePathFigure (path);
	GdipFree (tangents);

        return Ok;
}

GpStatus
GdipAddPathRectangle (GpPath *path, float x, float y, float width, float height)
{
	g_return_val_if_fail (path != NULL, InvalidParameter);

        append (path, x, y, PathPointTypeStart);
        append (path, x + width, y, PathPointTypeLine);
        append (path, x + width, y + height, PathPointTypeLine);
        append (path, x, y + height, PathPointTypeLine | PathPointTypeCloseSubpath);
        
        return Ok;
}

GpStatus
GdipAddPathRectangles (GpPath *path, const GpRectF *rects, int count)
{
        int i;
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (rects != NULL, InvalidParameter);

        for (i = 0; i < count; i++) {
                float x = rects[i].X;
                float y = rects[i].Y;
                float width = rects[i].Width;
                float height = rects[i].Height;
                GdipAddPathRectangle (path, x, y, width, height);
        }
        
        return Ok;
}

GpStatus
GdipAddPathEllipse (GpPath *path, float x, float y, float width, float height)
{
        float C1 = 0.552285;
        double rx = width / 2;
        double ry = height / 2;
        double cx = x + rx;
        double cy = y + ry;
	g_return_val_if_fail (path != NULL, InvalidParameter);

        /* origin */
        append (path, cx + rx, cy, PathPointTypeStart);

        /* quadrant I */
        append_bezier (path, 
                       cx + rx, cy - C1 * ry, 
                       cx + C1 * rx, cy - ry, 
                       cx, cy - ry);

        /* quadrant II */
        append_bezier (path,
                       cx - C1 * rx, cy - ry, 
                       cx - rx, cy - C1 * ry, 
                       cx - rx, cy);

        /* quadrant III */
        append_bezier (path,
                       cx - rx, cy + C1 * ry, 
                       cx - C1 * rx, cy + ry, 
                       cx, cy + ry);

        /* quadrant IV */
        append_bezier (path,
                       cx + C1 * rx, cy + ry, 
                       cx + rx, cy + C1 * ry, 
                       cx + rx, cy);

	/* close the path */
	GdipClosePathFigure (path);
        
        return Ok;
}

GpStatus
GdipAddPathPie (GpPath *path, float x, float y, float width, float height, float startAngle, float sweepAngle)
{
        float rx = width / 2;
        float ry = height / 2;

        /* center */
        int cx = x + rx;
        int cy = y + ry;	

        /* angles in radians */        
        float alpha = startAngle * PI / 180;

        /* adjust angle for ellipses */
        alpha = atan2 (rx * sin (alpha), ry * cos (alpha));

        float sin_alpha = sin (alpha);
        float cos_alpha = cos (alpha);

	g_return_val_if_fail (path != NULL, InvalidParameter);

        /* move to center */
        append (path, cx, cy, PathPointTypeStart);

        /* draw pie edge */
	if (fabs (sweepAngle) < 360)
		append (path, cx + rx * cos_alpha, cy + ry * sin_alpha,
			PathPointTypeLine);

	/* draw the arcs */
	append_arcs (path, x, y, width, height, startAngle, sweepAngle);
        
        /* draw pie edge */
	if (fabs (sweepAngle) < 360)
		append (path, cx, cy, PathPointTypeLine);

	/* close the path */
        return GdipClosePathFigure (path);
}

GpStatus
GdipAddPathPolygon (GpPath *path, const GpPointF *points, int count)
{
        int i;
        GpPointF *tmp = (GpPointF *) points;

	if (!path || !points || (count < 3))
		return InvalidParameter;
        
	append_point (path, *tmp, PathPointTypeStart);
        tmp ++;

        for (i = 1; i < count; i++, tmp++)
                append_point (path, *tmp, PathPointTypeLine);

        /*
         * Add a line from the last point back to the first point if
         * they're not the same
         */
        if (points [0].X != points [count - 1].X && points [0].Y != points [count - 1].Y)
                append_point (path, points [0], PathPointTypeLine);
        
	/* close the path */
        return GdipClosePathFigure (path);
}

GpStatus
GdipAddPathPath (GpPath *path, GpPath *addingPath, bool connect)
{
        int i, length;
	GpPathPointType first;
        GpPointF *pts;
        byte *types;

	if (!path || !addingPath)
		return InvalidParameter;

        length = addingPath->count;
        if (length < 1)
                return Ok;
        
        pts = GdipCalloc (sizeof (GpPointF), length);
        types = GdipCalloc (sizeof (byte), length);

        GdipGetPathPoints (addingPath, pts, length);
        GdipGetPathTypes (addingPath, types, length);

	/* We can connect only open figures. If first figure is closed
	 * it can't be connected.
	 */
	first = connect ? gdip_get_first_point_type (path) : PathPointTypeStart;

	append_point (path, pts [0], first); 

	for (i = 1; i < length; i++)
		append_point (path, pts [i], types [i]);

	GdipFree(pts);
	GdipFree(types);

	return Ok;
}

/* MonoTODO - deal with layoutRect, format... */
GpStatus 
GdipAddPathString (GpPath *path, GDIPCONST WCHAR *string, int length, 
	GDIPCONST GpFontFamily *family, int style, float emSize,
	GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *format)
{
	cairo_surface_t *cs;
	cairo_t *cr;
	cairo_path_t *cp;
	cairo_text_extents_t extents;
	cairo_matrix_t matrix;
	GpFont *font = NULL;
	GpStatus status;

	if (length == 0)
		return Ok;
	if (length < 0)
		return InvalidParameter;

	cs = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
	if (cairo_surface_status (cs) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy (cs);
		return OutOfMemory;
	}

	cr = cairo_create (cs);
	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS) {
		cairo_destroy (cr);
		cairo_surface_destroy (cs);
		return OutOfMemory;
	}

	unsigned char *utf8 = (unsigned char *) ucs2_to_utf8 ((const gunichar2 *)string, -1);
	if (!utf8) {
		cairo_destroy (cr);
		cairo_surface_destroy (cs);
		return OutOfMemory;
	}

	status = GdipCreateFont (family, emSize, style, UnitPixel, &font);
	if (status != Ok) {
		GdipFree (utf8);
		cairo_destroy (cr);
		cairo_surface_destroy (cs);
		return status;
	}

	cairo_set_font_face (cr, font->cairofnt);
	cairo_set_font_size (cr, font->sizeInPixels);

#if FALSE
WCHAR name[LF_FACESIZE];
GdipGetFamilyName (family, name, 0);
g_warning ("GdipAddString \"%s\" (family: %s, size %g)", utf8, ucs2_to_utf8 ((const gunichar2 *)name, -1), font->sizeInPixels);
#endif
	/* TODO - deal with layoutRect, format... ideally we would be calling a subset
	   of GdipDrawString that already does everything *and* preserve the whole path */
	if (layoutRect)
		cairo_move_to (cr, layoutRect->X, layoutRect->Y + font->sizeInPixels);
	cairo_text_path (cr, (const char*)utf8);

	/* get the font data from the cairo path and translate it as a gdi+ path */
	cp = cairo_copy_path (cr);
	if (cp) {
		int i;
		for (i=0; i < cp->num_data; i += cp->data[i].header.length) {
			GpPathPointType type = PathPointTypeStart;
			cairo_path_data_t *data = &cp->data[i];

			if ((i < cp->num_data - 1) && (data->header.type == CAIRO_PATH_CLOSE_PATH))
				type |= PathPointTypeCloseSubpath;

			switch (data->header.type) {
			case CAIRO_PATH_MOVE_TO:
				append (path, data[1].point.x, data[1].point.y, type);
				break;
			case CAIRO_PATH_LINE_TO:
				append (path, data[1].point.x, data[1].point.y, type | PathPointTypeLine);
				break;
			case CAIRO_PATH_CURVE_TO:
				append (path, data[1].point.x, data[1].point.y, PathPointTypeBezier);
				append (path, data[2].point.x, data[2].point.y, PathPointTypeBezier);
				append (path, data[3].point.x, data[3].point.y, type | PathPointTypeBezier);
				break;
			case CAIRO_PATH_CLOSE_PATH:
				break;
			}
		}

		cairo_path_destroy (cp);
	}

	if (font) {
		GdipDeleteFont (font);
	}
	GdipFree (utf8);
	cairo_destroy (cr);
	cairo_surface_destroy (cs);
	return Ok;
}

/* MonoTODO - same limitations as GdipAddString */
GpStatus
GdipAddPathStringI (GpPath *path, GDIPCONST WCHAR *string, int length,
	GDIPCONST GpFontFamily *family, int style, float emSize,
	GDIPCONST GpRect *layoutRect, GDIPCONST GpStringFormat *format)
{
	GpRectF *r = NULL;

	if (layoutRect) {
		GpRectF rect;

		rect.X = layoutRect->X;
		rect.Y = layoutRect->Y;
		rect.Width = layoutRect->Width;
		rect.Height = layoutRect->Height;
		r = &rect;
	}
	return GdipAddPathString (path, string, length, family, style, emSize, r, format);
}

GpStatus
GdipAddPathLineI (GpPath *path, int x1, int y1, int x2, int y2)
{
	return GdipAddPathLine (path, x1, y1, x2, y2);
}

GpStatus
GdipAddPathLine2I (GpPath* path, const GpPoint *points, int count)
{
	GpPoint *tmp;
	int i;

	if (!path || !points || (count < 0))
		return InvalidParameter;

        for (i = 0, tmp = (GpPoint*) points; i < count; i++, tmp++) {
                append (path, tmp->X, tmp->Y, PathPointTypeLine);
	}

	return Ok;
}

GpStatus
GdipAddPathArcI (GpPath *path, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
        return GdipAddPathArc (path, x, y, width, height, startAngle, sweepAngle);
}

GpStatus
GdipAddPathBezierI (GpPath *path, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	return GdipAddPathBezier (path, x1, y1, x2, y2, x3, y3, x4, y4);
}

GpStatus
GdipAddPathBeziersI (GpPath *path, const GpPoint *points, int count)
{
	GpPoint *tmp;
	int i;

	if (!path || !points)
		return InvalidParameter;

	/* first bezier requires 4 points, other 3 more points */
	if ((count < 4) || ((count % 3) != 1))
		return InvalidParameter;
        
	tmp = (GpPoint*) points;
        append (path, tmp->X, tmp->Y, PathPointTypeLine);
        tmp++;

        for (i = 1; i < count; i++, tmp++)
                append (path, tmp->X, tmp->Y, PathPointTypeBezier3);

        return Ok;
}

GpStatus
GdipAddPathCurveI (GpPath *path, const GpPoint *points, int count)
{
	return GdipAddPathCurve2I (path, points, count, 0.5);
}

GpStatus
GdipAddPathCurve2I (GpPath *path, const GpPoint *points, int count, float tension)
{
	GpPointF *pt;
	Status s;

	if (!points)
		return InvalidParameter;

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;

	/* here we must deal/accept curve with 2 points, GdipAddPathCurve3I doesn't */
	s = GdipAddPathCurve2 (path, pt, count, tension);

	GdipFree (pt);
	return s;
}

GpStatus
GdipAddPathCurve3I (GpPath *path, const GpPoint *points,
                    int count, int offset, int numberOfSegments, float tension)
{
	GpPointF *pt;
	Status s;

	if (!points)
		return InvalidParameter;

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;

	s = GdipAddPathCurve3 (path, pt, count, offset, numberOfSegments, tension);

	GdipFree (pt);
	return s;
}

GpStatus
GdipAddPathClosedCurveI (GpPath *path, const GpPoint *points, int count)
{
        return GdipAddPathClosedCurve2I (path, points, count, 0.5);
}

GpStatus
GdipAddPathClosedCurve2I (GpPath *path, const GpPoint *points, int count, float tension)
{
	GpPointF *pt;
	Status s;

	g_return_val_if_fail (points != NULL, InvalidParameter);

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;

	s = GdipAddPathClosedCurve2 (path, pt, count, tension);

        GdipFree (pt);

        return s;
}

GpStatus
GdipAddPathRectangleI (GpPath *path, int x, int y, int width, int height)
{
        return GdipAddPathRectangle (path, x, y, width, height);
}

GpStatus
GdipAddPathRectanglesI (GpPath *path, const GpRect *rects, int count)
{
        int i;
	g_return_val_if_fail (rects != NULL, InvalidParameter);

        for (i = 0; i < count; i++) {
                float x = (float) rects[i].X;
                float y = (float) rects[i].Y;
                float width =  (float) rects[i].Width;
                float height =  (float) rects[i].Height;
                GdipAddPathRectangle (path, x, y, width, height);
        }

        return Ok;
}

GpStatus
GdipAddPathEllipseI (GpPath *path, int x, int y, int width, int height)
{
        return GdipAddPathEllipse (path, x, y, width, height);
}

GpStatus
GdipAddPathPieI (GpPath *path, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
        return GdipAddPathPie (path, x, y, width, height, startAngle, sweepAngle);
}

GpStatus
GdipAddPathPolygonI (GpPath *path, const GpPoint *points, int count)
{
        int i;
        GpPoint *tmp;

	if (!path || !points || (count < 3))
		return InvalidParameter;

	tmp = (GpPoint *) points;
	append (path, tmp->X, tmp->Y, PathPointTypeStart);
        tmp ++;

        for (i = 1; i < count; i++, tmp++)
                append (path, tmp->X, tmp->Y, PathPointTypeLine);

        /*
         * Add a line from the last point back to the first point if
         * they're not the same
         */
        if (points [0].X != points [count - 1].X && points [0].Y != points [count - 1].Y)
                append (path, points [0].X, points [0].Y, PathPointTypeLine);
        
	/* close the path */
        return GdipClosePathFigure (path);
}

/* nr_curve_flatten comes from Sodipodi's libnr (public domain) available from http://www.sodipodi.com/ */
/* Mono changes: converted to float (from double), added recursion limit, use GArray */
static BOOL
nr_curve_flatten (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float flatness, int level, GArray *points)
{
	float dx1_0, dy1_0, dx2_0, dy2_0, dx3_0, dy3_0, dx2_3, dy2_3, d3_0_2;
	float s1_q, t1_q, s2_q, t2_q, v2_q;
	float f2, f2_q;
	float x00t, y00t, x0tt, y0tt, xttt, yttt, x1tt, y1tt, x11t, y11t;

	dx1_0 = x1 - x0;
	dy1_0 = y1 - y0;
	dx2_0 = x2 - x0;
	dy2_0 = y2 - y0;
	dx3_0 = x3 - x0;
	dy3_0 = y3 - y0;
	dx2_3 = x3 - x2;
	dy2_3 = y3 - y2;
	f2 = flatness;
	d3_0_2 = dx3_0 * dx3_0 + dy3_0 * dy3_0;
	if (d3_0_2 < f2) {
		float d1_0_2, d2_0_2;
		d1_0_2 = dx1_0 * dx1_0 + dy1_0 * dy1_0;
		d2_0_2 = dx2_0 * dx2_0 + dy2_0 * dy2_0;
		if ((d1_0_2 < f2) && (d2_0_2 < f2)) {
			goto nosubdivide;
		} else {
			goto subdivide;
		}
	}
	f2_q = f2 * d3_0_2;
	s1_q = dx1_0 * dx3_0 + dy1_0 * dy3_0;
	t1_q = dy1_0 * dx3_0 - dx1_0 * dy3_0;
	s2_q = dx2_0 * dx3_0 + dy2_0 * dy3_0;
	t2_q = dy2_0 * dx3_0 - dx2_0 * dy3_0;
	v2_q = dx2_3 * dx3_0 + dy2_3 * dy3_0;
	if ((t1_q * t1_q) > f2_q) goto subdivide;
	if ((t2_q * t2_q) > f2_q) goto subdivide;
	if ((s1_q < 0.0) && ((s1_q * s1_q) > f2_q)) goto subdivide;
	if ((v2_q < 0.0) && ((v2_q * v2_q) > f2_q)) goto subdivide;
	if (s1_q >= s2_q) goto subdivide;

 nosubdivide:
	{
	GpPointF pt;

	pt.X = x3;
	pt.Y = y3;
	g_array_append_val (points, pt);
	return TRUE;
	}
 subdivide:
	/* things gets *VERY* memory intensive without a limit */
	if (level >= FLATTEN_RECURSION_LIMIT)
		return FALSE;

	x00t = (x0 + x1) * 0.5;
	y00t = (y0 + y1) * 0.5;
	x0tt = (x0 + 2 * x1 + x2) * 0.25;
	y0tt = (y0 + 2 * y1 + y2) * 0.25;
	x1tt = (x1 + 2 * x2 + x3) * 0.25;
	y1tt = (y1 + 2 * y2 + y3) * 0.25;
	x11t = (x2 + x3) * 0.5;
	y11t = (y2 + y3) * 0.5;
	xttt = (x0tt + x1tt) * 0.5;
	yttt = (y0tt + y1tt) * 0.5;

	if (!nr_curve_flatten (x0, y0, x00t, y00t, x0tt, y0tt, xttt, yttt, flatness, level+1, points)) return FALSE;
	if (!nr_curve_flatten (xttt, yttt, x1tt, y1tt, x11t, y11t, x3, y3, flatness, level+1, points)) return FALSE;
	return TRUE;
}

static BOOL
gdip_convert_bezier_to_lines (GpPath *path, int index, float flatness, GArray *flat_points, GByteArray *flat_types)
{
	GArray *points;
	GpPointF start, first, second, end;
	GpPointF pt;
	byte type;
	int i;

	if ((index <= 0) || (index + 2 >= path->count))
		return FALSE; /* bad path data */

	start = g_array_index (path->points, GpPointF, index - 1);
	first = g_array_index (path->points, GpPointF, index);
	second = g_array_index (path->points, GpPointF, index + 1);
	end = g_array_index (path->points, GpPointF, index + 2);

	/* we can't add points directly to the original list as we could end up with too much recursion */
	points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
	if (!nr_curve_flatten (start.X, start.Y, first.X, first.Y, second.X, second.Y, end.X, end.Y, flatness, 0, points)) {
		/* curved path is too complex (i.e. would result in too many points) to render as a polygon */
		g_array_free (points, TRUE);
		return FALSE;
	}

	/* recursion was within limits, append the result to the original supplied list */
	if (points->len > 0) {
		g_array_append_val (flat_points, g_array_index (points, GpPointF, 0));
		type = PathPointTypeLine;
		g_byte_array_append (flat_types, &type, 1);
	}

	/* always PathPointTypeLine */
	type = PathPointTypeLine;
	for (i = 1; i < points->len; i++) {
		pt = g_array_index (points, GpPointF, i);
		g_array_append_val (flat_points, pt);
		g_byte_array_append (flat_types, &type, 1);
	}

	g_array_free (points, TRUE);
	return TRUE;
}

GpStatus 
GdipFlattenPath (GpPath *path, GpMatrix *matrix, float flatness)
{
	GpStatus status = Ok;
	GArray *points;
	GByteArray *types;
	int i;

	g_return_val_if_fail (path != NULL, InvalidParameter);

	/* apply matrix before flattening (as there's less points at this stage) */
	if (matrix) {
		status = GdipTransformPath (path, matrix);
		if (status != Ok)
			return status;
	}

	/* if no bezier are present then the path doesn't need to be flattened */
	if (!gdip_path_has_curve (path))
		return status;

	points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
	types = g_byte_array_new ();

	/* Iterate the current path and replace each bezier with multiple lines */
	for (i = 0; i < path->count; i++) {
		GpPointF point = g_array_index (path->points, GpPointF, i);
		byte type = g_array_index (path->types, byte, i);

		/* PathPointTypeBezier3 has the same value as PathPointTypeBezier */
		if ((type & PathPointTypeBezier) == PathPointTypeBezier) {
			if (!gdip_convert_bezier_to_lines (path, i, fabs (flatness), points, types)) {
				/* uho, too much recursion - do not pass go, do not collect 200$ */
				GpPointF pt;

				/* free the the partial flat */
				g_array_free (points, TRUE);
				g_byte_array_free (types, TRUE);

				/* mimic MS behaviour when recursion becomes a problem */
				/* note: it's not really an empty rectangle as the last point isn't closing */
				points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
				types = g_byte_array_new ();

				pt.X = pt.Y = 0;
				type = PathPointTypeStart;
				g_array_append_val (points, pt);
				g_byte_array_append (types, &type, 1);

				type = PathPointTypeLine;
				g_array_append_val (points, pt);
				g_byte_array_append (types, &type, 1);

				g_array_append_val (points, pt);
				g_byte_array_append (types, &type, 1);

				g_array_append_val (points, pt);
				g_byte_array_append (types, &type, 1);
				break;
			}
			/* beziers have 4 points: the previous one, the current and the next two */
			i += 2;
		} else {
			/* no change required, just copy the point */
			g_array_append_val (points, point);
			g_byte_array_append (types, &type, 1);
		}
	}

	/* free original path points and types */
	if (path->points != NULL)
		g_array_free (path->points, TRUE);
	if (path->types != NULL)
		g_byte_array_free (path->types, TRUE);

	/* transfer new path informations */
	path->points = points;
	path->types = types;
	path->count = points->len;

	/* note: no error code is given for excessive recursion */
	return Ok;
}

static GpStatus
gdip_prepare_path (GpPath *path, GpMatrix *matrix, float flatness)
{
	/* convert any curve into lines */
	if (gdip_path_has_curve (path)) {
		/* this will apply the matrix too (before flattening) */
		return GdipFlattenPath (path, matrix, flatness);
	} else if (!gdip_is_matrix_empty (matrix)) {
		/* no curve, but we still have a matrix to apply... */
		return GdipTransformPath (path, matrix);
	}

	/* no preparation required */
	return Ok;
}

/* MonoTODO - doesn't seems to be exposed in System.Drawing.dll */
GpStatus 
GdipWindingModeOutline (GpPath *path, GpMatrix *matrix, float flatness)
{
	GpStatus status;

	if (!path)
		return InvalidParameter;

	/* quick out */
	if (path->count == 0)
		return Ok;

	status = gdip_prepare_path (path, matrix, flatness);
	if (status != Ok)
		return status;

	/* TODO */

	return NotImplemented;
}

/* MonoTODO */
GpStatus 
GdipWidenPath (GpPath *nativePath, GpPen *pen, GpMatrix *matrix, float flatness)
{
	static int called = 0;
	GpStatus status;

	if (!nativePath || !pen)
		return InvalidParameter;

	/* (0) is deal within System.Drawing */
	/* (1) for compatibility with MS GDI+ (reported as FDBK49685) */
	if (nativePath->count <= 1)
		return OutOfMemory;

	status = gdip_prepare_path (nativePath, matrix, flatness);
	if (status != Ok)
		return status;

	/* TODO inner path (same number of points as the prepared path) */

	/* TODO outer path (twice the number of points as the prepared path) */

	if (!called) {
		g_warning ("NOT IMPLEMENTED: GdipWidenPath");
		called = 1;
	}
	return Ok;
}

/* MonoTODO */
GpStatus 
GdipWarpPath (GpPath *path, GpMatrix *matrix, const GpPointF *points, int count, 
              float srcx, float srcy, float srcwidth, float srcheight,
              WarpMode warpMode, float flatness)
{
	static int called = 0;
	GpStatus status;

	if (!path || !points || (count < 1))
		return InvalidParameter;

	/* quick out */
	if (path->count == 0)
		return Ok;

	/* an invalid warp mode resets the current path */
	/* a path with a single point will reset it too */
	if (((warpMode != WarpModePerspective) && (warpMode != WarpModeBilinear)) || (path->count == 1))
		return GdipResetPath (path);

	status = gdip_prepare_path (path, matrix, flatness);
	if (status != Ok)
		return status;

	/* TODO */

	if (!called) {
		g_warning ("NOT IMPLEMENTED: GdipWarpPath");
		called = 1;
	}
	return Ok;
}

GpStatus 
GdipTransformPath (GpPath* path, GpMatrix *matrix)
{
        PointF *points;
	int count;
        Status s;

	g_return_val_if_fail (path != NULL, InvalidParameter);

	count = path->count;
	if (count == 0)
		return Ok; /* GdipTransformMatrixPoints would fail */

	/* avoid allocation/free/calculation for null/identity matrix */
	if (gdip_is_matrix_empty (matrix))
		return Ok;

	points = g_array_to_array (path->points);
	if (!points)
		return OutOfMemory;

	s = GdipTransformMatrixPoints (matrix, points, count);

        path->points = array_to_g_array (points, count);

        GdipFree (points);
	if (!path->points)
		return OutOfMemory;

        return s;
}

GpStatus 
GdipGetPathWorldBounds (GpPath *path, GpRectF *bounds, const GpMatrix *matrix, const GpPen *pen)
{
	GpStatus status;
	GpPath *workpath;

	if (!path || !bounds)
		return InvalidParameter;

	if (path->count < 1) {
		/* special case #1 - Empty */
		bounds->X = 0.0f;
		bounds->Y = 0.0f;
		bounds->Width = 0.0f;
		bounds->Height = 0.0f;
		return Ok;
	}

	status = GdipClonePath (path, &workpath);
	if (status != Ok)
		return status;

	/* We don't need a very precise flat value to get the bounds (GDI+ isn't, big time) -
	 * however flattening helps by removing curves, making the rest of the algorithm a 
	 * lot simpler.
	 */

	/* note: only the matrix is applied if no curves are present in the path */
	status = GdipFlattenPath (workpath, (GpMatrix*)matrix, 25.0f);
	if (status == Ok) {
		int i;
		GpPointF points;

		points = g_array_index (workpath->points, GpPointF, 0);
		bounds->X = points.X;		/* keep minimum X here */
		bounds->Y = points.Y;		/* keep minimum Y here */
		if (workpath->count == 1) {
			/* special case #2 - Only one element */
			bounds->Width = 0.0f;
			bounds->Height = 0.0f;
			GdipDeletePath (workpath);
			return Ok;
		}

		bounds->Width = points.X;	/* keep maximum X here */
		bounds->Height = points.Y;	/* keep maximum Y here */

		for (i = 1; i < workpath->count; i++) {
			points = g_array_index (workpath->points, GpPointF, i);
			if (points.X < bounds->X)
				bounds->X = points.X;
			if (points.Y < bounds->Y)
				bounds->Y = points.Y;
			if (points.X > bounds->Width)
				bounds->Width = points.X;
			if (points.Y > bounds->Height)
				bounds->Height = points.Y;
		}

		/* convert maximum values (width/height) as length */
		bounds->Width -= bounds->X;
		bounds->Height -= bounds->Y;

		if (pen) {
			/* in calculation the pen's width is at least 1.0 */
			float width = (pen->width < 1.0f) ? 1.0f : pen->width;
			float halfw = (width / 2);
			
			bounds->X -= halfw;
			bounds->Y -= halfw;
			bounds->Width += width;
			bounds->Height += width;
		}
	}
	GdipDeletePath (workpath);
	return status;
}

GpStatus 
GdipGetPathWorldBoundsI (GpPath *path, GpRect *bounds, const GpMatrix *matrix, const GpPen *pen)
{
	GpRectF rect;
	GpStatus status;

	if (!path || !bounds)
		return InvalidParameter;

	status = GdipGetPathWorldBounds (path, &rect, matrix, pen);
	if (status != Ok)
		return status;	

	bounds->X = (int) rect.X;
	bounds->Y = (int) rect.Y;
	bounds->Width = (int) rect.Width;
	bounds->Height = (int) rect.Height;
	return Ok;
}

/* note: round[f] is C99 */
static int
iround (float d)
{
	double int_part = floor (d);
	return ((d - int_part) >= 0.5) ? int_part + 1.0 : int_part;
}

/* adapted from http://www.visibone.com/inpoly/inpoly.c.txt */
static BOOL
gdip_point_in_polygon (GpPath *path, int start, int end, float x, float y)
{
	GpPointF old, new;
	float x1, y1, x2, y2;
	int x0 = iround (x);
	int y0 = iround (y);
	BOOL inside = FALSE;
	int npoints = end - start + 1;
	int i;

	if (npoints < 3)
		return FALSE; /* not a polygon */

	old = g_array_index (path->points, GpPointF, end);
	for (i = 0; i < npoints ; i++) {
		new = g_array_index (path->points, GpPointF, start + i);
		if (new.X > old.X) {
			x1 = old.X;
			x2 = new.X;
			y1 = old.Y;
			y2 = new.Y;
		} else {
			x1 = new.X;
			x2 = old.X;
			y1 = new.Y;
			y2 = old.Y;
		}

		if ((new.X <= x0) == (x0 < old.X) && ((y0 - y1) * (x2 - x1) < (y2 - y1) * (x0 - x1))) {
			inside = !inside;
		}
		old = new;
	}
	return inside;
}

/* MonoTODO - GpGraphics is ignored */
GpStatus 
GdipIsVisiblePathPoint (GpPath *path, float x, float y, GpGraphics *graphics, bool *result)
{
	GpStatus status = Ok;
	GpPath *workpath;
	int start, end;

	if (!path || !result)
		return InvalidParameter;

	*result = FALSE;

	/* we clone the supplied path if it contains curves (we only deal with lines) */
	if (gdip_path_has_curve (path)) {
		status = GdipClonePath (path, &workpath);
		if (status != Ok)
			return status;

		status = GdipFlattenPath (workpath, NULL, 25.0f);
	} else {
		workpath = path;
	}

	if (graphics) {
		/* FIXME - graphics isn't always ignored, e.g. when we set the matrix, pageunit and pagescale */
	}

	/* there may be multiple polygons inside a path */
	for (start = 0, end = 0; end < workpath->count && !*result; end++) {
		byte type = g_array_index (workpath->types, byte, end);
		if (type & PathPointTypeCloseSubpath) {
			*result = gdip_point_in_polygon (workpath, start, end, x, y);
		} else if (type == PathPointTypeStart) {
			/* reset the start index */
			start = end;
		}
	}

	if (workpath != path)
		GdipDeletePath (workpath);
	return status;
}

/* MonoTODO - GpGraphics is ignored */
GpStatus 
GdipIsVisiblePathPointI (GpPath *path, int x, int y, GpGraphics *graphics, bool *result)
{
	return GdipIsVisiblePathPoint (path, x, y, graphics, result);
}

static BOOL
gdip_check_point_within_distance (float x0, float y0, GpPointF *p1, GpPointF *p2, float distance)
{
	float x1 = p1->X;
	float y1 = p1->Y;
	float x2 = p2->X;
	float y2 = p2->Y;
	float x2x1, y2y1;

	/* quick out (to avoid heavy calculations) for out of range points */
	if ((x0 < min (x1, x2) - distance) || (x0 > max (x1, x2) + distance) ||
		(y0 < min (y1, y2) - distance) || (y0 > max (y1, y2) + distance))
		return FALSE;

	/* close enough, do the full math */
	x2x1 = x2 - x1;
	y2y1 = y2 - y1;
	/* if the provided line is a point (simpler calculation and avoids a division by zero) */
	if ((x2x1 == 0.0) && (y2y1 == 0.0)) {
		/* check distance between two points */
		/* ref: http://mathworld.wolfram.com/Point-PointDistance2-Dimensional.html */
		float x1x0 = x1 - x0;
		float y1y0 = y1 - y0;
		return (sqrt ((x1x0 * x1x0) + (y1y0 * y1y0)) <= distance);
	} else {
		/* normal case: distance of a point to a line */
		/* ref: http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html */
		float d1 = fabs ((x2x1 * (y1 - y0)) - ((x1 - x0) * y2y1));
		float d2 = sqrt ((x2x1 * x2x1) + (y2y1 * y2y1));
		return (d1 / d2 <= distance);
	}
}

/* MonoTODO - GpGraphics is ignored */
GpStatus 
GdipIsOutlineVisiblePathPoint (GpPath *path, float x, float y, GpPen *pen, GpGraphics *graphics, bool *result)
{
	GpStatus status = Ok;
	GpPath *workpath;

	if (!path || !pen || !result)
		return InvalidParameter;

	*result = FALSE;

	if (path->count < 2) {
		/* FIXME - equality check ? */
		return Ok;
	}

	/* we clone the supplied path if it contains curves (we only deal with lines) */
	if (gdip_path_has_curve (path)) {
		status = GdipClonePath (path, &workpath);
		if (status != Ok)
			return status;

		status = GdipFlattenPath (workpath, NULL, 25.0f);
	} else {
		workpath = path;
	}

	if (graphics) {
		/* FIXME - graphics isn't always ignored, e.g. when we set the matrix, pageunit and pagescale */
	}

	if (status == Ok) {
		/* check if the supplied point is within half the pen's width of any path segment */
		float half_width = pen->width / 2;
		int start_index = 0;
		int i;
		byte type = 0;

		GpPointF p1 = g_array_index (workpath->points, GpPointF, 0);
		GpPointF p2;

		for (i = 1; i < path->count && !*result; i++) {
			/* check the line between the previous point and this point */
			p2 = g_array_index (workpath->points, GpPointF, i);
			*result = gdip_check_point_within_distance (x, y, &p1, &p2, half_width);

			/* check for closure (to match with the last starting point) */
			type = g_array_index (path->types, byte, i);
			if (!*result && (type & PathPointTypeCloseSubpath)) {
				p1 = g_array_index (workpath->points, GpPointF, start_index);
				/* compare last point with first (if the path is closed) */
				*result = gdip_check_point_within_distance (x, y, &p2, &p1, half_width);
			}

			/* switch point for the next line */
			p1 = p2;

			/* reset the start index */
			if (type == PathPointTypeStart)
				start_index = i;
		}
	}

	if (workpath != path)
		GdipDeletePath (workpath);
	return status;
}

/* MonoTODO - GpGraphics is ignored */
GpStatus 
GdipIsOutlineVisiblePathPointI (GpPath *path, int x, int y, GpPen *pen, GpGraphics *graphics, bool *result)
{
	return GdipIsOutlineVisiblePathPoint (path, x, y, pen, graphics, result);
}
