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

#ifndef __ADJUSTABLEARROWCAP_H__
#define __ADJUSTABLEARROWCAP_H__

/* AdjustableArrowCap public API (only!) */

GpStatus GdipCreateAdjustableArrowCap (REAL height, REAL width, BOOL isFilled, GpAdjustableArrowCap **arrowCap);

GpStatus GdipGetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, REAL *height);
GpStatus GdipSetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, REAL height);

GpStatus GdipGetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, REAL *width);
GpStatus GdipSetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, REAL width);

GpStatus GdipGetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, REAL *middleInset);
GpStatus GdipSetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, REAL middleInset);

GpStatus GdipSetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, BOOL isFilled);
GpStatus GdipGetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, BOOL *isFilled);

#endif
