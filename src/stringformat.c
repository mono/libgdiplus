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
 * TODO:
 
 */

#include "gdip.h"
#include "gdip_win32.h"


void gdip_stringformat_init(GpStringFormat* format)
{
    format->alignment = StringAlignmentNear;
    format->lineAlignment =  StringAlignmentNear;
    format->hotkeyPrefix = HotkeyPrefixNone;    
}

GpStatus
GdipCreateStringFormat(int formatAttributes, int language, GpStringFormat  **format)
{
    GpStringFormat* result = (GpStringFormat *) GdipAlloc (sizeof (GpStringFormat));
    gdip_stringformat_init(result);
    
    *format = result;
    return Ok;    
}

GpStatus
GdipDeleteStringFormat(GpStringFormat *format)
{
    if (!format) return InvalidParameter;

    GdipFree (format);
}

GpStatus
GdipSetStringFormatAlign(GpStringFormat *format, StringAlignment align)
{
    if (!format) return InvalidParameter;

    format->alignment = align;    
    return Ok;
}

GpStatus
GdipGetStringFormatAlign(GDIPCONST GpStringFormat *format, StringAlignment *align)
{
    if (!format) return InvalidParameter;

    *align = format->alignment;
    
    return Ok;
}

GpStatus
GdipSetStringFormatLineAlign(GpStringFormat *format, StringAlignment align)
{
    if (!format) return InvalidParameter;

    format->lineAlignment = align;
    
    return Ok;
}

GpStatus
GdipGetStringFormatLineAlign(GDIPCONST GpStringFormat *format, StringAlignment *align)
{
    if (!format) return InvalidParameter;

    *align = format->lineAlignment;
    
    return Ok;
}

GpStatus
GdipSetStringFormatTrimming(GpStringFormat  *format, StringTrimming trimming)
{
     return Ok;
}

GpStatus
GdipGetStringFormatTrimming(GDIPCONST GpStringFormat *format, StringTrimming *trimming)
{
     return Ok;
}

GpStatus
GdipSetStringFormatHotkeyPrefix(GpStringFormat *format, HotkeyPrefix hotkeyPrefix)
{
    if (!format) return InvalidParameter;

    format->hotkeyPrefix = hotkeyPrefix;

    return Ok;
}

GpStatus
GdipGetStringFormatHotkeyPrefix(GpStringFormat *format, HotkeyPrefix *hotkeyPrefix)
{
    if (!format) return InvalidParameter;

    *hotkeyPrefix = format->hotkeyPrefix;

    return Ok;
}

GpStatus
GdipSetStringFormatFlags(GpStringFormat *format, StringFormatFlags flags)
{
    if (!format) return InvalidParameter;

    format->formatFlags = flags;

    return Ok;    
}

GpStatus
GdipGetStringFormatFlags(GDIPCONST GpStringFormat *format, StringFormatFlags *flags)
{
    if (!format) return InvalidParameter;

    *flags = format->formatFlags;

    return Ok;
}

