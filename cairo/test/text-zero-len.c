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
 * Author: Behdad Esfahbod <behdad@behdad.org>
 */

/* Related bug 5177
 *
 * In short:
 *
 * _cairo_atsui_font_text_to_glyph with a zero-sized string crashes.
 *
 * Moreover, the fallback path in cairo_scaled_font_text_to_glyphs()
 * when handling a zero-sized string, allocates a zero-sized glyph array
 * and when NULL is returned by malloc, recognizes that as an out-of-memory
 * error.  The glibc implementation of malloc() does not return NULL from
 * malloc(0), but I don't think it's a safe assumption.
 *
 * By just bailing out on zero-sized text, we fix both issues.
 */

#include "cairo-test.h"

#define WIDTH  60
#define HEIGHT 60
#define NUM_TEXT 20
#define TEXT_SIZE 12

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "text-zero-len",
    "Tests show_text and text_path with a zero-sized string",
    WIDTH, HEIGHT,
    draw
};

/* Draw the word cairo at NUM_TEXT different angles */
static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_text_extents_t extents;

    cairo_select_font_face (cr, "Bitstream Vera Sans",
			    CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, 16);

    cairo_move_to (cr, 10, 25);
    cairo_show_text (cr, "");
    cairo_show_glyphs (cr, NULL, 0);
    cairo_show_glyphs (cr, (void*)8, 0);

    cairo_move_to (cr, 10, 55);
    cairo_text_path (cr, "");
    cairo_glyph_path (cr, (void*)8, 0);
    cairo_fill (cr);

    cairo_text_extents (cr, "", &extents);
    cairo_glyph_extents (cr, (void*)8, 0, &extents);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
