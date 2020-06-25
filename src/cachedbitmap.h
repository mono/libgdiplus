#ifndef __CACHEDBITMAP_H__
#define __CACHEDBITMAP_H__

GpStatus WINGDIPAPI GdipCreateCachedBitmap (GpBitmap *bitmap, GpGraphics *graphics, GpCachedBitmap **cachedBitmap);

GpStatus WINGDIPAPI GdipDeleteCachedBitmap (GpCachedBitmap *cachedBitmap);

#endif