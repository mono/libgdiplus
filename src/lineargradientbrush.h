/*
 * lineargradientbrush.h
 *
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
 *      Ravindra (rkumar@novell.com)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2004, 2007 Novell, Inc. http://www.novell.com
 */

#ifndef __LINEAR_GRADIENT_H__
#define __LINEAR_GRADIENT_H__

typedef enum {
	LinearGradientModeHorizontal		= 0,	/* angle = 0 deg    */
	LinearGradientModeVertical		= 1,	/* angle = 90 deg   */
	LinearGradientModeForwardDiagonal	= 2,	/* angle = 45 deg  */
	LinearGradientModeBackwardDiagonal	= 3	/* angle = 135 deg */
} LinearGradientMode;


GpStatus WINGDIPAPI GdipCreateLineBrushI (GDIPCONST GpPoint *point1, GDIPCONST GpPoint *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus WINGDIPAPI GdipCreateLineBrush (GDIPCONST GpPointF *point1, GDIPCONST GpPointF *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus WINGDIPAPI GdipCreateLineBrushFromRectI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus WINGDIPAPI GdipCreateLineBrushFromRect (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus WINGDIPAPI GdipCreateLineBrushFromRectWithAngleI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, REAL angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus WINGDIPAPI GdipCreateLineBrushFromRectWithAngle (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, REAL angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus WINGDIPAPI GdipGetLineBlendCount (GpLineGradient *brush, INT *count);
GpStatus WINGDIPAPI GdipSetLineBlend (GpLineGradient *brush, GDIPCONST REAL *blend, GDIPCONST REAL *positions, INT count);
GpStatus WINGDIPAPI GdipGetLineBlend (GpLineGradient *brush, REAL *blend, REAL *positions, INT count);
GpStatus WINGDIPAPI GdipSetLineGammaCorrection (GpLineGradient *brush, BOOL useGammaCorrection);
GpStatus WINGDIPAPI GdipGetLineGammaCorrection (GpLineGradient *brush, BOOL *useGammaCorrection);
GpStatus WINGDIPAPI GdipGetLinePresetBlendCount (GpLineGradient *brush, INT *count);
GpStatus WINGDIPAPI GdipSetLinePresetBlend (GpLineGradient *brush, GDIPCONST ARGB *blend, GDIPCONST REAL *positions, INT count);
GpStatus WINGDIPAPI GdipGetLinePresetBlend (GpLineGradient *brush, ARGB *blend, REAL *positions, INT count);
GpStatus WINGDIPAPI GdipSetLineColors (GpLineGradient *brush, ARGB color1, ARGB color2);
GpStatus WINGDIPAPI GdipGetLineColors (GpLineGradient *brush, ARGB *colors);
GpStatus WINGDIPAPI GdipGetLineRectI (GpLineGradient *brush, GpRect *rect);
GpStatus WINGDIPAPI GdipGetLineRect (GpLineGradient * brush, GpRectF *rect);
GpStatus WINGDIPAPI GdipSetLineTransform (GpLineGradient *brush, GDIPCONST GpMatrix *matrix);
GpStatus WINGDIPAPI GdipGetLineTransform (GpLineGradient *brush, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipSetLineWrapMode (GpLineGradient *brush, GpWrapMode wrapMode);
GpStatus WINGDIPAPI GdipGetLineWrapMode (GpLineGradient *brush, GpWrapMode *wrapMode);
GpStatus WINGDIPAPI GdipSetLineLinearBlend (GpLineGradient *brush, REAL focus, REAL scale);
GpStatus WINGDIPAPI GdipSetLineSigmaBlend (GpLineGradient *brush, REAL focus, REAL scale);
GpStatus WINGDIPAPI GdipMultiplyLineTransform (GpLineGradient *brush, GpMatrix *matrix, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipResetLineTransform (GpLineGradient *brush);
GpStatus WINGDIPAPI GdipRotateLineTransform (GpLineGradient *brush, REAL angle, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipScaleLineTransform (GpLineGradient *brush, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipTranslateLineTransform (GpLineGradient *brush, REAL dx, REAL dy, GpMatrixOrder order);

#endif
