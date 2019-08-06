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
 *      Duncan Mak (duncan@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003, 2006-2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __GRAPHICS_PATHITERATOR_H__
#define __GRAPHICS_PATHITERATOR_H__

GpStatus WINGDIPAPI GdipCreatePathIter (GpPathIterator **iterator, GpPath *path);
GpStatus WINGDIPAPI GdipDeletePathIter (GpPathIterator *iterator);

GpStatus WINGDIPAPI GdipPathIterGetCount (GpPathIterator *iterator, INT *count);
GpStatus WINGDIPAPI GdipPathIterGetSubpathCount (GpPathIterator *iterator, INT *count);
GpStatus WINGDIPAPI GdipPathIterCopyData (GpPathIterator *iterator, INT *resultCount, GpPointF *points, BYTE *types, INT startIndex, INT endIndex);
GpStatus WINGDIPAPI GdipPathIterHasCurve (GpPathIterator *iterator, BOOL *hasCurve);

GpStatus WINGDIPAPI GdipPathIterNextMarkerPath (GpPathIterator *iterator, INT *resultCount, GpPath *path);
GpStatus WINGDIPAPI GdipPathIterNextMarker (GpPathIterator *iterator, INT *resultCount, INT *startIndex, INT *endIndex);
GpStatus WINGDIPAPI GdipPathIterNextPathType (GpPathIterator *iterator, INT *resultCount, BYTE *pathType, INT *startIndex, INT *endIndex);
GpStatus WINGDIPAPI GdipPathIterNextSubpathPath (GpPathIterator *iterator, INT *resultCount, GpPath *path, BOOL *isClosed);
GpStatus WINGDIPAPI GdipPathIterNextSubpath (GpPathIterator *iterator, INT *resultCount, INT *startIndex, INT *endIndex, BOOL *isClosed);

GpStatus WINGDIPAPI GdipPathIterEnumerate (GpPathIterator *iterator, INT *resultCount, GpPointF *points, BYTE *types, INT count);
GpStatus WINGDIPAPI GdipPathIterRewind (GpPathIterator *iterator);


/* missing API
	GdipPathIterIsValid
*/

#endif
