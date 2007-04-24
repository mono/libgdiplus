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

GpStatus GdipCreateStringFormat (INT formatAttributes, LANGID language, GpStringFormat **format);
GpStatus GdipStringFormatGetGenericDefault (GpStringFormat **format);               
GpStatus GdipStringFormatGetGenericTypographic (GpStringFormat **format);                                                                                              
GpStatus GdipCloneStringFormat (GDIPCONST GpStringFormat* format, GpStringFormat** newFormat);
GpStatus GdipDeleteStringFormat (GpStringFormat *format);

GpStatus GdipGetStringFormatAlign (GDIPCONST GpStringFormat *format, StringAlignment *align);
GpStatus GdipSetStringFormatAlign (GpStringFormat *format, StringAlignment align);

GpStatus GdipGetStringFormatDigitSubstitution (GDIPCONST GpStringFormat *format, LANGID *language, StringDigitSubstitute *substitute);
GpStatus GdipSetStringFormatDigitSubstitution (GpStringFormat *format, LANGID language, StringDigitSubstitute substitute);

GpStatus GdipGetStringFormatFlags (GDIPCONST GpStringFormat *format, INT *flags);
GpStatus GdipSetStringFormatFlags (GpStringFormat *format, INT flags);

GpStatus GdipGetStringFormatHotkeyPrefix (GDIPCONST GpStringFormat *format, INT *hotkeyPrefix);
GpStatus GdipSetStringFormatHotkeyPrefix (GpStringFormat *format, INT hotkeyPrefix);

GpStatus GdipGetStringFormatLineAlign (GDIPCONST GpStringFormat *format, StringAlignment *align);
GpStatus GdipSetStringFormatLineAlign (GpStringFormat *format, StringAlignment align);

GpStatus GdipGetStringFormatMeasurableCharacterRangeCount (GDIPCONST GpStringFormat *format, INT *count);
GpStatus GdipSetStringFormatMeasurableCharacterRanges (GpStringFormat *format, INT rangeCount, GDIPCONST CharacterRange *ranges);

GpStatus GdipGetStringFormatTabStopCount (GDIPCONST GpStringFormat *format, INT *count);
GpStatus GdipGetStringFormatTabStops (GDIPCONST GpStringFormat *format, INT count, REAL *firstTabOffset, REAL *tabStops);
GpStatus GdipSetStringFormatTabStops (GpStringFormat *format, REAL firstTabOffset, INT count, GDIPCONST REAL *tabStops);

GpStatus GdipGetStringFormatTrimming (GDIPCONST GpStringFormat *format, StringTrimming *trimming);
GpStatus GdipSetStringFormatTrimming (GpStringFormat *format, StringTrimming trimming);

#endif
