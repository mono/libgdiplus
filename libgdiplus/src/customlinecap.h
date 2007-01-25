/*
 * customlinecap.h
 *
 * Authors:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) Novell, Inc. 2004. http://www.novell.com
 */

#ifndef _CUSTOM_LINECAP_H
#define _CUSTOM_LINECAP_H

#include "gdip.h"

/* Structures */
typedef struct _CapClass CapClass;

typedef struct _CustomLineCap {
	CapClass *vtable;
	GpPath *fill_path;
	GpPath *stroke_path;
	GpLineCap base_cap;
	GpLineCap start_cap;
	GpLineCap end_cap;
	GpLineJoin stroke_join;
	float base_inset;
	float width_scale;
} CustomLineCap;

typedef struct _AdjustableArrowCap {
	GpCustomLineCap base;
	float width;
	float height;
	float middle_inset;
	BOOL fill_state;
} AdjustableArrowCap;

struct _CapClass {
	CustomLineCapType type;
	GpStatus (*setup) (GpGraphics *graphics, GpCustomLineCap *cap);
	GpStatus (*clone_cap) (GpCustomLineCap *cap, GpCustomLineCap **clonedCap);
	GpStatus (*destroy) (GpCustomLineCap *cap);
};

/* CustomLineCap */
void gdip_custom_linecap_init (GpCustomLineCap *cap, CapClass *vt);
GpStatus gdip_linecap_setup (GpGraphics *graphics, GpCustomLineCap *customCap);

#endif /* _CUSTOM_LINECAP_H */
