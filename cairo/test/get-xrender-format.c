/*
 * Copyright © 2008 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: Carl D. Worth <cworth@cworth.org>
 */

#include "cairo-test.h"

#include "cairo-xlib.h"
#include "cairo-xlib-xrender.h"

#include "cairo-boilerplate-xlib.h"

int
main (void)
{
    Display *dpy;
    XRenderPictFormat *orig_format, *format;
    cairo_surface_t *surface;
    Pixmap pixmap;
    int screen;

    cairo_test_init ("get-xrender-format");

    dpy = XOpenDisplay (NULL);
    if (! dpy) {
	cairo_test_log ("Error: Cannot open display: %s.\n",
			XDisplayName (NULL));
	cairo_test_fini ();
	return CAIRO_TEST_UNTESTED;
    }

    screen = DefaultScreen (dpy);

    cairo_test_log ("Testing with image surface.\n");

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);

    format = cairo_xlib_surface_get_xrender_format (surface);
    if (format != NULL) {
	cairo_test_log ("Error: expected NULL for image surface\n");
	return CAIRO_TEST_FAILURE;
    }

    cairo_surface_destroy (surface);

    cairo_test_log ("Testing with non-xrender xlib surface.\n");

    pixmap = XCreatePixmap (dpy, DefaultRootWindow (dpy),
			    1, 1, DefaultDepth (dpy, screen));
    surface = cairo_xlib_surface_create (dpy, pixmap,
					 DefaultVisual (dpy, screen),
					 1, 1);
    orig_format = XRenderFindVisualFormat (dpy, DefaultVisual (dpy, screen));
    format = cairo_xlib_surface_get_xrender_format (surface);
    if (format != orig_format) {
	cairo_test_log ("Error: did not receive the same format as XRenderFindVisualFormat\n");
	return CAIRO_TEST_FAILURE;
    }
    cairo_surface_destroy (surface);
    XFreePixmap (dpy, pixmap);

    cairo_test_log ("Testing with xlib xrender surface.\n");

    orig_format = XRenderFindStandardFormat (dpy, PictStandardARGB32);
    pixmap = XCreatePixmap (dpy, DefaultRootWindow (dpy),
			    1, 1, 32);
    surface = cairo_xlib_surface_create_with_xrender_format (dpy,
							     pixmap,
							     DefaultScreenOfDisplay (dpy),
							     orig_format,
							     1, 1);
    format = cairo_xlib_surface_get_xrender_format (surface);
    if (format != orig_format) {
	cairo_test_log ("Error: did not receive the same format originally set\n");
	return CAIRO_TEST_FAILURE;
    }

    cairo_test_log ("Testing without the X Render extension.\n");

    cairo_boilerplate_xlib_surface_disable_render (surface);

    format = cairo_xlib_surface_get_xrender_format (surface);
    if (format != NULL) {
	cairo_test_log ("Error: did not receive a NULL format as expected\n");
	return CAIRO_TEST_FAILURE;
    }

    cairo_surface_destroy (surface);

    XCloseDisplay (dpy);

    cairo_debug_reset_static_data ();

    cairo_test_fini ();

    return CAIRO_TEST_SUCCESS;
}
