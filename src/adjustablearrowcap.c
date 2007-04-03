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
 * Copyright (C) 2003-2006 Novell, Inc. http://www.novell.com
 */

#include "gdip.h"
#include "customlinecap.h"

static GpStatus gdip_adjust_arrowcap_setup (GpGraphics *graphics, GpCustomLineCap *cap);
static GpStatus gdip_adjust_arrowcap_clone_cap (GpCustomLineCap *cap, GpCustomLineCap **clonedCap);
static GpStatus gdip_adjust_arrowcap_destroy (GpCustomLineCap *cap);

/*
 * we have a single copy of vtable for
 * all instances of adjustable arrowcap.
 */

static CapClass vtable = { CustomLineCapTypeAdjustableArrow,
			   gdip_adjust_arrowcap_setup,
			   gdip_adjust_arrowcap_clone_cap,
			   gdip_adjust_arrowcap_destroy };

static void
gdip_adjust_arrowcap_init (GpAdjustableArrowCap *arrow)
{
	gdip_custom_linecap_init (&arrow->base, &vtable);
	arrow->fill_state = TRUE;
	arrow->middle_inset = 0.0;
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

	newcap = (GpAdjustableArrowCap *) GdipAlloc (sizeof (GpAdjustableArrowCap));
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

/* AdjustableArrowCap functions */

// coverity[+alloc : arg-*3]
GpStatus
GdipCreateAdjustableArrowCap (float height, float width, bool isFilled, GpAdjustableArrowCap **arrowCap)
{
	GpAdjustableArrowCap *cap;

	if (!arrowCap)
		return InvalidParameter;

	cap = gdip_adjust_arrowcap_new ();
	if (!cap) {
		*arrowCap = NULL;
		return OutOfMemory;
	}

	cap->fill_state = isFilled;
	cap->width = width;
	cap->height = height;

	*arrowCap = cap;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, float height)
{
	if (!arrowCap)
		return InvalidParameter;

	arrowCap->height = height;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, float *height)
{
	if (!arrowCap || !height)
		return InvalidParameter;

	*(height) = arrowCap->height;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, float width)
{
	if (!arrowCap)
		return InvalidParameter;

	arrowCap->width = width;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, float *width)
{
	if (!arrowCap || !width)
		return InvalidParameter;

	*(width) = arrowCap->width;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, float middleInset)
{
	if (!arrowCap)
		return InvalidParameter;

	arrowCap->middle_inset = middleInset;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, float *middleInset)
{
	if (!arrowCap || !middleInset)
		return InvalidParameter;

	*(middleInset) = arrowCap->middle_inset;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, bool isFilled)
{
	if (!arrowCap)
		return InvalidParameter;

	arrowCap->fill_state = isFilled;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, bool *isFilled)
{
	if (!arrowCap || !isFilled)
		return InvalidParameter;

	*(isFilled) = arrowCap->fill_state;

	return Ok;
}
