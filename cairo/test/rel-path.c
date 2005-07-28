/*
 * $Id: rel-path.c,v 1.1 2005/05/02 19:36:20 keithp Exp $
 *
 * Copyright © 2005 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "cairo-test.h"

#define SIZE 10

cairo_test_t test = {
    "rel-path",
    "Tests calls to various relative path functions",
    SIZE, SIZE
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_move_to (cr, 0, 0);
    cairo_rel_move_to (cr, SIZE, SIZE/2);
    cairo_rel_line_to (cr, -SIZE, SIZE/2);
    cairo_rel_curve_to (cr,
			SIZE/2, -SIZE/2,
			SIZE*2/3, -SIZE/3,
			SIZE/2, -SIZE);
			
    cairo_stroke (cr);
    
    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
