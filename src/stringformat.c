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
        result->formatFlags = 0;
        result->trimming = StringTrimmingNone;
    
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

        GdipFree (format);
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

