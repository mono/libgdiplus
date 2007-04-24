/*
 * graphics-pathiterator.c
 *
 * Copyright (C) 2004,2006-2007 Novell Inc.
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
 
#include "graphics-pathiterator-private.h"
#include "graphics-path-private.h"
#include "font.h"

// coverity[+alloc : arg-*0]
GpStatus
GdipCreatePathIter (GpPathIterator **iterator, GpPath *path)
{
	GpPath *clone = NULL;
	GpPathIterator *iter;
	GpStatus status;

	if (!iterator)
		return InvalidParameter;

	iter = (GpPathIterator *) GdipAlloc (sizeof (GpPathIterator));
	if (iter == NULL)
		return OutOfMemory;

	/* supplying a path isn't required */
	if (path) {
		status = GdipClonePath (path, &clone);
		if (status != Ok) {
			GdipFree (iter);
			if (clone)
				GdipDeletePath (clone);
			return status;
		}
	}

	iter->path = clone;
	iter->markerPosition = 0;
	iter->subpathPosition = 0;
	iter->pathTypePosition = 0;

	*iterator = iter;

	return Ok;
}

GpStatus
GdipPathIterGetCount (GpPathIterator *iterator, int *count)
{
	if (!iterator || !count)
		return InvalidParameter;

	if (iterator->path)
		*count = iterator->path->count;
	else
		*count = 0;

	return Ok;
}

GpStatus
GdipPathIterGetSubpathCount (GpPathIterator *iterator, int *count)
{
	int numSubpaths = 0;

	if (!iterator || !count)
		return InvalidParameter;

	if (iterator->path) {
		int i;
		BYTE type;
		/* Number of subpaths = Number of starting points */
		for (i = 0; i < iterator->path->count; i++) {
			type = g_array_index (iterator->path->types, BYTE, i);
			if (type == PathPointTypeStart)
				numSubpaths++;
		}
	}

	*count = numSubpaths;

	return Ok;
}

GpStatus
GdipDeletePathIter (GpPathIterator *iterator)
{
	if (!iterator)
		return InvalidParameter;

	if (iterator->path) {
		GdipDeletePath (iterator->path);
		iterator->path = NULL;
	}

	GdipFree (iterator);

	return Ok;
}

GpStatus
GdipPathIterCopyData (GpPathIterator *iterator, int *resultCount, GpPointF *points, BYTE *types, int startIndex, int endIndex)
{
	int i, j;

	if (!iterator || !resultCount || !points || !types)
		return InvalidParameter;

	if (!iterator->path || (startIndex >= iterator->path->count) || (startIndex > endIndex) || 
		(endIndex >= iterator->path->count) || (startIndex < 0) || (endIndex < 0)) {
		*resultCount = 0;
		return Ok;
	}

	for (i = startIndex, j = 0; i <= endIndex; i++, j++) {
		points [j] = g_array_index (iterator->path->points, GpPointF, i);
		types [j] = g_array_index (iterator->path->types, BYTE, i);
	}

	*resultCount = j;

	return Ok;
}

GpStatus
GdipPathIterEnumerate (GpPathIterator *iterator, int *resultCount, GpPointF *points, BYTE *types, int count)
{
	int i = 0;

	if (!iterator || !resultCount || !points || !types)
		return InvalidParameter;

	if (iterator->path) {
		for (; i < count && i < iterator->path->count; i++) {
			points [i] = g_array_index (iterator->path->points, GpPointF, i);
			types [i] = g_array_index (iterator->path->types, BYTE, i);
		}
	}

	*resultCount = i;

	return Ok;
}

GpStatus
GdipPathIterHasCurve (GpPathIterator *iterator, BOOL *curve)
{
	if (!iterator || !curve)
		return InvalidParameter;

	*curve = gdip_path_has_curve (iterator->path);

	return Ok;
}

GpStatus
GdipPathIterNextMarkerPath (GpPathIterator *iterator, int *resultCount, GpPath *path)
{
	int index = 0;
	BYTE type;
	GpPointF point;

	if (!iterator || !resultCount)
		return InvalidParameter;

	/* There are no paths or markers or we are done with all the markers */
	if (!path || !iterator->path || (iterator->path->count == 0) ||
	    (iterator->markerPosition == iterator->path->count)) {
		*resultCount = 0;
		return Ok;
	}

	/* Clear the existing values from path */
	if (path->count > 0) {
		g_array_free (path->points, TRUE);
		g_byte_array_free (path->types, TRUE);

		path->points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
		path->types = g_byte_array_new ();
		path->count = 0;
	}

	for (index = iterator->markerPosition; index < iterator->path->count; index++) {
		type = g_array_index (iterator->path->types, BYTE, index);
		point = g_array_index (iterator->path->points, GpPointF, index);
		g_array_append_val (path->points, point);
		g_byte_array_append (path->types, &type, 1);
		path->count++;

		/* Copy the marker and stop copying the points when we reach a marker type */
		if (type & PathPointTypePathMarker) {
			index++;
			break;
		}
	}

	*resultCount = index - iterator->markerPosition;
	iterator->markerPosition = index;

	return Ok;
}

GpStatus
GdipPathIterNextMarker (GpPathIterator *iterator, int *resultCount, int *startIndex, int *endIndex)
{
	int index = 0;
	BYTE type;

	if (!iterator || !resultCount || !startIndex || !endIndex)
		return InvalidParameter;

	/* There are no markers or we are done with all the markers */
	if (!iterator->path || (iterator->path->count == 0) ||
	    (iterator->markerPosition == iterator->path->count)) {
		/* we don't touch startIndex and endIndex in this case */
		*resultCount = 0;
		return Ok;
	}
	
	/* Check for next marker */
	for (index = iterator->markerPosition; index < iterator->path->count; index++) {
		type = g_array_index (iterator->path->types, BYTE, index);
		if (type & PathPointTypePathMarker) {
			index++;
			break;
		}
	}

	*startIndex = iterator->markerPosition;
	*endIndex = index - 1;
	*resultCount =  (*endIndex) - (*startIndex) + 1;;

	iterator->markerPosition = index;

	return Ok;
}

GpStatus
GdipPathIterNextPathType (GpPathIterator *iterator, int *resultCount, BYTE *pathType, int *startIndex, int *endIndex)
{
	int index;
	BYTE currentType;
	BYTE lastTypeSeen;

	if (!iterator || !resultCount || !pathType || !startIndex || !endIndex)
		return InvalidParameter;

	/* There are no subpaths or we are done with all the subpaths */
	if (!iterator->path || (iterator->path->count == 0) || (iterator->subpathPosition == 0)) {
		/* we don't touch pathType, startIndex and endIndex in this case */
		*resultCount = 0;
		return Ok;
	}

	/* Pathtype position lags behind subpath position */
	else if (iterator->pathTypePosition < iterator->subpathPosition) {
		lastTypeSeen = g_array_index (iterator->path->types, BYTE, iterator->pathTypePosition + 1);
		/* Mask the flags */
		lastTypeSeen &= PathPointTypePathTypeMask;

		/* Check for the change in type */
		for (index = iterator->pathTypePosition + 2; index < iterator->subpathPosition; index++) {
			currentType = g_array_index (iterator->path->types, BYTE, index);
			currentType &= PathPointTypePathTypeMask;

			if (currentType != lastTypeSeen)
				break;
		}

		*startIndex = iterator->pathTypePosition;
		*endIndex = index - 1;
		*resultCount = (*endIndex) - (*startIndex) + 1;
		*pathType = lastTypeSeen;

		/* If lastTypeSeen is a line, it becomes the starting point for the next
		 * path type. We get this when we have connected figures. We need to step
		 * back in that case. We don't need to step back if we are finished with
		 * current subpath.
		 */
		if ((lastTypeSeen == PathPointTypeLine) && (index != iterator->subpathPosition))
			iterator->pathTypePosition = index - 1;
		else
			iterator->pathTypePosition = index;
	}

	/* If pathtype position and subpath position coincide we return the resultCount = 0 */
	else
		*resultCount = 0;

	return Ok;
}

GpStatus
GdipPathIterNextSubpathPath (GpPathIterator *iterator, int *resultCount, GpPath *path, BOOL *isClosed)
{
	int index = 0;
	GpPointF point;
	BYTE currentType;

	if (!iterator || !resultCount || !isClosed)
		return InvalidParameter;

	/* There are no subpaths or we are done with all the subpaths */
	if (!path || !iterator->path || (iterator->path->count == 0) || 
	    (iterator->subpathPosition == iterator->path->count)) {
		*resultCount = 0;
		*isClosed = TRUE;
		return Ok;
	}

	/* Clear the existing values from path */
	if (path->count > 0) {
		g_array_free (path->points, TRUE);
		g_byte_array_free (path->types, TRUE);

		path->points = g_array_new (FALSE, FALSE, sizeof (GpPointF));
		path->types = g_byte_array_new ();
		path->count = 0;
	}

	/* Copy the starting point */
	currentType = g_array_index (iterator->path->types, BYTE, iterator->subpathPosition);
	point = g_array_index (iterator->path->points, GpPointF, iterator->subpathPosition);
	g_array_append_val (path->points, point);
	g_byte_array_append (path->types, &currentType, 1);
	path->count++;

	/* Check for next start point */
	for (index = iterator->subpathPosition + 1; index < iterator->path->count; index++) {
		currentType = g_array_index (iterator->path->types, BYTE, index);

		/* Copy the start point till next start point */
		if (currentType == PathPointTypeStart)
			break;

		point = g_array_index (iterator->path->points, GpPointF, index);
		g_array_append_val (path->points, point);
		g_byte_array_append (path->types, &currentType, 1);
		path->count++;
	}

	*resultCount = index - iterator->subpathPosition;
	/* set positions for next iteration */
	iterator->pathTypePosition = iterator->subpathPosition;
	iterator->subpathPosition = index;

	/* Check if last subpath was closed */
	currentType = g_array_index (iterator->path->types, BYTE, index - 1);
	if (currentType & PathPointTypeCloseSubpath)
		*isClosed = TRUE;
	else
		*isClosed = FALSE;

	return Ok;
}

GpStatus
GdipPathIterNextSubpath (GpPathIterator *iterator, int *resultCount, int *startIndex, int *endIndex, BOOL *isClosed)
{
	int index = 0;
	BYTE currentType;

	if (!iterator || !resultCount || !startIndex || !endIndex || !isClosed)
		return InvalidParameter;

	/* There are no subpaths or we are done with all the subpaths */
	if (!iterator->path || (iterator->path->count == 0) || 
	    (iterator->subpathPosition == iterator->path->count)) {
		/* we don't touch startIndex and endIndex in this case */
		*resultCount = 0;
		*isClosed = TRUE;
		return Ok;
	}

	/* Check for next start point */
	for (index = iterator->subpathPosition + 1; index < iterator->path->count; index++) {
		currentType = g_array_index (iterator->path->types, BYTE, index);
		if (currentType == PathPointTypeStart)
			break;
	}

	*startIndex = iterator->subpathPosition;
	*endIndex = index - 1;
	*resultCount = (*endIndex) - (*startIndex) + 1;
	/* set positions for next iteration */
	iterator->pathTypePosition = iterator->subpathPosition;
	iterator->subpathPosition = index;

	/* check if last subpath was closed */
	currentType = g_array_index (iterator->path->types, BYTE, index - 1);
	if (currentType & PathPointTypeCloseSubpath)
		*isClosed = TRUE;
	else
		*isClosed = FALSE;

	return Ok;
}

GpStatus
GdipPathIterRewind (GpPathIterator *iterator)
{
	if (!iterator)
		return InvalidParameter;

	/* Reset the marker and subpath positions */
	iterator->markerPosition = 0;
	iterator->subpathPosition = 0;
	iterator->pathTypePosition = 0;

	return Ok;
}
