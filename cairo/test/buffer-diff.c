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
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <pixman.h>

#include "cairo-test.h"

#include "buffer-diff.h"
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

/* Compare two buffers, returning the number of pixels that are
 * different and the maximum difference of any single color channel in
 * result_ret.
 *
 * This function should be rewritten to compare all formats supported by
 * cairo_format_t instead of taking a mask as a parameter.
 */
static void
buffer_diff_core (unsigned char *_buf_a,
		  unsigned char *_buf_b,
		  unsigned char *_buf_diff,
		  int		width,
		  int		height,
		  int		stride_a,
		  int		stride_b,
		  int		stride_diff,
		  pixman_bits_t mask,
		  buffer_diff_result_t *result_ret)
{
    int x, y;
    pixman_bits_t *row_a, *row_b, *row;
    buffer_diff_result_t result = {0, 0};
    pixman_bits_t *buf_a = (pixman_bits_t*)_buf_a;
    pixman_bits_t *buf_b = (pixman_bits_t*)_buf_b;
    pixman_bits_t *buf_diff = (pixman_bits_t*)_buf_diff;

    stride_a /= sizeof(pixman_bits_t);
    stride_b /= sizeof(pixman_bits_t);
    stride_diff /= sizeof(pixman_bits_t);
    for (y = 0; y < height; y++)
    {
	row_a = buf_a + y * stride_a;
	row_b = buf_b + y * stride_b;
	row = buf_diff + y * stride_diff;
	for (x = 0; x < width; x++)
	{
	    /* check if the pixels are the same */
	    if ((row_a[x] & mask) != (row_b[x] & mask)) {
		int channel;
		pixman_bits_t diff_pixel = 0;

		/* calculate a difference value for all 4 channels */
		for (channel = 0; channel < 4; channel++) {
		    int value_a = (row_a[x] >> (channel*8)) & 0xff;
		    int value_b = (row_b[x] >> (channel*8)) & 0xff;
		    unsigned int diff;
		    diff = abs (value_a - value_b);
		    if (diff > result.max_diff)
			result.max_diff = diff;
		    diff *= 4;  /* emphasize */
		    if (diff)
		        diff += 128; /* make sure it's visible */
		    if (diff > 255)
		        diff = 255;
		    diff_pixel |= diff << (channel*8);
		}

		result.pixels_changed++;
		row[x] = diff_pixel;
	    } else {
		row[x] = 0;
	    }
	    row[x] |= 0xff000000; /* Set ALPHA to 100% (opaque) */
	}
    }

    *result_ret = result;
}

void
buffer_diff (unsigned char *buf_a,
	     unsigned char *buf_b,
	     unsigned char *buf_diff,
	     int	   width,
	     int	   height,
	     int	   stride_a,
	     int	   stride_b,
	     int	   stride_diff,
	     buffer_diff_result_t *result)
{
    buffer_diff_core(buf_a, buf_b, buf_diff,
		     width, height, stride_a, stride_b, stride_diff, 0xffffffff,
		     result);
}

void
buffer_diff_noalpha (unsigned char *buf_a,
		     unsigned char *buf_b,
		     unsigned char *buf_diff,
		     int	   width,
		     int	   height,
		     int	   stride_a,
		     int	   stride_b,
		     int	   stride_diff,
		     buffer_diff_result_t *result)
{
    buffer_diff_core(buf_a, buf_b, buf_diff,
		     width, height, stride_a, stride_b, stride_diff, 0x00ffffff,
		     result);
}

static cairo_status_t
stdio_write_func (void *closure, const unsigned char *data, unsigned int length)
{
    FILE *file = closure;

    if (fwrite (data, 1, length, file) != length)
	return CAIRO_STATUS_WRITE_ERROR;

    return CAIRO_STATUS_SUCCESS;
}

/* Flatten an ARGB surface by blending it over white. The resulting
 * surface, (still in ARGB32 format, but with only alpha==1.0
 * everywhere) is returned in the same surface pointer.
 *
 * The original surface will be destroyed.
 *
 * The (x,y) value specify an origin of interest for the original
 * image. The flattened image will be generated only from the box
 * extending from (x,y) to (width,height).
 */
static void
flatten_surface (cairo_surface_t **surface, int x, int y)
{
    cairo_surface_t *flat;
    cairo_t *cr;

    flat = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
				       cairo_image_surface_get_width (*surface) - x,
				       cairo_image_surface_get_height (*surface) - y);
    cairo_surface_set_device_offset (flat, -x, -y);

    cr = cairo_create (flat);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);
    cairo_set_source_surface (cr, *surface, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);

    cairo_surface_destroy (*surface);
    *surface = flat;
}

/* Image comparison code courtesy of Richard Worth <richard@theworths.org>
 * Returns number of pixels changed, (or -1 on error).
 * Also saves a "diff" image intended to visually show where the
 * images differ.
 *
 * The return value simply indicates whether a check was successfully
 * made, (as opposed to a file-not-found condition or similar). It
 * does not indicate anything about how much the images differ. For
 * that, see result.
 *
 * One failure mode is if the two images provided do not have the same
 * dimensions. In this case, this function will return
 * CAIRO_STATUS_SURFACE_TYPE_MISMATCH (which is a bit of an abuse, but
 * oh well).
 */
static cairo_status_t
image_diff_core (const char *filename_a,
		 const char *filename_b,
		 const char *filename_diff,
		 int		ax,
		 int		ay,
		 int		bx,
		 int		by,
		 buffer_diff_result_t *result,
		 cairo_bool_t	flatten)
{
    unsigned int width_a, height_a, stride_a;
    unsigned int width_b, height_b, stride_b;
    cairo_surface_t *surface_a, *surface_b, *surface_diff;

    surface_a = cairo_image_surface_create_from_png (filename_a);
    if (cairo_surface_status (surface_a))
	return cairo_surface_status (surface_a);

    surface_b = cairo_image_surface_create_from_png (filename_b);
    if (cairo_surface_status (surface_b)) {
	cairo_surface_destroy (surface_a);
	return cairo_surface_status (surface_b);
    }

    width_a = cairo_image_surface_get_width (surface_a) - ax;
    height_a = cairo_image_surface_get_height (surface_a) - ay;
    width_b = cairo_image_surface_get_width (surface_b) - bx;
    height_b = cairo_image_surface_get_height (surface_b) - by;

    if (width_a  != width_b  ||
	height_a != height_b)
    {
	cairo_test_log ("Error: Image size mismatch: (%dx%d) vs. (%dx%d)\n"
			"       for %s vs. %s\n",
			width_a, height_a,
			width_b, height_b,
			filename_a, filename_b);
	cairo_surface_destroy (surface_a);
	cairo_surface_destroy (surface_b);
	return CAIRO_STATUS_SURFACE_TYPE_MISMATCH;
    }

    if (flatten) {
	flatten_surface (&surface_a, ax, ay);
	flatten_surface (&surface_b, bx, by);
	ax = ay = bx = by = 0;
    }

    surface_diff = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					       width_a, height_a);

    stride_a = cairo_image_surface_get_stride (surface_a);
    stride_b = cairo_image_surface_get_stride (surface_b);

    buffer_diff (cairo_image_surface_get_data (surface_a)
		 + (ay * stride_a) + ax * 4,
		 cairo_image_surface_get_data (surface_b)
		 + (by * stride_b) + by * 4,
		 cairo_image_surface_get_data (surface_diff),
		 width_a, height_a,
		 stride_a, stride_b,
		 cairo_image_surface_get_stride (surface_diff),
		 result);

    if (result->pixels_changed) {
	FILE *png_file;

	if (filename_diff)
	    png_file = fopen (filename_diff, "wb");
	else
	    png_file = stdout;

	cairo_surface_write_to_png_stream (surface_diff, stdio_write_func, png_file);

	if (png_file != stdout)
	    fclose (png_file);
    } else {
	if (filename_diff)
	    xunlink (filename_diff);
    }

    cairo_surface_destroy (surface_a);
    cairo_surface_destroy (surface_b);
    cairo_surface_destroy (surface_diff);

    return CAIRO_STATUS_SUCCESS;
}

cairo_status_t
image_diff (const char *filename_a,
	    const char *filename_b,
	    const char *filename_diff,
	    int		ax,
	    int		ay,
	    int		bx,
	    int		by,
	    buffer_diff_result_t *result)
{
    return image_diff_core (filename_a, filename_b, filename_diff,
			    ax, ay, bx, by,
			    result, FALSE);
}

cairo_status_t
image_diff_flattened (const char *filename_a,
		      const char *filename_b,
		      const char *filename_diff,
		      int	  ax,
		      int	  ay,
		      int	  bx,
		      int	  by,
		      buffer_diff_result_t *result)
{
    return image_diff_core (filename_a, filename_b, filename_diff,
			    ax, ay, bx, by,
			    result, TRUE);
}
