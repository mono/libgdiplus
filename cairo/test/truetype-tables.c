/*
 * Copyright © 2006 Red Hat, Inc.
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
 * Author: Behdad Esfahbod <behdad@behdad.org>
 */
/*
 * Test that the structs we define for TrueType tables have the
 * correct size, ie. they are not padded.
 */

#include "cairo-test.h"

#include <cairo-truetype-subset-private.h>

int
main (void)
{
    cairo_test_status_t ret = CAIRO_TEST_SUCCESS;

    cairo_test_init ("truetype-tables");

    cairo_test_log ("Test that the size of TrueType table structs is correct\n");

#define check(st, sz) \
	if (sizeof (st) != (sz)) { \
	    cairo_test_log ("sizeof (%s): got %d, expected %d", #st, (int)sizeof (st), sz); \
	    ret = CAIRO_TEST_FAILURE; \
	}

    check (tt_head_t,	54);
    check (tt_hhea_t,	36);
    check (tt_maxp_t,	32);
    check (tt_name_record_t,	12);
    check (tt_name_t,	18);
    check (tt_name_t,	18);
    check (tt_composite_glyph_t,	18);
    check (tt_glyph_data_t,	28);

    cairo_test_fini ();

    return ret;
}
