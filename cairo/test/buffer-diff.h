/* imagediff - Compare two images
 *
 * Copyright © 2004 Richard D. Worth
 * Copyright © 2006 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the authors
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * The authors make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors: Richard D. Worth <richard@theworths.org>
 *	    Carl Worth <cworth@cworth.org>
 */

#ifndef BUFFER_DIFF_H
#define BUFFER_DIFF_H

#include "cairo-test.h"

typedef struct _buffer_diff_result {
    unsigned int pixels_changed;
    unsigned int max_diff;
} buffer_diff_result_t;

/* Compares two image surfaces
 *
 * Provides number of pixels changed and maximum single-channel
 * difference in result.
 *
 * Also fills in a "diff" surface intended to visually show where the
 * images differ.
 */
void
compare_surfaces (cairo_surface_t	*surface_a,
		  cairo_surface_t	*surface_b,
		  cairo_surface_t	*surface_diff,
		  buffer_diff_result_t *result);

/* Compares two image buffers ignoring the alpha channel.
 *
 * Provides number of pixels changed and maximum single-channel
 * difference in result.
 *
 * Also fills in a "diff" buffer intended to visually show where the
 * images differ.
 */
void
buffer_diff_noalpha (unsigned char *buf_a,
		     unsigned char *buf_b,
		     unsigned char *buf_diff,
		     int	    width,
		     int	    height,
		     int	    stride,
		     buffer_diff_result_t *result);

/* Compares two image buffers ignoring the alpha channel. A return
 * value of CAIRO_STATUS_SUCCESS indicates that a comparison was made,
 * (but the images may or may not differ). Failure modes include
 * CAIRO_STATUS_FILE_NOT_FOUND, CAIRO_STATUS_READ_ERROR,
 * CAIRO_STATUS_NO_MEMORY, and CAIRO_STATUS_SURFACE_TYPE_MISMATCH
 * (which is used if the image sizes differ).
 *
 * Provides number of pixels changed and maximum single-channel
 * difference in result.
 *
 * Also saves a "diff" image intended to visually show where the
 * images differ.
 */
cairo_status_t
image_diff (const char *filename_a,
	    const char *filename_b,
	    const char *filename_diff,
	    int		ax,
	    int		ay,
	    int		bx,
	    int		by,
	    buffer_diff_result_t *result);

/* Like image_diff, but blending the contents of b over white first. */
cairo_status_t
image_diff_flattened (const char *filename_a,
		      const char *filename_b,
		      const char *filename_diff,
                      int         ax,
                      int         ay,
                      int         bx,
                      int         by,
		      buffer_diff_result_t *result);

#endif
