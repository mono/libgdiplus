/*
 * adjustablearrowcap.c
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

void
gdip_adjust_arrowcap_init (GpAdjustableArrowCap *arrow)
{
	gdip_custom_linecap_init (&arrow->base, &vtable);
	arrow->fill_state = TRUE;
	arrow->middle_inset = 0.0;
}

GpAdjustableArrowCap *
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

	g_return_val_if_fail (cap != NULL, InvalidParameter);
	g_return_val_if_fail (clonedCap != NULL, InvalidParameter);

	newcap = (GpAdjustableArrowCap *) GdipAlloc (sizeof (GpAdjustableArrowCap));

	g_return_val_if_fail (newcap != NULL, OutOfMemory);

	memcpy (newcap, cap, sizeof (GpAdjustableArrowCap));
	*clonedCap = (GpCustomLineCap *) newcap;

	return Ok;
}

GpStatus
gdip_adjust_arrowcap_destroy (GpCustomLineCap *cap)
{
	g_return_val_if_fail (cap != NULL, InvalidParameter);

	GdipFree (cap);

	return Ok;
}

/* MonoTODO: Implement this when cairo supports cairo_stroke_path API */
GpStatus
gdip_adjust_arrowcap_setup (GpGraphics *graphics, GpCustomLineCap *customCap)
{
	GpAdjustableArrowCap *arrow;
	cairo_t *ct;
	GpStatus status;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	ct = graphics->ct;
	arrow = (GpAdjustableArrowCap *) customCap;

	g_return_val_if_fail (ct != NULL, InvalidParameter);

	status = NotImplemented;

	return status;
}

/* AdjustableArrowCap functions */
GpStatus
GdipCreateAdjustableArrowCap (float height, float width, bool isFilled, GpAdjustableArrowCap **arrowCap)
{
	GpAdjustableArrowCap *cap;

	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);

	cap = gdip_adjust_arrowcap_new ();
	g_return_val_if_fail (cap != NULL, OutOfMemory);

	cap->fill_state = isFilled;
	cap->width = width;
	cap->height = height;

	*arrowCap = cap;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, float height)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);

	arrowCap->height = height;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, float *height)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);
	g_return_val_if_fail (height != NULL, InvalidParameter);

	*(height) = arrowCap->height;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, float width)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);

	arrowCap->width = width;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, float *width)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);
	g_return_val_if_fail (width != NULL, InvalidParameter);

	*(width) = arrowCap->width;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, float middleInset)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);

	arrowCap->middle_inset = middleInset;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, float *middleInset)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);
	g_return_val_if_fail (middleInset != NULL, InvalidParameter);

	*(middleInset) = arrowCap->middle_inset;

	return Ok;
}

GpStatus
GdipSetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, bool isFilled)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);

	arrowCap->fill_state = isFilled;

	return Ok;
}

GpStatus
GdipGetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, bool *isFilled)
{
	g_return_val_if_fail (arrowCap != NULL, InvalidParameter);
	g_return_val_if_fail (isFilled != NULL, InvalidParameter);

	*(isFilled) = arrowCap->fill_state;

	return Ok;
}
