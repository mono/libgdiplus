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

#include "region-path-tree.h"
#include "graphics-path-private.h"

/*
 * gdip_region_clear_tree:
 * @tree: a GpPathTree to clear
 *
 * Recursively clear (delete path and free memory) the specified path tree.
 */
void 
gdip_region_clear_tree (GpPathTree *tree)
{
	if (!tree)
		return;

	if (tree->path) {
		GdipDeletePath (tree->path);
		tree->path = NULL;
	} else {
		gdip_region_clear_tree (tree->branch1);
		GdipFree (tree->branch1);
		gdip_region_clear_tree (tree->branch2);
		GdipFree (tree->branch2);
	}
}


/*
 * gdip_region_copy_tree:
 * @source: the GpPathTree to copy
 * @dest: the GpPathTree copy
 *
 * Recursively copy (and allocate) the @source path tree into @dest.
 * If @source is present then we must have a valid @dest.
 */
GpStatus
gdip_region_copy_tree (GpPathTree *source, GpPathTree *dest)
{
	GpStatus status;

	if (!source)
		return Ok;

	g_assert (dest);
	if (source->path) {
		status = GdipClonePath (source->path, &dest->path);
		if (status != Ok)
			return status;

		dest->branch1 = NULL;
		dest->branch2 = NULL;
	} else {
		dest->path = NULL;
		dest->mode = source->mode;
		dest->branch1 = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
		if (!dest->branch1)
			return OutOfMemory;

		status = gdip_region_copy_tree (source->branch1, dest->branch1);
		if (status != Ok)
			return status;

		dest->branch2 = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
		if (!dest->branch2)
			return OutOfMemory;

		status = gdip_region_copy_tree (source->branch2, dest->branch2);
		if (status != Ok)
			return status;
	}

	return Ok;
}


/*
 * gdip_region_get_tree_size:
 * @tree: a GpPathTree
 *
 * Recursively calculate the size (in bytes) required to serialized @tree.
 */
UINT
gdip_region_get_tree_size (GpPathTree *tree)
{
	UINT result;

	if (tree->path) {
		/* tag, count, fillmode, types and points */
		result = 3 * sizeof (UINT) + 
			(tree->path->count * sizeof (BYTE)) +
			(tree->path->count * sizeof (GpPointF));
	} else {
		/* tag, operation, size (branch1), branch1, size (branch2), branch2 */
		result = 4 * sizeof (guint32);
		result += gdip_region_get_tree_size (tree->branch1);
		result += gdip_region_get_tree_size (tree->branch2);
	}
	return result;
}


/*
 * gdip_region_deserialize_tree:
 * @data: a byte array
 * @size: the length of the byte array
 * @tree: a GpPathTree
 *
 * Recursively deserialize the @tree from the supplied buffer @data. Returns 
 * TRUE if the deserialization was possible, or FALSE if a problem was found
 * (e.g. @size mismatch, bad data...)
 */
BOOL
gdip_region_deserialize_tree (BYTE *data, int size, GpPathTree *tree)
{
	int len = sizeof (guint32);
	guint32 tag;

	memcpy (&tag, data, len);
	data += len;
	size -= len;

	switch (tag) {
	case REGION_TAG_PATH: {
		/* deserialize a path from the memory blob */
		guint32 count;
		FillMode mode;

		tree->mode = CombineModeReplace;
		tree->branch1 = NULL;
		tree->branch2 = NULL;
		/* count */
		memcpy (&count, data, len);
		data += len;
		size -= len;
		/* mode (FillMode, not CombineMode) */
		memcpy (&mode, data, len);
		data += len;
		size -= len;
		/* check that the size match the length of the type (byte) and 
		   GpPointF for the specified count */
		if (size == count + count * sizeof (GpPointF)) {
			BYTE* types = data;
			GpPointF *points = (GpPointF*) (data + count);
			return (GdipCreatePath2 (points, types, count, mode, &tree->path) == Ok);
		}
		return FALSE;
		}
		break;
	case REGION_TAG_TREE: {
		guint branch_size;
		tree->path = NULL;
		/* operation */
		memcpy (&tree->mode, data, len);
		data += len;
		size -= len;
		/* size (branch1) */
		memcpy (&branch_size, data, len);
		data += len;
		size -= len;
		/* deserialize a tree from the memory blob */
		tree->branch1 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!tree->branch1)
			return FALSE;

		if (!gdip_region_deserialize_tree (data, branch_size, tree->branch1))
			return FALSE;
		data += branch_size;
		size -= branch_size;
		/* size (branch2) */
		memcpy (&branch_size, data, len);
		data += len;
		size -= len;
		tree->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!tree->branch2)
			return FALSE;

		if (!gdip_region_deserialize_tree (data, branch_size, tree->branch2))
			return FALSE;
		}
		break;
	default:
		g_warning ("Invalid tag %d", tag);
		return FALSE;
	}
	return TRUE;
}


/*
 * gdip_region_serialize_tree:
 * @tree: a GpPathTree
 * @buffer: a byte array
 * @bufferSize: the length of the byte array
 * @sizeFilled: a pointer to a integer
 *
 * Recursively serialize the @tree data in the supplied @buffer. Returns TRUE
 * if the serialization was possible, or FALSE if a problem was found (e.g. 
 * @bufferSize too small). If successful @sizeFilled will contains the actual
 * number of bytes that were required to serialize @tree.
 */
BOOL
gdip_region_serialize_tree (GpPathTree *tree, BYTE *buffer, UINT bufferSize, UINT *sizeFilled)
{
	if (tree->path) {
		/* tag */
		guint32 temp = REGION_TAG_PATH;
		int len = sizeof (guint32);
		memcpy (buffer, &temp, len);
		buffer += len;
		*sizeFilled += len;
		/* count */
		memcpy (buffer, &tree->path->count, len);
		buffer += len;
		*sizeFilled += len;
		/* fill_mode */
		temp = tree->path->fill_mode;
		memcpy (buffer, &temp, len);
		buffer += len;
		*sizeFilled += len;
		/* types */
		len = tree->path->count;
		memcpy (buffer, tree->path->types, len);
		buffer += len;
		*sizeFilled += len;
		/* points */
		len = tree->path->count * sizeof (GpPointF);
		memcpy (buffer, tree->path->points, len);
		buffer += len;
		*sizeFilled += len;
	} else {
		/* tag */
		BYTE *original = buffer;
		guint32 temp = REGION_TAG_TREE;
		int len = sizeof (guint32);
		memcpy (buffer, &temp, len);
		buffer += len;
		*sizeFilled += len;
		/* operation */
		temp = tree->mode;
		memcpy (buffer, &temp, len);
		buffer += len;
		*sizeFilled += len;
		/* serialize branch 1 (size + branch) */
		temp = gdip_region_get_tree_size (tree->branch1);
		memcpy (buffer, &temp, len);
		buffer += len;
		*sizeFilled += len;
		if (!gdip_region_serialize_tree (tree->branch1, buffer, bufferSize - (buffer - original), sizeFilled))
			return FALSE;
		buffer += temp;
		/* serialize branch 2 (size + branch)  */
		temp = gdip_region_get_tree_size (tree->branch2);
		memcpy (buffer, &temp, len);
		buffer += len;
		*sizeFilled += len;
		if (!gdip_region_serialize_tree (tree->branch2, buffer, bufferSize - (buffer - original), sizeFilled))
			return FALSE;
	}
	return TRUE;
}


/*
 * gdip_region_transform_tree:
 * @tree: a GpPathTree
 * @matrix: the GpMatrix to apply to the tree
 *
 * Recursively apply the @matrix to the @tree.
 */
GpStatus
gdip_region_transform_tree (GpPathTree *tree, GpMatrix *matrix)
{
	if (tree->path) {
		return GdipTransformPath (tree->path, matrix);
	} else {
		GpStatus status;
		status = gdip_region_transform_tree (tree->branch1, matrix);
		if (status == Ok)
			status = gdip_region_transform_tree (tree->branch2, matrix);
		return status;
	}
}


/*
 * gdip_region_translate_tree:
 * @tree: a GpPathTree
 * @dx: the delta x to apply to each point
 * @dy: the delta y to apply to each point 
 *
 * Recursively apply the @dx, @dy translation to each point, of each path, 
 * in the @tree.
 */
void
gdip_region_translate_tree (GpPathTree *tree, float dx, float dy)
{
	if (tree->path) {
		int i;
		for (i = 0; i < tree->path->count; i++) {
			GpPointF *point = tree->path->points + i;
			point->X += dx;
			point->Y += dy;
		}
	} else {
		gdip_region_translate_tree (tree->branch1, dx, dy);
		gdip_region_translate_tree (tree->branch2, dx, dy);
	}
}
