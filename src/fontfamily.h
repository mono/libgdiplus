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

#ifndef __FONTFAMILY_H__
#define __FONTFAMILY_H__

/* FontFamily public API (only!) */

GpStatus WINGDIPAPI GdipCreateFontFamilyFromName (GDIPCONST WCHAR *name, GpFontCollection *fontCollection, GpFontFamily **fontFamily);
GpStatus WINGDIPAPI GdipCloneFontFamily (GpFontFamily *fontFamily, GpFontFamily **clonedFontFamily);
GpStatus WINGDIPAPI GdipDeleteFontFamily (GpFontFamily *fontFamily);
GpStatus WINGDIPAPI GdipGetCellAscent (GDIPCONST GpFontFamily *family, INT style, UINT16 *CellAscent);
GpStatus WINGDIPAPI GdipGetCellDescent (GDIPCONST GpFontFamily *family, INT style, UINT16 *CellDescent);
GpStatus WINGDIPAPI GdipGetGenericFontFamilySansSerif (GpFontFamily **nativeFamily);
GpStatus WINGDIPAPI GdipGetGenericFontFamilySerif (GpFontFamily **nativeFamily);
GpStatus WINGDIPAPI GdipGetGenericFontFamilyMonospace (GpFontFamily **nativeFamily);
GpStatus WINGDIPAPI GdipGetEmHeight (GDIPCONST GpFontFamily *family, INT style, UINT16 *EmHeight);
GpStatus WINGDIPAPI GdipGetFamilyName (GDIPCONST GpFontFamily *family, WCHAR name[LF_FACESIZE], LANGID language);
GpStatus WINGDIPAPI GdipGetLineSpacing (GDIPCONST GpFontFamily *family, INT style, UINT16 *LineSpacing);
GpStatus WINGDIPAPI GdipIsStyleAvailable (GDIPCONST GpFontFamily *family, INT style, BOOL *IsStyleAvailable);

#endif
