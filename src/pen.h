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

GpStatus WINGDIPAPI GdipCreatePen1 (ARGB argb, REAL width, GpUnit unit, GpPen **pen);
GpStatus WINGDIPAPI GdipCreatePen2 (GpBrush *brush, REAL width, GpUnit unit, GpPen **pen);
GpStatus WINGDIPAPI GdipClonePen (GpPen *pen, GpPen **clonepen);
GpStatus WINGDIPAPI GdipDeletePen (GpPen *pen);

GpStatus WINGDIPAPI GdipSetPenWidth (GpPen *pen, REAL width);
GpStatus WINGDIPAPI GdipGetPenWidth (GpPen *pen, REAL *width);

GpStatus WINGDIPAPI GdipGetPenUnit (GpPen *pen, GpUnit *unit);
GpStatus WINGDIPAPI GdipSetPenUnit (GpPen *pen, GpUnit unit);

GpStatus WINGDIPAPI GdipSetPenBrushFill (GpPen *pen, GpBrush *brush);
GpStatus WINGDIPAPI GdipGetPenBrushFill (GpPen *pen, GpBrush **brush);
GpStatus WINGDIPAPI GdipGetPenFillType (GpPen *pen, GpPenType *type);
GpStatus WINGDIPAPI GdipSetPenColor (GpPen *pen, ARGB color);
GpStatus WINGDIPAPI GdipGetPenColor (GpPen *pen, ARGB *color);
GpStatus WINGDIPAPI GdipSetPenDashStyle (GpPen *pen, GpDashStyle dashstyle);
GpStatus WINGDIPAPI GdipGetPenDashStyle (GpPen *pen, GpDashStyle *dashstyle);
GpStatus WINGDIPAPI GdipSetPenDashOffset (GpPen *pen, REAL offset);
GpStatus WINGDIPAPI GdipGetPenDashOffset (GpPen *pen, REAL *offset);
GpStatus WINGDIPAPI GdipGetPenDashCount (GpPen *pen, INT *count);
GpStatus WINGDIPAPI GdipSetPenDashArray (GpPen *pen, GDIPCONST REAL *dash, INT count);
GpStatus WINGDIPAPI GdipGetPenDashArray (GpPen *pen, REAL *dash, INT count);
GpStatus WINGDIPAPI GdipSetPenCompoundArray (GpPen *pen, GDIPCONST REAL *dash, INT count);
GpStatus WINGDIPAPI GdipGetPenCompoundArray (GpPen *pen, REAL *dash, INT count);
GpStatus WINGDIPAPI GdipGetPenCompoundCount (GpPen *pen, INT *count);
GpStatus WINGDIPAPI GdipSetPenMode (GpPen *pen, GpPenAlignment penMode);
GpStatus WINGDIPAPI GdipGetPenMode (GpPen *pen, GpPenAlignment *penMode);
GpStatus WINGDIPAPI GdipSetPenMiterLimit (GpPen *pen, REAL miterLimit);
GpStatus WINGDIPAPI GdipGetPenMiterLimit (GpPen *pen, REAL *miterLimit);

GpStatus WINGDIPAPI GdipSetPenLineCap197819 (GpPen *pen, GpLineCap startCap, GpLineCap endCap, GpDashCap dashCap);
GpStatus WINGDIPAPI GdipSetPenStartCap (GpPen *pen, GpLineCap startCap);
GpStatus WINGDIPAPI GdipGetPenStartCap (GpPen *pen, GpLineCap *startCap);
GpStatus WINGDIPAPI GdipSetPenEndCap (GpPen *pen, GpLineCap endCap);
GpStatus WINGDIPAPI GdipGetPenEndCap (GpPen *pen, GpLineCap *endCap);

GpStatus WINGDIPAPI GdipSetPenDashCap197819 (GpPen *pen, GpDashCap dashCap);
GpStatus WINGDIPAPI GdipGetPenDashCap197819 (GpPen *pen, GpDashCap *dashCap);

GpStatus WINGDIPAPI GdipSetPenLineJoin (GpPen *pen, GpLineJoin lineJoin);
GpStatus WINGDIPAPI GdipGetPenLineJoin (GpPen *pen, GpLineJoin *lineJoin);

GpStatus WINGDIPAPI GdipSetPenCustomStartCap (GpPen *pen, GpCustomLineCap *customCap);
GpStatus WINGDIPAPI GdipGetPenCustomStartCap (GpPen *pen, GpCustomLineCap **customCap);
GpStatus WINGDIPAPI GdipSetPenCustomEndCap (GpPen *pen, GpCustomLineCap *customCap);
GpStatus WINGDIPAPI GdipGetPenCustomEndCap (GpPen *pen, GpCustomLineCap **customCap);

GpStatus WINGDIPAPI GdipSetPenTransform (GpPen *pen, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipGetPenTransform (GpPen *pen, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipResetPenTransform (GpPen *pen);
GpStatus WINGDIPAPI GdipMultiplyPenTransform (GpPen *pen, GpMatrix *matrix, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipTranslatePenTransform (GpPen *pen, REAL dx, REAL dy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipScalePenTransform (GpPen *pen, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipRotatePenTransform (GpPen *pen, REAL angle, GpMatrixOrder order);

#endif
