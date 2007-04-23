/*
 * Copyright © 2007 Red Hat, Inc.
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
 * Author: Carl Worth <cworth@cworth.org>
 */

#include "cairo-test.h"

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "invalid-matrix",
    "Test that all relevant public functions return CAIRO_STATUS_INVALID_MATRIX as appropriate",
    0, 0,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_status_t status;
    cairo_surface_t *target;
    cairo_font_face_t *font_face;
    cairo_font_options_t *font_options;
    cairo_scaled_font_t *scaled_font;
    cairo_pattern_t *pattern;
    cairo_t *cr2;
    cairo_matrix_t identity, invalid = {
	4.0, 4.0,
	4.0, 4.0,
	4.0, 4.0
    };

#define CHECK_STATUS(status, function_name)						\
if ((status) == CAIRO_STATUS_SUCCESS) {							\
    cairo_test_log ("Error: %s with invalid matrix passed",				\
		    (function_name));							\
    return CAIRO_TEST_FAILURE;								\
} else if ((status) != CAIRO_STATUS_INVALID_MATRIX) {					\
    cairo_test_log ("Warning: %s with invalid matrix returned unexpected status "	\
		    "(%d): %s\n",							\
		    (function_name),							\
		    status,								\
		    cairo_status_to_string (status));					\
}

    cairo_matrix_init_identity (&identity);

    target = cairo_get_target (cr);

    /* test cairo_transform with invalid matrix */
    cr2 = cairo_create (target);
    cairo_transform (cr2, &invalid);

    status = cairo_status (cr2);
    CHECK_STATUS (status,"cairo_transform");

    cairo_destroy (cr2);

    /* test cairo_set_matrix with invalid matrix */
    cr2 = cairo_create (target);
    cairo_set_matrix (cr2, &invalid);

    status = cairo_status (cr2);
    CHECK_STATUS (status, "cairo_set_matrix");

    cairo_destroy (cr2);

    /* test cairo_set_font_matrix with invalid matrix */
    cr2 = cairo_create (target);
    cairo_set_font_matrix (cr2, &invalid);

    /* draw some text to force the font to be resolved */
    cairo_show_text (cr2, "hello");

    status = cairo_status (cr2);
    CHECK_STATUS (status, "cairo_set_font_matrix");

    cairo_destroy (cr2);

    /* test cairo_scaled_font_create with invalid matrix */
    font_face = cairo_get_font_face (cr);
    font_options = cairo_font_options_create ();
    cairo_get_font_options (cr, font_options);
    scaled_font = cairo_scaled_font_create (font_face,
					    &invalid,
					    &identity,
					    font_options);
    status = cairo_scaled_font_status (scaled_font);
    CHECK_STATUS (status, "cairo_scaled_font_create");

    cairo_scaled_font_destroy (scaled_font);

    scaled_font = cairo_scaled_font_create (font_face,
					    &identity,
					    &invalid,
					    font_options);
    status = cairo_scaled_font_status (scaled_font);
    CHECK_STATUS (status, "cairo_scaled_font_create");

    cairo_scaled_font_destroy (scaled_font);
    cairo_font_options_destroy (font_options);

    /* test cairo_pattern_set_matrix with invalid matrix */
    pattern = cairo_pattern_create_rgb (1.0, 1.0, 1.0);
    cairo_pattern_set_matrix (pattern, &invalid);
    status = cairo_pattern_status (pattern);
    CHECK_STATUS (status, "cairo_pattern_set_matrix");
    cairo_pattern_destroy (pattern);

    /* test cairo_matrix_invert with invalid matrix */
    status = cairo_matrix_invert (&invalid);
    CHECK_STATUS (status, "cairo_matrix_invert");

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
