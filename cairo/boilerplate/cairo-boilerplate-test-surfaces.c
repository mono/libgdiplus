/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 8; -*- */
/*
 * Copyright © 2004,2006 Red Hat, Inc.
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

#include "cairo-boilerplate.h"
#include "cairo-boilerplate-test-surfaces-private.h"

#include <test-fallback-surface.h>
#include <test-meta-surface.h>
#include <test-paginated-surface.h>

#include <assert.h>

cairo_surface_t *
_cairo_boilerplate_test_fallback_create_surface (const char			 *name,
						 cairo_content_t		  content,
						 int				  width,
						 int				  height,
						 cairo_boilerplate_mode_t	  mode,
						 void				**closure)
{
    *closure = NULL;
    return _cairo_test_fallback_surface_create (content, width, height);
}

cairo_surface_t *
_cairo_boilerplate_test_meta_create_surface (const char			 *name,
					     cairo_content_t		  content,
					     int			  width,
					     int			  height,
					     cairo_boilerplate_mode_t	  mode,
					     void			**closure)
{
    *closure = NULL;
    return _cairo_test_meta_surface_create (content, width, height);
}

static const cairo_user_data_key_t test_paginated_closure_key;

typedef struct {
    unsigned char *data;
    cairo_content_t content;
    int width;
    int height;
    int stride;
} test_paginated_closure_t;

cairo_surface_t *
_cairo_boilerplate_test_paginated_create_surface (const char			 *name,
						  cairo_content_t		  content,
						  int				  width,
						  int				  height,
						  cairo_boilerplate_mode_t	  mode,
						  void				**closure)
{
    test_paginated_closure_t *tpc;
    cairo_surface_t *surface;

    *closure = tpc = xmalloc (sizeof (test_paginated_closure_t));

    tpc->content = content;
    tpc->width = width;
    tpc->height = height;
    tpc->stride = width * 4;

    tpc->data = xcalloc (tpc->stride * height, 1);

    surface = _cairo_test_paginated_surface_create_for_data (tpc->data,
						       tpc->content,
						       tpc->width,
						       tpc->height,
						       tpc->stride);

    cairo_boilerplate_surface_set_user_data (surface,
					     &test_paginated_closure_key,
					     tpc, NULL);

    return surface;
}

/* The only reason we go through all these machinations to write a PNG
 * image is to _really ensure_ that the data actually landed in our
 * buffer through the paginated surface to the test_paginated_surface.
 *
 * If we didn't implement this function then the default
 * cairo_surface_write_to_png would result in the paginated_surface's
 * acquire_source_image function replaying the meta-surface to an
 * intermediate image surface. And in that case the
 * test_paginated_surface would not be involved and wouldn't be
 * tested.
 */
cairo_status_t
_cairo_boilerplate_test_paginated_surface_write_to_png (cairo_surface_t	*surface,
						        const char	*filename)
{
    cairo_surface_t *image;
    cairo_format_t format;
    test_paginated_closure_t *tpc;
    cairo_status_t status;

    /* show page first.  the automatic show_page is too late for us */
    /* XXX use cairo_surface_show_page() when that's added */
    cairo_t *cr = cairo_create (surface);
    cairo_show_page (cr);
    cairo_destroy (cr);

    tpc = cairo_surface_get_user_data (surface, &test_paginated_closure_key);

    format = cairo_boilerplate_format_from_content (tpc->content);

    image = cairo_image_surface_create_for_data (tpc->data,
						 format,
						 tpc->width,
						 tpc->height,
						 tpc->stride);

    status = cairo_surface_write_to_png (image, filename);
    if (status) {
	CAIRO_BOILERPLATE_LOG ("Error writing %s: %s. Exiting\n",
			       filename,
			       cairo_status_to_string (status));
	exit (1);
    }

    cairo_surface_destroy (image);

    return CAIRO_STATUS_SUCCESS;
}

void
_cairo_boilerplate_test_paginated_cleanup (void *closure)
{
    test_paginated_closure_t *tpc = closure;

    free (tpc->data);
    free (tpc);
}
