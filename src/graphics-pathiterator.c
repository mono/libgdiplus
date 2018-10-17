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
#include "general-private.h"
#include "graphics-path-private.h"
#include "font.h"

// coverity[+alloc : arg-*0]
GpStatus
GdipCreatePathIter (GpPathIterator **iterator, GpPath *path)
{
	GpPath *clone = NULL;
	GpPathIterator *iter;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

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
			type = iterator->path->types[i];
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
	if (!iterator || !resultCount || !points || !types)
		return InvalidParameter;

	if (!iterator->path || (startIndex >= iterator->path->count) || (startIndex > endIndex) || 
		(endIndex >= iterator->path->count) || (startIndex < 0) || (endIndex < 0)) {
		*resultCount = 0;
		return Ok;
	}

	memcpy (points, iterator->path->points + startIndex, (endIndex - startIndex + 1) * sizeof (GpPointF));
	memcpy (types, iterator->path->types + startIndex, (endIndex - startIndex + 1) * sizeof (BYTE));
	*resultCount = endIndex - startIndex + 1;

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
			points [i] = iterator->path->points[i];
			types [i] = iterator->path->types[i];
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
	int start, end;
	GpStatus status;

	status = GdipPathIterNextMarker (iterator, resultCount, &start, &end);
	if (status == Ok && *resultCount > 0) {
		GdipResetPath (path);
		gdip_path_ensure_size (path, *resultCount);
		memcpy (path->types, iterator->path->types + start, sizeof (BYTE) * *resultCount);
		memcpy (path->points, iterator->path->points + start, sizeof (GpPointF) * *resultCount);
		path->count = *resultCount;
	}

	return status;
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
	    (iterator->markerPosition >= iterator->path->count)) {
		/* we don't touch startIndex and endIndex in this case */
		*resultCount = 0;
		return Ok;
	}
	
	/* Check for next marker */
	for (index = iterator->markerPosition; index < iterator->path->count; index++) {
		type = iterator->path->types[index];
		if (type & PathPointTypePathMarker) {
			index++;
			break;
		}
	}

	*startIndex = iterator->markerPosition;
	*endIndex = index - 1;
	*resultCount =  (*endIndex) - (*startIndex) + 1;

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
		lastTypeSeen = iterator->path->types[iterator->pathTypePosition + 1];
		/* Mask the flags */
		lastTypeSeen &= PathPointTypePathTypeMask;

		/* Check for the change in type */
		for (index = iterator->pathTypePosition + 2; index < iterator->subpathPosition; index++) {
			currentType = iterator->path->types[index];
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
	int start, end;
	GpStatus status;

	status = GdipPathIterNextSubpath (iterator, resultCount, &start, &end, isClosed);
	if (status == Ok && *resultCount > 0) {
		GdipResetPath (path);
		gdip_path_ensure_size (path, *resultCount);
		memcpy (path->types, iterator->path->types + start, sizeof (BYTE) * *resultCount);
		memcpy (path->points, iterator->path->points + start, sizeof (GpPointF) * *resultCount);
		path->count = *resultCount;
	}

	return status;
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
		currentType = iterator->path->types[index];
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
	currentType = iterator->path->types[index - 1];
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
