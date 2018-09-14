/*
 * Copyright (C) 2004 Ximian inc. http://www.ximian.com
 * Copyright (C) 2004,2006-2007 Novell Inc. http://www.novell.com
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
 *	  Jordi Mas i Hernandez <jordi@ximian.com>, 2004
 *	  Ravindra <rkumar@novell.com>
 *	  Sebastien Pouliot  <sebastien@ximian.com>
 *
 */

#include "stringformat-private.h"
#include "general-private.h"

static GpStringFormat stringFormatDefault;
static GpStringFormat stringFormatTypographic;

static void
gdip_string_format_clear (GpStringFormat *format)
{
	if (format->tabStops) {
		GdipFree (format->tabStops);
		format->tabStops = NULL;
	}

	if (format->charRanges) {
		GdipFree (format->charRanges);
		format->charRanges = NULL;
	}
}

static void
gdip_string_format_init (GpStringFormat *result)
{
	result->alignment = StringAlignmentNear;
	result->lineAlignment =  StringAlignmentNear;
	result->hotkeyPrefix = HotkeyPrefixNone;
	result->formatFlags = 0;
	result->trimming = StringTrimmingCharacter;
	result->substitute = StringDigitSubstituteUser;
	result->language = 0;
	result->firstTabOffset = 0;
	result->tabStops = NULL;
	result->numtabStops = 0;
	result->charRanges = NULL;
	result->charRangeCount = 0;
}

void
gdip_create_generic_stringformats ()
{
	gdip_string_format_init (&stringFormatDefault);

	gdip_string_format_init (&stringFormatTypographic);
	stringFormatTypographic.formatFlags = StringFormatFlagsNoFitBlackBox | StringFormatFlagsLineLimit | StringFormatFlagsNoClip;
	stringFormatTypographic.trimming = StringTrimmingNone;
}

void
gdip_delete_generic_stringformats ()
{
	gdip_string_format_clear (&stringFormatDefault);
	gdip_string_format_clear (&stringFormatTypographic);
}

static GpStringFormat *
gdip_string_format_new ()
{
	GpStringFormat *result = GdipAlloc (sizeof (GpStringFormat));
	if (result)
		gdip_string_format_init (result);

	return result;
}

/* coverity[+alloc : arg-*2] */
GpStatus WINGDIPAPI
GdipCreateStringFormat (INT formatAttributes, LANGID language, GpStringFormat **format)
{
	GpStringFormat *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!format)
		return InvalidParameter;

	result = gdip_string_format_new ();
	if (!result) {
		*format = NULL;
		return OutOfMemory;
	}

	result->formatFlags = formatAttributes;

	*format = result;
	return Ok;
}

/* coverity[+alloc : arg-*0] */
GpStatus WINGDIPAPI
GdipStringFormatGetGenericDefault (GpStringFormat **format)
{
	if (!format)
		return InvalidParameter;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	*format = &stringFormatDefault;
	return Ok;
}

/* coverity[+alloc : arg-*0] */
GpStatus WINGDIPAPI
GdipStringFormatGetGenericTypographic (GpStringFormat **format)
{
	if (!format)
		return InvalidParameter;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	*format = &stringFormatTypographic;
	return Ok;
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI
GdipCloneStringFormat (GDIPCONST GpStringFormat *format,  GpStringFormat **newFormat)
{
	GpStringFormat *result;

	if (!format || !newFormat)
		return InvalidParameter;

	result = gdip_string_format_new ();
	if (!result)
		goto error;

	result->alignment = format->alignment;
	result->lineAlignment =  format->lineAlignment;
	result->hotkeyPrefix = format->hotkeyPrefix;
	result->formatFlags = format->formatFlags;
	result->trimming = format->trimming;
	result->substitute = format->substitute;
	result->language = format->language;
	result->firstTabOffset = format->firstTabOffset;
	result->numtabStops = format->numtabStops;
	result->charRangeCount = format->charRangeCount;

	/* Create a copy of tab stops for the clone */
	result->tabStops = (float *) GdipAlloc (sizeof (float) * format->numtabStops);
	if (!result->tabStops)
		goto error;

	memcpy (result->tabStops, format->tabStops, format->numtabStops * sizeof (float));

	/* Create a copy of char ranges for the clone */
	result->charRanges = (CharacterRange *) GdipAlloc (format->charRangeCount * sizeof (CharacterRange));
	if (result->charRanges == NULL)
		goto error;

	memcpy (result->charRanges, format->charRanges, format->charRangeCount * sizeof (CharacterRange));

	*newFormat = result;
	return Ok;

error:
	if (result)
		GdipDeleteStringFormat (result);

	*newFormat = NULL;
	return OutOfMemory;
}

GpStatus WINGDIPAPI
GdipDeleteStringFormat (GpStringFormat *format)
{
	if (!format)
		return InvalidParameter;

	// These are singletons cleared on shutdown.
	if (format == &stringFormatDefault || format == &stringFormatTypographic)
		return Ok;

	gdip_string_format_clear (format);

	GdipFree (format);
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatAlign (GpStringFormat *format, StringAlignment align)
{
	if (!format || align < StringAlignmentNear || align > StringAlignmentFar)
		return InvalidParameter;

	format->alignment = align;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatAlign (GDIPCONST GpStringFormat *format, StringAlignment *align)
{
	if (!format || !align)
		return InvalidParameter;

	*align = format->alignment;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatLineAlign (GpStringFormat *format, StringAlignment align)
{
	if (!format || align < StringAlignmentNear || align > StringAlignmentFar)
		return InvalidParameter;

	format->lineAlignment = align;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatLineAlign (GDIPCONST GpStringFormat *format, StringAlignment *align)
{
	if (!format || !align)
		return InvalidParameter;

	*align = format->lineAlignment;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatHotkeyPrefix (GpStringFormat *format, INT hotkeyPrefix)
{
	if (!format || hotkeyPrefix < HotkeyPrefixNone || hotkeyPrefix > HotkeyPrefixHide)
		return InvalidParameter;

	format->hotkeyPrefix = hotkeyPrefix;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatHotkeyPrefix (GDIPCONST GpStringFormat *format, INT *hotkeyPrefix)
{
	if (!format || !hotkeyPrefix)
		return InvalidParameter;

	*hotkeyPrefix = format->hotkeyPrefix;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatFlags (GpStringFormat *format, INT flags)
{
	if (!format)
		return InvalidParameter;

	format->formatFlags = flags;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatFlags (GDIPCONST GpStringFormat *format, INT *flags)
{
	if (!format || !flags)
		return InvalidParameter;

	*flags = format->formatFlags;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatTrimming (GpStringFormat *format,  StringTrimming trimming)
{
	if (!format || trimming < StringTrimmingNone || trimming > StringTrimmingEllipsisPath)
		return InvalidParameter;

	format->trimming = trimming;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatTrimming (GDIPCONST GpStringFormat *format, StringTrimming *trimming)
{
	if (!format || !trimming)
		return InvalidParameter;

	*trimming = format->trimming;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatTabStops (GpStringFormat *format, REAL firstTabOffset, INT count, GDIPCONST REAL *tabStops)
{
	REAL *tabStopsResult;

	if (!format || !tabStops)
		return InvalidParameter;

	if (count <= 0)
		return Ok;

	if (firstTabOffset < 0)
		return NotImplemented;

	for (int i = 0; i < count; i++) {
		if (tabStops[i] < 0)
			return NotImplemented;
	}

	/* Avoid allocating memory if the count is the same. */
	if (count != format->numtabStops) {
		tabStopsResult = GdipAlloc (count * sizeof (REAL));
		if (!tabStopsResult)
			return OutOfMemory;

		if (format->tabStops)
			GdipFree (format->tabStops);

		format->tabStops = tabStopsResult;
	}

	format->firstTabOffset = firstTabOffset;
	format->numtabStops = count;
	memcpy (format->tabStops, tabStops, count * sizeof (REAL));

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatDigitSubstitution (GDIPCONST GpStringFormat *format, LANGID *language, StringDigitSubstitute *substitute)
{
	if (!format)
		return InvalidParameter;

	if (language)
		*language = format->language;

	if (substitute)
		*substitute = format->substitute;

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatDigitSubstitution (GpStringFormat *format, LANGID language, StringDigitSubstitute substitute)
{
	if (!format)
		return InvalidParameter;

	format->language = language;
	format->substitute = substitute;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatTabStopCount (GDIPCONST GpStringFormat *format, INT *count)
{
	if (!format || !count)
		return InvalidParameter;

	*count = format->numtabStops;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatTabStops (GDIPCONST GpStringFormat *format, INT count, REAL *firstTabOffset, REAL *tabStops)
{
	int elems;

	if (!format || !firstTabOffset || !tabStops || count < 0)
		return InvalidParameter;

	elems = count<format->numtabStops ? count : format->numtabStops;

	*firstTabOffset = format->firstTabOffset;
	memcpy (tabStops, format->tabStops, elems * sizeof (REAL));

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetStringFormatMeasurableCharacterRangeCount (GDIPCONST GpStringFormat *format, INT *count)
{
	if (! format || !count)
		return InvalidParameter;

	*count = format->charRangeCount;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetStringFormatMeasurableCharacterRanges (GpStringFormat *format, INT rangeCount, GDIPCONST CharacterRange *ranges)
{
	CharacterRange *rangesResult = NULL;

	if (!format || !ranges)
		return InvalidParameter;

	if (rangeCount > 32)
		return ValueOverflow;

	if (rangeCount > 0) {
		if (format->charRangeCount == rangeCount) {
			/* Avoid allocating memory if the count is the same. */
			rangesResult = format->charRanges;
		}
		else {
			rangesResult = (CharacterRange *) GdipAlloc (rangeCount * sizeof (CharacterRange));
			if (!rangesResult)
				return OutOfMemory;
		}

		memcpy (rangesResult, ranges, rangeCount * sizeof (CharacterRange));
	}
	else {
		rangesResult = NULL;
		rangeCount = 0;
	}

	if (format->charRanges && format->charRangeCount != rangeCount) {
		GdipFree (format->charRanges);
		format->charRanges = NULL;
	}

	format->charRanges = rangesResult;
	format->charRangeCount = rangeCount;

	return Ok;
}
