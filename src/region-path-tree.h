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

#ifndef _REGION_PATH_TREE_H_
#define _REGION_PATH_TREE_H_

#include "gdiplus-private.h"
#include "matrix-private.h"

/* internal (private) API for regions path trees */

#define REGION_TAG_PATH		1
#define REGION_TAG_TREE		2

typedef struct GpPathTree {
	CombineMode		mode;
	GpPath*			path;
	struct GpPathTree*	branch1;
	struct GpPathTree*	branch2;
} GpPathTree;

void gdip_region_clear_tree (GpPathTree *tree) GDIP_INTERNAL;
GpStatus gdip_region_copy_tree (GpPathTree *source, GpPathTree *dest) GDIP_INTERNAL;

UINT gdip_region_get_tree_size (GpPathTree *tree) GDIP_INTERNAL;
BOOL gdip_region_deserialize_tree (BYTE *data, int size, GpPathTree *tree) GDIP_INTERNAL;
BOOL gdip_region_serialize_tree (GpPathTree *tree, BYTE *buffer, UINT bufferSize, UINT *sizeFilled) GDIP_INTERNAL;

void gdip_region_translate_tree (GpPathTree *tree, float dx, float dy) GDIP_INTERNAL;
GpStatus gdip_region_transform_tree (GpPathTree *tree, GpMatrix *matrix) GDIP_INTERNAL;

#endif /* _REGION_PATH_TREE_H_ */
