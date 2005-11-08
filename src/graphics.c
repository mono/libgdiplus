/*
 * graphics.c
 *
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
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
 *
 */

#include "gdip.h"
#include "brush.h"
#include <math.h>
#include <glib.h>

extern BOOL gdip_is_Point_in_RectF_inclusive (float x, float y, GpRectF* rect);
extern BOOL gdip_is_Point_in_RectF_inclusive (float x, float y, GpRectF* rect);
extern FT_Face gdip_cairo_ft_font_lock_face (cairo_font_face_t *cairofnt);
extern void gdip_cairo_ft_font_unlock_face (cairo_font_face_t *cairofnt);
void gdip_set_cairo_clipping (GpGraphics *graphics);
    
extern cairo_filter_t gdip_get_cairo_filter (InterpolationMode imode);

#define	NO_CAIRO_AA

#define MAX_GRAPHICS_STATE_STACK 512

void
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
	graphics->bounds.X = graphics->bounds.Y = graphics->bounds.Width = graphics->bounds.Height = 0;
	graphics->page_unit = UnitDisplay;
	graphics->scale = 1.0f;
	graphics->interpolation = InterpolationModeBilinear;
	graphics->last_pen = NULL;
	graphics->last_brush = NULL;
	graphics->composite_quality = CompositingQualityDefault;
	graphics->composite_mode = CompositingModeSourceOver;
	graphics->text_mode = TextRenderingHintSystemDefault;
	graphics->pixel_mode = PixelOffsetModeDefault;
	graphics->saved_status = NULL;
	graphics->saved_status_pos = 0;
	graphics->render_origin_x = 0;
	graphics->render_origin_y = 0;

	GdipSetSmoothingMode(graphics, SmoothingModeNone);
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
make_ellipse (GpGraphics *graphics, float x, float y, float width, float height, float aa_offset_x, float aa_offset_y)
{
        double rx = width / 2;
        double ry = height / 2;
        double cx = x + rx + aa_offset_x;
        double cy = y + ry + aa_offset_y;

        cairo_move_to (graphics->ct, cx + rx, cy);

        /* an approximate of the ellipse by drawing a curve in each
         * quartrant */
        cairo_curve_to (graphics->ct,
                        cx + rx, cy - C1 * ry,
                        cx + C1 * rx, cy - ry,
                        cx, cy - ry);
        
        cairo_curve_to (graphics->ct,
                        cx - C1 * rx, cy - ry,
                        cx - rx, cy - C1 * ry,
                        cx - rx, cy);

        cairo_curve_to (graphics->ct,
                        cx - rx, cy + C1 * ry,
                        cx - C1 * rx, cy + ry,
                        cx, cy + ry);
                
        cairo_curve_to (graphics->ct,
                        cx + C1 * rx, cy + ry,
                        cx + rx, cy + C1 * ry,
                        cx + rx, cy);

        cairo_close_path (graphics->ct);
}

static void
make_polygon (GpGraphics *graphics, GpPointF *points, int count, float aa_offset_x, float aa_offset_y)
{
        int i;
        cairo_move_to (graphics->ct, points [0].X + aa_offset_x, points [0].Y + aa_offset_y);

        for (i = 0; i < count; i++)
                cairo_line_to (graphics->ct, points [i].X + aa_offset_x, points [i].Y + aa_offset_y);

        /*
         * Draw a line from the last point back to the first point if
         * they're not the same
         */
        if (points [0].X != points [count].X && points [0].Y != points [count].Y)
                cairo_line_to (graphics->ct, points [0].X + aa_offset_x, points [0].Y + aa_offset_y);

        cairo_close_path (graphics->ct);
}

static void
make_polygon_from_integers (GpGraphics *graphics, GpPoint *points, int count, float aa_offset_x, float aa_offset_y)
{
        int i;
        cairo_move_to (graphics->ct, points [0].X + aa_offset_x, points [0].Y + aa_offset_y);

        for (i = 0; i < count; i++)
                cairo_line_to (graphics->ct, points [i].X + aa_offset_x, points [i].Y + aa_offset_y);

        /*
         * Draw a line from the last point back to the first point if
         * they're not the same
         */
        if (points [0].X != points [count].X && points [0].Y != points [count].Y)
                cairo_line_to (graphics->ct, points [0].X + aa_offset_x, points [0].Y + aa_offset_y);

        cairo_close_path (graphics->ct);
}

/*
 * Based on the algorithm described in
 *      http://www.stillhq.com/ctpfaq/2002/03/c1088.html#AEN1212
 */
static void
make_arc (GpGraphics *graphics, bool start, float x, float y, float width,
	  float height, float startAngle, float endAngle, float aa_offset_x, float aa_offset_y)
{       
        float rx = width / 2;
        float ry = height / 2;
        
        /* center */
        float cx = x + rx;
        float cy = y + ry;

        /* angles in radians */        
        float alpha = startAngle * PI / 180;
        float beta = endAngle * PI / 180;

        float delta = beta - alpha;
        float bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta /2);

        double sin_alpha = sin (alpha);
        double sin_beta = sin (beta);
        double cos_alpha = cos (alpha);
        double cos_beta = cos (beta);

        /* starting point */
        double sx = cx + rx * cos_alpha;
        double sy = cy + ry * sin_alpha;
	
        /* don't move to starting point if we're continuing an existing curve */
        if (start)
                cairo_move_to (graphics->ct, sx + aa_offset_x, sy + aa_offset_y);

	cx += graphics->aa_offset_x;
	cy += graphics->aa_offset_y;

        cairo_curve_to (graphics->ct,
                        cx + rx * (cos_alpha - bcp * sin_alpha),
                        cy + ry * (sin_alpha + bcp * cos_alpha),
                        cx + rx * (cos_beta  + bcp * sin_beta),
                        cy + ry * (sin_beta  - bcp * cos_beta),
                        cx + rx *  cos_beta,
                        cy + ry *  sin_beta);
}

static void
make_arcs (GpGraphics *graphics, float x, float y, float width, float height, float startAngle, float sweepAngle, float aa_offset_x, float aa_offset_y)
{
	int i;
	float drawn = 0;
	float endAngle = startAngle + sweepAngle;	
	int sign = (endAngle > 0) ? 1 : -1;
	int increment = sign * 90; 
	
	if (abs (sweepAngle) >= 360) {
		make_ellipse (graphics, x, y, width, height, aa_offset_x, aa_offset_y);
		return;
	}

	/* i is the number of sub-arcs drawn, each sub-arc can be at most 90 degrees.*/
	/* there can be no more then 4 subarcs, ie. 90 + 90 + 90 + (something less than 90) */
	for (i = 0; i < 4; i++) {
		float current = startAngle + drawn;

		/* we've drawn enough */
		if (abs (current) >= abs (endAngle))
			break;
		
		/* if we are not done yet */
		float additional = abs (current + increment) < abs (endAngle) ?
				increment : /* add the default increment */
				endAngle - current; /* otherwise, add the remainder */
		
		make_arc (graphics,
			  (i == 0) ? TRUE : FALSE,  /* only move to the starting pt in the 1st iteration */
			  x, y, width, height,      /* bounding rectangle */
			  current, current + additional, aa_offset_x, aa_offset_y);
		drawn += additional;		
	}
}

static void
make_pie (GpGraphics *graphics, float x, float y, 
          float width, float height, float startAngle, float sweepAngle, float aa_offset_x, float aa_offset_y)
{
	float rx = width / 2;
	float ry = height / 2;

	/* center */
	float cx = x + rx + aa_offset_x;
	float cy = y + ry + aa_offset_y;

	/* angles in radians */        
	float alpha = startAngle * PI / 180;

	double sin_alpha = sin (alpha);
	double cos_alpha = cos (alpha);

	/* move to center */
	cairo_move_to (graphics->ct, cx, cy);

	/* draw pie edge */
	cairo_line_to (graphics->ct,
			cx + rx * cos_alpha, 
			cy + ry * sin_alpha);

	/* draw the arcs */
	make_arcs (graphics, x, y, width, height, startAngle, sweepAngle, aa_offset_x, aa_offset_y);

	/* draws line back to center */
	cairo_line_to (graphics->ct, cx + aa_offset_x, cy + aa_offset_y);
}

static cairo_fill_rule_t
convert_fill_mode (GpFillMode fill_mode)
{
        if (fill_mode == FillModeAlternate) 
                return CAIRO_FILL_RULE_EVEN_ODD;
        else
                return CAIRO_FILL_RULE_WINDING;
}


#ifdef CAIRO_HAS_XLIB_SURFACE

GpStatus 
GdipCreateFromHDC (void *hDC, GpGraphics **graphics)
{
	GpGraphics *clone = (GpGraphics *) hDC;
	cairo_surface_t *surface;
	
	int x, y;
	unsigned int w, h, border_w, depth;
	Window root;
	XGetGeometry (clone->display, clone->drawable, &root,
		      &x, &y, &w, &h, &border_w, &depth);
	
	surface = cairo_xlib_surface_create(clone->display, clone->drawable,
	    DefaultVisual(clone->display, DefaultScreen(clone->display)),
	    w, h);
			
	*graphics = gdip_graphics_new (surface);
	if (!*graphics)
		return OutOfMemory;

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

GpStatus
GdipCreateFromQuartz_macosx (void *ctx, int width, int height, GpGraphics **graphics)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

        cairo_surface_t *surface;

	surface = cairo_quartz_surface_create(ctx, width, height);
	
	
	*graphics = gdip_graphics_new(surface);
	
	(*graphics)->bounds.Width = width;
	(*graphics)->bounds.Height = height;

	(*graphics)->type = gtOSXDrawable;

	return Ok;
}

#endif

#ifdef CAIRO_HAS_XLIB_SURFACE

GpStatus
GdipCreateFromXDrawable_linux(Drawable d, Display *dpy, GpGraphics **graphics)
{
	Window root_ignore;
	GpRect bounds;
	int bwidth_ignore, depth_ignore;
        cairo_surface_t *surface;
		
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	XGetGeometry (dpy, d, &root_ignore, &bounds.X,  &bounds.Y,
		(unsigned int *)&bounds.Width, (unsigned int *)&bounds.Height, 
		(unsigned int *)&bwidth_ignore, (unsigned int *)&depth_ignore);
	
	surface = cairo_xlib_surface_create(dpy, d,
		    DefaultVisual(dpy, DefaultScreen(dpy)),
		    bounds.Width, bounds.Height);
	
	
	*graphics = gdip_graphics_new(surface);
	cairo_surface_destroy (surface);

	(*graphics)->type = gtX11Drawable;
	(*graphics)->display = dpy;
	(*graphics)->drawable = d;

	GdipSetVisibleClip_linux (*graphics, &bounds);
	return Ok;
}

#endif

GpStatus 
GdipDeleteGraphics (GpGraphics *graphics)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	/* We don't destroy image because we did not create one. */
	if (graphics->copy_of_ctm) {
		GdipDeleteMatrix (graphics->copy_of_ctm);
		graphics->copy_of_ctm = NULL;
	}
	GdipDeleteRegion (graphics->clip);

	if (graphics->ct) {
		cairo_destroy (graphics->ct);
		graphics->ct = NULL;
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	if (graphicsState >= MAX_GRAPHICS_STATE_STACK || graphicsState > graphics->saved_status_pos)
		return InvalidParameter;

	pos_state = graphics->saved_status;
	pos_state += graphicsState;	

	gdip_cairo_matrix_copy (graphics->copy_of_ctm, &pos_state->matrix);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	if (graphics->clip)
		GdipDeleteRegion (graphics->clip);

	/* Save from GpState to Graphics  */
	gdip_cairo_matrix_copy (&pos_state->matrix, graphics->copy_of_ctm);
	GdipSetRenderingOrigin (graphics, pos_state->org_x, pos_state->org_y);
	GdipCloneRegion (pos_state->clip, &graphics->clip);

	graphics->composite_mode = pos_state->composite_mode;
	graphics->composite_quality = pos_state->composite_quality;
	graphics->scale = pos_state->scale;
	graphics->interpolation = pos_state->interpolation;
	graphics->page_unit = pos_state->page_unit;
	graphics->text_mode = pos_state->text_mode;
	graphics->pixel_mode = pos_state->pixel_mode;

	graphics->saved_status_pos = graphicsState;
	GdipSetSmoothingMode(graphics, pos_state->draw_mode);
	return Ok;
}

GpStatus 
GdipSaveGraphics (GpGraphics *graphics, unsigned int *state)
{
	GpState* pos_state;
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (state != NULL, InvalidParameter);

	if (graphics->saved_status == NULL) {
		graphics->saved_status = GdipCalloc (MAX_GRAPHICS_STATE_STACK, sizeof (GpState));
		graphics->saved_status_pos = 0;
	}

	if (graphics->saved_status_pos >= MAX_GRAPHICS_STATE_STACK) 
		return OutOfMemory;

	pos_state = graphics->saved_status;
	pos_state += graphics->saved_status_pos;

	if (pos_state->clip)
		GdipDeleteRegion (pos_state->clip);

	/* Save from Graphics to GpState */
	gdip_cairo_matrix_copy (&pos_state->matrix, graphics->copy_of_ctm);
	GdipGetRenderingOrigin (graphics, &pos_state->org_x, &pos_state->org_y);
	GdipCloneRegion (graphics->clip, &pos_state->clip);

	pos_state->composite_mode = graphics->composite_mode;
	pos_state->composite_quality = graphics->composite_quality;
	pos_state->interpolation = graphics->interpolation;
	pos_state->page_unit = graphics->page_unit;
	pos_state->scale = graphics->scale;
	pos_state->draw_mode = graphics->draw_mode;
	pos_state->text_mode = graphics->text_mode;
	pos_state->pixel_mode = graphics->pixel_mode;
	
	*state = graphics->saved_status_pos;
	graphics->saved_status_pos++;
	return Ok;
}

GpStatus
GdipResetWorldTransform (GpGraphics *graphics)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	cairo_matrix_init_identity (graphics->copy_of_ctm);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	gdip_cairo_matrix_copy (graphics->copy_of_ctm, matrix);
        cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
        return Ok;
}

GpStatus 
GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        cairo_get_matrix (graphics->ct, matrix);
        return Ok;
}

GpStatus
GdipMultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order)
{
        Status s;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	s = GdipMultiplyMatrix (graphics->copy_of_ctm, matrix, order);
        
        if (s != Ok)
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

GpStatus 
GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order)
{
	GpStatus s;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	s = GdipRotateMatrix (graphics->copy_of_ctm, angle, order);
	
        if (s != Ok)
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

GpStatus
GdipScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order)
{
        GpStatus s;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	s = GdipScaleMatrix (graphics->copy_of_ctm, sx, sy, order);

        if (s != Ok)
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return s;
        }
}

GpStatus 
GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order)
{
        GpStatus s;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	s = GdipTranslateMatrix (graphics->copy_of_ctm, dx, dy, order);

        if (s != Ok) 
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

/*
 * Draw operations.
 */

GpStatus
GdipDrawArc (GpGraphics *graphics, GpPen *pen, 
	     float x, float y, float width, float height, 
	     float startAngle, float sweepAngle)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We should
	 * have it set already.
	 */

	make_arcs (graphics, x, y, width, height, startAngle, sweepAngle, graphics->aa_offset_x, graphics->aa_offset_y);

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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
        cairo_move_to (graphics->ct, x1 + graphics->aa_offset_x, y1 + graphics->aa_offset_y);
        cairo_curve_to (graphics->ct, x2 + graphics->aa_offset_x, y2 + graphics->aa_offset_y, x3 + graphics->aa_offset_x, y3 + graphics->aa_offset_y, x4 + graphics->aa_offset_x, y4 + graphics->aa_offset_y);

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
GdipDrawBeziers (GpGraphics *graphics, GpPen *pen,
		 GpPointF *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
        cairo_move_to (graphics->ct, points [0].X + graphics->aa_offset_x, points [0].Y + graphics->aa_offset_y);

        for (i = 0; i < count - 3; i += 3) {
                j = i + 1;
                k = i + 2;
                cairo_curve_to (graphics->ct,
                                points [i].X + graphics->aa_offset_x, points [i].Y + graphics->aa_offset_y,
                                points [j].X + graphics->aa_offset_x, points [j].Y + graphics->aa_offset_y,
                                points [k].X + graphics->aa_offset_x, points [k].Y + graphics->aa_offset_y);
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
GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen,
		  GpPoint *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
        cairo_move_to (graphics->ct, points [0].X + graphics->aa_offset_x, points [0].Y + graphics->aa_offset_y);

        for (i = 0; i < count - 3; i += 3) {
                j = i + 1;
                k = i + 2;
                cairo_curve_to (graphics->ct,
                                points [i].X + graphics->aa_offset_x, points [i].Y + graphics->aa_offset_y,
                                points [j].X + graphics->aa_offset_x, points [j].Y + graphics->aa_offset_y,
                                points [k].X + graphics->aa_offset_x, points [k].Y + graphics->aa_offset_y);
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_ellipse (graphics, x, y, width, height, graphics->aa_offset_x, graphics->aa_offset_y);

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

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	gdip_cairo_matrix_copy (&saved, graphics->copy_of_ctm);
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	cairo_move_to (graphics->ct, x1 + graphics->aa_offset_x, y1 + graphics->aa_offset_y);
	cairo_line_to (graphics->ct, x2 + graphics->aa_offset_x, y2 + graphics->aa_offset_y);

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
GdipDrawLines (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count)
{
	int i;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
	g_return_val_if_fail (count >= 2, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	cairo_move_to (graphics->ct, points [0].X + graphics->aa_offset_x, points [0].Y + graphics->aa_offset_y);
	for (i = 1; i < count; i++)
		cairo_line_to (graphics->ct, points [i].X + graphics->aa_offset_x, points [i].Y + graphics->aa_offset_y);

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
GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count)
{
	int i;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
	g_return_val_if_fail (count >= 2, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	cairo_move_to (graphics->ct, points [0].X + graphics->aa_offset_x, points [0].Y + graphics->aa_offset_y);
	for (i = 1; i < count; i++)
		cairo_line_to (graphics->ct, points [i].X + graphics->aa_offset_x, points [i].Y + graphics->aa_offset_y);

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
gdip_plot_path (GpGraphics *graphics, GpPath *path, float aa_offset_x, float aa_offset_y)
{
        int length = path->count;
        int i, idx = 0;

        for (i = 0; i < length; ++i) {
                GpPointF pt = g_array_index (path->points, GpPointF, i);
                byte type = g_array_index (path->types, byte, i);
                GpPointF pts [3];

		/* mask the bits so that we get only the type value not the other flags */
                switch (type & PathPointTypePathTypeMask) {
                case PathPointTypeStart:
                        cairo_move_to (graphics->ct, pt.X + aa_offset_x, pt.Y + aa_offset_y);
                        break;

                case PathPointTypeLine:
                        cairo_line_to (graphics->ct, pt.X + aa_offset_x, pt.Y + aa_offset_y);
                        break;

                case PathPointTypeBezier:
                        /* make sure we only add at most 3 points to pts */
                        if (idx < 3) {
                                pts [idx] = pt;
                                idx ++;
                        }

                        /* once we've added 3 pts, we can draw the curve */
                        if (idx == 3) {
                                cairo_curve_to (graphics->ct,
                                                pts [0].X + aa_offset_x, pts [0].Y + aa_offset_y,
                                                pts [1].X + aa_offset_x, pts [1].Y + aa_offset_y,
                                                pts [2].X + aa_offset_x, pts [2].Y + aa_offset_y);
                                idx = 0;
                        }

                        break;
                default:
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (path != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	status = gdip_plot_path (graphics, path, graphics->aa_offset_x, graphics->aa_offset_y);

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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_pie (graphics, x, y, width, height, startAngle, sweepAngle, graphics->aa_offset_x, graphics->aa_offset_y);

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
GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon (graphics, points, count, graphics->aa_offset_x, graphics->aa_offset_y);

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
GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon_from_integers (graphics, points, count, graphics->aa_offset_x, graphics->aa_offset_y);

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

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        gdip_cairo_matrix_copy (&saved, graphics->copy_of_ctm);
        cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
	
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	cairo_rectangle (graphics->ct, x + graphics->aa_offset_x, y + graphics->aa_offset_y, width, height);

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
GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GpRectF *rects, int count)
{
	int i;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (rects != NULL, InvalidParameter);
	g_return_val_if_fail (count > 0, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++)
		cairo_rectangle (graphics->ct, rects [i].X + graphics->aa_offset_x, rects [i].Y + graphics->aa_offset_y, rects [i].Width, rects [i].Height);

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
GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GpRect *rects, int count)
{
	int i;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (rects != NULL, InvalidParameter);
	g_return_val_if_fail (count > 0, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++)
		cairo_rectangle (graphics->ct, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height);

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
make_curve (GpGraphics *graphics, GpPointF *points, GpPointF *tangents, int count, _CurveType type, float aa_offset_x, float aa_offset_y)
{
        int i, length = count;

        if (count <= 0)
                return;

        /* don't close to the curve if it's an open curve */
        if (type == CURVE_OPEN)
                length = count - 1;
        
        cairo_move_to (graphics->ct,
                        points [0].X + aa_offset_x, points [0].Y + aa_offset_y);

        for (i = 1; i <= length; i++) {
                int j = i - 1;
                int k = (i < count) ? i : 0;

                double x1 = points [j].X + tangents [j].X;
                double y1 = points [j].Y + tangents [j].Y;

                double x2 = points [k].X - tangents [k].X;
                double y2 = points [k].Y - tangents [k].Y;

                double x3 = points [k].X;
                double y3 = points [k].Y;

                cairo_curve_to (
                        graphics->ct,
                        x1 + aa_offset_x, y1 + aa_offset_y, x2 + aa_offset_x, y2 + aa_offset_y, x3 + aa_offset_x, y3 + aa_offset_y);
        }

        if (type == CURVE_CLOSE)
                cairo_close_path (graphics->ct);
}

GpStatus
GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count)
{
        return GdipDrawClosedCurve2 (graphics, pen, points, count, 0.5f);
}

GpStatus
GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count)
{
        return GdipDrawClosedCurve2I (graphics, pen, points, count, 0.5f);
}

GpStatus
GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, float tension)
{
        GpPointF *tangents;

        /* when tension is 0, draw straight lines */
        if (tension == 0)
                return GdipDrawPolygon (graphics, pen, points, count);

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);
        
	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	make_curve (graphics, points, tangents, count, CURVE_CLOSE, graphics->aa_offset_x, graphics->aa_offset_y);

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
GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, float tension)
{
	GpPointF *pt;
	GpStatus s;

	g_return_val_if_fail (points != NULL, InvalidParameter);

	pt = convert_points (points, count);

	s = GdipDrawClosedCurve (graphics, pen, pt, count);

        GdipFree (pt);

        return s;
}

GpStatus
GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count) 
{
        return GdipDrawCurve3 (graphics, pen, points, count, 0, count - 1, 0.5f);
}

GpStatus
GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count) 
{
        return GdipDrawCurve3I (graphics, pen, points, count, 0, count - 1, 0.5f);
}

GpStatus
GdipDrawCurve2 (GpGraphics *graphics, GpPen* pen, GpPointF *points, int count, float tension)
{
        return GdipDrawCurve3 (graphics, pen, points, count, 0, count - 1, tension);
}

GpStatus
GdipDrawCurve2I (GpGraphics *graphics, GpPen* pen, GpPoint *points, int count, float tension)
{
        return GdipDrawCurve3I (graphics, pen, points, count, 0, count - 1, tension);
}

GpStatus
GdipDrawCurve3 (GpGraphics *graphics, GpPen* pen, GpPointF *points, int count, int offset, float numOfSegments, float tension)
{
        GpPointF *tangents;

        /* draw lines if tension = 0 */
        if (tension == 0)
                return GdipDrawLines (graphics, pen, points, count);

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
        tangents = gdip_open_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
        make_curve (graphics, points, tangents, count, CURVE_OPEN, graphics->aa_offset_x, graphics->aa_offset_y);

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
GdipDrawCurve3I (GpGraphics *graphics, GpPen* pen, GpPoint *points, int count, int offset, float numOfSegments, float tension)
{
	GpPointF *pf;
	GpStatus s;

	g_return_val_if_fail (points != NULL, InvalidParameter);

	pf = convert_points (points, count);

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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_ellipse (graphics, x, y, width, height, 0, 0);
	
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	cairo_rectangle (graphics->ct, x, y, width, height);

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
GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GpRectF *rects, int count)
{
	int i;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (rects != NULL, InvalidParameter);
	g_return_val_if_fail (count > 0, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++)
		cairo_rectangle (graphics->ct, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height);

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
GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GpRect *rects, int count)
{
	int i;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (rects != NULL, InvalidParameter);
	g_return_val_if_fail (count > 0, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	for (i = 0; i < count; i++)
		cairo_rectangle (graphics->ct, rects [i].X, rects [i].Y, rects [i].Width, rects [i].Height);

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
GdipFillPie(GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, float startAngle, float sweepAngle)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_pie (graphics, x, y, width, height, startAngle, sweepAngle, 0, 0);

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
GdipFillPieI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
        return GdipFillPie (graphics, brush, x, y, width, height, startAngle, sweepAngle);
}

GpStatus
GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, 
		 GpPointF *points, int count, GpFillMode fillMode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon (graphics, points, count, 0, 0);
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
GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
	GpStatus status;
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (path != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	status = gdip_plot_path (graphics, path, 0, 0);

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
GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, 
		  GpPoint *points, int count, GpFillMode fillMode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	make_polygon_from_integers (graphics, points, count, 0, 0);
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
GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count)
{
        return GdipFillPolygon (graphics, brush, points, count, FillModeAlternate);
}

GpStatus
GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count)
{
        return GdipFillPolygonI (graphics, brush, points, count, FillModeAlternate);
}

GpStatus
GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count)
{
        return GdipFillClosedCurve2 (graphics, brush, points, count, 0.5f);
}

GpStatus
GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count)
{
        return GdipFillClosedCurve2I (graphics, brush, points, count, 0.5f);
}

GpStatus
GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count, float tension)
{
        GpPointF *tangents;

        /* when tension is 0, the edges are straight lines */
        if (tension == 0)
                return GdipFillPolygon2 (graphics, brush, points, count);

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (points != NULL, InvalidParameter);

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
	tangents = gdip_closed_curve_tangents (CURVE_MIN_TERMS, points, count, tension);
	make_curve (graphics, points, tangents, count, CURVE_CLOSE, 0, 0);

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
GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count, float tension)
{
	GpPointF *pt;
        GpStatus s;
	
	g_return_val_if_fail (points != NULL, InvalidParameter);

	pt  = convert_points (points, count);

	s = GdipFillClosedCurve2 (graphics, brush, pt, count, tension);

        GdipFree (pt);

        return s;
}

GpStatus
GdipFillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region)
{
        int i;
        GpRectF *rect;

        if (!graphics || !brush || !region)
		return InvalidParameter;

        if (!region->rects || region->cnt==0)
                return Ok;

	/* We use graphics->copy_of_ctm matrix for path creation. We
	 * should have it set already.
	 */
        for (i = 0, rect = region->rects; i < region->cnt; i++, rect++)
		cairo_rectangle (graphics->ct, rect->X, rect->Y, rect->Width, rect->Height);

	/* We do brush setup just before filling. */
	gdip_brush_setup (graphics, brush);
	cairo_fill (graphics->ct);

	/* Set the matrix back to graphics->copy_of_ctm for other functions.
	 * This overwrites the matrix set by brush setup.
	 */
	cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);

	return gdip_get_status (cairo_status (graphics->ct));
}


#undef DRAWSTRING_DEBUG

static int
CalculateStringWidths (cairo_t *ct, GDIPCONST GpFont *gdiFont, GDIPCONST gunichar2 *stringUnicode, unsigned long StringDetailElements, GpStringDetailStruct *StringDetails)
{
	size_t			i;
	cairo_text_extents_t	ext;
	GpStringDetailStruct	*CurrentDetail;
	unsigned char		utf8[5];


	CurrentDetail = StringDetails;

	for (i = 0; i < StringDetailElements; i++) {
		utf8[utf8_encode_ucs2char(*(stringUnicode + i), utf8)] = '\0';
		cairo_text_extents(ct, (const char *) utf8, &ext);
		CurrentDetail->Width = ext.x_advance;
		CurrentDetail++;
	}

	return StringDetailElements;
}

#ifdef NOT_USED
/* Currently not used */
static int
CalculateStringWidthsUTF8 (cairo_t *ct, GDIPCONST GpFont *gdiFont, const unsigned char *utf8, unsigned long StringDetailElements, GpStringDetailStruct *StringDetails)
{
	FT_Face			face;
	size_t			i;
	gunichar		*ucs4 = NULL;
	cairo_font_face_t	*Font;
	GpStringDetailStruct	*CurrentDetail;
	glong			NumOfGlyphs;
	cairo_matrix_t		matrix;

#ifdef DRAWSTRING_DEBUG
	printf("CalculateStringWidths(font, %s, %d, details) called\n", utf8, StringDetailElements);
#endif

	Font = (cairo_font_face_t *)gdiFont->cairofnt;
	face = gdip_cairo_ft_font_lock_face(Font);

        cairo_get_font_matrix(ct, &matrix);	
	cairo_matrix_scale(&matrix, gdiFont->sizeInPixels, gdiFont->sizeInPixels);

	ucs4 = g_utf8_to_ucs4 ((const gchar *) utf8, (glong)-1, NULL, &NumOfGlyphs, NULL);

	if ((NumOfGlyphs == 0) || (ucs4 == NULL)) {
		return 0;
	}

	CurrentDetail=StringDetails;
	for (i = 0; i < NumOfGlyphs; i++) {
		FT_Load_Glyph (face, FT_Get_Char_Index (face, ucs4[i]), FT_LOAD_DEFAULT);
		CurrentDetail->Width = DOUBLE_FROM_26_6 (face->glyph->advance.x);
		CurrentDetail++;
	}

	gdip_cairo_ft_font_unlock_face(Font);

	GdipFree(ucs4);

	
#ifdef DRAWSTRING_DEBUG
	printf("CalculateStringWidths: string >%s< translated into %d glyphs\n", utf8, NumOfGlyphs);
#endif
	return NumOfGlyphs;
}
#endif

GpStatus
MeasureOrDrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, GDIPCONST RectF *rc, GDIPCONST GpStringFormat *format, GpBrush *brush, RectF *boundingBox, int *codepointsFitted, int *linesFilled, int draw)
{
	unsigned char		*String;		/* Holds the UTF8 version of our sanitized string */
	WCHAR			*CleanString;		/* Holds the unicode version of our sanitized string */
	unsigned long		StringLen;		/* Length of CleanString */
	cairo_matrix_t		SavedMatrix;		
	GDIPCONST WCHAR		*Src;
	WCHAR	 		*Dest;
	GpStringFormat		*fmt;
	unsigned long		i;
	unsigned long		j;
	GpStringDetailStruct	*StringDetails;		/* Contains rendering details */
	GpStringDetailStruct	*CurrentDetail;
	GpStringDetailStruct	*CurrentLineStart;	/* For rendering engine, to bump LineLen */
	float			*TabStops;
	int			NumOfTabStops;
	int			WrapPoint;		/* Array index of wrap character */
	int			WrapX;			/* Width of text at wrap character */
	float			CursorX;		/* Current X position of drawing cursor */
	float			CursorY;		/* Current Y position of drawing cursor */
	int			MaxX;			/* Largest X of cursor */
	int			MaxXatY;		/* Y coordinate of line with largest X, needed for MaxX resetting on wrap */
	int			MaxY;			/* Largest Y of cursor */
	int			FrameWidth;		/* rc->Width (or rc->Height if vertical) */
	int			FrameHeight;		/* rc->Height (or rc->Width if vertical) */
	int			AlignHorz;		/* Horizontal Alignment mode */
	int			AlignVert;		/* Vertical Alignment mode */
	int			LineHeight;		/* Height of a line with given font */
	bool			HaveHotkeys=FALSE;	/* True if we find hotkey */
	cairo_font_extents_t	FontExtent;		/* Info about our font */
	bool			SetClipping=FALSE;	/* If clipping has been set */
	cairo_font_options_t	*FontOptions;

	/* Sanity; should we check for length==0? */
	if (!graphics || !stringUnicode || !font) {
		return(InvalidParameter);
	}

#ifdef DRAWSTRING_DEBUG
	printf("GdipDrawString(...) called (length=%d, fontsize=%d)\n", length, (int)font->sizeInPixels);
#endif

	/* Check and set any defaults */
	if (!format) {
		GdipStringFormatGetGenericDefault ((GpStringFormat **)&fmt);
	} else {
		fmt=(GpStringFormat *)format;
	}
	TabStops=fmt->tabStops;
	NumOfTabStops=fmt->numtabStops;

	/* Prepare our various buffers and variables */
	StringLen=length;
	StringDetails=GdipCalloc (StringLen+1, sizeof(GpStringDetailStruct));
	CleanString=GdipAlloc (sizeof(WCHAR)*(StringLen+1));

	if (!CleanString || !StringDetails) {
		if (CleanString) {
			GdipFree (CleanString);
		}
		if (StringDetails) {
			GdipFree (StringDetails);
		}
		if (format!=fmt) {
			GdipDeleteStringFormat (fmt);
		}
		return OutOfMemory;
	}

	/*
	  Set aliasing mode
	*/
	FontOptions = cairo_font_options_create();
	
	switch(graphics->text_mode) {
		default:
		case TextRenderingHintSystemDefault: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_DEFAULT);
			//cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_NONE);
			//cairo_font_options_set_subpixel_order(FontOptions, CAIRO_SUBPIXEL_ORDER_DEFAULT);
			//cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_DEFAULT);
			//cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_DEFAULT);
			break;
		}

		// FIXME - pick matching settings for each text mode
    		case TextRenderingHintSingleBitPerPixelGridFit:
    		case TextRenderingHintSingleBitPerPixel:
    		case TextRenderingHintAntiAliasGridFit:
    		case TextRenderingHintAntiAlias: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_DEFAULT);
			break;
		}

    		case TextRenderingHintClearTypeGridFit: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_DEFAULT);
			break;
		}
	}

	cairo_set_font_options(graphics->ct, FontOptions);
	cairo_font_options_destroy(FontOptions);

	// Do we want this here?

/* Commented out until we properly save/restore AA settings; should fix bug #76135
	cairo_set_antialias(graphics->ct, CAIRO_ANTIALIAS_NONE);
*/

	/*
	   Get font size information; how expensive is the cairo stuff here? 
	*/	
	cairo_set_font_face (graphics->ct, (cairo_font_face_t*) font->cairofnt);	/* Set our font; this will also be used for later drawing */

	/* is the following ok ? */
	cairo_get_font_matrix(graphics->ct, &SavedMatrix);	/* Save the matrix */
	cairo_set_font_size (graphics->ct, font->sizeInPixels);
	cairo_font_extents (graphics->ct, &FontExtent);		/* Get the size we're looking for */
/*	cairo_font_set_transform(font->cairofnt, SavedMatrix);*/	/* Restore the matrix */

	if ((LineHeight=FontExtent.ascent)<1) {
		LineHeight=1;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Font extents: ascent:%d, descent: %d, height:%d, maxXadvance:%d, maxYadvance:%d\n", (int)FontExtent.ascent, (int)FontExtent.descent, (int)FontExtent.height, (int)FontExtent.max_x_advance, (int)FontExtent.max_y_advance);
#endif

	/* Sanitize string, remove formatting chars and build description array */
#ifdef DRAWSTRING_DEBUG
	printf("GdipDrawString(...) Sanitizing string, StringLen=%d\n", StringLen);
#endif	
	
	Src=stringUnicode;
	Dest=CleanString;
	CurrentDetail=StringDetails;
	for (i=0; i<StringLen; i++) {
		switch(*Src) {
			case '\r': { /* CR */
				Src++;
				continue;
			}

			case '\t': { /* Tab */
				if (NumOfTabStops > 0) {
					CurrentDetail->Flags |= STRING_DETAIL_TAB;
				}
				Src++;
				continue;
			}

			case '\n': { /* LF */
				CurrentDetail->Flags |= STRING_DETAIL_LF;
				CurrentDetail->Linefeeds++;
				Src++;
				continue;
			}

			case '&': {
				/* We print *all* chars if no hotkeys */
				if (fmt->hotkeyPrefix==HotkeyPrefixNone) {
					break;
				}

				Src++;
				if (*Src=='&') {
					/* We skipped the first '&', the break will 
					   make us drop to copying the second */
					break;
				}
				CurrentDetail->Flags |= STRING_DETAIL_HOTKEY;
				HaveHotkeys=TRUE;
				continue;
			}

			/* Boy, this must be slow, FIXME somehow */
			default: {
				if (fmt->trimming!=StringTrimmingCharacter) {
					break;
				}
				/* Fall through */
			}

			case ' ': {
				/* Mark where we can break for a new line */
				CurrentDetail->Flags |= STRING_DETAIL_BREAK;
				break;
			}

		}
		*Dest=*Src;
		Src++;
		Dest++;
		CurrentDetail++;
	}
	*Dest='\0';	
	
	/* Recalculate StringLen; we may have shortened String */
	Dest=CleanString;
	StringLen=0;
	while (*Dest!=0) {
		StringLen++;
		Dest++;
	}

	/* Don't bother doing anything else if the length is 0 */
	if (StringLen == 0) {
		GdipFree(CleanString);
		GdipFree(StringDetails);
		if (format != fmt) {
			GdipDeleteStringFormat(fmt);
		}

		return 0;
	}
	
	/* Convert string from Gdiplus format to UTF8, suitable for cairo */
	String = (unsigned char *) ucs2_to_utf8 ((const gunichar2 *)CleanString, -1);
	if (!String) {
		GdipFree (CleanString);
		GdipFree (StringDetails);
		if (format!=fmt) {
			GdipDeleteStringFormat (fmt);
		}

		return OutOfMemory;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Sanitized string: >%s<, length %d (utf8-length:%d)\n", String, StringLen, strlen(String));
#endif

	/* Generate size array */
	if (CalculateStringWidths (graphics->ct, font, CleanString, StringLen, StringDetails)==0) {
		/* FIXME; pick right return code */
		GdipFree(String);
		GdipFree(StringDetails);
		GdipFree(CleanString);
		if (format!=fmt) {
			GdipDeleteStringFormat (fmt);
		}

		return 0;
	}
	GdipFree (String);

	CursorX=0;
	CursorY=0;
	MaxX=0;
	MaxXatY=0;
	MaxY=0;
	CurrentLineStart=StringDetails;
	CurrentDetail=StringDetails;
	CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
	WrapPoint=-1;
	WrapX=0;

	if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
		FrameWidth=rc->Height;
		FrameHeight=rc->Width;
	} else {
		FrameWidth=rc->Width;
		FrameHeight=rc->Height;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Frame %d x %d\n", FrameWidth, FrameHeight);
#endif
	for (i=0; i<StringLen; i++) {
		/* Handle tabs and new lines */
		if (CurrentDetail->Flags & STRING_DETAIL_TAB) {
			float	tab_pos;
			int	tab_index;

			tab_pos = fmt->firstTabOffset;
			tab_index = 0;
			while (CursorX > tab_pos) {
				tab_pos += TabStops[tab_index % NumOfTabStops];
				tab_index++;
			}
			CursorX = tab_pos;
			CurrentLineStart = CurrentDetail;
			CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
		}
		if (CurrentDetail->Flags & STRING_DETAIL_LF) {
			CursorX = 0;
			CursorY += CurrentDetail->Linefeeds*LineHeight;
			CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
			CurrentLineStart = CurrentDetail;
#ifdef DRAWSTRING_DEBUG
			{
				int j;

				for (j=0; j<CurrentDetail->Linefeeds; j++) {
					printf("<LF>\n");
				}
			}
#endif
		}

#ifdef DRAWSTRING_DEBUG
		printf("[%3d] X: %3d, Y:%3d, '%c'  | ", i, (int)CursorX, (int)CursorY, CleanString[i]>=32 ? CleanString[i] : '?');
#endif
		/* Remember where to wrap next, but only if wrapping allowed */
		if (((fmt->formatFlags & StringFormatFlagsNoWrap)==0) && (CurrentDetail->Flags & STRING_DETAIL_BREAK)) {
			WrapPoint=i+1;	/* We skip the break char itself, keeping it at the end of the old line */

			if (CursorX>MaxX) {
				WrapX=CursorX;
			} else {
				WrapX=MaxX;
			}
#ifdef DRAWSTRING_DEBUG
			printf("<W>");
#endif
		}

		/* New line voids any previous wrap point */
		if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
			WrapPoint=-1;
		}

		CurrentDetail->PosX=CursorX;
		CurrentDetail->PosY=CursorY;

		/* Advance cursor */
		CursorX+=CurrentDetail->Width;
		if (MaxX<CursorX) {
			MaxX=CursorX;
			MaxXatY=CursorY;
		}

		/* Time for a new line? Go back to last WrapPoint and wrap */
		if (FrameWidth && CursorX>FrameWidth) {
			if (WrapPoint!=-1) {
				/** Re-Calculate line lengths **/
				/* Old line */
				CurrentLineStart->LineLen-=i-WrapPoint;
				if (MaxXatY==CursorY) {
					MaxX=WrapX;
				}

				/* Remove the trailing space from being counted if we're not supposed to */
				if (((fmt->formatFlags & StringFormatFlagsMeasureTrailingSpaces)==0) && (WrapPoint>0)) {
					if (CleanString[WrapPoint-1]==' ') {
						if (MaxXatY==CursorY) {
							MaxX-=StringDetails[WrapPoint-1].Width;
						}
						StringDetails[WrapPoint-1].Width=0;
						CurrentLineStart->LineLen--;
					}
				}

				/* New line */
				CurrentLineStart=&(StringDetails[WrapPoint]);
				CurrentLineStart->Flags|=STRING_DETAIL_LINESTART;
				CurrentLineStart->LineLen=0;

				/* Generate CursorX/Y for new line */
				CursorY+=LineHeight;
				CursorX=CurrentLineStart->Width;

				i=WrapPoint;
#ifdef DRAWSTRING_DEBUG
				printf("\n<Forcing break at index %d, CursorX:%f, LineLen:%d>\n", WrapPoint, CursorX, CurrentLineStart->LineLen);
#endif
				CurrentDetail=&(StringDetails[WrapPoint]);
				CurrentDetail->PosX=0;
				CurrentDetail->PosY=CursorY;
				WrapPoint=-1;
			} else {
				/*
				   This line is too long and has no wrap points, check if we need to insert ellipsis.
				   To keep at least a bit of performance, we cheat - we don't actually calculate the
				   size of the elipsis chars but assume that they're always smaller than any other
				   character. And we don't try to hard to fit as many chars as possible.
				*/

				int	EndOfLine;

#ifdef DRAWSTRING_DEBUG
				printf("No wrappoint (yet) set\n");
#endif
				/* Find end of line, index i is the first char no longer visible on the line */
				EndOfLine=i;
				if ((fmt->formatFlags & StringFormatFlagsNoWrap)==0) {
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & STRING_DETAIL_LF)==0)) {
						EndOfLine++;
					}
				} else {
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & (STRING_DETAIL_LF | STRING_DETAIL_BREAK))==0)) {
						EndOfLine++;
					}
					if (EndOfLine<StringLen) {
						if (StringDetails[EndOfLine].Flags & STRING_DETAIL_BREAK) {
							EndOfLine++;
						}
					}
				}

				if ((fmt->trimming==StringTrimmingEllipsisWord) || (fmt->trimming==StringTrimmingEllipsisCharacter)) {
					if (CurrentLineStart->LineLen>3) {
						if (fmt->trimming==StringTrimmingEllipsisCharacter) {
							CleanString[i-1]='.';
							CleanString[i-2]='.';
							CleanString[i-3]='.';
						} else {
							int	found=0;

							j=i;
							while(j>(i-CurrentLineStart->LineLen)) {
								if (CleanString[j]==' ') {
									CleanString[j]='.';
									CurrentLineStart->LineLen-=i-j-1;

									if ((j+1)<EndOfLine) {
										CleanString[j+1]='.';
										CurrentLineStart->LineLen++;
									}

									if ((j+2)<EndOfLine) {
										CleanString[j+2]='.';
										CurrentLineStart->LineLen++;
									}
									found=1;
									break;
								}
								j--;
							}

							if (!found) {
								CleanString[i-1]='.';
								CleanString[i-2]='.';
								CleanString[i-3]='.';
							}
						}
					}
				} else if (fmt->trimming==StringTrimmingEllipsisPath) {
					int	k;
					float	LineWidth;

					/* Find end of line, index i is the first char no longer visible on the line */
					EndOfLine=i;
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & (STRING_DETAIL_LF | STRING_DETAIL_BREAK))==0)) {
						EndOfLine++;
					}

					/* Whack the center, make sure we've got space in the string */
					if (CurrentLineStart->LineLen>3) {
						j=i-(CurrentLineStart->LineLen/2);
						CleanString[j-1]='.';
						CleanString[j]='.';
						CleanString[j+1]='.';

						/* Have just enough to include our ellipsis */
						LineWidth=0;
						for (k=i-CurrentLineStart->LineLen; k<(j+1); k++) {
							LineWidth+=StringDetails[k].Width;
						}
						CurrentLineStart->LineLen=i-j+3;	/* 3=ellipsis */

						/* Now figure out how many chars from the end of the string we have to copy */
						j+=2;	/* Points to the char behind the last ellipsis */
						k=EndOfLine-1;
						while ((LineWidth+StringDetails[k].Width)<FrameWidth) {
							LineWidth+=StringDetails[k].Width;
							k--;
						}
						memcpy (&CleanString[j], &CleanString[k+1], sizeof(WCHAR)*(EndOfLine-k-1));

						CurrentLineStart->LineLen+=EndOfLine-k-1;
					} 
					
				} else {
#ifdef DRAWSTRING_DEBUG
					/* Just cut off the text */
					printf("End of line at index:%d\n", EndOfLine);
#endif
					CurrentLineStart->LineLen=EndOfLine;
				}

				if ((fmt->formatFlags & StringFormatFlagsNoWrap)!=0)
					break;

				/* New line */
				CurrentLineStart=&(StringDetails[EndOfLine]);
				CurrentLineStart->Flags|=STRING_DETAIL_LINESTART;
				CurrentLineStart->LineLen=0;

				/* Generate CursorX/Y for new line */
				CursorY+=LineHeight;
				CursorX=CurrentLineStart->Width;

				i=EndOfLine;

				CurrentDetail=&(StringDetails[EndOfLine]);
				CurrentDetail->PosX=0;
				CurrentDetail->PosY=CursorY;
			}
		}


		/* Still visible? */
		if ((FrameWidth && CursorX>FrameWidth) || (FrameHeight && ((CursorY>FrameHeight) || ((fmt->formatFlags & StringFormatFlagsLineLimit) && (CursorY+LineHeight)>FrameHeight)))) {
			CurrentDetail->Flags|=STRING_DETAIL_HIDDEN;
#ifdef DRAWSTRING_DEBUG
			if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
				printf("<LSTART-H>");
			} else {
				printf("<H>");
			}
#endif
		} else {
			if (MaxY<CursorY) {
				MaxY=CursorY;
			}
		}

#ifdef DRAWSTRING_DEBUG
		if (i % 3 == 2) {
			printf("\n");
		}
#endif

		CurrentDetail++;

		CurrentLineStart->LineLen++;
	}

	/* We ignored it above, for shorter of calculations, also, add a bit of padding */
#if 0
	if ((fmt->formatFlags & StringFormatFlagsNoFitBlackBox) == 0) {
		MaxX+=2;
	}
#endif
	MaxY+=LineHeight+FontExtent.descent+2;

#ifdef DRAWSTRING_DEBUG
	printf("\n");

	printf("Bounding box: %d x %d\n", MaxX, MaxY);

	printf("Line layout [Total len %d]:\n", StringLen);
	for (i=0; i<StringLen; i++) {
		if (StringDetails[i].Flags & STRING_DETAIL_LF) {
			for (j=0; j<StringDetails[i].Linefeeds; j++) {
				printf("\n");
			}
		}
		if (StringDetails[i].Flags & STRING_DETAIL_LINESTART) {
			printf("[Len %2d %dx%d] ", StringDetails[i].LineLen, (int)StringDetails[i].PosX, (int)StringDetails[i].PosY);
			for (j=0; j<StringDetails[i].LineLen; j++) {
				printf("%c", CleanString[i+j]);
			}
			i+=j-1;
			printf("\n");
		}
	}
#endif

	/* Prepare alignment handling */
	AlignHorz=fmt->alignment;
	if (fmt->formatFlags & StringFormatFlagsDirectionRightToLeft) {
		if (fmt->alignment==StringAlignmentNear) {
			AlignHorz=StringAlignmentFar;
		} else if (fmt->alignment==StringAlignmentFar) {
			AlignHorz=StringAlignmentNear;
		}
	}
	AlignVert=fmt->lineAlignment;

#if 0
	/* Alignment sanity checks, not sure about these, might not match MS */
	if (MaxX>rc->Width) {
		AlignHorz=StringAlignmentNear;
	}

	if (MaxY>rc->Height) {
		AlignVert=StringAlignmentNear;
	}
#endif

	/*
	   At this point we know our bounding box, what characters
	   are to be displayed and where every character goes
	*/
	if (boundingBox) {
		boundingBox->X=rc->X;
		boundingBox->Y=rc->Y;
		if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
			boundingBox->Width=MaxY;
			boundingBox->Height=MaxX;
		} else {
			boundingBox->Width=MaxX;
			boundingBox->Height=MaxY;
		}
		if (rc->Width>0 && boundingBox->Width>rc->Width) {
			boundingBox->Width=rc->Width;
		}
		if (rc->Height>0 && boundingBox->Height>rc->Height) {
			boundingBox->Height=rc->Height;
		}
	}

	if (codepointsFitted) {
		*codepointsFitted=StringLen;
	}

	if (linesFilled) {
		*linesFilled=MaxY / LineHeight;
	}

	if (draw) {
		/* Set our clipping rectangle */
		if ((rc->Width!=0) && (rc->Height!=0) && ((fmt->formatFlags & StringFormatFlagsNoClip)==0)) {
#ifdef DRAWSTRING_DEBUG
			printf("Setting clipping rectangle (%f, %f %fx%f)\n", rc->X, rc->Y, rc->Width, rc->Height);
#endif		
			/* We do not call cairo_reset_clip because we want to take previous clipping into account */
			cairo_rectangle (graphics->ct, rc->X + graphics->aa_offset_x, rc->Y + graphics->aa_offset_y, rc->Width, rc->Height);
			cairo_clip (graphics->ct);
			cairo_new_path (graphics->ct);
			SetClipping = TRUE;
		}

		/* Setup cairo */
		/* Save the font matrix */

		if (brush) {
			gdip_brush_setup (graphics, (GpBrush *)brush);
		} else {
			cairo_set_source_rgb (graphics->ct, 0., 0., 0.);
		}

		for (i=0; i<StringLen; i++) {
			if (StringDetails[i].Flags & STRING_DETAIL_LINESTART) {
				/* To support the LineLimit flag */
				if ((StringDetails[i].Flags & STRING_DETAIL_HIDDEN)!=0){
#ifdef DRAWSTRING_DEBUG
					printf("Hidding partially visible line\n");
#endif
					i=StringLen;
					continue;
				}

				String= (unsigned char *) ucs2_to_utf8 ((const gunichar2 *)(CleanString+i), StringDetails[i].LineLen);
#ifdef DRAWSTRING_DEBUG
				printf("Displaying line >%s< (%d chars)\n", String, StringDetails[i].LineLen);
#endif

				if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
					switch (AlignHorz) {
						case StringAlignmentNear: CursorX=rc->X + StringDetails[i].PosX; break;
						case StringAlignmentCenter: CursorX=rc->X + StringDetails[i].PosX+(rc->Width-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width)/2; break;
						case StringAlignmentFar: CursorX=rc->X + StringDetails[i].PosX+rc->Width-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width; break;
					}

					switch (AlignVert) {
						case StringAlignmentNear: CursorY=rc->Y+StringDetails[i].PosY+LineHeight; break;
						case StringAlignmentCenter: CursorY=rc->Y+(rc->Height-MaxY)/2+StringDetails[i].PosY+LineHeight; break;
						case StringAlignmentFar: CursorY=rc->Y+rc->Height-MaxY+StringDetails[i].PosY+LineHeight; break;
					}
					cairo_move_to (graphics->ct, CursorX + graphics->aa_offset_x, CursorY + graphics->aa_offset_y);
					cairo_show_text (graphics->ct, (const char *) String);
				} else {
					switch (AlignHorz) {
						case StringAlignmentNear: CursorY=rc->Y; break;
						case StringAlignmentCenter: CursorY=rc->Y+(rc->Height-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width)/2; break;
						case StringAlignmentFar: CursorY=rc->Y+rc->Height-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width; break;
					}

					switch (AlignVert) {
						case StringAlignmentNear: CursorX=rc->X + StringDetails[i].PosX+StringDetails[i].PosY; break;
						case StringAlignmentCenter: CursorX=rc->X + StringDetails[i].PosX+(rc->Width-MaxY)/2+StringDetails[i].PosY; break;
						case StringAlignmentFar: CursorX=rc->X + StringDetails[i].PosX+rc->Width-MaxY+StringDetails[i].PosY; break;
					}

					/* Rotate text for vertical drawing */
					cairo_save (graphics->ct);
					cairo_move_to (graphics->ct, CursorX + graphics->aa_offset_x, CursorY + graphics->aa_offset_y);
					cairo_rotate (graphics->ct, PI/2);
					cairo_show_text (graphics->ct, (const char *) String);
					cairo_restore (graphics->ct);
				}

#ifdef DRAWSTRING_DEBUG
				printf("Drawing %d chars at %d x %d (width=%f pixels)\n", StringDetails[i].LineLen, (int)CursorX, (int)CursorY, StringDetails[i+StringDetails[i].LineLen-1].PosX);
#endif
				GdipFree (String);

				if (font->style & (FontStyleUnderline | FontStyleStrikeout)) {
					double line_width = cairo_get_line_width (graphics->ct);

					/* Calculate the width of the line */
					cairo_set_line_width (graphics->ct, 1.0);
					j=StringDetails[i+StringDetails[i].LineLen-1].PosX+StringDetails[i+StringDetails[i].LineLen-1].Width;

					if (font->style & FontStyleStrikeout) {
						if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
							cairo_move_to (graphics->ct, (int)(CursorX) + graphics->aa_offset_x, (int)(CursorY-FontExtent.descent) + graphics->aa_offset_y);
							cairo_line_to (graphics->ct, (int)(CursorX+j) + graphics->aa_offset_x, (int)(CursorY-FontExtent.descent) + graphics->aa_offset_y);
						} else {
							cairo_move_to (graphics->ct, (int)(CursorX+FontExtent.descent) + graphics->aa_offset_x, (int)(CursorY) + graphics->aa_offset_y);
							cairo_line_to (graphics->ct, (int)(CursorX+FontExtent.descent) + graphics->aa_offset_x, (int)(CursorY+j) + graphics->aa_offset_y);
						}
					}

					if (font->style & FontStyleUnderline) {
						if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
							cairo_move_to (graphics->ct, (int)(CursorX) + graphics->aa_offset_x, (int)(CursorY+FontExtent.descent-2) + graphics->aa_offset_y);
							cairo_line_to (graphics->ct, (int)(CursorX+j) + graphics->aa_offset_x, (int)(CursorY+FontExtent.descent-2) + graphics->aa_offset_y);
						} else {
							cairo_move_to (graphics->ct, (int)(CursorX+FontExtent.descent-2) + graphics->aa_offset_x, (int)(CursorY) + graphics->aa_offset_y);
							cairo_line_to (graphics->ct, (int)(CursorX+FontExtent.descent-2) + graphics->aa_offset_x, (int)(CursorY+j) + graphics->aa_offset_y);
						}
					}

					cairo_stroke (graphics->ct);
					cairo_set_line_width (graphics->ct, line_width);
				}

				i+=StringDetails[i].LineLen-1;
			}
			
		}

		/* Handle Hotkey prefix */
		if (fmt->hotkeyPrefix==HotkeyPrefixShow && HaveHotkeys) {
			CurrentDetail=StringDetails;
			for (i=0; i<StringLen; i++) {
				if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
					if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
						switch (AlignHorz) {
							case StringAlignmentNear: CursorX=rc->X; break;
							case StringAlignmentCenter: CursorX=rc->X+(rc->Width-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width)/2; break;
							case StringAlignmentFar: CursorX=rc->X+rc->Width-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width; break;
						}

						switch (AlignVert) {
							case StringAlignmentNear: CursorY=rc->Y+StringDetails[i].PosY+LineHeight; break;
							case StringAlignmentCenter: CursorY=rc->Y+(rc->Height-MaxY)/2+StringDetails[i].PosY+LineHeight; break;
							case StringAlignmentFar: CursorY=rc->Y+rc->Height-MaxY+StringDetails[i].PosY+LineHeight; break;
						}
					} else {
						switch (AlignHorz) {
							case StringAlignmentNear: CursorY=rc->Y; break;
							case StringAlignmentCenter: CursorY=rc->Y+(rc->Height-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width)/2; break;
							case StringAlignmentFar: CursorY=rc->Y+rc->Height-StringDetails[i+StringDetails[i].LineLen-1].PosX-StringDetails[i+StringDetails[i].LineLen-1].Width; break;
						}

						switch (AlignVert) {
							case StringAlignmentNear: CursorX=rc->X+StringDetails[i].PosY; break;
							case StringAlignmentCenter: CursorX=rc->X+(rc->Width-MaxY)/2+StringDetails[i].PosY; break;
							case StringAlignmentFar: CursorX=rc->X+rc->Width-MaxY+StringDetails[i].PosY; break;
						}
					}
				}
				if (CurrentDetail->Flags & STRING_DETAIL_HOTKEY) {
					if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
						CursorX+=CurrentDetail->PosX;
						cairo_set_line_width(graphics->ct, 1);

						cairo_move_to (graphics->ct, (int)(CursorX) + graphics->aa_offset_x, (int)(CursorY+FontExtent.descent/2) + graphics->aa_offset_y);
						cairo_line_to (graphics->ct, (int)(CursorX+CurrentDetail->Width) + graphics->aa_offset_x, (int)(CursorY+FontExtent.descent/2) + graphics->aa_offset_y);
						cairo_stroke (graphics->ct);
						CursorX-=CurrentDetail->PosX;

					} else {
						CursorY+=CurrentDetail->PosX;
						cairo_move_to (graphics->ct, (int)(CursorX-FontExtent.descent/2) - graphics->aa_offset_x, (int)(CursorY) + graphics->aa_offset_y);
						cairo_line_to (graphics->ct, (int)(CursorX-FontExtent.descent/2) - graphics->aa_offset_x, (int)(CursorY+CurrentDetail->Width) + graphics->aa_offset_y);
						CursorY-=CurrentDetail->PosX;
					}
				}
				CurrentDetail++;
			}
		}

	}

	cairo_set_font_matrix(graphics->ct, &SavedMatrix);		/* Restore matrix to original values */

	/* Restore the graphics clipping region */
	if (SetClipping) {
		gdip_set_cairo_clipping (graphics);
	}

	/* Cleanup */
	GdipFree (CleanString);
	GdipFree (StringDetails);

	if (format != fmt) {
		GdipDeleteStringFormat (fmt);
	}
	return Ok;
}

GpStatus
GdipDrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, GDIPCONST RectF *rc, GDIPCONST GpStringFormat *fmt, GpBrush *brush)
{
	if (length == 0) {
		return Ok;
	}
	return (MeasureOrDrawString (graphics, stringUnicode, length, font, rc, fmt, brush, NULL, NULL, NULL, 1));
}

GpStatus
GdipMeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, GDIPCONST RectF *rc, GDIPCONST GpStringFormat *fmt,  RectF *boundingBox, int *codepointsFitted, int *linesFilled)
{
	if (length == 0) {
		if (boundingBox) {
			if (rc) {
				boundingBox->X = rc->X;
				boundingBox->Y = rc->Y;
			} else {
				boundingBox->X = 0;
				boundingBox->Y = 0;
			}
			boundingBox->Width = 0;
			boundingBox->Height = 0;
		}
		if (linesFilled) {
			*linesFilled = 0;
		}
		if (codepointsFitted) {
			*codepointsFitted = 0;
		}
		return Ok;
	}
	return (MeasureOrDrawString (graphics, stringUnicode, length, font, rc, fmt, NULL, boundingBox, codepointsFitted, linesFilled, 0));
}

static
GpStatus
MeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, GDIPCONST RectF *rc, GDIPCONST GpStringFormat *format, RectF *boundingBox, int *lineHeight, GpStringDetailStruct** strDetails)
{
	unsigned char		*String;		/* Holds the UTF8 version of our sanitized string */
	WCHAR			*CleanString;		/* Holds the unicode version of our sanitized string */
	unsigned long		StringLen;		/* Length of CleanString */
	cairo_matrix_t		SavedMatrix;		
	GDIPCONST WCHAR		*Src;
	WCHAR	 		*Dest;
	GpStringFormat		*fmt;
	unsigned long		i;
	unsigned long		j;
	GpStringDetailStruct	*StringDetails;		/* Contains rendering details */
	GpStringDetailStruct	*CurrentDetail;
	GpStringDetailStruct	*CurrentLineStart;	/* For rendering engine, to bump LineLen */
	float			*TabStops;
	int			NumOfTabStops;
	int			WrapPoint;		/* Array index of wrap character */
	int			WrapX;			/* Width of text at wrap character */
	float			CursorX;		/* Current X position of drawing cursor */
	float			CursorY;		/* Current Y position of drawing cursor */
	int			MaxX;			/* Largest X of cursor */
	int			MaxXatY;		/* Y coordinate of line with largest X, needed for MaxX resetting on wrap */
	int			MaxY;			/* Largest Y of cursor */
	int			FrameWidth;		/* rc->Width (or rc->Height if vertical) */
	int			FrameHeight;		/* rc->Height (or rc->Width if vertical) */
	int			AlignHorz;		/* Horizontal Alignment mode */
	int			AlignVert;		/* Vertical Alignment mode */
	int			LineHeight;		/* Height of a line with given font */
	bool			HaveHotkeys=FALSE;	/* True if we find hotkey */
	cairo_font_extents_t	FontExtent;		/* Info about our font */

	/* Sanity; should we check for length==0? */
	if (!graphics || !stringUnicode || !font) {
		return(InvalidParameter);
	}

#ifdef DRAWSTRING_DEBUG
	printf("GdipDrawString(...) called (length=%d, fontsize=%d)\n", length, (int)font->sizeInPixels);
#endif

	/* Check and set any defaults */
	if (!format) {
		GdipStringFormatGetGenericDefault ((GpStringFormat **)&fmt);
	} else {
		fmt=(GpStringFormat *)format;
	}
	TabStops=fmt->tabStops;
	NumOfTabStops=fmt->numtabStops;

	/* Prepare our various buffers and variables */
	StringLen=length;
	StringDetails=GdipCalloc (StringLen+1, sizeof(GpStringDetailStruct));
	CleanString=GdipAlloc (sizeof(WCHAR)*(StringLen+1));

	if (!CleanString || !StringDetails) {
		if (CleanString) {
			GdipFree (CleanString);
		}
		if (StringDetails) {
			GdipFree (StringDetails);
		}
		if (format!=fmt) {
			GdipDeleteStringFormat (fmt);
		}
		return OutOfMemory;
	}

	/*
	   Get font size information; how expensive is the cairo stuff here? 
	*/

	cairo_set_font_face (graphics->ct, (cairo_font_face_t*) font->cairofnt);	/* Set our font; this will also be used for later drawing */
	cairo_get_font_matrix (graphics->ct, &SavedMatrix);	/* Save the matrix */
	cairo_set_font_size (graphics->ct, font->sizeInPixels);
	cairo_font_extents (graphics->ct, &FontExtent);		/* Get the size we're looking for */


	if ((LineHeight=FontExtent.ascent)<1) {
		LineHeight=1;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Font extents: ascent:%d, descent: %d, height:%d, maxXadvance:%d, maxYadvance:%d\n", (int)FontExtent.ascent, (int)FontExtent.descent, (int)FontExtent.height, (int)FontExtent.max_x_advance, (int)FontExtent.max_y_advance);
#endif

	/* Sanitize string, remove formatting chars and build description array */
#ifdef DRAWSTRING_DEBUG
	printf("GdipDrawString(...) Sanitizing string, StringLen=%d\n", StringLen);
#endif
	Src=stringUnicode;
	Dest=CleanString;
	CurrentDetail=StringDetails;
	for (i=0; i<StringLen; i++) {
		switch(*Src) {
			case '\r': { /* CR */
				Src++;
				continue;
			}

			case '\t': { /* Tab */
				if (NumOfTabStops > 0) {
					CurrentDetail->Flags |= STRING_DETAIL_TAB;
				}
				Src++;
				continue;
			}

			case '\n': { /* LF */
				CurrentDetail->Flags |= STRING_DETAIL_LF;
				CurrentDetail->Linefeeds++;
				Src++;
				continue;
			}

			case '&': {
				/* We print *all* chars if no hotkeys */
				if (fmt->hotkeyPrefix==HotkeyPrefixNone) {
					break;
				}

				Src++;
				if (*Src=='&') {
					/* We skipped the first '&', the break will 
					   make us drop to copying the second */
					break;
				}
				CurrentDetail->Flags |= STRING_DETAIL_HOTKEY;
				HaveHotkeys=TRUE;
				continue;
			}

			/* Boy, this must be slow, FIXME somehow */
			default: {
				if (fmt->trimming!=StringTrimmingCharacter) {
					break;
				}
				/* Fall through */
			}

			case ' ': {
				/* Mark where we can break for a new line */
				CurrentDetail->Flags |= STRING_DETAIL_BREAK;
				break;
			}

		}
		*Dest=*Src;
		Src++;
		Dest++;
		CurrentDetail++;
	}
	*Dest='\0';
	
	/* Recalculate StringLen; we may have shortened String */
	Dest=CleanString;
	StringLen=0;
	while (*Dest!=0) {
		StringLen++;
		Dest++;
	}

	/* Convert string from Gdiplus format to UTF8, suitable for cairo */
	String=(unsigned char *) ucs2_to_utf8 ((const gunichar2 *)CleanString, -1);
	if (!String) {
		GdipFree (CleanString);
		GdipFree (StringDetails);
		if (format!=fmt) {
			GdipDeleteStringFormat (fmt);
		}
		return OutOfMemory;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Sanitized string: >%s<, length %d (utf8-length:%d)\n", String, StringLen, strlen(String));
#endif

	/* Generate size array */
	if (CalculateStringWidths (graphics->ct, font, CleanString, StringLen, StringDetails)==0) {
		/* FIXME; pick right return code */
		GdipFree(String);
		GdipFree(StringDetails);
		GdipFree(CleanString);
		if (format!=fmt) {
			GdipDeleteStringFormat (fmt);
		}
		return InvalidParameter;
	}
	GdipFree (String);

	CursorX=rc->X;
	CursorY=rc->Y;
	MaxX=0;
	MaxXatY=0;
	MaxY=0;
	CurrentLineStart=StringDetails;
	CurrentDetail=StringDetails;
	CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
	WrapPoint=-1;
	WrapX=0;

	if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
		FrameWidth=rc->Height;
		FrameHeight=rc->Width;
	} else {
		FrameWidth=rc->Width;
		FrameHeight=rc->Height;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Frame %d x %d\n", FrameWidth, FrameHeight);
#endif
	for (i=0; i<StringLen; i++) {
		/* Handle tabs and new lines */
		if (CurrentDetail->Flags & STRING_DETAIL_TAB) {
			float	tab_pos;
			int	tab_index;

			tab_pos = fmt->firstTabOffset;
			tab_index = 0;
			while (CursorX > tab_pos) {
				tab_pos += TabStops[tab_index % NumOfTabStops];
				tab_index++;
			}
			CursorX = tab_pos;
			CurrentLineStart = CurrentDetail;
			CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
		}
		if (CurrentDetail->Flags & STRING_DETAIL_LF) {
			CursorX=0;
			CursorY+=CurrentDetail->Linefeeds*LineHeight;
			CurrentDetail->Flags|=STRING_DETAIL_LINESTART;
			CurrentLineStart=CurrentDetail;
#ifdef DRAWSTRING_DEBUG
			{
				int j;

				for (j=0; j<CurrentDetail->Linefeeds; j++) {
					printf("<LF>\n");
				}
			}
#endif
		}

#ifdef DRAWSTRING_DEBUG
		printf("[%3d] X: %3d, Y:%3d, '%c'  | ", i, (int)CursorX, (int)CursorY, CleanString[i]>=32 ? CleanString[i] : '?');
#endif
		/* Remember where to wrap next, but only if wrapping allowed */
		if (((fmt->formatFlags & StringFormatFlagsNoWrap)==0) && (CurrentDetail->Flags & STRING_DETAIL_BREAK)) {
			WrapPoint=i+1;	/* We skip the break char itself, keeping it at the end of the old line */

			if (CursorX>MaxX) {
				WrapX=CursorX;
			} else {
				WrapX=MaxX;
			}
#ifdef DRAWSTRING_DEBUG
			printf("<W>");
#endif
		}

		/* New line voids any previous wrap point */
		if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
			WrapPoint=-1;
		}

		CurrentDetail->PosX=CursorX;
		CurrentDetail->PosY=CursorY;

		/* Advance cursor */
		CursorX+=CurrentDetail->Width;
		if (MaxX<CursorX) {
			MaxX=CursorX;
			MaxXatY=CursorY;
		}

		/* Time for a new line? Go back to last WrapPoint and wrap */
		if (FrameWidth && CursorX>FrameWidth) {
			if (WrapPoint!=-1) {
				/** Re-Calculate line lengths **/
				/* Old line */
				CurrentLineStart->LineLen-=i-WrapPoint;
				if (MaxXatY==CursorY) {
					MaxX=WrapX;
				}

				/* Remove the trailing space from being counted if we're not supposed to */
				if (((fmt->formatFlags & StringFormatFlagsMeasureTrailingSpaces)==0) && (WrapPoint>0)) {
					if (CleanString[WrapPoint-1]==' ') {
						if (MaxXatY==CursorY) {
							MaxX-=StringDetails[WrapPoint-1].Width;
						}
						StringDetails[WrapPoint-1].Width=0;
						CurrentLineStart->LineLen--;
					}
				}

				/* New line */
				CurrentLineStart=&(StringDetails[WrapPoint]);
				CurrentLineStart->Flags|=STRING_DETAIL_LINESTART;
				CurrentLineStart->LineLen=0;

				/* Generate CursorX/Y for new line */
				CursorY+=LineHeight;
				CursorX=CurrentLineStart->Width;

				i=WrapPoint;
#ifdef DRAWSTRING_DEBUG
				printf("\n<Forcing break at index %d, CursorX:%f, LineLen:%d>\n", WrapPoint, CursorX, CurrentLineStart->LineLen);
#endif
				CurrentDetail=&(StringDetails[WrapPoint]);
				CurrentDetail->PosX=0;
				CurrentDetail->PosY=CursorY;
				WrapPoint=-1;
			} else {
				/*
				   This line is too long and has no wrap points, check if we need to insert ellipsis.
				   To keep at least a bit of performance, we cheat - we don't actually calculate the
				   size of the elipsis chars but assume that they're always smaller than any other
				   character. And we don't try to hard to fit as many chars as possible.
				*/

				int	EndOfLine;

#ifdef DRAWSTRING_DEBUG
				printf("No wrappoint (yet) set\n");
#endif
				/* Find end of line, index i is the first char no longer visible on the line */
				EndOfLine=i;
				if ((fmt->formatFlags & StringFormatFlagsNoWrap)==0) {
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & STRING_DETAIL_LF)==0)) {
						EndOfLine++;
					}
				} else {
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & (STRING_DETAIL_LF | STRING_DETAIL_BREAK))==0)) {
						EndOfLine++;
					}
					if (EndOfLine<StringLen) {
						if (StringDetails[EndOfLine].Flags & STRING_DETAIL_BREAK) {
							EndOfLine++;
						}
					}
				}

				if ((fmt->trimming==StringTrimmingEllipsisWord) || (fmt->trimming==StringTrimmingEllipsisCharacter)) {
					if (CurrentLineStart->LineLen>3) {
						if (fmt->trimming==StringTrimmingEllipsisCharacter) {
							CleanString[i-1]='.';
							CleanString[i-2]='.';
							CleanString[i-3]='.';
						} else {
							int	found=0;

							j=i;
							while(j>(i-CurrentLineStart->LineLen)) {
								if (CleanString[j]==' ') {
									CleanString[j]='.';
									CurrentLineStart->LineLen-=i-j-1;

									if ((j+1)<EndOfLine) {
										CleanString[j+1]='.';
										CurrentLineStart->LineLen++;
									}

									if ((j+2)<EndOfLine) {
										CleanString[j+2]='.';
										CurrentLineStart->LineLen++;
									}
									found=1;
									break;
								}
								j--;
							}

							if (!found) {
								CleanString[i-1]='.';
								CleanString[i-2]='.';
								CleanString[i-3]='.';
							}
						}
					}
				} else if (fmt->trimming==StringTrimmingEllipsisPath) {
					int	k;
					float	LineWidth;

					/* Find end of line, index i is the first char no longer visible on the line */
					EndOfLine=i;
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & (STRING_DETAIL_LF | STRING_DETAIL_BREAK))==0)) {
						EndOfLine++;
					}

					/* Whack the center, make sure we've got space in the string */
					if (CurrentLineStart->LineLen>3) {
						j=i-(CurrentLineStart->LineLen/2);
						CleanString[j-1]='.';
						CleanString[j]='.';
						CleanString[j+1]='.';

						/* Have just enough to include our ellipsis */
						LineWidth=0;
						for (k=i-CurrentLineStart->LineLen; k<(j+1); k++) {
							LineWidth+=StringDetails[k].Width;
						}
						CurrentLineStart->LineLen=i-j+3;	/* 3=ellipsis */

						/* Now figure out how many chars from the end of the string we have to copy */
						j+=2;	/* Points to the char behind the last ellipsis */
						k=EndOfLine-1;
						while ((LineWidth+StringDetails[k].Width)<FrameWidth) {
							LineWidth+=StringDetails[k].Width;
							k--;
						}
						memcpy (&CleanString[j], &CleanString[k+1], sizeof(WCHAR)*(EndOfLine-k-1));

						CurrentLineStart->LineLen+=EndOfLine-k-1;
					} 
					
#ifdef DRAWSTRING_DEBUG
				} else {
					/* Just cut off the text */
					printf("End of line at index:%d\n", EndOfLine);
#endif
				}

				/* New line */
				CurrentLineStart=&(StringDetails[EndOfLine]);
				CurrentLineStart->Flags|=STRING_DETAIL_LINESTART;
				CurrentLineStart->LineLen=0;

				/* Generate CursorX/Y for new line */
				CursorY+=LineHeight;
				CursorX=CurrentLineStart->Width;

				i=EndOfLine;

				CurrentDetail=&(StringDetails[EndOfLine]);
				CurrentDetail->PosX=0;
				CurrentDetail->PosY=CursorY;
			}
		}


		/* Still visible? */
		if ((FrameWidth && CursorX>FrameWidth) || (FrameHeight && ((CursorY>FrameHeight) || ((fmt->formatFlags & StringFormatFlagsLineLimit) && (CursorY+LineHeight)>FrameHeight)))) {
			CurrentDetail->Flags|=STRING_DETAIL_HIDDEN;
#ifdef DRAWSTRING_DEBUG
			if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
				printf("<LSTART-H>");
			} else {
				printf("<H>");
			}
#endif
		} else {
			if (MaxY<CursorY) {
				MaxY=CursorY;
			}
		}

#ifdef DRAWSTRING_DEBUG
		if (i % 3 == 2) {
			printf("\n");
		}
		printf ("curr details: x: %f y: %f wd: %f\n", CurrentDetail->PosX, CurrentDetail->PosY,CurrentDetail->Width);
#endif
		CurrentDetail++;
		CurrentLineStart->LineLen++;
	}

	/* We ignored it above, for shorter of calculations, also, add a bit of padding */
	MaxX+=2;
	MaxY+=LineHeight+FontExtent.descent+2;

#ifdef DRAWSTRING_DEBUG
	printf("\n");

	printf("Bounding box: %d x %d\n", MaxX, MaxY);

	printf("Line layout [Total len %d]:\n", StringLen);
	for (i=0; i<StringLen; i++) {
		if (StringDetails[i].Flags & STRING_DETAIL_LF) {
			for (j=0; j<StringDetails[i].Linefeeds; j++) {
				printf("\n");
			}
		}
		if (StringDetails[i].Flags & STRING_DETAIL_LINESTART) {
			printf("[Len %2d %dx%d] ", StringDetails[i].LineLen, (int)StringDetails[i].PosX, (int)StringDetails[i].PosY);
			for (j=0; j<StringDetails[i].LineLen; j++) {
				printf("%c", CleanString[i+j]);
			}
			i+=j-1;
			printf("\n");
		}
	}
#endif

	/* Prepare alignment handling */
	AlignHorz=fmt->alignment;
	if (fmt->formatFlags & StringFormatFlagsDirectionRightToLeft) {
		if (fmt->alignment==StringAlignmentNear) {
			AlignHorz=StringAlignmentFar;
		} else if (fmt->alignment==StringAlignmentFar) {
			AlignHorz=StringAlignmentNear;
		}
	}
	AlignVert=fmt->lineAlignment;

#if 0
	/* Alignment sanity checks, not sure about these, might not match MS */
	if (MaxX>rc->Width) {
		AlignHorz=StringAlignmentNear;
	}

	if (MaxY>rc->Height) {
		AlignVert=StringAlignmentNear;
	}
#endif

	/*
	   At this point we know our bounding box, what characters
	   are to be displayed and where every character goes
	*/
	if (boundingBox) {
		boundingBox->X=rc->X;
		boundingBox->Y=rc->Y;
		if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
			boundingBox->Width=MaxY;
			boundingBox->Height=MaxX;
		} else {
			boundingBox->Width=MaxX;
			boundingBox->Height=MaxY;
		}
		if (rc->Width>0 && boundingBox->Width>rc->Width) {
			boundingBox->Width=rc->Width;
		}
		if (rc->Height>0 && boundingBox->Height>rc->Height) {
			boundingBox->Height=rc->Height;
		}
	}

	cairo_set_font_matrix(graphics->ct, &SavedMatrix); /* Restore matrix to original values */


	/* Cleanup */
	GdipFree (CleanString);

	if (format != fmt) {
		GdipDeleteStringFormat (fmt);
	}

	*lineHeight = LineHeight;
	*strDetails = StringDetails;

	return Ok;
}

GpStatus
GdipMeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length,
			    GDIPCONST GpFont *font, GDIPCONST GpRectF *layoutRect,
			    GDIPCONST GpStringFormat *format, int regionCount, GpRegion **regions)
{
	int			i, j, start, end;
	int			lineHeight;
	CharacterRange		range;
	GpStatus		status;
	GpStringDetailStruct*	strDetails;
	RectF			boundingBox;
	RectF			charRect;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (stringUnicode != NULL, InvalidParameter);
	g_return_val_if_fail (length != 0, InvalidParameter);
	g_return_val_if_fail (font != NULL, InvalidParameter);
	g_return_val_if_fail (layoutRect != NULL, InvalidParameter);
	g_return_val_if_fail (format != NULL, InvalidParameter);
	g_return_val_if_fail (regions != NULL, InvalidParameter);
	g_return_val_if_fail (regionCount == format->charRangeCount, InvalidParameter);

	/* No char range or bounding rect is set for measurements */
	if (format->charRangeCount == 0 || layoutRect->Width == 0 || layoutRect->Height == 0)
		return Ok;

	/* string measurements */
	status = MeasureString (graphics, stringUnicode, length, font, layoutRect,
				format, &boundingBox, &lineHeight, &strDetails);

	if (status != Ok)
		return status;

	/* Create a region for every char range */
	for (i = 0; i < format->charRangeCount; i++) {
		range = format->charRanges [i];
		GdipSetEmpty (regions [i]); 

		if (range.length > 0)
			start = range.first;
		else
			start = range.first + range.length;

		end = start + range.length;

		/* sanity check on charRange. negative lengths are allowed */
		if (range.first < 0)
			return InvalidParameter;

		if (start < 0 || end > length)
			return InvalidParameter;

		/* calculate the regions */
		for (j = start; j < end; j++) {
			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				charRect.X = strDetails [j].PosY;
				charRect.Y = strDetails [j].PosX;
				charRect.Width = lineHeight;
				charRect.Height = strDetails [j].Width;
			}
			else {
				charRect.X = strDetails [j].PosX;
				charRect.Y = strDetails [j].PosY;
				charRect.Width = strDetails [j].Width;
				charRect.Height = lineHeight;
			}
			status = GdipCombineRegionRect (regions [i], &charRect, CombineModeUnion);
			if (status == Ok)
				continue;
			else
				break;
		}
		if (status == Ok)
			continue;
		else
			break;
	}

	GdipFree (strDetails);

	return Ok;
}

GpStatus 
GdipSetRenderingOrigin (GpGraphics *graphics, int x, int y)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	graphics->render_origin_x = x;
	graphics->render_origin_y = y;

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipGetRenderingOrigin (GpGraphics *graphics, int *x, int *y)
{
        double cx, cy;
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (x != NULL, InvalidParameter);
	g_return_val_if_fail (y != NULL, InvalidParameter);

        *x = graphics->render_origin_x;
        *y = graphics->render_origin_y;

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipGetDpiX (GpGraphics *graphics, float *dpi)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (dpi != NULL, InvalidParameter);

	*dpi = gdip_get_display_dpi ();
        return Ok;
}

GpStatus 
GdipGetDpiY (GpGraphics *graphics, float *dpi)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (dpi != NULL, InvalidParameter);

	*dpi = gdip_get_display_dpi ();
        return Ok;
}

GpStatus
GdipGraphicsClear (GpGraphics *graphics, ARGB color)
{
	double red, green, blue, alpha;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	blue = color & 0xff;
	green = (color >> 8) & 0xff;
	red = (color >> 16) & 0xff;
	alpha = (color >> 24) & 0xff;

	/* Save the existing color/alpha/pattern settings */
	cairo_save (graphics->ct);

	cairo_set_source_rgba (graphics->ct, red / 255, green / 255, blue / 255, alpha / 255);
	cairo_rectangle (graphics->ct, 0, 0, graphics->bounds.Width, graphics->bounds.Height);
	cairo_fill (graphics->ct);

	/* Restore the color/alpha/pattern settings */
	cairo_restore (graphics->ct);

	return Ok;
}

GpStatus
GdipSetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	graphics->interpolation = interpolationMode;
	return Ok;
}

GpStatus
GdipGetInterpolationMode (GpGraphics *graphics, InterpolationMode *imode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (imode != NULL, InvalidParameter);

	*imode = graphics->interpolation;
	return Ok;
}

GpStatus
GdipSetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);

	graphics->text_mode = mode;

	return Ok;
}

GpStatus
GdipGetTextRenderingHint(GpGraphics *graphics, TextRenderingHint *mode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (mode != NULL, InvalidParameter);

	*mode = graphics->text_mode;

	return Ok;
}

GpStatus
GdipSetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pixelOffsetMode != PixelOffsetModeInvalid, InvalidParameter);
	graphics->pixel_mode = pixelOffsetMode;
	
	return Ok;
}

GpStatus
GdipGetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pixelOffsetMode != NULL, InvalidParameter);
	
	*pixelOffsetMode = graphics->pixel_mode;
	return Ok;
}

GpStatus
GdipSetTextContrast (GpGraphics *graphics, UINT contrast)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (contrast >= 0 && contrast <= 14, InvalidParameter);

	return Ok;
}

GpStatus
GdipGetTextContrast (GpGraphics *graphics, UINT *contrast)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (contrast != NULL, InvalidParameter);

	*contrast = 4;              /* default */
	return Ok;
}

GpStatus
GdipSetSmoothingMode (GpGraphics *graphics, SmoothingMode mode)
{
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (mode != SmoothingModeInvalid, InvalidParameter);

	graphics->draw_mode = mode;

	switch (mode) {
		case SmoothingModeAntiAlias:
		case SmoothingModeHighQuality: {
			cairo_set_antialias(graphics->ct, CAIRO_ANTIALIAS_DEFAULT);
			graphics->aa_offset_x = 0;
			graphics->aa_offset_y = 0;
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (mode != NULL, InvalidParameter);

	*mode = graphics->draw_mode;
	return Ok;
}

GpStatus
GdipBeginContainer (GpGraphics *graphics, GDIPCONST GpRectF* dstrect, GDIPCONST GpRectF *srcrect, GpUnit unit, GpGraphicsContainer *state)
{
	return NotImplemented;
}

GpStatus
GdipBeginContainer2 (GpGraphics *graphics, GpGraphicsContainer* state)
{
	return NotImplemented;
}

GpStatus
GdipBeginContainerI (GpGraphics *graphics, GDIPCONST GpRect* dstrect, GDIPCONST GpRect *srcrect, GpUnit unit, GpGraphicsContainer *state)
{
	return NotImplemented;
}

GpStatus
GdipEndContainer (GpGraphics *graphics, GpGraphicsContainer state)
{
	return NotImplemented;
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

extern bool gdip_is_InfiniteRegion (GpRegion *region);

void
gdip_set_cairo_clipping (GpGraphics *graphics)
{
        GpRectF* rect;
        int i;

	cairo_reset_clip (graphics->ct);

	if (gdip_is_InfiniteRegion (graphics->clip)) {
		return;
	}
	
	/* Set Cairo rectangles that Cairo will use for clipping */ 
        for (i = 0, rect = graphics->clip->rects; i < graphics->clip->cnt; i++, rect++) {
		cairo_rectangle (graphics->ct, rect->X, rect->Y, rect->Width, rect->Height);
	}
	
	cairo_clip (graphics->ct);
	cairo_new_path (graphics->ct);
}

GpStatus
GdipSetClipGraphics (GpGraphics *graphics, GpGraphics *srcgraphics, CombineMode combineMode)
{
	if (!graphics || !srcgraphics)
		return InvalidParameter;
		
	GdipDeleteRegion (graphics->clip);
	GdipCloneRegion (srcgraphics->clip, &graphics->clip);
	gdip_set_cairo_clipping (graphics);
	return Ok;
}

GpStatus
GdipSetClipRect (GpGraphics *graphics, float x, float y, float width, float height, CombineMode combineMode)
{
	GpRectF rect;
	
	if (!graphics)
		return InvalidParameter;

	rect.X = x; rect.Y = y;
	rect.Width = width; rect.Height = height;
		
	GdipCombineRegionRect (graphics->clip, &rect, combineMode);
	gdip_set_cairo_clipping (graphics);
	return Ok;
}

GpStatus
GdipSetClipRectI (GpGraphics *graphics, UINT x, UINT y, UINT width, UINT height, CombineMode combineMode)
{
	GpRect rect;
	
	if (!graphics)
		return InvalidParameter;
	
	rect.X = x; rect.Y = y;
	rect.Width = width; rect.Height = height;
		
	GdipCombineRegionRectI (graphics->clip, &rect, combineMode);	
	gdip_set_cairo_clipping (graphics);
	return Ok;
}

GpStatus
GdipSetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode)
{
	return NotImplemented;
}

GpStatus
GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode)
{
	if (!graphics || !region)
		return InvalidParameter;
	
	GdipCombineRegionRegion (graphics->clip, region, combineMode);	
	gdip_set_cairo_clipping (graphics);
	return Ok;
}

GpStatus
GdipSetClipHrgn (GpGraphics *graphics, void *hRgn, CombineMode combineMode)
{
	return NotImplemented;
}

GpStatus
GdipResetClip (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;
	
	GdipSetInfinite (graphics->clip);
	cairo_reset_clip (graphics->ct);
	return Ok;
}

GpStatus
GdipTranslateClip (GpGraphics *graphics, float dx, float dy)
{
	return GdipTranslateRegion (graphics->clip, dx, dy);
}

GpStatus
GdipTranslateClipI (GpGraphics *graphics, UINT dx, UINT dy)
{
	return GdipTranslateRegionI (graphics->clip, dx, dy);
}

GpStatus
GdipGetClip (GpGraphics *graphics, GpRegion *region)
{
	if (!graphics || !region)
		return InvalidParameter;
	
	if (region->rects)
		GdipFree (region->rects);
	
	region->rects = (GpRectF *) GdipAlloc (sizeof (GpRectF) * graphics->clip->cnt);
	memcpy (region->rects, graphics->clip->rects, sizeof (GpRectF) * graphics->clip->cnt);
	region->cnt = graphics->clip->cnt;
	return Ok;
}


GpStatus
GdipGetClipBounds (GpGraphics *graphics, GpRectF *rect)
{
	return GdipGetRegionBounds (graphics->clip, graphics, rect);
}

GpStatus
GdipGetClipBoundsI (GpGraphics *graphics, GpRect *rect)
{
	GpRectF rectF;
	Status status;
	
	status =  GdipGetRegionBounds (graphics->clip, graphics, &rectF);
	
	if (status != Ok)
		return status;
	
	graphics->bounds.X = rectF.X;
	graphics->bounds.Y = rectF.Y;
	graphics->bounds.Width = rectF.Width;
	graphics->bounds.Height = rectF.Height;
	
	return Ok;
}

GpStatus
GdipIsClipEmpty (GpGraphics *graphics, BOOL *result)
{
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
		
	rect->X = graphics->bounds.X;
	rect->Y = graphics->bounds.Y;
	rect->Width = graphics->bounds.Width;
	rect->Height = graphics->bounds.Height;
	
	return Ok;
}

GpStatus
GdipGetVisibleClipBoundsI (GpGraphics *graphics, GpRect *rect)
{	
	GpRectF rectF;
	
	if (!graphics || !rect)
		return InvalidParameter;
	
	GdipGetVisibleClipBounds (graphics, &rectF);
	
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
GdipIsVisiblePointI (GpGraphics *graphics, UINT x, UINT y, BOOL *result)
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
GdipIsVisibleRectI (GpGraphics *graphics, UINT x, UINT y, UINT width, UINT height, BOOL *result)
{
	return GdipIsVisibleRect (graphics, x, y, width, height, result);
}

GpStatus
GdipSetCompositingMode (GpGraphics *graphics, GpCompositingMode compositingMode)
{
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
GdipGetCompositingMode (GpGraphics *graphics, GpCompositingMode *compositingMode)
{
	*compositingMode = graphics->composite_mode;
	return Ok;
}

GpStatus
GdipSetCompositingQuality (GpGraphics *graphics, GpCompositingQuality compositingQuality)
{
	/* In Cairo there is no way of setting this, always use high quality */

	graphics->composite_quality = compositingQuality;
	return Ok;
}

GpStatus
GdipGetCompositingQuality (GpGraphics *graphics, GpCompositingQuality *compositingQuality)
{
	*compositingQuality = graphics->composite_quality;
	return Ok;
}

GpStatus
GdipGetNearestColor (GpGraphics *graphics, ARGB *argb)
{
	return NotImplemented;
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (scale != NULL, InvalidParameter);

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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (unit != NULL, InvalidParameter);

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
