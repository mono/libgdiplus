/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 8; -*- */
/*
 * Copyright © 2004,2006 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Red Hat, Inc. not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Red Hat, Inc. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * RED HAT, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carl D. Worth <cworth@cworth.org>
 */

#include "cairo-boilerplate.h"
#include "cairo-boilerplate-xlib.h"
#include "cairo-boilerplate-xlib-private.h"

#include <cairo-xlib-xrender.h>
#include <cairo-xlib-surface-private.h>

typedef struct _xlib_target_closure
{
    Display *dpy;
    Drawable drawable;
    cairo_bool_t drawable_is_pixmap;
} xlib_target_closure_t;

void
_cairo_boilerplate_xlib_synchronize (void *closure)
{
    xlib_target_closure_t *xtc = closure;
    XImage *ximage;

    ximage = XGetImage (xtc->dpy, xtc->drawable,
			0, 0, 1, 1, AllPlanes, ZPixmap);
    if (ximage != NULL)
	XDestroyImage (ximage);
}

/* For the xlib backend we distinguish between TEST and PERF mode in a
 * couple of ways.
 *
 * For TEST, we always test against pixmaps of depth 32 (for
 * COLOR_ALPHA) or 24 (for COLOR) and we use XSynchronize to make it
 * easier to debug problems.
 *
 * For PERF, we test against 32-bit pixmaps for COLOR_ALPHA, but for
 * COLOR we test against _windows_ at the depth of the default visual.
 * For obvious reasons, we don't use XSynchronize.
 */
static cairo_surface_t *
_cairo_boilerplate_xlib_test_create_surface (Display			*dpy,
					     cairo_content_t		 content,
					     int			 width,
					     int			 height,
					     xlib_target_closure_t	*xtc)
{
    XRenderPictFormat *xrender_format;

    /* This kills performance, but it makes debugging much
     * easier. That's why we have it here when in TEST mode, but not
     * over in PERF mode. */
    XSynchronize (xtc->dpy, 1);

    /* XXX: Currently we don't do any xlib testing when the X server
     * doesn't have the Render extension. We could do better here,
     * (perhaps by converting the tests from ARGB32 to RGB24). One
     * step better would be to always test the non-Render fallbacks
     * for each test even if the server does have the Render
     * extension. That would probably be through another
     * cairo_boilerplate_target which would use an extended version of
     * cairo_test_xlib_disable_render.  */
    switch (content) {
    case CAIRO_CONTENT_COLOR_ALPHA:
	xrender_format = XRenderFindStandardFormat (dpy, PictStandardARGB32);
	break;
    case CAIRO_CONTENT_COLOR:
	xrender_format = XRenderFindStandardFormat (dpy, PictStandardRGB24);
	break;
    case CAIRO_CONTENT_ALPHA:
    default:
	CAIRO_BOILERPLATE_LOG ("Invalid content for xlib test: %d\n", content);
	return NULL;
    }
    if (xrender_format == NULL) {
	CAIRO_BOILERPLATE_LOG ("X server does not have the Render extension.\n");
	return NULL;
    }

    xtc->drawable = XCreatePixmap (dpy, DefaultRootWindow (dpy),
				   width, height, xrender_format->depth);
    xtc->drawable_is_pixmap = TRUE;

    return cairo_xlib_surface_create_with_xrender_format (dpy, xtc->drawable,
							  DefaultScreenOfDisplay (dpy),
							  xrender_format,
							  width, height);
}

static cairo_surface_t *
_cairo_boilerplate_xlib_perf_create_surface (Display			*dpy,
					     cairo_content_t		 content,
					     int			 width,
					     int			 height,
					     xlib_target_closure_t	*xtc)
{
    XSetWindowAttributes attr;
    XRenderPictFormat *xrender_format;
    Visual *visual;

    switch (content) {
    case CAIRO_CONTENT_COLOR_ALPHA:
	xrender_format = XRenderFindStandardFormat (dpy, PictStandardARGB32);
	xtc->drawable = XCreatePixmap (dpy, DefaultRootWindow (dpy),
				       width, height, xrender_format->depth);
	xtc->drawable_is_pixmap = TRUE;
	break;
    case CAIRO_CONTENT_COLOR:
	visual = DefaultVisual (dpy, DefaultScreen (dpy));
	xrender_format = XRenderFindVisualFormat (dpy, visual);
	attr.override_redirect = True;
	xtc->drawable = XCreateWindow (dpy, DefaultRootWindow (dpy), 0, 0,
				       width, height, 0, xrender_format->depth,
				       InputOutput, visual, CWOverrideRedirect, &attr);
	XMapWindow (dpy, xtc->drawable);
	xtc->drawable_is_pixmap = FALSE;
	break;
    case CAIRO_CONTENT_ALPHA:
    default:
	CAIRO_BOILERPLATE_LOG ("Invalid content for xlib test: %d\n", content);
	return NULL;
    }
    if (xrender_format == NULL) {
	CAIRO_BOILERPLATE_LOG ("X server does not have the Render extension.\n");
	return NULL;
    }

    return cairo_xlib_surface_create_with_xrender_format (dpy, xtc->drawable,
							  DefaultScreenOfDisplay (dpy),
							  xrender_format,
							  width, height);
}

cairo_surface_t *
_cairo_boilerplate_xlib_create_surface (const char			 *name,
					cairo_content_t			  content,
					int				  width,
					int				  height,
					cairo_boilerplate_mode_t	  mode,
					void				**closure)
{
    xlib_target_closure_t *xtc;
    Display *dpy;

    *closure = xtc = xmalloc (sizeof (xlib_target_closure_t));

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

    xtc->dpy = dpy = XOpenDisplay (NULL);
    if (xtc->dpy == NULL) {
	CAIRO_BOILERPLATE_LOG ("Failed to open display: %s\n", XDisplayName(0));
	return NULL;
    }

    if (mode == CAIRO_BOILERPLATE_MODE_TEST)
	return _cairo_boilerplate_xlib_test_create_surface (dpy, content, width, height, xtc);
    else /* mode == CAIRO_BOILERPLATE_MODE_PERF */
	return _cairo_boilerplate_xlib_perf_create_surface (dpy, content, width, height, xtc);
}

void
_cairo_boilerplate_xlib_cleanup (void *closure)
{
    xlib_target_closure_t *xtc = closure;

    if (xtc->drawable_is_pixmap)
	XFreePixmap (xtc->dpy, xtc->drawable);
    else
	XDestroyWindow (xtc->dpy, xtc->drawable);
    XCloseDisplay (xtc->dpy);
    free (xtc);
}

cairo_status_t
cairo_boilerplate_xlib_surface_disable_render (cairo_surface_t *abstract_surface)
{
    cairo_xlib_surface_t *surface = (cairo_xlib_surface_t*) abstract_surface;

    if (cairo_surface_get_type (abstract_surface) != CAIRO_SURFACE_TYPE_XLIB)
	return CAIRO_STATUS_SURFACE_TYPE_MISMATCH;

    surface->render_major = surface->render_minor = -1;

    return CAIRO_STATUS_SUCCESS;
}
