/*
 * general-private.h
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

#ifndef __GENERAL_PRIVATE_H__
#define __GENERAL_PRIVATE_H__

#include "gdiplus-private.h"

#define PI				3.14159265358979323846
#define DEGTORAD			(PI / 180.0)

/* looks like they are not available everywhere :( */
#ifndef G_MAXINT32
#define G_MAXINT32			2147483647
#define G_MININT16			-32768
#define G_MAXINT16			32767
#endif

#define MAKE_ARGB_RGB(r,g,b)		(((r&0xff) << 16) | ((g&0xff) <<  8) | (b&0xff))
#define MAKE_ARGB_ARGB(a,r,g,b)		(((guint32)(a&0xff) << 24) | ((r&0xff) << 16) | ((g&0xff) << 8) | (b&0xff))

#if !defined(min)
#define min(a,b)			(a < b ? a : b)
#endif

#if !defined(max)
#define max(a,b)			(a > b ? a : b)
#endif

#if !defined(__has_attribute)
#define __has_attribute(x) 0
#endif

#if __has_attribute(used) || defined(__GNUC__)
#define ATTRIBUTE_USED __attribute__((used))
#else
#define ATTRIBUTE_USED
#endif

/* avoid direct floating point comparison */
#define gdip_near_zero(value)		((value >= -0.00059604645f) && (value <= 0.00059604645f))
#define gdip_near_one(value)		gdip_near_zero (value - 1.0f)

/* avoid integer overflows when int/float/int conversion are used (see #79643) */
#define SAFE_FLOAT_TO_UINT32(value)    ((value < 0) ? 0 : (value >= G_MAXINT32) ? G_MAXINT32 : (((int)value) < 0 ? G_MAXINT32 : (int)value))

/* avoid fp division and multiplication that would return the same number */
#define OPTIMIZE_CONVERSION(g)		((g->type != gtPostScript) && ((g->page_unit == UnitPixel) || (g->page_unit == UnitWorld)))

/* macros for common conversions, i.e. to or from UnitCairoPoint */
#define gdip_unitx_convgr(g,f)		(gdip_unit_conversion (g->page_unit, UnitCairoPoint, g->dpi_x, g->type, f))
#define gdip_unity_convgr(g,f)		(gdip_unit_conversion (g->page_unit, UnitCairoPoint, g->dpi_y, g->type, f))
#define gdip_convgr_unitx(g,f)		(gdip_unit_conversion (UnitCairoPoint, g->page_unit, g->dpi_x, g->type, f))
#define gdip_convgr_unity(g,f)		(gdip_unit_conversion (UnitCairoPoint, g->page_unit, g->dpi_y, g->type, f))

/* pixman always use 32bits for pixman_bits_t */
#define gdip_align_stride(s)		{ s += (sizeof(guint32)-1); s &= ~(sizeof(guint32)-1); }

/* avoid floating point division/multiplications when pre-multiplying the alpha channel with R, G and B values */
extern const BYTE pre_multiplied_table[256][256];
extern const BYTE pre_multiplied_table_reverse[256][256];
extern BOOL gdiplusInitialized;

#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1,6,0)
	/* older cairo has a (signed) 15(1)/16(2)bits pixel positioning, while GDI+ use (signed) 23 bits (infinity).
	 * Using larger values confuse the bits used for subpixel positioning.
	 * (1) http://lists.freedesktop.org/archives/cairo/2006-June/007251.html
	 * (2) testing shows artefacts if using more than 15 bits
	 */
	#define CAIRO_LOW_LIMIT		-16384
	#define CAIRO_HIGH_LIMIT	16383
	#define CAIRO_LIMIT(v)		((v < CAIRO_LOW_LIMIT) ? CAIRO_LOW_LIMIT : (v > CAIRO_HIGH_LIMIT) ? CAIRO_HIGH_LIMIT : v)
#else
	/* since cairo 1.6 values are split 24/8 */
	#define CAIRO_LIMIT(v)		v
#endif

/* other shared functions */
int iround (float d) GDIP_INTERNAL;
float gdip_erf (float x, float std, float mean) GDIP_INTERNAL;

float gdip_get_display_dpi () GDIP_INTERNAL;
GpStatus gdip_get_status (cairo_status_t status) GDIP_INTERNAL;
GpStatus gdip_get_pattern_status (cairo_pattern_t *pat) GDIP_INTERNAL;

gchar *utf16_to_utf8 (const gunichar2 *ucs2, int length) GDIP_INTERNAL;
BOOL utf8_to_ucs2 (const gchar *utf8, gunichar2 *ucs2, int ucs2_len) GDIP_INTERNAL;
int utf8_encode_ucs2char (gunichar2 unichar, unsigned char *dest) GDIP_INTERNAL;

/* for drawing curves */
GpPointF *convert_points (const GpPoint *points, int count) GDIP_INTERNAL;
GpRectF *convert_rects (const GpRect *rect, int count) GDIP_INTERNAL;
GpPointF *gdip_closed_curve_tangents (int terms, const GpPointF *points, int count, float tension) GDIP_INTERNAL;
GpPointF *gdip_open_curve_tangents (int terms,  const GpPointF *points, int count, float tension) GDIP_INTERNAL;

/* Conversion functions. */
void gdip_RectF_from_Rect (const GpRect* rect, GpRectF* rectf) GDIP_INTERNAL;
void gdip_Rect_from_RectF (const GpRectF* rectf, GpRect* rect) GDIP_INTERNAL;
void gdip_PointF_from_Point (const GpPoint* point, GpPointF* pointf) GDIP_INTERNAL;
void gdip_Point_from_PointF (const GpPointF* pointf, GpPoint* point) GDIP_INTERNAL;

/* Rect functions */
void gdip_normalize_rectangle (const GpRectF *rect, GpRectF *normalized) GDIP_INTERNAL;
BOOL gdip_is_rectF_empty (const GpRectF *rect, BOOL allowNegative) GDIP_INTERNAL;

/* memory */
void* gdip_realloc (void *org, int size) GDIP_INTERNAL;
void* gdip_calloc (size_t nelem, size_t elsize) GDIP_INTERNAL;

/* checksums */
DWORD gdip_crc32 (const BYTE *buf, size_t size) GDIP_INTERNAL;

#include "general.h"

#endif
