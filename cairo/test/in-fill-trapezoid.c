/*
 * Copyright © 2008 Chris Wilson
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
 * Author: Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairo-test.h"

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "in-fill-trapezoid",
    "Test _cairo_trap_contains via cairo_in_fill",
    0, 0,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_test_status_t ret = CAIRO_TEST_SUCCESS;

    cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);

    /* simple rectangle */
    cairo_new_path (cr);
    cairo_rectangle (cr, -10, -10, 20, 20);
    if (! cairo_in_fill (cr, 0, 0)) {
	cairo_test_log ("Error: Failed to find point inside rectangle\n");
	ret = CAIRO_TEST_FAILURE;
    }

    /* simple circle */
    cairo_new_path (cr);
    cairo_arc (cr, 0, 0, 10, 0, 2 * M_PI);
    if (! cairo_in_fill (cr, 0, 0)) {
	cairo_test_log ("Error: Failed to find point inside circle\n");
	ret = CAIRO_TEST_FAILURE;
    }

    /* holey rectangle */
    cairo_new_path (cr);
    cairo_rectangle (cr, -10, -10, 20, 20);
    cairo_rectangle (cr, -5, -5, 10, 10);
    if (cairo_in_fill (cr, 0, 0)) {
	cairo_test_log ("Error: Found an unexpected point inside rectangular hole\n");
	ret = CAIRO_TEST_FAILURE;
    }

    /* holey circle */
    cairo_new_path (cr);
    cairo_arc (cr, 0, 0, 10, 0, 2 * M_PI);
    cairo_arc (cr, 0, 0, 5, 0, 2 * M_PI);
    if (cairo_in_fill (cr, 0, 0)) {
	cairo_test_log ("Error: Found an unexpected point inside circular hole\n");
	ret = CAIRO_TEST_FAILURE;
    }

    return ret;
}

int
main (void)
{
    return cairo_test (&test);
}
