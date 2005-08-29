/* imagediff - Compare two images
 *
 * Copyright © 2004 Richard D. Worth
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Richard Worth
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Richard Worth makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * RICHARD WORTH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL RICHARD WORTH BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Richard D. Worth <richard@theworths.org> */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <pixman.h>

#include "cairo-test.h"

#include "buffer-diff.h"
#include "read-png.h"
#include "write-png.h"
#include "xmalloc.h"

static void
xunlink (const char *pathname)
{
    if (unlink (pathname) < 0 && errno != ENOENT) {
	cairo_test_log ("  Error: Cannot remove %s: %s\n",
			pathname, strerror (errno));
	exit (1);
    }
}


/* This function should be rewritten to compare all formats supported by
 * cairo_format_t instead of taking a mask as a parameter.
 */
static int
buffer_diff_core (unsigned char *_buf_a,
		  unsigned char *_buf_b,
		  unsigned char *_buf_diff,
		  int		width,
		  int		height,
		  int		stride,
		  pixman_bits_t mask)
{
    int x, y;
    pixman_bits_t *row_a, *row_b, *row;
    int pixels_changed = 0;
    pixman_bits_t *buf_a = (pixman_bits_t*)_buf_a;
    pixman_bits_t *buf_b = (pixman_bits_t*)_buf_b;
    pixman_bits_t *buf_diff = (pixman_bits_t*)_buf_diff;

    stride /= sizeof(pixman_bits_t);
    for (y = 0; y < height; y++)
    {
	row_a = buf_a + y * stride;
	row_b = buf_b + y * stride;
	row = buf_diff + y * stride;
	for (x = 0; x < width; x++)
	{
	    /* check if the pixels are the same */
	    if ((row_a[x] & mask) != (row_b[x] & mask)) {
		int channel;
		pixman_bits_t diff_pixel = 0;

		/* calculate a difference value for all 4 channels */
		for (channel = 0; channel < 4; channel++) {
		    unsigned char value_a = (row_a[x] >> (channel*8));
		    unsigned char value_b = (row_b[x] >> (channel*8));
		    double diff;
		    diff = value_a - value_b;
		    diff_pixel |= (unsigned char)(128 + diff / 3.0) << (channel*8);
		}

		pixels_changed++;
		row[x] = diff_pixel;
	    } else {
		row[x] = 0;
	    }
	    row[x] |= 0xff000000; /* Set ALPHA to 100% (opaque) */
	}
    }

    return pixels_changed;
}

int
buffer_diff (unsigned char *buf_a,
	     unsigned char *buf_b,
	     unsigned char *buf_diff,
	     int	   width,
	     int	   height,
	     int	   stride)
{
    return buffer_diff_core(buf_a, buf_b, buf_diff, width, height, stride, 0xffffffff);
}

int
buffer_diff_noalpha (unsigned char *buf_a,
		     unsigned char *buf_b,
		     unsigned char *buf_diff,
		     int	   width,
		     int	   height,
		     int	   stride)
{
    return buffer_diff_core(buf_a, buf_b, buf_diff, width, height, stride, 0x00ffffff);
}

/* Image comparison code courtesy of Richard Worth <richard@theworths.org>
 * Returns number of pixels changed, (or -1 on error).
 * Also saves a "diff" image intended to visually show where the
 * images differ.
 */
int
image_diff (const char *filename_a,
	    const char *filename_b,
	    const char *filename_diff)
{
    int pixels_changed;
    unsigned int width_a, height_a, stride_a;
    unsigned int width_b, height_b, stride_b;
    unsigned char *buf_a, *buf_b, *buf_diff;
    read_png_status_t status;

    status = read_png_argb32 (filename_a, &buf_a, &width_a, &height_a, &stride_a);
    if (status)
	return -1;

    status = read_png_argb32 (filename_b, &buf_b, &width_b, &height_b, &stride_b);
    if (status) {
	free (buf_a);
	return -1;
    }

    if (width_a  != width_b  ||
	height_a != height_b ||
	stride_a != stride_b)
    {
	cairo_test_log ("Error: Image size mismatch: (%dx%d@%d) vs. (%dx%d@%d)\n"
			"       for %s vs. %s\n",
			width_a, height_a, stride_a,
			width_b, height_b, stride_b,
			filename_a, filename_b);
	free (buf_a);
	free (buf_b);
	return -1;
    }

    buf_diff = xcalloc (stride_a * height_a, 1);

    pixels_changed = buffer_diff (buf_a, buf_b, buf_diff,
				  width_a, height_a, stride_a);

    if (pixels_changed) {
	FILE *png_file = fopen (filename_diff, "wb");
	write_png_argb32 (buf_diff, png_file, width_a, height_a, stride_a);
	fclose (png_file);
    } else {
	xunlink (filename_diff);
    }

    free (buf_a);
    free (buf_b);
    free (buf_diff);

    return pixels_changed;
}
