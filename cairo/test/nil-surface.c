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
#include <stddef.h>

/* Test to verify fixes for the following similar bugs:
 *
 *	https://bugs.freedesktop.org/show_bug.cgi?id=4088
 *	https://bugs.freedesktop.org/show_bug.cgi?id=3915
 */

cairo_test_t test = {
    "nil-surface",
    "Test that nil surfaces do not make cairo crash.",
    1, 1
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_pattern_t *pattern;
    cairo_t *cr2;

    /*
     * 1. Test file-not-found from surface->pattern->cairo_t
     */

    /* Make a custom context to not interfere with the one passed in. */
    cr2 = cairo_create (cairo_get_target (cr));

    /* First, let's make a nil surface. */
    surface = cairo_image_surface_create_from_png ("___THIS_FILE_DOES_NOT_EXIST___");

    /* Let the error propagate into a nil pattern. */
    pattern = cairo_pattern_create_for_surface (surface);

    /* Then let it propagate into the cairo_t. */
    cairo_set_source (cr2, pattern);
    cairo_paint (cr2);

    cairo_pattern_destroy (pattern);
    cairo_surface_destroy (surface);

    /* Check that the error made it all that way. */
    if (cairo_status (cr2) != CAIRO_STATUS_FILE_NOT_FOUND) {
	cairo_test_log ("Error: Received status of \"%s\" rather than expected \"%s\"\n",
			cairo_status_to_string (cairo_status (cr2)),
			cairo_status_to_string (CAIRO_STATUS_FILE_NOT_FOUND));
	return CAIRO_TEST_FAILURE;
    }

    cairo_destroy (cr2);

    /*
     * 2. Test NULL pointer pattern->cairo_t
     */
    cr2 = cairo_create (cairo_get_target (cr));

    /* First, trigger the NULL pointer status. */
    pattern = cairo_pattern_create_for_surface (NULL);

    /* Then let it propagate into the cairo_t. */
    cairo_set_source (cr2, pattern);
    cairo_paint (cr2);

    cairo_pattern_destroy (pattern);

    /* Check that the error made it all that way. */
    if (cairo_status (cr2) != CAIRO_STATUS_NULL_POINTER) {
	cairo_test_log ("Error: Received status of \"%s\" rather than expected \"%s\"\n",
			cairo_status_to_string (cairo_status (cr2)),
			cairo_status_to_string (CAIRO_STATUS_NULL_POINTER));
	return CAIRO_TEST_FAILURE;
    }

    cairo_destroy (cr2);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
