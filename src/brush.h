/*
 * brush.h
 *
 * Authors:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) Novell, Inc. 2004.
 */

#ifndef _BRUSH_H
#define _BRUSH_H

#include "gdip.h"

/* Enums */

typedef enum {
        BrushTypeSolidColor = 0,     /* Solid Brush */
        BrushTypeHatchFill = 1,      /* Hatch Brush */
        BrushTypeTextureFill = 2,    /* Texture Brush */
        BrushTypePathGradient = 3,   /* PathGradient Brush */
        BrushTypeLinearGradient = 4  /* LinearGradient Brush */
} GpBrushType, BrushType;

/* Structures */

typedef struct _Brush GpBrush;

typedef struct _BrushClass {

	/* Brush virtual functions */
        GpBrushType type;
	void (*setup) (GpGraphics *graphics, GpBrush *brush);
	void (*clone_brush) (GpBrush *brush, GpBrush **clonedBrush);
	void (*destroy) (GpBrush *brush);
} BrushClass;

typedef struct _Brush {
	BrushClass *vtable;
} Brush;

void gdip_brush_init (GpBrush *brush, BrushClass* vtable);
void gdip_brush_setup (GpGraphics *graphics, GpBrush *brush);
void clone_brush (GpBrush *brush, GpBrush **clonedBrush);
void gdip_brush_destroy (GpBrush *brush);

GpStatus GdipCloneBrush (GpBrush *brush, GpBrush **clonedBrush); 
GpStatus GdipDeleteBrush (GpBrush *brush);
GpStatus GdipGetBrushType (GpBrush *brush, GpBrushType *type);

#endif /* _BRUSH_H */
