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

static void * x11drvHandle = 0;

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
	printf ("****************** Starting up\n");
	GDIP_display = _get_wine_display ();
	if (GDIP_display == 0){
		GDIP_display = XOpenDisplay(0);
		closeDisplay = 1;
	}

	/* printf ("GdiplusStartup. GDIP_Display %p\n", GDIP_display); */
	initializeGdipWin32 ();
	*token = 1;
	return Ok;
}

void 
GdiplusShutdown(unsigned long *token)
{
	if (closeDisplay) {
		XCloseDisplay(GDIP_display);
	}
	_unload_x11drv ();
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
	float dpis = 72;    /* We just do not want to return 0 ever */
	
	Display* display=XOpenDisplay(NULL);
	char* val = XGetDefault(display, "Xft", "dpi");
   	XCloseDisplay(display);
	if (val) {
		dpis = atof(val);
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
_cairo_matrix_compute_scale_factors (cairo_matrix_t *matrix, double *sx, double *sy)
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

    _cairo_matrix_compute_scale_factors (matrix, &scale_x, &scale_y);

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
gdpi_utf8_to_glyphs (cairo_ft_font_t	*font,
		 const unsigned char	*utf8,
		 double			x0,
		 double			y0,
		 cairo_glyph_t		**glyphs,
		 size_t			*nglyphs)
{
    FT_Face face = font->face;
    double x = 0., y = 0.;
    size_t i;
    FT_ULong *ucs4 = NULL;

    ucs4 = (FT_ULong *)g_utf8_to_ucs4 (utf8, (glong)-1, NULL, (glong *)nglyphs, NULL);

    if (ucs4 == NULL)
        return 0;

    *glyphs = (cairo_glyph_t *) malloc ((*nglyphs) * (sizeof (cairo_glyph_t)));
    if (*glyphs == NULL)
    {
        g_free (ucs4);
        return 0;
    }

    _install_font_matrix (&font->base.matrix, face);

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

