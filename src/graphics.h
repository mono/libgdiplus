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

GpStatus WINGDIPAPI GdipBeginContainer (GpGraphics *graphics, GDIPCONST GpRectF *dstrect, GDIPCONST GpRectF *srcrect, GpUnit unit, GraphicsContainer *state);
GpStatus WINGDIPAPI GdipBeginContainer2 (GpGraphics *graphics, GraphicsContainer *state);
GpStatus WINGDIPAPI GdipBeginContainerI (GpGraphics *graphics, GDIPCONST GpRect *dstrect, GDIPCONST GpRect *srcrect, GpUnit unit, GraphicsContainer *state);
GpStatus WINGDIPAPI GdipEndContainer (GpGraphics *graphics, GraphicsContainer state);
GpStatus WINGDIPAPI GdipFlush (GpGraphics *graphics, GpFlushIntention intention);

GpStatus WINGDIPAPI GdipCreateFromHDC (HDC hdc, GpGraphics **graphics);
GpStatus WINGDIPAPI GdipCreateFromHDC2 (HDC hdc, HANDLE hDevice, GpGraphics **graphics);

GpStatus WINGDIPAPI GdipCreateFromHWND(HWND hwnd, GpGraphics **graphics);
GpStatus WINGDIPAPI GdipCreateFromHWNDICM(HWND hwnd, GpGraphics **graphics);
GpStatus WINGDIPAPI GdipDeleteGraphics (GpGraphics *graphics);

GpStatus WINGDIPAPI GdipGetDC (GpGraphics *graphics, HDC *hdc);
GpStatus WINGDIPAPI GdipReleaseDC (GpGraphics *graphics, HDC hdc);

GpStatus WINGDIPAPI GdipRestoreGraphics (GpGraphics *graphics, GraphicsState state);
GpStatus WINGDIPAPI GdipSaveGraphics (GpGraphics *graphics, GraphicsState *state);

GpStatus WINGDIPAPI GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipMultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipResetWorldTransform (GpGraphics *graphics);
GpStatus WINGDIPAPI GdipScaleWorldTransform (GpGraphics *graphics, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipRotateWorldTransform (GpGraphics *graphics, REAL angle, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipTranslateWorldTransform (GpGraphics *graphics, REAL dx, REAL dy, GpMatrixOrder order);

GpStatus WINGDIPAPI GdipDrawArc (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipDrawArcI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipDrawBezier (GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);
GpStatus WINGDIPAPI GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, INT x4, INT y4);
GpStatus WINGDIPAPI GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipDrawCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipDrawCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipDrawCurve3 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, INT offset, INT numOfSegments, REAL tension);
GpStatus WINGDIPAPI GdipDrawCurve3I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, INT offset, INT numOfSegments, REAL tension);
GpStatus WINGDIPAPI GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipDrawEllipseI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipDrawImage (GpGraphics *graphics, GpImage *image, REAL x, REAL y);
GpStatus WINGDIPAPI GdipDrawImageI (GpGraphics *graphics, GpImage *image, INT x, INT y);
GpStatus WINGDIPAPI GdipDrawImageRect (GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, INT count);
GpStatus WINGDIPAPI GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, INT count);
GpStatus WINGDIPAPI GdipDrawImagePointRect (GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit);
GpStatus WINGDIPAPI GdipDrawImagePointRectI (GpGraphics *graphics, GpImage *image, INT x, INT y, INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit);
GpStatus WINGDIPAPI GdipDrawImageRectRect (GpGraphics *graphics, GpImage *image, REAL dstx, REAL dsty, REAL dstwidth, REAL dstheight, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus WINGDIPAPI GdipDrawImageRectRectI (GpGraphics *graphics, GpImage *image, INT dstx, INT dsty, INT dstwidth, INT dstheight, INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus WINGDIPAPI GdipDrawImagePointsRect (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *points, INT count, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus WINGDIPAPI GdipDrawImagePointsRectI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *points, INT count, INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus WINGDIPAPI GdipDrawLine (GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1, REAL x2, REAL y2);
GpStatus WINGDIPAPI GdipDrawLineI (GpGraphics *graphics, GpPen *pen, INT x1, INT y1, INT x2, INT y2);
GpStatus WINGDIPAPI GdipDrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path);
GpStatus WINGDIPAPI GdipDrawPie (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipDrawPieI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipDrawRectangle (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, INT count);
GpStatus WINGDIPAPI GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects, INT count);
GpStatus WINGDIPAPI GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count, REAL tension, GpFillMode fillMode);
GpStatus WINGDIPAPI GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count, REAL tension, GpFillMode fillMode);
GpStatus WINGDIPAPI GdipFillEllipse (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, INT count);
GpStatus WINGDIPAPI GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRect *rects, INT count);
GpStatus WINGDIPAPI GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count, GpFillMode fillMode);
GpStatus WINGDIPAPI GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count, GpFillMode fillMode);
GpStatus WINGDIPAPI GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path);
GpStatus WINGDIPAPI GdipFillPie (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipFillPieI( GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipFillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region);
GpStatus WINGDIPAPI GdipGraphicsClear (GpGraphics *graphics, ARGB color);

GpStatus WINGDIPAPI GdipGetDpiX( GpGraphics *graphics, REAL *dpi);
GpStatus WINGDIPAPI GdipGetDpiY (GpGraphics *graphics, REAL *dpi);
GpStatus WINGDIPAPI GdipGetNearestColor (GpGraphics *graphics, ARGB *argb);

GpStatus WINGDIPAPI GdipSetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode);
GpStatus WINGDIPAPI GdipGetCompositingMode (GpGraphics *graphics, CompositingMode *compositingMode);
GpStatus WINGDIPAPI GdipSetCompositingQuality (GpGraphics *graphics, CompositingQuality compositingQuality);
GpStatus WINGDIPAPI GdipGetCompositingQuality (GpGraphics *graphics, CompositingQuality *compositingQuality);
GpStatus WINGDIPAPI GdipSetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode);
GpStatus WINGDIPAPI GdipGetInterpolationMode (GpGraphics *graphics, InterpolationMode *interpolationMode);
GpStatus WINGDIPAPI GdipResetPageTransform (GpGraphics *graphics);;
GpStatus WINGDIPAPI GdipSetPageScale (GpGraphics *graphics, REAL scale);
GpStatus WINGDIPAPI GdipGetPageScale (GpGraphics *graphics, REAL *scale);
GpStatus WINGDIPAPI GdipSetPageUnit (GpGraphics *graphics, GpUnit unit);
GpStatus WINGDIPAPI GdipGetPageUnit (GpGraphics *graphics, GpUnit *unit);
GpStatus WINGDIPAPI GdipSetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode);
GpStatus WINGDIPAPI GdipGetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode);
GpStatus WINGDIPAPI GdipSetRenderingOrigin (GpGraphics *graphics, INT x, INT y);
GpStatus WINGDIPAPI GdipGetRenderingOrigin (GpGraphics *graphics, INT *x, INT *y);
GpStatus WINGDIPAPI GdipSetSmoothingMode (GpGraphics *graphics, SmoothingMode smoothingMode);
GpStatus WINGDIPAPI GdipGetSmoothingMode (GpGraphics *graphics, SmoothingMode *smoothingMode);
GpStatus WINGDIPAPI GdipSetTextContrast (GpGraphics *graphics, UINT contrast);
GpStatus WINGDIPAPI GdipGetTextContrast (GpGraphics *graphics, UINT *contrast);
GpStatus WINGDIPAPI GdipSetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode);
GpStatus WINGDIPAPI GdipGetTextRenderingHint (GpGraphics *graphics, TextRenderingHint *mode);

GpStatus WINGDIPAPI GdipIsVisiblePoint (GpGraphics *graphics, REAL x, REAL y, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisiblePointI (GpGraphics *graphics, INT x, INT y, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisibleRect (GpGraphics *graphics, REAL x, REAL y, REAL width, REAL height, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisibleRectI (GpGraphics *graphics, INT x, INT y, INT width, INT height, BOOL *result);
GpStatus WINGDIPAPI GdipTransformPoints (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipTransformPointsI (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPoint *points, INT count);

GpStatus WINGDIPAPI GdipResetClip (GpGraphics *graphics);
GpStatus WINGDIPAPI GdipGetClip (GpGraphics *graphics, GpRegion *region);
GpStatus WINGDIPAPI GdipGetClipBounds (GpGraphics *graphics, GpRectF *rect);
GpStatus WINGDIPAPI GdipGetClipBoundsI (GpGraphics *graphics, GpRect *rect);
GpStatus WINGDIPAPI GdipGetVisibleClipBounds (GpGraphics *graphics, GpRectF *rect);
GpStatus WINGDIPAPI GdipGetVisibleClipBoundsI (GpGraphics *graphics, GpRect *rect);
GpStatus WINGDIPAPI GdipIsClipEmpty (GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisibleClipEmpty (GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipSetClipGraphics (GpGraphics *graphics, GpGraphics *srcgraphics, CombineMode combineMode);
GpStatus WINGDIPAPI GdipSetClipHrgn (GpGraphics *graphics, void *hRgn, CombineMode combineMode);
GpStatus WINGDIPAPI GdipSetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode);
GpStatus WINGDIPAPI GdipSetClipRect (GpGraphics *graphics, REAL x, REAL y, REAL width, REAL height, CombineMode combineMode);
GpStatus WINGDIPAPI GdipSetClipRectI (GpGraphics *graphics, INT x, INT y, INT width, INT height, CombineMode combineMode);
GpStatus WINGDIPAPI GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode);
GpStatus WINGDIPAPI GdipSetVisibleClip_linux (GpGraphics *graphics, GpRect *rect);
GpStatus WINGDIPAPI GdipTranslateClip (GpGraphics *graphics, REAL dx, REAL dy);
GpStatus WINGDIPAPI GdipTranslateClipI (GpGraphics *graphics, INT dx, INT dy);

GpStatus WINGDIPAPI GdipEnumerateMetafileDestPoint (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST PointF *destPoint,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestPointI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoint,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);

GpStatus WINGDIPAPI GdipEnumerateMetafileDestRect (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST RectF *destRect,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestRectI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Rect *destRect,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes );

GpStatus WINGDIPAPI GdipEnumerateMetafileDestPoints (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST PointF *destPoints, INT count,
    EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestPointsI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoints, INT count,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes );

GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPoint (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST PointF *destPoint, GDIPCONST RectF *srcRect,
    Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes * imageAttributes);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPointI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoint, GDIPCONST Rect *srcRect,
    Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);

GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestRect (GpGraphics * graphics, GDIPCONST GpMetafile *metafile, GDIPCONST RectF *destRect, GDIPCONST RectF *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestRectI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Rect *destRect, GDIPCONST Rect *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);

GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPoints (GpGraphics *graphics, GDIPCONST GpMetafile * metafile, GDIPCONST PointF *destPoints, INT count, GDIPCONST RectF *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPointsI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoints, INT count, GDIPCONST Rect *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes);

HPALETTE WINGDIPAPI GdipCreateHalftonePalette();

#endif
