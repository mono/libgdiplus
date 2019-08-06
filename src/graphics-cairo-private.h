/*
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
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
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef __GRAPHICS_CAIRO_PRIVATE_H__
#define __GRAPHICS_CAIRO_PRIVATE_H__

#include "gdiplus-private.h"
#include "brush-private.h"
#include "matrix-private.h"

/* constant for make_ellipse */
#define C1	0.552285

/*
 * Handling of pens with a width greater than 1 is not identical between GDI+ and Cairo
 *
 * On cairo >= 1.12 pen adjustment is not required or tons of tests senstive to this break.
 */
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 12, 0)
#define gdip_cairo_pen_width_needs_adjustment(pen)	(0)
#else
#define gdip_cairo_pen_width_needs_adjustment(pen)	(((int)(pen->width) & 1) == 0)
#endif

cairo_fill_rule_t gdip_convert_fill_mode (FillMode fill_mode) GDIP_INTERNAL;
GpStatus gdip_plot_path (GpGraphics *graphics, GpPath *path, BOOL antialiasing) GDIP_INTERNAL;


GpStatus cairo_DrawArc (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, 
	float sweepAngle) GDIP_INTERNAL;

GpStatus cairo_DrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) GDIP_INTERNAL;

GpStatus cairo_DrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count, 
	float tension) GDIP_INTERNAL;
GpStatus cairo_FillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, 
	float tension, GpFillMode fillMode) GDIP_INTERNAL;
GpStatus cairo_DrawCurve3 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, int count, int offset, 
	int numOfSegments, float tension) GDIP_INTERNAL;

GpStatus cairo_DrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height) GDIP_INTERNAL;
GpStatus cairo_FillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height) GDIP_INTERNAL;

GpStatus cairo_DrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) GDIP_INTERNAL;

GpStatus cairo_DrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, int count) GDIP_INTERNAL;
GpStatus cairo_FillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, int count) GDIP_INTERNAL;

GpStatus cairo_DrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path) GDIP_INTERNAL;
GpStatus cairo_FillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path) GDIP_INTERNAL;

GpStatus cairo_DrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, 
	float startAngle, float sweepAngle) GDIP_INTERNAL;
GpStatus cairo_FillPie (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, 
	float startAngle, float sweepAngle) GDIP_INTERNAL;

GpStatus cairo_DrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) GDIP_INTERNAL;
GpStatus cairo_FillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, 
	FillMode fillMode) GDIP_INTERNAL;

GpStatus cairo_FillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region) GDIP_INTERNAL;

GpStatus cairo_GraphicsClear (GpGraphics *graphics, ARGB color) GDIP_INTERNAL;
GpStatus cairo_SetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode) GDIP_INTERNAL;
GpStatus cairo_SetSmoothingMode (GpGraphics *graphics, SmoothingMode mode) GDIP_INTERNAL;

GpStatus cairo_SetGraphicsClip (GpGraphics *graphics) GDIP_INTERNAL;
GpStatus cairo_ResetClip (GpGraphics *graphics) GDIP_INTERNAL;

GpStatus cairo_ResetWorldTransform (GpGraphics *graphics) GDIP_INTERNAL;
GpStatus cairo_SetWorldTransform (GpGraphics *graphics, GpMatrix *matrix) GDIP_INTERNAL;

#endif
