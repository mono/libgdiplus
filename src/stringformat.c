/*
 * Copyright (c) 2004 Ximian
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
 * Author:
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004
 *
 */

#include "gdip.h"
#include "gdip_win32.h"

GpStatus
GdipCreateStringFormat(int formatAttributes, int language, GpStringFormat  **format)
{
        if (!format)
                return InvalidParameter;
	
        GpStringFormat *result = (GpStringFormat *) GdipAlloc (sizeof (GpStringFormat));
        result->alignment = StringAlignmentNear;
        result->lineAlignment =  StringAlignmentNear;
        result->hotkeyPrefix = HotkeyPrefixNone;
        result->formatFlags = formatAttributes; 
        result->trimming = StringTrimmingNone;
        result->substitute = DigitSubstituteNone;
        result->firstTabOffset = 0;
        result->tabStops = NULL;
        result->numtabStops = 0;

        *format = result;
        return Ok;
}

GpStatus
GdipStringFormatGetGenericDefault(GpStringFormat **format)
{
        if (!format)
                return InvalidParameter;

        GpStringFormat *result = (GpStringFormat *) GdipAlloc (sizeof (GpStringFormat));
        result->alignment = StringAlignmentNear;
        result->lineAlignment =  StringAlignmentNear;
        result->hotkeyPrefix = HotkeyPrefixNone;
        result->formatFlags = 0;
        result->trimming = StringTrimmingCharacter;
        result->substitute = DigitSubstituteNone;
        result->firstTabOffset = 0;
        result->tabStops = NULL;
        result->numtabStops = 0;

        *format = result;
        return Ok;
}             

GpStatus
GdipStringFormatGetGenericTypographic(GpStringFormat **format)
{
        if (!format)
                return InvalidParameter;

        GpStringFormat *result = (GpStringFormat *) GdipAlloc (sizeof (GpStringFormat));
        result->alignment = StringAlignmentNear;
        result->lineAlignment =  StringAlignmentNear;
        result->hotkeyPrefix = HotkeyPrefixNone;
        result->formatFlags = StringFormatFlagsNoFitBlackBox | StringFormatFlagsLineLimit | StringFormatFlagsNoClip;
        result->trimming = StringTrimmingNone;
        result->substitute = DigitSubstituteNone;
        result->firstTabOffset = 0;
        result->tabStops = NULL;
        result->numtabStops = 0;

        *format = result;
        return Ok; 
}

GpStatus
GdipCloneStringFormat(GDIPCONST GpStringFormat *format,  GpStringFormat **newFormat)
{
        if (!format|| !newFormat)
                return InvalidParameter;

        GpStringFormat *result = (GpStringFormat *) GdipAlloc (sizeof (GpStringFormat));

        memcpy (result, format, sizeof (GpStringFormat));
    
        *newFormat = result;
        return Ok; 
}


GpStatus
GdipDeleteStringFormat(GpStringFormat *format)
{
        if (!format)
                return InvalidParameter;
                
        if (format->tabStops)
                free (format->tabStops);

        GdipFree (format);
        return Ok;
}

GpStatus
GdipSetStringFormatAlign(GpStringFormat *format, StringAlignment align)
{
        if (!format)
                return InvalidParameter;

        format->alignment = align;    
        return Ok;
}

GpStatus
GdipGetStringFormatAlign(GDIPCONST GpStringFormat *format, StringAlignment *align)
{
        if (!format)
                return InvalidParameter;

        *align = format->alignment;    
        return Ok;
}

GpStatus
GdipSetStringFormatLineAlign(GpStringFormat *format, StringAlignment align)
{
        if (!format)
                return InvalidParameter;

        format->lineAlignment = align;    
        return Ok;
}

GpStatus
GdipGetStringFormatLineAlign(GDIPCONST GpStringFormat *format, StringAlignment *align)
{
        if (!format)
                return InvalidParameter;

        *align = format->lineAlignment;    
        return Ok;
}

GpStatus
GdipSetStringFormatHotkeyPrefix(GpStringFormat *format, HotkeyPrefix hotkeyPrefix)
{
        if (!format)
                return InvalidParameter;

        format->hotkeyPrefix = hotkeyPrefix;
        return Ok;
}

GpStatus
GdipGetStringFormatHotkeyPrefix(GpStringFormat *format, HotkeyPrefix *hotkeyPrefix)
{
        if (!format)
                return InvalidParameter;

        *hotkeyPrefix = format->hotkeyPrefix;
        return Ok;
}

GpStatus
GdipSetStringFormatFlags(GpStringFormat *format, StringFormatFlags flags)
{
        if (!format)
                return InvalidParameter;

        format->formatFlags = flags; 
        return Ok;    
}

GpStatus
GdipGetStringFormatFlags(GDIPCONST GpStringFormat *format, StringFormatFlags *flags)
{
        if (!format)
                return InvalidParameter;

        *flags = format->formatFlags;
        return Ok;
}


GpStatus
GdipSetStringFormatTrimming(GpStringFormat *format,  StringTrimming trimming)
{
        if (!format)
                return InvalidParameter;

        format->trimming = trimming;
        return Ok;
}

GpStatus
GdipGetStringFormatTrimming(GDIPCONST GpStringFormat *format, StringTrimming *trimming)
{
        if (!format)
                return InvalidParameter;

        *trimming = format->trimming;
        return Ok;
}

GpStatus
GdipSetStringFormatTabStops(GpStringFormat *format, float firstTabOffset, int count, float *tabStops)
{
        int i;
        float *pItemSrc = tabStops;
        float *pItemTrg;

        if (!format || !tabStops)
                return InvalidParameter;

        if (format->tabStops)
                free (format->tabStops);

        format->firstTabOffset = firstTabOffset;

        if (count == 0) {
                format->tabStops = NULL;
                format->numtabStops = 0;
                return Ok;
        }

        pItemTrg = format->tabStops = malloc (sizeof(float) * count);

        if (!pItemTrg)
                return OutOfMemory;

        for (i = 0; i < count; i++, pItemSrc++, pItemTrg++)
                *pItemTrg = *pItemSrc;
                
        format->numtabStops = count;    
        return Ok;        
}

GpStatus
GdipGetStringFormatDigitSubstitution(GDIPCONST GpStringFormat *format, int *language, DigitSubstitute *substitute)
{
        if (!format)
                return InvalidParameter;

        *substitute = format->substitute;
        return Ok;                
}

GpStatus
GdipSetStringFormatDigitSubstitution(GpStringFormat *format, int language, DigitSubstitute substitute)
{
        if (!format)
                return InvalidParameter;
                
        format->substitute = substitute;
        return Ok;                
}

GpStatus
GdipGetStringFormatTabStopCount(GDIPCONST GpStringFormat *format, int *count)
{
        if (!format || !count)
                return InvalidParameter;

        *count = format->numtabStops;

        return Ok;                
}

GpStatus
GdipGetStringFormatTabStops(GDIPCONST GpStringFormat *format, int count, float *firstTabOffset, float *tabStops)
{
        int i;
        float *pItemSrc = format->tabStops;
        float *pItemTrg = tabStops;
        int elems;
        
        if (!format || !firstTabOffset || !tabStops)
                return InvalidParameter;

        elems = count<format->numtabStops ? count : format->numtabStops;        
        for (i = 0; i < elems; i++, pItemSrc++, pItemTrg++)
                *pItemTrg = *pItemSrc;
                
        *firstTabOffset = format->firstTabOffset;
                
        return Ok;                    
}
