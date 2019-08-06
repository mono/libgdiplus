/*
 * adjustablearrowcap.c
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
 * Copyright (C) 2003-2007 Novell, Inc. http://www.novell.com
 */

#include "adjustablearrowcap-private.h"
#include "general-private.h"
#include "graphics-private.h"

static GpStatus gdip_adjust_arrowcap_setup (GpGraphics *graphics, GpCustomLineCap *cap);
static GpStatus gdip_adjust_arrowcap_clone_cap (GpCustomLineCap *cap, GpCustomLineCap **clonedCap);
static GpStatus gdip_adjust_arrowcap_destroy (GpCustomLineCap *cap);
static GpStatus gdip_adjust_arrowcap_draw (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *Cap, float x, float y, float otherend_x, float otherend_y);

/*
 * we have a single copy of vtable for
 * all instances of adjustable arrowcap.
 */

static CapClass vtable = { CustomLineCapTypeAdjustableArrow,
			   gdip_adjust_arrowcap_setup,
			   gdip_adjust_arrowcap_clone_cap,
			   gdip_adjust_arrowcap_destroy,
			   gdip_adjust_arrowcap_draw };

static void
gdip_adjust_arrowcap_init (GpAdjustableArrowCap *arrow)
{
	gdip_custom_linecap_init (&arrow->base, &vtable);
	arrow->fill_state = TRUE;
	arrow->middle_inset = 0.0;
	arrow->width = 0.0;
	arrow->height = 0.0;
}

static GpAdjustableArrowCap*
gdip_adjust_arrowcap_new (void)
{
	GpAdjustableArrowCap *result = (GpAdjustableArrowCap *) GdipAlloc (sizeof (GpAdjustableArrowCap));

	if (result)
		gdip_adjust_arrowcap_init (result);

	return result;
}

GpStatus
gdip_adjust_arrowcap_clone_cap (GpCustomLineCap *cap, GpCustomLineCap **clonedCap)
{
	GpAdjustableArrowCap *newcap;

	if (!cap || !clonedCap)
		return InvalidParameter;

	newcap = gdip_adjust_arrowcap_new ();
	if (!newcap)
		return OutOfMemory;

	memcpy (newcap, cap, sizeof (GpAdjustableArrowCap));
	*clonedCap = (GpCustomLineCap *) newcap;

	return Ok;
}

GpStatus
gdip_adjust_arrowcap_destroy (GpCustomLineCap *cap)
{
	if (!cap)
		return InvalidParameter;

	GdipFree (cap);
	return Ok;
}

/* MonoTODO: Implement this when cairo supports cairo_stroke_path API */
GpStatus
gdip_adjust_arrowcap_setup (GpGraphics *graphics, GpCustomLineCap *customCap)
{
	if (!graphics || !customCap)
		return InvalidParameter;

	return NotImplemented;
}

GpStatus
gdip_adjust_arrowcap_draw (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *customCap, float x, float y, float otherend_x, float otherend_y)
{
	double angle;
	GpAdjustableArrowCap *arrowcap;
	float w;
	float h;
	float penwidth;

	if (!graphics || !customCap)
		return InvalidParameter;

	penwidth = pen->width;
	if (penwidth < 2.0) {
		/* Seems to be a minimum */
		penwidth = 2.0;
	}

	arrowcap = (GpAdjustableArrowCap *)customCap;
	w = arrowcap->width / 2;
	h = arrowcap->height;

	angle = gdip_custom_linecap_angle (x, y, otherend_x, otherend_y);

	cairo_save (graphics->ct);

	/* FIXME: handle base_inset (including set/get!) */
	cairo_translate (graphics->ct, x, y);
	cairo_rotate (graphics->ct, angle);

	gdip_cairo_move_to (graphics, 0, 0, TRUE, TRUE);
	gdip_cairo_line_to (graphics, (double)-w * penwidth, (double)-h * penwidth, TRUE, TRUE);
	gdip_cairo_line_to (graphics, (double)w * penwidth, (double)-h * penwidth, TRUE, TRUE);
	gdip_cairo_line_to (graphics, 0, 0, TRUE, TRUE);

	if (arrowcap->fill_state) {
		/* FIXME: handle middle_inset */
		cairo_fill_preserve (graphics->ct);
	}

	cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

	return Ok;
}

static void
update_adjustablearrowcap (GpAdjustableArrowCap *cap)
{
	if (cap->width) {
		cap->base.base_inset = cap->height / cap->width;
	} else {
		cap->base.base_inset = 0;
	}

	cap->base.base_cap = LineCapTriangle;
}

/* AdjustableArrowCap functions */

// coverity[+alloc : arg-*3]
GpStatus WINGDIPAPI
GdipCreateAdjustableArrowCap (REAL height, REAL width, BOOL isFilled, GpAdjustableArrowCap **cap)
{
	GpAdjustableArrowCap *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!cap)
		return InvalidParameter;

	result = gdip_adjust_arrowcap_new ();
	if (!result) {
		*cap = NULL;
		return OutOfMemory;
	}

	result->fill_state = isFilled;
	result->width = width;
	result->height = height;
	update_adjustablearrowcap (result);

	*cap = result;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetAdjustableArrowCapHeight (GpAdjustableArrowCap *cap, REAL height)
{
	if (!cap)
		return InvalidParameter;

	if (height != cap->height) {
		cap->height = height;
		update_adjustablearrowcap (cap);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetAdjustableArrowCapHeight (GpAdjustableArrowCap *cap, REAL *height)
{
	if (!cap || !height)
		return InvalidParameter;

	*height = cap->height;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetAdjustableArrowCapWidth (GpAdjustableArrowCap *cap, REAL width)
{
	if (!cap)
		return InvalidParameter;

	if (width != cap->width) {
		cap->width = width;
		update_adjustablearrowcap (cap);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetAdjustableArrowCapWidth (GpAdjustableArrowCap *cap, REAL *width)
{
	if (!cap || !width)
		return InvalidParameter;

	*width = cap->width;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *cap, REAL middleInset)
{
	if (!cap)
		return InvalidParameter;

	if (middleInset != cap->middle_inset) {
		cap->middle_inset = middleInset;
		update_adjustablearrowcap (cap);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *cap, REAL *middleInset)
{
	if (!cap || !middleInset)
		return InvalidParameter;

	*middleInset = cap->middle_inset;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetAdjustableArrowCapFillState (GpAdjustableArrowCap *cap, BOOL isFilled)
{
	if (!cap)
		return InvalidParameter;

	if (isFilled != cap->fill_state) {
		cap->fill_state = isFilled;
		update_adjustablearrowcap (cap);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetAdjustableArrowCapFillState (GpAdjustableArrowCap *cap, BOOL *isFilled)
{
	if (!cap || !isFilled)
		return InvalidParameter;

	*isFilled = cap->fill_state;
	return Ok;
}
