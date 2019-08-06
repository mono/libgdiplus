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

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __REGION_BITMAP_H__
#define __REGION_BITMAP_H__

#include "gdiplus-private.h"
#include "bitmap-private.h"

/*
 * REGION_MAX_BITMAP_SIZE defines the size limit of the region bitmap we keep
 * in memory. The current value is 2 megabits which should be enough for any 
 * on-screen region. Before changing this value remember that a "real", but 
 * temporary, ARGB32 bitmap (32 times bigger, i.e. 8MB) may be allocated when 
 * converting the path into the region bitmap.
 */
#define REGION_MAX_BITMAP_SIZE		(2 * 1024 * 1024 >> 3)

#define SHAPE_SIZE(shape)		(((shape)->Width * (shape)->Height) >> 3)


typedef struct {
	int X;
	int Y;
	int Width;
	int Height;
	unsigned char *Mask;
	BOOL reduced;
} GpRegionBitmap;


void gdip_region_bitmap_ensure (GpRegion *region) GDIP_INTERNAL;
GpRegionBitmap* gdip_region_bitmap_from_path (GpPath *path) GDIP_INTERNAL;
GpRegionBitmap* gdip_region_bitmap_clone (GpRegionBitmap *bitmap) GDIP_INTERNAL;

void gdip_region_bitmap_free (GpRegionBitmap *bitmap) GDIP_INTERNAL;
void gdip_region_bitmap_invalidate (GpRegion *region) GDIP_INTERNAL;

BOOL gdip_region_bitmap_compare (GpRegionBitmap *shape1, GpRegionBitmap *shape2) GDIP_INTERNAL;
BOOL gdip_region_bitmap_is_point_visible (GpRegionBitmap *bitmap, int x, int y) GDIP_INTERNAL;
BOOL gdip_region_bitmap_is_rect_visible (GpRegionBitmap *bitmap, GpRect *rect) GDIP_INTERNAL;

int gdip_region_bitmap_get_scans (GpRegionBitmap *bitmap, GpRectF *rect) GDIP_INTERNAL;

void gdip_region_bitmap_get_smallest_rect (GpRegionBitmap *bitmap, GpRect *rect) GDIP_INTERNAL;
void gdip_region_bitmap_shrink (GpRegionBitmap *bitmap, BOOL always_shrink) GDIP_INTERNAL;

cairo_surface_t *gdip_region_bitmap_to_cairo_surface (GpRegionBitmap *bitmap) GDIP_INTERNAL;

GpRegionBitmap* gdip_region_bitmap_combine (GpRegionBitmap *bitmap1, GpRegionBitmap* bitmap2, CombineMode combineMode) GDIP_INTERNAL;

#endif
