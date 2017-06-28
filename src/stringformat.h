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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __STRINGFORMAT_H__
#define __STRINGFORMAT_H__

/* StringFormat public API (only!) */

GpStatus WINGDIPAPI GdipCreateStringFormat (INT formatAttributes, LANGID language, GpStringFormat **format);
GpStatus WINGDIPAPI GdipStringFormatGetGenericDefault (GpStringFormat **format);
GpStatus WINGDIPAPI GdipStringFormatGetGenericTypographic (GpStringFormat **format);
GpStatus WINGDIPAPI GdipCloneStringFormat (GDIPCONST GpStringFormat* format, GpStringFormat** newFormat);
GpStatus WINGDIPAPI GdipDeleteStringFormat (GpStringFormat *format);

GpStatus WINGDIPAPI GdipGetStringFormatAlign (GDIPCONST GpStringFormat *format, StringAlignment *align);
GpStatus WINGDIPAPI GdipSetStringFormatAlign (GpStringFormat *format, StringAlignment align);

GpStatus WINGDIPAPI GdipGetStringFormatDigitSubstitution (GDIPCONST GpStringFormat *format, LANGID *language, StringDigitSubstitute *substitute);
GpStatus WINGDIPAPI GdipSetStringFormatDigitSubstitution (GpStringFormat *format, LANGID language, StringDigitSubstitute substitute);

GpStatus WINGDIPAPI GdipGetStringFormatFlags (GDIPCONST GpStringFormat *format, INT *flags);
GpStatus WINGDIPAPI GdipSetStringFormatFlags (GpStringFormat *format, INT flags);

GpStatus WINGDIPAPI GdipGetStringFormatHotkeyPrefix (GDIPCONST GpStringFormat *format, INT *hotkeyPrefix);
GpStatus WINGDIPAPI GdipSetStringFormatHotkeyPrefix (GpStringFormat *format, INT hotkeyPrefix);

GpStatus WINGDIPAPI GdipGetStringFormatLineAlign (GDIPCONST GpStringFormat *format, StringAlignment *align);
GpStatus WINGDIPAPI GdipSetStringFormatLineAlign (GpStringFormat *format, StringAlignment align);

GpStatus WINGDIPAPI GdipGetStringFormatMeasurableCharacterRangeCount (GDIPCONST GpStringFormat *format, INT *count);
GpStatus WINGDIPAPI GdipSetStringFormatMeasurableCharacterRanges (GpStringFormat *format, INT rangeCount, GDIPCONST CharacterRange *ranges);

GpStatus WINGDIPAPI GdipGetStringFormatTabStopCount (GDIPCONST GpStringFormat *format, INT *count);
GpStatus WINGDIPAPI GdipGetStringFormatTabStops (GDIPCONST GpStringFormat *format, INT count, REAL *firstTabOffset, REAL *tabStops);
GpStatus WINGDIPAPI GdipSetStringFormatTabStops (GpStringFormat *format, REAL firstTabOffset, INT count, GDIPCONST REAL *tabStops);

GpStatus WINGDIPAPI GdipGetStringFormatTrimming (GDIPCONST GpStringFormat *format, StringTrimming *trimming);
GpStatus WINGDIPAPI GdipSetStringFormatTrimming (GpStringFormat *format, StringTrimming trimming);

#endif
