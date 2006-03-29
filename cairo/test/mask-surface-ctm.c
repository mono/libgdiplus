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

cairo_test_t test = {
    "mask-surface-ctm",
    "Test that cairo_mask_surface is affected properly by the CTM",
    10, 10
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *mask;
    uint32_t data[] = {
	0x80000000, 0x80000000,
	0x80000000, 0x80000000,
    };

    mask = cairo_image_surface_create_for_data ((unsigned char *) data,
						CAIRO_FORMAT_ARGB32, 2, 2, 8);

    cairo_set_source_rgb (cr, 1.0, 0, 0);

    /* We can translate with the CTM, with the mask_surface offset, or
     * with both. */

    /* 1. CTM alone. */
    cairo_save (cr);
    {
	cairo_translate (cr, 2, 2);
	cairo_mask_surface (cr, mask, 0, 0);
    }
    cairo_restore (cr);

    /* 2. Offset alone. */
    cairo_mask_surface (cr, mask, 4, 4);

    /* 3. CTM + offset */
    cairo_translate (cr, 2, 2);
    cairo_mask_surface (cr, mask, 4, 4);

    cairo_surface_destroy (mask);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
