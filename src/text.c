/*
 * text.c
 *
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

#include "gdiplus-private.h"

#ifdef USE_PANGO_RENDERING
	#include "text-pango-private.h"
#else
	#include "text-cairo-private.h"
#endif

#include "text-metafile-private.h"

/*
 * Text API - validate and delegate
 */

GpStatus WINGDIPAPI
GdipDrawString (GpGraphics *graphics, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFont *font, GDIPCONST RectF *layoutRect,
	GDIPCONST GpStringFormat *stringFormat, GpBrush *brush)
{
	GDIPCONST WCHAR *ptr = NULL;

	if (length == 0) {
		return Ok;
	} else if (length == -1) {
		ptr = string;
		length = 0;
		while (*ptr != 0) {
			length++;
			ptr++;
		}
	}

	if (!graphics || !string || !font || !layoutRect)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return text_DrawString (graphics, string, length, font, layoutRect, stringFormat, brush);
	case GraphicsBackEndMetafile:
		return metafile_DrawString (graphics, string, length, font, layoutRect, stringFormat, brush);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipMeasureString (GpGraphics *graphics, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFont *font, GDIPCONST RectF *layoutRect,
	GDIPCONST GpStringFormat *stringFormat, RectF *boundingBox, INT *codepointsFitted, INT *linesFilled)
{
	GDIPCONST WCHAR *ptr = NULL;

	if (length == 0) {
		if (boundingBox) {
			if (layoutRect) {
				boundingBox->X = layoutRect->X;
				boundingBox->Y = layoutRect->Y;
			} else {
				boundingBox->X = 0;
				boundingBox->Y = 0;
			}
			boundingBox->Width = 0;
			boundingBox->Height = 0;
		}
		if (linesFilled) {
			*linesFilled = 0;
		}
		if (codepointsFitted) {
			*codepointsFitted = 0;
		}
		return Ok;
	} else if (length == -1) {
		ptr = string;
		length = 0;
		while (*ptr != 0) {
			length++;
			ptr++;
		}
	}

	if (!graphics || !string || !font || !layoutRect)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
	/* a metafile-based graphics returns the correct measures but doesn't record anything */
	case GraphicsBackEndMetafile:
		return text_MeasureString (graphics, string, length, font, layoutRect, stringFormat, boundingBox,
			codepointsFitted, linesFilled);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipMeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *string, INT length, GDIPCONST GpFont *font,
	GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *stringFormat, INT regionCount, GpRegion **regions)
{
	/* note: a NULL format is invalid */
	if (!graphics || !string || (length == 0) || !font || !layoutRect || !stringFormat || !regions)
		return InvalidParameter;

	/* No char range or bounding rect is set for measurements */
	if (stringFormat->charRangeCount == 0) {
		*regions = NULL;
		return Ok;
	}

	/* if non-zero (previous check) format->charRangeCount must match regionCount */
	if (regionCount != stringFormat->charRangeCount)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
	/* a metafile-based graphics returns the correct measures but doesn't record anything */
	case GraphicsBackEndMetafile:
		return text_MeasureCharacterRanges (graphics, string, length, font, layoutRect, stringFormat, regionCount,
			regions);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipDrawDriverString (GpGraphics *graphics, GDIPCONST UINT16 *text, INT length, GDIPCONST GpFont *font,
	GDIPCONST GpBrush *brush, GDIPCONST PointF *positions, INT flags, GDIPCONST GpMatrix *matrix)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipMeasureDriverString (GpGraphics *graphics, GDIPCONST UINT16 *text, INT length, GDIPCONST GpFont *font,
	GDIPCONST PointF *positions, INT flags, GDIPCONST GpMatrix *matrix, RectF *boundingBox)
{
	return NotImplemented;
}