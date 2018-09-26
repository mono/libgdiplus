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

#ifndef __REGION_H__
#define __REGION_H__

GpStatus WINGDIPAPI GdipCreateRegion (GpRegion **region);
GpStatus WINGDIPAPI GdipCreateRegionRect (GDIPCONST GpRectF *rect, GpRegion **region);
GpStatus WINGDIPAPI GdipCreateRegionRectI (GDIPCONST GpRect *rect, GpRegion **region);
GpStatus WINGDIPAPI GdipCreateRegionPath (GpPath *path, GpRegion **region);
GpStatus WINGDIPAPI GdipCreateRegionRgnData (GDIPCONST BYTE *regionData, INT size, GpRegion **region);

GpStatus WINGDIPAPI GdipCloneRegion(GpRegion *region, GpRegion **cloneRegion);
GpStatus WINGDIPAPI GdipDeleteRegion(GpRegion *region);
GpStatus WINGDIPAPI GdipSetInfinite(GpRegion *region);
GpStatus WINGDIPAPI GdipSetEmpty(GpRegion *region);

GpStatus WINGDIPAPI GdipCombineRegionRect(GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode);
GpStatus WINGDIPAPI GdipCombineRegionRectI(GpRegion *region, GDIPCONST GpRect *rect, CombineMode combineMode);
GpStatus WINGDIPAPI GdipCombineRegionPath(GpRegion *region, GpPath *path, CombineMode combineMode);
GpStatus WINGDIPAPI GdipCombineRegionRegion(GpRegion *region,  GpRegion *region2, CombineMode combineMode);

GpStatus WINGDIPAPI GdipGetRegionBounds(GpRegion *region, GpGraphics *graphics, GpRectF *rect);
GpStatus WINGDIPAPI GdipGetRegionHRgn(GpRegion *region, GpGraphics *graphics, HRGN *hRgn);
GpStatus WINGDIPAPI GdipIsEmptyRegion(GpRegion *region, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsInfiniteRegion(GpRegion *region, GpGraphics *graphics, BOOL *result);

GpStatus WINGDIPAPI GdipIsVisibleRegionPoint(GpRegion *region, REAL x, REAL y, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisibleRegionPointI(GpRegion *region, INT x, INT y, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisibleRegionRect(GpRegion *region, REAL x, REAL y, REAL width, REAL height, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipIsVisibleRegionRectI(GpRegion *region, INT x, INT y, INT width, INT height, GpGraphics *graphics, BOOL *result);

GpStatus WINGDIPAPI GdipGetRegionScansCount(GpRegion *region, UINT *count, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipGetRegionScans(GpRegion *region, GpRectF *rects, INT *count, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipGetRegionScansI(GpRegion *region, GpRect *rects, INT *count, GpMatrix *matrix);

GpStatus WINGDIPAPI GdipIsEqualRegion(GpRegion *region, GpRegion *region2, GpGraphics *graphics, BOOL *result);
GpStatus WINGDIPAPI GdipGetRegionDataSize(GpRegion *region, UINT * bufferSize);
GpStatus WINGDIPAPI GdipGetRegionData(GpRegion *region, BYTE * buffer, UINT bufferSize, UINT *sizeFilled);

GpStatus WINGDIPAPI GdipTranslateRegion(GpRegion *region, REAL dx, REAL dy);
GpStatus WINGDIPAPI GdipTranslateRegionI(GpRegion *region, INT dx, INT dy);
GpStatus WINGDIPAPI GdipTransformRegion(GpRegion *region, GpMatrix *matrix);

#endif
