/*
 * graphics-path.c
 *
 * Copyright (C) 2004, Novell Inc.
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
 *
 * Author: Ravindra (rkumar@novell.com)
 *
 */
 
#include "gdip.h"
#include "graphics-path.h"

/* marker to keep track of the marker position */
static int marker = 0;
/* subpath to keep track of the subpath position */
static int subpath = 0;

GpStatus
GdipCreatePathIter (GpPathIterator **iterator, GpPath *path)
{
	GpPathIterator *iter;

	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (iterator != NULL, InvalidParameter);

	iter = (GpPathIterator *) GdipAlloc (sizeof (GpPathIterator));
	if (iter == NULL)
		return OutOfMemory;

	iter->path = path;
	*iterator = iter;

	return Ok;
}

GpStatus
GdipPathIterGetCount (GpPathIterator *iterator, int *count)
{
	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (count != NULL, InvalidParameter);

	*count = iterator->path->count;

	return Ok;
}

GpStatus
GdipPathIterGetSubpathCount (GpPathIterator *iterator, int *count)
{
	int i;
	int numSubpaths = 0;
	byte type;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (count != NULL, InvalidParameter);

	/* number of subpaths = num of starting points */
	for (i = 0; i < iterator->path->count; i++) {
		type = g_array_index (iterator->path->types, byte, i);
		if (type == PathPointTypeStart)
			numSubpaths++;
	}

	*count = numSubpaths;

	return Ok;
}

GpStatus
GdipDeletePathIter (GpPathIterator *iterator)
{
	g_return_val_if_fail (iterator != NULL, InvalidParameter);

	GdipFree (iterator);

	return Ok;
}

GpStatus
GdipPathIterCopyData (GpPathIterator *iterator, int *resultCount, GpPointF *points, byte *types, int startIndex, int endIndex)
{
	int i, j;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
	g_return_val_if_fail (types != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);

	if (startIndex >= iterator->path->count ||
	    endIndex >= iterator->path->count ||
	    endIndex > startIndex) {
		*resultCount = 0;
		return Ok;
	}

	for (i = startIndex, j = 0; i <= endIndex; i++, j++) {
		points [j] = g_array_index (iterator->path->points, GpPointF, i);
		types [j] = g_array_index (iterator->path->types, byte, i);
	}

	*resultCount = j + 1;

	return Ok;
}

GpStatus
GdipPathIterEnumerate (GpPathIterator *iterator, int *resultCount, GpPointF *points, byte *types, int count)
{
	int i;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
	g_return_val_if_fail (types != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);

	for (i = 0; i < count && i < iterator->path->count; i++) {
		points [i] = g_array_index (iterator->path->points, GpPointF, i);
		types [i] = g_array_index (iterator->path->types, byte, i);
	}

	*resultCount = i;

	return Ok;
}

GpStatus
GdipPathIterHasCurve (GpPathIterator *iterator, BOOL *curve)
{
	int i;
	byte type;
	BOOL hasCurve = FALSE;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (curve != NULL, InvalidParameter);

	/* we have a curve if any point type is bezier or bezier3 */
	for (i = 0; i < iterator->path->count; i++) {
		type = g_array_index (iterator->path->types, byte, i);
		if (type == PathPointTypeBezier || type == PathPointTypeBezier3) {
			hasCurve = TRUE;
			break;
		}
	}

	*curve = hasCurve;

	return Ok;
}

GpStatus
GdipPathIterNextMarkerPath (GpPathIterator *iterator, int *resultCount, GpPath *path)
{
	int index;
	byte type;
	GpPointF point;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);
	g_return_val_if_fail (path != NULL, InvalidParameter);

	if (marker == iterator->path->count)
		return Ok;

	/* clear the existing values from path */
	if (path->count > 0) {
		g_array_free (path->points, FALSE);
		g_byte_array_free (path->types, FALSE);
	}

	for (index = marker; index < iterator->path->count; index++) {
		type = g_array_index (iterator->path->types, byte, index);
		point = g_array_index (iterator->path->points, GpPointF, index);
		g_array_append_val (path->points, point);
		g_byte_array_append (path->types, &type, 1);

		/* copy the marker and stop copying the points when we reach a marker type */
		if (type & PathPointTypePathMarker) {
			index++;
			break;
		}
	}

	*resultCount = index - marker;
	marker = index;

	return Ok;
}

GpStatus
GdipPathIterNextMarker (GpPathIterator *iterator, int *resultCount, int *startIndex, int *endIndex)
{
	int index;
	byte type;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);
	g_return_val_if_fail (startIndex != NULL, InvalidParameter);
	g_return_val_if_fail (endIndex != NULL, InvalidParameter);

	if (marker == iterator->path->count) {
		*startIndex = 0;
		*endIndex = 0;
		*resultCount = 0;
		return Ok;
	}
	
	for (index = marker; index < iterator->path->count; index++) {
		type = g_array_index (iterator->path->types, byte, index);
		if (type & PathPointTypePathMarker)
			break;
	}

	*startIndex = marker;
	*endIndex = index;

	index++;

	*resultCount = index - marker;
	marker = index;

	return Ok;
}

GpStatus
GdipPathIterNextPathType (GpPathIterator *iterator, int *resultCount, byte *pathType, int *startIndex, int *endIndex)
{
	return NotImplemented;
}

GpStatus
GdipPathIterNextSubpathPath (GpPathIterator *iterator, int *resultCount, GpPath *path, BOOL *isClosed)
{
	return NotImplemented;
}

GpStatus
GdipPathIterNextSubpath (GpPathIterator *iterator, int *resultCount, int *startIndex, int *endIndex, BOOL *isClosed)
{
	return NotImplemented;
}

GpStatus
GdipPathIterRewind (GpPathIterator *iterator)
{
	g_return_val_if_fail (iterator != NULL, InvalidParameter);

	/* reset the marker and subpath cursors */
	marker = 0;
	subpath = 0;

	return Ok;
}
