/*
 * general.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
 * Copyright (C) 2006, 2007 Novell, Inc (http://www.novell.com)
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
 *   Alexandre Pigolkine(pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "general-private.h"
#include "codecs-private.h"
#include "graphics-private.h"
#include "font-private.h"
#include "carbon-private.h"

/* large table to avoid a division and three multiplications when premultiplying alpha into R, G and B */
#include "alpha-premul-table.inc"

static BOOL startup = FALSE;

GpStatus 
GdiplusStartup (ULONG_PTR *token, const GdiplusStartupInput *input, GdiplusStartupOutput *output)
{
	GpStatus status = Ok;
	/* don't initialize multiple time, e.g. for each appdomain */
	if (!startup) {
		startup = TRUE;
		status = initCodecList ();
		if (status != Ok)
			return status;
		FcInit ();
		*token = 1;
		gdip_get_display_dpi();
	}
	return status;
}

void 
GdiplusShutdown (ULONG_PTR token)
{
	if (startup) {
		releaseCodecList ();
		gdip_font_clear_pattern_cache ();
#if HAVE_FCFINI
		FcFini ();
#endif
		startup = FALSE; /* in case we want to restart it */
	}
}


/* Memory */
void *
GdipAlloc (size_t size)
{
	return malloc (size);
}

void *
gdip_calloc (size_t nelem, size_t elsize)
{
	return calloc (nelem, elsize);
}

void *
gdip_realloc (void *org, int size)
{
	return realloc (org, size);
}

void 
GdipFree (void *ptr)
{
	free (ptr);
}

/* Helpers */
GpStatus 
gdip_get_status (cairo_status_t status)
{
        if (status == CAIRO_STATUS_SUCCESS)
                return Ok;

        else {
                switch (status) {

                case CAIRO_STATUS_NO_MEMORY:
                        return OutOfMemory;

                case CAIRO_STATUS_INVALID_MATRIX:
                case CAIRO_STATUS_INVALID_RESTORE:
                case CAIRO_STATUS_INVALID_POP_GROUP:
                        return InvalidParameter;                

                case CAIRO_STATUS_NO_CURRENT_POINT:
                        return WrongState;

                default:
                        return GenericError;
                }
        }
}

float
gdip_get_display_dpi ()
{
	static float dpis = 0;
	Display* display;

	if (dpis == 0) {
#if __APPLE__
		float h_dpi, v_dpi;
		gdip_get_display_dpi_carbon (&h_dpi, &v_dpi);

		dpis = h_dpi;
		return dpis;
#else
		char *val;

		display = XOpenDisplay (0);
		/* If the display is openable lets try to read dpi from it; otherwise use a default of 96.0f */
		if (display) {
			val = XGetDefault (display, "Xft", "dpi");
			if (val) {
				dpis = atof (val);
			} else {
				dpis = 96.0f;
			}
			XCloseDisplay (display);
		} else {
			dpis = 96.0f;
		}
#endif
	}

	return dpis;
}


/* Curve handling
 *
 * See Richard Rasala, Explicit Cubic Spline Interpolation Formulas, in
 *     Andrew S. Glassner, Graphics Gems, Academic Press, 1990, 579-584.
 *
 * and the Tangent class in JPT (http://www.ccs.neu.edu/jpt/jpt_2_3/index.htm)
 *
 */

GpPointF *
convert_points (const GpPoint *point, int count)
{
        int i;
        GpPointF *retval = (GpPointF *) GdipAlloc (sizeof (GpPointF) * count);
	if (!retval)
		return NULL;

        for (i = 0; i < count; i++) {
                retval [i].X = (float) point [i].X;
                retval [i].Y = (float) point [i].Y;
        }

        return retval;
}

GpPointF *
gdip_open_curve_tangents (int terms, const GpPointF *points, int count, float tension)
{
        float coefficient = tension / 3.0;
        int i;

        GpPointF *tangents = (GpPointF *) GdipAlloc (sizeof (GpPointF) * count);
	if (!tangents)
		return NULL;

        /* initialize everything to zero to begin with */
        for (i = 0; i < count; i++) {
                tangents [i].X = 0;
                tangents [i].Y = 0;
        }

        if (count <= 2)
                return tangents;

        for (i = 0; i < count; i++) {
                int r = i + 1;
                int s = i - 1;

                if (r >= count) r = count - 1;
                if (s < 0) s = 0;

                tangents [i].X += (coefficient * (points [r].X - points [s].X));
                tangents [i].Y += (coefficient * (points [r].Y - points [s].Y));
        }

        return tangents;        
}

GpPointF *
gdip_closed_curve_tangents (int terms, const GpPointF *points, int count, float tension)
{
        float coefficient = tension / 3.0;
        int i;
        GpPointF *tangents = (GpPointF *) GdipAlloc (sizeof (GpPointF) * count);
	if (!tangents)
		return NULL;

        /* initialize everything to zero to begin with */
        for (i = 0; i < count; i++) {
                tangents [i].X = 0;
                tangents [i].Y = 0;
        }

        if (count <= 2)
                return tangents;

        for (i = 0; i < count; i++) {
                int r = i + 1;
                int s = i - 1;

                if (r >= count) r -= count;
                if (s < 0) s += count;
		
                tangents [i].X += (coefficient * (points [r].X - points [s].X));
                tangents [i].Y += (coefficient * (points [r].Y - points [s].Y));
        }

        return tangents;
}

/* re-enabled if/when required */
#if FALSE
/* this function comes from fcmp.sf.net */
int
fcmp (double x1, double x2, double epsilon)
{
        int exponent;
        double delta;
        double difference;
  
        /* Get exponent(max(fabs(x1), fabs(x2))) and store it in exponent. */

        /* If neither x1 nor x2 is 0, */
        /* this is equivalent to max(exponent(x1), exponent(x2)). */

        /* If either x1 or x2 is 0, its exponent returned by frexp would be 0, */
        /* which is much larger than the exponents of numbers close to 0 in */
        /* magnitude. But the exponent of 0 should be less than any number */
        /* whose magnitude is greater than 0. */
  
        /* So we only want to set exponent to 0 if both x1 and */
        /* x2 are 0. Hence, the following works for all x1 and x2. */

        frexp (fabs (x1) > fabs (x2) ? x1 : x2, &exponent);

        /* Do the comparison. */

        /* delta = epsilon * pow(2, exponent) */

        /* Form a neighborhood around x2 of size delta in either direction. */
        /* If x1 is within this delta neighborhood of x2, x1 == x2. */
        /* Otherwise x1 > x2 or x1 < x2, depending on which side of */
        /* the neighborhood x1 is on. */
  
        delta = ldexp (epsilon, exponent); 
  
        difference = x1 - x2;

        if (difference > delta)
                return 1; /* x1 > x2 */
        else if (difference < -delta) 
                return -1;  /* x1 < x2 */
        else /* -delta <= difference <= delta */
                return 0;  /* x1 == x2 */
}
#endif

/* note: round[f] is C99 */
int
iround (float d)
{
	double int_part = floor (d);
	return ((d - int_part) >= 0.5) ? int_part + 1.0 : int_part;
}

/* This function is used by gradient brushes for calculating the erf required for 
 * calculating the integral of the normal distribution equation.
 */
float
gdip_erf (float x, float std, float mean)
{
	/* std refers to standard deviation
	 *
	 * ERF: http://mathworld.wolfram.com/Erf.html
	 * 
	 * erf (z) = (2 / sqrt (pi)) * infinite sum of [(pow (-1, n) * pow (z, 2n+1))/(n! * (2n+1))]
	 * using Maclaurin series.
	 */
	float series;
	float constant = 2.0 / sqrt (PI);
	float z = (x - mean) / std;
	z /= sqrt (2.0);

	series = (z
		  - pow (z, 3) / 3.0
		  + pow (z, 5) / 10.0
		  - pow (z, 7) / 42.0
		  + pow (z, 9) / 216.0
		  - pow (z, 11) / 1420.0);

	return constant * series;
}

/*
 convert a ucs2 string to utf8
 length = number of characters to convert, -1 to indicate the whole string
*/

gchar *
ucs2_to_utf8(const gunichar2 *ucs2, int length) {
	const gunichar2	*ptr;
	const gunichar2	*end;
	gunichar	*dest;
	gunichar	*uni;
	gchar		*utf8;

	/* Count length */
	if (length == -1) {
		ptr = ucs2;
		length = 0;
		while (*ptr != 0) {
			ptr++;
			length++;
		}
	}


	uni = GdipAlloc((length + 1) * sizeof(gunichar));
	if (uni == NULL) {
		return NULL;
	}

	dest = uni;
	ptr = ucs2;
	end = ptr + length;
	while (ptr != end) {
		if (*ptr < 0xd800 || *ptr >= 0xe000) {
			*dest = *ptr;
			dest++;
		}
		ptr++;
	}
	*dest = 0;
	dest++;
	
	utf8 = (gchar *) g_ucs4_to_utf8 ((const gunichar *)uni, -1, NULL, NULL, NULL);

	GdipFree(uni);

	return utf8;
}

BOOL
utf8_to_ucs2(const gchar *utf8, gunichar2 *ucs2, int ucs2_len) {
	int 		i;
	glong		items_read;
	glong		count;
	gunichar	*ucs4;
	gunichar2	*ptr;

	items_read = 0;
	count = 0;

	ucs2_len--;	/* Space for null terminator */

	ucs4 = g_utf8_to_ucs4(utf8, -1, &items_read, &count, NULL);
	if (ucs4 == NULL) {
		return FALSE;
	}

	ptr = (gunichar2 *)ucs2;
	for (i = 0; (i < count) && (i < ucs2_len); i++) {
		if (ucs4[i] < 0x10000 && !(ucs4[i] >= 0xd800 && ucs4[i] < 0xe000)) {
			*ptr = (gunichar2)ucs4[i];
			ptr++;
		}	/* we're simply ignoring any chars that don't fit into ucs2 */
	}
	ucs2[i] = 0;	/* terminate */

	/* free the intermediate ucs4 string */
	GdipFree(ucs4);

	return TRUE;
}

int
utf8_encode_ucs2char(gunichar2 unichar, BYTE *dest)
{
	if (unichar < 0x0080) {					/* 0000-007F */
		dest[0] = (BYTE)(unichar);
		return (1);
	}
	if(unichar < 0x0800) {					/* 0080-07FF */
		dest[0] = (BYTE)(0xC0 | ((unichar & 0x07C0) >> 6));
		dest[1] = (BYTE)(0x80 | (unichar & 0x003F));
		return (2);
	}
								/* 0800-FFFF */
	dest[0] = (BYTE)(0xE0 | ((unichar & 0xF000) >> 12));
	dest[1] = (BYTE)(0x80 | ((unichar & 0x0FC0) >> 6));
	dest[2] = (BYTE)(0x80 | (unichar & 0x003F));
	return (3);	
}

/* re-enabled if/when required */
#if FALSE
/* This function only handles UCS-2 */
int
utf8_decode_ucs2char (const BYTE *src, gunichar2 *uchar)
{
	if (src[0] <= 0x7F) {			/* 0000-007F: one byte (0xxxxxxx) */
		*uchar = (gunichar2)src[0];
		return (1);
	}
	if (src[0] <= 0xDF) {			/* 0080-07FF: two bytes (110xxxxx 10xxxxxx) */
		*uchar = ((((gunichar2)src[0]) & 0x001F) << 6) |
			((((gunichar2)src[1]) & 0x003F) << 0);
		return (2);
	}
						/* 0800-FFFF: three bytes (1110xxxx 10xxxxxx 10xxxxxx) */
	*uchar = ((((gunichar2)src[0]) & 0x000F) << 12) |
		((((gunichar2)src[1]) & 0x003F) << 6) |
		((((gunichar2)src[2]) & 0x003F) << 0);
	return (3);
}
#endif

GpStatus
gdip_get_pattern_status (cairo_pattern_t *pat)
{
	if (!pat)
		return OutOfMemory;

	switch (cairo_pattern_status (pat)) {
	case CAIRO_STATUS_SUCCESS:
		return Ok;
	case CAIRO_STATUS_NO_MEMORY:
		cairo_pattern_destroy (pat);
		return OutOfMemory;
	default:
		cairo_pattern_destroy (pat);
		return InvalidParameter;
	}
}

/*
 * Our anti-aliasing hack (adding an X and Y offset when drawing) cannot work if the current matrix 
 * scales the coordinates. E.g. 0 scale 100 == 0, while (0 + 0.5) scale 100 == 50.
 */

BOOL
gdip_is_scaled (GpGraphics *graphics)
{
	cairo_matrix_t matrix;
	cairo_get_matrix (graphics->ct, &matrix);
	return ((matrix.xx != 1.0f) || (matrix.yy != 1.0f));
}

/* cairo has a (signed) 15(1)/16(2)bits pixel positioning, while GDI+ use (signed) 23 bits (infinity).
 * Using larger values confuse the bits used for subpixel positioning.
 * (1) http://lists.freedesktop.org/archives/cairo/2006-June/007251.html
 * (2) testing shows artefacts if using more than 15 *signed* bits (i.e. -16384 to 16383)
 */

void
gdip_cairo_rectangle (GpGraphics *graphics, double x, double y, double width, double height, BOOL antialiasing)
{
	double x2, y2;

	/* avoid unit conversion whenever possible */
	if (!OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}

	/* apply antialiasing offset (if required and if no scaling is in effect) */
	if (antialiasing && !gdip_is_scaled (graphics)) {
		x += graphics->aa_offset_x;
		y += graphics->aa_offset_y;
	}

	/* ensure that each point (x,y and x+width,y+height) are within the 16 bits bounds */
	x2 = x + width;
	y2 = y + height;

	/* put everything between cairo limits */
	x = CAIRO_LIMIT (x);
	y = CAIRO_LIMIT (y);
	x2 = CAIRO_LIMIT (x2);
	y2 = CAIRO_LIMIT (y2);

	/* and recompute the final width and length */
	cairo_rectangle (graphics->ct, x, y, (x2 - x), (y2 - y));
}

void
gdip_cairo_move_to (GpGraphics *graphics, double x, double y, BOOL convert_units, BOOL antialiasing)
{
	/* avoid unit conversion whenever possible */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
	}

	/* apply antialiasing offset (if required and if no scaling is in effect) */
	if (antialiasing && !gdip_is_scaled (graphics)) {
		/* note that we can't add AA to a 0 position (or scaling will not work) */
		x += graphics->aa_offset_x;
		y += graphics->aa_offset_y;
	}

	/* put everything between cairo limits */
	x = CAIRO_LIMIT (x);
	y = CAIRO_LIMIT (y);

        cairo_move_to (graphics->ct, x, y);
}

void
gdip_cairo_line_to (GpGraphics *graphics, double x, double y, BOOL convert_units, BOOL antialiasing)
{
	/* avoid unit conversion whenever possible */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
	}

	/* apply antialiasing offset (if required and if no scaling is in effect) */
	if (antialiasing && !gdip_is_scaled (graphics)) {
		x += graphics->aa_offset_x;
		y += graphics->aa_offset_y;
	}

	/* put everything between cairo limits */
	x = CAIRO_LIMIT (x);
	y = CAIRO_LIMIT (y);

        cairo_line_to (graphics->ct, x, y);
}

void
gdip_cairo_curve_to (GpGraphics *graphics, double x1, double y1, double x2, double y2, double x3, double y3, 
	BOOL convert_units, BOOL antialiasing)
{
	/* avoid unit conversion whenever possible */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x1 = gdip_unitx_convgr (graphics, x1);
		y1 = gdip_unity_convgr (graphics, y1);
		x2 = gdip_unitx_convgr (graphics, x2);
		y2 = gdip_unity_convgr (graphics, y2);
		x3 = gdip_unitx_convgr (graphics, x3);
		y3 = gdip_unity_convgr (graphics, y3);
	}

	/* apply antialiasing offset (if required and if no scaling is in effect) */
	if (antialiasing && !gdip_is_scaled (graphics)) {
		x1 += graphics->aa_offset_x;
		y1 += graphics->aa_offset_y;
		x2 += graphics->aa_offset_x;
		y2 += graphics->aa_offset_y;
		x3 += graphics->aa_offset_x;
		y3 += graphics->aa_offset_y;
	}

	/* put everything between cairo limits */
	x1 = CAIRO_LIMIT (x1);
	y1 = CAIRO_LIMIT (y1);
	x2 = CAIRO_LIMIT (x2);
	y2 = CAIRO_LIMIT (y2);
	x3 = CAIRO_LIMIT (x3);
	y3 = CAIRO_LIMIT (y3);

	cairo_curve_to (graphics->ct, x1, y1, x2, y2, x3, y3);
}
