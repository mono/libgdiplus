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

#define TEXT_SIZE 12
#define PAD 4
#define TEXT "text"

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "font-matrix-translation",
    "Test that translation in a font matrix can be used to offset a string",
    38, 34,
    draw
};

static void
box_text (cairo_t *cr, const char *utf8, double x, double y)
{
    double line_width;
    cairo_text_extents_t extents;

    cairo_save (cr);

    cairo_text_extents (cr, utf8, &extents);
    line_width = cairo_get_line_width (cr);
    cairo_rectangle (cr,
		     x + extents.x_bearing - line_width / 2,
		     y + extents.y_bearing - line_width / 2,
		     extents.width  + line_width,
		     extents.height + line_width);
    cairo_stroke (cr);

    cairo_move_to (cr, x, y);
    cairo_show_text (cr, utf8);

    cairo_restore (cr);
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_text_extents_t extents;
    cairo_matrix_t matrix;

    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* white */
    cairo_paint (cr);

    cairo_select_font_face (cr, "Bitstream Vera Sans",
			    CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, TEXT_SIZE);

    cairo_translate (cr, PAD, PAD);
    cairo_set_line_width (cr, 1.0);

    cairo_text_extents (cr, TEXT, &extents);

    /* Draw text and bounding box */
    cairo_set_source_rgb (cr, 0, 0, 0); /* black */
    box_text (cr, TEXT, 0, - extents.y_bearing);

    /* Then draw again with the same coordinates, but with a font
     * matrix to position the text below and shifted a bit to the
     * right. */
    cairo_matrix_init_translate (&matrix, TEXT_SIZE / 2, TEXT_SIZE + PAD);
    cairo_matrix_scale (&matrix, TEXT_SIZE, TEXT_SIZE);
    cairo_set_font_matrix (cr, &matrix);

    cairo_set_source_rgb (cr, 0, 0, 1); /* blue */
    box_text (cr, TEXT, 0, - extents.y_bearing);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
