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

/* Solid brush function */
GpStatus GdipCreateSolidFill (int color, GpSolidFill **brush);
GpStatus GdipGetSolidFillColor (GpSolidFill *brush, int *color);
GpStatus GdipSetSolidFillColor (GpSolidFill *brush, int color);

#endif /* _SOLIDBRUSH_H */
