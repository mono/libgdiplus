/*
 * graphics-pathiterator.c
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

GpStatus
GdipCreatePathIter (GpPathIterator **iterator, GpPath *path)
{
	GpPath *clone;
	GpPathIterator *iter;

	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (iterator != NULL, InvalidParameter);

	iter = (GpPathIterator *) GdipAlloc (sizeof (GpPathIterator));
	if (iter == NULL)
		return OutOfMemory;

	GdipClonePath (path, &clone);
	if (clone == NULL) {
		GdipFree (iter);
		return OutOfMemory;
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

	/* Number of subpaths = Number of starting points */
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

	GdipDeletePath (iterator->path);

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
	    startIndex > endIndex ||
	    endIndex >= iterator->path->count) {
		*resultCount = 0;
		return Ok;
	}

	for (i = startIndex, j = 0; i <= endIndex; i++, j++) {
		points [j] = g_array_index (iterator->path->points, GpPointF, i);
		types [j] = g_array_index (iterator->path->types, byte, i);
	}

	*resultCount = j;

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

	/* We have a curve if any point type is bezier or bezier3 */
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
	int index = 0;
	byte type;
	GpPointF point;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);

	/* There are no markers or we are done with all the markers */
	if ((iterator->path->count == 0) ||
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
		type = g_array_index (iterator->path->types, byte, index);
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
	byte type;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);
	g_return_val_if_fail (startIndex != NULL, InvalidParameter);
	g_return_val_if_fail (endIndex != NULL, InvalidParameter);

	/* There are no markers or we are done with all the markers */
	if ((iterator->path->count == 0) ||
	    (iterator->markerPosition == iterator->path->count)) {
		*resultCount = 0;
		return Ok;
	}
	
	/* Check for next marker */
	for (index = iterator->markerPosition; index < iterator->path->count; index++) {
		type = g_array_index (iterator->path->types, byte, index);
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
GdipPathIterNextPathType (GpPathIterator *iterator, int *resultCount, byte *pathType, int *startIndex, int *endIndex)
{
	int index;
	byte currentType;
	byte lastTypeSeen;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (pathType != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);
	g_return_val_if_fail (startIndex != NULL, InvalidParameter);
	g_return_val_if_fail (endIndex != NULL, InvalidParameter);

	/* There are no subpaths or we are done with all the subpaths */
	if ((iterator->path->count == 0) || (iterator->subpathPosition == 0)) {
		*resultCount = 0;
		return Ok;
	}

	/* Pathtype position lags behind subpath position */
	else if (iterator->pathTypePosition < iterator->subpathPosition) {
		lastTypeSeen = g_array_index (iterator->path->types, byte, iterator->pathTypePosition + 1);
		/* Mask the flags */
		lastTypeSeen &= PathPointTypePathTypeMask;

		/* Check for the change in type */
		for (index = iterator->pathTypePosition + 2; index < iterator->subpathPosition; index++) {
			currentType = g_array_index (iterator->path->types, byte, index);
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
	byte currentType;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (path != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);
	g_return_val_if_fail (isClosed != NULL, InvalidParameter);

	/* There are no subpaths or we are done with all the subpaths */
	if ((iterator->path->count == 0) || 
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
	currentType = g_array_index (iterator->path->types, byte, iterator->subpathPosition);
	point = g_array_index (iterator->path->points, GpPointF, iterator->subpathPosition);
	g_array_append_val (path->points, point);
	g_byte_array_append (path->types, &currentType, 1);
	path->count++;

	/* Check for next start point */
	for (index = iterator->subpathPosition + 1; index < iterator->path->count; index++) {
		currentType = g_array_index (iterator->path->types, byte, index);

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
	currentType = g_array_index (iterator->path->types, byte, index - 1);
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
	byte currentType;

	g_return_val_if_fail (iterator != NULL, InvalidParameter);
	g_return_val_if_fail (resultCount != NULL, InvalidParameter);
	g_return_val_if_fail (startIndex != NULL, InvalidParameter);
	g_return_val_if_fail (endIndex != NULL, InvalidParameter);
	g_return_val_if_fail (isClosed != NULL, InvalidParameter);

	/* There are no subpaths or we are done with all the subpaths */
	if ((iterator->path->count == 0) || 
	    (iterator->subpathPosition == iterator->path->count)) {
		*resultCount = 0;
		*startIndex = 0;
		*endIndex = 0;
		*isClosed = TRUE;
		return Ok;
	}

	/* Check for next start point */
	for (index = iterator->subpathPosition + 1; index < iterator->path->count; index++) {
		currentType = g_array_index (iterator->path->types, byte, index);
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
	currentType = g_array_index (iterator->path->types, byte, index - 1);
	if (currentType & PathPointTypeCloseSubpath)
		*isClosed = TRUE;
	else
		*isClosed = FALSE;

	return Ok;
}

GpStatus
GdipPathIterRewind (GpPathIterator *iterator)
{
	g_return_val_if_fail (iterator != NULL, InvalidParameter);

	/* Reset the marker and subpath positions */
	iterator->markerPosition = 0;
	iterator->subpathPosition = 0;
	iterator->pathTypePosition = 0;

	return Ok;
}
