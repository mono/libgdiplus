/*
 * Copyright (c) 2004 Ximian
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
 * Author:
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004
 *
 */

#include "gdip.h"
#include "gdip_win32.h"

GpStatus
GdipCreateRegion(GpRegion **region)
{
        GpRegion *result;

        result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	*region = result;

	return Ok;
        
}


GpStatus
GdipCreateRegionRect(GDIPCONST GpRectF *rect, GpRegion **region)
{

        return NotImplemented;
}


GpStatus
GdipCreateRegionRectI(GDIPCONST GpRect *rect, GpRegion **region)
{

        return NotImplemented;
}


GpStatus
GdipCreateRegionPath(GpPath *path, GpRegion **region)
{

        return NotImplemented;
}


GpStatus
GdipCloneRegion(GpRegion *region, GpRegion **cloneRegion)
{
        GpRegion *result = (GpRegion *) GdipAlloc (sizeof (GpRegion));

        memcpy (result, region, sizeof (GpRegion));

        *cloneRegion = result;
        return Ok; 
        
}



GpStatus
GdipDeleteRegion(GpRegion *region)
{

        return NotImplemented;
}


GpStatus
GdipSetInfinite(GpRegion *region)
{

        return NotImplemented;
}


GpStatus
GdipSetEmpty(GpRegion *region)
{

        return NotImplemented;
}


GpStatus
GdipCombineRegionRect(GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode)
{

        return NotImplemented;
}


GpStatus
GdipCombineRegionRectI(GpRegion *region, GDIPCONST GpRect *rect, CombineMode combineMode)
{

        return NotImplemented;
}


GpStatus
GdipCombineRegionPath(GpRegion *region, GpPath *path, CombineMode combineMode)
{

        return NotImplemented;
}


GpStatus
GdipCombineRegionRegion(GpRegion *region,  GpRegion *region2, CombineMode combineMode)
{

        return NotImplemented;
}


GpStatus
GdipTransformRegion(GpRegion *region, GpMatrix *matrix)
{

        return NotImplemented;
}


GpStatus
GdipGetRegionBounds(GpRegion *region, GpGraphics *graphics, GpRectF *rect)
{

        return NotImplemented;
}


GpStatus
GdipIsEmptyRegion(GpRegion *region, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsInfiniteRegion(GpRegion *region, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionPoint(GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionPointI(GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionRect(GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionRectI(GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}






