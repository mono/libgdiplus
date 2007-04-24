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

GpStatus GdipCreatePath (GpFillMode brushMode, GpPath **path);
GpStatus GdipCreatePath2 (GDIPCONST GpPointF *points, GDIPCONST BYTE *types, INT count, GpFillMode fillMode, GpPath **path);
GpStatus GdipCreatePath2I (GDIPCONST GpPoint *points, GDIPCONST BYTE *types, INT count, GpFillMode fillMode, GpPath **path);
GpStatus GdipClonePath (GpPath *path, GpPath **clonePath);
GpStatus GdipDeletePath (GpPath *path);
GpStatus GdipResetPath (GpPath *path);

GpStatus GdipGetPointCount (GpPath *path, INT *count);
GpStatus GdipGetPathTypes (GpPath *path, BYTE *types, INT count);
GpStatus GdipGetPathPoints (GpPath *path, GpPointF *points, INT count);
GpStatus GdipGetPathPointsI (GpPath *path, GpPoint *points, INT count);
GpStatus GdipGetPathData (GpPath *path, GpPathData *pathData);

GpStatus GdipGetPathFillMode (GpPath *path, GpFillMode *fillmode);
GpStatus GdipSetPathFillMode (GpPath *path, GpFillMode fillmode);

GpStatus GdipStartPathFigure (GpPath *path);
GpStatus GdipClosePathFigure (GpPath *path);
GpStatus GdipClosePathFigures (GpPath *path);

GpStatus GdipSetPathMarker (GpPath *path);
GpStatus GdipClearPathMarker (GpPath *path);

GpStatus GdipReversePath (GpPath *path);
GpStatus GdipGetPathLastPoint (GpPath *path, GpPointF *lastPoint);

GpStatus GdipAddPathLine (GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2);
GpStatus GdipAddPathLine2 (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus GdipAddPathArc (GpPath *path, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus GdipAddPathBezier (GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);
GpStatus GdipAddPathBeziers (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus GdipAddPathCurve (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus GdipAddPathCurve2 (GpPath *path, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus GdipAddPathCurve3 (GpPath *path, GDIPCONST GpPointF *points, INT count, INT offset, INT numberOfSegments, REAL tension);
GpStatus GdipAddPathClosedCurve (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus GdipAddPathClosedCurve2 (GpPath *path, GDIPCONST GpPointF *points, INT count, REAL tension);
GpStatus GdipAddPathRectangle (GpPath *path, REAL x, REAL y, REAL width, REAL height);
GpStatus GdipAddPathRectangles (GpPath *path, GDIPCONST GpRectF *rects, INT count);
GpStatus GdipAddPathEllipse (GpPath *path, REAL x, REAL y, REAL width, REAL height);
GpStatus GdipAddPathPie (GpPath *path, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
GpStatus GdipAddPathPolygon (GpPath *path, GDIPCONST GpPointF *points, INT count);
GpStatus GdipAddPathPath (GpPath *path, GDIPCONST GpPath *addingPath, BOOL connect);
GpStatus GdipAddPathString (GpPath *path, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFontFamily *family, INT style, 
	REAL emSize, GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *format);

GpStatus GdipAddPathLineI (GpPath *path, INT x1, INT y1, INT x2, INT y2);
GpStatus GdipAddPathLine2I (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus GdipAddPathArcI (GpPath *path, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus GdipAddPathBezierI (GpPath *path, INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, INT x4, INT y4);
GpStatus GdipAddPathBeziersI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus GdipAddPathCurveI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus GdipAddPathCurve2I (GpPath *path, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus GdipAddPathCurve3I (GpPath *path, GDIPCONST GpPoint *points, INT count, INT offset, INT numberOfSegments, REAL tension);
GpStatus GdipAddPathClosedCurveI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus GdipAddPathClosedCurve2I (GpPath *path, GDIPCONST GpPoint *points, INT count, REAL tension);
GpStatus GdipAddPathRectangleI (GpPath *path, INT x, INT y, INT width, INT height);
GpStatus GdipAddPathRectanglesI (GpPath *path, GDIPCONST GpRect *rects, INT count);
GpStatus GdipAddPathEllipseI (GpPath *path, INT x, INT y, INT width, INT height);
GpStatus GdipAddPathPieI (GpPath *path, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle);
GpStatus GdipAddPathPolygonI (GpPath *path, GDIPCONST GpPoint *points, INT count);
GpStatus GdipAddPathStringI (GpPath *path, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFontFamily *family, INT style, 
	REAL emSize, GDIPCONST GpRect *layoutRect, GDIPCONST GpStringFormat *format);

GpStatus GdipFlattenPath (GpPath *path, GpMatrix *matrix, REAL flatness);
GpStatus GdipWindingModeOutline (GpPath *path, GpMatrix *matrix, REAL flatness);
GpStatus GdipWidenPath (GpPath *nativePath, GpPen *pen, GpMatrix *matrix, REAL flatness);
GpStatus GdipWarpPath (GpPath *nativePath, GpMatrix *matrix, GDIPCONST GpPointF *points, INT count, REAL srcx, REAL srcy, 
	REAL srcwidth, REAL srcheight, WarpMode warpMode, REAL flatness);
GpStatus GdipTransformPath (GpPath* path, GpMatrix *matrix);
GpStatus GdipGetPathWorldBounds (GpPath *path, GpRectF *bounds, GDIPCONST GpMatrix *matrix, GDIPCONST GpPen *pen);
GpStatus GdipGetPathWorldBoundsI (GpPath *path, GpRect *bounds, GDIPCONST GpMatrix *matrix, GDIPCONST GpPen *pen);
GpStatus GdipIsVisiblePathPoint (GpPath *path, REAL x, REAL y, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisiblePathPointI (GpPath *path, INT x, INT y, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsOutlineVisiblePathPoint (GpPath *path, REAL x, REAL y, GpPen *pen, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsOutlineVisiblePathPointI (GpPath *path, INT x, INT y, GpPen *pen, GpGraphics *graphics, BOOL *result);

#endif
