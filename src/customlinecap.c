/*
 * customlinecap.c
 *
 * Copyright (C) Novell, Inc. 2003-2004.
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
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc. http://www.novell.com
 */

#include "customlinecap-private.h"
#include "general-private.h"
#include "graphics-path-private.h"
#include "graphics-private.h"
#include "graphics-cairo-private.h"

static GpStatus gdip_custom_linecap_setup (GpGraphics *graphics, GpCustomLineCap *cap);
static GpStatus gdip_custom_linecap_clone_cap (GpCustomLineCap *cap, GpCustomLineCap **clonedCap);
static GpStatus gdip_custom_linecap_destroy (GpCustomLineCap *cap);
static GpStatus gdip_custom_linecap_draw (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *cap, float x, float y, float otherend_x, float otherend_y);

/*
 * we have a single copy of vtable for
 * all instances of custom linecap.
 */

static CapClass vtable = { CustomLineCapTypeDefault,
			   gdip_custom_linecap_setup,
			   gdip_custom_linecap_clone_cap,
			   gdip_custom_linecap_destroy,
			   gdip_custom_linecap_draw };

void
gdip_custom_linecap_init (GpCustomLineCap *cap, CapClass *vt)
{
	cap->vtable = vt;
	cap->base_cap = LineCapTriangle;
	cap->start_cap = LineCapFlat;
	cap->end_cap = LineCapFlat;
	cap->stroke_join = LineJoinMiter;
	cap->base_inset = 0.0;
	cap->width_scale = 1.0;
	cap->fill_path = NULL;
	cap->stroke_path = NULL;
}

static GpCustomLineCap*
gdip_custom_linecap_new (void)
{
	GpCustomLineCap *result = (GpCustomLineCap *) GdipAlloc (sizeof (GpCustomLineCap));

	if (result)
		gdip_custom_linecap_init (result, &vtable);

	return result;
}

GpStatus
gdip_custom_linecap_clone_cap (GpCustomLineCap *cap, GpCustomLineCap **clonedCap)
{
	GpCustomLineCap *newcap;
	GpPath *fillpath = NULL, *strokepath = NULL;

	if (!cap || !clonedCap)
		return InvalidParameter;

	newcap = (GpCustomLineCap *) GdipAlloc (sizeof (GpCustomLineCap));
	if (!newcap)
		return OutOfMemory;

	newcap->vtable = cap->vtable;
	newcap->base_cap = cap->base_cap;
	newcap->start_cap = cap->start_cap;
	newcap->end_cap = cap->end_cap;
	newcap->stroke_join = cap->stroke_join;
	newcap->base_inset = cap->base_inset;
	newcap->width_scale = cap->width_scale;

	if (cap->fill_path) {
		if (GdipClonePath (cap->fill_path, &fillpath) != Ok) {
			if (fillpath != NULL)
				GdipFree (fillpath);
			GdipFree (newcap);
			return OutOfMemory;
		}
	}
	newcap->fill_path = fillpath;

	if (cap->stroke_path) {
		if (GdipClonePath (cap->stroke_path, &strokepath) != Ok) {
			if (strokepath != NULL)
				GdipFree (strokepath);
			GdipFree (fillpath);
			GdipFree (newcap);
			return OutOfMemory;
		}
	}
	newcap->stroke_path = strokepath;

	*clonedCap = newcap;

	return Ok;
}

GpStatus
gdip_custom_linecap_destroy (GpCustomLineCap *cap)
{
	if (!cap)
		return InvalidParameter;

	if (cap->fill_path) {
		GdipDeletePath (cap->fill_path);
		cap->fill_path = NULL;
	}
	if (cap->stroke_path) {
		GdipDeletePath (cap->stroke_path);
		cap->stroke_path = NULL;
	}
	GdipFree (cap);

	return Ok;
}

/* MonoTODO: Implement this function when cairo supports cairo_stroke_path API */
static GpStatus
gdip_custom_linecap_setup (GpGraphics *graphics, GpCustomLineCap *customCap)
{
	if (!graphics || !customCap)
		return InvalidParameter;

	return NotImplemented;
}

double
gdip_custom_linecap_angle (float x, float y, float otherend_x, float otherend_y)
{
	float slope;
	double angle;

	if (x == otherend_x) {
		slope = 0;
		if (y < otherend_y) {
			angle = PI;
		} else {
			angle = PI * 2;
		}
	} else if (y == otherend_y) {
		slope = 0;
		if (x < otherend_x) {
			angle = PI/2;
		} else {
			angle = PI/-2;
		}
	} else {
		if (y < otherend_y) {
			slope = (otherend_y - y) / (otherend_x - x);
			if (x < otherend_x) {
				angle = PI/2;
			} else {
				angle = PI/-2;
			}
		} else {
			slope = (otherend_x - x) / (y - otherend_y);
			angle = 0;
		}
	}

	angle += atan (slope);

	return angle;
}

GpStatus
gdip_custom_linecap_draw (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *customCap, float x, float y, float otherend_x, float otherend_y)
{
	double angle;
	int points;
	int i, idx = 0;
	float penwidth;

	if (!graphics || !pen || !customCap)
		return InvalidParameter;

	penwidth = pen->width;
	angle = gdip_custom_linecap_angle (x, y, otherend_x, otherend_y);

	cairo_save (graphics->ct);

	/* FIXME: handle base_inset (including set/get!) */
	cairo_translate (graphics->ct, x, y);
	cairo_rotate (graphics->ct, angle);

	if (customCap->stroke_path) {
		GpPath *path = customCap->stroke_path;
		points = path->count;

		for (i = 0; i < points; i++) {
			/* Adapted from gdip_plot_path() */
			GpPointF point = path->points[i];
			BYTE type = path->types[i];
			GpPointF pts [3];

			/* mask the bits so that we get only the type value not the other flags */
			switch (type & PathPointTypePathTypeMask) {
			case PathPointTypeStart:
				gdip_cairo_move_to (graphics, (double)point.X * penwidth, (double)point.Y * penwidth, TRUE, TRUE);
				break;

			case PathPointTypeLine:
				gdip_cairo_line_to (graphics, (double)point.X * penwidth, (double)point.Y * penwidth, TRUE, TRUE);
				break;

			case PathPointTypeBezier:
				/* make sure we only add at most 3 points to pts */
				if (idx < 3) {
					pts [idx] = point;
					idx ++;
				}

				/* once we've added 3 pts, we can draw the curve */
				if (idx == 3) {
					gdip_cairo_curve_to (graphics, (double)pts[0].X * penwidth, (double)pts[0].Y * penwidth, (double)pts[1].X * penwidth, (double)pts[1].Y * penwidth, (double)pts[2].X * penwidth, (double)pts[2].Y * penwidth, TRUE, TRUE);
					idx = 0;
				}
				break;

			default:
				g_warning ("Unknown PathPointType %d", type);
				return NotImplemented;
			}

			/* close the subpath */
			if (type & PathPointTypeCloseSubpath) {
				cairo_close_path (graphics->ct);
			}
		}

		gdip_pen_setup (graphics, pen);
		cairo_stroke (graphics->ct);
		gdip_cairo_set_matrix (graphics, graphics->copy_of_ctm);
	}

	/* FIXME: handle fill_path */

	cairo_restore (graphics->ct);

	return gdip_get_status (cairo_status (graphics->ct));
}

/* this setup function gets called from pen */

GpStatus
gdip_linecap_setup (GpGraphics *graphics, GpCustomLineCap *customCap)
{
	if (!graphics || !customCap)
		return InvalidParameter;

	return customCap->vtable->setup (graphics, customCap);
}

/* this draw function gets called from pen */

GpStatus
gdip_linecap_draw (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *customCap, float x, float y, float otherend_x, float otherend_y)
{
	if (!graphics || !pen || !customCap)
		return InvalidParameter;

	return customCap->vtable->draw (graphics, pen, customCap, x, y, otherend_x, otherend_y);
}

/* CustomLineCap functions */

// coverity[+alloc : arg-*4]
GpStatus WINGDIPAPI
GdipCreateCustomLineCap (GpPath *fillPath, GpPath *strokePath, GpLineCap baseCap, REAL baseInset, GpCustomLineCap **customCap)
{
	GpStatus status;
	GpCustomLineCap *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if ((!fillPath && !strokePath) || !customCap)
		return InvalidParameter;

	result = gdip_custom_linecap_new ();
	if (!result)
		return OutOfMemory;

	if (fillPath) {
		status = GdipClonePath (fillPath, &result->fill_path);
		if (status != Ok) {
			GdipDeleteCustomLineCap (result);
			return status;
		}
	}

	if (strokePath) {
		status = GdipClonePath (strokePath, &result->stroke_path);
		if (status != Ok) {
			GdipDeleteCustomLineCap (result);
			return status;
		}
	}

	if (baseCap >= LineCapFlat && baseCap <= LineCapTriangle) {
		result->base_cap = baseCap;
	} else {
		result->base_cap = LineCapFlat;
	}

	result->base_inset = baseInset;

	*customCap = result;
	return Ok;
}

GpStatus WINGDIPAPI
GdipDeleteCustomLineCap (GpCustomLineCap *customCap)
{
	if (!customCap)
		return InvalidParameter;

	return customCap->vtable->destroy (customCap);
}

GpStatus WINGDIPAPI
GdipCloneCustomLineCap (GpCustomLineCap *customCap, GpCustomLineCap **clonedCap)
{
	if (!customCap || !clonedCap)
		return InvalidParameter;

	return customCap->vtable->clone_cap (customCap, clonedCap);
}

GpStatus WINGDIPAPI
GdipGetCustomLineCapType (GpCustomLineCap *customCap, CustomLineCapType *capType)
{
	if (!customCap || !capType)
		return InvalidParameter;

	*capType = customCap->vtable->type;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap startCap, GpLineCap endCap)
{
	if (!customCap || startCap < LineCapFlat || startCap > LineCapTriangle || endCap < LineCapFlat || endCap > LineCapTriangle)
		return InvalidParameter;

	customCap->start_cap = startCap;
	customCap->end_cap = endCap;

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap *startCap, GpLineCap *endCap)
{
	if (!customCap || !startCap || !endCap)
		return InvalidParameter;

	*startCap = customCap->start_cap;
	*endCap = customCap->end_cap;

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin lineJoin)
{
	if (!customCap)
		return InvalidParameter;

	customCap->stroke_join = lineJoin;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin *lineJoin)
{
	if (!customCap || !lineJoin)
		return InvalidParameter;

	*lineJoin = customCap->stroke_join;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap baseCap)
{
	if (!customCap || baseCap > LineCapTriangle)
		return InvalidParameter;

	customCap->base_cap = baseCap;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap *baseCap)
{
	if (!customCap || !baseCap)
		return InvalidParameter;

	*baseCap = customCap->base_cap;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetCustomLineCapBaseInset (GpCustomLineCap *customCap, REAL inset)
{
	if (!customCap)
		return InvalidParameter;

	customCap->base_inset = inset;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetCustomLineCapBaseInset (GpCustomLineCap *customCap, REAL *inset)
{
	if (!customCap || !inset)
		return InvalidParameter;

	*inset = customCap->base_inset;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetCustomLineCapWidthScale (GpCustomLineCap *customCap, REAL widthScale)
{
	if (!customCap)
		return InvalidParameter;

	customCap->width_scale = widthScale;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetCustomLineCapWidthScale (GpCustomLineCap *customCap, REAL *widthScale)
{
	if (!customCap || !widthScale)
		return InvalidParameter;

	*widthScale = customCap->width_scale;
	return Ok;
}
