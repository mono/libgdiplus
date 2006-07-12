/*
 * Copyright © 2006 Jeff Muizelaar
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Jeff Muizelaar not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Jeff Muizelaar makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * JEFF MUIZELAAR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL JEFF MUIZELAAR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Jeff Muizelaar <jeff@infidigm.net>
 */

#include "cairo-test.h"

#define IMAGE_WIDTH 40
#define IMAGE_HEIGHT 22

cairo_test_t test = {
    "degenerate-path",
    "Tests the behaviour of degenerate paths with different cap types",
    IMAGE_WIDTH, IMAGE_HEIGHT
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    const cairo_line_cap_t cap[] = { CAIRO_LINE_CAP_ROUND, CAIRO_LINE_CAP_SQUARE, CAIRO_LINE_CAP_BUTT };
    int i;

    cairo_set_source_rgb (cr, 1, 0, 0);

    for (i=0; i<ARRAY_SIZE(cap); i++) {
	cairo_set_line_cap (cr, cap[i]);

	cairo_set_line_width (cr, 6);
	cairo_move_to (cr, 6, 6);
	cairo_line_to (cr, 6, 6);
	cairo_stroke (cr);

	cairo_move_to (cr, 6, 15);
	cairo_close_path (cr);
	cairo_stroke (cr);

	cairo_translate (cr, 12, 0);
    }
    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
