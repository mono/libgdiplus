/*
 * Copyright (C) 2006 Novell, Inc (http://www.novell.com)
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

#include "gdip.h"

/* internal (private) API for regions path trees */

#define REGION_TAG_PATH		1
#define REGION_TAG_TREE		2

void gdip_region_clear_tree (GpPathTree *tree);
void gdip_region_copy_tree (GpPathTree *source, GpPathTree *dest);

UINT gdip_region_get_tree_size (GpPathTree *tree);
BOOL gdip_region_deserialize_tree (BYTE *data, int size, GpPathTree *tree);
BOOL gdip_region_serialize_tree (GpPathTree *tree, BYTE *buffer, UINT bufferSize, UINT *sizeFilled);

void gdip_region_translate_tree (GpPathTree *tree, float dx, float dy);
GpStatus gdip_region_transform_tree (GpPathTree *tree, GpMatrix *matrix);

#endif /* _REGION_PATH_TREE_H_ */
