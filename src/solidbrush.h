/*
 * solidbrush.h
 *
 * Authors:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) Novell, Inc. 2004.
 */

#ifndef _SOLIDBRUSH_H
#define _SOLIDBRUSH_H

#include "brush.h"

/* Structures */

typedef struct _SolidFill GpSolidFill;

typedef struct _SolidFill {
	GpBrush base;
	int color;
} SolidFill;

/* Solid fill */
GpSolidFill *gdip_solidfill_new (void);
void gdip_solidfill_init (GpSolidFill *brush);
void gdip_solidfill_setup (GpGraphics *graphics, GpBrush *brush);
void gdip_solidfill_clone (GpBrush *brush, GpBrush **clonedBrush);
void gdip_solidfill_destroy (GpBrush *brush);

GpStatus GdipCreateSolidFill (int color, GpSolidFill **brush);
GpStatus GdipGetSolidFillColor (GpSolidFill *brush, int *color);
GpStatus GdipSetSolidFillColor (GpSolidFill *brush, int color);

#endif /* _SOLIDBRUSH_H */
