/*
 * Copyright © 2007 Chris Wilson.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Chris Wilson. Not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Chris Wilson makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * CHRIS WILSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL CHRIS WILSON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Chris Wilson <chris at chris-wilson.co.uk>
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "cairo-test.h"

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "solid-pattern-cache-stress",
    "Stress the solid pattern cache and ensure it behaves",
    0, 0,
    draw
};
#include <cairo.h>
#include <stdlib.h>

#define LOOPS 10
#define NRAND 100

#ifndef HAVE_DRAND48
#define drand48() (rand () / (double) RAND_MAX)
#endif

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    int loop;
    int i;

    for (loop = 0; loop < LOOPS; loop++) {
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); /* black */
	cairo_set_source_rgb (cr, 1.0, 0.0, 0.0); /* red */
	cairo_set_source_rgb (cr, 0.0, 1.0, 0.0); /* green */
	cairo_set_source_rgb (cr, 1.0, 1.0, 0.0); /* yellow */
	cairo_set_source_rgb (cr, 0.0, 0.0, 1.0); /* blue */
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0); /* magenta */
	cairo_set_source_rgb (cr, 0.0, 1.0, 1.0); /* cyan */
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* white */

	cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0); /* black */
	cairo_set_source_rgba (cr, 1.0, 0.0, 0.0, 1.0); /* red */
	cairo_set_source_rgba (cr, 0.0, 1.0, 0.0, 1.0); /* green */
	cairo_set_source_rgba (cr, 1.0, 1.0, 0.0, 1.0); /* yellow */
	cairo_set_source_rgba (cr, 0.0, 0.0, 1.0, 1.0); /* blue */
	cairo_set_source_rgba (cr, 1.0, 0.0, 1.0, 1.0); /* magenta */
	cairo_set_source_rgba (cr, 0.0, 1.0, 1.0, 1.0); /* cyan */
	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0); /* white */

	for (i = 0; i < NRAND; i++)
	    cairo_set_source_rgba (cr,
		                   drand48 (),
				   drand48 (),
				   drand48 (),
				   drand48 ());
    }

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}

