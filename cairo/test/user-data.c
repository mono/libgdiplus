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
 * Author: Kristian Høgsberg <krh@redhat.com>
 */

#include <cairo.h>
#include <assert.h>
#include <stdlib.h>

static void
destroy_data1 (void *p)
{
    *(int *) p = 1;
}

static void
destroy_data2 (void *p)
{
    *(int *) p = 2;
}

int
main (void)
{
    cairo_surface_t *surface;
    static const cairo_user_data_key_t key1, key2;
    int data1, data2;

    data1 = 0;
    data2 = 0;
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
    assert (cairo_surface_set_user_data (surface, &key1, &data1, destroy_data1)
	    == CAIRO_STATUS_SUCCESS);
    assert (cairo_surface_set_user_data (surface, &key2, &data2, destroy_data2)
	    == CAIRO_STATUS_SUCCESS);
    assert (cairo_surface_get_user_data (surface, &key1) == &data1);
    assert (cairo_surface_set_user_data (surface, &key1, NULL, NULL)
	    == CAIRO_STATUS_SUCCESS);
    assert (cairo_surface_get_user_data (surface, &key1) == NULL);
    assert (data1 == 1);
    assert (data2 == 0);

    assert (cairo_surface_set_user_data (surface, &key2, NULL, NULL)
	    == CAIRO_STATUS_SUCCESS);
    assert (data2 == 2);
    
    data1 = 0;
    assert (cairo_surface_set_user_data (surface, &key1, &data1, NULL)
	    == CAIRO_STATUS_SUCCESS);
    assert (cairo_surface_set_user_data (surface, &key1, NULL, NULL)
	    == CAIRO_STATUS_SUCCESS);
    assert (data1 == 0);
    assert (cairo_surface_get_user_data (surface, &key1) == NULL);

    assert (cairo_surface_set_user_data (surface, &key1, &data1, destroy_data1)
	    == CAIRO_STATUS_SUCCESS);
    cairo_surface_destroy (surface);
    assert (data1 == 1);
    assert (data2 == 2);

    return 0;
}
