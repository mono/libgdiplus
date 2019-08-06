/*
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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __REGION_PRIVATE_H__
#define __REGION_PRIVATE_H__

#include "gdiplus-private.h"
#include "matrix-private.h"
#include "region-bitmap.h"
#include "region-path-tree.h"

/* In the integer world infinity has limited bounds ;-) */
#define REGION_INFINITE_POSITION	-4194304
#define REGION_INFINITE_LENGTH		8388608

typedef enum {
    RegionTypeRect,
    RegionTypePath,
    RegionTypeInfinite
} RegionType;

typedef enum {
    RegionDataRect          = 0x10000000,
    RegionDataPath          = 0x10000001,
    RegionDataEmptyRect     = 0x10000002,
    RegionDataInfiniteRect  = 0x10000003
} RegionDataType;

typedef struct {
    DWORD size;
    DWORD checksum;
    DWORD magic;
    DWORD combiningOps;
} RegionHeader;

struct _Region {
    guint32		type;
    int		cnt;
    GpRectF*	rects;
    GpPathTree*	tree;
    GpRegionBitmap*	bitmap;
};

BOOL gdip_is_InfiniteRegion (const GpRegion *region) GDIP_INTERNAL;
BOOL gdip_is_Point_in_RectF_inclusive (float x, float y, GpRectF* rect) GDIP_INTERNAL;

void gdip_clear_region (GpRegion *region) GDIP_INTERNAL;
GpStatus gdip_copy_region (GpRegion *source, GpRegion *dest) GDIP_INTERNAL;

#include "region.h"

#endif
