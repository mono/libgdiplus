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

#ifndef __PATHGRADIENTBRUSH_PRIVATE_H__
#define __PATHGRADIENTBRUSH_PRIVATE_H__

#include "gdiplus-private.h"
#include "brush-private.h"
#include "matrix-private.h"

/* _N: normalize to 0.0 .. 1.0 */
#define ARGB_ALPHA_N(argb)	((double)((argb >> 24) & 0xff) / 255.0)
#define ARGB_RED_N(argb)	((double)((argb >> 16) & 0xff) / 255.0)
#define ARGB_GREEN_N(argb)	((double)((argb >>  8) & 0xff) / 255.0)
#define ARGB_BLUE_N(argb)	((double)((argb)       & 0xff) / 255.0)

struct _PathGradient {
	GpBrush			base;
	GpPath			*boundary;
	ARGB			*boundaryColors;
	int			boundaryColorsCount;
	GpPointF		center;
	ARGB			centerColor;
	GpPointF		focusScales;
	GpRectF			rectangle;
	Blend			*blend;
	InterpolationColors	*presetColors;
	GpWrapMode		wrapMode;
	cairo_matrix_t		transform;
	cairo_pattern_t		*pattern;
	BOOL		useGammaCorrection;
};

#include "pathgradientbrush.h"

#endif
