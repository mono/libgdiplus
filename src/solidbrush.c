/*
 * solidbrush.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
 * Copyright (C) 2007 Novell, Inc. http://www.novell.com
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
 *   Alexandre Pigolkine(pigolkine@gmx.de)
 *   Ravindra (rkumar@novell.com)
 */

#include "solidbrush-private.h"
#include "general-private.h"
#include "graphics-private.h"

static GpStatus gdip_solidfill_setup (GpGraphics *graphics, GpBrush *brush);
static GpStatus gdip_solidfill_clone (GpBrush *brush, GpBrush **clonedBrush);
static GpStatus gdip_solidfill_destroy (GpBrush *brush);

/*
 * we have a single copy of vtable for
 * all instances of solidbrush.
 */

static BrushClass vtable = { BrushTypeSolidColor,
			     gdip_solidfill_setup, 
			     gdip_solidfill_clone,
			     gdip_solidfill_destroy };

static void 
gdip_solidfill_init (GpSolidFill *brush)
{
	gdip_brush_init (&brush->base, &vtable);
	brush->color = 0x00000000;
}

static GpSolidFill*
gdip_solidfill_new ()
{
	GpSolidFill *result = (GpSolidFill *) GdipAlloc (sizeof (GpSolidFill));
	
	if (result)
		gdip_solidfill_init (result);

	return result;
}

GpStatus
gdip_solidfill_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpSolidFill *solid;

	if (!graphics || !brush)
		return InvalidParameter;

	solid = (GpSolidFill *) brush;

	/* We extract the ARGB components of the color, only if the
	 * brush is changed.
	 */
	if (solid->base.changed) {
		BYTE a = (solid->color & 0xFF000000) >> 24;
		BYTE r = (solid->color & 0x00FF0000) >> 16;
		BYTE g = (solid->color & 0x0000FF00) >> 8;
		BYTE b = (solid->color & 0x000000FF);

		if (a == 0) {
			solid->A = 0;
			solid->R = 0;
			solid->G = 0;
			solid->B = 0;
		}
		else {
			solid->A = (double) a / 255.0;
			solid->R = (double) r / 255.0;
			solid->G = (double) g / 255.0;
			solid->B = (double) b / 255.0;
		}
	}

	cairo_set_source_rgba (graphics->ct, solid->R, solid->G, solid->B, solid->A);
	return Ok;
}

GpStatus
gdip_solidfill_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpSolidFill *result;
	GpSolidFill *solid;

	result = gdip_solidfill_new ();
	if (!result)
		return OutOfMemory;

	solid = (GpSolidFill *) brush;

	result->base = solid->base;
	result->color = solid->color;
	result->base.changed = TRUE;

	*clonedBrush = (GpBrush *) result;
	return Ok;
}

GpStatus
gdip_solidfill_destroy (GpBrush *brush)
{
	return Ok;
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateSolidFill (ARGB color, GpSolidFill **brush)
{
	GpSolidFill *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!brush)
		return InvalidParameter;

	result = gdip_solidfill_new ();
	if (!result)
		return OutOfMemory;

	result->color = color;
	
	*brush = result;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetSolidFillColor (GpSolidFill *brush, ARGB color)
{
	if (!brush)
		return InvalidParameter;

	brush->color = color;
	brush->base.changed = TRUE;

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetSolidFillColor (GpSolidFill *brush, ARGB *color)
{
	if (!brush || !color)
		return InvalidParameter;

	*color = brush->color;
	return Ok;
}
