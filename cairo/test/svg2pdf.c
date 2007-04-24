/*
 * Copyright © 2005 Red Hat, Inc.
 * Copyright © 2006 Red Hat, Inc.
 * Copyright © 2007 Red Hat, Inc.
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
 * Authors: Kristian Høgsberg <krh@redhat.com>
 *	    Carl Worth <cworth@redhat.com>
 *	    Behdad Esfahbod <besfahbo@redhat.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

#include <cairo-pdf.h>

#define FAIL(msg)							\
    do { fprintf (stderr, "FAIL: %s\n", msg); exit (-1); } while (0)

#define PIXELS_PER_POINT 1

int main (int argc, char *argv[])
{
    GError *error = NULL;
    RsvgHandle *handle;
    RsvgDimensionData dim;
    double width, height;
    const char *filename = argv[1];
    const char *output_filename = argv[2];
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_status_t status;

    if (argc != 3)
	FAIL ("usage: svg2pdf input_file.svg output_file.pdf");

    g_type_init ();

    rsvg_set_default_dpi (72.0);
    handle = rsvg_handle_new_from_file (filename, &error);
    if (error != NULL)
	FAIL (error->message);

    rsvg_handle_get_dimensions (handle, &dim);
    width = dim.width;
    height = dim.height;

    surface = cairo_pdf_surface_create (output_filename, width, height);
    cr = cairo_create (surface);

    /* Clear background */
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* white */
    cairo_paint (cr);

    rsvg_handle_render_cairo (handle, cr);

    status = cairo_status (cr);
    if (status)
	FAIL (cairo_status_to_string (status));

    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    return 0;
}
