/*
 * Copyright (C) 2006-2007 Novell, Inc (http://www.novell.com)
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
 *          Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

/* Graphics public API (only!) */

typedef unsigned int GraphicsContainer;

GpStatus GdipBeginContainer (GpGraphics *graphics, GDIPCONST GpRectF* dstrect, GDIPCONST GpRectF *srcrect, GpUnit unit, GraphicsContainer *state);
GpStatus GdipBeginContainer2 (GpGraphics *graphics, GraphicsContainer* state);
GpStatus GdipBeginContainerI (GpGraphics *graphics, GDIPCONST GpRect* dstrect, GDIPCONST GpRect *srcrect, GpUnit unit, GraphicsContainer *state);
GpStatus GdipEndContainer (GpGraphics *graphics, GraphicsContainer state);

GpStatus GdipCreateFromHDC (HDC hDC, GpGraphics **graphics);
GpStatus GdipDeleteGraphics (GpGraphics *graphics);

GpStatus GdipGetDC (GpGraphics *graphics, HDC *hDC);
GpStatus GdipReleaseDC (GpGraphics *graphics, HDC hDC);

GpStatus GdipRestoreGraphics (GpGraphics *graphics, unsigned int graphicsState);
GpStatus GdipSaveGraphics(GpGraphics *graphics, unsigned int * state);

GpStatus GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix);
GpStatus GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix);
GpStatus GdipMultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipResetWorldTransform (GpGraphics *graphics);
GpStatus GdipScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order);
GpStatus GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order);
GpStatus GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order);

GpStatus GdipDrawArc (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus GdipDrawArcI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus GdipDrawBezier (GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);
GpStatus GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, INT x4, INT y4);
GpStatus GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus GdipDrawCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus GdipDrawCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus GdipDrawCurve3 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, INT offset, INT numOfSegments, REAL tension);
GpStatus GdipDrawCurve3I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, INT offset, INT numOfSegments, REAL tension);
GpStatus GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height);
GpStatus GdipDrawEllipseI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height);
GpStatus GdipDrawImage (GpGraphics *graphics, GpImage *image, REAL x, REAL y);
GpStatus GdipDrawImageI (GpGraphics *graphics, GpImage *image, INT x, INT y);
GpStatus GdipDrawImageRect (GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL width, REAL height);
GpStatus GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, INT x, INT y, INT width, INT height);
GpStatus GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, INT count);
GpStatus GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, INT count);
GpStatus GdipDrawImagePointRect (GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit);
GpStatus GdipDrawImagePointRectI (GpGraphics *graphics, GpImage *image, INT x, INT y, INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit);
GpStatus GdipDrawImageRectRect (GpGraphics *graphics, GpImage *image, REAL dstx, REAL dsty, REAL dstwidth, REAL dstheight, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImageRectRectI (GpGraphics *graphics, GpImage *image, INT dstx, INT dsty, INT dstwidth, INT dstheight, INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImagePointsRect (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *points, INT count, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImagePointsRectI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *points, INT count, INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawLine (GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1, REAL x2, REAL y2);
GpStatus GdipDrawLineI (GpGraphics *graphics, GpPen *pen, INT x1, INT y1, INT x2, INT y2);
GpStatus GdipDrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path);
GpStatus GdipDrawPie (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus GdipDrawPieI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus GdipDrawRectangle (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height);
GpStatus GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height);
GpStatus GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, INT count);
GpStatus GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects, INT count);
GpStatus GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count);
GpStatus GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count);
GpStatus GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus GdipFillEllipse (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height);
GpStatus GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height);
GpStatus GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, REAL x1, REAL y1, REAL x2, REAL y2);
GpStatus GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, INT x1, INT y1, INT x2, INT y2);
GpStatus GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, INT count);
GpStatus GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRect *rects, INT count);
GpStatus GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count, FillMode fillMode);
GpStatus GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count, FillMode fillMode);
GpStatus GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count);
GpStatus GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count);
GpStatus GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path);
GpStatus GdipFillPie (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus GdipFillPieI( GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus GdipFillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region);
GpStatus GdipGraphicsClear (GpGraphics *graphics, ARGB color);

GpStatus GdipGetDpiX( GpGraphics *graphics, REAL *dpi);
GpStatus GdipGetDpiY (GpGraphics *graphics, REAL *dpi);
GpStatus GdipGetNearestColor (GpGraphics *graphics, ARGB *argb);

GpStatus GdipSetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode);
GpStatus GdipGetCompositingMode (GpGraphics *graphics, CompositingMode *compositingMode);
GpStatus GdipSetCompositingQuality (GpGraphics *graphics, CompositingQuality compositingQuality);
GpStatus GdipGetCompositingQuality (GpGraphics *graphics, CompositingQuality *compositingQuality);
GpStatus GdipSetInteroplationMode (GpGraphics *graphics, InterpolationMode imode);
GpStatus GdipGetInteroplationMode (GpGraphics *graphics, InterpolationMode *imode);
GpStatus GdipSetPageScale (GpGraphics *graphics, REAL scale);
GpStatus GdipGetPageScale (GpGraphics *graphics, REAL *scale);
GpStatus GdipSetPageUnit (GpGraphics *graphics, GpUnit unit);
GpStatus GdipGetPageUnit (GpGraphics *graphics, GpUnit *unit);
GpStatus GdipSetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode);
GpStatus GdipGetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode);
GpStatus GdipSetRenderingOrigin (GpGraphics *graphics, INT x, INT y);
GpStatus GdipGetRenderingOrigin (GpGraphics *graphics, INT *x, INT *y);
GpStatus GdipSetSmoothingMode (GpGraphics *graphics, SmoothingMode mode);
GpStatus GdipGetSmoothingMode (GpGraphics *graphics, SmoothingMode *mode);
GpStatus GdipSetTextContrast (GpGraphics *graphics, UINT contrast);
GpStatus GdipGetTextContrast (GpGraphics *graphics, UINT *contrast);
GpStatus GdipSetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode);
GpStatus GdipGetTextRenderingHint (GpGraphics *graphics, TextRenderingHint *mode);

GpStatus GdipIsVisiblePoint (GpGraphics *graphics, REAL x, REAL y, BOOL *result);
GpStatus GdipIsVisiblePointI (GpGraphics *graphics, INT x, INT y, BOOL *result);
GpStatus GdipIsVisibleRect (GpGraphics *graphics, REAL x, REAL y, REAL width, REAL height, BOOL *result);
GpStatus GdipIsVisibleRectI (GpGraphics *graphics, INT x, INT y, INT width, INT height, BOOL *result);
GpStatus GdipTransformPoints (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPointF *points, INT count);
GpStatus GdipTransformPointsI (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPoint *points, INT count);

GpStatus GdipResetClip (GpGraphics *graphics);
GpStatus GdipGetClip (GpGraphics *graphics, GpRegion *region);
GpStatus GdipGetClipBounds (GpGraphics *graphics, GpRectF *rect);
GpStatus GdipGetClipBoundsI (GpGraphics *graphics, GpRect *rect);
GpStatus GdipGetVisibleClipBounds (GpGraphics *graphics, GpRectF *rect);
GpStatus GdipGetVisibleClipBoundsI (GpGraphics *graphics, GpRect *rect);
GpStatus GdipIsClipEmpty (GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleClipEmpty (GpGraphics *graphics, BOOL *result);
GpStatus GdipSetClipGraphics (GpGraphics *graphics, GpGraphics *srcgraphics, CombineMode combineMode);
GpStatus GdipSetClipHrgn (GpGraphics *graphics, void *hRgn, CombineMode combineMode);
GpStatus GdipSetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode);
GpStatus GdipSetClipRect (GpGraphics *graphics, REAL x, REAL y, REAL width, REAL height, CombineMode combineMode);
GpStatus GdipSetClipRectI (GpGraphics *graphics, INT x, INT y, INT width, INT height, CombineMode combineMode);
GpStatus GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode);
GpStatus GdipSetVisibleClip_linux (GpGraphics *graphics, GpRect *rect);
GpStatus GdipTranslateClip (GpGraphics *graphics, REAL dx, REAL dy);
GpStatus GdipTranslateClipI (GpGraphics *graphics, INT dx, INT dy);

/* missing API
	GdipCreateFromHDC2
	GdipCreateFromHWND
	GdipCreateFromHWNDICM
	GdipCreateHalftonePalette
	GdipResetPageTransform
 */

#endif
