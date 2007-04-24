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

GpStatus GdipCreateFont (GDIPCONST GpFontFamily* family, float emSize, int style, Unit unit, GpFont **font);
GpStatus GdipCreateFontFromDC (void *hdc, GpFont **font);
GpStatus GdipDeleteFont (GpFont* font);
GpStatus GdipGetLogFontA (GpFont *font, GpGraphics *graphics, LOGFONTA *logfontA);
GpStatus GdipGetLogFontW (GpFont *font, GpGraphics *graphics, LOGFONTW *logfontW);
GpStatus GdipGetFontHeight (GDIPCONST GpFont *font, GDIPCONST GpGraphics *graphics, float *height);
GpStatus GdipGetFontHeightGivenDPI (GDIPCONST GpFont *font, float dpi, float *height);
GpStatus GdipGetFontSize (GpFont *font, float *size);

/* missing API
	GdipCreateFontFromLogfontA
	GdipCreateFontFromLogfontW
	GdipCloneFont
	GdipGetFamily
	GdipGetFontStyle
	GdipGetFontUnit
*/

/* libgdiplus extra API (not availble in MSGDI+ but exported from libgdiplus) */
GpStatus GdipCreateFontFromHfontA (HFONT hfont, GpFont **font, void *lf);

#endif
