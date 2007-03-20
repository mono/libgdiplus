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

#include "cairo-test.h"

int
main (void)
{
    cairo_surface_t *surface;
    cairo_pattern_t *solid_rgb, *solid_rgba, *surface_pattern, *linear, *radial;

    cairo_test_init ("pattern-get-type");

    cairo_test_log ("Creating patterns of all types\n");

    solid_rgb = cairo_pattern_create_rgb (0.0, 0.1, 0.2);
    solid_rgba = cairo_pattern_create_rgba (0.3, 0.4, 0.5, 0.6);
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					  1, 1);
    surface_pattern = cairo_pattern_create_for_surface (surface);
    linear = cairo_pattern_create_linear (0.0, 0.0, 10.0, 10.0);
    radial = cairo_pattern_create_radial (10.0, 10.0, 0.1,
					  10.0, 10.0, 1.0);

    cairo_test_log ("Verifying return values of cairo_pattern_get_type\n");

    if (cairo_pattern_get_type (solid_rgb) != CAIRO_PATTERN_TYPE_SOLID)
	return CAIRO_TEST_FAILURE;

    if (cairo_pattern_get_type (solid_rgba) != CAIRO_PATTERN_TYPE_SOLID)
	return CAIRO_TEST_FAILURE;

    if (cairo_pattern_get_type (surface_pattern) != CAIRO_PATTERN_TYPE_SURFACE)
	return CAIRO_TEST_FAILURE;

    if (cairo_pattern_get_type (linear) != CAIRO_PATTERN_TYPE_LINEAR)
	return CAIRO_TEST_FAILURE;

    if (cairo_pattern_get_type (radial) != CAIRO_PATTERN_TYPE_RADIAL)
	return CAIRO_TEST_FAILURE;

    cairo_test_log ("Cleaning up\n");

    cairo_pattern_destroy (solid_rgb);
    cairo_pattern_destroy (solid_rgba);
    cairo_pattern_destroy (surface_pattern);
    cairo_surface_destroy (surface);
    cairo_pattern_destroy (linear);
    cairo_pattern_destroy (radial);

    cairo_test_fini ();

    return CAIRO_TEST_SUCCESS;
}
