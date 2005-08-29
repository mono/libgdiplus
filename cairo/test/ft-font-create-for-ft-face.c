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
#include <cairo-ft.h>

cairo_test_t test = {
    "ft-font-create-for-ft-face",
    "Simple test to verify that cairo_ft_font_create_for_ft_face doesn't crash.",
    0, 0
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    FcPattern *pattern;
    cairo_font_face_t *font_face;
    cairo_scaled_font_t *scaled_font;
    cairo_font_options_t *font_options;
    cairo_font_extents_t font_extents;
    cairo_matrix_t font_matrix, ctm;
    FT_Face ft_face;

    /* We're trying here to get our hands on _some_ FT_Face but we do
     * not at all care which one, so an empty pattern should work just
     * fine. */
    pattern = FcPatternCreate ();
    if (!pattern)
	return CAIRO_TEST_FAILURE;

    font_face = cairo_ft_font_face_create_for_pattern (pattern);

    cairo_matrix_init_identity (&font_matrix);

    cairo_get_matrix (cr, &ctm);

    font_options = cairo_font_options_create ();

    scaled_font = cairo_scaled_font_create (font_face,
					    &font_matrix,
					    &ctm,
					    font_options);

    ft_face = cairo_ft_scaled_font_lock_face (scaled_font);

    cairo_font_options_destroy (font_options);
    cairo_font_face_destroy (font_face);
    FcPatternDestroy (pattern);

    if (!ft_face) {
	cairo_scaled_font_destroy (scaled_font);
	return CAIRO_TEST_FAILURE;
    }

    /* phew, that was a lot of work. But at least we didn't ever have
     * to call freetype directly, nor did we have to make many (any?)
     * assumptions about the current system.
     *
     * Now, on to the simple thing we actually want to test.
     */
    font_face = cairo_ft_font_face_create_for_ft_face (ft_face, 0);

    /* Set the font_face and force cairo to actually use it for
     * something. */
    cairo_set_font_face (cr, font_face);
    cairo_font_extents (cr, &font_extents);

    /* Finally, even more cleanup */
    cairo_font_face_destroy (font_face);
    cairo_ft_scaled_font_unlock_face (scaled_font);
    cairo_scaled_font_destroy (scaled_font);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
