/*
 * pathgradientbrush.h
 *
 * Author:
 *	Vladimir Vukicevic (vladimir@pobox.com)
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
    GpPointF center;
    ARGB centerColor;
    GpWrapMode wrapMode;
    GpMatrix *transform;
};

typedef struct _GpPathGradient GpPathGradient;

GpStatus GdipCreatePathGradient (GDIPCONST GpPointF *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient);

GpStatus GdipCreatePathGradientI (GDIPCONST GpPoint *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient);

GpStatus GdipCreatePathGradientFromPath (GDIPCONST GpPath *path, GpPathGradient **polyGradient);

GpStatus GdipGetPathGradientCenterColor (GpPathGradient *brush, ARGB *colors);
GpStatus GdipSetPathGradientCenterColor (GpPathGradient *brush, ARGB colors);

GpStatus GdipGetPathGradientSurroundColorsWithCount (GpPathGradient *brush, ARGB *color, int *count);
GpStatus GdipSetPathGradientSurroundColorsWithCount (GpPathGradient *brush, GDIPCONST ARGB *color, int *count);

GpStatus GdipGetPathGradientPath (GpPathGradient *brush, GpPath *path);
GpStatus GdipSetPathGradientPath (GpPathGradient *brush, GDIPCONST GpPath *path);

GpStatus GdipGetPathGradientCenterPoint (GpPathGradient *brush, GpPointF *points);
GpStatus GdipGetPathGradientCenterPointI (GpPathGradient *brush, GpPoint *points);
GpStatus GdipSetPathGradientCenterPoint (GpPathGradient *brush, GDIPCONST GpPointF *points);
GpStatus GdipSetPathGradientCenterPointI (GpPathGradient *brush, GDIPCONST GpPoint *points);

GpStatus GdipGetPathGradientRect (GpPathGradient *brush, GpRectF *rect);
GpStatus GdipGetPathGradientRectI (GpPathGradient *brush, GpRect *rect);

GpStatus GdipGetPathGradientPointCount (GpPathGradient *brush, int *count);
GpStatus GdipGetPathGradientSurroundColorCount (GpPathGradient *brush, int *count);

GpStatus GdipSetPathGradientGammaCorrection (GpPathGradient *brush, BOOL useGammaCorrection);
GpStatus GdipGetPathGradientGammaCorrection (GpPathGradient *brush, BOOL *useGammaCorrection);

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
