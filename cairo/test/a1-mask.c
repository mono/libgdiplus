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
 * Authors: Jeff Muizelaar <jeff@infidigm.net>
 *	    Carl Worth <cworth@cworth.org>
 *	    Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairo-test.h"

static cairo_test_draw_function_t draw;

#define MASK_WIDTH 10
#define MASK_HEIGHT 8

#ifdef WORDS_BIGENDIAN
#define MASK 0x28, 0x55
#else
#define MASK 0x14, 0xAA
#endif
static unsigned char mask[(MASK_WIDTH + 7) / 8 * MASK_HEIGHT] = {
    MASK,
    MASK,
    MASK,
    MASK,
    MASK,
    MASK,
    MASK,
    MASK,
};

cairo_test_t test = {
    "a1-mask",
    "test masks of CAIRO_FORMAT_A1",
    MASK_WIDTH, MASK_HEIGHT,
    draw
};


static cairo_test_status_t
check_status (cairo_status_t status, cairo_status_t expected)
{
    if (status == expected)
	return CAIRO_TEST_SUCCESS;

    cairo_test_log ("Error: Expected status value %d (%s), received %d (%s)\n",
		    expected,
		    cairo_status_to_string (expected),
		    status,
		    cairo_status_to_string (status));
    return CAIRO_TEST_FAILURE;
}

static cairo_test_status_t
test_surface_with_width_and_stride (int width, int stride,
				    cairo_status_t expected)
{
    cairo_test_status_t status;
    cairo_surface_t *surface;
    cairo_t *cr;
    int len;
    unsigned char *data;

    cairo_test_log ("Creating surface with width %d and stride %d\n",
		    width, stride);

    len = stride;
    if (len < 0)
	len = -len;
    data = xmalloc (len);

    surface = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_A1,
						   width, 1, stride);
    cr = cairo_create (surface);

    cairo_paint (cr);

    status = check_status (cairo_surface_status (surface), expected);
    if (status)
	goto BAIL;

    status = check_status (cairo_status (cr), expected);
    if (status)
	goto BAIL;

  BAIL:
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    free (data);
    return status;
}

static cairo_test_status_t
draw (cairo_t *cr, int dst_width, int dst_height)
{
    int test_width, test_stride, stride, row;
    unsigned char *src, *dst, *mask_aligned;
    cairo_surface_t *surface;
    cairo_pattern_t *pattern;
    cairo_test_status_t status;
    cairo_status_t expected;

    for (test_width = 0; test_width < 40; test_width++) {
	test_stride = (test_width + 7) / 8;
	stride = cairo_format_stride_for_width (CAIRO_FORMAT_A1,
						test_width);

	/* First create a surface using the width as the stride, (most
	 * of these should fail). */
	expected = (stride == test_stride) ?
	    CAIRO_STATUS_SUCCESS : CAIRO_STATUS_INVALID_STRIDE;

	status = test_surface_with_width_and_stride (test_width,
						     test_stride,
						     expected);
	if (status)
	    return status;

	status = test_surface_with_width_and_stride (test_width,
						     -test_stride,
						     expected);
	if (status)
	    return status;


	/* Then create a surface using the correct stride, (should
	   always succeed).*/
	status = test_surface_with_width_and_stride (test_width,
						     stride,
						     CAIRO_STATUS_SUCCESS);
	if (status)
	    return status;

	status = test_surface_with_width_and_stride (test_width,
						     -stride,
						     CAIRO_STATUS_SUCCESS);
	if (status)
	    return status;
    }

    /* Now test actually drawing through our mask data, allocating and
     * copying with the proper stride. */
    surface = cairo_image_surface_create (CAIRO_FORMAT_A1,
					  MASK_WIDTH,
					  MASK_HEIGHT);

    mask_aligned = cairo_image_surface_get_data (surface);
    if (mask_aligned != NULL) {
	stride = cairo_image_surface_get_stride (surface);
	src = mask;
	dst = mask_aligned;
	for (row = 0; row < MASK_HEIGHT; row++) {
	    memcpy (dst, src, (MASK_WIDTH + 7) / 8);
	    src += (MASK_WIDTH + 7) / 8;
	    dst += stride;
	}
    }

    /* Paint background blue */
    cairo_set_source_rgb (cr, 0, 0, 1); /* blue */
    cairo_paint (cr);

    /* Then paint red through our mask */
    pattern = cairo_pattern_create_for_surface (surface);

    cairo_set_source_rgb (cr, 1, 0, 0); /* red */
    cairo_mask (cr, pattern);

    cairo_pattern_destroy (pattern);
    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
