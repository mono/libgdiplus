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

#ifndef _REGION_H_
#define _REGION_H_

#include "gdip.h"
#include "region-bitmap.h"
#include "region-path-tree.h"

/* internal (private) API for regions */

#define REGION_INFINITE_POSITION	-4194304
#define REGION_INFINITE_LENGTH		8388608

BOOL gdip_is_InfiniteRegion (GpRegion *region) GDIP_INTERNAL;
BOOL gdip_is_Point_in_RectF_inclusive (float x, float y, GpRectF* rect) GDIP_INTERNAL;

void gdip_clear_region (GpRegion *region) GDIP_INTERNAL;
void gdip_copy_region (GpRegion *source, GpRegion *dest) GDIP_INTERNAL;

#endif /* _REGION_H_ */
