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

#include "pdiff.h"
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
		  int		stride,
		  pixman_bits_t mask,
		  buffer_diff_result_t *result_ret)
{
    int x, y;
    pixman_bits_t *row_a, *row_b, *row;
    buffer_diff_result_t result = {0, 0};
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
compare_surfaces (cairo_surface_t	*surface_a,
		  cairo_surface_t	*surface_b,
		  cairo_surface_t	*surface_diff,
		  buffer_diff_result_t	*result)
{
    /* These default values were taken straight from the
     * perceptualdiff program. We'll probably want to tune these as
     * necessary. */
    double gamma = 2.2;
    double luminance = 100.0;
    double field_of_view = 45.0;
    int discernible_pixels_changed;

    /* First, we run cairo's old buffer_diff algorithm which looks for
     * pixel-perfect images, (we do this first since the test suite
     * runs about 3x slower if we run pdiff_compare first).
     */
    buffer_diff_core (cairo_image_surface_get_data (surface_a),
		      cairo_image_surface_get_data (surface_b),
		      cairo_image_surface_get_data (surface_diff),
		      cairo_image_surface_get_width (surface_a),
		      cairo_image_surface_get_height (surface_a),
		      cairo_image_surface_get_stride (surface_a),
		      0xffffffff,
		      result);
    if (result->pixels_changed == 0)
	return;

    cairo_test_log ("%d pixels differ (with maximum difference of %d) from reference image\n",
		    result->pixels_changed, result->max_diff);

    /* Then, if there are any different pixels, we give the pdiff code
     * a crack at the images. If it decides that there are no visually
     * discernible differences in any pixels, then we accept this
     * result as good enough. */
    discernible_pixels_changed = pdiff_compare (surface_a, surface_b,
						gamma, luminance, field_of_view);
    if (discernible_pixels_changed == 0) {
	result->pixels_changed = 0;
	cairo_test_log ("But perceptual diff finds no visually discernible difference.\n"
			"Accepting result.\n");
    }
}

void
buffer_diff_noalpha (unsigned char *buf_a,
		     unsigned char *buf_b,
		     unsigned char *buf_diff,
		     int	   width,
		     int	   height,
		     int	   stride,
		     buffer_diff_result_t *result)
{
    buffer_diff_core(buf_a, buf_b, buf_diff,
		     width, height, stride, 0x00ffffff,
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

/* Given an image surface, create a new surface that has the same
 * contents as the sub-surface with its origin at x,y.
 *
 * The original surface will be destroyed.
 */
static void
extract_sub_surface (cairo_surface_t **surface, int x, int y)
{
    cairo_surface_t *sub;
    cairo_t *cr;

    sub = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
				      cairo_image_surface_get_width (*surface) - x,
				      cairo_image_surface_get_height (*surface) - y);

    /* We don't use a device offset like flatten_surface. That's not
     * for any important reason, (the results should be
     * identical). This style just seemed more natural to me this
     * time, so I'm leaving both here so I can look at both to see
     * which I like better. */
    cr = cairo_create (sub);
    cairo_set_source_surface (cr, *surface, -x, -y);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);
    cairo_destroy (cr);

    cairo_surface_destroy (*surface);
    *surface = sub;
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
    cairo_status_t status;
    unsigned int width_a, height_a, stride_a;
    unsigned int width_b, height_b, stride_b;
    cairo_surface_t *surface_a, *surface_b, *surface_diff;

    surface_a = cairo_image_surface_create_from_png (filename_a);
    status = cairo_surface_status (surface_a);
    if (status) {
	cairo_test_log ("Error: Failed to create surface from %s: %s\n",
			filename_a, cairo_status_to_string (status));
	return status;
    }

    surface_b = cairo_image_surface_create_from_png (filename_b);
    status = cairo_surface_status (surface_b);
    if (status) {
	cairo_test_log ("Error: Failed to create surface from %s: %s\n",
			filename_b, cairo_status_to_string (status));
	cairo_surface_destroy (surface_a);
	return status;
    }

    if (flatten) {
	flatten_surface (&surface_a, ax, ay);
	flatten_surface (&surface_b, bx, by);
	ax = ay = bx = by = 0;
    }

    if (ax || ay) {
	extract_sub_surface (&surface_a, ax, ay);
	ax = ay = 0;
    }

    if (bx || by) {
	extract_sub_surface (&surface_b, bx, by);
	bx = by = 0;
    }

    width_a = cairo_image_surface_get_width (surface_a);
    height_a = cairo_image_surface_get_height (surface_a);
    stride_a = cairo_image_surface_get_stride (surface_a);
    width_b = cairo_image_surface_get_width (surface_b);
    height_b = cairo_image_surface_get_height (surface_b);
    stride_b = cairo_image_surface_get_stride (surface_b);

    if (width_a  != width_b  ||
	height_a != height_b ||
	stride_a != stride_b)
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

    surface_diff = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					       width_a, height_a);

    compare_surfaces (surface_a, surface_b, surface_diff, result);

    status = CAIRO_STATUS_SUCCESS;
    if (result->pixels_changed) {
	FILE *png_file;

	if (filename_diff)
	    png_file = fopen (filename_diff, "wb");
	else
	    png_file = stdout;

	status = cairo_surface_write_to_png_stream (surface_diff, stdio_write_func, png_file);

	if (png_file != stdout)
	    fclose (png_file);
    } else {
	if (filename_diff)
	    xunlink (filename_diff);
    }

    cairo_surface_destroy (surface_a);
    cairo_surface_destroy (surface_b);
    cairo_surface_destroy (surface_diff);

    return status;
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
