/*
 * pathgradientbrush.h
 *
 * Authors:
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc.
 */

#ifndef _PATHGRADIENTBRUSH_
#define _PATHGRADIENTBRUSH_

#include "gdip.h"
#include "brush.h"

struct _GpPathGradient {
	GpBrush base;
	GpPath *boundary;
	ARGB *boundaryColors;
	int boundaryColorsCount;
	GpPointF center;
	ARGB centerColor;
	GpPointF focusScales;
	GpRectF rectangle;
	Blend *blend;
	InterpolationColors *presetColors;
	GpWrapMode wrapMode;
	cairo_matrix_t transform;
	cairo_pattern_t *pattern;
};

typedef struct _GpPathGradient GpPathGradient;

GpStatus GdipCreatePathGradient (GDIPCONST GpPointF *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient);
GpStatus GdipCreatePathGradientI (GDIPCONST GpPoint *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient);
GpStatus GdipCreatePathGradientFromPath (GDIPCONST GpPath *path, GpPathGradient **polyGradient);

GpStatus GdipGetPathGradientCenterColor (GpPathGradient *brush, ARGB *colors);
GpStatus GdipSetPathGradientCenterColor (GpPathGradient *brush, ARGB colors);

GpStatus GdipGetPathGradientSurroundColorCount (GpPathGradient *brush, int *count);
GpStatus GdipGetPathGradientSurroundColorsWithCount (GpPathGradient *brush, ARGB *color, int *count);
GpStatus GdipSetPathGradientSurroundColorsWithCount (GpPathGradient *brush, GDIPCONST ARGB *color, int *count);

GpStatus GdipGetPathGradientCenterPoint (GpPathGradient *brush, GpPointF *points);
GpStatus GdipSetPathGradientCenterPoint (GpPathGradient *brush, GDIPCONST GpPointF *points);

GpStatus GdipGetPathGradientRect (GpPathGradient *brush, GpRectF *rect);

GpStatus GdipGetPathGradientBlendCount (GpPathGradient *brush, int *count);
GpStatus GdipGetPathGradientBlend (GpPathGradient *brush, float *blend, float *positions, int count);
GpStatus GdipSetPathGradientBlend (GpPathGradient *brush, GDIPCONST float *blend, GDIPCONST float *positions, int count);

GpStatus GdipGetPathGradientPresetBlendCount (GpPathGradient *brush, int *count);
GpStatus GdipGetPathGradientPresetBlend (GpPathGradient *brush, ARGB *blend, float *positions, int count);
GpStatus GdipSetPathGradientPresetBlend (GpPathGradient *brush, GDIPCONST ARGB *blend, GDIPCONST float *positions, int count);

GpStatus GdipSetPathGradientSigmaBlend (GpPathGradient *brush, float focus, float scale);
GpStatus GdipSetPathGradientLinearBlend (GpPathGradient *brush, float focus, float scale);

GpStatus GdipGetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode *wrapMode);
GpStatus GdipSetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode wrapMode);

GpStatus GdipGetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix);
GpStatus GdipSetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix);

GpStatus GdipResetPathGradientTransform (GpPathGradient *brush);
GpStatus GdipMultiplyPathGradientTransform (GpPathGradient *brush, GDIPCONST GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipTranslatePathGradientTransform (GpPathGradient *brush, float dx, float dy, GpMatrixOrder order);
GpStatus GdipScalePathGradientTransform (GpPathGradient *brush, float sx, float sy, GpMatrixOrder order);
GpStatus GdipRotatePathGradientTransform (GpPathGradient *brush, float angle, GpMatrixOrder order);

GpStatus GdipGetPathGradientFocusScales (GpPathGradient *brush, float *xScale, float *yScale);
GpStatus GdipSetPathGradientFocusScales (GpPathGradient *brush, float xScale, float yScale);

#endif /* _PATHGRADIENTBRUSH_ */
