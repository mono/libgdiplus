/*
 * Copyright © 2008 Chris Wilson
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Chris Wilson not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Chris Wilson makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * CHRIS WILSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL CHRIS WILSON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairo-test.h"
#include <cairo-glitz.h>
#include <cairo-xlib.h>
#include <cairo-xlib-xrender.h>

#define NAME "glitz"
#include "surface-source.c"

static cairo_user_data_key_t closure_key;

#if CAIRO_CAN_TEST_GLITZ_GLX_SURFACE
#include <glitz-glx.h>

struct closure {
    Display        *dpy;
    Window          win;
};

static void
cleanup (void *data)
{
    struct closure *closure = data;

    glitz_glx_fini ();

    if (closure->win)
	XDestroyWindow (closure->dpy, closure->win);

    XCloseDisplay (closure->dpy);

    free (closure);
}

static glitz_surface_t *
_glitz_glx_create_surface (glitz_format_name_t		  formatname,
			   int				  width,
			   int				  height,
			   struct closure		*closure)
{
    Display                 * dpy = closure->dpy;
    int			      scr = DefaultScreen(dpy);
    glitz_drawable_format_t   templ;
    glitz_drawable_format_t * dformat = NULL;
    unsigned long             mask;
    glitz_drawable_t        * drawable = NULL;
    glitz_format_t          * format;
    glitz_surface_t         * sr;

    XSizeHints                xsh;
    XSetWindowAttributes      xswa;
    XVisualInfo             * vinfo;

    memset(&templ, 0, sizeof(templ));
    templ.color.red_size = 8;
    templ.color.green_size = 8;
    templ.color.blue_size = 8;
    templ.color.alpha_size = 8;
    templ.color.fourcc = GLITZ_FOURCC_RGB;
    templ.samples = 1;

    glitz_glx_init (NULL);

    mask = GLITZ_FORMAT_SAMPLES_MASK |
	   GLITZ_FORMAT_FOURCC_MASK |
	   GLITZ_FORMAT_RED_SIZE_MASK |
	   GLITZ_FORMAT_GREEN_SIZE_MASK |
	   GLITZ_FORMAT_BLUE_SIZE_MASK;
    if (formatname == GLITZ_STANDARD_ARGB32)
	mask |= GLITZ_FORMAT_ALPHA_SIZE_MASK;

    /* Try for a pbuffer first */
    if (!getenv("CAIRO_TEST_FORCE_GLITZ_WINDOW"))
	dformat = glitz_glx_find_pbuffer_format (dpy, scr, mask, &templ, 0);

    if (dformat) {
	closure->win = None;

	drawable = glitz_glx_create_pbuffer_drawable (dpy, scr, dformat,
						      width, height);
	if (!drawable)
	    goto FAIL;
    } else {
	/* No pbuffer, try window */
	dformat = glitz_glx_find_window_format (dpy, scr, mask, &templ, 0);

	if (!dformat)
	    goto FAIL;

	vinfo = glitz_glx_get_visual_info_from_format(dpy,
						      DefaultScreen(dpy),
						      dformat);

	if (!vinfo)
	    goto FAIL;

	xsh.flags = PSize;
	xsh.x = 0;
	xsh.y = 0;
	xsh.width = width;
	xsh.height = height;

	xswa.colormap = XCreateColormap (dpy, RootWindow(dpy, scr),
					 vinfo->visual, AllocNone);
	closure->win = XCreateWindow (dpy, RootWindow(dpy, scr),
				      xsh.x, xsh.y, xsh.width, xsh.height,
				      0, vinfo->depth, CopyFromParent,
				      vinfo->visual, CWColormap, &xswa);
	XFree (vinfo);

	drawable =
	    glitz_glx_create_drawable_for_window (dpy, scr,
						  dformat, closure->win,
						  width, height);

	if (!drawable)
	    goto DESTROY_WINDOW;
    }

    format = glitz_find_standard_format (drawable, formatname);
    if (!format)
	goto DESTROY_DRAWABLE;

    sr = glitz_surface_create (drawable, format, width, height, 0, NULL);
    if (!sr)
	goto DESTROY_DRAWABLE;

    if (closure->win == None || dformat->doublebuffer) {
	glitz_surface_attach (sr, drawable, GLITZ_DRAWABLE_BUFFER_BACK_COLOR);
    } else {
	XMapWindow (closure->dpy, closure->win);
	glitz_surface_attach (sr, drawable, GLITZ_DRAWABLE_BUFFER_FRONT_COLOR);
    }

    glitz_drawable_destroy (drawable);

    return sr;
 DESTROY_DRAWABLE:
    glitz_drawable_destroy (drawable);
 DESTROY_WINDOW:
    if (closure->win)
	XDestroyWindow (dpy, closure->win);
 FAIL:
    return NULL;
}

static cairo_surface_t *
create_source_surface (int size)
{
    struct closure *closure;
    glitz_surface_t  *glitz_surface;
    cairo_surface_t *surface;

    closure = xcalloc (1, sizeof (struct closure));

    closure->dpy = XOpenDisplay (getenv("CAIRO_TEST_GLITZ_DISPLAY"));
    if (closure->dpy == NULL) {
	free (closure);
	return NULL;
    }

    glitz_surface = _glitz_glx_create_surface (GLITZ_STANDARD_ARGB32,
					       size, size,
					       closure);
    if (glitz_surface == NULL) {
	XCloseDisplay (closure->dpy);
	free (closure);
	return NULL;
    }

    surface = cairo_glitz_surface_create (glitz_surface);

    cairo_surface_set_user_data (surface, &closure_key, closure, cleanup);

    return surface;
}
#endif
