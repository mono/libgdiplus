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

#include <stdio.h>
#include <stdlib.h>

#include "buffer-diff.h"
#include "read-png.h"
#include "write-png.h"
#include "xmalloc.h"

int
main (int argc, char *argv[])
{
    int total_pixels_changed;

    unsigned int ax, ay, bx, by;

    if (argc != 3 && argc != 7) {
	fprintf (stderr, "Usage: %s image1.png image2.png [ax ay bx by]\n", argv[0]);
	fprintf (stderr, "Computes an output image designed to present a \"visual diff\" such that even\n");
	fprintf (stderr, "small errors in single pixels are readily apparent in the output.\n");
	fprintf (stderr, "The output image is written on stdout.\n");
	exit (1);
    }

    if (argc == 7) {
	ax = strtoul (argv[3], NULL, 0);
	ay = strtoul (argv[4], NULL, 0);
	bx = strtoul (argv[5], NULL, 0);
	by = strtoul (argv[6], NULL, 0);
    } else {
	ax = ay = bx = by = 0;
    }

    total_pixels_changed = image_diff (argv[1], argv[2], NULL, ax, ay, bx, by);

    if (total_pixels_changed)
	fprintf (stderr, "Total pixels changed: %d\n", total_pixels_changed);

    return (total_pixels_changed != 0);
}
