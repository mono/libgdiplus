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

#include "graphics-metafile-private.h"

/*
 * NOTE: all parameter's validations are done inside graphics.c
 */

#define FIT_IN_INT16(x)		(((x) >= G_MININT16) && ((x) <= G_MAXINT16))

#define EMFPLUS_FLAGS_USE_SINGLE	0x0000
#define EMFPLUS_FLAGS_FILLMODE_WINDING	0x2000
#define EMFPLUS_FLAGS_USE_INT16		0x4000
#define EMFPLUS_FLAGS_USE_ARGB		0x8000

static BOOL
RectFitInInt16 (int x, int y, int width, int height)
{
	return (FIT_IN_INT16(x) && FIT_IN_INT16(y) && FIT_IN_INT16(width) && FIT_IN_INT16(height));
}

static BOOL
GpRectFitInInt16 (GDIPCONST GpRect *rect)
{
	return (FIT_IN_INT16(rect->X) && FIT_IN_INT16(rect->Y) && FIT_IN_INT16(rect->Width) && FIT_IN_INT16(rect->Height));
}

static BOOL
GpRectArrayFitInInt16 (GDIPCONST GpRect *rects, int count)
{
	GpRect *r = (GpRect*) rects;
	int i;
	for (i = 0; i < count; i++, r++) {
		if (!GpRectFitInInt16 (r))
			return FALSE;
	}
	return TRUE;
}

static GpRectF *
convert_rects (GDIPCONST GpRect *rects, int count)
{
        int i;
        GpRectF *result = (GpRectF *) GdipAlloc (sizeof (GpRectF) * count);
	if (!result)
		return NULL;

        for (i = 0; i < count; i++) {
                result [i].X = rects [i].X;
                result [i].Y = rects [i].Y;
                result [i].Width = rects [i].Width;
                result [i].Height = rects [i].Height;
        }
        return result;
}

static BOOL
GetARGBFromBrush (GDIPCONST GpBrush* brush, ARGB *color)
{
	/* if brush is a solid brush we can use (and return) it's color, otherwise it must be treated as an object */
	return FALSE;
}

/* DrawArcs - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawArc.html */

GpStatus
metafile_DrawArc (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, 
	float sweepAngle)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawArcI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_DrawArc (graphics, pen, x, y, width, height, startAngle, sweepAngle);
/* TODO */
	return Ok;
}

/* DrawBeziers - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawBeziers.html */

GpStatus 
metafile_DrawBezier (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2, float x3, float y3, 
	float x4, float y4)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawBezierI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
/* TODO */
	return Ok;
}

GpStatus 
metafile_DrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawBeziersI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
/* TODO */
	return Ok;
}

/*
 * DrawClosedCurve - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawClosedCurve.html
 */

GpStatus
metafile_DrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count, float tension)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count, float tension)
{
/* TODO */
	return Ok;
}

/*
 * FillClosedCurve - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillClosedCurve.html
 */

GpStatus
metafile_FillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, float tension)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_FillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count, float tension)
{
/* TODO */
	return Ok;
}

/*
 * DrawCurve - ?
 */

GpStatus
metafile_DrawCurve3 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, int count, int offset, int numOfSegments, 
	float tension)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawCurve3I (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPoint *points, int count, int offset, int numOfSegments,
	float tension)
{
/* TODO */
	return Ok;
}

/*
 * DrawEllipse - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawEllipse.html
 */

GpStatus 
metafile_DrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height)
{	
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawEllipseI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_DrawEllipse (graphics, pen, x, y, width, height);
/* TODO */
	return Ok;
}

/*
 * FillEllipse - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillEllipse.html
 */

GpStatus
metafile_FillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_FillEllipseI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_FillEllipse (graphics, brush, x, y, width, height);
/* TODO */
	return Ok;
}

/*
 * DrawLines - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawLines.html
 */

GpStatus
metafile_DrawLine (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawLineI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2)
{
/* TODO */
	return Ok;
}

GpStatus 
metafile_DrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawLinesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
/* TODO */
	return Ok;
}

/*
 * DrawPath - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawPath.html
 */
GpStatus
metafile_DrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
/* TODO */
	return Ok;
}

/*
 * FillPath - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillPath.html
 */

GpStatus
metafile_FillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
/* TODO */
	return Ok;
}

/*
 * DrawPie - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawPie.html
 */

GpStatus
metafile_DrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, 
	float startAngle, float sweepAngle)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawPieI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_DrawPie (graphics, pen, x, y, width, height, startAngle, sweepAngle);
/* TODO */
	return Ok;
}

/*
 * FillPie - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillPie.html
 */

GpStatus
metafile_FillPie (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, 
	float startAngle, float sweepAngle)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_FillPieI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height, 
	float startAngle, float sweepAngle)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_FillPie (graphics, brush, x, y, width, height, startAngle, sweepAngle);
/* TODO */
	return Ok;
}

/*
 * DrawPolygon - ?
 */

GpStatus
metafile_DrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawPolygonI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
/* TODO */
	return Ok;
}

/*
 * FillPolygon - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillPolygon.html
 */

GpStatus
metafile_FillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, FillMode fillMode)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_FillPolygonI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count, FillMode fillMode)
{
/* TODO */
	return Ok;
}

/*
 * DrawRects - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/DrawRects.html
 */

GpStatus
metafile_DrawRectangle (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawRectangleI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_DrawRectangleI (graphics, pen, x, y, width, height);
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, int count)
{
/* TODO */
	return Ok;
}

GpStatus
metafile_DrawRectanglesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects, int count)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!GpRectArrayFitInInt16 (rects, count)) {
		GpStatus status;
		GpRectF *rf = convert_rects (rects, count);
		if (!rf)
			return OutOfMemory;

		status = metafile_DrawRectangles (graphics, pen, rf, count);
		GdipFree (rf);
		return status;
	}
/* TODO */
	return Ok;
}

/*
 * FillRects - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillRects.html
 */

GpStatus
metafile_FillRectangle (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height)
{
/* TODO */
	return Ok;
}

GpStatus 
metafile_FillRectangleI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!RectFitInInt16 (x, y, width, height))
		return metafile_FillRectangle (graphics, brush, x, y, width, height);
/* TODO */
	return Ok;
}

GpStatus 
metafile_FillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, int count)
{
/* TODO */
	return Ok;
}

GpStatus 
metafile_FillRectanglesI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRect *rects, int count)
{
	/* every rectangle must fit into a INT16 or we must use the float version */
	if (!GpRectArrayFitInInt16 (rects, count)) {
		GpStatus status;
		GpRectF *rf = convert_rects (rects, count);
		if (!rf)
			return OutOfMemory;

		status = metafile_FillRectangles (graphics, brush, rf, count);
		GdipFree (rf);
		return status;
	}
/* TODO */
	return Ok;
}

/*
 * FillRegion - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillRegion.html
 */

GpStatus
metafile_FillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region)
{
/* TODO */
	return Ok;
}

/*
 * Clear - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/Clear.html
 */

GpStatus
metafile_GraphicsClear (GpGraphics *graphics, ARGB color)
{
/* TODO */
	return Ok;
}

/*
 * SetCompositingMode - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetCompositingMode.html
 */

GpStatus
metafile_SetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode)
{
/* TODO */
	return Ok;
}

/*
 * SetCompositingQuality - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetCompositingQuality.html
 */

GpStatus
metafile_SetCompositingQuality (GpGraphics *graphics, CompositingQuality compositingQuality)
{
/* TODO */
	return Ok;
}

/*
 * SetInterpolationMode - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetInterpolationMode.html
 */

GpStatus
metafile_SetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode)
{
/* TODO */
	return Ok;
}

/*
 * SetPixelOffsetMode - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetPixelOffsetMode.html
 */

GpStatus
metafile_SetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode)
{
/* TODO */
	return Ok;
}

/*
 * SetPageTransform (SetPageUnit+SetPageScale) - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetPageTransform.html
 */

GpStatus
metafile_SetPageTransform (GpGraphics *graphics, GpUnit unit, float scale)
{
/* TODO */
	return Ok;
}

/*
 * SetRenderingOrigin - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetRenderingOrigin.html
 */

GpStatus 
metafile_SetRenderingOrigin (GpGraphics *graphics, int x, int y)
{
/* TODO */
	return Ok;
}

/*
 * SetSmoothingMode - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetAntiAliasMode.html
 */

GpStatus
metafile_SetSmoothingMode (GpGraphics *graphics, SmoothingMode mode)
{
/* TODO */
	return Ok;
}

/*
 * GdipSetTextContrast - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetTextContrast.html
 */

GpStatus
metafile_SetTextContrast (GpGraphics *graphics, UINT contrast)
{
/* TODO */
	return Ok;
}

/*
 * SetTextRenderingHint - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetTextRenderingHint.html
 */

GpStatus
metafile_SetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode)
{
/* TODO */
	return Ok;
}

/*
 * ResetClip - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/ResetClip.html
 */

GpStatus
metafile_ResetClip (GpGraphics *graphics)
{
/* TODO */
	return Ok;
}

/*
 * SetClipPath - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetClipPath.html
 */

GpStatus
metafile_SetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode)
{
/* TODO */
	return Ok;
}

/*
 * SetClipRect - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetClipRect.html
 */

GpStatus
metafile_SetClipRect (GpGraphics *graphics, float x, float y, float width, float height, CombineMode combineMode)
{
/* TODO */
	return Ok;
}

/*
 * SetClipRegion - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetClipRegion.html
 */

GpStatus
metafile_SetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode)
{
/* TODO */
	return Ok;
}

/*
 * OffsetClip - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/OffsetClip.html
 */

GpStatus
metafile_TranslateClip (GpGraphics *graphics, float dx, float dy)
{
/* TODO */
	return Ok;
}

/*
 * ResetWorldTransform - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/ResetWorldTransform.html
 */

GpStatus
metafile_ResetWorldTransform (GpGraphics *graphics)
{
/* TODO */
	return Ok;
}

/*
 * SetWorldTransform - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/SetWorldTransform.html
 */

GpStatus
metafile_SetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
/* TODO */
	return Ok;
}

/*
 * http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/MultiplyWorldTransform.html
 */

GpStatus
metafile_MultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order)
{
/* TODO */
	return Ok;
}

/*
 * RotateWorldTransform - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/RotateWorldTransform.html
 */

GpStatus
metafile_RotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order)
{
/* TODO */
	return Ok;
}

/*
 * ScaleWorldTransform - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/ScaleWorldTransform.html
 */

GpStatus
metafile_ScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order)
{
/* TODO */
	return Ok;
}

/*
 * TranslateWorldTransform - http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/TranslateWorldTransform.html
 */

GpStatus
metafile_TranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order)
{
/* TODO */
	return Ok;
}
