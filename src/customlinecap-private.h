/*
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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __CUSTOMLINECAP_PRIVATE_H__
#define __CUSTOMLINECAP_PRIVATE_H__

#include "gdiplus-private.h"

typedef struct _CapClass {
	CustomLineCapType type;
	GpStatus (*setup) (GpGraphics *graphics, GpCustomLineCap *cap);
	GpStatus (*clone_cap) (GpCustomLineCap *cap, GpCustomLineCap **clonedCap);
	GpStatus (*destroy) (GpCustomLineCap *cap);
	GpStatus (*draw) (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *cap, float x, float y, float otherend_x, float otherend_y);
} CapClass;

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

void gdip_custom_linecap_init (GpCustomLineCap *cap, CapClass *vt) GDIP_INTERNAL;
GpStatus gdip_linecap_setup (GpGraphics *graphics, GpCustomLineCap *customCap) GDIP_INTERNAL;
GpStatus gdip_linecap_draw (GpGraphics *graphics, GpPen *pen, GpCustomLineCap *customCap, float x, float y, float otherend_x, float otherend_y) GDIP_INTERNAL;
double gdip_custom_linecap_angle (float x, float y, float otherend_x, float otherend_y);

#include "customlinecap.h"

#endif
