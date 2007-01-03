/*
 * Copyright (C) 2006 Novell, Inc (http://www.novell.com)
 *
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
 *          Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef _GENERAL_H_
#define _GENERAL_H_

#include "gdip.h"

/* avoid direct floating point comparison */
#define gdip_near_zero(value)	((value >= -0.0001f) && (value <= 0.0001f))
#define gdip_near_one(value)	((value >= 0.9999f) && (value <= 1.0001f))

/* avoid integer overflows when int/float/int conversion are used (see #79643) */
#define SAFE_FLOAT_TO_UINT32(value)	((value < 0) ? 0 : (value >= G_MAXINT32) ? G_MAXINT32 : value)

/* avoid fp division and multiplication that would return the same number */
#define OPTIMIZE_CONVERSION(g)	(((g->page_unit == UnitDisplay) && (g->type != gtPostScript)) || \
	(g->page_unit == UnitPixel) || (g->page_unit == UnitWorld))

/* macros for common conversions, i.e. to or from UnitCairoPoint */
#define gdip_unitx_convgr(g,f)		(gdip_unit_conversion (g->page_unit, UnitCairoPoint, g->dpi_x, g->type, f))
#define gdip_unity_convgr(g,f)		(gdip_unit_conversion (g->page_unit, UnitCairoPoint, g->dpi_y, g->type, f))
#define gdip_convgr_unitx(g,f)		(gdip_unit_conversion (UnitCairoPoint, g->page_unit, g->dpi_x, g->type, f))
#define gdip_convgr_unity(g,f)		(gdip_unit_conversion (UnitCairoPoint, g->page_unit, g->dpi_y, g->type, f))

/* avoid floating point division/multiplications when pre-multiplying the alpha channel with R, G and B values */
const byte pre_multiplied_table[256][256];

/* cairo has a (signed) 15(1)/16(2)bits pixel positioning, while GDI+ use (signed) 23 bits (infinity).
 * Using larger values confuse the bits used for subpixel positioning.
 * (1) http://lists.freedesktop.org/archives/cairo/2006-June/007251.html
 * (2) testing shows artefacts if using more than 15 bits
 */

#define CAIRO_LOW_LIMIT		-16384
#define CAIRO_HIGH_LIMIT	16383
#define CAIRO_LIMIT(v)		((v < CAIRO_LOW_LIMIT) ? CAIRO_LOW_LIMIT : (v > CAIRO_HIGH_LIMIT) ? CAIRO_HIGH_LIMIT : v)

/* prototypes for cairo wrappers to deal with coordonates limits, unit conversion and antialiasing) */
void gdip_cairo_rectangle (GpGraphics *graphics, double x, double y, double width, double height, BOOL antialiasing);
void gdip_cairo_move_to (GpGraphics *graphics, double x, double y, BOOL convert_units, BOOL antialiasing);
void gdip_cairo_line_to (GpGraphics *graphics, double x, double y, BOOL convert_units, BOOL antialiasing);
void gdip_cairo_curve_to (GpGraphics *graphics, double x1, double y1, double x2, double y2, double x3, double y3, 
	BOOL convert_units, BOOL antialiasing);

#endif
