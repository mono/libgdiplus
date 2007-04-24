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

#include "gdiplus-private.h"
#include "brush.h"


GpStatus GdipCreatePathGradient (GDIPCONST GpPointF *points, INT count, GpWrapMode wrapMode, GpPathGradient **polyGradient);
GpStatus GdipCreatePathGradientI (GDIPCONST GpPoint *points, INT count, GpWrapMode wrapMode, GpPathGradient **polyGradient);
GpStatus GdipCreatePathGradientFromPath (GDIPCONST GpPath *path, GpPathGradient **polyGradient);

GpStatus GdipGetPathGradientCenterColor (GpPathGradient *brush, ARGB *colors);
GpStatus GdipSetPathGradientCenterColor (GpPathGradient *brush, ARGB colors);

GpStatus GdipGetPathGradientSurroundColorCount (GpPathGradient *brush, INT *count);
GpStatus GdipGetPathGradientSurroundColorsWithCount (GpPathGradient *brush, ARGB *color, INT *count);
GpStatus GdipSetPathGradientSurroundColorsWithCount (GpPathGradient *brush, GDIPCONST ARGB *color, INT *count);

GpStatus GdipGetPathGradientCenterPoint (GpPathGradient *brush, GpPointF *points);
GpStatus GdipSetPathGradientCenterPoint (GpPathGradient *brush, GDIPCONST GpPointF *points);

GpStatus GdipGetPathGradientRect (GpPathGradient *brush, GpRectF *rect);

GpStatus GdipGetPathGradientBlendCount (GpPathGradient *brush, INT *count);
GpStatus GdipGetPathGradientBlend (GpPathGradient *brush, REAL *blend, REAL *positions, INT count);
GpStatus GdipSetPathGradientBlend (GpPathGradient *brush, GDIPCONST REAL *blend, GDIPCONST REAL *positions, INT count);

GpStatus GdipGetPathGradientPresetBlendCount (GpPathGradient *brush, INT *count);
GpStatus GdipGetPathGradientPresetBlend (GpPathGradient *brush, ARGB *blend, REAL *positions, INT count);
GpStatus GdipSetPathGradientPresetBlend (GpPathGradient *brush, GDIPCONST ARGB *blend, GDIPCONST REAL *positions, INT count);

GpStatus GdipSetPathGradientSigmaBlend (GpPathGradient *brush, REAL focus, REAL scale);
GpStatus GdipSetPathGradientLinearBlend (GpPathGradient *brush, REAL focus, REAL scale);

GpStatus GdipGetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode *wrapMode);
GpStatus GdipSetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode wrapMode);

GpStatus GdipGetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix);
GpStatus GdipSetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix);

GpStatus GdipResetPathGradientTransform (GpPathGradient *brush);
GpStatus GdipMultiplyPathGradientTransform (GpPathGradient *brush, GDIPCONST GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipTranslatePathGradientTransform (GpPathGradient *brush, REAL dx, REAL dy, GpMatrixOrder order);
GpStatus GdipScalePathGradientTransform (GpPathGradient *brush, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus GdipRotatePathGradientTransform (GpPathGradient *brush, REAL angle, GpMatrixOrder order);

GpStatus GdipGetPathGradientFocusScales (GpPathGradient *brush, REAL *xScale, REAL *yScale);
GpStatus GdipSetPathGradientFocusScales (GpPathGradient *brush, REAL xScale, REAL yScale);

/* missing API
	GdipGetPathGradientPath
	GdipSetPathGradientPath
	GdipGetPathGradientCenterPointI
	GdipSetPathGradientCenterPointI
	GdipGetPathGradientRectI
	GdipGetPathGradientGammaCorrection
	GdipSetPathGradientGammaCorrection
*/

#endif
