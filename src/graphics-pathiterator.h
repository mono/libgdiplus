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

GpStatus GdipCreatePathIter (GpPathIterator **iterator, GpPath *path);
GpStatus GdipDeletePathIter (GpPathIterator *iterator);

GpStatus GdipPathIterGetCount (GpPathIterator *iterator, INT *count);
GpStatus GdipPathIterGetSubpathCount (GpPathIterator *iterator, INT *count);
GpStatus GdipPathIterCopyData (GpPathIterator *iterator, INT *resultCount, GpPointF *points, BYTE *types, INT startIndex, INT endIndex);
GpStatus GdipPathIterHasCurve (GpPathIterator *iterator, BOOL *hasCurve);

GpStatus GdipPathIterNextMarkerPath (GpPathIterator *iterator, INT *resultCount, GpPath *path);
GpStatus GdipPathIterNextMarker (GpPathIterator *iterator, INT *resultCount, INT *startIndex, INT *endIndex);
GpStatus GdipPathIterNextPathType (GpPathIterator *iterator, INT *resultCount, BYTE *pathType, INT *startIndex, INT *endIndex);
GpStatus GdipPathIterNextSubpathPath (GpPathIterator *iterator, INT *resultCount, GpPath *path, BOOL *isClosed);
GpStatus GdipPathIterNextSubpath (GpPathIterator *iterator, INT *resultCount, INT *startIndex, INT *endIndex, BOOL *isClosed);

GpStatus GdipPathIterEnumerate (GpPathIterator *iterator, INT *resultCount, GpPointF *points, BYTE *types, INT count);
GpStatus GdipPathIterRewind (GpPathIterator *iterator);


/* missing API
	GdipPathIterIsValid
*/

#endif
