/*
 * lineargradientbrush.h
 *
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc. http://www.novell.com
 */

#ifndef _LINEAR_GRADIENT_H
#define _LINEAR_GRADIENT_H

#include "brush.h"

#define DEFAULT_GRADIENT_ANGLE	45.0f

typedef enum {
	LinearGradientModeHorizontal = 0,	/* angle = 0 deg    */
	LinearGradientModeVertical = 1,		/* angle = 90 deg   */
	LinearGradientModeForwardDiagonal = 2,	/* angle = 45 deg  */
	LinearGradientModeBackwardDiagonal = 3	/* angle = 135 deg */
} LinearGradientMode;

typedef struct _LineGradient {
	GpBrush base;
	ARGB lineColors [2];
	GpPointF points [2];
	GpRectF *rectangle;
	GpMatrix *matrix;
	GpWrapMode wrapMode;
	float angle;
	Blend *blend;
	InterpolationColors *presetColors;
	cairo_pattern_t *pattern;
	BOOL isAngleScalable;
	BOOL gammaCorrection; /* FIXME: Not used */
} LineGradient;

typedef struct _LineGradient GpLineGradient;
typedef LinearGradientMode GpLinearGradientMode;

/* LinearGradientBrush functions */
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

#endif /* _LINEAR_GRADIENT_H */
