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

#include "gdip.h"
#include "customlinecap.h"

static GpStatus gdip_custom_linecap_setup (GpGraphics *graphics, GpCustomLineCap *cap);
static GpStatus gdip_custom_linecap_clone_cap (GpCustomLineCap *cap, GpCustomLineCap **clonedCap);
static GpStatus gdip_custom_linecap_destroy (GpCustomLineCap *cap);

/*
 * we have a single copy of vtable for
 * all instances of custom linecap.
 */

static CapClass vtable = { CustomLineCapTypeDefault,
			   gdip_custom_linecap_setup,
			   gdip_custom_linecap_clone_cap,
			   gdip_custom_linecap_destroy };

void
gdip_custom_linecap_init (GpCustomLineCap *cap, CapClass *vt)
{
	cap->vtable = vt;
	cap->base_cap = LineCapFlat;
	cap->start_cap = LineCapFlat;
	cap->end_cap = LineCapFlat;
	cap->stroke_join = LineJoinMiter;
	cap->base_inset = 0.0;
	/* LAMESPEC: Default value is documented as 1.0, but actually it is 0.0 */
	cap->width_scale = 0.0;
}

GpCustomLineCap *
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

	g_return_val_if_fail (cap != NULL, InvalidParameter);
	g_return_val_if_fail (clonedCap != NULL, InvalidParameter);

	newcap = (GpCustomLineCap *) GdipAlloc (sizeof (GpCustomLineCap));

	g_return_val_if_fail (newcap != NULL, OutOfMemory);

	*newcap = *cap;
	*clonedCap = newcap;

	return Ok;
}

GpStatus
gdip_custom_linecap_destroy (GpCustomLineCap *cap)
{
	g_return_val_if_fail (cap != NULL, InvalidParameter);

	GdipFree (cap);

	return Ok;
}

/* MonoTODO: Implement this function when cairo supports cairo_stroke_path API */
GpStatus
gdip_custom_linecap_setup (GpGraphics *graphics, GpCustomLineCap *customCap)
{
	cairo_pattern_t *pattern;
	cairo_t *ct;
	GpStatus status;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	ct = graphics->ct;

	g_return_val_if_fail (ct != NULL, InvalidParameter);

	status = NotImplemented;

	return status;
}

/* this setup function gets called from pen */

GpStatus
gdip_linecap_setup (GpGraphics *graphics, GpCustomLineCap *customCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	return customCap->vtable->setup (graphics, customCap);
}

/* CustomLineCap functions */
GpStatus
GdipCreateCustomLineCap (GpPath *fillPath, GpPath *strokePath, GpLineCap baseCap, float baseInset, GpCustomLineCap **customCap)
{
	GpCustomLineCap *cap;

	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail ((fillPath != NULL || strokePath != NULL), InvalidParameter);

	cap = gdip_custom_linecap_new ();
	g_return_val_if_fail (cap != NULL, OutOfMemory);

	cap->fill_path = fillPath;
	cap->stroke_path = strokePath;
	cap->base_cap = baseCap;
	cap->base_inset = baseInset;

	*customCap = cap;

	return Ok;
}

GpStatus
GdipDeleteCustomLineCap (GpCustomLineCap *customCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	return customCap->vtable->destroy (customCap);
}

GpStatus
GdipCloneCustomLineCap (GpCustomLineCap *customCap, GpCustomLineCap **clonedCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail (clonedCap != NULL, InvalidParameter);

	return customCap->vtable->clone_cap (customCap, clonedCap);
}

GpStatus
GdipSetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap startCap, GpLineCap endCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	customCap->start_cap = startCap;
	customCap->end_cap = endCap;

	return Ok;
}

GpStatus
GdipGetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap *startCap, GpLineCap *endCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail (startCap != NULL, InvalidParameter);
	g_return_val_if_fail (endCap != NULL, InvalidParameter);

	*(startCap) = customCap->start_cap;
	*(endCap) = customCap->end_cap;

	return Ok;
}

GpStatus
GdipSetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin lineJoin)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	customCap->stroke_join = lineJoin;

	return Ok;
}

GpStatus
GdipGetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin *lineJoin)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail (lineJoin != NULL, InvalidParameter);

	*(lineJoin) = customCap->stroke_join;

	return Ok;
}

GpStatus
GdipSetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap baseCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	customCap->base_cap = baseCap;

	return Ok;
}

GpStatus
GdipGetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap *baseCap)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail (baseCap != NULL, InvalidParameter);

	*(baseCap) = customCap->base_cap;

	return Ok;
}

GpStatus
GdipSetCustomLineCapBaseInset (GpCustomLineCap *customCap, float inset)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	customCap->base_inset = inset;

	return Ok;
}

GpStatus
GdipGetCustomLineCapBaseInset (GpCustomLineCap *customCap, float *inset)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail (inset != NULL, InvalidParameter);

	*(inset) = customCap->base_inset;

	return Ok;
}

GpStatus
GdipSetCustomLineCapWidthScale (GpCustomLineCap *customCap, float widthScale)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);

	customCap->width_scale = widthScale;

	return Ok;
}

GpStatus
GdipGetCustomLineCapWidthScale (GpCustomLineCap *customCap, float *widthScale)
{
	g_return_val_if_fail (customCap != NULL, InvalidParameter);
	g_return_val_if_fail (widthScale != NULL, InvalidParameter);

	*(widthScale) = customCap->width_scale;

	return Ok;
}
