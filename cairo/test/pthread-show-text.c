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

/* Test case for bug #4299:

   Assertion fails in "cairo-font.c" when using multithreads
   https://bugs.freedesktop.org/show_bug.cgi?id=4299
*/

#include "cairo-test.h"
#include "xmalloc.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#if HAVE_FCFINI
#include <fontconfig/fontconfig.h>
#endif

static void *
start (void *closure)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    int i;

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
    cr = cairo_create (surface);

    cairo_save (cr);
    {
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
    }
    cairo_restore (cr);

    cairo_move_to (cr, 1, 1);

    for (i=0; i < 10; i++) {
	cairo_set_font_size (cr, 8 + i);
	cairo_show_text (cr, "Hello world.\n");
    }

    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    return NULL;
}

int
main (int argc, char *argv[0])
{
    int err;
    int i, num_threads;
    pthread_t *pthread;

    if (argc > 1) {
	num_threads = atoi (argv[1]);
    } else {
	num_threads = 20;
    }

    cairo_test_init ("pthread-show-text");

    cairo_test_log ("Running with %d threads.\n", num_threads);

    pthread = xmalloc (num_threads * sizeof (pthread_t));

    for (i = 0; i < num_threads; i++) {
	err = pthread_create (&pthread[i], NULL, start, NULL);
	if (err) {
	    cairo_test_log ("pthread_create failed: %s\n", strerror(err));
	    return CAIRO_TEST_FAILURE;
	}
    }

    for (i = 0; i < num_threads; i++)
	pthread_join (pthread[i], NULL);

    free (pthread);

    cairo_debug_reset_static_data ();
#if HAVE_FCFINI
    FcFini ();
#endif

    return CAIRO_TEST_SUCCESS;
}
