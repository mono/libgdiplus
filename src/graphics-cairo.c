/*
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
 * Copyright (C) 2006-2007 Novell, Inc (http://www.novell.com)
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
 *   Alexandre Pigolkine (pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "graphics-cairo-private.h"
#include "graphics-private.h"
#include "graphics-path-private.h"

/*
 * NOTE: all parameter's validations are done inside graphics.c
 */

/* helper functions to avoid a lot of repetitive code */

static GpStatus
fill_graphics_with_brush (GpGraphics *graphics, GpBrush *brush, BOOL stroke)
{
	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);

	/* don't stroke if scaled (since the pen thickness will be scaled too!) */
	if (stroke && !gdip_is_scaled (graphics)) {
		/* stroke only using 1 pixel width - see #413461 */
		double width = cairo_get_line_width (graphics->ct);
		cairo_set_line_width (graphics->ct, 1.0);
		cairo_stroke_preserve (graphics->ct);
		cairo_set_line_width (graphics->ct, width);
	}

	cairo_close_path (graphics->ct);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	gdip_cairo_set_matrix (graphics, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

static GpStatus
stroke_graphics_with_pen (GpGraphics *graphics, GpPen *pen)
{
	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	gdip_cairo_set_matrix (graphics, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

cairo_fill_rule_t
gdip_convert_fill_mode (FillMode fill_mode)
{
	if (fill_mode == FillModeAlternate) 
		return CAIRO_FILL_RULE_EVEN_ODD;
	else
		return CAIRO_FILL_RULE_WINDING;
}

static void 
make_ellipse (GpGraphics *graphics, float x, float y, float width, float height, BOOL convert_units, BOOL antialiasing)
{
	double rx, ry, cx, cy;

	/* if required deal, once and for all, with unit conversion */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}

	rx = width / 2;
	ry = height / 2;
	cx = x + rx;
	cy = y + ry;

	/* if required deal, once and for all, with antialiasing */
	if (antialiasing && !gdip_is_scaled (graphics)) {
		cx += graphics->aa_offset_x;
		cy += graphics->aa_offset_y;
	}

	gdip_cairo_move_to (graphics, cx + rx, cy, FALSE, FALSE);

	/* an approximate of the ellipse by drawing a curve in each
	 * quadrants */
	gdip_cairo_curve_to (graphics,
			cx + rx, cy - C1 * ry,
			cx + C1 * rx, cy - ry,
			cx, cy - ry,
			FALSE, FALSE);

	gdip_cairo_curve_to (graphics,
			cx - C1 * rx, cy - ry,
			cx - rx, cy - C1 * ry,
			cx - rx, cy,
			FALSE, FALSE);

	gdip_cairo_curve_to (graphics,
			cx - rx, cy + C1 * ry,
			cx - C1 * rx, cy + ry,
			cx, cy + ry,
			FALSE, FALSE);

	gdip_cairo_curve_to (graphics,
			cx + C1 * rx, cy + ry,
			cx + rx, cy + C1 * ry,
			cx + rx, cy,
			FALSE, FALSE);

	cairo_close_path (graphics->ct);
}

/*
 * Based on the algorithm described in
 *      http://www.stillhq.com/ctpfaq/2002/03/c1088.html#AEN1212
 */
static void
make_arc (GpGraphics *graphics, BOOL start, float x, float y, float width,
	  float height, float startAngle, float endAngle, BOOL antialiasing)
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
	bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta /2);

	sin_alpha = sin (alpha);
	sin_beta = sin (beta);
	cos_alpha = cos (alpha);
	cos_beta = cos (beta);

	/* don't move to starting point if we're continuing an existing curve */
	if (start) {
		/* starting point */
		double sx = cx + rx * cos_alpha;
		double sy = cy + ry * sin_alpha;
		gdip_cairo_move_to (graphics, sx, sy, FALSE, antialiasing);
	}

	gdip_cairo_curve_to (graphics,
					cx + rx * (cos_alpha - bcp * sin_alpha),
					cy + ry * (sin_alpha + bcp * cos_alpha),
					cx + rx * (cos_beta  + bcp * sin_beta),
					cy + ry * (sin_beta  - bcp * cos_beta),
					cx + rx * cos_beta, cy + ry * sin_beta,
		FALSE, antialiasing);
}

static void
make_arcs (GpGraphics *graphics, float x, float y, float width, float height, float startAngle, float sweepAngle, 
	BOOL convert_units, BOOL antialiasing)
{
	int i;
	float drawn = 0;
	float endAngle;
	BOOL enough = FALSE;

	/* if required deal, once and for all, with unit conversions */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}
	
	if (fabs (sweepAngle) >= 360) {
		/* FALSE -> units are already converted */
		make_ellipse (graphics, x, y, width, height, FALSE, antialiasing);
		return;
	}

	endAngle = startAngle + sweepAngle;
	/* if we end before the start then reverse positions (to keep increment positive) */
	if (endAngle < startAngle) {
		double temp = endAngle;
		endAngle = startAngle;
		startAngle = temp;
	}

	/* i is the number of sub-arcs drawn, each sub-arc can be at most 90 degrees.*/
	/* there can be no more then 4 subarcs, ie. 90 + 90 + 90 + (something less than 90) */
	for (i = 0; i < 4; i++) {
		float current = startAngle + drawn;
		float additional;

		if (enough)
			return;

		additional = endAngle - current; /* otherwise, add the remainder */
		if (additional > 90) {
			additional = 90.0;
		} else {
			/* a near zero value will introduce bad artefact in the drawing (#78999) */
			if (gdip_near_zero (additional))
				return;

			enough = TRUE;
		}

		make_arc (graphics, (i == 0),	/* only move to the starting pt in the 1st iteration */
			  x, y, width, height,	/* bounding rectangle */
			  current, current + additional, antialiasing);
		drawn += additional;		
	}
}

GpStatus
cairo_DrawArc (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, 
	float sweepAngle)
{
	/* We use graphics->copy_of_ctm matrix for path creation. We should
	 * have it set already.
	 */
	make_arcs (graphics, x, y, width, height, startAngle, sweepAngle, TRUE, TRUE);

	return stroke_graphics_with_pen (graphics, pen);
}

GpStatus 
cairo_DrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	int i, j, k;
		
	/* We use graphics->copy_of_ctm matrix for path creation. We should have it set already. */
	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, TRUE);

	for (i = 1, j = 2, k = 3; k < count; i += 3, j += 3, k += 3) {
	gdip_cairo_curve_to (graphics, points [i].X, points [i].Y, points [j].X, points [j].Y,
		points [k].X, points [k].Y, TRUE, TRUE);
	}

	return stroke_graphics_with_pen (graphics, pen);
}

static void
make_curve (GpGraphics *graphics, GDIPCONST GpPointF *points, GpPointF *tangents, int offset, int length,
	_CurveType type, BOOL antialiasing)
{
	int i;

	gdip_cairo_move_to (graphics, points [offset].X, points [offset].Y, FALSE, antialiasing);

	for (i = offset; i < offset + length; i++) {
		int j = i + 1;

		double x1 = points [i].X + tangents [i].X;
		double y1 = points [i].Y + tangents [i].Y;

		double x2 = points [j].X - tangents [j].X;
		double y2 = points [j].Y - tangents [j].Y;

		double x3 = points [j].X;
		double y3 = points [j].Y;

		gdip_cairo_curve_to (graphics, x1, y1, x2, y2, x3, y3, FALSE, antialiasing);
	}

	if (type == CURVE_CLOSE) {
		/* complete (close) the curve using the first point */
		double x1 = points [i].X + tangents [i].X;
		double y1 = points [i].Y + tangents [i].Y;

		double x2 = points [0].X - tangents [0].X;
		double y2 = points [0].Y - tangents [0].Y;

		double x3 = points [0].X;
		double y3 = points [0].Y;

		gdip_cairo_curve_to (graphics, x1, y1, x2, y2, x3, y3, FALSE, antialiasing);

		cairo_close_path (graphics->ct);
	}
}

GpStatus
cairo_DrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count, float tension)
{
	GpStatus status;
	GpPointF *tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

	make_curve (graphics, points, tangents, 0, count - 1, CURVE_CLOSE, TRUE);
	status = stroke_graphics_with_pen (graphics, pen);

	GdipFree (tangents);        
	return status;
}

GpStatus
cairo_FillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, float tension, GpFillMode fillMode)
{
	GpStatus status;
	GpPointF *tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

	make_curve (graphics, points, tangents, 0, count - 1, CURVE_CLOSE, FALSE);
	cairo_set_fill_rule (graphics->ct, gdip_convert_fill_mode (fillMode));
	status = fill_graphics_with_brush (graphics, brush, FALSE);

	GdipFree (tangents);
	return status;
}

GpStatus
cairo_DrawCurve3 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, int count, int offset, int numOfSegments,
	float tension)
{
	GpStatus status;
	GpPointF *tangents = gdip_open_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

	make_curve (graphics, points, tangents, offset, numOfSegments, CURVE_OPEN, TRUE);
	status = stroke_graphics_with_pen (graphics, pen);

	GdipFree (tangents);
	return status;
}

/*
 * Ellipses
 */

GpStatus 
cairo_DrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height)
{	
	/* We use graphics->copy_of_ctm matrix for path creation. We should have it set already. */
	make_ellipse (graphics, x, y, width, height, TRUE, TRUE);

	return stroke_graphics_with_pen (graphics, pen);
}

GpStatus
cairo_FillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height)
{
	/* We use graphics->copy_of_ctm matrix for path creation. We should have it set already. */
	make_ellipse (graphics, x, y, width, height, TRUE, FALSE);
	
	return fill_graphics_with_brush (graphics, brush, FALSE);
}

GpStatus 
cairo_DrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	int i;
	float last_x, last_y, prev_x, prev_y;
	GpStatus ret;

	/* We use graphics->copy_of_ctm matrix for path creation. We should have it set already. */
	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, TRUE);

	for (i = 1; i < count; i++) {
		gdip_cairo_line_to (graphics, points [i].X, points [i].Y, TRUE, TRUE);
		prev_x = points [i - 1].X;
		prev_y = points [i - 1].Y;
		last_x = points [i].X;
		last_y = points [i].Y;
	}

	ret = stroke_graphics_with_pen (graphics, pen);

	if (count > 1) {
		gdip_pen_draw_custom_start_cap (graphics, pen, points [0].X, points [0].Y, points [1].X, points [1].Y);
		gdip_pen_draw_custom_end_cap (graphics, pen, last_x, last_y, prev_x, prev_y);
	}

	return ret;
}

GpStatus
gdip_plot_path (GpGraphics *graphics, GpPath *path, BOOL antialiasing)
{
	int length = path->count;
	int i, idx = 0;

	for (i = 0; i < length; ++i) {
		GpPointF pt = path->points[i];
		BYTE type = path->types[i];
		GpPointF pts [3];

		/* mask the bits so that we get only the type value not the other flags */
		switch (type & PathPointTypePathTypeMask) {
			case PathPointTypeStart:
				gdip_cairo_move_to (graphics, pt.X, pt.Y, TRUE, antialiasing);
				break;

			case PathPointTypeLine:
				gdip_cairo_line_to (graphics, pt.X, pt.Y, TRUE, antialiasing);
				break;

			case PathPointTypeBezier:
				/* make sure we only add at most 3 points to pts */
				if (idx < 3) {
						pts [idx] = pt;
						idx ++;
				}

				/* once we've added 3 pts, we can draw the curve */
				if (idx == 3) {
					gdip_cairo_curve_to (graphics, pts[0].X, pts[0].Y, pts[1].X, pts[1].Y,
						pts[2].X, pts[2].Y, TRUE, antialiasing);

					idx = 0;
				}

				break;
			default:
				g_warning ("Unknown PathPointType %d", type);
				return NotImplemented;
			}

	/* close the subpath */
	if (type & PathPointTypeCloseSubpath)
		cairo_close_path (graphics->ct);
	}

	return Ok;
}

GpStatus
cairo_DrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
	GpStatus ret;
	int count;
	GpPointF *points;

	/* We use graphics->copy_of_ctm matrix for path creation. We should have it set already. */
	GpStatus status = gdip_plot_path (graphics, path, TRUE);
	if (status != Ok)
		return status;

	ret = stroke_graphics_with_pen (graphics, pen);

	/* Draw any custom pen end caps */

	status = GdipGetPointCount (path, &count);

	/* To know the angle of the end cap, we need the penultimate point.
	   Unfortunately there's no way of getting it without getting all
	   the points :-(
	 */
	if (status == Ok && count > 1) {
		points = gdip_calloc (count, sizeof(GpPointF));
		if (points == NULL) {
			return OutOfMemory;
		}
		status = GdipGetPathPoints (path, points, count);

		if (status == Ok) {
			gdip_pen_draw_custom_start_cap (graphics, pen, points [0].X, points [0].Y, points [1].X, points [1].Y);
			gdip_pen_draw_custom_end_cap (graphics, pen, points [count - 1].X, points [count - 1].Y, points [count - 2].X, points [count - 2].Y);
		}

		GdipFree (points);
	}

	return ret;
}

/* FIXME - this doesn't match MS behaviour when we use really complex paths with internal intersections */
GpStatus
cairo_FillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
	/* We use graphics->copy_of_ctm matrix for path creation. We should have it set already. */
	GpStatus status = gdip_plot_path (graphics, path, TRUE);
	if (status != Ok)
		return status;

	cairo_set_fill_rule (graphics->ct, gdip_convert_fill_mode (path->fill_mode));

	// filled paths includes the stroke
	return fill_graphics_with_brush (graphics, brush, TRUE);
}

static void
make_pie (GpGraphics *graphics, float x, float y, float width, float height,
	float startAngle, float sweepAngle, BOOL antialiasing)
{
	float rx, ry, cx, cy, alpha;
	double sin_alpha, cos_alpha;

	/* if required deal, once and for all, with unit conversions */
	if (!OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}

	rx = width / 2;
	ry = height / 2;

	/* center */
	cx = x + rx;
	cy = y + ry;

	/* angles in radians */        
	alpha = startAngle * PI / 180;
	
	/* adjust angle for ellipses */
	alpha = atan2 (rx * sin (alpha), ry * cos (alpha));

	sin_alpha = sin (alpha);
	cos_alpha = cos (alpha);

	/* if required deal, once and for all, with antialiasing */
	if (antialiasing && !gdip_is_scaled (graphics)) {
		cx += graphics->aa_offset_x;
		cy += graphics->aa_offset_y;
	}

	/* draw pie edge */
	if (fabs (sweepAngle) >= 360)
		gdip_cairo_move_to (graphics, cx + rx * cos_alpha, cy + ry * sin_alpha, FALSE, FALSE);
	else {
		gdip_cairo_move_to (graphics, cx, cy, FALSE, FALSE);
		gdip_cairo_line_to (graphics, cx + rx * cos_alpha, cy + ry * sin_alpha, FALSE, FALSE);
	}

	/* draw the arcs */
	make_arcs (graphics, x, y, width, height, startAngle, sweepAngle, FALSE, antialiasing);

	/* draws line back to center */
	if (fabs (sweepAngle) >= 360)
		gdip_cairo_move_to (graphics, cx, cy, FALSE, FALSE);
	else
		gdip_cairo_line_to (graphics, cx, cy, FALSE, FALSE);
}

GpStatus
cairo_DrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, 
	float startAngle, float sweepAngle)
{
	make_pie (graphics, x, y, width, height, startAngle, sweepAngle, TRUE);
	return stroke_graphics_with_pen (graphics, pen);
}

GpStatus
cairo_FillPie (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, 
	float startAngle, float sweepAngle)
{
	make_pie (graphics, x, y, width, height, startAngle, sweepAngle, FALSE);
	return fill_graphics_with_brush (graphics, brush, FALSE);
}

/*
 * Polygons
 */

static void
make_polygon (GpGraphics *graphics, GDIPCONST GpPointF *points, int count, BOOL antialiasing)
{
	int i;

	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, antialiasing);

	for (i = 0; i < count; i++) {
		gdip_cairo_line_to (graphics, points [i].X, points [i].Y, TRUE, antialiasing);
	}

	/*
	* Draw a line from the last point back to the first point if
	* they're not the same
	*/
	if (points [0].X != points [count-1].X && points [0].Y != points [count-1].Y) {
		gdip_cairo_line_to (graphics, points [0].X, points [0].Y, TRUE, antialiasing);
	}

	cairo_close_path (graphics->ct);
}

GpStatus
cairo_DrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	make_polygon (graphics, points, count, TRUE);
	return stroke_graphics_with_pen (graphics, pen);
}

GpStatus
cairo_FillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, FillMode fillMode)
{
	make_polygon (graphics, points, count, FALSE);
	cairo_set_fill_rule (graphics->ct, gdip_convert_fill_mode (fillMode));
	return fill_graphics_with_brush (graphics, brush, FALSE);
}

/*
 * Rectangle(s)
 */

GpStatus
cairo_DrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, int count)
{
	BOOL draw = FALSE;
	BOOL adjust = gdip_cairo_pen_width_needs_adjustment (pen);
	int i;

	for (i = 0; i < count; i++) {
		float x = rects [i].X;
		float y = rects [i].Y;
		float w = rects [i].Width;
		float h = rects [i].Height;

		/* don't draw/fill rectangles with negative width/height (bug #77129) */
		if ((w < 0) || (h < 0))
			continue;

		/* wide pen position can be different between cairo and gdi+ */
		if (adjust) {
			x -= 1.0f;
			y -= 1.0f;
		}

		gdip_cairo_rectangle (graphics, x, y, w, h, TRUE);
		draw = TRUE;
	}

	if (!draw)
		return Ok;

	return stroke_graphics_with_pen (graphics, pen);
}

GpStatus 
cairo_FillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, int count)
{
	BOOL draw = FALSE;
	int i;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++) {
		/* don't draw/fill rectangles with negative width/height (bug #77129) */
		if ((rects [i].Width < 0) || (rects [i].Height < 0))
			continue;

		gdip_cairo_rectangle (graphics, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height, FALSE);
		draw = TRUE;
	}

	/* shortcut if no rectangles were drawn into the graphics */
	if (!draw)
		return Ok;

	return fill_graphics_with_brush (graphics, brush, FALSE);
}

/*
 * Regions
 */

GpStatus
cairo_FillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region)
{
	/* if this is a region with a complex path */
	if (region->type == RegionTypePath) {
		GpStatus status;
		cairo_surface_t *mask_surface;

		/* (optimization) if if the path is empty, return immediately */
		if (!region->tree)
			return Ok;

		/* (optimization) if there is only one path, then we do not need the bitmap */
		if (region->tree->path) {
			/* if the path is empty, return OK */
			if (region->tree->path->count == 0)
				return Ok;

			/* else fill the single path */
			return cairo_FillPath (graphics, brush, region->tree->path);
		}

		gdip_region_bitmap_ensure (region);
		if (!region->bitmap)
			return OutOfMemory;

		mask_surface = gdip_region_bitmap_to_cairo_surface (region->bitmap);
		cairo_save (graphics->ct);
	
		/* We do brush setup just before filling. */
		gdip_brush_setup (graphics, brush);

		cairo_close_path (graphics->ct);
		cairo_mask_surface (graphics->ct, mask_surface, region->bitmap->X, region->bitmap->Y);
		cairo_fill (graphics->ct);

		status = gdip_get_status (cairo_status (graphics->ct));

		cairo_restore (graphics->ct);
		cairo_surface_destroy (mask_surface);

		return status;
	}

	/* if there's no rectangles, we can return directly */
	if (!region->rects || (region->cnt == 0))
		return Ok;

	return cairo_FillRectangles (graphics, brush, region->rects, region->cnt);
}

/*
 *
 */

GpStatus
cairo_GraphicsClear (GpGraphics *graphics, ARGB color)
{
	double blue = color & 0xff;
	double green = (color >> 8) & 0xff;
	double red = (color >> 16) & 0xff;
	double alpha = (color >> 24);

	/* Save the existing color/alpha/pattern settings */
	cairo_save (graphics->ct);

	cairo_set_source_rgba (graphics->ct, red / 255, green / 255, blue / 255, alpha / 255);
	cairo_set_operator (graphics->ct, CAIRO_OPERATOR_SOURCE);
	cairo_paint (graphics->ct);

	/* Restore the color/alpha/pattern settings */
	cairo_restore (graphics->ct);
	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
cairo_SetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode)
{
	switch (compositingMode) {
	case CompositingModeSourceOver:
		cairo_set_operator (graphics->ct, CAIRO_OPERATOR_OVER);
		break;
	case CompositingModeSourceCopy:
		cairo_set_operator (graphics->ct, CAIRO_OPERATOR_SOURCE);
		break;
	}
	return Ok;
}

GpStatus
cairo_SetSmoothingMode (GpGraphics *graphics, SmoothingMode mode)
{
	switch (mode) {
	case SmoothingModeAntiAlias:
	case SmoothingModeHighQuality:
		cairo_set_antialias (graphics->ct, CAIRO_ANTIALIAS_DEFAULT);
		graphics->aa_offset_x = 0.5;
		graphics->aa_offset_y = 0.5;
		break;

	case SmoothingModeNone:
	case SmoothingModeDefault:
	case SmoothingModeHighSpeed:
	default:
		cairo_set_antialias (graphics->ct, CAIRO_ANTIALIAS_NONE);
		graphics->aa_offset_x = CAIRO_AA_OFFSET_X;
		graphics->aa_offset_y = CAIRO_AA_OFFSET_Y;
		break;
	}
	return Ok;
}

/*
 * Clipping
 */

/*
	Since cairo does autoclipping and it hides the clipping rectangles from the API, the
	best thing for now is keep track of what the user wants and let Cairo do its autoclipping
*/

GpStatus
cairo_SetGraphicsClip (GpGraphics *graphics)
{
	GpRegion *work;
	GpRectF* rect;
	int i;

	cairo_reset_clip (graphics->ct);
 
	if (gdip_is_InfiniteRegion (graphics->clip))
		return Ok;

	if (gdip_is_matrix_empty (graphics->clip_matrix)) {
		work = graphics->clip;
	} else {
		GdipCloneRegion (graphics->clip, &work);
		GdipTransformRegion (work, graphics->clip_matrix);
	}

	switch (work->type) {
	case RegionTypeRect:
		for (i = 0, rect = work->rects; i < work->cnt; i++, rect++) {
			gdip_cairo_rectangle (graphics, rect->X, rect->Y, rect->Width, rect->Height, FALSE);
		}
		break;
	case RegionTypePath:
		if (work->tree && work->tree->path)
			gdip_plot_path (graphics, work->tree->path, FALSE);
		else {
			UINT count;
			GpMatrix matrix;
			cairo_matrix_init_identity (&matrix);
			/* I admit that's a (not so cute) hack - anyone with a better idea ? */
			if ((GdipGetRegionScansCount (work, &count, &matrix) == Ok) && (count > 0)) {
				GpRectF *rects = (GpRectF*) GdipAlloc (count * sizeof (GpRectF));
				if (rects) {
					INT countTemp;
					GdipGetRegionScans (work, rects, &countTemp, &matrix);
					for (i = 0, rect = rects; i < countTemp; i++, rect++) {
						gdip_cairo_rectangle (graphics, rect->X, rect->Y, rect->Width, rect->Height, FALSE);
					}
					GdipFree (rects);
				}
			}
		}
		break;
	default:
		g_warning ("Unknown region type %d", work->type);
		break;
	}
	
	cairo_clip (graphics->ct);

	/* destroy the clone, if one was needed */
	if (work != graphics->clip)
		GdipDeleteRegion (work);

	return Ok;
}

GpStatus
cairo_ResetClip (GpGraphics *graphics)
{
	cairo_reset_clip (graphics->ct);
	return gdip_get_status (cairo_status (graphics->ct));
}

/*
 * World Transforms
 */

GpStatus
cairo_ResetWorldTransform (GpGraphics *graphics)
{
	gdip_cairo_set_matrix (graphics, graphics->copy_of_ctm);
	cairo_reset_clip (graphics->ct);
	cairo_SetGraphicsClip (graphics);
	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
cairo_SetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	gdip_cairo_set_matrix (graphics, matrix);
	cairo_SetGraphicsClip (graphics);
	return gdip_get_status (cairo_status (graphics->ct));
}
