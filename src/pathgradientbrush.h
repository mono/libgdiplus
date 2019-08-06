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
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Ravindra (rkumar@novell.com)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2004, 2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __PATHGRADIENTBRUSH_H__
#define __PATHGRADIENTBRUSH_H__

#include "brush.h"

GpStatus WINGDIPAPI GdipCreatePathGradient (GDIPCONST GpPointF *points, INT count, GpWrapMode wrapMode, GpPathGradient **polyGradient);
GpStatus WINGDIPAPI GdipCreatePathGradientI (GDIPCONST GpPoint *points, INT count, GpWrapMode wrapMode, GpPathGradient **polyGradient);
GpStatus WINGDIPAPI GdipCreatePathGradientFromPath (GDIPCONST GpPath *path, GpPathGradient **polyGradient);

GpStatus WINGDIPAPI GdipGetPathGradientCenterColor (GpPathGradient *brush, ARGB *colors);
GpStatus WINGDIPAPI GdipSetPathGradientCenterColor (GpPathGradient *brush, ARGB colors);

GpStatus WINGDIPAPI GdipGetPathGradientSurroundColorCount (GpPathGradient *brush, INT *count);
GpStatus WINGDIPAPI GdipGetPathGradientSurroundColorsWithCount (GpPathGradient *brush, ARGB *color, INT *count);
GpStatus WINGDIPAPI GdipSetPathGradientSurroundColorsWithCount (GpPathGradient *brush, GDIPCONST ARGB *color, INT *count);

GpStatus WINGDIPAPI GdipGetPathGradientPath (GpPathGradient *brush, GpPath *path);
GpStatus WINGDIPAPI GdipSetPathGradientPath (GpPathGradient *brush, GDIPCONST GpPath *path);

GpStatus WINGDIPAPI GdipGetPathGradientCenterPoint (GpPathGradient *brush, GpPointF *points);
GpStatus WINGDIPAPI GdipGetPathGradientCenterPointI (GpPathGradient *brush, GpPoint* points);
GpStatus WINGDIPAPI GdipSetPathGradientCenterPoint (GpPathGradient *brush, GDIPCONST GpPointF *points);
GpStatus WINGDIPAPI GdipSetPathGradientCenterPointI (GpPathGradient *brush, GDIPCONST GpPoint* points);

GpStatus WINGDIPAPI GdipGetPathGradientRect (GpPathGradient *brush, GpRectF *rect);
GpStatus WINGDIPAPI GdipGetPathGradientRectI (GpPathGradient *brush, GpRect *rect);

GpStatus WINGDIPAPI GdipGetPathGradientPointCount (GpPathGradient *brush, INT *count);

GpStatus WINGDIPAPI GdipSetPathGradientGammaCorrection (GpPathGradient *brush, BOOL useGammaCorrection);
GpStatus WINGDIPAPI GdipGetPathGradientGammaCorrection (GpPathGradient *brush, BOOL *useGammaCorrection);

GpStatus WINGDIPAPI GdipGetPathGradientBlendCount (GpPathGradient *brush, INT *count);
GpStatus WINGDIPAPI GdipGetPathGradientBlend (GpPathGradient *brush, REAL *blend, REAL *positions, INT count);
GpStatus WINGDIPAPI GdipSetPathGradientBlend (GpPathGradient *brush, GDIPCONST REAL *blend, GDIPCONST REAL *positions, INT count);

GpStatus WINGDIPAPI GdipGetPathGradientPresetBlendCount (GpPathGradient *brush, INT *count);
GpStatus WINGDIPAPI GdipGetPathGradientPresetBlend (GpPathGradient *brush, ARGB *blend, REAL *positions, INT count);
GpStatus WINGDIPAPI GdipSetPathGradientPresetBlend (GpPathGradient *brush, GDIPCONST ARGB *blend, GDIPCONST REAL *positions, INT count);

GpStatus WINGDIPAPI GdipSetPathGradientSigmaBlend (GpPathGradient *brush, REAL focus, REAL scale);
GpStatus WINGDIPAPI GdipSetPathGradientLinearBlend (GpPathGradient *brush, REAL focus, REAL scale);

GpStatus WINGDIPAPI GdipGetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode *wrapMode);
GpStatus WINGDIPAPI GdipSetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode wrapMode);

GpStatus WINGDIPAPI GdipGetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipSetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix);

GpStatus WINGDIPAPI GdipResetPathGradientTransform (GpPathGradient *brush);
GpStatus WINGDIPAPI GdipMultiplyPathGradientTransform (GpPathGradient *brush, GDIPCONST GpMatrix *matrix, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipTranslatePathGradientTransform (GpPathGradient *brush, REAL dx, REAL dy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipScalePathGradientTransform (GpPathGradient *brush, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipRotatePathGradientTransform (GpPathGradient *brush, REAL angle, GpMatrixOrder order);

GpStatus WINGDIPAPI GdipGetPathGradientFocusScales (GpPathGradient *brush, REAL *xScale, REAL *yScale);
GpStatus WINGDIPAPI GdipSetPathGradientFocusScales (GpPathGradient *brush, REAL xScale, REAL yScale);

#endif
