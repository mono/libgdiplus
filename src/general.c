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
 *   Frederik Carlier <frederik.carlier@quamotion.mobi>
 */

#include "general-private.h"
#include "codecs-private.h"
#include "graphics-private.h"
#include "font-private.h"
#include "stringformat-private.h"
#include "carbon-private.h"
#ifdef WIN32
#include "win32-private.h"
#endif

/* large table to avoid a division and three multiplications when premultiplying alpha into R, G and B */
#include "alpha-premul-table.inc"

BOOL gdiplusInitialized = FALSE;
static BOOL suppressBackgroundThread = FALSE;

GpStatus WINGDIPAPI
GdiplusStartup (ULONG_PTR *token, const GdiplusStartupInput *input, GdiplusStartupOutput *output)
{
	GpStatus status;

	if (!token || !input)
		return InvalidParameter;
	if (input->SuppressBackgroundThread && !output)
		return InvalidParameter;
	if (input->GdiplusVersion != 1 && input->GdiplusVersion != 2)
		return UnsupportedGdiplusVersion;

	/* Don't initialize multiple time, e.g. for each appdomain. */
	if (gdiplusInitialized)
		return Ok;
	
	gdiplusInitialized = TRUE;

	status = initCodecList ();
	if (status != Ok)
		return status;

	FcInit ();

	/* A fontconfig instance which didn't find a configfile is unbelievably
		cranky, so let's just write out a small xml file and make fontconfig
		happy */
	if (FcConfigFilename (0) == NULL)
	{
		/* Newer versions of font-config have FcConfigParseAndLoadFromMemory,
		   which we could use to avoid generating a temporary file. But meanwhile,
		   we are stuck with this workaround. */
		char namebuf[512];
#ifdef WIN32
		FILE *fi = CreateTempFile (namebuf);
#else
		strcpy ((char *) namebuf, "/tmp/ffXXXXXX");
		int fd = mkstemp ((char *) namebuf);
		FILE *fi = fdopen (fd, "wb");
#endif

		if (fi) {
			fprintf (fi, "<?xml version=\"1.0\"?>\n");
			fprintf (fi, "<fontconfig>\n");
#if defined(WIN32)
			fprintf (fi, "<dir>WINDOWSFONTDIR</dir>\n");
#elif defined(__APPLE__)
			fprintf (fi, "<dir>/System/Library/Fonts</dir>\n");
#else
			fprintf (fi, "<dir>~/.fonts</dir>\n");
#endif

#if defined(WIN32)
			fprintf (fi, "<cachedir>WINDOWSTEMPDIR_FONTCONFIG_CACHE</cachedir>\n");
#else
			fprintf (fi, "<cachedir>~/.fontconfig</cachedir>\n");
#endif
			fprintf (fi, "</fontconfig>\n");
			fclose (fi);

			FcConfig* c = FcConfigCreate ();
			FcConfigParseAndLoad (c, (FcChar8*)namebuf, 1);
			remove (namebuf);

			FcConfigBuildFonts (c);
			FcConfigSetCurrent (c);

			// FcConfig is reference-counted, so it's OK to call destroy here.
			FcConfigDestroy (c);
		}
	}

	gdip_get_display_dpi();
	gdip_create_generic_stringformats ();

	if (input->SuppressBackgroundThread) {
		output->NotificationHook = GdiplusNotificationHook;
		output->NotificationUnhook = GdiplusNotificationUnhook;
	}

	*token = 1;
	suppressBackgroundThread = input->SuppressBackgroundThread;

	return Ok;
}

void
WINGDIPAPI GdiplusShutdown (ULONG_PTR token)
{
	if (gdiplusInitialized) {
		releaseCodecList ();
		gdip_font_clear_pattern_cache ();
		gdip_delete_system_fonts ();
		gdip_delete_generic_stringformats ();
#if HAVE_FCFINI
		FcFini ();
#endif
		gdiplusInitialized = FALSE; /* in case we want to restart it */
		suppressBackgroundThread = FALSE;
	}
}


/* Memory */
WINGDIPAPI void *
GdipAlloc (size_t size)
{
	if (!gdiplusInitialized)
		return NULL;

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

WINGDIPAPI void
GdipFree (void *ptr)
{
	free (ptr);
}

GpStatus WINGDIPAPI
GdiplusNotificationHook (ULONG_PTR *token)
{
	if (!suppressBackgroundThread)
		return GenericError;
	if (!token)
		return InvalidParameter;

	/* Initialize the token with a dummy value. */
	*token = 1;

	return Ok;
}

void WINGDIPAPI
GdiplusNotificationUnhook (ULONG_PTR token)
{
	/* Does nothing in libgdiplus. */
}

/* libgdiplus-specific API */
WINGDIPAPI char*
GetLibgdiplusVersion ()
{
	return g_strdup (VERSION);
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

	if (dpis == 0) {
#if __APPLE__
		float h_dpi, v_dpi;
		gdip_get_display_dpi_carbon (&h_dpi, &v_dpi);

		dpis = h_dpi;
		return dpis;
#elif defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
		char *val;

		Display* display;
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
#elif WIN32
		dpis = gdip_get_display_dpi_win32 ();
#else
		dpis = 96.0f;
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
	if (!point || count < 0)
		return NULL;

	GpPointF *retval = (GpPointF *) GdipAlloc (sizeof (GpPointF) * count);
	if (!retval)
		return NULL;

	for (int i = 0; i < count; i++)
		gdip_PointF_from_Point (&point[i], &retval[i]);

	return retval;
}

GpRectF *
convert_rects (const GpRect *rect, int count)
{
	if (!rect || count < 0)
		return NULL;

	GpRectF *retval = (GpRectF *) GdipAlloc (sizeof (GpRectF) * count);
	if (!retval)
		return NULL;

	for (int i = 0; i < count; i++)
		gdip_RectF_from_Rect (&rect[i], &retval[i]);

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
 convert a utf16 string to utf8
 length = number of characters to convert, -1 to indicate the whole string
*/

gchar *
utf16_to_utf8(const gunichar2 *ucs2, int length) {
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
		} else if (ptr + 1 != end && ptr[1] < 0xe000 && ptr[1] >= 0xdc00) {
			/* UTF-16 support: Convert high and low surrogate to 32-bit code. */
			*dest++ = ((gunichar)ptr[0] - 0xd800) * 0x400 + ((gunichar)ptr[1] - 0xdc00) + 0x10000;
			ptr++;
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

void
gdip_cairo_set_matrix (GpGraphics *graphics, GpMatrix *matrixPageUnits)
{
	float x0 = matrixPageUnits->x0;
	float y0 = matrixPageUnits->y0;

	/* avoid unit conversion whenever possible */
	if (!OPTIMIZE_CONVERSION (graphics)) {
		x0 = gdip_unitx_convgr (graphics, x0);
		y0 = gdip_unity_convgr (graphics, y0);
	}

	/* do not apply antialiasing trick to transformation matrix */

	/* put everything between cairo limits */
	x0 = CAIRO_LIMIT (x0);
	y0 = CAIRO_LIMIT (y0);

	GpMatrix matrixCopy;
	gdip_cairo_matrix_copy (&matrixCopy, matrixPageUnits);
	matrixCopy.x0 = x0;
	matrixCopy.y0 = y0;

	cairo_set_matrix (graphics->ct, &matrixCopy);
}

void gdip_RectF_from_Rect (const GpRect* rect, GpRectF* rectf)
{
	rectf->X = rect->X;
	rectf->Y = rect->Y;
	rectf->Width = rect->Width;
	rectf->Height = rect->Height;
}

void gdip_Rect_from_RectF (const GpRectF* rectf, GpRect* rect)
{
	rect->X = iround (rectf->X);
	rect->Y = iround (rectf->Y);
	rect->Width = iround (rectf->Width);
	rect->Height = iround (rectf->Height);
}

void gdip_PointF_from_Point (const GpPoint* point, GpPointF* pointf)
{
	pointf->X = point->X;
	pointf->Y = point->Y;
}

void gdip_Point_from_PointF (const GpPointF* pointf, GpPoint* point)
{
	point->X = iround (pointf->X);
	point->Y = iround (pointf->Y);
}

/* Pre-process negative width and height, without modifying the originals */
void
gdip_normalize_rectangle (const GpRectF *rect, GpRectF *normalized)
{
	float width = rect->Width;
	float height = rect->Height;

	if (width < 0) {
		normalized->X = rect->X + width;
		normalized->Width = fabs (width);
	} else {
		normalized->X = rect->X;
		normalized->Width = width;
	}

	if (rect->Height < 0) {
		normalized->Y = rect->Y + height;
		normalized->Height = fabs (height);
	} else {
		normalized->Y = rect->Y;
		normalized->Height = height;
	}
}

BOOL
gdip_is_rectF_empty (const GpRectF *rect, BOOL allowNegative)
{
	if (!rect)
		return FALSE;

	if (rect->Width == 0 || rect->Height == 0)
		return TRUE;

	return allowNegative && (rect->Width < 0 || rect->Height < 0);
}


static DWORD crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

DWORD
gdip_crc32 (const BYTE *buf, size_t size)
{
	// This is a modified version of the implementation of crc32 in the Darwin kernel.
	// The original code can be found at https://opensource.apple.com/source/xnu/xnu-792.13.8/bsd/libkern/crc32.c
	// The code has been modified to match GDI+ by setting the initial value to 0 and by returning
	// crc, instead of ~crc.
	DWORD crc = 0;
	while (size--)
		crc = crc32_tab[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

	return crc;
}
