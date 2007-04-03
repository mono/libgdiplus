/*
 * brush.h
 *
 * Authors:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) Novell, Inc. 2004. http://www.novell.com
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
 */

#ifndef _BRUSH_H
#define _BRUSH_H

#include "gdip.h"

/* Structures */

typedef struct _BrushClass {

	/* Brush virtual functions */
        GpBrushType type;
	GpStatus (*setup) (GpGraphics *graphics, GpBrush *brush);
	GpStatus (*clone_brush) (GpBrush *brush, GpBrush **clonedBrush);
	GpStatus (*destroy) (GpBrush *brush);
} BrushClass;

typedef struct _Brush {
	BrushClass *vtable;
	BOOL changed;
} Brush;

/* used by gradient brushes */
typedef struct _Blend {
	float *factors;
	float *positions;
	int count;
} Blend;

typedef struct _InterpolationColors {
	ARGB* colors;
	float *positions;
	int count;
} InterpolationColors;


GpStatus GdipCloneBrush (GpBrush *brush, GpBrush **clonedBrush); 
GpStatus GdipDeleteBrush (GpBrush *brush);
GpStatus GdipGetBrushType (GpBrush *brush, GpBrushType *type);

/* private */
void gdip_brush_init (GpBrush *brush, BrushClass* vtable) GDIP_INTERNAL;
GpStatus gdip_brush_setup (GpGraphics *graphics, GpBrush *brush) GDIP_INTERNAL;

#endif /* _BRUSH_H */
