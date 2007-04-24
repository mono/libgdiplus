#ifndef __TEXT_H__
#define __TEXT_H__

/* Text public API (only!) */

GpStatus GdipDrawString (GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font, 
	GDIPCONST RectF *layoutRect, GDIPCONST GpStringFormat *stringFormat, GpBrush *brush);

GpStatus GdipMeasureString (GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font, 
	GDIPCONST RectF *layoutRect, GDIPCONST GpStringFormat *stringFormat,  RectF *boundingBox, int *codepointsFitted,
	int *linesFilled);

GpStatus GdipMeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font, 
	GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *stringFormat, int regionCount, GpRegion **regions);

/* missing API
	GdipDrawDriverString
	GdipMeasureDriverString
 */

#endif
