/*
 * graphics-path.h
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
 *      Duncan Mak (duncan@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003, 2006-2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __GRAPHICS_PATH_H__
#define __GRAPHICS_PATH_H__

GpStatus WINGDIPAPI GdipCreatePath (GpFillMode brushMode, GpPath **path);
GpStatus WINGDIPAPI GdipCreatePath2 (GDIPCONST GpPointF *points, GDIPCONST BYTE *types, INT count, GpFillMode fillMode, GpPath **path);
GpStatus WINGDIPAPI GdipCreatePath2I (GDIPCONST GpPoint *points, GDIPCONST BYTE *types, INT count, GpFillMode fillMode, GpPath **path);
GpStatus WINGDIPAPI GdipClonePath (GpPath *path, GpPath **clonePath);
GpStatus WINGDIPAPI GdipDeletePath (GpPath *path);
GpStatus WINGDIPAPI GdipResetPath (GpPath *path);

GpStatus WINGDIPAPI GdipGetPointCount (GpPath *path, INT *count);
GpStatus WINGDIPAPI GdipGetPathTypes (GpPath *path, BYTE *types, INT count);
GpStatus WINGDIPAPI GdipGetPathPoints (GpPath *path, GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipGetPathPointsI (GpPath *path, GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipGetPathData (GpPath *path, GpPathData *pathData);

GpStatus WINGDIPAPI GdipGetPathFillMode (GpPath *path, GpFillMode *fillmode);
GpStatus WINGDIPAPI GdipSetPathFillMode (GpPath *path, GpFillMode fillmode);

GpStatus WINGDIPAPI GdipStartPathFigure (GpPath *path);
GpStatus WINGDIPAPI GdipClosePathFigure (GpPath *path);
GpStatus WINGDIPAPI GdipClosePathFigures (GpPath *path);

GpStatus WINGDIPAPI GdipSetPathMarker (GpPath *path);
GpStatus WINGDIPAPI GdipClearPathMarkers (GpPath *path);

GpStatus WINGDIPAPI GdipReversePath (GpPath *path);
GpStatus WINGDIPAPI GdipGetPathLastPoint (GpPath *path, GpPointF *lastPoint);

GpStatus WINGDIPAPI GdipAddPathLine (GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2);
GpStatus WINGDIPAPI GdipAddPathLine2 (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipAddPathArc (GpPath *path, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipAddPathBezier (GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);
GpStatus WINGDIPAPI GdipAddPathBeziers (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipAddPathCurve (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipAddPathCurve2 (GpPath *path, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipAddPathCurve3 (GpPath *path, GDIPCONST GpPointF *points, INT count, INT offset, INT numberOfSegments, REAL tension);
GpStatus WINGDIPAPI GdipAddPathClosedCurve (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipAddPathClosedCurve2 (GpPath *path, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipAddPathRectangle (GpPath *path, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipAddPathRectangles (GpPath *path, GDIPCONST GpRectF *rects, INT count);
GpStatus WINGDIPAPI GdipAddPathEllipse (GpPath *path, REAL x, REAL y, REAL width, REAL height);
GpStatus WINGDIPAPI GdipAddPathPie (GpPath *path, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipAddPathPolygon (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus WINGDIPAPI GdipAddPathPath (GpPath *path, GDIPCONST GpPath *addingPath, BOOL connect);
GpStatus WINGDIPAPI GdipAddPathString (GpPath *path, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFontFamily *family, INT style, 
	REAL emSize, GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *format);

GpStatus WINGDIPAPI GdipAddPathLineI (GpPath *path, INT x1, INT y1, INT x2, INT y2);
GpStatus WINGDIPAPI GdipAddPathLine2I (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipAddPathArcI (GpPath *path, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipAddPathBezierI (GpPath *path, INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, INT x4, INT y4);
GpStatus WINGDIPAPI GdipAddPathBeziersI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipAddPathCurveI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipAddPathCurve2I (GpPath *path, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipAddPathCurve3I (GpPath *path, GDIPCONST GpPoint *points, INT count, INT offset, INT numberOfSegments, REAL tension);
GpStatus WINGDIPAPI GdipAddPathClosedCurveI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipAddPathClosedCurve2I (GpPath *path, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus WINGDIPAPI GdipAddPathRectangleI (GpPath *path, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipAddPathRectanglesI (GpPath *path, GDIPCONST GpRect *rects, INT count);
GpStatus WINGDIPAPI GdipAddPathEllipseI (GpPath *path, INT x, INT y, INT width, INT height);
GpStatus WINGDIPAPI GdipAddPathPieI (GpPath *path, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus WINGDIPAPI GdipAddPathPolygonI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus WINGDIPAPI GdipAddPathStringI (GpPath *path, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFontFamily *family, INT style, 
	REAL emSize, GDIPCONST GpRect *layoutRect, GDIPCONST GpStringFormat *format);

GpStatus WINGDIPAPI GdipFlattenPath (GpPath *path, GpMatrix *matrix, REAL flatness);
GpStatus WINGDIPAPI GdipWindingModeOutline (GpPath *path, GpMatrix *matrix, REAL flatness);
GpStatus WINGDIPAPI GdipWidenPath (GpPath *nativePath, GpPen *pen, GpMatrix *matrix, REAL flatness);
GpStatus WINGDIPAPI GdipWarpPath (GpPath *nativePath, GpMatrix *matrix, GDIPCONST GpPointF *points, INT count, REAL srcx, REAL srcy, 
	REAL srcwidth, REAL srcheight, WarpMode warpMode, REAL flatness);
GpStatus WINGDIPAPI GdipTransformPath (GpPath* path, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipGetPathWorldBounds (GpPath *path, GpRectF *bounds, GDIPCONST GpMatrix *matrix, GDIPCONST GpPen *pen);
GpStatus WINGDIPAPI GdipGetPathWorldBoundsI (GpPath *path, GpRect *bounds, GDIPCONST GpMatrix *matrix, GDIPCONST GpPen *pen);
GpStatus WINGDIPAPI GdipIsVisiblePathPoint (GpPath *path, REAL x, REAL y, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisiblePathPointI (GpPath *path, INT x, INT y, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsOutlineVisiblePathPoint (GpPath *path, REAL x, REAL y, GpPen *pen, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsOutlineVisiblePathPointI (GpPath *path, INT x, INT y, GpPen *pen, GpGraphics *graphics, BOOL *result);

#endif
