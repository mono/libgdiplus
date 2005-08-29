/*
 * Copyright © 2005 Red Hat, Inc.
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

#include "cairo-test.h"

cairo_test_t test = {
    "surface-pattern",
    "Tests use of a surface pattern",
    36, 36
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_t *cr2;
    cairo_pattern_t *pattern;
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					  6, 6);

    cr2 = cairo_create (surface);
    {
	cairo_rectangle (cr2, 0, 0, 3, 3);
	cairo_rectangle (cr2, 3, 3, 3, 3);
	cairo_set_source_rgb (cr2, 1, 1, 0);
	cairo_fill (cr2);
    
	cairo_rectangle (cr2, 3, 0, 3, 3);
	cairo_rectangle (cr2, 0, 3, 3, 3);
	cairo_set_source_rgb (cr2, 0, 0, 1);
	cairo_fill (cr2);
    }
    cairo_destroy (cr2);

    pattern = cairo_pattern_create_for_surface (surface);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    cairo_set_source (cr, pattern);

    cairo_rectangle (cr, 0, 0, width, height);
    cairo_fill (cr);

    cairo_pattern_destroy (pattern);
    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
