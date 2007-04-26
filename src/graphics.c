/*
 * graphics.c
 *
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
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
 *   Alexandre Pigolkine (pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "graphics-private.h"
#include "region-private.h"
#include "graphics-path-private.h"
#include "brush-private.h"
#include "matrix-private.h"
#include "bitmap-private.h"
#include "stringformat.h"

#define	NO_CAIRO_AA

#define MAX_GRAPHICS_STATE_STACK 512

float
gdip_unit_conversion (Unit from, Unit to, float dpi, GraphicsType type, float nSrc)
{	
	float inchs = 0;

	switch (from) {
	case UnitDocument:
      		inchs = nSrc / 300.0f;
      		break;
    	case UnitInch:
      		inchs = nSrc;
      		break;
    	case UnitMillimeter:
      		inchs = nSrc / 25.4f;
      		break;
	case UnitDisplay:
		if (type == gtPostScript) { /* Uses 1/100th on printers */
			inchs = nSrc / 100;
		} else { /* Pixel for video display */
			inchs = nSrc / dpi;
		}
		break;
	case UnitPixel:
    	case UnitWorld:
      		inchs = nSrc / dpi;
	      	break;
    	case UnitPoint:
      		inchs = nSrc / 72.0f;
      		break;
	case UnitCairoPoint:
		if (type == gtPostScript) { /* Uses 1/100th on printers */
			inchs = nSrc / 72.0f;
		} else { /* Pixel for video display */
			inchs = nSrc / dpi;
		}
		break;
    	default:
		return nSrc;
	}

	switch (to) {
	case UnitDocument:
		return inchs * 300.0f;
	case UnitInch:
		return inchs;
	case UnitMillimeter:
		return inchs * 25.4f;
	case UnitDisplay:
		if (type == gtPostScript) { /* Uses 1/100th on printers */
			return inchs * 100;
		} else { /* Pixel for video display */
			return inchs * dpi;
		}
	case UnitPixel:
	case UnitWorld:
		return inchs * dpi;
	case UnitPoint:
		return inchs * 72.0f;
	case UnitCairoPoint:
		if (type == gtPostScript) { /* Uses 1/100th on printers */
			return inchs * 72.0f;
		} else { /* Pixel for video display */
			return inchs * dpi;
		}
	default:
		return nSrc;
	}
}

static void
gdip_graphics_reset (GpGraphics *graphics)
{
	/* if required, previous_matrix will be assigned later (e.g. containers) */
	cairo_matrix_init_identity (&graphics->previous_matrix);
	GdipResetClip (graphics);
	cairo_matrix_init_identity (graphics->clip_matrix);
	graphics->page_unit = UnitDisplay;
	graphics->scale = 1.0f;
	graphics->interpolation = InterpolationModeBilinear;
	graphics->composite_quality = CompositingQualityDefault;
	graphics->composite_mode = CompositingModeSourceOver;
	graphics->text_mode = TextRenderingHintSystemDefault;
	graphics->pixel_mode = PixelOffsetModeDefault;
	graphics->text_contrast = DEFAULT_TEXT_CONTRAST;

	GdipSetSmoothingMode(graphics, SmoothingModeNone);
}

static void
gdip_graphics_init (GpGraphics *graphics, cairo_surface_t *surface)
{
	graphics->ct = cairo_create (surface);
	GdipCreateMatrix (&graphics->copy_of_ctm);
	cairo_matrix_init_identity (graphics->copy_of_ctm);
	cairo_identity_matrix (graphics->ct);

#ifndef NO_CAIRO_AA
        cairo_set_shape_format (graphics->ct, CAIRO_FORMAT_A1);
#endif
	graphics->image = 0;
	graphics->type = gtUndefined;
        /* cairo_select_font_face (graphics->ct, "serif:12"); */
	cairo_select_font_face (graphics->ct, "serif:12", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	GdipCreateRegion (&graphics->clip);
	GdipCreateMatrix (&graphics->clip_matrix);
	graphics->bounds.X = graphics->bounds.Y = graphics->bounds.Width = graphics->bounds.Height = 0;
	graphics->last_pen = NULL;
	graphics->last_brush = NULL;
	graphics->saved_status = NULL;
	graphics->saved_status_pos = 0;
	graphics->render_origin_x = 0;
	graphics->render_origin_y = 0;
	graphics->dpi_x = graphics->dpi_y = 0;

	gdip_graphics_reset (graphics);
}

GpGraphics *
gdip_graphics_new (cairo_surface_t *surface)
{
	GpGraphics *result = (GpGraphics *) GdipAlloc (sizeof (GpGraphics));

	if (result)
		gdip_graphics_init (result, surface);

	return result;
}

#define C1 0.552285
static void 
make_ellipse (GpGraphics *graphics, float x, float y, float width, float height, BOOL convert_units, BOOL antialiasing)
{
	double rx, ry, cx, cy;

	/* if required deal, once and for all, with unit conversion */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}

	rx = width / 2;
	ry = height / 2;
	cx = x + rx;
	cy = y + ry;

	/* if required deal, once and for all, with antialiasing */
	if (antialiasing) {
		cx += graphics->aa_offset_x;
		cy += graphics->aa_offset_y;
	}

	gdip_cairo_move_to (graphics, cx + rx, cy, FALSE, FALSE);

        /* an approximate of the ellipse by drawing a curve in each
         * quadrants */
	gdip_cairo_curve_to (graphics,
			cx + rx, cy - C1 * ry,
			cx + C1 * rx, cy - ry,
			cx, cy - ry,
			FALSE, FALSE);
        
	gdip_cairo_curve_to (graphics,
			cx - C1 * rx, cy - ry,
			cx - rx, cy - C1 * ry,
			cx - rx, cy,
			FALSE, FALSE);

	gdip_cairo_curve_to (graphics,
			cx - rx, cy + C1 * ry,
			cx - C1 * rx, cy + ry,
			cx, cy + ry,
			FALSE, FALSE);
                
	gdip_cairo_curve_to (graphics,
			cx + C1 * rx, cy + ry,
			cx + rx, cy + C1 * ry,
			cx + rx, cy,
			FALSE, FALSE);

        cairo_close_path (graphics->ct);
}

static void
make_polygon (GpGraphics *graphics, GDIPCONST GpPointF *points, int count, BOOL antialiasing)
{
        int i;

	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, antialiasing);

	for (i = 0; i < count; i++) {
		gdip_cairo_line_to (graphics, points [i].X, points [i].Y, TRUE, antialiasing);
	}

        /*
         * Draw a line from the last point back to the first point if
	 * they're not the same
         */
	if (points [0].X != points [count-1].X && points [0].Y != points [count-1].Y) {
		gdip_cairo_line_to (graphics, points [0].X, points [0].Y, TRUE, antialiasing);
	}

        cairo_close_path (graphics->ct);
}

static void
make_polygon_from_integers (GpGraphics *graphics, GDIPCONST GpPoint *points, int count, BOOL antialiasing)
{
        int i;

	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, antialiasing);

	for (i = 0; i < count; i++) {
		gdip_cairo_line_to (graphics, points [i].X, points [i].Y, TRUE, antialiasing);
	}

        /*
         * Draw a line from the last point back to the first point if
	 * they're not the same
         */
	if (points [0].X != points [count-1].X && points [0].Y != points [count-1].Y) {
		gdip_cairo_line_to (graphics, points [0].X, points [0].Y, TRUE, antialiasing);
	}

        cairo_close_path (graphics->ct);
}

/*
 * Based on the algorithm described in
 *      http://www.stillhq.com/ctpfaq/2002/03/c1088.html#AEN1212
 */
static void
make_arc (GpGraphics *graphics, BOOL start, float x, float y, float width,
	  float height, float startAngle, float endAngle, BOOL antialiasing)
{       
	float delta, bcp;
	double sin_alpha, sin_beta, cos_alpha, cos_beta;
	double sx, sy;

        float rx = width / 2;
        float ry = height / 2;
        
        /* center */
        float cx = x + rx;
        float cy = y + ry;

        /* angles in radians */        
        float alpha = startAngle * PI / 180;
        float beta = endAngle * PI / 180;

        /* adjust angles for ellipses */
	alpha = atan2 (rx * sin (alpha), ry * cos (alpha));
	beta = atan2 (rx * sin (beta), ry * cos (beta));

	if (fabs (beta - alpha) > M_PI){
		if (beta > alpha)
			beta -= 2 * PI;
		else
			alpha -= 2 * PI;
	}
	
	delta = beta - alpha;
	bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta /2);

	sin_alpha = sin (alpha);
	sin_beta = sin (beta);
	cos_alpha = cos (alpha);
	cos_beta = cos (beta);

        /* starting point */
	sx = cx + rx * cos_alpha;
	sy = cy + ry * sin_alpha;
	
        /* don't move to starting point if we're continuing an existing curve */
        if (start)
                gdip_cairo_move_to (graphics, sx, sy, FALSE, antialiasing);

        gdip_cairo_curve_to (graphics,
                        cx + rx * (cos_alpha - bcp * sin_alpha),
                        cy + ry * (sin_alpha + bcp * cos_alpha),
                        cx + rx * (cos_beta  + bcp * sin_beta),
                        cy + ry * (sin_beta  - bcp * cos_beta),
                        cx + rx * cos_beta, cy + ry * sin_beta,
			FALSE, antialiasing);
}

static void
make_arcs (GpGraphics *graphics, float x, float y, float width, float height, float startAngle, float sweepAngle, 
	BOOL convert_units, BOOL antialiasing)
{
	int i;
	float drawn = 0;
	float endAngle = startAngle + sweepAngle;	
	int increment = (endAngle > 0) ? 90 : -90;
	BOOL enough = FALSE;

	/* if required deal, once and for all, with unit conversions */
	if (convert_units && !OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}
	
	if (fabs (sweepAngle) >= 360) {
		/* FALSE -> units are already converted */
		make_ellipse (graphics, x, y, width, height, FALSE, antialiasing);
		return;
	}

	/* i is the number of sub-arcs drawn, each sub-arc can be at most 90 degrees.*/
	/* there can be no more then 4 subarcs, ie. 90 + 90 + 90 + (something less than 90) */
	for (i = 0; i < 4; i++) {
		float current = startAngle + drawn;
		float additional;

		if (enough)
			return;
		
		if (fabs (current + increment) < fabs (endAngle))
			additional = increment; /* add the default increment */
		else {
			additional = endAngle - current; /* otherwise, add the remainder */
			enough = TRUE;
		}

		/* a near zero value will introduce bad artefact in the drawing (#78999) */
		if (gdip_near_zero (additional))
			return;

		make_arc (graphics,
			  (i == 0) ? TRUE : FALSE,  /* only move to the starting pt in the 1st iteration */
			  x, y, width, height,      /* bounding rectangle */
			  current, current + additional, antialiasing);
		drawn += additional;		
	}
}

static void
make_pie (GpGraphics *graphics, float x, float y, float width, float height,
	float startAngle, float sweepAngle, BOOL antialiasing)
{
	float rx, ry, cx, cy, alpha;
	double sin_alpha, cos_alpha;

	/* if required deal, once and for all, with unit conversions */
	if (!OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}

	rx = width / 2;
	ry = height / 2;

	/* center */
	cx = x + rx;
	cy = y + ry;

	/* angles in radians */        
	alpha = startAngle * PI / 180;
	
        /* adjust angle for ellipses */
        alpha = atan2 (rx * sin (alpha), ry * cos (alpha));

	sin_alpha = sin (alpha);
	cos_alpha = cos (alpha);

	/* if required deal, once and for all, with antialiasing */
	if (antialiasing) {
		cx += graphics->aa_offset_x;
		cy += graphics->aa_offset_y;
	}

	/* draw pie edge */
	if (fabs (sweepAngle) >= 360)
		gdip_cairo_move_to (graphics, cx + rx * cos_alpha, cy + ry * sin_alpha, FALSE, FALSE);
	else {
		gdip_cairo_move_to (graphics, cx, cy, FALSE, FALSE);
		gdip_cairo_line_to (graphics, cx + rx * cos_alpha, cy + ry * sin_alpha, FALSE, FALSE);
	}

	/* draw the arcs */
	make_arcs (graphics, x, y, width, height, startAngle, sweepAngle, FALSE, antialiasing);

	/* draws line back to center */
	if (fabs (sweepAngle) >= 360)
		gdip_cairo_move_to (graphics, cx, cy, FALSE, FALSE);
	else
		gdip_cairo_line_to (graphics, cx, cy, FALSE, FALSE);
}

static cairo_fill_rule_t
convert_fill_mode (FillMode fill_mode)
{
        if (fill_mode == FillModeAlternate) 
                return CAIRO_FILL_RULE_EVEN_ODD;
        else
                return CAIRO_FILL_RULE_WINDING;
}


#ifdef CAIRO_HAS_XLIB_SURFACE

// coverity[+alloc : arg-*1]
GpStatus 
GdipCreateFromHDC (void *hDC, GpGraphics **graphics)
{
	GpGraphics *clone = (GpGraphics *) hDC;
	cairo_surface_t *surface;
	
	int x, y;
	unsigned int w, h, border_w, depth;
	Window root;

#ifdef CAIRO_HAS_PS_SURFACE

	if (clone->type == gtPostScript) {
		*graphics = clone;
		return Ok;
	}
#endif

	XGetGeometry (clone->display, clone->drawable, &root,
		      &x, &y, &w, &h, &border_w, &depth);
	
	surface = cairo_xlib_surface_create(clone->display, clone->drawable,
	    DefaultVisual(clone->display, DefaultScreen(clone->display)),
	    w, h);
			
	*graphics = gdip_graphics_new (surface);
	if (!*graphics)
		return OutOfMemory;

	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);

	return Ok;
}

GpStatus
GdipCreateFromHWND (void *hwnd, GpGraphics **graphics)
{
	return NotImplemented;
}

#endif

#ifdef CAIRO_HAS_QUARTZ_SURFACE

/* This is a dirty hack; but it gets us around header include issues for now
 FIXME: We need to split all the X11 stuff off into its own file(s) so that
 different backends / font backends can be easily introduced in the future. */
cairo_surface_t *cairo_quartz_surface_create(void *ctx, int width, int height);

// coverity[+alloc : arg-*3]
GpStatus
GdipCreateFromQuartz_macosx (void *ctx, int width, int height, GpGraphics **graphics)
{
	if (!graphics)
		return InvalidParameter;

        cairo_surface_t *surface;

	surface = cairo_quartz_surface_create(ctx, width, height);
	
	
	*graphics = gdip_graphics_new(surface);
	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);
	
	(*graphics)->bounds.Width = width;
	(*graphics)->bounds.Height = height;

	(*graphics)->type = gtOSXDrawable;

	return Ok;
}

#endif

#ifdef CAIRO_HAS_XLIB_SURFACE

// coverity[+alloc : arg-*2]
GpStatus
GdipCreateFromXDrawable_linux(Drawable d, Display *dpy, GpGraphics **graphics)
{
	Window root_ignore;
	GpRect bounds;
	int bwidth_ignore, depth_ignore;
        cairo_surface_t *surface;

	if (!graphics)
		return InvalidParameter;

	XGetGeometry (dpy, d, &root_ignore, &bounds.X,  &bounds.Y,
		(unsigned int *)&bounds.Width, (unsigned int *)&bounds.Height, 
		(unsigned int *)&bwidth_ignore, (unsigned int *)&depth_ignore);
	
	surface = cairo_xlib_surface_create(dpy, d,
		    DefaultVisual(dpy, DefaultScreen(dpy)),
		    bounds.Width, bounds.Height);
	
	
	*graphics = gdip_graphics_new(surface);
	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);

	(*graphics)->type = gtX11Drawable;
	(*graphics)->display = dpy;
	(*graphics)->drawable = d;

	GdipSetVisibleClip_linux (*graphics, &bounds);
	return Ok;
}

#endif

#ifdef CAIRO_HAS_XLIB_SURFACE
static int
ignore_error_handler (Display *dpy, XErrorEvent *event)
{
	return Success;
}
#endif

GpStatus 
GdipDeleteGraphics (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;

	/* We don't destroy image because we did not create one. */
	if (graphics->copy_of_ctm) {
		GdipDeleteMatrix (graphics->copy_of_ctm);
		graphics->copy_of_ctm = NULL;
	}

	if (graphics->clip) {
		GdipDeleteRegion (graphics->clip);
		graphics->clip = NULL;
	}

	if (graphics->clip_matrix) {
		GdipDeleteMatrix (graphics->clip_matrix);
		graphics->clip_matrix = NULL;
	}

	if (graphics->ct) {
#ifdef CAIRO_HAS_XLIB_SURFACE
		int (*old_error_handler)(Display *dpy, XErrorEvent *ev) = NULL;
		if (graphics->type == gtX11Drawable)
			old_error_handler = XSetErrorHandler (ignore_error_handler);
#endif

		cairo_destroy (graphics->ct);
		graphics->ct = NULL;

#ifdef CAIRO_HAS_XLIB_SURFACE
		if (graphics->type == gtX11Drawable)
			XSetErrorHandler (old_error_handler);
#endif
	}

	if (graphics->saved_status) {
		GpState* pos_state = graphics->saved_status;
		int i;
		
		for (i = 0; i < MAX_GRAPHICS_STATE_STACK; i++, pos_state++) {
			if (pos_state->clip)
				GdipDeleteRegion (pos_state->clip);		
		}
		
		GdipFree (graphics->saved_status);
		graphics->saved_status = NULL;
	}	

	GdipFree (graphics);
	return Ok;
}

GpStatus 
GdipGetDC (GpGraphics *graphics, void **hDC)
{
	/* For our gdi+ the hDC is equivalent to the graphics handle */
	if (hDC) {
		*hDC = (void *)graphics;
	}
	return Ok;
}

GpStatus 
GdipReleaseDC (GpGraphics *graphics, void *hDC)
{
	if (hDC != (void *)graphics) {
		return InvalidParameter;
	}
	return Ok;
}


GpStatus 
GdipRestoreGraphics (GpGraphics *graphics, unsigned int graphicsState)
{
	GpState* pos_state;

	if (!graphics)
		return InvalidParameter;
	
///printf("[%s %d] GdipRestoreGraphics called\n", __FILE__, __LINE__);

	if (graphicsState >= MAX_GRAPHICS_STATE_STACK || graphicsState > graphics->saved_status_pos)
		return InvalidParameter;

	pos_state = graphics->saved_status;
	pos_state += graphicsState;	

	/* Save from GpState to Graphics  */
	gdip_cairo_matrix_copy (graphics->copy_of_ctm, &pos_state->matrix);
	gdip_cairo_matrix_copy (&graphics->previous_matrix, &pos_state->previous_matrix);

	GdipSetRenderingOrigin (graphics, pos_state->org_x, pos_state->org_y);

	if (graphics->clip)
		GdipDeleteRegion (graphics->clip);
	GdipCloneRegion (pos_state->clip, &graphics->clip);
	gdip_cairo_matrix_copy (graphics->clip_matrix, &pos_state->clip_matrix);

	graphics->composite_mode = pos_state->composite_mode;
	graphics->composite_quality = pos_state->composite_quality;
	graphics->interpolation = pos_state->interpolation;
	graphics->page_unit = pos_state->page_unit;
	graphics->scale = pos_state->scale;
	GdipSetSmoothingMode(graphics, pos_state->draw_mode);
	graphics->text_mode = pos_state->text_mode;
	graphics->pixel_mode = pos_state->pixel_mode;
	graphics->text_contrast = pos_state->text_contrast;

	graphics->saved_status_pos = graphicsState;

	/* re-adjust clipping (region and matrix) */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	/* GdipCloneRegion was called, but for some reason, not registred as an allocation */
	/* coverity[freed_arg] */
	gdip_set_cairo_clipping (graphics);
	return Ok;
}

GpStatus 
GdipSaveGraphics (GpGraphics *graphics, unsigned int *state)
{
	GpState* pos_state;

	if (!graphics || !state)
		return InvalidParameter;

///printf("[%s %d] GdipSaveGraphics called\n", __FILE__, __LINE__);
	if (graphics->saved_status == NULL) {
		graphics->saved_status = gdip_calloc (MAX_GRAPHICS_STATE_STACK, sizeof (GpState));
		graphics->saved_status_pos = 0;
	}

	if (graphics->saved_status_pos >= MAX_GRAPHICS_STATE_STACK) 
		return OutOfMemory;

	pos_state = graphics->saved_status;
	pos_state += graphics->saved_status_pos;

	/* Save from Graphics to GpState */
	gdip_cairo_matrix_copy (&pos_state->matrix, graphics->copy_of_ctm);
	GdipGetRenderingOrigin (graphics, &pos_state->org_x, &pos_state->org_y);

	gdip_cairo_matrix_copy (&pos_state->previous_matrix, &graphics->previous_matrix);

	if (pos_state->clip)
		GdipDeleteRegion (pos_state->clip);
	GdipCloneRegion (graphics->clip, &pos_state->clip);
	gdip_cairo_matrix_copy (&pos_state->clip_matrix, graphics->clip_matrix);

	pos_state->composite_mode = graphics->composite_mode;
	pos_state->composite_quality = graphics->composite_quality;
	pos_state->interpolation = graphics->interpolation;
	pos_state->page_unit = graphics->page_unit;
	pos_state->scale = graphics->scale;
	pos_state->draw_mode = graphics->draw_mode;
	pos_state->text_mode = graphics->text_mode;
	pos_state->pixel_mode = graphics->pixel_mode;
	pos_state->text_contrast = graphics->text_contrast;
	
	*state = graphics->saved_status_pos;
	graphics->saved_status_pos++;
	return Ok;
}

GpStatus
GdipResetWorldTransform (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;

///printf("[%s %d] GdipResetWorldTransform called\n", __FILE__, __LINE__);
	cairo_matrix_init_identity (graphics->copy_of_ctm);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	cairo_matrix_init_identity (graphics->clip_matrix);
	cairo_reset_clip (graphics->ct);
	gdip_set_cairo_clipping (graphics);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	GpStatus status;
	BOOL invertible;

	if (!graphics || !matrix)
		return InvalidParameter;

///printf("[%s %d] GdipSetWorldTransform called\n", __FILE__, __LINE__);
	/* optimization - inverting an identity matrix result in the identity matrix */
	if (gdip_is_matrix_empty (matrix))
		return GdipResetWorldTransform (graphics);

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	gdip_cairo_matrix_copy (graphics->copy_of_ctm, matrix);
        cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	gdip_cairo_matrix_copy (graphics->clip_matrix, matrix);
	status = GdipInvertMatrix (graphics->clip_matrix);

	if (status == Ok)
		gdip_set_cairo_clipping (graphics);
        return status;
}

GpStatus 
GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	if (!graphics || !matrix)
		return InvalidParameter;

	/* get the effective matrix from cairo */
        cairo_get_matrix (graphics->ct, matrix);
	/* if we're inside a container then the previous matrix are hidden */
	if (!gdip_is_matrix_empty (&graphics->previous_matrix)) {
		cairo_matrix_t inverted;
		/* substract the previous matrix from the effective matrix */
		gdip_cairo_matrix_copy (&inverted, &graphics->previous_matrix);
		cairo_matrix_invert (&inverted);
		return GdipMultiplyMatrix (matrix, &inverted, MatrixOrderAppend);
	}
        return Ok;
}

static GpStatus
apply_world_to_bounds (GpGraphics *graphics)
{
	GpStatus status;
	GpPointF pts[2];

	gdip_set_cairo_clipping (graphics);

	pts[0].X = graphics->bounds.X;
	pts[0].Y = graphics->bounds.Y;
	pts[1].X = graphics->bounds.X + graphics->bounds.Width;
	pts[1].Y = graphics->bounds.Y + graphics->bounds.Height;
	status = GdipTransformMatrixPoints (graphics->clip_matrix, (GpPointF*)&pts, 2);
	if (status != Ok)
		return status;

	if (pts[0].X > pts[1].X) {
		graphics->bounds.X = pts[1].X;
		graphics->bounds.Width = iround (pts[0].X - pts[1].X);
	} else {
		graphics->bounds.X = pts[0].X;
		graphics->bounds.Width = iround (pts[1].X - pts[0].X);
	}
	if (pts[0].Y > pts[1].Y) {
		graphics->bounds.Y = pts[1].Y;
		graphics->bounds.Height = iround (pts[0].Y - pts[1].Y);
	} else {
		graphics->bounds.Y = pts[0].Y;
		graphics->bounds.Height = iround (pts[1].Y - pts[0].Y);
	}
	return Ok;
}

GpStatus
GdipMultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order)
{
        Status s;
	int invertible;
	GpMatrix inverted;

///printf("[%s %d] GdipMultiplyWorldTransform called\n", __FILE__, __LINE__);
	if (!graphics)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	s = GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible || (s != Ok))
		return InvalidParameter;

	s = GdipMultiplyMatrix (graphics->copy_of_ctm, matrix, order);
        if (s != Ok)
                return s;

	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	/* Multiply the inverted matrix with the clipping matrix */
	gdip_cairo_matrix_copy (&inverted, matrix);
	s = GdipInvertMatrix (&inverted);
        if (s != Ok)
                return s;

	s = GdipMultiplyMatrix (graphics->clip_matrix, &inverted, order);
	if (s == Ok)
		s = apply_world_to_bounds (graphics);
	return s;
}

GpStatus 
GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order)
{
	GpStatus s;

	if (!graphics)
		return InvalidParameter;
///printf("[%s %d] GdipRotateWorldTransform called\n", __FILE__, __LINE__);

	s = GdipRotateMatrix (graphics->copy_of_ctm, angle, order);
        if (s != Ok)
                return s;

	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	s = GdipRotateMatrix (graphics->clip_matrix, -angle, gdip_matrix_reverse_order (order));
	if (s == Ok)
		s = apply_world_to_bounds (graphics);
	return s;
}

GpStatus
GdipScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order)
{
        GpStatus s;

///printf("[%s %d] GdipScaleWorldTransform called\n", __FILE__, __LINE__);

	if (!graphics || (sx == 0.0f) || (sy == 0.0f))
		return InvalidParameter;

	s = GdipScaleMatrix (graphics->copy_of_ctm, sx, sy, order);
        if (s != Ok)
                return s;

	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	s = GdipScaleMatrix (graphics->clip_matrix, (1.0f / sx), (1.0f / sy), gdip_matrix_reverse_order (order));
	if (s == Ok)
		s = apply_world_to_bounds (graphics);
	return s;
}

GpStatus 
GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order)
{
        GpStatus s;

	if (!graphics)
		return InvalidParameter;
///printf("[%s %d] GdipTranslateWorldTransform called\n", __FILE__, __LINE__);

	s = GdipTranslateMatrix (graphics->copy_of_ctm, dx, dy, order);
        if (s != Ok) 
                return s;

	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	s = GdipTranslateMatrix (graphics->clip_matrix, -dx, -dy, gdip_matrix_reverse_order (order));
	if (s == Ok)
		s = apply_world_to_bounds (graphics);
	return s;
}

/*
 * Draw operations.
 */

GpStatus
GdipDrawArc (GpGraphics *graphics, GpPen *pen, 
	     float x, float y, float width, float height, 
	     float startAngle, float sweepAngle)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We should
	 * have it set already.
	 */
	make_arcs (graphics, x, y, width, height, startAngle, sweepAngle, TRUE, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawArcI (GpGraphics *graphics, GpPen *pen, 
	      int x, int y, int width, int height, 
	      float startAngle, float sweepAngle)
{
        return GdipDrawArc (graphics, pen, x, y, width, height, startAngle, sweepAngle);
}

GpStatus 
GdipDrawBezier (GpGraphics *graphics, GpPen *pen, 
                float x1, float y1, float x2, float y2,
                float x3, float y3, float x4, float y4)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_move_to (graphics, x1, y1, TRUE, TRUE);
	gdip_cairo_curve_to (graphics, x2, y2, x3, y3, x4, y4, TRUE, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, 
			  int x1, int y1, int x2, int y2,
			  int x3, int y3, int x4, int y4)
{
        return GdipDrawBezier (graphics, pen,
			       x1, y1, x2, y2, x3, y3, x4, y4);
}

GpStatus 
GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

	if (!graphics || !pen || !points)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, TRUE);

        for (i = 0; i < count - 3; i += 3) {
                j = i + 1;
                k = i + 2;
		gdip_cairo_curve_to (graphics, points [i].X, points [i].Y, points [j].X, points [j].Y,
			points [k].X, points [k].Y, TRUE, TRUE);
        }

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

	if (!graphics || !pen || !points)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, TRUE);

        for (i = 0; i < count - 3; i += 3) {
                j = i + 1;
                k = i + 2;
		gdip_cairo_curve_to (graphics, points [i].X, points [i].Y, points [j].X, points [j].Y,
			points [k].X, points [k].Y, TRUE, TRUE);
        }

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, 
		 float x, float y, float width, float height)
{	
	if (!graphics || !pen)
		return InvalidParameter;
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_ellipse (graphics, x, y, width, height, TRUE, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawEllipseI (GpGraphics *graphics, GpPen *pen,
		  int x, int y, int width, int height)
{
        return GdipDrawEllipse (graphics, pen, x, y, width, height);
}

GpStatus
GdipDrawLine (GpGraphics *graphics, GpPen *pen,
	      float x1, float y1, float x2, float y2)
{
	cairo_matrix_t saved;

	if (!graphics || !pen)
		return InvalidParameter;

	gdip_cairo_matrix_copy (&saved, graphics->copy_of_ctm);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_move_to (graphics, x1, y1, TRUE, TRUE);
	gdip_cairo_line_to (graphics, x2, y2, TRUE, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	gdip_cairo_matrix_copy (graphics->copy_of_ctm, &saved);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawLineI (GpGraphics *graphics, GpPen *pen, 
	       int x1, int y1, int x2, int y2)
{
        return GdipDrawLine (graphics, pen, x1, y1, x2, y2);
}

GpStatus 
GdipDrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	int i;

	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, TRUE);

	for (i = 1; i < count; i++) {
		gdip_cairo_line_to (graphics, points [i].X, points [i].Y, TRUE, TRUE);
	}

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
	int i;

	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_move_to (graphics, points [0].X, points [0].Y, TRUE, TRUE);

	for (i = 1; i < count; i++) {
		gdip_cairo_line_to (graphics, points [i].X, points [i].Y, TRUE, TRUE);
	}

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

static GpStatus
gdip_plot_path (GpGraphics *graphics, GpPath *path, BOOL antialiasing)
{
        int length = path->count;
        int i, idx = 0;

        for (i = 0; i < length; ++i) {
                GpPointF pt = g_array_index (path->points, GpPointF, i);
                BYTE type = g_array_index (path->types, BYTE, i);
                GpPointF pts [3];

		/* mask the bits so that we get only the type value not the other flags */
                switch (type & PathPointTypePathTypeMask) {
                case PathPointTypeStart:
			gdip_cairo_move_to (graphics, pt.X, pt.Y, TRUE, antialiasing);
                        break;

                case PathPointTypeLine:
			gdip_cairo_line_to (graphics, pt.X, pt.Y, TRUE, antialiasing);
                        break;

                case PathPointTypeBezier:
                        /* make sure we only add at most 3 points to pts */
                        if (idx < 3) {
                                pts [idx] = pt;
                                idx ++;
                        }

                        /* once we've added 3 pts, we can draw the curve */
                        if (idx == 3) {
				gdip_cairo_curve_to (graphics, pts[0].X, pts[0].Y, pts[1].X, pts[1].Y,
					pts[2].X, pts[2].Y, TRUE, antialiasing);

                                idx = 0;
                        }

                        break;
                default:
			g_warning ("Unknown PathPointType %d", type);
                        return NotImplemented;
                }

		/* close the subpath */
		if (type & PathPointTypeCloseSubpath)
			cairo_close_path (graphics->ct);
        }
	return Ok;
}

GpStatus
GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
	GpStatus status;

	if (!graphics || !pen || !path)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	status = gdip_plot_path (graphics, path, FALSE);

        /* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
        cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return status;
}

GpStatus
GdipDrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, 
	     float width, float height, float startAngle, float sweepAngle)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_pie (graphics, x, y, width, height, startAngle, sweepAngle, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawPieI (GpGraphics *graphics, GpPen *pen, int x, int y, 
	      int width, int height, float startAngle, float sweepAngle)
{
        return GdipDrawPie (graphics, pen, x, y, width, height, startAngle, sweepAngle);
}

GpStatus
GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon (graphics, points, count, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon_from_integers (graphics, points, count, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectangle (GpGraphics *graphics, GpPen *pen,
		   float x, float y, float width, float height)
{
        cairo_matrix_t saved;

	if (!graphics || !pen)
		return InvalidParameter;

	/* don't draw/fill rectangles with negative width/height (bug #77129) */
	if ((width < 0) || (height < 0))
		return Ok;

        gdip_cairo_matrix_copy (&saved, graphics->copy_of_ctm);
        cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_rectangle (graphics, x, y, width, height, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);
	
	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	gdip_cairo_matrix_copy (graphics->copy_of_ctm, &saved);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen,
		    int x, int y, int width, int height)
{
        return GdipDrawRectangle (graphics, pen, x, y, width, height);
}

GpStatus
GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, int count)
{
	BOOL draw = FALSE;
	int i;

	if (!graphics || !pen || !rects || count <= 0)
		return InvalidParameter;
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++) {
		/* don't draw/fill rectangles with negative width/height (bug #77129) */
		if ((rects [i].Width < 0) || (rects [i].Height < 0))
			continue;

		gdip_cairo_rectangle (graphics, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height, TRUE);
		draw = TRUE;
	}

	if (!draw)
		return Ok;

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	
	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects, int count)
{
	BOOL draw = FALSE;
	int i;

	if (!graphics || !pen || !rects || count <= 0)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++) {
		/* don't draw/fill rectangles with negative width/height (bug #77129) */
		if ((rects [i].Width < 0) || (rects [i].Height < 0))
			continue;

		gdip_cairo_rectangle (graphics, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height, FALSE);
		draw = TRUE;
	}

	if (!draw)
		return Ok;

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

static void
make_curve (GpGraphics *graphics, GDIPCONST GpPointF *points, GpPointF *tangents, int offset, int length,
	_CurveType type, BOOL antialiasing)
{
        int i;

	gdip_cairo_move_to (graphics, points [offset].X, points [offset].Y, FALSE, antialiasing);

	for (i = offset; i < offset + length; i++) {
		int j = i + 1;

		double x1 = points [i].X + tangents [i].X;
		double y1 = points [i].Y + tangents [i].Y;

		double x2 = points [j].X - tangents [j].X;
		double y2 = points [j].Y - tangents [j].Y;

		double x3 = points [j].X;
		double y3 = points [j].Y;

		gdip_cairo_curve_to (graphics, x1, y1, x2, y2, x3, y3, FALSE, antialiasing);
        }

        if (type == CURVE_CLOSE) {
		/* complete (close) the curve using the first point */
		double x1 = points [i].X + tangents [i].X;
		double y1 = points [i].Y + tangents [i].Y;

		double x2 = points [0].X - tangents [0].X;
		double y2 = points [0].Y - tangents [0].Y;

		double x3 = points [0].X;
		double y3 = points [0].Y;

		gdip_cairo_curve_to (graphics, x1, y1, x2, y2, x3, y3, FALSE, antialiasing);

                cairo_close_path (graphics->ct);
	}
}

GpStatus
GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
        return GdipDrawClosedCurve2 (graphics, pen, points, count, 0.5f);
}

GpStatus
GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
        return GdipDrawClosedCurve2I (graphics, pen, points, count, 0.5f);
}

GpStatus
GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count, float tension)
{
        GpPointF *tangents;

        /* when tension is 0, draw straight lines */
        if (tension == 0)
                return GdipDrawPolygon (graphics, pen, points, count);

	if (!graphics || !pen || !points || count <= 2)
		return InvalidParameter;
        
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

	make_curve (graphics, points, tangents, 0, count - 1, CURVE_CLOSE, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	GdipFree (tangents);        

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count, float tension)
{
	GpPointF *pt;
	GpStatus s;

	if (!points || count <= 0)
		return InvalidParameter;

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;

	s = GdipDrawClosedCurve2 (graphics, pen, pt, count, tension);

        GdipFree (pt);

        return s;
}

GpStatus
GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count) 
{
	if (count == 2) {
		return GdipDrawLines (graphics, pen, points, count);
	} else {
		int segments = (count > 3) ? (count - 1) : (count - 2);
	        return GdipDrawCurve3 (graphics, pen, points, count, 0, segments, 0.5f);
	}
}

GpStatus
GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count) 
{
	if (count == 2) {
		return GdipDrawLinesI (graphics, pen, points, count);
	} else {
		int segments = (count > 3) ? (count - 1) : (count - 2);
	        return GdipDrawCurve3I (graphics, pen, points, count, 0, segments, 0.5f);
	}
}

GpStatus
GdipDrawCurve2 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, int count, float tension)
{
	if (count == 2) {
		return GdipDrawLines (graphics, pen, points, count);
	} else {
		int segments = (count > 3) ? (count - 1) : (count - 2);
	        return GdipDrawCurve3 (graphics, pen, points, count, 0, segments, tension);
	}
}

GpStatus
GdipDrawCurve2I (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPoint *points, int count, float tension)
{
	if (count == 2) {
		return GdipDrawLinesI (graphics, pen, points, count);
	} else {
		int segments = (count > 3) ? (count - 1) : (count - 2);
	        return GdipDrawCurve3I (graphics, pen, points, count, 0, segments, tension);
	}
}

GpStatus
GdipDrawCurve3 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, int count, int offset, int numOfSegments, float tension)
{
        GpPointF *tangents;

        /* draw lines if tension = 0 */
        if (tension == 0)
                return GdipDrawLines (graphics, pen, points, count);

	if (!graphics || !pen || !points || numOfSegments < 1)
		return InvalidParameter;

	/* we need 3 points for the first curve, 2 more for each curves */
	/* and it's possible to use a point prior to the offset (to calculate) */
	if ((offset == 0) && (numOfSegments == 1) && (count < 3))
		return InvalidParameter;
	else if (numOfSegments >= count - offset)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
        tangents = gdip_open_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;
	make_curve (graphics, points, tangents, offset, numOfSegments, CURVE_OPEN, TRUE);

	/* We do pen setup just before stroking. */
	gdip_pen_setup (graphics, pen);
	cairo_stroke (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by pen setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

        GdipFree (tangents);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawCurve3I (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPoint *points, int count, int offset, int numOfSegments, float tension)
{
	GpPointF *pf;
	GpStatus s;

	if (!points || count <= 0)
		return InvalidParameter;

	pf = convert_points (points, count);
	if (!pf)
		return OutOfMemory;

	s = GdipDrawCurve3 (graphics, pen, pf, count, offset, numOfSegments, tension);

        GdipFree (pf);

        return s;
}

/*
 * Fills
 */
GpStatus
GdipFillEllipse (GpGraphics *graphics, GpBrush *brush,
		 float x, float y, float width, float height)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_ellipse (graphics, x, y, width, height, TRUE, FALSE);
	
	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);       
	
	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	
	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush,
		  int x, int y, int width, int height)
{
        return GdipFillEllipse (graphics, brush, x, y, width, height);
}

GpStatus 
GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, 
		   float x, float y, float width, float height)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* don't draw/fill rectangles with negative width/height (bug #77129) */
	if ((width < 0) || (height < 0))
		return Ok;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	gdip_cairo_rectangle (graphics, x, y, width, height, FALSE);

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, 
		   int x, int y, int width, int height)
{
	return GdipFillRectangle (graphics, brush, x, y, width, height);
}

GpStatus 
GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, int count)
{
	BOOL draw = FALSE;
	int i;

	if (!graphics || !brush || !rects || count <= 0)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++) {
		/* don't draw/fill rectangles with negative width/height (bug #77129) */
		if ((rects [i].Width < 0) || (rects [i].Height < 0))
			continue;

		gdip_cairo_rectangle (graphics, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height, FALSE);
		draw = TRUE;
	}

	/* shortcut if no rectangles were drawn into the graphics */
	if (!draw)
		return Ok;

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRect *rects, int count)
{
	BOOL draw = FALSE;
	int i;

	if (!graphics || !brush || !rects || count <= 0)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++) {
		/* don't draw/fill rectangles with negative width/height (bug #77129) */
		if ((rects [i].Width < 0) || (rects [i].Height < 0))
			continue;

		gdip_cairo_rectangle (graphics, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height, FALSE);
		draw = TRUE;
	}

	/* shortcut if no rectangles were drawn into the graphics */
	if (!draw)
		return Ok;

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPie (GpGraphics *graphics, GpBrush *brush,
	     float x, float y, float width, float height,
	     float startAngle, float sweepAngle)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_pie (graphics, x, y, width, height, startAngle, sweepAngle, FALSE);

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPieI (GpGraphics *graphics, GpBrush *brush,
	      int x, int y, int width, int height,
	      float startAngle, float sweepAngle)
{
        return GdipFillPie (graphics, brush, x, y, width, height, startAngle, sweepAngle);
}

GpStatus
GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, FillMode fillMode)
{
	if (!graphics || !brush || !points)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon (graphics, points, count, FALSE);
	cairo_set_fill_rule (graphics->ct, convert_fill_mode (fillMode));

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

/* FIXME - this doesn't match MS behaviour when we use really complex paths with internal intersections */
GpStatus
GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
	GpStatus status;
	if (!graphics || !brush || !path)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	status = gdip_plot_path (graphics, path, FALSE);
	cairo_set_fill_rule (graphics->ct, convert_fill_mode (path->fill_mode));

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return status;
}

GpStatus
GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count, FillMode fillMode)
{
	if (!graphics || !brush || !points)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon_from_integers (graphics, points, count, FALSE);
	cairo_set_fill_rule (graphics->ct, convert_fill_mode (fillMode));

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count)
{
        return GdipFillPolygon (graphics, brush, points, count, FillModeAlternate);
}

GpStatus
GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count)
{
        return GdipFillPolygonI (graphics, brush, points, count, FillModeAlternate);
}

GpStatus
GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count)
{
        return GdipFillClosedCurve2 (graphics, brush, points, count, 0.5f);
}

GpStatus
GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count)
{
        return GdipFillClosedCurve2I (graphics, brush, points, count, 0.5f);
}

GpStatus
GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, float tension)
{
        GpPointF *tangents;

        /* when tension is 0, the edges are straight lines */
        if (tension == 0)
                return GdipFillPolygon2 (graphics, brush, points, count);

	if (!graphics || !brush || !points || count <= 0)
		return InvalidParameter;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	if (!tangents)
		return OutOfMemory;

	make_curve (graphics, points, tangents, 0, count - 1, CURVE_CLOSE, FALSE);

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	GdipFree (tangents);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count, float tension)
{
	GpPointF *pt;
        GpStatus s;
	
	if (!points || count <= 0)
		return InvalidParameter;

	pt = convert_points (points, count);
	if (!pt)
		return OutOfMemory;

	s = GdipFillClosedCurve2 (graphics, brush, pt, count, tension);

        GdipFree (pt);

        return s;
}

GpStatus
GdipFillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region)
{
        if (!graphics || !brush || !region)
		return InvalidParameter;

	/* if this is a region with a complex path */
	if (region->type == RegionTypePath) {
		GpStatus status;
		GpBitmap *bitmap = NULL;

		/* (optimization) if if the path is empty, return immediately */
		if (!region->tree)
			return Ok;

		/* (optimization) if there is only one path, then we do not need the bitmap */
		if (region->tree->path) {
			/* if the path is empty, return OK */
			if (region->tree->path->count == 0)
				return Ok;

			/* else fill the single path */
			return GdipFillPath (graphics, brush, region->tree->path);
		}

		gdip_region_bitmap_ensure (region);
		if (!region->bitmap)
			return OutOfMemory;

		status = GdipCreateBitmapFromGraphics (region->bitmap->Width, region->bitmap->Height, graphics, &bitmap);
		if (status == Ok) {
			GpGraphics *bitgraph = NULL;
			status = GdipGetImageGraphicsContext ((GpImage*)bitmap, &bitgraph);
			if (status == Ok) {
				/* fill the "full" rectangle using the specified brush */
				GdipFillRectangle (bitgraph, brush, 0, 0, region->bitmap->Width, region->bitmap->Height);

				/* adjust bitmap alpha (i.e. shape the brushed-rectangle like the region) */
				gdip_region_bitmap_apply_alpha (bitmap, region->bitmap);

				/* draw the region */
				status = GdipDrawImageRect (graphics, (GpImage*)bitmap, region->bitmap->X, region->bitmap->Y,
					region->bitmap->Width, region->bitmap->Height);
			}
			if (bitgraph)
				GdipDeleteGraphics (bitgraph);
		}
		if (bitmap)
			GdipDisposeImage ((GpImage*)bitmap);
		return status;
	}

	/* if there's no rectangles, we can return directly */
	if (!region->rects || (region->cnt == 0))
		return Ok;

	return GdipFillRectangles (graphics, brush, region->rects, region->cnt);
}


GpStatus 
GdipSetRenderingOrigin (GpGraphics *graphics, int x, int y)
{
	if (!graphics)
		return InvalidParameter;

	graphics->render_origin_x = x;
	graphics->render_origin_y = y;

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipGetRenderingOrigin (GpGraphics *graphics, int *x, int *y)
{
	if (!graphics || !x || !y)
		return InvalidParameter;

        *x = graphics->render_origin_x;
        *y = graphics->render_origin_y;

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipGetDpiX (GpGraphics *graphics, float *dpi)
{
	if (!graphics || !dpi)
		return InvalidParameter;

	*dpi = graphics->dpi_x;
        return Ok;
}

GpStatus 
GdipGetDpiY (GpGraphics *graphics, float *dpi)
{
	if (!graphics || !dpi)
		return InvalidParameter;

	*dpi = graphics->dpi_y;
        return Ok;
}

GpStatus
GdipGraphicsClear (GpGraphics *graphics, ARGB color)
{
	double red, green, blue, alpha;

	if (!graphics)
		return InvalidParameter;

	blue = color & 0xff;
	green = (color >> 8) & 0xff;
	red = (color >> 16) & 0xff;
	alpha = (color >> 24) & 0xff;

	/* Save the existing color/alpha/pattern settings */
	cairo_save (graphics->ct);

	cairo_set_source_rgba (graphics->ct, red / 255, green / 255, blue / 255, alpha / 255);
	cairo_set_operator (graphics->ct, CAIRO_OPERATOR_SOURCE);
	cairo_paint (graphics->ct);

	/* Restore the color/alpha/pattern settings */
	cairo_restore (graphics->ct);

	return Ok;
}

GpStatus
GdipSetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode)
{
	if (!graphics)
		return InvalidParameter;

	graphics->interpolation = interpolationMode;
	return Ok;
}

GpStatus
GdipGetInterpolationMode (GpGraphics *graphics, InterpolationMode *imode)
{
	if (!graphics || !imode)
		return InvalidParameter;

	*imode = graphics->interpolation;
	return Ok;
}

GpStatus
GdipSetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode)
{
	if (!graphics)
		return InvalidParameter;

	graphics->text_mode = mode;

	return Ok;
}

GpStatus
GdipGetTextRenderingHint(GpGraphics *graphics, TextRenderingHint *mode)
{
	if (!graphics || !mode) 
		return InvalidParameter;
	
	*mode = graphics->text_mode;

	return Ok;
}

/* MonoTODO - pixel offset mode isn't supported */
GpStatus
GdipSetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode)
{
	if (!graphics || (pixelOffsetMode == PixelOffsetModeInvalid))
		return InvalidParameter;
	
	/* FIXME: changing pixel mode affects other properties (e.g. the visible clip bounds) */
	graphics->pixel_mode = pixelOffsetMode;
	return Ok;
}

/* MonoTODO - pixel offset mode isn't supported */
GpStatus
GdipGetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode)
{
	if (!graphics || !pixelOffsetMode)
		return InvalidParameter;
	
	*pixelOffsetMode = graphics->pixel_mode;
	return Ok;
}

/* MonoTODO - text contrast isn't supported */
GpStatus
GdipSetTextContrast (GpGraphics *graphics, UINT contrast)
{
	/** The gamma correction value must be between 0 and 12.
	 * The default value is 4. */
	if (!graphics || contrast < 0 || contrast > 12)
		return InvalidParameter; 

	graphics->text_contrast = contrast;
	return Ok;
}

/* MonoTODO - text contrast isn't supported */
GpStatus
GdipGetTextContrast (GpGraphics *graphics, UINT *contrast)
{
	if (!graphics || !contrast)
		return InvalidParameter;

	*contrast = graphics->text_contrast;
	return Ok;
}

GpStatus
GdipSetSmoothingMode (GpGraphics *graphics, SmoothingMode mode)
{
	if (!graphics)
		return InvalidParameter;

	graphics->draw_mode = mode;

	switch (mode) {
		case SmoothingModeAntiAlias:
		case SmoothingModeHighQuality: {
			cairo_set_antialias(graphics->ct, CAIRO_ANTIALIAS_DEFAULT);
			graphics->aa_offset_x = 0.5;
			graphics->aa_offset_y = 0.5;
			break;
		}

		case SmoothingModeNone:
		case SmoothingModeDefault:
		case SmoothingModeHighSpeed:
		default: {
			cairo_set_antialias(graphics->ct, CAIRO_ANTIALIAS_NONE);
			graphics->aa_offset_x = CAIRO_AA_OFFSET_X;
			graphics->aa_offset_y = CAIRO_AA_OFFSET_Y;
			break;
		}
		
	}

	return Ok;
}

GpStatus
GdipGetSmoothingMode (GpGraphics *graphics, SmoothingMode *mode)
{
	if (!graphics || !mode)
		return InvalidParameter;

	*mode = graphics->draw_mode;
	return Ok;
}

/* MonoTODO - dstrect, srcrect and unit support isn't implemented */
GpStatus
GdipBeginContainer (GpGraphics *graphics, GDIPCONST GpRectF* dstrect, GDIPCONST GpRectF *srcrect, GpUnit unit, GraphicsContainer *state)
{
	if (!graphics || !dstrect || !srcrect || (unit < UnitPixel) || (unit > UnitMillimeter))
		return InvalidParameter;

	return GdipBeginContainer2 (graphics, state);
}

GpStatus
GdipBeginContainer2 (GpGraphics *graphics, GraphicsContainer* state)
{
	GpStatus status;

	if (!graphics || !state)
		return InvalidParameter;

	status = GdipSaveGraphics (graphics, state);
	if (status == Ok) {
		/* reset most properties to defaults after saving them */
		gdip_graphics_reset (graphics);
		/* copy the current effective matrix as the preivous matrix */
		gdip_cairo_matrix_copy (&graphics->previous_matrix, graphics->copy_of_ctm);
	}
	return status;
}

/* MonoTODO - depends on incomplete GdipBeginContainer */
GpStatus
GdipBeginContainerI (GpGraphics *graphics, GDIPCONST GpRect* dstrect, GDIPCONST GpRect *srcrect, GpUnit unit, GraphicsContainer *state)
{
	GpRectF dr, sr;

	if (!dstrect || !srcrect)
		return InvalidParameter;

	dr.X = dstrect->X;
	dr.Y = dstrect->Y;
	dr.Width = dstrect->Width;
	dr.Height = dstrect->Height;

	sr.X = srcrect->X;
	sr.Y = srcrect->Y;
	sr.Width = srcrect->Width;
	sr.Height = srcrect->Height;

	return GdipBeginContainer (graphics, &dr, &sr, unit, state);
}

GpStatus
GdipEndContainer (GpGraphics *graphics, GraphicsContainer state)
{
	if (!graphics)
		return InvalidParameter;

	return GdipRestoreGraphics (graphics, state);
}

GpStatus
GdipFlush (GpGraphics *graphics, GpFlushIntention intention)
{
	cairo_surface_t* surface;

	if (!graphics)
		return InvalidParameter;

	surface = cairo_get_target (graphics->ct);
	cairo_surface_flush (surface);
	return Ok;
}

/*
	Since cairo does autoclipping and it hides the clipping rectangles from the API, the
	best thing for now is keep track of what the user wants and let Cairo do its autoclipping
*/

void
gdip_set_cairo_clipping (GpGraphics *graphics)
{
	GpRegion *work;
        GpRectF* rect;
        int i;

	cairo_reset_clip (graphics->ct);
 
	if (gdip_is_InfiniteRegion (graphics->clip))
		return;

	if (gdip_is_matrix_empty (graphics->clip_matrix)) {
		work = graphics->clip;
	} else {
		GdipCloneRegion (graphics->clip, &work);
		GdipTransformRegion (work, graphics->clip_matrix);
	}

	switch (work->type) {
	case RegionTypeRectF:
	        for (i = 0, rect = work->rects; i < work->cnt; i++, rect++) {
			gdip_cairo_rectangle (graphics, rect->X, rect->Y, rect->Width, rect->Height, FALSE);
		}
		break;
	case RegionTypePath:
		if (work->tree && work->tree->path)
			gdip_plot_path (graphics, work->tree->path, TRUE);
		else {
			int count;
			/* I admit that's a (not so cute) hack - anyone with a better idea ? */
			if ((GdipGetRegionScansCount (work, &count, NULL) == Ok) && (count > 0)) {
				GpRectF *rects = (GpRectF*) GdipAlloc (count * sizeof (GpRectF));
				if (rects) {
					GdipGetRegionScans (work, rects, &count, NULL);
				        for (i = 0, rect = rects; i < count; i++, rect++) {
						gdip_cairo_rectangle (graphics, rect->X, rect->Y, rect->Width, rect->Height, FALSE);
					}
					GdipFree (rects);
				}
			}
		}
		break;
	default:
		g_warning ("Unknown region type %d", work->type);
		break;
	}
	
	cairo_clip (graphics->ct);

	/* destroy the clone, if one was needed */
	if (work != graphics->clip)
		GdipDeleteRegion (work);
}

GpStatus
GdipSetClipGraphics (GpGraphics *graphics, GpGraphics *srcgraphics, CombineMode combineMode)
{
	if (!graphics || !srcgraphics)
		return InvalidParameter;

	return GdipSetClipRegion (graphics, srcgraphics->clip, combineMode);
}

GpStatus
GdipSetClipRect (GpGraphics *graphics, float x, float y, float width, float height, CombineMode combineMode)
{
	GpStatus status;
	GpRectF rect;
	GpRegion *region = NULL;
///printf("[%s %d] GdipSetClipRect %f, %f, %fx%f called\n", __FILE__, __LINE__, x, y, width, height);	
	if (!graphics)
		return InvalidParameter;

	rect.X = x;
	rect.Y = y;
	rect.Width = width;
	rect.Height = height;

	status = GdipCreateRegionRect (&rect, &region);
	if (status == Ok)
		status = GdipSetClipRegion (graphics, region, combineMode);
	if (region)
		GdipDeleteRegion (region);
	return status;
}

GpStatus
GdipSetClipRectI (GpGraphics *graphics, int x, int y, int width, int height, CombineMode combineMode)
{
	return GdipSetClipRect (graphics, x, y, width, height, combineMode);
}

GpStatus
GdipSetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode)
{
	GpStatus status;

///printf("[%s %d] GdipSetClipPath called\n", __FILE__, __LINE__);	
	if (!graphics || !path)
		return InvalidParameter;

	status = GdipCombineRegionPath (graphics->clip, path, combineMode);	
	if (status == Ok) {
		cairo_reset_clip (graphics->ct);
		gdip_set_cairo_clipping (graphics);
	}
	return status;
}

GpStatus
GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode)
{
	GpStatus status;
	GpRegion *work;
///printf("[%s %d] GdipSetClipRegion called\n", __FILE__, __LINE__);	

	if (!graphics || !region)
		return InvalidParameter;

	/* if the matrix is empty, avoid region cloning and transform */
	if (gdip_is_matrix_empty (graphics->clip_matrix)) {
		work = region;
	} else {
		cairo_matrix_t inverted;

		gdip_cairo_matrix_copy (&inverted, graphics->clip_matrix);
		cairo_matrix_invert (&inverted);

		GdipCloneRegion (region, &work);
		GdipTransformRegion (work, &inverted);
	}

	status = GdipCombineRegionRegion (graphics->clip, work, combineMode);	
	if (status == Ok) {
		cairo_reset_clip (graphics->ct);
		gdip_set_cairo_clipping (graphics);
	}

	if (work != region)
		GdipDeleteRegion (work);

	return status;
}

/* Note: not exposed in System.Drawing.dll */
GpStatus
GdipSetClipHrgn (GpGraphics *graphics, void *hRgn, CombineMode combineMode)
{
	GpStatus status;

	if (!graphics)
		return InvalidParameter;

	if (hRgn) {
		status = GdipSetClipRegion (graphics, (GpRegion*)hRgn, combineMode);
	} else {
		/* hRng == NULL means an infinite region */
		GpRegion *work;
		status = GdipCreateRegion (&work);
		if (status == Ok) {
			status = GdipSetClipRegion (graphics, work, combineMode);
			GdipDeleteRegion (work);
		}
	}
	return status;
}

GpStatus
GdipResetClip (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;

///printf("[%s %d] GdipResetClip called\n", __FILE__, __LINE__);	
	GdipSetInfinite (graphics->clip);
	cairo_matrix_init_identity (graphics->clip_matrix);
	cairo_reset_clip (graphics->ct);
	return Ok;
}

GpStatus
GdipTranslateClip (GpGraphics *graphics, float dx, float dy)
{
	GpStatus status;

///printf("[%s %d] GdipTranslateClip %f %f called\n", __FILE__, __LINE__, dx, dy);	
	if (!graphics)
		return InvalidParameter;

	status = GdipTranslateRegion (graphics->clip, dx, dy);
	if (status == Ok)
		gdip_set_cairo_clipping (graphics);
	return status;
}

GpStatus
GdipTranslateClipI (GpGraphics *graphics, int dx, int dy)
{
	GpStatus status;

	if (!graphics)
		return InvalidParameter;

	status = GdipTranslateRegionI (graphics->clip, dx, dy);
	if (status == Ok)
		gdip_set_cairo_clipping (graphics);
	return status;
}

GpStatus
GdipGetClip (GpGraphics *graphics, GpRegion *region)
{
	if (!graphics || !region)
		return InvalidParameter;

	gdip_clear_region (region);
	gdip_copy_region (graphics->clip, region);

	if (gdip_is_matrix_empty (graphics->clip_matrix))
		return Ok;
	return GdipTransformRegion (region, graphics->clip_matrix);
}


GpStatus
GdipGetClipBounds (GpGraphics *graphics, GpRectF *rect)
{
	GpStatus status;
	GpRegion *work;

	if (!graphics || !rect)
		return InvalidParameter;

	/* if the matrix is empty, avoid region cloning and transform */
	if (gdip_is_matrix_empty (graphics->clip_matrix)) {
		work = graphics->clip;
	} else {
		GdipCloneRegion (graphics->clip, &work);
		GdipTransformRegion (work, graphics->clip_matrix);
	}

	status = GdipGetRegionBounds (work, graphics, rect);

	if (work != graphics->clip)
		GdipDeleteRegion (work);

	return status;
}

GpStatus
GdipGetClipBoundsI (GpGraphics *graphics, GpRect *rect)
{
	GpRectF rectF;
	Status status;

	if (!graphics || !rect)
		return InvalidParameter;
	
	status =  GdipGetRegionBounds (graphics->clip, graphics, &rectF);
	
	if (status != Ok)
		return status;

	rect->X = rectF.X;
	rect->Y = rectF.Y;
	rect->Width = rectF.Width;
	rect->Height = rectF.Height;
	
	return Ok;
}

GpStatus
GdipIsClipEmpty (GpGraphics *graphics, BOOL *result)
{
	if (!graphics)
		return InvalidParameter;

	return GdipIsEmptyRegion (graphics->clip, graphics, result);
}

GpStatus
GdipSetVisibleClip_linux (GpGraphics *graphics, GpRect *rect)
{
	if (!graphics || !rect)
		return InvalidParameter;		
		
	graphics->bounds.X = rect->X;
	graphics->bounds.Y = rect->Y;
	graphics->bounds.Width = rect->Width;
	graphics->bounds.Height = rect->Height;
	return Ok;
}

GpStatus
GdipGetVisibleClipBounds (GpGraphics *graphics, GpRectF *rect)
{
	if (!graphics || !rect)
		return InvalidParameter;

	if (!gdip_is_InfiniteRegion (graphics->clip)) {
		GpRectF clipbound;
		GpStatus status = GdipGetClipBounds (graphics, &clipbound);
		if (status != Ok)
			return status;

		/* intersect clipping with bounds (for clips bigger than the graphics) */
		rect->X = (clipbound.X > graphics->bounds.X) ? clipbound.X : graphics->bounds.X;
		rect->Y = (clipbound.Y > graphics->bounds.Y) ? clipbound.Y : graphics->bounds.Y;
		rect->Width = (((clipbound.X + clipbound.Width) < (graphics->bounds.X + graphics->bounds.Width)) ? 
			(clipbound.X + clipbound.Width) : (graphics->bounds.X + graphics->bounds.Width)) - rect->X;
		rect->Height = (((clipbound.Y + clipbound.Height) < (graphics->bounds.Y + graphics->bounds.Height)) ? 
			(clipbound.Y + clipbound.Height) : (graphics->bounds.Y + graphics->bounds.Height)) - rect->Y;
	} else {
		rect->X = graphics->bounds.X;
		rect->Y = graphics->bounds.Y;
		rect->Width = graphics->bounds.Width;
		rect->Height = graphics->bounds.Height;
	}
	return Ok;
}

GpStatus
GdipGetVisibleClipBoundsI (GpGraphics *graphics, GpRect *rect)
{
	GpStatus status;
	GpRectF rectF;
	
	if (!graphics || !rect)
		return InvalidParameter;
	
	status = GdipGetVisibleClipBounds (graphics, &rectF);
	if (status != Ok)
		return status;
	
	rect->X = rectF.X;
	rect->Y = rectF.Y;
	rect->Width = rectF.Width;
	rect->Height = rectF.Height;
	
	return Ok;
}

GpStatus
GdipIsVisibleClipEmpty (GpGraphics *graphics, BOOL *result)
{
	if (!graphics || !result)
		return InvalidParameter;
		
	if (graphics->bounds.Width == 0 || graphics->bounds.Height == 0)
		*result = TRUE;
	else
		*result = FALSE;
		
	return Ok;
	
}


GpStatus
GdipIsVisiblePoint (GpGraphics *graphics, float x, float y, BOOL *result)
{
	GpRectF rectF;
	
	if (!graphics || !result)
		return InvalidParameter;
		
	rectF.X = graphics->bounds.X;
	rectF.Y = graphics->bounds.Y;
	rectF.Width = graphics->bounds.Width;
	rectF.Height = graphics->bounds.Height;	

        *result = gdip_is_Point_in_RectF_inclusive (x, y, &rectF);

        return Ok;
}

GpStatus
GdipIsVisiblePointI (GpGraphics *graphics, int x, int y, BOOL *result)
{
	return GdipIsVisiblePoint (graphics, (float) x, (float) y, result);
}

GpStatus
GdipIsVisibleRect (GpGraphics *graphics, float x, float y, float width, float height, BOOL *result)
{
	BOOL found = FALSE;
	float posy, posx;
	GpRectF recthit, boundsF;

	if (!graphics || !result)
		return InvalidParameter;

	if (width ==0 || height ==0) {
		*result = FALSE;
		return Ok;
	}
	
	boundsF.X = graphics->bounds.X;
	boundsF.Y = graphics->bounds.Y;
	boundsF.Width = graphics->bounds.Width;
	boundsF.Height = graphics->bounds.Height;	

	recthit.X = x; recthit.Y = y;
	recthit.Width = width; recthit.Height = height;

	/* Any point of intersection ?*/
	for (posy = 0; posy < recthit.Height+1; posy++) {	
		for (posx = 0; posx < recthit.Width +1; posx++) {
			if (gdip_is_Point_in_RectF_inclusive (recthit.X + posx , recthit.Y + posy, &boundsF) == TRUE) {
				found = TRUE;
				break;
			}
		}
	}
	
	*result = found;
	return Ok;
}

GpStatus
GdipIsVisibleRectI (GpGraphics *graphics, int x, int y, int width, int height, BOOL *result)
{
	return GdipIsVisibleRect (graphics, x, y, width, height, result);
}

GpStatus
GdipSetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode)
{
	if (!graphics)
		return InvalidParameter;

	graphics->composite_mode = compositingMode;

	switch (compositingMode) {
		case CompositingModeSourceOver: {
			cairo_set_operator(graphics->ct, CAIRO_OPERATOR_OVER);
			break;
		}

		case CompositingModeSourceCopy: {
			cairo_set_operator(graphics->ct, CAIRO_OPERATOR_SOURCE);
			break;
		}
	}
	return Ok;
}

GpStatus
GdipGetCompositingMode (GpGraphics *graphics, CompositingMode *compositingMode)
{
	if (!graphics || !compositingMode)
		return InvalidParameter;

	*compositingMode = graphics->composite_mode;
	return Ok;
}

GpStatus
GdipSetCompositingQuality (GpGraphics *graphics, CompositingQuality compositingQuality)
{
	if (!graphics)
		return InvalidParameter;

	/* In Cairo there is no way of setting this, always use high quality */

	graphics->composite_quality = compositingQuality;
	return Ok;
}

GpStatus
GdipGetCompositingQuality (GpGraphics *graphics, CompositingQuality *compositingQuality)
{
	if (!graphics || !compositingQuality)
		return InvalidParameter;

	*compositingQuality = graphics->composite_quality;
	return Ok;
}

GpStatus
GdipGetNearestColor (GpGraphics *graphics, ARGB *argb)
{
	return Ok;
}

GpStatus
GdipSetPageScale (GpGraphics *graphics, float scale)
{
	if (!graphics) 
		return InvalidParameter;
	
	graphics->scale = scale;	
	return Ok;
}

GpStatus
GdipGetPageScale (GpGraphics *graphics, float *scale)
{
	if (!graphics | !scale)
		return InvalidParameter;

	*scale = graphics->scale;
	return Ok;
}

GpStatus
GdipSetPageUnit (GpGraphics *graphics, GpUnit unit)
{
	if (!graphics) 
		return InvalidParameter;

	graphics->page_unit = unit;
	return Ok;
}

GpStatus
GdipGetPageUnit (GpGraphics *graphics, GpUnit *unit)
{
	if (!graphics || !unit)
		return InvalidParameter;

	*unit = graphics->page_unit;
	return Ok;
}

GpStatus
GdipTransformPoints (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPointF *points, int count)
{
        static int      called = 0;

        if (!called) {
                printf("NOT IMPLEMENTED YET:GdipTransformPoints (GpGraphics *graphics, GpCoordinateSpace destSpace %d, GpCoordinateSpace srcSpace %d, GpPointF *points, int count %d)\n", destSpace, srcSpace, count);
        }
        /* return NotImplemented; */
        return Ok;
}

GpStatus
GdipTransformPointsI (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPoint *points, int count)
{
        static int      called = 0;

        if (!called) {
                printf("NOT IMPLEMENTED YET:GdipTransformPointsI (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPoint *points, int count)\n");
        }
        /* return NotImplemented; */
        return Ok;
}
