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

#define NUM_GRADIENTS 4
#define NUM_EXTEND 4
#define SIZE 60
#define WIDTH (SIZE * NUM_GRADIENTS)
#define HEIGHT (SIZE * NUM_EXTEND)

cairo_test_t test = {
    "radial-gradient",
    "Simple test of radial gradients",
    WIDTH, HEIGHT,
    draw
};

static void
draw_gradient (cairo_t 		*cr,
	       int		x,
	       int		y,
	       int		size,
	       double		offset,
	       double		inner_radius,
	       cairo_extend_t	extend)
{
    cairo_pattern_t *pattern;

    cairo_save (cr);

    pattern = cairo_pattern_create_radial (x + size/2.0 + offset,
					   y + size/2.0 + offset, inner_radius,
					   x + size/2.0,
					   y + size/2.0, size/3.0);
    cairo_pattern_add_color_stop_rgba (pattern, 0.0,
				       1.0, 0.0, 0.0, 1.0);
    cairo_pattern_add_color_stop_rgba (pattern, sqrt (1.0 / 2.0),
				       0.0, 1.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba (pattern, 1.0,
				       0.0, 0.0, 1.0, 0.5);
    cairo_pattern_set_extend (pattern, extend);

    cairo_rectangle (cr, x, y, size, size);
    cairo_clip (cr);

    cairo_set_source (cr, pattern);
    cairo_paint (cr);

    cairo_pattern_destroy (pattern);

    cairo_restore (cr);
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    int i, j;
    double inner_radius, offset;
    cairo_extend_t extend[NUM_EXTEND] = {
	CAIRO_EXTEND_NONE,
	CAIRO_EXTEND_REPEAT,
	CAIRO_EXTEND_REFLECT,
	CAIRO_EXTEND_PAD
    };

    cairo_test_paint_checkered (cr);

    for (j = 0; j < NUM_EXTEND; j++) {
	for (i = 0; i < NUM_GRADIENTS; i++) {
	    offset = i % 2 ? SIZE / 12.0 : 0.0;
	    inner_radius = i >= NUM_EXTEND / 2 ? SIZE / 6.0 : 0.0;
	    draw_gradient (cr, i * SIZE, j * SIZE, SIZE,
			   offset, inner_radius, extend[j]);
	}
    }

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
