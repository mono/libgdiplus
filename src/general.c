/*
 * general.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
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
 */

#include "gdip.h"
#include <dlfcn.h>
#include <cairo.h>
#include <math.h>

//#define DEBUG_MEMLEAKS	1

/* Startup / shutdown */

struct startupInput
{
	unsigned int version;             
	void       * ptr; 
	int          threadOpt;
	int          codecOpt;
};


struct startupOutput
{
	void *hook;
	void *unhook;
};

extern void gdip_release_cachedfonts ();

static GList* g_mem_allocations;

GpStatus 
GdiplusStartup(unsigned long *token, const struct startupInput *input, struct startupOutput *output)
{
	g_mem_allocations = NULL;
        initCodecList (); 
	*token = 1;
	gdip_get_display_dpi();
	return Ok;
}

void 
GdiplusShutdown(unsigned long *token)
{
#ifdef DEBUG_MEMLEAKS
	GList* list = NULL;
#endif
	releaseCodecList ();
	gdip_release_cachedfonts ();

#ifdef DEBUG_MEMLEAKS
	for (list = g_list_first (g_mem_allocations); list != NULL; list = g_list_next (list)) {
		printf ("Memory block not free'd at %x\n", list->data);
	}
#endif
}


/* Memory */
void *
GdipAlloc (int size)
{
#ifdef DEBUG_MEMLEAKS		
	void* block = malloc (size);
	g_mem_allocations =  g_list_append (g_mem_allocations, block);
	printf ("alloc %x %u\n", block, size);
	return block;
#else
	return malloc (size);
#endif
}

void *
GdipCalloc (size_t nelem, size_t elsize)
{
#ifdef DEBUG_MEMLEAKS		
	void* block = calloc (nelem, elsize);
	g_mem_allocations =  g_list_append (g_mem_allocations, block);
	printf ("calloc %x %u\n", block, nelem);
	return block;
#else
	return calloc (nelem, elsize);
#endif

}

void 
GdipFree (void *ptr)
{
#ifdef DEBUG_MEMLEAKS	
	GList* list = NULL;

	list = g_list_find (g_list_first (g_mem_allocations), ptr);
	if (list != NULL) {
		list  = g_list_delete_link  (g_list_first (g_mem_allocations), list);
		g_mem_allocations = g_list_last (list);
	}

	printf ("free %x\n", ptr);
	free (ptr);
#else
	free (ptr);
#endif
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
                //case CAIRO_STATUS_NO_TARGET_SURFACE:
                        return WrongState;

                default:
                        return GenericError;
                }
        }
}

/* Converts the given interpolation value to cairo_filter_t */
cairo_filter_t
gdip_get_cairo_filter (InterpolationMode imode)
{
	cairo_filter_t filter;

	switch (imode) {

	case InterpolationModeHighQuality:
	case InterpolationModeHighQualityBilinear:
	case InterpolationModeHighQualityBicubic:
		filter = CAIRO_FILTER_BEST;
		break;

	case InterpolationModeNearestNeighbor:
		filter = CAIRO_FILTER_NEAREST;
		break;

	case InterpolationModeBilinear:
		filter = CAIRO_FILTER_BILINEAR;
		break;

	case InterpolationModeBicubic:
		filter = CAIRO_FILTER_GAUSSIAN;
		break;
		
	case InterpolationModeLowQuality:
		filter = CAIRO_FILTER_FAST;
		break;

	case InterpolationModeDefault:
	default:
		filter = CAIRO_FILTER_GOOD;
		break;
	}

	return filter;
}

float
gdip_get_display_dpi()
{
	static float dpis = 0;
	Display* display;

	if (dpis == 0) {
		char *val;

		display = XOpenDisplay (0);
		/* If the display is openable lets try to read dpi from it; otherwise use a default of 96.0f */
		if (display) {
			val = XGetDefault (display, "Xft", "dpi");
			XCloseDisplay (display);
			if (val) {
				dpis = atof (val);
			} else {
				dpis = 96.0f;
			}
		} else {
			dpis = 96.0f;
		}
	}

	return dpis;
}

void gdip_unitConversion(Unit fromUnit, Unit toUnit, float nSrc, float* nTrg)
{
    float inchs = 0;
    *nTrg = 0;

    switch (fromUnit) {
    case UnitDisplay:
      inchs = nSrc / 75.0f;
      break;
    case UnitDocument:
      inchs = nSrc / 300.0f;
      break;
    case UnitInch:
      inchs = nSrc;
      break;
    case UnitMillimeter:
      inchs = nSrc / 25.4f;
      break;
    case UnitPixel:
    case UnitWorld:
      inchs = nSrc / gdip_get_display_dpi();
      break;
    case UnitPoint:
      inchs = nSrc / 72.0f;
      break;
    default:
      break;
    }

    switch (toUnit) {
    case UnitDisplay:
      *nTrg = inchs * 75.0f;
      break;
    case UnitDocument:
      *nTrg = inchs * 300.0f;
      break;
    case UnitInch:
      *nTrg = inchs;
      break;
    case UnitMillimeter:
      *nTrg = inchs * 25.4f;
      break;
    case UnitPixel:
    case UnitWorld:
      *nTrg = inchs * gdip_get_display_dpi();
      break;
    case UnitPoint:
      *nTrg = inchs * 72.0f;
      break;
    default:
      break;
    }

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

        /* initialize everything to zero to begin with */
        for (i = 0; i < count; i++) {
                tangents [i].X = 0;
                tangents [i].Y = 0;
        }

        if (count <= 2)
                return tangents;

        for (i = 1; i < count - 1; i++) {
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

cairo_status_t
gdip_cairo_set_surface_pattern (cairo_t *t, cairo_surface_t *s)
{
    cairo_pattern_t *pat;
    pat = cairo_pattern_create_for_surface (s);
    if (pat == NULL)
	return CAIRO_STATUS_NO_MEMORY;
    cairo_set_source (t, pat);
    cairo_pattern_destroy (pat);

    return cairo_status (t);
}

void
gdip_rect_expand_by (GpRectF *rect, GpPointF *point)
{
    /* This method is somewhat stupid, because GpRect is x,y width,height,
     * instead of x0,y0 x1,y1.
     */
    float x0 = rect->X;
    float y0 = rect->Y;
    float x1 = x0 + rect->Width;
    float y1 = y0 + rect->Height;

    if (point->X < x0)
        x0 = point->X;
    else if (point->X > x1)
        x1 = point->X;

    if (point->Y < y0)
        y0 = point->Y;
    else if (point->Y > y1)
        y1 = point->Y;

    rect->X = x0;
    rect->Y = y0;
    rect->Width = (x1 - x0);
    rect->Height = (y1 - y0);
}

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

/* This function is used by gradient brushes for calculating the erf required for 
 * calculating the integral of the normal distribution equation.
 */
float gdip_erf (float x, float std, float mean)
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
	unsigned int	ch;
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

bool
utf8_to_ucs2(const gchar *utf8, gunichar2 *ucs2, int ucs2_len) {
	int 		i;
	glong		items_read;
	glong		count;
	gunichar	*ucs4;
	unsigned char	*ptr;

	items_read = 0;
	count = 0;

	ucs2_len--;	/* Space for null terminator */

	ucs4 = g_utf8_to_ucs4(utf8, -1, &items_read, &count, NULL);
	if (ucs4 == NULL) {
		return FALSE;
	}

	ptr = (unsigned char *)ucs2;
	for (i = 0; (i < count) && (i < ucs2_len); i++) {
		if (ucs4[i] < 0x1000 && !(ucs4[i] >= 0xd800 && ucs4[i] < 0xe000)) {
			ptr[0] = (unsigned char)ucs4[i];
			ptr[1] = (unsigned char)(ucs4[i] >> 8);
			ptr += 2;
		}	/* we're simply ignoring any chars that don't fit into ucs2 */
	}
	ucs2[ptr - (unsigned char *)ucs2] = 0;	/* terminate */

	/* free the intermediate ucs4 string */
	GdipFree(ucs4);

	return TRUE;
}

int
utf8_encode_ucs2char(gunichar2 unichar, unsigned char *dest)
{
	if (unichar < 0x0080) {					/* 0000-007F */
		dest[0] = (unsigned char)(unichar);
		return (1);
	}
	if(unichar < 0x0800) {					/* 0080-07FF */
		dest[0] = (unsigned char)(0xC0 | ((unichar & 0x07C0) >> 6));
		dest[1] = (unsigned char)(0x80 | (unichar & 0x003F));
		return (2);
	}
								/* 0800-FFFF */
	dest[0] = (unsigned char)(0xE0 | ((unichar & 0xF000) >> 12));
	dest[1] = (unsigned char)(0x80 | ((unichar & 0x0FC0) >> 6));
	dest[2] = (unsigned char)(0x80 | (unichar & 0x003F));
	return (3);	
}

/* This function only handles UCS-2 */
int
utf8_decode_ucs2char(const unsigned char *src, gunichar2 *uchar)
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

cairo_content_t
from_cairoformat_to_content (cairo_format_t format)
{
    	switch (format) {
    	case CAIRO_FORMAT_RGB24:
		return CAIRO_CONTENT_COLOR;
	case CAIRO_FORMAT_A8:
		return CAIRO_CONTENT_ALPHA;
	case CAIRO_FORMAT_ARGB32:
	default:
		return CAIRO_CONTENT_COLOR_ALPHA;
    }
}

