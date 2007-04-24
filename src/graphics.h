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

GpStatus GdipCreateFromHDC (void *hDC, GpGraphics **graphics);
GpStatus GdipDeleteGraphics (GpGraphics *graphics);
GpStatus GdipGetDC (GpGraphics *graphics, void **hDC);
GpStatus GdipReleaseDC (GpGraphics *graphics, void *hDC);
GpStatus GdipRestoreGraphics (GpGraphics *graphics, unsigned int graphicsState);
GpStatus GdipSaveGraphics(GpGraphics *graphics, unsigned int * state);
GpStatus GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix);
GpStatus GdipMultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipResetWorldTransform (GpGraphics *graphics);
GpStatus GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix);
GpStatus GdipScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order);
GpStatus GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order);
GpStatus GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order);
GpStatus GdipDrawArc (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, float sweepAngle);
GpStatus GdipDrawArcI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle);
GpStatus GdipDrawBezier (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
GpStatus GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
GpStatus GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawCurve2 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, float tension);
GpStatus GdipDrawCurve2I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, float tension);
GpStatus GdipDrawCurve3 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, int offset, int numOfSegments, float tension);
GpStatus GdipDrawCurve3I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, int offset, int numOfSegments, float tension);
GpStatus GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, float tension);
GpStatus GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, float tension);
GpStatus GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height);
GpStatus GdipDrawImage (GpGraphics *graphics, GpImage *image, float x, float y);
GpStatus GdipDrawImageI (GpGraphics *graphics, GpImage *image, int x, int y);
GpStatus GdipDrawImageRect (GpGraphics *graphics, GpImage *image, float x, float y, float width, float height);
GpStatus GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, int x, int y, int width, int height);
GpStatus GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, int count);
GpStatus GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, int count);
GpStatus GdipDrawImagePointRect (GpGraphics *graphics, GpImage *image, float x, float y, float srcx, float srcy, float srcwidth, float srcheight, GpUnit srcUnit);
GpStatus GdipDrawImagePointRectI (GpGraphics *graphics, GpImage *image, int x, int y, int srcx, int srcy, int srcwidth, int srcheight, GpUnit srcUnit);
GpStatus GdipDrawImageRectRect (GpGraphics *graphics, GpImage *image, float dstx, float dsty, float dstwidth, float dstheight, float srcx, float srcy, float srcwidth, float srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImageRectRectI (GpGraphics *graphics, GpImage *image, int dstx, int dsty, int dstwidth, int dstheight, int srcx, int srcy, int srcwidth, int srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImagePointsRect (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *points, int count, float srcx, float srcy, float srcwidth, float srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImagePointsRectI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *points, int count, int srcx, int srcy, int srcwidth, int srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawLine (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2);
GpStatus GdipDrawLineI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2);
GpStatus GdipDrawLines (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path);
GpStatus GdipDrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, float sweepAngle);
GpStatus GdipDrawPieI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle);
GpStatus GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawRectangle (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height);
GpStatus GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height);
GpStatus GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GpRectF *rects, int count);
GpStatus GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GpRect *rects, int count);
GpStatus GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count);
GpStatus GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count);
GpStatus GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count, float tension);
GpStatus GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count, float tension);
GpStatus GdipFillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height);
GpStatus GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height);
GpStatus GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, float x1, float y1, float x2, float y2);
GpStatus GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, int x1, int y1, int x2, int y2);
GpStatus GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GpRectF *rects, int count);
GpStatus GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GpRect *rects, int count);
GpStatus GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count, FillMode fillMode);
GpStatus GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count, FillMode fillMode);
GpStatus GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count);
GpStatus GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count);
GpStatus GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path);
GpStatus GdipSetRenderingOrigin (GpGraphics *graphics, int x, int y);
GpStatus GdipGetRenderingOrigin (GpGraphics *graphics, int *x, int *y);
GpStatus GdipGetDpiX(GpGraphics *graphics, float *dpi);
GpStatus GdipGetDpiY(GpGraphics *graphics, float *dpi);
GpStatus GdipFillPie(GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, float startAngle, float sweepAngle);
GpStatus GdipFillPieI(GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height, float startAngle, float sweepAngle);
GpStatus GdipGraphicsClear(GpGraphics *graphics, ARGB color);
GpStatus GdipFillRegion(GpGraphics *graphics, GpBrush *brush, GpRegion *region);
GpStatus GdipSetInteroplationMode(GpGraphics *graphics, InterpolationMode imode);
GpStatus GdipGetInteroplationMode(GpGraphics *graphics, InterpolationMode *imode);
GpStatus GdipSetTextRenderingHint(GpGraphics *graphics, TextRenderingHint mode);
GpStatus GdipGetTextRenderingHint(GpGraphics *graphics, TextRenderingHint *mode);
GpStatus GdipSetPixelOffsetMode(GpGraphics *graphics, PixelOffsetMode pixelOffsetMode);
GpStatus GdipGetPixelOffsetMode(GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode);
GpStatus GdipSetTextContrast(GpGraphics *graphics, UINT contrast);
GpStatus GdipGetTextContrast(GpGraphics *graphics, UINT *contrast);
GpStatus GdipSetSmoothingMode(GpGraphics *graphics, SmoothingMode mode);
GpStatus GdipGetSmoothingMode(GpGraphics *graphics, SmoothingMode *mode);
GpStatus GdipResetClip (GpGraphics *graphics);
GpStatus GdipSetClipGraphics (GpGraphics *graphics, GpGraphics *srcgraphics, CombineMode combineMode);
GpStatus GdipSetClipHrgn (GpGraphics *graphics, void *hRgn, CombineMode combineMode);
GpStatus GdipSetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode);
GpStatus GdipSetClipRect (GpGraphics *graphics, float x, float y, float width, float height, CombineMode combineMode);
GpStatus GdipSetClipRectI (GpGraphics *graphics, int x, int y, int width, int height, CombineMode combineMode);
GpStatus GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode);
GpStatus GdipSetVisibleClip_linux (GpGraphics *graphics, GpRect *rect);
GpStatus GdipTranslateClip (GpGraphics *graphics, float dx, float dy);
GpStatus GdipTranslateClipI (GpGraphics *graphics, int dx, int dy);

#endif
