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

#ifndef __FONT_H__
#define __FONT_H__

/* Font public API (only!) */

GpStatus WINGDIPAPI GdipCreateFont (GDIPCONST GpFontFamily *family, REAL emSize, INT style, Unit unit, GpFont **font);
GpStatus WINGDIPAPI GdipCreateFontFromDC (HDC hdc, GpFont **font);

GpStatus WINGDIPAPI GdipCreateFontFromLogfontA (HDC hdc, GDIPCONST LOGFONTA *logfont, GpFont **font);
GpStatus WINGDIPAPI GdipCreateFontFromLogfontW (HDC hdc, GDIPCONST LOGFONTW *logfont, GpFont **font);

GpStatus WINGDIPAPI GdipCloneFont (GpFont *font, GpFont **cloneFont);

GpStatus WINGDIPAPI GdipDeleteFont (GpFont *font);
GpStatus WINGDIPAPI GdipGetLogFontA (GpFont *font, GpGraphics *graphics, LOGFONTA *logfontA);
GpStatus WINGDIPAPI GdipGetLogFontW (GpFont *font, GpGraphics *graphics, LOGFONTW *logfontW);
GpStatus WINGDIPAPI GdipGetFontHeight (GDIPCONST GpFont *font, GDIPCONST GpGraphics *graphics, REAL *height);
GpStatus WINGDIPAPI GdipGetFontHeightGivenDPI (GDIPCONST GpFont *font, REAL dpi, REAL *height);
GpStatus WINGDIPAPI GdipGetFontSize (GpFont *font, REAL *size);
GpStatus WINGDIPAPI GdipGetFontStyle (GpFont *font, INT *style);
GpStatus WINGDIPAPI GdipGetFontUnit (GpFont *font, Unit *unit);
GpStatus WINGDIPAPI GdipGetFamily (GpFont *font, GpFontFamily **family);

/* libgdiplus extra API (not availble in MSGDI+ but exported from libgdiplus) */
GpStatus WINGDIPAPI GdipCreateFontFromHfontA (HFONT hfont, GpFont **font, void *lf);

#endif
