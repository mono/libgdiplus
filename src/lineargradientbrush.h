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

#include "brush-private.h"

typedef enum {
	LinearGradientModeHorizontal		= 0,	/* angle = 0 deg    */
	LinearGradientModeVertical		= 1,	/* angle = 90 deg   */
	LinearGradientModeForwardDiagonal	= 2,	/* angle = 45 deg  */
	LinearGradientModeBackwardDiagonal	= 3	/* angle = 135 deg */
} LinearGradientMode;


GpStatus GdipCreateLineBrushI (GDIPCONST GpPoint *point1, GDIPCONST GpPoint *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus GdipCreateLineBrush (GDIPCONST GpPointF *point1, GDIPCONST GpPointF *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus GdipCreateLineBrushFromRectI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus GdipCreateLineBrushFromRect (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus GdipCreateLineBrushFromRectWithAngleI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, float angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus GdipCreateLineBrushFromRectWithAngle (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, float angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient);
GpStatus GdipGetLineBlendCount (GpLineGradient *brush, int *count);
GpStatus GdipSetLineBlend (GpLineGradient *brush, GDIPCONST float *blend, GDIPCONST float *positions, int count);
GpStatus GdipGetLineBlend (GpLineGradient *brush, float *blend, float *positions, int count);
GpStatus GdipSetLineGammaCorrection (GpLineGradient *brush, BOOL useGammaCorrection);
GpStatus GdipGetLineGammaCorrection (GpLineGradient *brush, BOOL *useGammaCorrection);
GpStatus GdipGetLinePresetBlendCount (GpLineGradient *brush, int *count);
GpStatus GdipSetLinePresetBlend (GpLineGradient *brush, GDIPCONST ARGB *blend, GDIPCONST float *positions, int count);
GpStatus GdipGetLinePresetBlend (GpLineGradient *brush, ARGB *blend, float *positions, int count);
GpStatus GdipSetLineColors (GpLineGradient *brush, ARGB color1, ARGB color2);
GpStatus GdipGetLineColors (GpLineGradient *brush, ARGB *colors);
GpStatus GdipGetLineRectI (GpLineGradient *brush, GpRect *rect);
GpStatus GdipGetLineRect (GpLineGradient * brush, GpRectF *rect);
GpStatus GdipSetLineTransform (GpLineGradient *brush, GDIPCONST GpMatrix *matrix);
GpStatus GdipGetLineTransform (GpLineGradient *brush, GpMatrix *matrix);
GpStatus GdipSetLineWrapMode (GpLineGradient *brush, GpWrapMode wrapMode);
GpStatus GdipGetLineWrapMode (GpLineGradient *brush, GpWrapMode *wrapMode);
GpStatus GdipSetLineLinearBlend (GpLineGradient *brush, float focus, float scale);
GpStatus GdipSetLineSigmaBlend (GpLineGradient *brush, float focus, float scale);
GpStatus GdipMultiplyLineTransform (GpLineGradient *brush, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipResetLineTransform (GpLineGradient *brush);
GpStatus GdipRotateLineTransform (GpLineGradient *brush, float angle, GpMatrixOrder order);
GpStatus GdipScaleLineTransform (GpLineGradient *brush, float sx, float sy, GpMatrixOrder order);
GpStatus GdipTranslateLineTransform (GpLineGradient *brush, float dx, float dy, GpMatrixOrder order);

#endif
