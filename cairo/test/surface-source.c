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

static cairo_test_draw_function_t draw;
static cairo_surface_t *create_source_surface (int size);

#define SIZE 90

cairo_test_t test = {
    NAME "-surface-source",
    "Test using various surfaces as the source",
    SIZE, SIZE,
    draw
};

static void
draw_pattern (cairo_surface_t *surface, int surface_size)
{
    cairo_t *cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_rectangle (cr,
		     0, 0,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr);
    cairo_set_source_rgb (cr, 1, 0, 0);
    cairo_rectangle (cr,
		     surface_size / 2, 0,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr);
    cairo_set_source_rgb (cr, 0, 1, 0);
    cairo_rectangle (cr,
		     0, surface_size / 2,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr);
    cairo_set_source_rgb (cr, 0, 0, 1);
    cairo_rectangle (cr,
		     surface_size / 2, surface_size / 2,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr);

    cairo_destroy (cr);
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    int surface_size;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_paint (cr);

    surface_size = SIZE - 30;
    surface = create_source_surface (surface_size);
    draw_pattern (surface, surface_size);

    cairo_set_source_surface (cr, surface, 15, 15);
    cairo_surface_destroy (surface);
    cairo_paint (cr);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    cairo_surface_t *surface;

    surface = create_source_surface (SIZE);
    if (surface == NULL) /* can't create the source so skip the test */
	return CAIRO_TEST_SUCCESS;

    cairo_surface_destroy (surface);

    return cairo_test (&test);
}
