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
#include "cairo-boilerplate-pdf.h"
#include "cairo-boilerplate-pdf-private.h"

#include <cairo-pdf.h>
#include <cairo-pdf-surface-private.h>
#include <cairo-paginated-surface-private.h>

cairo_user_data_key_t pdf_closure_key;

typedef struct _pdf_target_closure
{
    char		*filename;
    int			 width;
    int			 height;
    cairo_surface_t	*target;
} pdf_target_closure_t;

#define ARRAY_LENGTH(__array) ((int) (sizeof (__array) / sizeof (__array[0])))

cairo_surface_t *
_cairo_boilerplate_pdf_create_surface (const char		 *name,
				       cairo_content_t		  content,
				       int			  width,
				       int			  height,
				       cairo_boilerplate_mode_t	  mode,
				       void			**closure)
{
    pdf_target_closure_t *ptc;
    cairo_surface_t *surface;

    /* Sanitize back to a real cairo_content_t value. */
    if (content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED)
	content = CAIRO_CONTENT_COLOR_ALPHA;

    *closure = ptc = xmalloc (sizeof (pdf_target_closure_t));

    ptc->width = width;
    ptc->height = height;

    xasprintf (&ptc->filename, "%s-pdf-%s-out.pdf",
	       name, cairo_boilerplate_content_name (content));

    surface = cairo_pdf_surface_create (ptc->filename, width, height);
    if (cairo_surface_status (surface)) {
	free (ptc->filename);
	free (ptc);
	return NULL;
    }
    cairo_surface_set_fallback_resolution (surface, 72., 72.);

    if (content == CAIRO_CONTENT_COLOR) {
	ptc->target = surface;
	surface = cairo_surface_create_similar (ptc->target,
						CAIRO_CONTENT_COLOR,
						width, height);
    } else {
	ptc->target = NULL;
    }

    cairo_boilerplate_surface_set_user_data (surface,
					     &pdf_closure_key,
					     ptc, NULL);

    return surface;
}

cairo_status_t
_cairo_boilerplate_pdf_surface_write_to_png (cairo_surface_t *surface, const char *filename)
{
    pdf_target_closure_t *ptc = cairo_surface_get_user_data (surface, &pdf_closure_key);
    char    command[4096];

    /* Both surface and ptc->target were originally created at the
     * same dimensions. We want a 1:1 copy here, so we first clear any
     * device offset on surface.
     *
     * In a more realistic use case of device offsets, the target of
     * this copying would be of a different size than the source, and
     * the offset would be desirable during the copy operation. */
    cairo_surface_set_device_offset (surface, 0, 0);

    if (ptc->target) {
	cairo_t *cr;
	cr = cairo_create (ptc->target);
	cairo_set_source_surface (cr, surface, 0, 0);
	cairo_paint (cr);
	cairo_show_page (cr);
	cairo_destroy (cr);

	cairo_surface_finish (surface);
	surface = ptc->target;
    }

    cairo_surface_finish (surface);
    sprintf (command, "./pdf2png %s %s 1",
	     ptc->filename, filename);

    if (system (command) != 0)
	return CAIRO_STATUS_WRITE_ERROR;

    return CAIRO_STATUS_SUCCESS;
}

void
_cairo_boilerplate_pdf_cleanup (void *closure)
{
    pdf_target_closure_t *ptc = closure;
    if (ptc->target)
	cairo_surface_destroy (ptc->target);
    free (ptc->filename);
    free (ptc);
}

cairo_status_t
cairo_boilerplate_pdf_surface_force_fallbacks (cairo_surface_t *abstract_surface)
{
    cairo_paginated_surface_t *paginated = (cairo_paginated_surface_t*) abstract_surface;
    cairo_pdf_surface_t *surface;

    if (cairo_surface_get_type (abstract_surface) != CAIRO_SURFACE_TYPE_PDF)
	return CAIRO_STATUS_SURFACE_TYPE_MISMATCH;

    surface = (cairo_pdf_surface_t*) paginated->target;

    surface->force_fallbacks = TRUE;

    return CAIRO_STATUS_SUCCESS;
}
