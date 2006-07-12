/*
 * Copyright © 2006 Red Hat, Inc.
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

#include <stdio.h>
#include <cairo.h>

#include <cairo-pdf.h>
#include <cairo-pdf-test.h>

#include <cairo-ps.h>
#include <cairo-ps-test.h>

#include <cairo-svg.h>
#include <cairo-svg-test.h>

#include "cairo-test.h"

/* This test exists to test cairo_surface_set_fallback_resolution
 */

#define INCHES_TO_POINTS(in) ((in) * 72.0)
#define SIZE INCHES_TO_POINTS(1)

static void
draw (cairo_t *cr, double width, double height, double ppi)
{
    char message[80];

    cairo_save (cr);

    cairo_new_path (cr);

    cairo_set_line_width (cr, .05 * SIZE / 2.0);
    cairo_arc (cr, SIZE / 2.0, SIZE / 2.0,
	       0.75 * SIZE / 2.0,
	       0, 2.0 * M_PI);
    cairo_stroke (cr);

    cairo_arc (cr, SIZE / 2.0, SIZE / 2.0,
	       0.6 * SIZE / 2.0,
	       0, 2.0 * M_PI);
    cairo_fill (cr);

    cairo_move_to (cr, .4 * SIZE/2.0, SIZE/2.0);
    sprintf (message, "Fallback PPI: %g", ppi);
    cairo_set_source_rgb (cr, 1, 1, 1); /* white */
    cairo_set_font_size (cr, .1 * SIZE / 2.0);
    cairo_show_text (cr, message);

    cairo_restore (cr);
}

#define NUM_BACKENDS 3
typedef enum {
    PDF, PS, SVG
} backend_t;
static const char *backend_filename[NUM_BACKENDS] = {
    "fallback-resolution.pdf",
    "fallback-resolution.ps",
    "fallback-resolution.svg"
};

int
main (void)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_status_t status;
    double ppi[] = { 600., 300., 150., 75., 37.5 };
    backend_t backend;
    int page, num_pages;

    num_pages = sizeof (ppi) / sizeof (ppi[0]);

    printf("\n");

    for (backend=0; backend < NUM_BACKENDS; backend++) {

	/* Create backend-specific surface and force image fallbacks. */
	switch (backend) {
	case PDF:
	    surface = cairo_pdf_surface_create (backend_filename[backend],
						SIZE, SIZE);
	    _cairo_pdf_test_force_fallbacks ();
	    break;
	case PS:
	    surface = cairo_ps_surface_create (backend_filename[backend],
					       SIZE, SIZE);
	    _cairo_ps_test_force_fallbacks ();
	    break;
	case SVG:
	    surface = cairo_svg_surface_create (backend_filename[backend],
						SIZE, SIZE * num_pages);
	    _cairo_svg_test_force_fallbacks ();
	    break;
	}

	cr = cairo_create (surface);

	for (page = 0; page < num_pages; page++)
	{
	    cairo_surface_set_fallback_resolution (surface, ppi[page], ppi[page]);

	    draw (cr, SIZE, SIZE, ppi[page]);

	    /* Backend-specific means of "advancing a page" */
	    switch (backend) {
	    case PDF:
	    case PS:
		cairo_show_page (cr);
		break;
	    case SVG:
		/* Since the SVG backend doesn't natively support multiple
		 * pages, we just move further down for each logical
		 * page, then finally do a show_page at the end. */
		if (page < num_pages - 1) {
		    cairo_translate (cr, 0, SIZE);
		} else {
		    /* XXX: The goal of this test is to show the
		     * effect of several different fallback
		     * resolutions in a single output document. But
		     * since fallback_resolution only takes effect at
		     * the time of show_page, we only get once for the
		     * SVG backend. I'm just re-setting the first one
		     * here so we actually get legible output.
		     *
		     * To fix this properly we'll need some sort of
		     * multi-page support in the SVG backend I think.
		     */
		    cairo_surface_set_fallback_resolution (surface, ppi[0], ppi[0]);
		    cairo_show_page (cr);
		}
		break;
	    }
	}

	status = cairo_status (cr);

	cairo_destroy (cr);
	cairo_surface_destroy (surface);

	if (status) {
	    cairo_test_log ("Failed to create pdf surface for file %s: %s\n",
			    backend_filename[backend],
			    cairo_status_to_string (status));
	    return CAIRO_TEST_FAILURE;
	}

	printf ("fallback-resolution: Please check %s to ensure it looks correct.\n",
		backend_filename[backend]);
    }

    return CAIRO_TEST_SUCCESS;
}
