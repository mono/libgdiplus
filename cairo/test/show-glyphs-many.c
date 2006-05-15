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

#include <string.h>

/* Bug history
 *
 * 2006-01-07  Jon Hellan  <hellan@acm.org>
 *
 *   Jon opened the following bug report:
 *
 *	_XError from XRenderCompositeText8
 *	https://bugs.freedesktop.org/show_bug.cgi?id=5528
 *
 * 2006-03-02  Carl Worth  <cworth@cworth.org>
 *
 *   I wrote this test case to demonstrate the bug.
 *
 *   Approach:
 *
 *	Draw 65535 glyphs white-on-white all on top of each other.
 *
 *   Rationale:
 *
 *	The number 65535 comes from the original bug report.
 *
 *	I would use cairo_show_text with a long string of 'x's say,
 *	but then the surface would need to be enormous to contain
 *	them. A smaller surface could be used, but I fear that at some
 *	point the off-surface glyph drawing would be optimized away
 *	and not exercise the bug.
 *
 *	So, to keep the surface size under control, I use
 *	cairo_show_glyphs which allows me to place the glyphs all on
 *	top of each other. But, since cairo doesn't provide any
 *	character-to-glyphs mapping, I can't get a reliable glyph
 *	index (for character 'x' for example). So I just "guess" a
 *	glyph index and use white-on-white drawing to ignore the
 *	result. (I don't care what's drawn---I just want to ensure
 *	that things don't crash.)
 *
 *  Status: I replicated bug. The largest value of NUM_GLYPHS for
 *      which I saw success is 21842.
 */

#define TEXT_SIZE 12
#define NUM_GLYPHS 65535

/* This is the index into the font for what glyph we'll draw. Since we
 * don't guarantee we'll get any particular font, we can't relibably
 * get any particular glyph. But we don't care what we draw anyway,
 * (see discussion of white-on-white drawing above). For what it's
 * worth, this appears to be giving me 'M' with Bitstream Vera
 * Sans Mono. */
#define GLYPH_INDEX 48

cairo_test_t test = {
    "show-glyphs-many",
    "Test that cairo_show_glyps works when handed 'many' glyphs",
    9, 11
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_glyph_t glyphs[NUM_GLYPHS];
    cairo_font_options_t *font_options;
    int i;

    /* Initialize our giant array of glyphs. */
    for (i=0; i < NUM_GLYPHS; i++) {
	glyphs[i].index = GLYPH_INDEX;
	glyphs[i].x = 1.0;
	glyphs[i].y = height - 1;
    }

    /* Paint white background. */
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* white */
    cairo_paint (cr);

    cairo_select_font_face (cr, "Bitstream Vera Sans Mono",
			    CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, TEXT_SIZE);

    font_options = cairo_font_options_create ();

    cairo_font_options_set_hint_style (font_options, CAIRO_HINT_STYLE_NONE);
    cairo_font_options_set_antialias (font_options, CAIRO_ANTIALIAS_GRAY);

    cairo_set_font_options (cr, font_options);
    cairo_font_options_destroy (font_options);

    cairo_show_glyphs (cr, glyphs, NUM_GLYPHS);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
