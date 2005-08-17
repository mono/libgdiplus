/*
 * Copyright © Jeff Muizelaar
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
 * JEFF MUIZELAAR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Jeff Muizelaar <jeff@infidigm.net>
 */

#include "cairo-test.h"

cairo_test_t test = {
    "a8-mask",
    "test masks of CAIRO_FORMAT_A8",
    8, 8
};

static unsigned char mask[] = {
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0,
    0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_pattern_t *pattern;

    cairo_set_source_rgb (cr, 0, 0, 1);
    cairo_paint (cr);

    surface = cairo_image_surface_create_for_data (mask, CAIRO_FORMAT_A8,
						   7, 8, 7);
    pattern = cairo_pattern_create_for_surface (surface);

    cairo_set_source_rgb (cr, 1, 0, 0);
    cairo_mask (cr, pattern);

    cairo_pattern_destroy (pattern);
    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test_expect_failure (&test, draw,
	    "image backend fails because libpixman only handles (stride % sizeof(pixman_bits) == 0)");
}
