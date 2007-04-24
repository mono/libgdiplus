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

GpStatus GdipCreateRegion (GpRegion **region);
GpStatus GdipCreateRegionRect (GDIPCONST GpRectF *rect, GpRegion **region);
GpStatus GdipCreateRegionRectI (GDIPCONST GpRect *rect, GpRegion **region);
GpStatus GdipCreateRegionPath (GpPath *path, GpRegion **region);
GpStatus GdipCreateRegionRgnData (GDIPCONST BYTE *regionData, INT size, GpRegion **region);

GpStatus GdipCloneRegion(GpRegion *region, GpRegion **cloneRegion);
GpStatus GdipDeleteRegion(GpRegion *region);
GpStatus GdipSetInfinite(GpRegion *region);
GpStatus GdipSetEmpty(GpRegion *region);

GpStatus GdipCombineRegionRect(GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode);
GpStatus GdipCombineRegionRectI(GpRegion *region, GDIPCONST GpRect *rect, CombineMode combineMode);
GpStatus GdipCombineRegionPath(GpRegion *region, GpPath *path, CombineMode combineMode);
GpStatus GdipCombineRegionRegion(GpRegion *region,  GpRegion *region2, CombineMode combineMode);

GpStatus GdipGetRegionBounds(GpRegion *region, GpGraphics *graphics, GpRectF *rect);
GpStatus GdipIsEmptyRegion(GpRegion *region, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsInfiniteRegion(GpRegion *region, GpGraphics *graphics, BOOL *result);

GpStatus GdipIsVisibleRegionPoint(GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionPointI(GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionRect(GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionRectI(GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result);

GpStatus GdipGetRegionScansCount(GpRegion *region, int* count, GpMatrix* matrix);
GpStatus GdipGetRegionScans(GpRegion *region, GpRectF* rects, int* count, GpMatrix* matrix);

GpStatus GdipIsEqualRegion(GpRegion *region, GpRegion *region2, GpGraphics *graphics, BOOL *result);
GpStatus GdipGetRegionDataSize(GpRegion *region, UINT * bufferSize);
GpStatus GdipGetRegionData(GpRegion *region, BYTE * buffer, UINT bufferSize, UINT *sizeFilled);

GpStatus GdipTranslateRegion(GpRegion *region, float dx, float dy);
GpStatus GdipTranslateRegionI(GpRegion *region, int dx, int dy);
GpStatus GdipTransformRegion(GpRegion *region, GpMatrix *matrix);

#endif
