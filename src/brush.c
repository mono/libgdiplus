/*
 * brush.c
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

#include "brush-private.h"
#include "graphics-private.h"

void
gdip_brush_init (GpBrush *brush, BrushClass* vtable)
{
	brush->vtable = vtable;

	/* Set the changed state to true, so that we don't miss the
	 * first setup of the brush.
	 */
	brush->changed = TRUE;
}

GpStatus
gdip_brush_setup (GpGraphics *graphics, GpBrush *brush)
{
	/* Don't need to setup, if brush is the same as the cached brush and
	 * it is not changed. Just comparing pointers may not be sufficient
	 * to say that the brushes are same. It is possible to have different
	 * brush on the same memory, but probability is very low. We would
	 * need a function to check the equality of the brushes in that case.
	 */
	if (brush == graphics->last_brush && !brush->changed)
		return Ok;
	else {
		GpStatus status = brush->vtable->setup (graphics, brush);
		if (status == Ok) {
			brush->changed = FALSE;
			graphics->last_brush = brush;
		}

		return status;
	}
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI
GdipCloneBrush (GpBrush *brush, GpBrush **clonedBrush)
{
	if (!brush || !clonedBrush)
		return InvalidParameter;

	return brush->vtable->clone_brush (brush, clonedBrush);
}

GpStatus WINGDIPAPI
GdipDeleteBrush (GpBrush *brush)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	status = brush->vtable->destroy (brush);
	GdipFree (brush);

	return status;
}

GpStatus WINGDIPAPI
GdipGetBrushType (GpBrush *brush, GpBrushType *type)
{
	if (!brush || !type)
		return InvalidParameter;

	*type = brush->vtable->type;
	return Ok;
}
