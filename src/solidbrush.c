/*
 * solidbrush.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
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
 */

#include "gdip.h"
#include "solidbrush.h"

/*
 * we have a single copy of vtable for
 * all instances of solidbrush.
 */

static BrushClass vtable = { BrushTypeSolidColor,
			     gdip_solidfill_setup, 
			     gdip_solidfill_clone,
			     gdip_solidfill_destroy };

void 
gdip_solidfill_init (GpSolidFill *brush)
{
	gdip_brush_init (&brush->base, &vtable);
	brush->color = 0;
}

GpSolidFill *
gdip_solidfill_new (void)
{
	GpSolidFill *result = (GpSolidFill *) GdipAlloc (sizeof (GpSolidFill));
	
	gdip_solidfill_init (result);
	return result;
}

void
gdip_solidfill_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpSolidFill *solid = (GpSolidFill *) brush;
        int R = (solid->color & 0x00FF0000 ) >> 16;
        int G = (solid->color & 0x0000FF00 ) >> 8;
        int B = (solid->color & 0x000000FF );
        cairo_set_rgb_color (graphics->ct, (double) R, (double) G, (double) B);
}

void
gdip_solidfill_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpSolidFill *result = (GpSolidFill *) GdipAlloc (sizeof (GpSolidFill));
	GpSolidFill *solid = (GpSolidFill *) brush;

	result->base = solid->base;
        result->color = solid->color;

	*clonedBrush = (GpBrush *) result;
}

void 
gdip_solidfill_destroy (GpBrush *brush)
{
	GdipFree (brush);
}

GpStatus 
GdipCreateSolidFill (int color, GpSolidFill **brush)
{
	*brush = gdip_solidfill_new ();
	(*brush)->color = color;
	return Ok;
}

GpStatus
GdipSetSolidFillColor (GpSolidFill *brush, int color)
{
        brush->color = color;
        return Ok;
}

GpStatus
GdipGetSolidFillColor (GpSolidFill *brush, int *color)
{
        *color = brush->color;
        return Ok;
}
