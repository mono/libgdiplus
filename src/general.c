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

Display *GDIP_display = 0;
int      closeDisplay = 0;

void * x11drvHandle = 0;

static void _load_x11drv ()
{
	if (x11drvHandle == 0) {
		char *path = getenv ("_WINE_SHAREDLIB_PATH");
		char *full;

		/* We are called also without Wine */
		if (path == NULL)
			return;

		full = malloc (strlen (path) + 30);
		strcpy (full, path);
		strcat (full, "/");
		strcat (full, "x11drv.dll.so");
		
		x11drvHandle = dlopen (full, 1);
		free (full);
	}
}

static void _unload_x11drv ()
{
	if (x11drvHandle != 0) {
		dlclose (x11drvHandle);
	}
}

Display *_get_wine_display ()
{
	Display * result = 0;
	_load_x11drv ();
	if (x11drvHandle != 0) {
		Display **addr = dlsym(x11drvHandle,"gdi_display");
		if (addr) {
			result = *addr;
		}
	}
	return result;
}


GpStatus 
GdiplusStartup(unsigned long *token, const struct startupInput *input, struct startupOutput *output)
{
    if (getenv ("GDIPLUS_NOX") == NULL) {
        GDIP_display = _get_wine_display ();
        if (GDIP_display == 0){
            GDIP_display = XOpenDisplay(0);
            closeDisplay = 1;
        }
    } else {
        GDIP_display = 0;
    }

        initCodecList (); 
        initializeGdipWin32 ();
	*token = 1;
	return Ok;
}

void 
GdiplusShutdown(unsigned long *token)
{
	if (closeDisplay && GDIP_display) {
		XCloseDisplay(GDIP_display);
	}
	_unload_x11drv ();
        releaseCodecList ();
	shutdownGdipWin32();
}


/* Memory */
void *
GdipAlloc (int size)
{
	return malloc (size);
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
                case CAIRO_STATUS_NO_TARGET_SURFACE:
                        return WrongState;

                default:
                        return GenericError;
                }
        }
}

float
gdip_get_display_dpi()
{
    static float dpis = 0;

    if (dpis == 0) {
        if (GDIP_display == NULL) {
            dpis = 72.0f;
        } else {
            char *val = XGetDefault(GDIP_display, "Xft", "dpi");
            if (val) {
                dpis = atof(val);
            } else {
		dpis = 72.0f;
	    }
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




/*
    Functionality to be replaced by Cairo when available from
    the standard API
*/

/* Compute the amount that each basis vector is scaled by. */
cairo_status_t
_gdip_cairo_matrix_compute_scale_factors (cairo_matrix_t *matrix, double *sx, double *sy)
{
    double x, y;

    x = 1.0;
    y = 0.0;
    cairo_matrix_transform_distance (matrix, &x, &y);
    *sx = sqrt(x*x + y*y);

    x = 0.0;
    y = 1.0;
    cairo_matrix_transform_distance (matrix, &x, &y);
    *sy = sqrt(x*x + y*y);

    return CAIRO_STATUS_SUCCESS;
}

static void
_install_font_matrix(cairo_matrix_t *matrix, FT_Face face)
{
    cairo_matrix_t normalized;
    double scale_x, scale_y;
    double xx, xy, yx, yy, tx, ty;
    FT_Matrix mat;

    _gdip_cairo_matrix_compute_scale_factors (matrix, &scale_x, &scale_y);

    cairo_matrix_copy (&normalized, matrix);

    cairo_matrix_scale (&normalized, 1.0 / scale_x, 1.0 / scale_y);
    cairo_matrix_get_affine (&normalized,
                             &xx /* 00 */ , &yx /* 01 */,
                             &xy /* 10 */, &yy /* 11 */,
                             &tx, &ty);

    mat.xx = DOUBLE_TO_16_16(xx);
    mat.xy = -DOUBLE_TO_16_16(xy);
    mat.yx = -DOUBLE_TO_16_16(yx);
    mat.yy = DOUBLE_TO_16_16(yy);

    FT_Set_Transform(face, &mat, NULL);
    FT_Set_Char_Size(face,
		     DOUBLE_TO_26_6(scale_x),
		     DOUBLE_TO_26_6(scale_y),
		     0, 0);
}

int
gdpi_utf8_to_glyphs (cairo_font_t	*font,
                 cairo_matrix_t		matrix,
		 const unsigned char	*utf8,
		 double			x0,
		 double			y0,
		 cairo_glyph_t		**glyphs,
		 size_t			*nglyphs)
{
    FT_Face face = cairo_ft_font_face(font);
    double		x = 0.0;
    double		y = 0.0;
    size_t		i;
    FT_ULong		*ucs4 = NULL;

    ucs4 = (FT_ULong *)g_utf8_to_ucs4 (utf8, (glong)-1, NULL, (glong *)nglyphs, NULL);

    if (ucs4 == NULL)
        return 0;

    *glyphs = (cairo_glyph_t *) malloc ((*nglyphs) * (sizeof (cairo_glyph_t)));
    if (*glyphs == NULL)
    {
        g_free (ucs4);
        return 0;
    }

    _install_font_matrix (&matrix, face);

    for (i = 0; i < *nglyphs; i++)
    {
        (*glyphs)[i].index = FT_Get_Char_Index (face, ucs4[i]);
        (*glyphs)[i].x = x0 + x;
        (*glyphs)[i].y = y0 + y;

        FT_Load_Glyph (face, (*glyphs)[i].index, FT_LOAD_DEFAULT);

        x += DOUBLE_FROM_26_6 (face->glyph->advance.x);
        y -= DOUBLE_FROM_26_6 (face->glyph->advance.y);
    }

    g_free (ucs4);
    return 1;
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
    cairo_set_pattern (t, pat);
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
