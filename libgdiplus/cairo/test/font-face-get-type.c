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
    cairo_t *cr;
    cairo_font_face_t *font_face;

    cairo_test_init ("font-face-get-type");

    cairo_test_log ("Creating cairo context and obtaining a font face\n");

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
    cr = cairo_create (surface);

    cairo_select_font_face (cr, "Bitstream Vera Sans",
			    CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_NORMAL);

    font_face = cairo_get_font_face (cr);

    cairo_test_log ("Testing return value of cairo_font_face_get_type\n");

    if (cairo_font_face_get_type (font_face) != CAIRO_FONT_TYPE_TOY) {
	cairo_test_log ("Unexpected value %d from cairo_font_face_get_type (expected %d)\n",
			cairo_font_face_get_type (font_face), CAIRO_FONT_TYPE_TOY);
	return CAIRO_TEST_FAILURE;
    }

    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}
