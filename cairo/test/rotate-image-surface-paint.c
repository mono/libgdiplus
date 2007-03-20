/*
 * Copyright © 2005, 2007 Red Hat, Inc.
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

static cairo_test_draw_function_t draw;

#define SIZE 20
#define PAD 2

cairo_test_t test = {
    "rotate-image-surface-paint",
    "Test call sequence: image_surface_create; rotate; set_source_surface; paint"
    "\nThis test is known to fail on the ps backend currently",
    SIZE, SIZE,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    int surface_size = sqrt ((SIZE - 2*PAD)*(SIZE - 2*PAD)/2);
    cairo_t *cr_surface;

    /* Create an image surface with my favorite four colors in each
     * quadrant. */
    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
					  surface_size, surface_size);
    cr_surface = cairo_create (surface);
    cairo_set_source_rgb (cr_surface, 1, 1, 1);
    cairo_rectangle (cr_surface,
		     0, 0,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_set_source_rgb (cr_surface, 1, 0, 0);
    cairo_rectangle (cr_surface,
		     surface_size / 2, 0,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_set_source_rgb (cr_surface, 0, 1, 0);
    cairo_rectangle (cr_surface,
		     0, surface_size / 2,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_set_source_rgb (cr_surface, 0, 0, 1);
    cairo_rectangle (cr_surface,
		     surface_size / 2, surface_size / 2,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_destroy (cr_surface);

    /* First paint opaque background (black) so we don't need separate
     * ARGB32 and RGB24 reference images. */
    cairo_set_source_rgb (cr, 0, 0, 0); /* black */
    cairo_paint (cr);

    cairo_translate(cr, SIZE/2, SIZE/2);
    cairo_rotate (cr, M_PI / 4.0);
    cairo_translate (cr, -surface_size/2, -surface_size/2);

    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_pattern_set_filter (cairo_get_source (cr), CAIRO_FILTER_NEAREST);
    cairo_paint (cr);

    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
