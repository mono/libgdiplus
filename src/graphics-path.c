/*
 * graphics-path.c
 *
 * Copyright (C) 2003-2007, Novell Inc. (http://www.novell.com)
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
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */
 
#include "graphics-path-private.h"
#include "matrix-private.h"
#include "font-private.h"
#include "graphics-cairo-private.h"

#ifdef USE_PANGO_RENDERING
	#include "text-pango-private.h"
#endif

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

static BYTE *
g_byte_array_to_array (GByteArray *p)
{
        int length = p->len;
        BYTE *types = (BYTE*) GdipAlloc (sizeof (BYTE) * length);
	if (!types)
		return NULL;

        memcpy (types, p->data, p->len * sizeof (BYTE));
        
        return types;
}

static GByteArray *
array_to_g_byte_array (const BYTE *types, int count)
{
        GByteArray *p = g_byte_array_sized_new (count);
        g_byte_array_append (p, types, count);
        return p;
}

/* return TRUE if the specified path has (at least one) curves, FALSE otherwise */
BOOL
gdip_path_has_curve (GpPath *path)
{
	int i;

	if (!path)
		return FALSE;

	for (i = 0; i < path->count; i++) {
		if (g_array_index (path->types, BYTE, i) == PathPointTypeBezier)
			return TRUE;
	}

	return FALSE;
}

/*
 * Return the correct point type when adding a new shape to the path.
 */
static PathPointType
gdip_get_first_point_type (GpPath *path)
{
	PathPointType type;

	/* check for a new figure flag or an empty path */ 
	if (path->start_new_fig || (path->count == 0))
		return PathPointTypeStart;

	/* check if the previous point is a closure */
	type = g_array_index (path->types, BYTE, path->count - 1);
	if (type & PathPointTypeCloseSubpath)
		return PathPointTypeStart;
	else
		return PathPointTypeLine;
}

static void
append (GpPath *path, float x, float y, PathPointType type, BOOL compress)
{
        BYTE t = (BYTE) type;
	GpPointF pt;

	/* in some case we're allowed to compress identical points */
	if (compress && (path->count > 0)) {
		/* points (X, Y) must be identical */
		GpPointF lastPoint = g_array_index (path->points, GpPointF, path->count - 1);
		if ((lastPoint.X == x) && (lastPoint.Y == y)) {
			/* types need not be identical but must handle closed subpaths */
			PathPointType last_type = g_array_index (path->types, BYTE, path->count - 1);
			if ((last_type & PathPointTypeCloseSubpath) != PathPointTypeCloseSubpath)
				return;
		}
	}

	if (path->start_new_fig)
		t = PathPointTypeStart;
	/* if we closed a subpath, then start new figure and append */
	else if (path->count > 0) {
		type = g_array_index (path->types, BYTE, path->count - 1);
		if (type & PathPointTypeCloseSubpath)
			t = PathPointTypeStart;
        }

	pt.X = x;
	pt.Y = y;

	g_array_append_val (path->points, pt);
	g_byte_array_append (path->types, &t, 1);
	path->count++;

	path->start_new_fig = FALSE;
}

static void
append_point (GpPath *path, GpPointF pt, PathPointType type, BOOL compress)
{
        append (path, pt.X, pt.Y, type, compress);
}

static void
append_bezier (GpPath *path, float x1, float y1, float x2, float y2, float x3, float y3)
{
        append (path, x1, y1, PathPointTypeBezier3, FALSE);
        append (path, x2, y2, PathPointTypeBezier3, FALSE);
        append (path, x3, y3, PathPointTypeBezier3, FALSE);
}

static void
append_curve (GpPath *path, const GpPointF *points, GpPointF *tangents, int offset, int length, _CurveType type)
{
	int i;
	PathPointType ptype = ((type == CURVE_CLOSE) || (path->count == 0)) ? PathPointTypeStart : PathPointTypeLine;

	append_point (path, points [offset], ptype, TRUE);
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

/* coverity[+alloc : arg-*1] */
GpStatus
GdipCreatePath (FillMode fillMode, GpPath **path)
{
	if (!path)
		return InvalidParameter;

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

/* coverity[+alloc : arg-*4] */
GpStatus
GdipCreatePath2 (const GpPointF *points, const BYTE *types, int count, FillMode fillMode, GpPath **path)
{
	GArray *pts;
	GByteArray *t;

	if (!path || !points || !types || (count < 0))
		return InvalidParameter;

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

/* coverity[+alloc : arg-*4] */
GpStatus
GdipCreatePath2I (const GpPoint *points, const BYTE *types, int count, FillMode fillMode, GpPath **path)
{
	GpPointF *pt;
        GpStatus s;

	if (!points || !types || !path)
		return InvalidParameter;

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;
	
	s = GdipCreatePath2 (pt, types, count, fillMode, path);

        GdipFree (pt);

        return s;
}

/* coverity[+alloc : arg-*1] */
GpStatus
GdipClonePath (GpPath *path, GpPath **clonePath)
{
	int i;
	BYTE type;
	GpPointF point;

	if (!path || !clonePath)
		return InvalidParameter;

        *clonePath = (GpPath *) GdipAlloc (sizeof (GpPath));
	if (!*clonePath)
		return OutOfMemory;

        (*clonePath)->fill_mode = path->fill_mode;
        (*clonePath)->count = path->count;
        (*clonePath)->points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
        (*clonePath)->types = g_byte_array_new ();
	for (i = 0; i < path->count; i++) {
		point = g_array_index (path->points, GpPointF, i);
		type = g_array_index (path->types, BYTE, i);
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
GdipGetPointCount (GpPath *path, int *count)
{
	if (!path || !count)
		return InvalidParameter;

	*count = path->count;
	return Ok;
}

GpStatus
GdipGetPathTypes (GpPath *path, BYTE *types, int count)
{
	int i;

	if (!path || !types || (count < 1))
		return InvalidParameter;

	if (count > path->count)
		count = path->count;

        for (i = 0; i < count; i++)
                types [i] = path->types->data [i];
        
        return Ok;
}

GpStatus
GdipGetPathPoints (GpPath *path, GpPointF *points, int count)
{
	int i;

	if (!path || !points || (count < 1))
		return InvalidParameter;

	if (count > path->count)
		count = path->count;

        for (i = 0; i < count; i++) {
		GpPointF point = g_array_index (path->points, GpPointF, i);
                points [i].X = point.X;
                points [i].Y = point.Y;
	}

        return Ok;
}

GpStatus
GdipGetPathPointsI (GpPath *path, GpPoint *points, int count)
{
	int i;

	if (!path || !points || (count < 1))
		return InvalidParameter;

        for (i = 0; i < count; i++) {
                GpPoint point = g_array_index (path->points, GpPoint, i);
                points [i].X = (int) point.X;
                points [i].Y = (int) point.Y; 
        }

        return Ok;
}

GpStatus
GdipGetPathFillMode (GpPath *path, FillMode *fillMode)
{
	if (!path || !fillMode)
		return InvalidParameter;

        *fillMode = path->fill_mode;
        
        return Ok;
}

GpStatus
GdipSetPathFillMode (GpPath *path, FillMode fillMode)
{
	if (!path)
		return InvalidParameter;

        path->fill_mode = fillMode;
        
        return Ok;
}

GpStatus
GdipGetPathData (GpPath *path, GpPathData *pathData)
{
	if (!path || !pathData)
		return InvalidParameter;

        pathData->Points = g_array_to_array (path->points);
	if (!pathData->Points)
		return OutOfMemory;

        pathData->Types = g_byte_array_to_array (path->types);
	if (!pathData->Types) {
		GdipFree (pathData->Points);
		pathData->Points = NULL;
		return OutOfMemory;
	}

	/* don't return the count unless we have the data */
        pathData->Count = path->count;
        
        return Ok;
}

GpStatus
GdipStartPathFigure (GpPath *path)
{
	if (!path)
		return InvalidParameter;

	path->start_new_fig = TRUE;

	return Ok;
}

GpStatus
GdipClosePathFigure (GpPath *path)
{
	if (!path)
		return InvalidParameter;

	if (path->count > 0) {
		BYTE *last = &g_array_index (path->types, BYTE, path->count - 1);
		*last |= PathPointTypeCloseSubpath;
	}
	path->start_new_fig = TRUE;

	return Ok;
}

GpStatus
GdipClosePathFigures (GpPath *path)
{
	int index = 0;
	BYTE currentType;
	BYTE lastType;
	GByteArray *oldTypes;

	if (!path)
		return InvalidParameter;

	/* first point is not closed */
	if (path->count <= 1)
		return Ok;

	oldTypes = path->types;
	path->types = g_byte_array_new ();

	lastType = g_array_index (oldTypes, BYTE, index);
	index++;

	for (index = 1; index < path->count; index++) {
		currentType = g_array_index (oldTypes, BYTE, index);
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
	BYTE current;

	if (!path)
		return InvalidParameter;

	if (path->count == 0)
		return Ok;

	current = g_array_index (path->types, BYTE, path->count - 1);

        g_byte_array_remove_index (path->types, path->count - 1);

        current |= PathPointTypePathMarker;

        g_byte_array_append (path->types, &current, 1);

        return Ok;
}

GpStatus
GdipClearPathMarkers (GpPath *path)
{
        int i;
	BYTE current;
	GByteArray *cleared;

	if (!path)
		return InvalidParameter;

	/* shortcut to avoid allocations */
	if (path->count == 0)
		return Ok;

	cleared = g_byte_array_new ();

        for (i = 0; i < path->count; i++) {
                current = g_array_index (path->types, BYTE, i);

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

/*
 * Append old_types[start, end] to new_types, adjusting flags.
 */
static void
reverse_subpath_adjust_flags (int start, int end, GByteArray *old_types, GByteArray *new_types, BOOL *prev_had_marker)
{
	BYTE t, prev_first, prev_last;
	int i;
	
	/* Copy all but PathPointTypeStart */
	if (end != start)
		g_byte_array_append (new_types, old_types->data + start + 1, end - start);
	
	/* Append PathPointTypeStart */
	t = PathPointTypeStart;
	g_byte_array_append (new_types, &t, 1);
	
	g_assert (new_types->len == end + 1);
	
	prev_first = g_array_index (old_types, BYTE, start);
	prev_last = g_array_index (old_types, BYTE, end);
	
	/* Remove potential flags from our future start point */
	if (end != start)
		new_types->data[end - 1] &= PathPointTypePathTypeMask;
	/* Set the flags on our to-be-last point */
	if (prev_last & PathPointTypeDashMode)
		new_types->data[start] |= PathPointTypeDashMode;
	if (prev_last & PathPointTypeCloseSubpath)
		new_types->data[start] |= PathPointTypeCloseSubpath;
	
	/*
	 * Swap markers
	 */
	for (i = start + 1; i < end; i++) {
		if (g_array_index (old_types, BYTE, i - 1) & PathPointTypePathMarker)
			new_types->data[i] |= PathPointTypePathMarker;
		else
			new_types->data[i] &= ~PathPointTypePathMarker;
	}
	
	/* If the last point of the previous subpath had a marker, we inherit it */
	if (*prev_had_marker)
		new_types->data[start] |= PathPointTypePathMarker;
	else
		new_types->data[start] &= ~PathPointTypePathMarker;
	
	*prev_had_marker = ((prev_last & PathPointTypePathMarker) == PathPointTypePathMarker);
}

GpStatus
GdipReversePath (GpPath *path)
{
	int length, i;
	GArray *points;
	GByteArray *types;
	int start = 0;
	BOOL prev_had_marker = FALSE;

	if (!path)
		return InvalidParameter;

	length = path->count;
	/* shortcut */
	if (length <= 1)
		return Ok;

	/* PathTypes reversal */

	/* First adjust the flags for each subpath */
	types = g_byte_array_sized_new (length);
	if (!types)
		return OutOfMemory;

	for (i = 1; i < length; i++) {
		BYTE t = g_array_index (path->types, BYTE, i);
		if ((t & PathPointTypePathTypeMask) == PathPointTypeStart) {
			reverse_subpath_adjust_flags (start, i - 1, path->types, types, &prev_had_marker);
			start = i;
		}
	}
	if (start < length - 1)
		reverse_subpath_adjust_flags (start, length - 1, path->types, types, &prev_had_marker);

	/* Then reverse the resulting array */
	for (i = 0; i < (length >> 1); i++) {
		BYTE *a = &g_array_index (types, BYTE, i);
		BYTE *b = &g_array_index (types, BYTE, length - i - 1);
		BYTE temp = *a;
		*a = *b;
		*b = temp;
	}
	g_byte_array_free (path->types, TRUE);
	path->types = types;

	/* PathPoints reversal
	 * note: if length is odd then the middle point doesn't need to switch side
	 */
        for (i = 0; i < (length >> 1); i++) {
		GpPointF *first = &g_array_index (path->points, GpPointF, i);
		GpPointF *last = &g_array_index (path->points, GpPointF, length - i - 1);

		GpPointF temp;
		temp.X = first->X;
		temp.Y = first->Y;
		first->X = last->X;
		first->Y = last->Y;
		last->X = temp.X;
		last->Y = temp.Y;
        }
        
        return Ok;
}

GpStatus
GdipGetPathLastPoint (GpPath *path, GpPointF *lastPoint)
{
	if (!path || !lastPoint || (path->count <= 0))
		return InvalidParameter;

	*lastPoint = g_array_index (path->points, GpPointF, path->count - 1);
	return Ok;
}

GpStatus
GdipAddPathLine (GpPath *path, float x1, float y1, float x2, float y2)
{
	if (!path)
		return InvalidParameter;

	/* only the first point can be compressed (i.e. removed if identical to previous) */
	append (path, x1, y1, PathPointTypeLine, TRUE);
	append (path, x2, y2, PathPointTypeLine, FALSE);

	return Ok;
}

GpStatus
GdipAddPathLine2 (GpPath *path, const GpPointF *points, int count)
{
	int i;
	GpPointF *tmp;

	if (!path || !points || (count < 0))
		return InvalidParameter;

	/* only the first point can be compressed (i.e. removed if identical to previous) */
	for (i = 0, tmp = (GpPointF*) points; i < count; i++, tmp++)
		append (path, tmp->X, tmp->Y, PathPointTypeLine, (i == 0));
        
        return Ok;
}

static void
append_arc (GpPath *path, BOOL start, float x, float y, float width, float height, float startAngle, float endAngle)
{
	float delta, bcp;
	double sin_alpha, sin_beta, cos_alpha, cos_beta;

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

	delta = beta - alpha;
	// http://www.stillhq.com/ctpfaq/2001/comp.text.pdf-faq-2001-04.txt (section 2.13)
	bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta / 2);

	sin_alpha = sin (alpha);
	sin_beta = sin (beta);
	cos_alpha = cos (alpha);
	cos_beta = cos (beta);

	/* move to the starting point if we're not continuing a curve */
	if (start) {
		/* starting point */
		double sx = cx + rx * cos_alpha;
		double sy = cy + ry * sin_alpha;
		append (path, sx, sy, PathPointTypeLine, FALSE);
	}

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
	int increment;
	float endAngle;
	BOOL enough = FALSE;

	if (fabs (sweepAngle) >= 360) {
		GdipAddPathEllipse (path, x, y, width, height);
		return;
	}

	endAngle = startAngle + sweepAngle;
	increment = (endAngle < startAngle) ? -90 : 90;

	/* i is the number of sub-arcs drawn, each sub-arc can be at most 90 degrees.*/
	/* there can be no more then 4 subarcs, ie. 90 + 90 + 90 + (something less than 90) */
	for (i = 0; i < 4; i++) {
		float current = startAngle + drawn;
		float additional;

		if (enough)
			return;
		
		additional = endAngle - current; /* otherwise, add the remainder */
		if (fabs (additional) > 90) {
			additional = increment;
		} else {
			/* a near zero value will introduce bad artefact in the drawing (#78999) */
			if (gdip_near_zero (additional))
				return;

			enough = TRUE;
		}

		append_arc (path, (i == 0),           /* only move to the starting pt in the 1st iteration */
			    x, y, width, height,      /* bounding rectangle */
			    current, current + additional);
		drawn += additional;
	}
}

GpStatus
GdipAddPathArc (GpPath *path, float x, float y, 
                float width, float height, float startAngle, float sweepAngle)
{
	if (!path)
		return InvalidParameter;

	/* draw the arcs */
	append_arcs (path, x, y, width, height, startAngle, sweepAngle);

        return Ok;
}

GpStatus
GdipAddPathBezier (GpPath *path, 
                   float x1, float y1, float x2, float y2, 
                   float x3, float y3, float x4, float y4)
{
	if (!path)
		return InvalidParameter;

        append (path, x1, y1, PathPointTypeLine, TRUE);
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
        
        append_point (path, *tmp, PathPointTypeLine, TRUE);
        tmp++;

        for (i = 1; i < count; i++, tmp++)
                append_point (path, *tmp, PathPointTypeBezier3, FALSE);

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

	tangents = gdip_open_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
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

	if (!path || !points || (numberOfSegments < 1))
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

	if (!path || !points || (count < 3))
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
	if (!path)
		return InvalidParameter;

	if ((width == 0.0) || (height == 0.0))
		return Ok;

        append (path, x, y, PathPointTypeStart, FALSE);
        append (path, x + width, y, PathPointTypeLine, FALSE);
        append (path, x + width, y + height, PathPointTypeLine, FALSE);
        append (path, x, y + height, PathPointTypeLine | PathPointTypeCloseSubpath, FALSE);
        
        return Ok;
}

GpStatus
GdipAddPathRectangles (GpPath *path, const GpRectF *rects, int count)
{
        int i;

	if (!path || !rects)
		return InvalidParameter;

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
        double rx = width / 2;
        double ry = height / 2;
        double cx = x + rx;
        double cy = y + ry;

	if (!path)
		return InvalidParameter;

        /* origin */
        append (path, cx + rx, cy, PathPointTypeStart, FALSE);

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
	float sin_alpha, cos_alpha;

        float rx = width / 2;
        float ry = height / 2;

        /* center */
        int cx = x + rx;
        int cy = y + ry;	

        /* angles in radians */        
        float alpha = startAngle * PI / 180;

        /* adjust angle for ellipses */
        alpha = atan2 (rx * sin (alpha), ry * cos (alpha));

	sin_alpha = sin (alpha);
	cos_alpha = cos (alpha);

	if (!path)
		return InvalidParameter;

        /* move to center */
        append (path, cx, cy, PathPointTypeStart, FALSE);

        /* draw pie edge */
	if (fabs (sweepAngle) < 360)
		append (path, cx + rx * cos_alpha, cy + ry * sin_alpha, PathPointTypeLine, FALSE);

	/* draw the arcs */
	append_arcs (path, x, y, width, height, startAngle, sweepAngle);
        
        /* draw pie edge */
	if (fabs (sweepAngle) < 360)
		append (path, cx, cy, PathPointTypeLine, FALSE);

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

	/* note: polygon points are never compressed (i.e. removed if identical) */

	append_point (path, *tmp, PathPointTypeStart, FALSE);
	tmp ++;

	for (i = 1; i < count; i++, tmp++)
		append_point (path, *tmp, PathPointTypeLine, FALSE);

        /*
         * Add a line from the last point back to the first point if
         * they're not the same
         */
	if (points [0].X != points [count - 1].X && points [0].Y != points [count - 1].Y)
		append_point (path, points [0], PathPointTypeLine, FALSE);
        
	/* close the path */
        return GdipClosePathFigure (path);
}

GpStatus
GdipAddPathPath (GpPath *path, GDIPCONST GpPath *addingPath, BOOL connect)
{
        int i, length;
	PathPointType first;
        GpPointF *pts;
        BYTE *types;

	if (!path || !addingPath)
		return InvalidParameter;

        length = addingPath->count;
        if (length < 1)
                return Ok;
        
        pts = gdip_calloc (sizeof (GpPointF), length);
	if (!pts)
		return OutOfMemory;
        types = gdip_calloc (sizeof (BYTE), length);
	if (!types) {
		GdipFree (pts);
		return OutOfMemory;
	}

        GdipGetPathPoints ((GpPath*)addingPath, pts, length);
        GdipGetPathTypes ((GpPath*)addingPath, types, length);

	/* We can connect only open figures. If first figure is closed
	 * it can't be connected.
	 */
	first = connect ? gdip_get_first_point_type (path) : PathPointTypeStart;

	append_point (path, pts [0], first, FALSE); 

	for (i = 1; i < length; i++)
		append_point (path, pts [i], types [i], FALSE);

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
	BYTE *utf8 = NULL;

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

	utf8 = (BYTE*) ucs2_to_utf8 ((const gunichar2 *)string, -1);
	if (!utf8) {
		cairo_destroy (cr);
		cairo_surface_destroy (cs);
		return OutOfMemory;
	}

	status = GdipCreateFont (family, emSize, style, UnitPixel, &font);
	if (status != Ok) {
		if (font)
			GdipDeleteFont (font);
		GdipFree (utf8);
		cairo_destroy (cr);
		cairo_surface_destroy (cs);
		return status;
	}

	if (layoutRect)
		cairo_move_to (cr, layoutRect->X, layoutRect->Y + font->sizeInPixels);

#ifdef USE_PANGO_RENDERING
	{
	GpRectF box;
	PangoLayout* layout; 

	cairo_save (cr);
	layout = gdip_pango_setup_layout (cr, string, length, font, layoutRect, &box, format);
	pango_cairo_layout_path (cr, layout);
	g_object_unref (layout);
	cairo_restore (cr);
	}
#else
	cairo_set_font_face (cr, gdip_get_cairo_font_face (font));
	cairo_set_font_size (cr, font->sizeInPixels);
	/* TODO - deal with layoutRect, format... ideally we would be calling a subset
	   of GdipDrawString that already does everything *and* preserve the whole path */
	cairo_text_path (cr, (const char*)utf8);
#endif

	/* get the font data from the cairo path and translate it as a gdi+ path */
	cp = cairo_copy_path (cr);
	if (cp) {
		int i;
		for (i=0; i < cp->num_data; i += cp->data[i].header.length) {
			PathPointType type = PathPointTypeStart;
			cairo_path_data_t *data = &cp->data[i];

			if ((i < cp->num_data - 1) && (data->header.type == CAIRO_PATH_CLOSE_PATH))
				type |= PathPointTypeCloseSubpath;

			switch (data->header.type) {
			case CAIRO_PATH_MOVE_TO:
				append (path, data[1].point.x, data[1].point.y, type, FALSE);
				break;
			case CAIRO_PATH_LINE_TO:
				append (path, data[1].point.x, data[1].point.y, type | PathPointTypeLine, FALSE);
				break;
			case CAIRO_PATH_CURVE_TO:
				append (path, data[1].point.x, data[1].point.y, PathPointTypeBezier, FALSE);
				append (path, data[2].point.x, data[2].point.y, PathPointTypeBezier, FALSE);
				append (path, data[3].point.x, data[3].point.y, type | PathPointTypeBezier, FALSE);
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

	/* only the first point can be compressed (i.e. removed if identical to previous) */
	for (i = 0, tmp = (GpPoint*) points; i < count; i++, tmp++)
		append (path, tmp->X, tmp->Y, PathPointTypeLine, (i == 0));

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
        append (path, tmp->X, tmp->Y, PathPointTypeLine, TRUE);
        tmp++;

        for (i = 1; i < count; i++, tmp++)
                append (path, tmp->X, tmp->Y, PathPointTypeBezier3, FALSE);

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
	GpStatus s;

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
	GpStatus s;

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
	GpStatus s;

	if (!path || !points)
		return InvalidParameter;

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

	if (!path || !rects)
		return InvalidParameter;

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

	/* note: polygon points are never compressed (i.e. removed if identical) */

	tmp = (GpPoint *) points;
	append (path, tmp->X, tmp->Y, PathPointTypeStart, FALSE);
	tmp++;

	for (i = 1; i < count; i++, tmp++)
		append (path, tmp->X, tmp->Y, PathPointTypeLine, FALSE);

        /*
         * Add a line from the last point back to the first point if
         * they're not the same
         */
	if (points [0].X != points [count - 1].X && points [0].Y != points [count - 1].Y)
		append (path, points [0].X, points [0].Y, PathPointTypeLine, FALSE);
        
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
	BYTE type;
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

	if (!path)
		return InvalidParameter;

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
		BYTE type = g_array_index (path->types, BYTE, i);

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
        GpStatus s;

	if (!path)
		return InvalidParameter;

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
	GpPath *workpath = NULL;

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
	if (status != Ok) {
		if (workpath)
			GdipDeletePath (workpath);
		return status;
	}

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

GpStatus 
GdipIsVisiblePathPoint (GpPath *path, float x, float y, GpGraphics *graphics, BOOL *result)
{
	GpStatus status = Ok;
	cairo_surface_t* s = NULL;
	GpGraphics *g;
	GpUnit page_unit = UnitPixel;

	if (!path || !result)
		return InvalidParameter;

	if (graphics) {
		g = graphics;
		cairo_save (g->ct);
		page_unit = g->page_unit;
	} else {
		/* create a temporary context */
		s = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
		g = gdip_graphics_new (s);
	}

	cairo_new_path (g->ct);
	/* unit tests shows that PageUnit isn't consireded (well x, y are probably considered to be the same unit format ) */
	g->page_unit = UnitPixel;
	status = gdip_plot_path (g, path, FALSE);
	if (status == Ok) {
		cairo_set_fill_rule (g->ct, gdip_convert_fill_mode (path->fill_mode));
		cairo_set_antialias (g->ct, CAIRO_ANTIALIAS_NONE);
		*result = cairo_in_fill (g->ct, x + 1.0 /* CAIRO_AA_OFFSET_X */, y + CAIRO_AA_OFFSET_Y);
	} else {
		*result = FALSE;
	}

	if (graphics) {
		/* restore GpGraphics to original state */
		cairo_restore (graphics->ct);
		g->page_unit = page_unit;
	} else {
		/* delete temporary context */
		cairo_surface_destroy (s);
		GdipDeleteGraphics (g);
	}

	return status;
}

GpStatus 
GdipIsVisiblePathPointI (GpPath *path, int x, int y, GpGraphics *graphics, BOOL *result)
{
	return GdipIsVisiblePathPoint (path, x, y, graphics, result);
}

GpStatus 
GdipIsOutlineVisiblePathPoint (GpPath *path, float x, float y, GpPen *pen, GpGraphics *graphics, BOOL *result)
{
	GpStatus status = Ok;
	cairo_surface_t* s = NULL;
	GpGraphics *g;
	GpUnit page_unit = UnitPixel;

	if (!path || !pen || !result)
		return InvalidParameter;

	if (graphics) {
		g = graphics;
		cairo_save (graphics->ct);
		page_unit = g->page_unit;
	} else {
		/* create a temporary context */
		s = cairo_image_surface_create (CAIRO_FORMAT_A1, 1, 1);
		g = gdip_graphics_new (s);
	}

	cairo_new_path (g->ct);
	/* unit tests shows that PageUnit isn't consireded (well x, y are probably considered to be the same unit format ) */
	g->page_unit = UnitPixel;
	status = gdip_plot_path (g, path, FALSE);
	if (status == Ok) {
		/* we must fight around cairo AA */
		cairo_set_antialias (g->ct, CAIRO_ANTIALIAS_NONE);
		cairo_set_line_width (g->ct, pen->width - CAIRO_AA_OFFSET_Y);
		*result = cairo_in_stroke (g->ct, x, y);
	} else {
		*result = FALSE;
	}

	if (graphics) {
		/* restore GpGraphics to original state */
		cairo_restore (graphics->ct);
		g->page_unit = page_unit;
	} else {
		/* delete temporary context */
		cairo_surface_destroy (s);
		GdipDeleteGraphics (g);
	}

	return status;
}

GpStatus 
GdipIsOutlineVisiblePathPointI (GpPath *path, int x, int y, GpPen *pen, GpGraphics *graphics, BOOL *result)
{
	return GdipIsOutlineVisiblePathPoint (path, x, y, pen, graphics, result);
}

