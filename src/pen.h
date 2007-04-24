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

#ifndef __PEN_H__
#define __PEN_H__

GpStatus GdipCreatePen1 (ARGB argb, REAL width, GpUnit unit, GpPen **pen);
GpStatus GdipCreatePen2 (GpBrush *brush, REAL width, GpUnit unit, GpPen **pen);
GpStatus GdipClonePen (GpPen *pen, GpPen **clonedpen);
GpStatus GdipDeletePen (GpPen *pen);

GpStatus GdipSetPenWidth (GpPen *pen, REAL width);
GpStatus GdipGetPenWidth (GpPen *pen, REAL *width);

GpStatus GdipSetPenBrushFill (GpPen *pen, GpBrush *brush);
GpStatus GdipGetPenBrushFill (GpPen *pen, GpBrush **brush);
GpStatus GdipGetPenFillType (GpPen *pen, GpPenType *penType);
GpStatus GdipSetPenColor (GpPen *pen, ARGB color);
GpStatus GdipGetPenColor (GpPen *pen, ARGB *color);
GpStatus GdipSetPenDashStyle (GpPen *pen, GpDashStyle dashStyle);
GpStatus GdipGetPenDashStyle (GpPen *pen, GpDashStyle *dashStyle);
GpStatus GdipSetPenDashOffset (GpPen *pen, REAL offset);
GpStatus GdipGetPenDashOffset (GpPen *pen, REAL *offset);
GpStatus GdipGetPenDashCount (GpPen *pen, INT *count);
GpStatus GdipSetPenDashArray (GpPen *pen, GDIPCONST REAL *dash, INT count);
GpStatus GdipGetPenDashArray (GpPen *pen, REAL *dash, INT count);
GpStatus GdipSetPenCompoundArray (GpPen *pen, GDIPCONST REAL *dash, INT count);
GpStatus GdipGetPenCompoundArray (GpPen *pen, REAL *dash, INT count);
GpStatus GdipGetPenCompoundCount (GpPen *pen, INT *count);
GpStatus GdipSetPenMode (GpPen *pen, GpPenAlignment penMode);
GpStatus GdipGetPenMode (GpPen *pen, GpPenAlignment *penMode);
GpStatus GdipSetPenMiterLimit (GpPen *pen, REAL miterLimit);
GpStatus GdipGetPenMiterLimit (GpPen *pen, REAL *miterLimit);

GpStatus GdipSetPenLineCap197819 (GpPen *pen, GpLineCap startCap, GpLineCap endCap, GpDashCap dashCap);
GpStatus GdipSetPenStartCap (GpPen *pen, GpLineCap startCap);
GpStatus GdipGetPenStartCap (GpPen *pen, GpLineCap *startCap);
GpStatus GdipSetPenEndCap (GpPen *pen, GpLineCap endCap);
GpStatus GdipGetPenEndCap (GpPen *pen, GpLineCap *endCap);

GpStatus GdipSetPenDashCap197819 (GpPen *pen, GpDashCap dashCap);
GpStatus GdipGetPenDashCap197819 (GpPen *pen, GpDashCap *dashCap);

GpStatus GdipSetPenLineJoin (GpPen *pen, GpLineJoin lineJoin);
GpStatus GdipGetPenLineJoin (GpPen *pen, GpLineJoin *lineJoin);

GpStatus GdipSetPenCustomStartCap (GpPen *pen, GpCustomLineCap *customCap);
GpStatus GdipGetPenCustomStartCap (GpPen *pen, GpCustomLineCap **customCap);
GpStatus GdipSetPenCustomEndCap (GpPen *pen, GpCustomLineCap *customCap);
GpStatus GdipGetPenCustomEndCap (GpPen *pen, GpCustomLineCap **customCap);

GpStatus GdipSetPenTransform (GpPen *pen, GpMatrix *matrix);
GpStatus GdipGetPenTransform (GpPen *pen, GpMatrix *matrix);
GpStatus GdipResetPenTransform (GpPen *pen);
GpStatus GdipMultiplyPenTransform (GpPen *pen, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipTranslatePenTransform (GpPen *pen, REAL dx, REAL dy, GpMatrixOrder order);
GpStatus GdipScalePenTransform (GpPen *pen, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus GdipRotatePenTransform (GpPen *pen, REAL angle, GpMatrixOrder order);

/* missing API
	GdipGetPenUnit
	GdipSetPenUnit
*/

#endif
