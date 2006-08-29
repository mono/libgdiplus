/*
 * Copyright © 2004 Red Hat, Inc.
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

/* Bug history
 *
 * 2004-11-03 Steve Chaplin <stevech1097@yahoo.com.au>
 *
 *   Reported bug on mailing list:
 *
 *	From: Steve Chaplin <stevech1097@yahoo.com.au>
 *	To: cairo@cairographics.org
 *	Date: Thu, 04 Nov 2004 00:00:17 +0800
 *	Subject: [cairo] Rotated text bug on drawable target
 *
 * 	The attached file draws text rotated 90 degrees first to a PNG file and
 *	then to a drawable. The PNG file looks fine, the text on the drawable is
 *	unreadable.
 *
 *	Steve
 *
 * 2004-11-03 Carl Worth <cworth@cworth.org>
 *
 *   Looks like the major problems with this bug appeared in the great
 *   font rework between 0.1.23 and 0.2.0. And it looks like we need
 *   to fix the regression test suite to test the xlib target (since
 *   the bug does not show up in the png backend).
 *
 *   Hmm... Actually, things don't look perfect even in the PNG
 *   output. Look at how that 'o' moves around. It's particularly off
 *   in the case where it's rotated by PI.
 *
 *   And I'm still not sure about what to do for test cases with
 *   text--a new version of freetype will change everything. We may
 *   need to add a simple backend for stroked fonts and add a simple
 *   builtin font to cairo for pixel-perfect tests with text.
 *
 * 2005-08-23
 *
 *   It appears that the worst placement and glyph selection problems
 *   have now been resolved. In the past some letters were noticeably
 *   of a different size at some rotations, and there was a lot of
 *   drift away from the baseline. These problems do not appear
 *   anymore.
 *
 *   Another thing that helps is that we now have font options which
 *   we can use to disable hinting in order to get more repeatable
 *   results. I'm doing that in this test now.
 *
 *   There are still some subtle positioning problems which I'm
 *   assuming are due to the lack of finer-than-whole-pixel glyph
 *   positioning. I'm generating a reference image now by replacing
 *   cairo_show_text with cairo_text_path; cairo_fill. This will let
 *   us look more closely at the remaining positioning problems. (In
 *   particular, I want to make sure we're rounding as well as
 *   possible).
 */

#include "cairo-test.h"

#define WIDTH  150
#define HEIGHT 150
#define NUM_TEXT 20
#define TEXT_SIZE 12

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "text-rotate",
    "Tests show_text under various rotations"
    "\nminor bugs in positioning rotated glyphs",
    WIDTH, HEIGHT,
    draw
};

/* Draw the word cairo at NUM_TEXT different angles */
static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    int i, x_off, y_off;
    cairo_text_extents_t extents;
    cairo_font_options_t *font_options;
    static char text[] = "cairo";

    cairo_select_font_face (cr, "Bitstream Vera Sans",
			    CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, TEXT_SIZE);

    font_options = cairo_font_options_create ();

    cairo_get_font_options (cr, font_options);
    cairo_font_options_set_hint_metrics (font_options, CAIRO_HINT_METRICS_OFF);

    cairo_set_font_options (cr, font_options);
    cairo_font_options_destroy (font_options);

    cairo_set_source_rgb (cr, 0, 0, 0);

    cairo_translate (cr, WIDTH/2.0, HEIGHT/2.0);

    cairo_text_extents (cr, text, &extents);

    if (NUM_TEXT == 1) {
	x_off = y_off = 0;
    } else {
	y_off = - floor (0.5 + extents.height / 2.0);
	x_off = floor (0.5 + (extents.height+1) / (2 * tan (M_PI/NUM_TEXT)));
    }

    for (i=0; i < NUM_TEXT; i++) {
	cairo_save (cr);
	cairo_rotate (cr, 2*M_PI*i/NUM_TEXT);
	cairo_set_line_width (cr, 1.0);
	cairo_rectangle (cr, x_off - 0.5, y_off - 0.5, extents.width + 1, extents.height + 1);
	cairo_set_source_rgb (cr, 1, 0, 0);
	cairo_stroke (cr);
	cairo_move_to (cr, x_off - extents.x_bearing, y_off - extents.y_bearing);
	cairo_set_source_rgb (cr, 0, 0, 0);
#if CAIRO_TEST_GENERATE_REFERENCE_IMAGE
	cairo_text_path (cr, "cairo");
	cairo_fill (cr);
#else
	cairo_show_text (cr, "cairo");
#endif
	cairo_restore (cr);
    }

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
