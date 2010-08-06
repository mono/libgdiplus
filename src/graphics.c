/*
 * graphics.c
 *
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
 * Copyright (C) 2006-2007 Novell, Inc (http://www.novell.com)
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
#include "graphics-cairo-private.h"
#include "graphics-metafile-private.h"
#include "region-private.h"
#include "graphics-path-private.h"
#include "brush-private.h"
#include "matrix-private.h"
#include "bitmap-private.h"
#include "metafile-private.h"

#include <cairo-features.h>

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
gdip_graphics_common_init (GpGraphics *graphics)
{
	graphics->image = NULL;
	graphics->type = gtUndefined;

	cairo_identity_matrix (graphics->ct);

	GdipCreateMatrix (&graphics->copy_of_ctm);
	cairo_matrix_init_identity (graphics->copy_of_ctm);

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

	graphics->display = NULL;
	graphics->drawable = NULL;

	gdip_graphics_reset (graphics);
}

static void
gdip_graphics_cairo_init (GpGraphics *graphics, cairo_surface_t *surface)
{
	graphics->backend = GraphicsBackEndCairo;

	graphics->metafile = NULL;
	graphics->ct = cairo_create (surface);

#ifndef NO_CAIRO_AA
        cairo_set_shape_format (graphics->ct, CAIRO_FORMAT_A1);
#endif
        /* cairo_select_font_face (graphics->ct, "serif:12"); */
	cairo_select_font_face (graphics->ct, "serif:12", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	gdip_graphics_common_init (graphics);
}

GpGraphics *
gdip_graphics_new (cairo_surface_t *surface)
{
	GpGraphics *result = (GpGraphics *) GdipAlloc (sizeof (GpGraphics));

	if (result)
		gdip_graphics_cairo_init (result, surface);

	return result;
}

static void
gdip_graphics_metafile_init (GpGraphics *graphics, GpMetafile *metafile)
{
	graphics->backend = GraphicsBackEndMetafile;
	/* some API requires a valid cairo context (even on a metafile-based graphics) */
	graphics->metasurface = cairo_image_surface_create (CAIRO_FORMAT_A1, 1, 1);
	graphics->ct = cairo_create (graphics->metasurface);
	graphics->metafile = metafile;

	gdip_graphics_common_init (graphics);
}

GpGraphics*
gdip_metafile_graphics_new (GpMetafile *metafile)
{
	GpGraphics *result = (GpGraphics *) GdipAlloc (sizeof (GpGraphics));

	if (result)
		gdip_graphics_metafile_init (result, metafile);

	return result;
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

	if (!hDC)
		return OutOfMemory;

#ifdef CAIRO_HAS_PS_SURFACE

	if (clone->type == gtPostScript) {
		*graphics = clone;
		return Ok;
	}
#endif

	if (clone->type == gtMemoryBitmap)
		return GdipGetImageGraphicsContext (clone->image, graphics);
	
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

	if ((*graphics)->drawable)
		(*graphics)->drawable = clone->drawable;

	if ((*graphics)->display)
		(*graphics)->display = clone->display;	

	return Ok;
}

GpStatus
GdipCreateFromHWND (void *hwnd, GpGraphics **graphics)
{
	return NotImplemented;
}

#endif

#ifdef CAIRO_HAS_QUARTZ_SURFACE
// coverity[+alloc : arg-*3]
GpStatus
GdipCreateFromContext_macosx (void *ctx, int width, int height, GpGraphics **graphics)
{
	cairo_surface_t *surface;

	if (!graphics)
		return InvalidParameter;

	surface = cairo_quartz_surface_create (0, width, height);

	*graphics = gdip_graphics_new(surface);
	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);
	
	(*graphics)->bounds.Width = width;
	(*graphics)->bounds.Height = height;

	(*graphics)->type = gtOSXDrawable;
	(*graphics)->cg_context = ctx;

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

	if (graphics->backend == GraphicsBackEndMetafile) {
		/* if recording this is where we save the metafile (stream or file) */
		if (graphics->metafile->recording)
			gdip_metafile_stop_recording (graphics->metafile);
		cairo_surface_destroy (graphics->metasurface);
		graphics->metasurface = NULL;
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
GdipGetDC (GpGraphics *graphics, HDC *hDC)
{
	/* For our gdi+ the hDC is equivalent to the graphics handle */
	if (hDC) {
		*hDC = (void *)graphics;
	}
	return Ok;
}

GpStatus 
GdipReleaseDC (GpGraphics *graphics, HDC hDC)
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
	return cairo_SetGraphicsClip (graphics);
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

	cairo_matrix_init_identity (graphics->copy_of_ctm);
	cairo_matrix_init_identity (graphics->clip_matrix);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_ResetWorldTransform (graphics);
	case GraphicsBackEndMetafile:
		return metafile_ResetWorldTransform (graphics);
	default:
		return GenericError;
	}
}

GpStatus
GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	GpStatus status;
	BOOL invertible;

	if (!graphics || !matrix)
		return InvalidParameter;

	/* optimization - inverting an identity matrix result in the identity matrix */
	if (gdip_is_matrix_empty (matrix))
		return GdipResetWorldTransform (graphics);

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	gdip_cairo_matrix_copy (graphics->copy_of_ctm, matrix);
	gdip_cairo_matrix_copy (graphics->clip_matrix, matrix);

	/* we already know it's invertible */
	GdipInvertMatrix (graphics->clip_matrix);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_SetWorldTransform (graphics, matrix);
	case GraphicsBackEndMetafile:
		return metafile_SetWorldTransform (graphics, matrix);
	default:
		return GenericError;
	}
}

GpStatus 
GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	if (!graphics || !matrix)
		return InvalidParameter;

	/* get the effective matrix from cairo */
	gdip_cairo_matrix_copy (matrix, graphics->copy_of_ctm);

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
        GpStatus s;
	BOOL invertible;
	GpMatrix inverted;

	if (!graphics)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	s = GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible || (s != Ok))
		return InvalidParameter;

	s = GdipMultiplyMatrix (graphics->copy_of_ctm, matrix, order);
        if (s != Ok)
                return s;

	/* Multiply the inverted matrix with the clipping matrix */
	gdip_cairo_matrix_copy (&inverted, matrix);
	s = GdipInvertMatrix (&inverted);
        if (s != Ok)
                return s;

	s = GdipMultiplyMatrix (graphics->clip_matrix, &inverted, order);
	if (s != Ok)
		return s;

	apply_world_to_bounds (graphics);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* not a typo - we apply to calculated matrix to cairo context */
		return cairo_SetWorldTransform (graphics, graphics->copy_of_ctm);
	case GraphicsBackEndMetafile:
		return metafile_MultiplyWorldTransform (graphics, matrix, order);
	default:
		return GenericError;
	}
}

GpStatus 
GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order)
{
	GpStatus s;

	if (!graphics)
		return InvalidParameter;

	s = GdipRotateMatrix (graphics->copy_of_ctm, angle, order);
        if (s != Ok)
                return s;

	s = GdipRotateMatrix (graphics->clip_matrix, -angle, gdip_matrix_reverse_order (order));
	if (s != Ok)
		return s;

	apply_world_to_bounds (graphics);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* not a typo - we apply to calculated matrix to cairo context */
		return cairo_SetWorldTransform (graphics, graphics->copy_of_ctm);
	case GraphicsBackEndMetafile:
		return metafile_RotateWorldTransform (graphics, angle, order);
	default:
		return GenericError;
	}
}

GpStatus
GdipScaleWorldTransform (GpGraphics *graphics, float sx, float sy, GpMatrixOrder order)
{
	GpStatus s;

	if (!graphics || (sx == 0.0f) || (sy == 0.0f))
		return InvalidParameter;

        s = GdipScaleMatrix (graphics->copy_of_ctm, sx, sy, order);
        if (s != Ok)
                return s;

	s = GdipScaleMatrix (graphics->clip_matrix, (1.0f / sx), (1.0f / sy), gdip_matrix_reverse_order (order));
	if (s != Ok)
		return s;

	apply_world_to_bounds (graphics);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* not a typo - we apply to calculated matrix to cairo context */
		return cairo_SetWorldTransform (graphics, graphics->copy_of_ctm);
	case GraphicsBackEndMetafile:
		return metafile_ScaleWorldTransform (graphics, sx, sy, order);
	default:
		return GenericError;
	}
}

GpStatus 
GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order)
{
	GpStatus s;

	if (!graphics)
		return InvalidParameter;

        s = GdipTranslateMatrix (graphics->copy_of_ctm, dx, dy, order);
        if (s != Ok) 
                return s;

	s = GdipTranslateMatrix (graphics->clip_matrix, -dx, -dy, gdip_matrix_reverse_order (order));
	if (s != Ok)
		return s;
	
	apply_world_to_bounds (graphics);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* not a typo - we apply to calculated matrix to cairo context */
		return cairo_SetWorldTransform (graphics, graphics->copy_of_ctm);
	case GraphicsBackEndMetafile:
		return metafile_TranslateWorldTransform (graphics, dx, dy, order);
	default:
		return GenericError;
	}
}

/*
 * Draw operations - validate parameters and delegate to cairo/metafile backends
 */

GpStatus
GdipDrawArc (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, float sweepAngle)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawArc (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	case GraphicsBackEndMetafile:
		return metafile_DrawArc (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawArcI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawArcI (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	case GraphicsBackEndMetafile:
		return metafile_DrawArcI (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	default:
		return GenericError;
	}
}

GpStatus 
GdipDrawBezier (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2, float x3, float y3, 
	float x4, float y4)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawBezier (graphics, pen, x1, y1, x2, y2, x3, y3, x4, y4);
	case GraphicsBackEndMetafile:
		return metafile_DrawBezier (graphics, pen, x1, y1, x2, y2, x3, y3, x4, y4);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawBezierI (graphics, pen, x1, y1, x2, y2, x3, y3, x4, y4);
	case GraphicsBackEndMetafile:
		return metafile_DrawBezierI (graphics, pen, x1, y1, x2, y2, x3, y3, x4, y4);
	default:
		return GenericError;
	}
}

GpStatus 
GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	if (count == 0)
		return Ok;

	if (!graphics || !pen || !points)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawBeziers (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawBeziers (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
	if (count == 0)
		return Ok;

	if (!graphics || !pen || !points)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawBeziersI (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawBeziersI (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus 
GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height)
{	
	if (!graphics || !pen)
		return InvalidParameter;
	
	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawEllipse (graphics, pen, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_DrawEllipse (graphics, pen, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawEllipseI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawEllipseI (graphics, pen, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_DrawEllipseI (graphics, pen, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawLine (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawLine (graphics, pen, x1, y1, x2, y2);
	case GraphicsBackEndMetafile:
		return metafile_DrawLine (graphics, pen, x1, y1, x2, y2);
	default:
		return GenericError;
	}
}

GpStatus 
GdipDrawLineI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2)
{
	if (!graphics || !pen)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawLineI (graphics, pen, x1, y1, x2, y2);
	case GraphicsBackEndMetafile:
		return metafile_DrawLineI (graphics, pen, x1, y1, x2, y2);
	default:
		return GenericError;
	}
}

GpStatus 
GdipDrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawLines (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawLines (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus 
GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawLinesI (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawLinesI (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
	if (!graphics || !pen || !path)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawPath (graphics, pen, path);
	case GraphicsBackEndMetafile:
		return metafile_DrawPath (graphics, pen, path);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, float sweepAngle)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawPie (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	case GraphicsBackEndMetafile:
		return metafile_DrawPie (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawPieI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawPieI (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	case GraphicsBackEndMetafile:
		return metafile_DrawPieI (graphics, pen, x, y, width, height, startAngle, sweepAngle);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, int count)
{
	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawPolygon (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawPolygon (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count)
{
	if (!graphics || !pen || !points || count < 2)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawPolygonI (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawPolygonI (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawRectangle (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* don't draw/fill rectangles with negative width/height (bug #77129) */
	if ((width < 0) || (height < 0))
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawRectangle (graphics, pen, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_DrawRectangle (graphics, pen, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height)
{
	if (!graphics || !pen)
		return InvalidParameter;

	/* don't draw/fill rectangles with negative width/height (bug #77129) */
	if ((width < 0) || (height < 0))
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawRectangle (graphics, pen, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_DrawRectangle (graphics, pen, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, int count)
{
	if (!graphics || !pen || !rects || count <= 0)
		return InvalidParameter;
	
	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawRectangles (graphics, pen, rects, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawRectangles (graphics, pen, rects, count);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects, int count)
{
	if (!graphics || !pen || !rects || count <= 0)
		return InvalidParameter;
	
	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawRectanglesI (graphics, pen, rects, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawRectanglesI (graphics, pen, rects, count);
	default:
		return GenericError;
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
	/* when tension is 0, draw straight lines */
	if (tension == 0)
		return GdipDrawPolygon (graphics, pen, points, count);

	if (!graphics || !pen || !points || count <= 2)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawClosedCurve2 (graphics, pen, points, count, tension);
	case GraphicsBackEndMetafile:
		return metafile_DrawClosedCurve2 (graphics, pen, points, count, tension);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, int count, float tension)
{
	/* when tension is 0, draw straight lines */
	if (tension == 0)
		return GdipDrawPolygonI (graphics, pen, points, count);

	if (!graphics || !pen || !points || count <= 2)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawClosedCurve2I (graphics, pen, points, count, tension);
	case GraphicsBackEndMetafile:
		return metafile_DrawClosedCurve2I (graphics, pen, points, count, tension);
	default:
		return GenericError;
	}
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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawCurve3 (graphics, pen, points, count, offset, numOfSegments, tension);
	case GraphicsBackEndMetafile:
		return metafile_DrawCurve3 (graphics, pen, points, count, offset, numOfSegments, tension);
	default:
		return GenericError;
	}
}

GpStatus
GdipDrawCurve3I (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPoint *points, int count, int offset, int numOfSegments, float tension)
{
	/* draw lines if tension = 0 */
	if (tension == 0)
		return GdipDrawLinesI (graphics, pen, points, count);

	if (!graphics || !pen || !points || numOfSegments < 1)
		return InvalidParameter;

	/* we need 3 points for the first curve, 2 more for each curves */
	/* and it's possible to use a point prior to the offset (to calculate) */
	if ((offset == 0) && (numOfSegments == 1) && (count < 3))
		return InvalidParameter;
	else if (numOfSegments >= count - offset)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawCurve3I (graphics, pen, points, count, offset, numOfSegments, tension);
	case GraphicsBackEndMetafile:
		return metafile_DrawCurve3I (graphics, pen, points, count, offset, numOfSegments, tension);
	default:
		return GenericError;
	}
}

/*
 * Fills
 */
GpStatus
GdipFillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height)
{
	if (!graphics || !brush)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillEllipse (graphics, brush, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_FillEllipse (graphics, brush, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height)
{
	if (!graphics || !brush)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillEllipseI (graphics, brush, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_FillEllipseI (graphics, brush, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus 
GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* don't draw/fill rectangles with negative width/height (bug #77129) */
	if ((width < 0) || (height < 0))
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillRectangle (graphics, brush, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_FillRectangle (graphics, brush, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus 
GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* don't draw/fill rectangles with negative width/height (bug #77129) */
	if ((width < 0) || (height < 0))
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillRectangleI (graphics, brush, x, y, width, height);
	case GraphicsBackEndMetafile:
		return metafile_FillRectangleI (graphics, brush, x, y, width, height);
	default:
		return GenericError;
	}
}

GpStatus 
GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, int count)
{
	if (!graphics || !brush || !rects || count <= 0)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillRectangles (graphics, brush, rects, count);
	case GraphicsBackEndMetafile:
		return metafile_FillRectangles (graphics, brush, rects, count);
	default:
		return GenericError;
	}
}

GpStatus 
GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRect *rects, int count)
{
	if (!graphics || !brush || !rects || count <= 0)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillRectanglesI (graphics, brush, rects, count);
	case GraphicsBackEndMetafile:
		return metafile_FillRectanglesI (graphics, brush, rects, count);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillPie (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, 
	float startAngle, float sweepAngle)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillPie (graphics, brush, x, y, width, height, startAngle, sweepAngle);
	case GraphicsBackEndMetafile:
		return metafile_FillPie (graphics, brush, x, y, width, height, startAngle, sweepAngle);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillPieI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height, float startAngle, float sweepAngle)
{
	if (!graphics || !brush)
		return InvalidParameter;

	/* We don't do anything, if sweep angle is zero. */
	if (sweepAngle == 0)
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillPieI (graphics, brush, x, y, width, height, startAngle, sweepAngle);
	case GraphicsBackEndMetafile:
		return metafile_FillPieI (graphics, brush, x, y, width, height, startAngle, sweepAngle);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
	if (!graphics || !brush || !path)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillPath (graphics, brush, path);
	case GraphicsBackEndMetafile:
		return metafile_FillPath (graphics, brush, path);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, int count, FillMode fillMode)
{
	if (!graphics || !brush || !points)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillPolygon (graphics, brush, points, count, fillMode);
	case GraphicsBackEndMetafile:
		return metafile_FillPolygon (graphics, brush, points, count, fillMode);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count, FillMode fillMode)
{
	if (!graphics || !brush || !points)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillPolygonI (graphics, brush, points, count, fillMode);
	case GraphicsBackEndMetafile:
		return metafile_FillPolygonI (graphics, brush, points, count, fillMode);
	default:
		return GenericError;
	}
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
        /* when tension is 0, the edges are straight lines */
        if (tension == 0)
                return GdipFillPolygon2 (graphics, brush, points, count);

	if (!graphics || !brush || !points || count <= 0)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillClosedCurve2 (graphics, brush, points, count, tension);
	case GraphicsBackEndMetafile:
		return metafile_FillClosedCurve2 (graphics, brush, points, count, tension);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, int count, float tension)
{
        /* when tension is 0, the edges are straight lines */
        if (tension == 0)
                return GdipFillPolygon2I (graphics, brush, points, count);

	if (!graphics || !brush || !points || count <= 0)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillClosedCurve2I (graphics, brush, points, count, tension);
	case GraphicsBackEndMetafile:
		return metafile_FillClosedCurve2I (graphics, brush, points, count, tension);
	default:
		return GenericError;
	}
}

GpStatus
GdipFillRegion (GpGraphics *graphics, GpBrush *brush, GpRegion *region)
{
        if (!graphics || !brush || !region)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillRegion (graphics, brush, region);
	case GraphicsBackEndMetafile:
		return metafile_FillRegion (graphics, brush, region);
	default:
		return GenericError;
	}
}

GpStatus 
GdipSetRenderingOrigin (GpGraphics *graphics, int x, int y)
{
	if (!graphics)
		return InvalidParameter;

	graphics->render_origin_x = x;
	graphics->render_origin_y = y;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetRenderingOrigin (graphics, x, y);
	default:
		return GenericError;
	}
}

GpStatus 
GdipGetRenderingOrigin (GpGraphics *graphics, int *x, int *y)
{
	if (!graphics || !x || !y)
		return InvalidParameter;

        *x = graphics->render_origin_x;
        *y = graphics->render_origin_y;
	return Ok;
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
	if (!graphics)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_GraphicsClear (graphics, color);
	case GraphicsBackEndMetafile:
		return metafile_GraphicsClear (graphics, color);
	default:
		return GenericError;
	}
}

GpStatus
GdipSetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode)
{
	if (!graphics)
		return InvalidParameter;

	graphics->interpolation = interpolationMode;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetInterpolationMode (graphics, interpolationMode);
	default:
		return GenericError;
	}
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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetTextRenderingHint (graphics, mode);
	default:
		return GenericError;
	}
}

GpStatus
GdipGetTextRenderingHint (GpGraphics *graphics, TextRenderingHint *mode)
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
	
	graphics->pixel_mode = pixelOffsetMode;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* FIXME: changing pixel mode affects other properties (e.g. the visible clip bounds) */
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetPixelOffsetMode (graphics, pixelOffsetMode);
	default:
		return GenericError;
	}
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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetTextContrast (graphics, contrast);
	default:
		return GenericError;
	}
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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_SetSmoothingMode (graphics, mode);
	case GraphicsBackEndMetafile:
		return metafile_SetSmoothingMode (graphics, mode);
	default:
		return GenericError;
	}
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

#ifdef CAIRO_HAS_QUARTZ_SURFACE
	if (graphics->type == gtOSXDrawable) {
		CGRect rect;
	
		rect.origin.x = 0;
		rect.origin.y = 0;
		rect.size.width = graphics->bounds.Width;
		rect.size.height = graphics->bounds.Height;
		void *image = CGBitmapContextCreateImage (cairo_quartz_surface_get_cg_context (surface));
		CGContextDrawImage (graphics->cg_context, rect, image);
		CGImageRelease (image);
	}
#endif
	return Ok;
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

	if (!graphics)
		return InvalidParameter;

	rect.X = x;
	rect.Y = y;
	rect.Width = width;
	rect.Height = height;

	status = GdipCreateRegionRect (&rect, &region);
	if (status != Ok)
		goto cleanup;

	/* if the matrix is empty, avoid region transformation */
	if (!gdip_is_matrix_empty (graphics->clip_matrix)) {
		cairo_matrix_t inverted;

		gdip_cairo_matrix_copy (&inverted, graphics->clip_matrix);
		cairo_matrix_invert (&inverted);

		GdipTransformRegion (region, &inverted);
	}

	status = GdipCombineRegionRegion (graphics->clip, region, combineMode);	
	if (status != Ok)
		goto cleanup;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* adjust cairo clipping according to graphics->clip */
		status = cairo_SetGraphicsClip (graphics);
		break;
	case GraphicsBackEndMetafile:
		status = metafile_SetClipRect (graphics, x, y, width, height, combineMode);
		break;
	default:
		status = GenericError;
		break;
	}

cleanup:
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

	if (!graphics || !path)
		return InvalidParameter;

	status = GdipCombineRegionPath (graphics->clip, path, combineMode);	
	if (status != Ok)
		return status;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* adjust cairo clipping according to graphics->clip */
		return cairo_SetGraphicsClip (graphics);
	case GraphicsBackEndMetafile:
		return metafile_SetClipPath (graphics, path, combineMode);
	default:
		return GenericError;
	}
}

GpStatus
GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode)
{
	GpStatus status;
	GpRegion *work;

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
	if (status != Ok)
		goto cleanup;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* adjust cairo clipping according to graphics->clip */
		status = cairo_SetGraphicsClip (graphics);
		break;
	case GraphicsBackEndMetafile:
		status = metafile_SetClipRegion (graphics, region, combineMode);
		break;
	default:
		status = GenericError;
		break;
	}

cleanup:
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

	GdipSetInfinite (graphics->clip);
	cairo_matrix_init_identity (graphics->clip_matrix);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_ResetClip (graphics);
	case GraphicsBackEndMetafile:
		return metafile_ResetClip (graphics);
	default:
		return GenericError;
	}
}

GpStatus
GdipTranslateClip (GpGraphics *graphics, float dx, float dy)
{
	GpStatus status;

	if (!graphics)
		return InvalidParameter;

	status = GdipTranslateRegion (graphics->clip, dx, dy);
	if (status != Ok)
		return status;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* adjust cairo clipping according to graphics->clip */
		return cairo_SetGraphicsClip (graphics);
	case GraphicsBackEndMetafile:
		return metafile_TranslateClip (graphics, dx, dy);
	default:
		return GenericError;
	}
}

GpStatus
GdipTranslateClipI (GpGraphics *graphics, int dx, int dy)
{
	return GdipTranslateClip (graphics, dx, dy);
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
	GpStatus status;

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
		
	*result = (graphics->bounds.Width == 0 || graphics->bounds.Height == 0);
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
	return GdipIsVisiblePoint (graphics, x, y, result);
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

	recthit.X = x;
	recthit.Y = y;
	recthit.Width = width;
	recthit.Height = height;

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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_SetCompositingMode (graphics, compositingMode);
	case GraphicsBackEndMetafile:
		return metafile_SetCompositingMode (graphics, compositingMode);
	default:
		return GenericError;
	}
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

	graphics->composite_quality = compositingQuality;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* In Cairo there is no way of setting this, always use high quality */
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetCompositingQuality (graphics, compositingQuality);
	default:
		return GenericError;
	}
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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		/* page unit and scale are in the same EMF+ record */
		return metafile_SetPageTransform (graphics, graphics->page_unit, scale);
	default:
		return GenericError;
	}
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

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		/* page unit and scale are in the same EMF+ record */
		return metafile_SetPageTransform (graphics, unit, graphics->scale);
	default:
		return GenericError;
	}
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
