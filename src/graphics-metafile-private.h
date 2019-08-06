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

#ifndef __GRAPHICS_METAFILE_PRIVATE_H__
#define __GRAPHICS_METAFILE_PRIVATE_H__

#include "gdiplus-private.h"
#include "matrix-private.h"

GpStatus metafile_DrawArc (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, 
	float sweepAngle) GDIP_INTERNAL;

GpStatus metafile_DrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) GDIP_INTERNAL;

GpStatus metafile_DrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count, 
	float tension) GDIP_INTERNAL;
GpStatus metafile_FillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, 
	float tension, GpFillMode fillMode) GDIP_INTERNAL;
GpStatus metafile_DrawCurve3 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, int count, int offset, 
	int numOfSegments, float tension) GDIP_INTERNAL;

GpStatus metafile_DrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height) GDIP_INTERNAL;
GpStatus metafile_FillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height) GDIP_INTERNAL;

GpStatus metafile_DrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) GDIP_INTERNAL;

GpStatus metafile_FillRectangle (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height) GDIP_INTERNAL;
GpStatus metafile_DrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, int count) GDIP_INTERNAL;
GpStatus metafile_FillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, int count) GDIP_INTERNAL;

GpStatus metafile_DrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path) GDIP_INTERNAL;
GpStatus metafile_FillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path) GDIP_INTERNAL;

GpStatus metafile_DrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, 
	float startAngle, float sweepAngle) GDIP_INTERNAL;
GpStatus metafile_FillPie (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, 
	float startAngle, float sweepAngle) GDIP_INTERNAL;

GpStatus metafile_DrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) GDIP_INTERNAL;
GpStatus metafile_FillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, 
	FillMode fillMode) GDIP_INTERNAL;

GpStatus metafile_FillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region) GDIP_INTERNAL;

GpStatus metafile_GraphicsClear (GpGraphics *graphics, ARGB color) GDIP_INTERNAL;
GpStatus metafile_SetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode) GDIP_INTERNAL;
GpStatus metafile_SetCompositingQuality (GpGraphics *graphics, CompositingQuality compositingQuality) GDIP_INTERNAL;
GpStatus metafile_SetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode) GDIP_INTERNAL;
GpStatus metafile_SetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode) GDIP_INTERNAL;
GpStatus metafile_SetRenderingOrigin (GpGraphics *graphics, int x, int y) GDIP_INTERNAL;
GpStatus metafile_SetPageTransform (GpGraphics *graphics, GpUnit unit, float scale) GDIP_INTERNAL;
GpStatus metafile_SetSmoothingMode (GpGraphics *graphics, SmoothingMode mode) GDIP_INTERNAL;
GpStatus metafile_SetTextContrast (GpGraphics *graphics, UINT contrast) GDIP_INTERNAL;
GpStatus metafile_SetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode) GDIP_INTERNAL;

GpStatus metafile_ResetClip (GpGraphics *graphics) GDIP_INTERNAL;
GpStatus metafile_SetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode) GDIP_INTERNAL;
GpStatus metafile_SetClipRect (GpGraphics *graphics, float x, float y, float width, float height, 
	CombineMode combineMode) GDIP_INTERNAL;
GpStatus metafile_SetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode) GDIP_INTERNAL;
GpStatus metafile_TranslateClip (GpGraphics *graphics, float dx, float dy) GDIP_INTERNAL;

GpStatus metafile_ResetWorldTransform (GpGraphics *graphics) GDIP_INTERNAL;
GpStatus metafile_SetWorldTransform (GpGraphics *graphics, GpMatrix *matrix) GDIP_INTERNAL;
GpStatus metafile_MultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order) GDIP_INTERNAL;
GpStatus metafile_RotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order) GDIP_INTERNAL;
GpStatus metafile_ScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order) GDIP_INTERNAL;
GpStatus metafile_TranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order) GDIP_INTERNAL;

#endif
