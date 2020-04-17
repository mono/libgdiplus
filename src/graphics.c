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
 *   Frederik Carlier <frederik.carlier@quamotion.mobi>
 */

#include "graphics-private.h"
#include "general-private.h"
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
	graphics->orig_bounds.X = graphics->orig_bounds.Y = graphics->orig_bounds.Width = graphics->orig_bounds.Height = 0;
	graphics->last_pen = NULL;
	graphics->last_brush = NULL;
	graphics->saved_status = NULL;
	graphics->saved_status_pos = 0;
	graphics->render_origin_x = 0;
	graphics->render_origin_y = 0;
	graphics->dpi_x = graphics->dpi_y = 0;
	graphics->state = GraphicsStateValid;

#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
	graphics->display = (Display*)NULL;
	graphics->drawable = (Drawable)NULL;
#endif

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

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateFromHDC (HDC hdc, GpGraphics **graphics)
{
	GpGraphics *clone = (GpGraphics *)hdc;
#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
	cairo_surface_t *surface;
	int x, y;
	unsigned int w, h, border_w, depth;
#endif

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!graphics)
		return InvalidParameter;

	if (!hdc)
		return OutOfMemory;

#if defined(WIN32)
	// HDC returns to a device context. The remainer of this function assumes that hdc really
	// is a GpGrahpics, but that's almost guaranteed to be not the case on Windows. Just fail
	// quickly instead of segfauling.
	return NotImplemented;
#endif

#ifdef CAIRO_HAS_PS_SURFACE

	if (clone->type == gtPostScript) {
		*graphics = clone;
		return Ok;
	}
#endif

	if (clone->type == gtMemoryBitmap)
		return GdipGetImageGraphicsContext (clone->image, graphics);

#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
	Window root;
	XGetGeometry (clone->display, clone->drawable, &root,
		      &x, &y, &w, &h, &border_w, &depth);
	
	surface = cairo_xlib_surface_create(clone->display, clone->drawable,
	    DefaultVisual(clone->display, DefaultScreen(clone->display)),
	    w, h);
			
	*graphics = gdip_graphics_new (surface);
	if (!*graphics) {
		cairo_surface_destroy (surface);
		return OutOfMemory;
	}

	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);

	if ((*graphics)->drawable)
		(*graphics)->drawable = clone->drawable;

	if ((*graphics)->display)
		(*graphics)->display = clone->display;	

	return Ok;
#endif

	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipCreateFromHDC2 (HDC hdc, HANDLE hDevice, GpGraphics **graphics)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!graphics)
		return InvalidParameter;

	if (hDevice)
		return NotImplemented;

	return GdipCreateFromHDC (hdc, graphics);
}

GpStatus WINGDIPAPI
GdipCreateFromHWND (HWND hwnd, GpGraphics **graphics)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!graphics)
		return InvalidParameter;

	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipCreateFromHWNDICM (HWND hwnd, GpGraphics **graphics)
{
	return GdipCreateFromHWND (hwnd, graphics);
}

#ifdef CAIRO_HAS_QUARTZ_SURFACE
// coverity[+alloc : arg-*3]
GpStatus
GdipCreateFromContext_macosx (void *ctx, int width, int height, GpGraphics **graphics)
{
	cairo_surface_t *surface;

	if (!graphics)
		return InvalidParameter;

	surface = cairo_quartz_surface_create (0, width, height);

	*graphics = gdip_graphics_new (surface);
	if (!*graphics) {
		cairo_surface_destroy (surface);
		return OutOfMemory;
	}

	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);
	
	(*graphics)->bounds.Width = width;
	(*graphics)->bounds.Height = height;
	(*graphics)->orig_bounds.Width = width;
	(*graphics)->orig_bounds.Height = height;

	(*graphics)->type = gtOSXDrawable;
	(*graphics)->cg_context = ctx;

	return Ok;
}

#endif

#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE

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
	if (!*graphics) {
		cairo_surface_destroy (surface);
		return OutOfMemory;
	}

	(*graphics)->dpi_x = (*graphics)->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);

	(*graphics)->type = gtX11Drawable;
	(*graphics)->display = dpy;
	(*graphics)->drawable = d;

	GdipSetVisibleClip_linux (*graphics, &bounds);
	return Ok;
}

#endif

#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
static int
ignore_error_handler (Display *dpy, XErrorEvent *event)
{
	return Success;
}
#endif

GpStatus WINGDIPAPI
GdipDeleteGraphics (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;

	if (graphics->state != GraphicsStateValid)
		return ObjectBusy;

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
#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
		int (*old_error_handler)(Display *dpy, XErrorEvent *ev) = NULL;
		if (graphics->type == gtX11Drawable)
			old_error_handler = XSetErrorHandler (ignore_error_handler);
#endif

		cairo_destroy (graphics->ct);
		graphics->ct = NULL;

#if defined(HAVE_X11) && CAIRO_HAS_XLIB_SURFACE
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

GpStatus WINGDIPAPI
GdipGetDC (GpGraphics *graphics, HDC *hdc)
{
	if (!graphics || !hdc)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*hdc = (void *)graphics;
	graphics->state = GraphicsStateBusy;

	return Ok;
}

GpStatus WINGDIPAPI
GdipReleaseDC (GpGraphics *graphics, HDC hdc)
{
	if (!graphics || !hdc || graphics->state != GraphicsStateBusy)
		return InvalidParameter;

	if (hdc != (void *)graphics)
		return InvalidParameter;

	graphics->state = GraphicsStateValid;

	return Ok;
}

GpStatus WINGDIPAPI
GdipRestoreGraphics (GpGraphics *graphics, GraphicsState state)
{
	GpStatus status;
	GpState *pos_state;

	if (!graphics)
		return InvalidParameter;

	if (state <= 0 || (state - 1) >= MAX_GRAPHICS_STATE_STACK || (state - 1) > graphics->saved_status_pos)
		return InvalidParameter;

	pos_state = graphics->saved_status;
	pos_state += (state - 1);	

	/* Save from GpState to Graphics  */
	gdip_cairo_matrix_copy (graphics->copy_of_ctm, &pos_state->matrix);
	gdip_cairo_matrix_copy (&graphics->previous_matrix, &pos_state->previous_matrix);

	GdipSetRenderingOrigin (graphics, pos_state->org_x, pos_state->org_y);

	if (graphics->clip)
		GdipDeleteRegion (graphics->clip);
	status = GdipCloneRegion (pos_state->clip, &graphics->clip);
	if (status != Ok)
		return status;

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

	graphics->saved_status_pos = state - 1;

	/* re-adjust clipping (region and matrix) */
	gdip_cairo_set_matrix (graphics, graphics->copy_of_ctm);

	/* GdipCloneRegion was called, but for some reason, not registred as an allocation */
	/* coverity[freed_arg] */
	return cairo_SetGraphicsClip (graphics);
}

GpStatus WINGDIPAPI
GdipSaveGraphics (GpGraphics *graphics, GraphicsState *state)
{
	GpStatus status;
	GpState* pos_state;

	if (!graphics || !state)
		return InvalidParameter;

	if (!graphics->saved_status) {
		graphics->saved_status = gdip_calloc (MAX_GRAPHICS_STATE_STACK, sizeof (GpState));
		if (!graphics->saved_status)
			return OutOfMemory;

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
	status = GdipCloneRegion (graphics->clip, &pos_state->clip);
	if (status != Ok)
		return status;

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
	
	*state = graphics->saved_status_pos + 1; // make sure GraphicsState is non-zero for compat with GDI+
	graphics->saved_status_pos++;
	return Ok;
}

static void
apply_world_to_bounds (GpGraphics *graphics)
{
	GpPointF pts[2];

	pts[0].X = graphics->orig_bounds.X;
	pts[0].Y = graphics->orig_bounds.Y;
	pts[1].X = graphics->orig_bounds.X + graphics->orig_bounds.Width;
	pts[1].Y = graphics->orig_bounds.Y + graphics->orig_bounds.Height;
	GdipTransformMatrixPoints (graphics->clip_matrix, pts, 2);

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
}

GpStatus WINGDIPAPI
GdipResetWorldTransform (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;

	if (!gdip_is_matrix_empty (&graphics->previous_matrix)) {
		/* inside a container only reset to the previous transform */
		gdip_cairo_matrix_copy (graphics->copy_of_ctm, &graphics->previous_matrix);
		gdip_cairo_matrix_copy (graphics->clip_matrix, &graphics->previous_matrix);
		GdipInvertMatrix (graphics->clip_matrix);
	} else {
		cairo_matrix_init_identity (graphics->copy_of_ctm);
		cairo_matrix_init_identity (graphics->clip_matrix);
	}

	apply_world_to_bounds (graphics);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_ResetWorldTransform (graphics);
	case GraphicsBackEndMetafile:
		return metafile_ResetWorldTransform (graphics);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	BOOL invertible;

	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!matrix)
		return InvalidParameter;

	// Inverting an identity matrix result in the identity matrix.
	if (gdip_is_matrix_empty (matrix))
		return GdipResetWorldTransform (graphics);

	// The matrix must be invertible to be used.
	GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible)
		return InvalidParameter;

	GpMatrix matrixCopy;
	gdip_cairo_matrix_copy (&matrixCopy, matrix);
	
	if (!gdip_is_matrix_empty (&graphics->previous_matrix)) {
		/* inside a container the transform is appended to the previous transform */
		GdipMultiplyMatrix (&matrixCopy, &graphics->previous_matrix, MatrixOrderAppend);
	}
	
	gdip_cairo_matrix_copy (graphics->copy_of_ctm, &matrixCopy);
	gdip_cairo_matrix_copy (graphics->clip_matrix, &matrixCopy);
	
	/* we already know it's invertible */
	GdipInvertMatrix (graphics->clip_matrix);

	apply_world_to_bounds (graphics);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_SetWorldTransform (graphics, &matrixCopy);
	case GraphicsBackEndMetafile:
		return metafile_SetWorldTransform (graphics, &matrixCopy);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
	if (!graphics || !matrix)
		return InvalidParameter;
	
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
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

GpStatus WINGDIPAPI
GdipRotateWorldTransform (GpGraphics *graphics, REAL angle, GpMatrixOrder order)
{
	GpStatus s;

	if (!graphics)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipScaleWorldTransform (GpGraphics *graphics, REAL sx, REAL sy, GpMatrixOrder order)
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

GpStatus WINGDIPAPI
GdipTranslateWorldTransform (GpGraphics *graphics, REAL dx, REAL dy, GpMatrixOrder order)
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

GpStatus WINGDIPAPI
GdipDrawArc (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || width <= 0 || height <= 0)
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

GpStatus WINGDIPAPI
GdipDrawArcI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle)
{
	return GdipDrawArc (graphics, pen, x, y, width, height, startAngle, sweepAngle);
}

GpStatus WINGDIPAPI
GdipDrawBezier (GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3,
	REAL x4, REAL y4)
{
	PointF points[4] = {
		{x1, y1},
		{x2, y2},
		{x3, y3},
		{x4, y4}
	};
	
	return GdipDrawBeziers (graphics, pen, points, 4);
}

GpStatus WINGDIPAPI
GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, INT x4, INT y4)
{
	return GdipDrawBezier (graphics, pen, x1, y1, x2, y2, x3, y3, x4, y4);
}

GpStatus WINGDIPAPI
GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count)
{
	if (!graphics || !points || count <= 0 || (count > 3 && (count % 3) != 1))
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen)
		return InvalidParameter;

	// Nop if the count is too small to fit any bezier paths.
	if (count < 3)
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawBeziers (graphics, pen, points, count);
	case GraphicsBackEndMetafile:
		return metafile_DrawBeziers (graphics, pen, points, count);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count)
{
	GpStatus status;
	GpPointF *pointsF;
	
	if (!points || count < 0)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawBeziers (graphics, pen, pointsF, count);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height)
{	
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen)
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

GpStatus WINGDIPAPI
GdipDrawEllipseI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height)
{
	return GdipDrawEllipse (graphics, pen, x, y, width, height);
}

GpStatus WINGDIPAPI
GdipDrawLine (GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1, REAL x2, REAL y2)
{
	PointF points[2] = {
		{x1, y1},
		{x2, y2}
	};
	return GdipDrawLines (graphics, pen, points, 2);
}

GpStatus WINGDIPAPI
GdipDrawLineI (GpGraphics *graphics, GpPen *pen, INT x1, INT y1, INT x2, INT y2)
{
	return GdipDrawLine (graphics, pen, x1, y1, x2, y2);
}

GpStatus WINGDIPAPI
GdipDrawLines (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count)
{
	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || count < 2)
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

GpStatus WINGDIPAPI
GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawLines (graphics, pen, pointsF, count);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || !path)
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

GpStatus WINGDIPAPI
GdipDrawPie (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || width <= 0 || height <= 0)
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

GpStatus WINGDIPAPI
GdipDrawPieI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle)
{
	return GdipDrawPie (graphics, pen, x, y, width, height, startAngle, sweepAngle);
}

GpStatus WINGDIPAPI
GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count)
{
	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || count < 2)
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

GpStatus WINGDIPAPI
GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawPolygon (graphics, pen, pointsF, count);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawRectangle (GpGraphics *graphics, GpPen *pen, REAL x, REAL y, REAL width, REAL height)
{
	RectF rect = {x, y, width, height};
	return GdipDrawRectangles (graphics, pen, &rect, 1);
}

GpStatus WINGDIPAPI
GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen, INT x, INT y, INT width, INT height)
{
	return GdipDrawRectangle (graphics, pen, x, y, width, height);
}

GpStatus WINGDIPAPI
GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects, INT count)
{
	if (!graphics || !rects || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen)
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

GpStatus WINGDIPAPI
GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects, INT count)
{
	GpStatus status;
	GpRectF *rectsF;

	if (!rects || count < 0)
		return InvalidParameter;
	
	rectsF = convert_rects (rects, count);
	if (!rectsF)
		return OutOfMemory;

	status = GdipDrawRectangles (graphics, pen, rectsF, count);

	GdipFree (rectsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count)
{
	return GdipDrawClosedCurve2 (graphics, pen, points, count, 0.5f);
}

GpStatus WINGDIPAPI
GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count)
{
	return GdipDrawClosedCurve2I (graphics, pen, points, count, 0.5f);
}

GpStatus WINGDIPAPI
GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count, REAL tension)
{
	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || count < 3)
		return InvalidParameter;

	/* when tension is 0, draw straight lines */
	if (tension == 0)
		return GdipDrawPolygon (graphics, pen, points, count);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawClosedCurve2 (graphics, pen, points, count, tension);
	case GraphicsBackEndMetafile:
		return metafile_DrawClosedCurve2 (graphics, pen, points, count, tension);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count, REAL tension)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawClosedCurve2 (graphics, pen, pointsF, count, tension);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points, INT count)
{
	return GdipDrawCurve2 (graphics, pen, points, count, 0.5f);
}

GpStatus WINGDIPAPI
GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points, INT count)
{
	return GdipDrawCurve2I (graphics, pen, points, count, 0.5f);
}

GpStatus WINGDIPAPI
GdipDrawCurve2 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, INT count, REAL tension)
{
	if (count == 2)
		return GdipDrawLines (graphics, pen, points, count);

	return GdipDrawCurve3 (graphics, pen, points, count, 0, count - 1, tension);
}

GpStatus WINGDIPAPI
GdipDrawCurve2I (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPoint *points, INT count, REAL tension)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawCurve2 (graphics, pen, pointsF, count, tension);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawCurve3 (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPointF *points, INT count, INT offset, INT numOfSegments, REAL tension)
{
	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!pen || count < 2 || offset < 0 || offset >= count)
		return InvalidParameter;
	if (numOfSegments < 1 || numOfSegments >= count - offset)
		return InvalidParameter;

	/* draw lines if tension = 0 */
	if (tension == 0)
		return GdipDrawLines (graphics, pen, points, count);

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_DrawCurve3 (graphics, pen, points, count, offset, numOfSegments, tension);
	case GraphicsBackEndMetafile:
		return metafile_DrawCurve3 (graphics, pen, points, count, offset, numOfSegments, tension);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipDrawCurve3I (GpGraphics *graphics, GpPen* pen, GDIPCONST GpPoint *points, INT count, INT offset, INT numOfSegments, REAL tension)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawCurve3 (graphics, pen, pointsF, count, offset, numOfSegments, tension);

	GdipFree (pointsF);
	return status;
}

/*
 * Fills
 */
GpStatus WINGDIPAPI
GdipFillEllipse (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!brush)
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

GpStatus WINGDIPAPI
GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height)
{
	return GdipFillEllipse (graphics, brush, x, y, width, height);
}

GpStatus WINGDIPAPI
GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height)
{
	RectF rect = {x, y, width, height};
	return GdipFillRectangles (graphics, brush, &rect, 1);
}

GpStatus WINGDIPAPI
GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height)
{
	return GdipFillRectangle (graphics, brush, x, y, width, height);
}

GpStatus WINGDIPAPI
GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRectF *rects, INT count)
{
	if (!graphics || !rects || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!brush)
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

GpStatus WINGDIPAPI
GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpRect *rects, INT count)
{
	GpStatus status;
	GpRectF *rectsF;

	if (count < 0)
		return OutOfMemory;
	if (!rects)
		return InvalidParameter;

	rectsF = convert_rects (rects, count);
	if (!rectsF)
		return OutOfMemory;

	status = GdipFillRectangles (graphics, brush, rectsF, count);

	GdipFree (rectsF);
	return status;
}

GpStatus WINGDIPAPI
GdipFillPie (GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height,
	REAL startAngle, REAL sweepAngle)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!brush || width <= 0 || height <= 0)
		return InvalidParameter;

	// Don't do anything if sweep angle is zero.
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

GpStatus WINGDIPAPI
GdipFillPieI (GpGraphics *graphics, GpBrush *brush, INT x, INT y, INT width, INT height, REAL startAngle, REAL sweepAngle)
{
	return GdipFillPie (graphics, brush, x, y, width, height, startAngle, sweepAngle);
}

GpStatus WINGDIPAPI
GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!brush || !path)
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

GpStatus WINGDIPAPI
GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count, GpFillMode fillMode)
{
	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!brush || fillMode > FillModeWinding)
		return InvalidParameter;

	// Don't do anything if sweep angle is zero.
	if (count < 2)
		return Ok;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillPolygon (graphics, brush, points, count, fillMode);
	case GraphicsBackEndMetafile:
		return metafile_FillPolygon (graphics, brush, points, count, fillMode);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count, GpFillMode fillMode)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipFillPolygon (graphics, brush, pointsF, count, fillMode);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count)
{
	return GdipFillPolygon (graphics, brush, points, count, FillModeAlternate);
}

GpStatus WINGDIPAPI
GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count)
{
	return GdipFillPolygonI (graphics, brush, points, count, FillModeAlternate);
}

GpStatus WINGDIPAPI
GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count)
{
	return GdipFillClosedCurve2 (graphics, brush, points, count, 0.5f, FillModeAlternate);
}

GpStatus WINGDIPAPI
GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count)
{
	return GdipFillClosedCurve2I (graphics, brush, points, count, 0.5f, FillModeAlternate);
}

GpStatus WINGDIPAPI
GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPointF *points, INT count, REAL tension, GpFillMode fillMode)
{
	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!brush || fillMode > FillModeWinding)
		return InvalidParameter;
	
	// Nop if the count is too small.
	if (count < 3)
		return Ok;

	/* when tension is 0, the edges are straight lines */
	if (tension == 0)
		return GdipFillPolygon2 (graphics, brush, points, count);

	if (!graphics || !brush || !points)
		return InvalidParameter;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_FillClosedCurve2 (graphics, brush, points, count, tension, fillMode);
	case GraphicsBackEndMetafile:
		return metafile_FillClosedCurve2 (graphics, brush, points, count, tension, fillMode);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GDIPCONST GpPoint *points, INT count, REAL tension, GpFillMode fillMode)
{
	GpStatus status;
	GpPointF *pointsF;

	if (count < 0)
		return OutOfMemory;
	if (!points)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipFillClosedCurve2 (graphics, brush, pointsF, count, tension, fillMode);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
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

GpStatus WINGDIPAPI
GdipSetRenderingOrigin (GpGraphics *graphics, INT x, INT y)
{
	if (!graphics)
		return InvalidParameter;
	
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipGetRenderingOrigin (GpGraphics *graphics, INT *x, INT *y)
{
	if (!graphics || !x || !y)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*x = graphics->render_origin_x;
	*y = graphics->render_origin_y;
	
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetDpiX (GpGraphics *graphics, REAL *dpi)
{
	if (!graphics || !dpi)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*dpi = graphics->dpi_x;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetDpiY (GpGraphics *graphics, REAL *dpi)
{
	if (!graphics || !dpi)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*dpi = graphics->dpi_y;
	return Ok;
}

GpStatus WINGDIPAPI
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

GpStatus WINGDIPAPI
GdipSetInterpolationMode (GpGraphics *graphics, InterpolationMode interpolationMode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (interpolationMode <= InterpolationModeInvalid || interpolationMode > InterpolationModeHighQualityBicubic)
		return InvalidParameter;

	switch (interpolationMode) {
		case InterpolationModeDefault:
		case InterpolationModeLowQuality:
			graphics->interpolation = InterpolationModeBilinear;
			break;
		case InterpolationModeHighQuality:
			graphics->interpolation = InterpolationModeHighQualityBicubic;
			break;
		default:
			graphics->interpolation = interpolationMode;
			break;
	}

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return Ok;
	case GraphicsBackEndMetafile:
		return metafile_SetInterpolationMode (graphics, interpolationMode);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipGetInterpolationMode (GpGraphics *graphics, InterpolationMode *interpolationMode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!interpolationMode)
		return InvalidParameter;

	*interpolationMode = graphics->interpolation;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetTextRenderingHint (GpGraphics *graphics, TextRenderingHint mode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (mode > TextRenderingHintClearTypeGridFit)
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

GpStatus WINGDIPAPI
GdipGetTextRenderingHint (GpGraphics *graphics, TextRenderingHint *mode)
{
	if (!graphics || !mode)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	
	*mode = graphics->text_mode;
	return Ok;
}

/* MonoTODO - pixel offset mode isn't supported */
GpStatus WINGDIPAPI
GdipSetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode pixelOffsetMode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (pixelOffsetMode <= PixelOffsetModeInvalid || pixelOffsetMode > PixelOffsetModeHalf)
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
GpStatus WINGDIPAPI
GdipGetPixelOffsetMode (GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode)
{
	if (!graphics || !pixelOffsetMode)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	
	*pixelOffsetMode = graphics->pixel_mode;
	return Ok;
}

/* MonoTODO - text contrast isn't supported */
GpStatus WINGDIPAPI
GdipSetTextContrast (GpGraphics *graphics, UINT contrast)
{
	// The gamma correction value must be less than 12.
	// The default value is 4.
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (contrast > 12)
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
GpStatus WINGDIPAPI
GdipGetTextContrast (GpGraphics *graphics, UINT *contrast)
{
	if (!graphics || !contrast)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*contrast = graphics->text_contrast;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetSmoothingMode (GpGraphics *graphics, SmoothingMode smoothingMode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (smoothingMode <= SmoothingModeInvalid || smoothingMode > SmoothingModeAntiAlias + 1)
		return InvalidParameter;

	switch (smoothingMode) {
		case SmoothingModeDefault:
		case SmoothingModeHighSpeed:
			graphics->draw_mode = SmoothingModeNone;
			break;
		case SmoothingModeHighQuality:
			graphics->draw_mode = SmoothingModeAntiAlias;
			break;
		default:
			graphics->draw_mode = smoothingMode;
			break;
	}

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		return cairo_SetSmoothingMode (graphics, smoothingMode);
	case GraphicsBackEndMetafile:
		return metafile_SetSmoothingMode (graphics, smoothingMode);
	default:
		return GenericError;
	}
}

GpStatus WINGDIPAPI
GdipGetSmoothingMode (GpGraphics *graphics, SmoothingMode *smoothingMode)
{
	if (!graphics || !smoothingMode)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*smoothingMode = graphics->draw_mode;
	return Ok;
}

/* MonoTODO - dstrect, srcrect and unit support isn't implemented */
GpStatus WINGDIPAPI
GdipBeginContainer (GpGraphics *graphics, GDIPCONST GpRectF* dstrect, GDIPCONST GpRectF *srcrect, GpUnit unit, GraphicsContainer *state)
{
	if (!graphics || !dstrect || !srcrect || (unit < UnitPixel) || (unit > UnitMillimeter))
		return InvalidParameter;

	return GdipBeginContainer2 (graphics, state);
}

GpStatus WINGDIPAPI
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

GpStatus WINGDIPAPI
GdipBeginContainerI (GpGraphics *graphics, GDIPCONST GpRect* dstrect, GDIPCONST GpRect *srcrect, GpUnit unit, GraphicsContainer *state)
{
	GpRectF dstrectF;
	GpRectF srcrectF;

	if (!dstrect || !srcrect)
		return InvalidParameter;
	
	gdip_RectF_from_Rect (dstrect, &dstrectF);
	gdip_RectF_from_Rect (srcrect, &srcrectF);
	return GdipBeginContainer (graphics, &dstrectF, &srcrectF, unit, state);
}

GpStatus WINGDIPAPI
GdipEndContainer (GpGraphics *graphics, GraphicsContainer state)
{
	if (!graphics)
		return InvalidParameter;

	return GdipRestoreGraphics (graphics, state);
}

GpStatus WINGDIPAPI
GdipFlush (GpGraphics *graphics, GpFlushIntention intention)
{
	cairo_surface_t* surface;

	if (!graphics)
		return InvalidParameter;

	if (graphics->state != GraphicsStateValid)
		return ObjectBusy;

	surface = cairo_get_target (graphics->ct);
	cairo_surface_flush (surface);

#ifdef CAIRO_HAS_QUARTZ_SURFACE
	if (graphics->type == gtOSXDrawable) {
		CGRect rect;
	
		rect.origin.x = 0;
		rect.origin.y = 0;
		rect.size.width = graphics->orig_bounds.Width;
		rect.size.height = graphics->orig_bounds.Height;
		void *image = CGBitmapContextCreateImage (cairo_quartz_surface_get_cg_context (surface));
		CGContextDrawImage (graphics->cg_context, rect, image);
		CGImageRelease (image);
	}
#endif
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetClipGraphics (GpGraphics *graphics, GpGraphics *srcgraphics, CombineMode combineMode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!srcgraphics)
		return InvalidParameter;
	if (srcgraphics->state == GraphicsStateBusy)
		return ObjectBusy;

	return GdipSetClipRegion (graphics, srcgraphics->clip, combineMode);
}

GpStatus WINGDIPAPI
GdipSetClipRect (GpGraphics *graphics, REAL x, REAL y, REAL width, REAL height, CombineMode combineMode)
{
	GpStatus status;
	GpRectF rect;
	GpRegion *region = NULL;

	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (combineMode > CombineModeComplement)
		return InvalidParameter;

	rect.X = x;
	rect.Y = y;
	rect.Width = width;
	rect.Height = height;
	gdip_normalize_rectangle (&rect, &rect);

	// Match GDI+ behaviour by setting empty rects to an empty region.
	if (gdip_is_rectF_empty (&rect, /* allowNegative */ FALSE)) {
		status = GdipCreateRegion (&region);
		if (status != Ok)
			return status;
		
		GdipSetEmpty (region);
	}
	else {		
		status = GdipCreateRegionRect (&rect, &region);
		if (status != Ok)
			return status;
	}

	status = GdipSetClipRegion (graphics, region, combineMode);
	GdipDeleteRegion (region);

	return status;
}

GpStatus WINGDIPAPI
GdipSetClipRectI (GpGraphics *graphics, INT x, INT y, INT width, INT height, CombineMode combineMode)
{
	return GdipSetClipRect (graphics, x, y, width, height, combineMode);
}

GpStatus WINGDIPAPI
GdipSetClipPath (GpGraphics *graphics, GpPath *path, CombineMode combineMode)
{
	GpStatus status;
	GpPath *work;

	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!path || combineMode > CombineModeComplement)
		return InvalidParameter;

	/* if the matrix is empty, avoid path cloning and transform */
	if (gdip_is_matrix_empty (graphics->clip_matrix)) {
		work = path;
	} else {
		cairo_matrix_t inverted;

		gdip_cairo_matrix_copy (&inverted, graphics->clip_matrix);
		cairo_matrix_invert (&inverted);

		status = GdipClonePath (path, &work);
		if (status != Ok)
			return status;
		GdipTransformPath (work, &inverted);
	}

	status = GdipCombineRegionPath (graphics->clip, work, combineMode);	
	if (status != Ok)
		goto cleanup;

	switch (graphics->backend) {
	case GraphicsBackEndCairo:
		/* adjust cairo clipping according to graphics->clip */
		status = cairo_SetGraphicsClip (graphics);
		break;
	case GraphicsBackEndMetafile:
		status = metafile_SetClipPath (graphics, path, combineMode);
		break;
	default:
		status = GenericError;
		break;
	}

cleanup:
	if (work != path)
		GdipDeletePath (work);
	return status;	
}

GpStatus WINGDIPAPI
GdipSetClipRegion (GpGraphics *graphics, GpRegion *region, CombineMode combineMode)
{
	GpStatus status;
	GpRegion *work;

	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!region || combineMode > CombineModeComplement)
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
GpStatus WINGDIPAPI
GdipSetClipHrgn (GpGraphics *graphics, void *hRgn, CombineMode combineMode)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!hRgn)
		return InvalidParameter;
		
	return GdipSetClipRegion (graphics, (GpRegion*)hRgn, combineMode);
}

GpStatus WINGDIPAPI
GdipResetClip (GpGraphics *graphics)
{
	if (!graphics)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipTranslateClip (GpGraphics *graphics, REAL dx, REAL dy)
{
	GpStatus status;

	if (!graphics)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipTranslateClipI (GpGraphics *graphics, INT dx, INT dy)
{
	return GdipTranslateClip (graphics, dx, dy);
}

GpStatus WINGDIPAPI
GdipGetClip (GpGraphics *graphics, GpRegion *region)
{
	if (!graphics || !region)
		return InvalidParameter;
	
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	gdip_clear_region (region);
	gdip_copy_region (graphics->clip, region);

	if (gdip_is_matrix_empty (graphics->clip_matrix))
		return Ok;
	return GdipTransformRegion (region, graphics->clip_matrix);
}

GpStatus WINGDIPAPI
GdipGetClipBounds (GpGraphics *graphics, GpRectF *rect)
{
	GpStatus status;
	BOOL empty;
	GpRegion *work;

	if (!graphics || !rect)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	// The clip bounds for empty bounds should be translated.
	GdipIsEmptyRegion (graphics->clip, graphics, &empty);
	if (empty) {
		status = GdipGetRegionBounds (graphics->clip, graphics, rect);
		if (status != Ok)
			return status;

		rect->X += graphics->clip_matrix->x0;
		rect->Y += graphics->clip_matrix->y0;

		return Ok;
	}

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

GpStatus WINGDIPAPI
GdipGetClipBoundsI (GpGraphics *graphics, GpRect *rect)
{
	GpRectF rectF;
	GpStatus status;

	if (!rect)
		return InvalidParameter;

	status = GdipGetClipBounds (graphics, &rectF);
	if (status != Ok)
		return status;

	gdip_Rect_from_RectF (&rectF, rect);
	return Ok;
}

GpStatus WINGDIPAPI
GdipIsClipEmpty (GpGraphics *graphics, BOOL *result)
{
	if (!graphics || !result)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	return GdipIsEmptyRegion (graphics->clip, graphics, result);
}

GpStatus WINGDIPAPI
GdipSetVisibleClip_linux (GpGraphics *graphics, GpRect *rect)
{
	if (!graphics || !rect)
		return InvalidParameter;

	graphics->orig_bounds.X = rect->X;
	graphics->orig_bounds.Y = rect->Y;
	graphics->orig_bounds.Width = rect->Width;
	graphics->orig_bounds.Height = rect->Height;
	graphics->bounds.X = rect->X;
	graphics->bounds.Y = rect->Y;
	graphics->bounds.Width = rect->Width;
	graphics->bounds.Height = rect->Height;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetVisibleClipBounds (GpGraphics *graphics, GpRectF *rect)
{
	if (!graphics || !rect)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipGetVisibleClipBoundsI (GpGraphics *graphics, GpRect *rect)
{
	GpStatus status;
	GpRectF rectF;

	if (!graphics || !rect)
		return InvalidParameter;

	status = GdipGetVisibleClipBounds (graphics, &rectF);
	if (status != Ok)
		return status;

	gdip_Rect_from_RectF (&rectF, rect);
	return Ok;
}

GpStatus WINGDIPAPI
GdipIsVisibleClipEmpty (GpGraphics *graphics, BOOL *result)
{
	GpRectF visibleClipBounds;

	if (!graphics || !result)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	
	GdipGetVisibleClipBounds (graphics, &visibleClipBounds);

	*result = visibleClipBounds.Width == 0 || visibleClipBounds.Height == 0;
	return Ok;
}

GpStatus WINGDIPAPI
GdipIsVisiblePoint (GpGraphics *graphics, REAL x, REAL y, BOOL *result)
{
	GpRectF rectF;

	if (!graphics || !result)
		return InvalidParameter;

	gdip_RectF_from_Rect (&graphics->bounds, &rectF);
	*result = gdip_is_Point_in_RectF_inclusive (x, y, &rectF);
	return Ok;
}

GpStatus WINGDIPAPI
GdipIsVisiblePointI (GpGraphics *graphics, INT x, INT y, BOOL *result)
{
	return GdipIsVisiblePoint (graphics, x, y, result);
}

GpStatus WINGDIPAPI
GdipIsVisibleRect (GpGraphics *graphics, REAL x, REAL y, REAL width, REAL height, BOOL *result)
{
	BOOL found = FALSE;
	float posy, posx;
	GpRectF recthit, boundsF;

	if (!graphics || !result)
		return InvalidParameter;

	if (width == 0 || height == 0) {
		*result = FALSE;
		return Ok;
	}
	
	gdip_RectF_from_Rect (&graphics->bounds, &boundsF);
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

GpStatus WINGDIPAPI
GdipIsVisibleRectI (GpGraphics *graphics, INT x, INT y, INT width, INT height, BOOL *result)
{
	return GdipIsVisibleRect (graphics, x, y, width, height, result);
}

GpStatus WINGDIPAPI
GdipSetCompositingMode (GpGraphics *graphics, CompositingMode compositingMode)
{
	if (!graphics)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipGetCompositingMode (GpGraphics *graphics, CompositingMode *compositingMode)
{
	if (!graphics || !compositingMode)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*compositingMode = graphics->composite_mode;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetCompositingQuality (GpGraphics *graphics, CompositingQuality compositingQuality)
{
	if (!graphics)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

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

GpStatus WINGDIPAPI
GdipGetCompositingQuality (GpGraphics *graphics, CompositingQuality *compositingQuality)
{
	if (!graphics || !compositingQuality)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*compositingQuality = graphics->composite_quality;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetNearestColor (GpGraphics *graphics, ARGB *argb)
{
	return Ok;
}

GpStatus WINGDIPAPI
GdipResetPageTransform (GpGraphics *graphics)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipSetPageScale (GpGraphics *graphics, REAL scale)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (scale <= 0.0 || scale > 1000000032)
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

GpStatus WINGDIPAPI
GdipGetPageScale (GpGraphics *graphics, REAL *scale)
{
	if (!graphics | !scale)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*scale = graphics->scale;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetPageUnit (GpGraphics *graphics, GpUnit unit)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (unit <= UnitWorld || unit > UnitCairoPoint)
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

GpStatus WINGDIPAPI
GdipGetPageUnit (GpGraphics *graphics, GpUnit *unit)
{
	if (!graphics || !unit)
		return InvalidParameter;

	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;

	*unit = graphics->page_unit;
	return Ok;
}

GpStatus WINGDIPAPI
GdipTransformPoints (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPointF *points, INT count)
{
	static int called = 0;

	if (!called) {
		printf("NOT IMPLEMENTED YET:GdipTransformPoints (GpGraphics *graphics, GpCoordinateSpace destSpace %d, GpCoordinateSpace srcSpace %d, GpPointF *points, int count %d)\n", destSpace, srcSpace, count);
	}
	/* return NotImplemented; */
	return Ok;
}

GpStatus WINGDIPAPI
GdipTransformPointsI (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPoint *points, INT count)
{
	static int called = 0;

	if (!called) {
		printf("NOT IMPLEMENTED YET:GdipTransformPointsI (GpGraphics *graphics, GpCoordinateSpace destSpace, GpCoordinateSpace srcSpace, GpPoint *points, int count)\n");
	}
	/* return NotImplemented; */
	return Ok;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileDestPoint (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST PointF *destPoint,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileDestPointI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoint,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileDestRect (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST RectF *destRect,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileDestRectI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Rect *destRect,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes )
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileDestPoints (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST PointF *destPoints, INT count,
    EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileDestPointsI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoints, INT count,
	EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes )
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileSrcRectDestPoint (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST PointF *destPoint, GDIPCONST RectF *srcRect,
    Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes * imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileSrcRectDestPointI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoint, GDIPCONST Rect *srcRect,
    Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileSrcRectDestRect (GpGraphics * graphics, GDIPCONST GpMetafile *metafile, GDIPCONST RectF *destRect, GDIPCONST RectF *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileSrcRectDestRectI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Rect *destRect, GDIPCONST Rect *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileSrcRectDestPoints (GpGraphics *graphics, GDIPCONST GpMetafile * metafile, GDIPCONST PointF *destPoints, INT count, GDIPCONST RectF *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipEnumerateMetafileSrcRectDestPointsI (GpGraphics *graphics, GDIPCONST GpMetafile *metafile, GDIPCONST Point *destPoints, INT count, GDIPCONST Rect *srcRect,
	Unit srcUnit, EnumerateMetafileProc callback, VOID *callbackData, GDIPCONST GpImageAttributes *imageAttributes)
{
	return NotImplemented;
}

HPALETTE WINGDIPAPI
GdipCreateHalftonePalette()
{
	return NULL;
}
