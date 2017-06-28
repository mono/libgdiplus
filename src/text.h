/*
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
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
 *   Alexandre Pigolkine (pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef __TEXT_H__
#define __TEXT_H__

/* Text public API (only!) */

GpStatus WINGDIPAPI GdipDrawString (GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font,
	GDIPCONST RectF *layoutRect, GDIPCONST GpStringFormat *stringFormat, GpBrush *brush);

GpStatus WINGDIPAPI GdipMeasureString (GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font,
	GDIPCONST RectF *layoutRect, GDIPCONST GpStringFormat *stringFormat,  RectF *boundingBox, int *codepointsFitted,
	int *linesFilled);

GpStatus WINGDIPAPI GdipMeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font,
	GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *stringFormat, int regionCount, GpRegion **regions);

/* missing API
	GdipDrawDriverString
	GdipMeasureDriverString
 */

#endif
