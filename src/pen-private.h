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

#ifndef __PEN_PRIVATE_H__
#define __PEN_PRIVATE_H__

#include "gdiplus-private.h"
#include "matrix-private.h"

struct _Pen {
	int		color;
        GpBrush		*brush;
	BOOL		own_brush;
	float		width;
        float		miter_limit;
        GpLineJoin	line_join;
        GpDashStyle	dash_style;
	GpLineCap	line_cap;	/* Cairo supports only same kind of end caps for both the ends. */
	GpLineCap	end_cap;
	GpDashCap	dash_cap;	/* not supported */
	int		compound_count;
	float		*compound_array;
        GpPenAlignment	mode;
        float		dash_offset;
        int		dash_count;
	BOOL		own_dash_array;	/* flag to mark if pen maintains its own array or global array */
        float		*dash_array;
        GpUnit		unit;		/* Always set to UnitWorld. */
	cairo_matrix_t	matrix;
        BOOL		changed;	/* flag to mark if pen is changed and needs setup */
	GpCustomLineCap *custom_start_cap;
	GpCustomLineCap *custom_end_cap;
};

GpStatus gdip_pen_setup (GpGraphics *graphics, GpPen *pen) GDIP_INTERNAL;
GpStatus gdip_pen_draw_custom_start_cap (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2);
GpStatus gdip_pen_draw_custom_end_cap (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2);

#include "pen.h"

#endif
