/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2002 University of Southern California
 * Copyright © 2005 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is University of Southern
 * California.
 *
 * Contributor(s):
 *	Carl D. Worth <cworth@cworth.org>
 */

#include <stdlib.h>

#include "cairoint.h"
#include "cairo-gstate-private.h"

const cairo_surface_t _cairo_surface_nil = {
    &cairo_image_surface_backend,	/* backend */
    -1,					/* ref_count */
    CAIRO_STATUS_NO_MEMORY,		/* status */
    FALSE,				/* finished */
    { 0,	/* size */
      0,	/* num_elements */
      0,	/* element_size */
      NULL,	/* elements */
    },					/* user_data */
    0.0,				/* device_x_offset */
    0.0,				/* device_y_offset */
    0,					/* next_clip_serial */
    0					/* current_clip_serial */
};

const cairo_surface_t _cairo_surface_nil_file_not_found = {
    &cairo_image_surface_backend,	/* backend */
    -1,					/* ref_count */
    CAIRO_STATUS_FILE_NOT_FOUND,	/* status */
    FALSE,				/* finished */
    { 0,	/* size */
      0,	/* num_elements */
      0,	/* element_size */
      NULL,	/* elements */
    },					/* user_data */
    0.0,				/* device_x_offset */
    0.0,				/* device_y_offset */
    0,					/* next_clip_serial */
    0					/* current_clip_serial */
};

const cairo_surface_t _cairo_surface_nil_read_error = {
    &cairo_image_surface_backend,	/* backend */
    -1,					/* ref_count */
    CAIRO_STATUS_READ_ERROR,		/* status */
    FALSE,				/* finished */
    { 0,	/* size */
      0,	/* num_elements */
      0,	/* element_size */
      NULL,	/* elements */
    },					/* user_data */
    0.0,				/* device_x_offset */
    0.0,				/* device_y_offset */
    0,					/* next_clip_serial */
    0					/* current_clip_serial */
};

/**
 * _cairo_surface_set_error:
 * @surface: a surface
 * @status: a status value indicating an error, (eg. not
 * CAIRO_STATUS_SUCCESS)
 * 
 * Sets surface->status to @status and calls _cairo_error;
 *
 * All assignments of an error status to surface->status should happen
 * through _cairo_surface_set_error() or else _cairo_error() should be
 * called immediately after the assignment.
 *
 * The purpose of this function is to allow the user to set a
 * breakpoint in _cairo_error() to generate a stack trace for when the
 * user causes cairo to detect an error.
 **/
static void
_cairo_surface_set_error (cairo_surface_t *surface,
			  cairo_status_t status)
{
    /* Don't overwrite an existing error. This preserves the first
     * error, which is the most significant. It also avoids attempting
     * to write to read-only data (eg. from a nil surface). */
    if (surface->status == CAIRO_STATUS_SUCCESS)
	surface->status = status;

    _cairo_error (status);
}

/**
 * cairo_surface_status:
 * @surface: a #cairo_surface_t
 * 
 * Checks whether an error has previously occurred for this
 * surface.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS, %CAIRO_STATUS_NULL_POINTER,
 * %CAIRO_STATUS_NO_MEMORY, %CAIRO_STATUS_READ_ERROR,
 * %CAIRO_STATUS_INVALID_CONTENT, %CAIRO_STATUS_INVALUE_FORMAT, or
 * %CAIRO_STATUS_INVALID_VISUAL.
 **/
cairo_status_t
cairo_surface_status (cairo_surface_t *surface)
{
    return surface->status;
}

void
_cairo_surface_init (cairo_surface_t			*surface,
		     const cairo_surface_backend_t	*backend)
{
    surface->backend = backend;

    surface->ref_count = 1;
    surface->status = CAIRO_STATUS_SUCCESS;
    surface->finished = FALSE;

    _cairo_user_data_array_init (&surface->user_data);

    surface->device_x_offset = 0.0;
    surface->device_y_offset = 0.0;
    surface->device_x_scale = 1.0;
    surface->device_y_scale = 1.0;

    surface->next_clip_serial = 0;
    surface->current_clip_serial = 0;
}

cairo_surface_t *
_cairo_surface_create_similar_scratch (cairo_surface_t *other,
				       cairo_content_t	content,
				       int		width,
				       int		height)
{
    cairo_format_t format = _cairo_format_from_content (content);

    if (other->status)
	return (cairo_surface_t*) &_cairo_surface_nil;

    if (other->backend->create_similar)
	return other->backend->create_similar (other, content, width, height);
    else
	return cairo_image_surface_create (format, width, height);
}

/**
 * cairo_surface_create_similar:
 * @other: an existing surface used to select the backend of the new surface
 * @content: the content for the new surface
 * @width: width of the new surface, (in device-space units)
 * @height: height of the new surface (in device-space units)
 * 
 * Create a new surface that is as compatible as possible with an
 * existing surface. The new surface will use the same backend as
 * @other unless that is not possible for some reason.
 * 
 * Return value: a pointer to the newly allocated surface. The caller
 * owns the surface and should call cairo_surface_destroy when done
 * with it.
 *
 * This function always returns a valid pointer, but it will return a
 * pointer to a "nil" surface if @other is already in an error state
 * or any other error occurs.
 **/
cairo_surface_t *
cairo_surface_create_similar (cairo_surface_t  *other,
			      cairo_content_t	content,
			      int		width,
			      int		height)
{
    if (other->status)
	return (cairo_surface_t*) &_cairo_surface_nil;

    if (! CAIRO_CONTENT_VALID (content)) {
	_cairo_error (CAIRO_STATUS_INVALID_CONTENT);
	return (cairo_surface_t*) &_cairo_surface_nil;
    }

    return _cairo_surface_create_similar_solid (other, content,
						width, height,
						CAIRO_COLOR_TRANSPARENT);
}

cairo_surface_t *
_cairo_surface_create_similar_solid (cairo_surface_t	 *other,
				     cairo_content_t	  content,
				     int		  width,
				     int		  height,
				     const cairo_color_t *color)
{
    cairo_status_t status;
    cairo_surface_t *surface;

    surface = _cairo_surface_create_similar_scratch (other, content,
						     width, height);
    if (surface->status) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_surface_t*) &_cairo_surface_nil;
    }
    
    status = _cairo_surface_fill_rectangle (surface,
					    CAIRO_OPERATOR_SOURCE, color,
					    0, 0, width, height);
    if (status) {
	cairo_surface_destroy (surface);
	_cairo_error (status);
	return (cairo_surface_t*) &_cairo_surface_nil;
    }

    return surface;
}

cairo_clip_mode_t
_cairo_surface_get_clip_mode (cairo_surface_t *surface)
{
    if (surface->backend->intersect_clip_path != NULL)
	return CAIRO_CLIP_MODE_PATH;
    else if (surface->backend->set_clip_region != NULL)
	return CAIRO_CLIP_MODE_REGION;
    else
	return CAIRO_CLIP_MODE_MASK;
}

/**
 * cairo_surface_reference:
 * @surface: a #cairo_surface_t
 * 
 * Increases the reference count on @surface by one. This prevents
 * @surface from being destroyed until a matching call to
 * cairo_surface_destroy() is made.
 *
 * Return value: the referenced #cairo_surface_t.
 **/
cairo_surface_t *
cairo_surface_reference (cairo_surface_t *surface)
{
    if (surface == NULL)
	return NULL;

    if (surface->ref_count == (unsigned int)-1)
	return surface;

    assert (surface->ref_count > 0);

    surface->ref_count++;

    return surface;
}

/**
 * cairo_surface_destroy:
 * @surface: a #cairo_t
 * 
 * Decreases the reference count on @surface by one. If the result is
 * zero, then @surface and all associated resources are freed.  See
 * cairo_surface_reference().
 **/
void
cairo_surface_destroy (cairo_surface_t *surface)
{
    if (surface == NULL)
	return;

    if (surface->ref_count == (unsigned int)-1)
	return;

    assert (surface->ref_count > 0);

    surface->ref_count--;
    if (surface->ref_count)
	return;

    cairo_surface_finish (surface);

    _cairo_user_data_array_fini (&surface->user_data);

    free (surface);
}
slim_hidden_def(cairo_surface_destroy);

/**
 * cairo_surface_finish:
 * @surface: the #cairo_surface_t to finish
 * 
 * This function finishes the surface and drops all references to
 * external resources.  For example, for the Xlib backend it means
 * that cairo will no longer access the drawable, which can be freed.
 * After calling cairo_surface_finish() the only valid operations on a
 * surface are getting and setting user data and referencing and
 * destroying it.  Further drawing to the surface will not affect the
 * surface but will instead trigger a CAIRO_STATUS_SURFACE_FINISHED
 * error.
 *
 * When the last call to cairo_surface_destroy() decreases the
 * reference count to zero, cairo will call cairo_surface_finish() if
 * it hasn't been called already, before freeing the resources
 * associated with the surface.
 **/
void
cairo_surface_finish (cairo_surface_t *surface)
{
    cairo_status_t status;

    if (surface->finished) {
	_cairo_surface_set_error (surface, CAIRO_STATUS_SURFACE_FINISHED);
	return;
    }

    if (surface->backend->finish == NULL) {
	surface->finished = TRUE;
	return;
    }

    if (!surface->status && surface->backend->flush) {
	status = surface->backend->flush (surface);
	if (status) {
	    _cairo_surface_set_error (surface, status);
	    return;
	}
    }

    status = surface->backend->finish (surface);
    if (status) {
	_cairo_surface_set_error (surface, status);
	return;
    }

    surface->finished = TRUE;
}

/**
 * cairo_surface_get_user_data:
 * @surface: a #cairo_surface_t
 * @key: the address of the #cairo_user_data_key_t the user data was
 * attached to
 * 
 * Return user data previously attached to @surface using the specified
 * key.  If no user data has been attached with the given key this
 * function returns %NULL.
 * 
 * Return value: the user data previously attached or %NULL.
 **/
void *
cairo_surface_get_user_data (cairo_surface_t		 *surface,
			     const cairo_user_data_key_t *key)
{
    return _cairo_user_data_array_get_data (&surface->user_data,
					    key);
}

/**
 * cairo_surface_set_user_data:
 * @surface: a #cairo_surface_t
 * @key: the address of a #cairo_user_data_key_t to attach the user data to
 * @user_data: the user data to attach to the surface
 * @destroy: a #cairo_destroy_func_t which will be called when the
 * surface is destroyed or when new user data is attached using the
 * same key.
 * 
 * Attach user data to @surface.  To remove user data from a surface,
 * call this function with the key that was used to set it and %NULL
 * for @data.
 *
 * Return value: %CAIRO_STATUS_SUCCESS or %CAIRO_STATUS_NO_MEMORY if a
 * slot could not be allocated for the user data.
 **/
cairo_status_t
cairo_surface_set_user_data (cairo_surface_t		 *surface,
			     const cairo_user_data_key_t *key,
			     void			 *user_data,
			     cairo_destroy_func_t	 destroy)
{
    if (surface->ref_count == -1)
	return CAIRO_STATUS_NO_MEMORY;
    
    return _cairo_user_data_array_set_data (&surface->user_data,
					    key, user_data, destroy);
}

/**
 * cairo_surface_get_font_options:
 * @surface: a #cairo_surface_t
 * @options: a #cairo_font_options_t object into which to store
 *   the retrieved options. All existing values are overwritten
 * 
 * Retrieves the default font rendering options for the surface.
 * This allows display surfaces to report the correct subpixel order
 * for rendering on them, print surfaces to disable hinting of
 * metrics and so forth. The result can then be used with
 * cairo_scaled_font_create().
 **/
void
cairo_surface_get_font_options (cairo_surface_t       *surface,
				cairo_font_options_t  *options)
{
    if (!surface->finished && surface->backend->get_font_options) {
	surface->backend->get_font_options (surface, options);
    } else {
	_cairo_font_options_init_default (options);
    }
}

/**
 * cairo_surface_flush:
 * @surface: a #cairo_surface_t
 * 
 * Do any pending drawing for the surface and also restore any
 * temporary modification's cairo has made to the surface's
 * state. This function must be called before switching from
 * drawing on the surface with cairo to drawing on it directly
 * with native APIs. If the surface doesn't support direct access,
 * then this function does nothing.
 **/
void
cairo_surface_flush (cairo_surface_t *surface)
{
    if (surface->status)
	return;

    if (surface->finished) {
	_cairo_surface_set_error (surface, CAIRO_STATUS_SURFACE_FINISHED);
	return;
    }

    if (surface->backend->flush) {
	cairo_status_t status;

	status = surface->backend->flush (surface);
	
	if (status)
	    _cairo_surface_set_error (surface, status);
    }
}

/**
 * cairo_surface_mark_dirty:
 * @surface: a #cairo_surface_t
 *
 * Tells cairo that drawing has been done to surface using means other
 * than cairo, and that cairo should reread any cached areas. Note
 * that you must call cairo_surface_flush() before doing such drawing.
 */
void
cairo_surface_mark_dirty (cairo_surface_t *surface)
{
    cairo_surface_mark_dirty_rectangle (surface, 0, 0, -1, -1);
}

/**
 * cairo_surface_mark_dirty_rectangle:
 * @surface: a #cairo_surface_t
 * @x: X coordinate of dirty rectangle
 * @y: Y coordinate of dirty rectangle
 * @width: width of dirty rectangle
 * @height: height of dirty rectangle
 *
 * Like cairo_surface_mark_dirty(), but drawing has been done only to
 * the specified rectangle, so that cairo can retain cached contents
 * for other parts of the surface.
 */
void
cairo_surface_mark_dirty_rectangle (cairo_surface_t *surface,
				    int              x,
				    int              y,
				    int              width,
				    int              height)
{
    if (surface->status)
	return;

    if (surface->finished) {
	_cairo_surface_set_error (surface, CAIRO_STATUS_SURFACE_FINISHED);
	return;
    }

    if (surface->backend->mark_dirty_rectangle) {
	cairo_status_t status;
	
	status = surface->backend->mark_dirty_rectangle (surface, x, y, width, height);
	
	if (status)
	    _cairo_surface_set_error (surface, status);
    }
}

/**
 * cairo_surface_set_device_offset:
 * @surface: a #cairo_surface_t
 * @x_offset: the offset in the X direction, in device units
 * @y_offset: the offset in the Y direction, in device units
 * 
 * Sets an offset that is added to the device coordinates determined
 * by the CTM when drawing to @surface. One use case for this function
 * is when we want to create a #cairo_surface_t that redirects drawing
 * for a portion of an onscreen surface to an offscreen surface in a
 * way that is completely invisible to the user of the cairo
 * API. Setting a transformation via cairo_translate() isn't
 * sufficient to do this, since functions like
 * cairo_device_to_user() will expose the hidden offset.
 *
 * Note that the offset only affects drawing to the surface, not using
 * the surface in a surface pattern.
 **/
void
cairo_surface_set_device_offset (cairo_surface_t *surface,
				 double           x_offset,
				 double           y_offset)
{
    if (surface->status)
	return;

    if (surface->finished) {
	_cairo_surface_set_error (surface, CAIRO_STATUS_SURFACE_FINISHED);
	return;
    }

    surface->device_x_offset = x_offset * surface->device_x_scale;
    surface->device_y_offset = y_offset * surface->device_y_scale;
}

/**
 * _cairo_surface_acquire_source_image:
 * @surface: a #cairo_surface_t
 * @image_out: location to store a pointer to an image surface that
 *    has identical contents to @surface. This surface could be @surface
 *    itself, a surface held internal to @surface, or it could be a new
 *    surface with a copy of the relevant portion of @surface.
 * @image_extra: location to store image specific backend data
 * 
 * Gets an image surface to use when drawing as a fallback when drawing with
 * @surface as a source. _cairo_surface_release_source_image() must be called
 * when finished.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS if a an image was stored in @image_out.
 * %CAIRO_INT_STATUS_UNSUPPORTED if an image cannot be retrieved for the specified
 * surface. Or %CAIRO_STATUS_NO_MEMORY.
 **/
cairo_status_t
_cairo_surface_acquire_source_image (cairo_surface_t         *surface,
				     cairo_image_surface_t  **image_out,
				     void                   **image_extra)
{
    assert (!surface->finished);

    return surface->backend->acquire_source_image (surface,  image_out, image_extra);
}

/**
 * _cairo_surface_release_source_image:
 * @surface: a #cairo_surface_t
 * @image_extra: same as return from the matching _cairo_surface_acquire_source_image()
 * 
 * Releases any resources obtained with _cairo_surface_acquire_source_image()
 **/
void
_cairo_surface_release_source_image (cairo_surface_t        *surface,
				     cairo_image_surface_t  *image,
				     void                   *image_extra)
{
    assert (!surface->finished);

    if (surface->backend->release_source_image)
	surface->backend->release_source_image (surface, image, image_extra);
}

/**
 * _cairo_surface_acquire_dest_image:
 * @surface: a #cairo_surface_t
 * @interest_rect: area of @surface for which fallback drawing is being done.
 *    A value of %NULL indicates that the entire surface is desired.
 * @image_out: location to store a pointer to an image surface that includes at least
 *    the intersection of @interest_rect with the visible area of @surface.
 *    This surface could be @surface itself, a surface held internal to @surface,
 *    or it could be a new surface with a copy of the relevant portion of @surface.
 *    If a new surface is created, it should have the same channels and depth
 *    as @surface so that copying to and from it is exact.
 * @image_rect: location to store area of the original surface occupied 
 *    by the surface stored in @image.
 * @image_extra: location to store image specific backend data
 * 
 * Retrieves a local image for a surface for implementing a fallback drawing
 * operation. After calling this function, the implementation of the fallback
 * drawing operation draws the primitive to the surface stored in @image_out
 * then calls _cairo_surface_release_dest_image(),
 * which, if a temporary surface was created, copies the bits back to the
 * main surface and frees the temporary surface.
 *
 * Return value: %CAIRO_STATUS_SUCCESS or %CAIRO_STATUS_NO_MEMORY.
 *  %CAIRO_INT_STATUS_UNSUPPORTED can be returned but this will mean that
 *  the backend can't draw with fallbacks. It's possible for the routine
 *  to store NULL in @local_out and return %CAIRO_STATUS_SUCCESS;
 *  that indicates that no part of @interest_rect is visible, so no drawing
 *  is necessary. _cairo_surface_release_dest_image() should not be called in that
 *  case.
 **/
cairo_status_t
_cairo_surface_acquire_dest_image (cairo_surface_t         *surface,
				   cairo_rectangle_t       *interest_rect,
				   cairo_image_surface_t  **image_out,
				   cairo_rectangle_t       *image_rect,
				   void                   **image_extra)
{
    assert (!surface->finished);

    return surface->backend->acquire_dest_image (surface, interest_rect,
						 image_out, image_rect, image_extra);
}

/**
 * _cairo_surface_release_dest_image:
 * @surface: a #cairo_surface_t
 * @interest_rect: same as passed to the matching _cairo_surface_acquire_dest_image()
 * @image: same as returned from the matching _cairo_surface_acquire_dest_image()
 * @image_rect: same as returned from the matching _cairo_surface_acquire_dest_image()
 * @image_extra: same as return from the matching _cairo_surface_acquire_dest_image()
 * 
 * Finishes the operation started with _cairo_surface_acquire_dest_image(), by, if
 * necessary, copying the image from @image back to @surface and freeing any
 * resources that were allocated.
 **/
void
_cairo_surface_release_dest_image (cairo_surface_t        *surface,
				   cairo_rectangle_t      *interest_rect,
				   cairo_image_surface_t  *image,
				   cairo_rectangle_t      *image_rect,
				   void                   *image_extra)
{
    assert (!surface->finished);

    if (surface->backend->release_dest_image)
	surface->backend->release_dest_image (surface, interest_rect,
					      image, image_rect, image_extra);
}

/**
 * _cairo_surface_clone_similar:
 * @surface: a #cairo_surface_t
 * @src: the source image
 * @clone_out: location to store a surface compatible with @surface
 *   and with contents identical to @src. The caller must call
 *   cairo_surface_destroy() on the result.
 * 
 * Creates a surface with contents identical to @src but that
 *   can be used efficiently with @surface. If @surface and @src are
 *   already compatible then it may return a new reference to @src.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS if a surface was created and stored
 *   in @clone_out. Otherwise %CAIRO_INT_STATUS_UNSUPPORTED or another
 *   error like %CAIRO_STATUS_NO_MEMORY.
 **/
cairo_status_t
_cairo_surface_clone_similar (cairo_surface_t  *surface,
			      cairo_surface_t  *src,
			      cairo_surface_t **clone_out)
{
    cairo_status_t status;
    cairo_image_surface_t *image;
    void *image_extra;
    
    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    if (surface->backend->clone_similar == NULL)
	return CAIRO_INT_STATUS_UNSUPPORTED;
      
    status = surface->backend->clone_similar (surface, src, clone_out);
    if (status != CAIRO_INT_STATUS_UNSUPPORTED)
	return status;

    status = _cairo_surface_acquire_source_image (src, &image, &image_extra);
    if (status != CAIRO_STATUS_SUCCESS)
	return status;
    
    status = surface->backend->clone_similar (surface, &image->base, clone_out);

    /* If the above failed point, we could implement a full fallback
     * using acquire_dest_image, but that's going to be very
     * inefficient compared to a backend-specific implementation of
     * clone_similar() with an image source. So we don't bother
     */
    
    _cairo_surface_release_source_image (src, image, image_extra);
    return status;
}

typedef struct {
    cairo_surface_t *dst;
    cairo_rectangle_t extents;
    cairo_image_surface_t *image;
    cairo_rectangle_t image_rect;
    void *image_extra;
} fallback_state_t;

/**
 * _fallback_init:
 * 
 * Acquire destination image surface needed for an image-based
 * fallback.
 * 
 * Return value: CAIRO_INT_STATUS_NOTHING_TO_DO if the extents are not
 * visible, CAIRO_STATUS_SUCCESS if some portion is visible and all
 * went well, or some error status otherwise.
 **/
static cairo_int_status_t
_fallback_init (fallback_state_t *state,
		cairo_surface_t  *dst,
		int               x,
		int               y,
		int               width,
		int               height)
{
    cairo_status_t status;

    state->extents.x = x;
    state->extents.y = y;
    state->extents.width = width;
    state->extents.height = height;
    
    state->dst = dst;

    status = _cairo_surface_acquire_dest_image (dst, &state->extents,
						&state->image, &state->image_rect,
						&state->image_extra);
    if (status)
	return status;

    /* XXX: This NULL value tucked away in state->image is a rather
     * ugly interface. Cleaner would be to push the
     * CAIRO_INT_STATUS_NOTHING_TO_DO value down into
     * _cairo_surface_acquire_dest_image and its backend
     * counterparts. */
    if (state->image == NULL)
	return CAIRO_INT_STATUS_NOTHING_TO_DO;

    return CAIRO_STATUS_SUCCESS;
}

static void
_fallback_fini (fallback_state_t *state)
{
    _cairo_surface_release_dest_image (state->dst, &state->extents,
				       state->image, &state->image_rect,
				       state->image_extra);
}

static cairo_status_t
_fallback_composite (cairo_operator_t	operator,
		     cairo_pattern_t	*src,
		     cairo_pattern_t	*mask,
		     cairo_surface_t	*dst,
		     int		src_x,
		     int		src_y,
		     int		mask_x,
		     int		mask_y,
		     int		dst_x,
		     int		dst_y,
		     unsigned int	width,
		     unsigned int	height)
{
    fallback_state_t state;
    cairo_status_t status;

    status = _fallback_init (&state, dst, dst_x, dst_y, width, height);
    if (status) {
	if (status == CAIRO_INT_STATUS_NOTHING_TO_DO)
	    return CAIRO_STATUS_SUCCESS;
	return status;
    }

    status = state.image->base.backend->composite (operator, src, mask,
						   &state.image->base,
						   src_x, src_y, mask_x, mask_y,
						   dst_x - state.image_rect.x,
						   dst_y - state.image_rect.y,
						   width, height);
    _fallback_fini (&state);

    return status;
}

cairo_status_t
_cairo_surface_composite (cairo_operator_t	operator,
			  cairo_pattern_t	*src,
			  cairo_pattern_t	*mask,
			  cairo_surface_t	*dst,
			  int			src_x,
			  int			src_y,
			  int			mask_x,
			  int			mask_y,
			  int			dst_x,
			  int			dst_y,
			  unsigned int		width,
			  unsigned int		height)
{
    cairo_int_status_t status;

    if (mask) {
	/* These operators aren't interpreted the same way by the backends;
	 * they are implemented in terms of other operators in cairo-gstate.c
	 */
	assert (operator != CAIRO_OPERATOR_SOURCE && operator != CAIRO_OPERATOR_CLEAR);
    }

    if (dst->status)
	return dst->status;
	
    if (dst->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    if (dst->backend->composite) {
	status = dst->backend->composite (operator,
					  src, mask, dst,
					  src_x, src_y,
					  mask_x, mask_y,
					  dst_x, dst_y,
					  width, height);
	if (status != CAIRO_INT_STATUS_UNSUPPORTED)
	    return status;
    }

    return _fallback_composite (operator,
				src, mask, dst,
				src_x, src_y,
				mask_x, mask_y,
				dst_x, dst_y,
				width, height);
}

/**
 * _cairo_surface_fill_rectangle:
 * @surface: a #cairo_surface_t
 * @operator: the operator to apply to the rectangle
 * @color: the source color
 * @x: X coordinate of rectangle, in backend coordinates
 * @y: Y coordinate of rectangle, in backend coordinates
 * @width: width of rectangle, in backend coordinates
 * @height: height of rectangle, in backend coordinates
 * 
 * Applies an operator to a rectangle using a solid color as the source.
 * See _cairo_surface_fill_rectangles() for full details.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS or the error that occurred
 **/
cairo_status_t
_cairo_surface_fill_rectangle (cairo_surface_t	   *surface,
			       cairo_operator_t	    operator,
			       const cairo_color_t *color,
			       int		    x,
			       int		    y,
			       int		    width,
			       int		    height)
{
    cairo_rectangle_t rect;

    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;

    return _cairo_surface_fill_rectangles (surface, operator, color, &rect, 1);
}

/**
 * _cairo_surface_fill_region:
 * @surface: a #cairo_surface_t
 * @operator: the operator to apply to the region
 * @color: the source color
 * @region: the region to modify, in backend coordinates
 * 
 * Applies an operator to a set of rectangles specified as a
 * #pixman_region16_t using a solid color as the source.
 * See _cairo_surface_fill_rectangles() for full details.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS or the error that occurred
 **/
cairo_status_t
_cairo_surface_fill_region (cairo_surface_t	   *surface,
			    cairo_operator_t	    operator,
			    const cairo_color_t    *color,
			    pixman_region16_t      *region)
{
    int num_rects = pixman_region_num_rects (region);
    pixman_box16_t *boxes = pixman_region_rects (region);
    cairo_rectangle_t *rects;
    cairo_status_t status;
    int i;

    if (!num_rects)
	return CAIRO_STATUS_SUCCESS;
    
    rects = malloc (sizeof (pixman_rectangle_t) * num_rects);
    if (!rects)
	return CAIRO_STATUS_NO_MEMORY;

    for (i = 0; i < num_rects; i++) {
	rects[i].x = boxes[i].x1;
	rects[i].y = boxes[i].y1;
	rects[i].width = boxes[i].x2 - boxes[i].x1;
	rects[i].height = boxes[i].y2 - boxes[i].y1;
    }

    status =  _cairo_surface_fill_rectangles (surface, operator,
					      color, rects, num_rects);
    
    free (rects);

    return status;
}

static cairo_status_t
_fallback_fill_rectangles (cairo_surface_t	*surface,
			   cairo_operator_t	operator,
			   const cairo_color_t	*color,
			   cairo_rectangle_t	*rects,
			   int			num_rects)
{
    fallback_state_t state;
    cairo_rectangle_t *offset_rects = NULL;
    cairo_status_t status;
    int x1, y1, x2, y2;
    int i;

    if (num_rects <= 0)
	return CAIRO_STATUS_SUCCESS;

    /* Compute the bounds of the rectangles, so that we know what area of the
     * destination surface to fetch
     */
    x1 = rects[0].x;
    y1 = rects[0].y;
    x2 = rects[0].x + rects[0].width;
    y2 = rects[0].y + rects[0].height;
    
    for (i = 1; i < num_rects; i++) {
	if (rects[i].x < x1)
	    x1 = rects[i].x;
	if (rects[i].y < y1)
	    y1 = rects[i].y;

	if (rects[i].x + rects[i].width > x2)
	    x2 = rects[i].x + rects[i].width;
	if (rects[i].y + rects[i].height > y2)
	    y2 = rects[i].y + rects[i].height;
    }

    status = _fallback_init (&state, surface, x1, y1, x2 - x1, y2 - y1);
    if (status) {
	if (status == CAIRO_INT_STATUS_NOTHING_TO_DO)
	    return CAIRO_STATUS_SUCCESS;
	return status;
    }

    /* If the fetched image isn't at 0,0, we need to offset the rectangles */
    
    if (state.image_rect.x != 0 || state.image_rect.y != 0) {
	offset_rects = malloc (sizeof (cairo_rectangle_t) * num_rects);
	if (offset_rects == NULL) {
	    status = CAIRO_STATUS_NO_MEMORY;
	    goto DONE;
	}

	for (i = 0; i < num_rects; i++) {
	    offset_rects[i].x = rects[i].x - state.image_rect.x;
	    offset_rects[i].y = rects[i].y - state.image_rect.y;
	    offset_rects[i].width = rects[i].width;
	    offset_rects[i].height = rects[i].height;
	}

	rects = offset_rects;
    }

    status = state.image->base.backend->fill_rectangles (&state.image->base,
							 operator, color,
							 rects, num_rects);

    free (offset_rects);

 DONE:
    _fallback_fini (&state);

    return status;
}

/**
 * _cairo_surface_fill_rectangles:
 * @surface: a #cairo_surface_t
 * @operator: the operator to apply to the region
 * @color: the source color
 * @rects: the rectangles to modify, in backend coordinates
 * @num_rects: the number of rectangles in @rects
 * 
 * Applies an operator to a set of rectangles using a solid color
 * as the source. Note that even if the operator is an unbounded operator
 * such as %CAIRO_OPERATOR_IN, only the given set of rectangles
 * is affected. This differs from _cairo_surface_composite_trapezoids()
 * where the entire destination rectangle is cleared.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS or the error that occurred
 **/
cairo_status_t
_cairo_surface_fill_rectangles (cairo_surface_t		*surface,
				cairo_operator_t	operator,
				const cairo_color_t	*color,
				cairo_rectangle_t	*rects,
				int			num_rects)
{
    cairo_int_status_t status;

    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    if (num_rects == 0)
	return CAIRO_STATUS_SUCCESS;

    if (surface->backend->fill_rectangles) {
	status = surface->backend->fill_rectangles (surface,
						    operator,
						    color,
						    rects, num_rects);
	if (status != CAIRO_INT_STATUS_UNSUPPORTED)
	    return status;
    }

    return _fallback_fill_rectangles (surface, operator, color, rects, num_rects);
}

cairo_int_status_t
_cairo_surface_fill_path (cairo_operator_t	operator,
			  cairo_pattern_t	*pattern,
			  cairo_surface_t	*dst,
			  cairo_path_fixed_t	*path,
			  cairo_fill_rule_t	fill_rule,
			  double		tolerance)
{
    if (dst->backend->fill_path)
	return dst->backend->fill_path (operator, pattern, dst, path,
					fill_rule, tolerance);
    else
	return CAIRO_INT_STATUS_UNSUPPORTED;
}

  
static cairo_status_t
_fallback_composite_trapezoids (cairo_operator_t	operator,
				cairo_pattern_t		*pattern,
				cairo_surface_t		*dst,
				cairo_antialias_t	antialias,
				int			src_x,
				int			src_y,
				int			dst_x,
				int			dst_y,
				unsigned int		width,
				unsigned int		height,
				cairo_trapezoid_t	*traps,
				int			num_traps)
{
    fallback_state_t state;
    cairo_trapezoid_t *offset_traps = NULL;
    cairo_status_t status;
    int i;

    status = _fallback_init (&state, dst, dst_x, dst_y, width, height);
    if (status) {
	if (status == CAIRO_INT_STATUS_NOTHING_TO_DO)
	    return CAIRO_STATUS_SUCCESS;
	return status;
    }

    /* If the destination image isn't at 0,0, we need to offset the trapezoids */
    
    if (state.image_rect.x != 0 || state.image_rect.y != 0) {

	cairo_fixed_t xoff = _cairo_fixed_from_int (state.image_rect.x);
	cairo_fixed_t yoff = _cairo_fixed_from_int (state.image_rect.y);
	
	offset_traps = malloc (sizeof (cairo_trapezoid_t) * num_traps);
	if (!offset_traps) {
	    status = CAIRO_STATUS_NO_MEMORY;
	    goto DONE;
	}

	for (i = 0; i < num_traps; i++) {
	    offset_traps[i].top = traps[i].top - yoff;
	    offset_traps[i].bottom = traps[i].bottom - yoff;
	    offset_traps[i].left.p1.x = traps[i].left.p1.x - xoff;
	    offset_traps[i].left.p1.y = traps[i].left.p1.y - yoff;
	    offset_traps[i].left.p2.x = traps[i].left.p2.x - xoff;
	    offset_traps[i].left.p2.y = traps[i].left.p2.y - yoff;
	    offset_traps[i].right.p1.x = traps[i].right.p1.x - xoff;
	    offset_traps[i].right.p1.y = traps[i].right.p1.y - yoff;
	    offset_traps[i].right.p2.x = traps[i].right.p2.x - xoff;
	    offset_traps[i].right.p2.y = traps[i].right.p2.y - yoff;
	}

	traps = offset_traps;
    }

    state.image->base.backend->composite_trapezoids (operator, pattern,
						     &state.image->base,
						     antialias,
						     src_x, src_y,
						     dst_x - state.image_rect.x,
						     dst_y - state.image_rect.y,
						     width, height, traps, num_traps);
    if (offset_traps)
	free (offset_traps);

 DONE:
    _fallback_fini (&state);
    
    return status;
}

cairo_status_t
_cairo_surface_composite_trapezoids (cairo_operator_t		operator,
				     cairo_pattern_t		*pattern,
				     cairo_surface_t		*dst,
				     cairo_antialias_t		antialias,
				     int			src_x,
				     int			src_y,
				     int			dst_x,
				     int			dst_y,
				     unsigned int		width,
				     unsigned int		height,
				     cairo_trapezoid_t		*traps,
				     int			num_traps)
{
    cairo_int_status_t status;

    /* These operators aren't interpreted the same way by the backends;
     * they are implemented in terms of other operators in cairo-gstate.c
     */
    assert (operator != CAIRO_OPERATOR_SOURCE && operator != CAIRO_OPERATOR_CLEAR);

    if (dst->status)
	return dst->status;

    if (dst->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    if (dst->backend->composite_trapezoids) {
	status = dst->backend->composite_trapezoids (operator,
						     pattern, dst,
						     antialias,
						     src_x, src_y,
						     dst_x, dst_y,
						     width, height,
						     traps, num_traps);
	if (status != CAIRO_INT_STATUS_UNSUPPORTED)
	    return status;
    }

    return  _fallback_composite_trapezoids (operator, pattern, dst,
					    antialias,
					    src_x, src_y,
					    dst_x, dst_y,
					    width, height,
					    traps, num_traps);
}

cairo_status_t
_cairo_surface_copy_page (cairo_surface_t *surface)
{
    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    /* It's fine if some backends just don't support this. */
    if (surface->backend->copy_page == NULL)
	return CAIRO_STATUS_SUCCESS;

    return surface->backend->copy_page (surface);
}

cairo_status_t
_cairo_surface_show_page (cairo_surface_t *surface)
{
    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    /* It's fine if some backends just don't support this. */
    if (surface->backend->show_page == NULL)
	return CAIRO_STATUS_SUCCESS;

    return surface->backend->show_page (surface);
}

/**
 * _cairo_surface_get_current_clip_serial:
 * @surface: the #cairo_surface_t to return the serial number for
 *
 * Returns the serial number associated with the current
 * clip in the surface.  All gstate functions must
 * verify that the correct clip is set in the surface before
 * invoking any surface drawing function
 */
unsigned int
_cairo_surface_get_current_clip_serial (cairo_surface_t *surface)
{
    return surface->current_clip_serial;
}

/**
 * _cairo_surface_allocate_clip_serial:
 * @surface: the #cairo_surface_t to allocate a serial number from
 *
 * Each surface has a separate set of clipping serial numbers, and
 * this function allocates one from the specified surface.  As zero is
 * reserved for the special no-clipping case, this function will not
 * return that except for an in-error surface, (ie. surface->status !=
 * CAIRO_STATUS_SUCCESS).
 */
unsigned int
_cairo_surface_allocate_clip_serial (cairo_surface_t *surface)
{
    unsigned int    serial;
    
    if (surface->status)
	return 0;

    if ((serial = ++(surface->next_clip_serial)) == 0)
	serial = ++(surface->next_clip_serial);
    return serial;
}

/**
 * _cairo_surface_reset_clip:
 * @surface: the #cairo_surface_t to reset the clip on
 *
 * This function sets the clipping for the surface to
 * None, which is to say that drawing is entirely
 * unclipped.  It also sets the clip serial number
 * to zero.
 */
cairo_status_t
_cairo_surface_reset_clip (cairo_surface_t *surface)
{
    cairo_status_t  status;

    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;
    
    surface->current_clip_serial = 0;

    if (surface->backend->intersect_clip_path) {
	status = surface->backend->intersect_clip_path (surface,
							NULL,
							CAIRO_FILL_RULE_WINDING,
							0,
							CAIRO_ANTIALIAS_DEFAULT);
	if (status)
	    return status;
    }

    if (surface->backend->set_clip_region != NULL) {
	status = surface->backend->set_clip_region (surface, NULL);
	if (status)
	    return status;
    }

    return CAIRO_STATUS_SUCCESS;
}

/**
 * _cairo_surface_set_clip_region:
 * @surface: the #cairo_surface_t to reset the clip on
 * @region: the #pixman_region16_t to use for clipping
 * @serial: the clip serial number associated with the region
 *
 * This function sets the clipping for the surface to
 * the specified region and sets the surface clipping
 * serial number to the associated serial number.
 */
cairo_status_t
_cairo_surface_set_clip_region (cairo_surface_t	    *surface,
				pixman_region16_t   *region,
				unsigned int	    serial)
{
    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;
    
    assert (surface->backend->set_clip_region != NULL);
    
    surface->current_clip_serial = serial;

    return surface->backend->set_clip_region (surface, region);
}

cairo_int_status_t
_cairo_surface_intersect_clip_path (cairo_surface_t    *surface,
				    cairo_path_fixed_t *path,
				    cairo_fill_rule_t   fill_rule,
				    double		tolerance,
				    cairo_antialias_t	antialias)
{
    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;
    
    assert (surface->backend->intersect_clip_path != NULL);

    return surface->backend->intersect_clip_path (surface,
						  path,
						  fill_rule,
						  tolerance,
						  antialias);
}

static cairo_status_t
_cairo_surface_set_clip_path_recursive (cairo_surface_t *surface,
					cairo_clip_path_t *clip_path)
{
    cairo_status_t status;

    if (clip_path == NULL)
	return CAIRO_STATUS_SUCCESS;

    status = _cairo_surface_set_clip_path_recursive (surface, clip_path->prev);
    if (status)
	return status;

    return surface->backend->intersect_clip_path (surface,
						  &clip_path->path,
						  clip_path->fill_rule,
						  clip_path->tolerance,
						  clip_path->antialias);
}

/**
 * _cairo_surface_set_clip_path:
 * @surface: the #cairo_surface_t to set the clip on
 * @clip_path: the clip path to set
 * @serial: the clip serial number associated with the clip path
 * 
 * Sets the given clipping path for the surface and assigns the
 * clipping serial to the surface.
 **/
static cairo_status_t
_cairo_surface_set_clip_path (cairo_surface_t	*surface,
			      cairo_clip_path_t	*clip_path,
			      unsigned int	serial)
{
    cairo_status_t status;

    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    assert (surface->backend->intersect_clip_path != NULL);

    status = surface->backend->intersect_clip_path (surface,
						    NULL,
						    CAIRO_FILL_RULE_WINDING,
						    0,
						    CAIRO_ANTIALIAS_DEFAULT);
    if (status)
	return status;

    status = _cairo_surface_set_clip_path_recursive (surface, clip_path);
    if (status)
	return status;

    surface->current_clip_serial = serial;

    return CAIRO_STATUS_SUCCESS;
}

cairo_status_t
_cairo_surface_set_clip (cairo_surface_t *surface, cairo_clip_t *clip)
{
    if (!surface)
	return CAIRO_STATUS_NULL_POINTER;
    if (clip->serial == _cairo_surface_get_current_clip_serial (surface))
	return CAIRO_STATUS_SUCCESS;
    
    if (clip->path)
	return _cairo_surface_set_clip_path (surface,
					     clip->path,
					     clip->serial);
    
    if (clip->region)
	return _cairo_surface_set_clip_region (surface, 
					       clip->region,
					       clip->serial);
    
    return _cairo_surface_reset_clip (surface);
}

/**
 * _cairo_surface_get_extents:
 * @surface: the #cairo_surface_t to fetch extents for
 *
 * This function returns a bounding box for the surface.  The
 * surface bounds are defined as a region beyond which no
 * rendering will possibly be recorded, in otherwords, 
 * it is the maximum extent of potentially usable
 * coordinates.  For simple pixel-based surfaces,
 * it can be a close bound on the retained pixel
 * region.  For virtual surfaces (PDF et al), it
 * cannot and must extend to the reaches of the
 * target system coordinate space.
 */

cairo_status_t
_cairo_surface_get_extents (cairo_surface_t   *surface,
			    cairo_rectangle_t *rectangle)
{
    if (surface->status)
	return surface->status;

    if (surface->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    return surface->backend->get_extents (surface, rectangle);
}

cairo_status_t
_cairo_surface_show_glyphs (cairo_scaled_font_t	        *scaled_font,
			    cairo_operator_t		operator,
			    cairo_pattern_t		*pattern,
			    cairo_surface_t		*dst,
			    int				source_x,
			    int				source_y,
			    int				dest_x,
			    int				dest_y,
			    unsigned int		width,
			    unsigned int		height,
			    const cairo_glyph_t		*glyphs,
			    int				num_glyphs)
{
    cairo_status_t status;

    if (dst->status)
	return dst->status;

    if (dst->finished)
	return CAIRO_STATUS_SURFACE_FINISHED;

    if (dst->backend->show_glyphs)
	status = dst->backend->show_glyphs (scaled_font, operator, pattern, dst,
					    source_x, source_y,
					    dest_x, dest_y,
					    width, height,
					    glyphs, num_glyphs);
    else
	status = CAIRO_INT_STATUS_UNSUPPORTED;

    return status;
}

static cairo_status_t
_cairo_surface_composite_fixup_unbounded_internal (cairo_surface_t            *dst,
						   cairo_rectangle_t          *src_rectangle,
						   cairo_rectangle_t          *mask_rectangle,
						   int			       dst_x,
						   int			       dst_y,
						   unsigned int		       width,
						   unsigned int		       height)
{
    cairo_rectangle_t dst_rectangle;
    cairo_rectangle_t drawn_rectangle;
    pixman_region16_t *drawn_region;
    pixman_region16_t *clear_region;
    cairo_status_t status = CAIRO_STATUS_SUCCESS;

    /* The area that was drawn is the area in the destination rectangle but not within
     * the source or the mask.
     */
    dst_rectangle.x = dst_x;
    dst_rectangle.y = dst_y;
    dst_rectangle.width = width;
    dst_rectangle.height = height;

    drawn_rectangle = dst_rectangle;

    if (src_rectangle)
	_cairo_rectangle_intersect (&drawn_rectangle, src_rectangle);

    if (mask_rectangle)
	_cairo_rectangle_intersect (&drawn_rectangle, mask_rectangle);

    /* Now compute the area that is in dst_rectangle but not in drawn_rectangle
     */
    drawn_region = _cairo_region_create_from_rectangle (&drawn_rectangle);
    clear_region = _cairo_region_create_from_rectangle (&dst_rectangle);
    if (!drawn_region || !clear_region) {
	status = CAIRO_STATUS_NO_MEMORY;
	goto CLEANUP_REGIONS;
    }

    if (pixman_region_subtract (clear_region, clear_region, drawn_region) != PIXMAN_REGION_STATUS_SUCCESS) {
	status = CAIRO_STATUS_NO_MEMORY;
	goto CLEANUP_REGIONS;
    }

    status = _cairo_surface_fill_region (dst, CAIRO_OPERATOR_SOURCE,
					 CAIRO_COLOR_TRANSPARENT,
					 clear_region);

 CLEANUP_REGIONS:
    if (drawn_region)
	pixman_region_destroy (drawn_region);
    if (clear_region)
	pixman_region_destroy (clear_region);

    return status;
}

/**
 * _cairo_surface_composite_fixup_unbounded:
 * @dst: the destination surface
 * @src_attr: source surface attributes (from _cairo_pattern_acquire_surface())
 * @src_width: width of source surface
 * @src_height: height of source surface
 * @mask_attr: mask surface attributes or %NULL if no mask
 * @mask_width: width of mask surface
 * @mask_height: height of mask surface
 * @src_x: @src_x from _cairo_surface_composite()
 * @src_y: @src_y from _cairo_surface_composite()
 * @mask_x: @mask_x from _cairo_surface_composite()
 * @mask_y: @mask_y from _cairo_surface_composite()
 * @dst_x: @dst_x from _cairo_surface_composite()
 * @dst_y: @dst_y from _cairo_surface_composite()
 * @width: @width from _cairo_surface_composite()
 * @height: @height_x from _cairo_surface_composite()
 * 
 * Eeek! Too many parameters! This is a helper function to take care of fixing
 * up for bugs in libpixman and RENDER where, when asked to composite an
 * untransformed surface with an unbounded operator (like CLEAR or SOURCE)
 * only the region inside both the source and the mask is affected.
 * This function clears the region that should have been drawn but was wasn't.
 **/
cairo_status_t
_cairo_surface_composite_fixup_unbounded (cairo_surface_t            *dst,
					  cairo_surface_attributes_t *src_attr,
					  int                         src_width,
					  int                         src_height,
					  cairo_surface_attributes_t *mask_attr,
					  int                         mask_width,
					  int                         mask_height,
					  int			      src_x,
					  int			      src_y,
					  int			      mask_x,
					  int			      mask_y,
					  int			      dst_x,
					  int			      dst_y,
					  unsigned int		      width,
					  unsigned int		      height)
{
    cairo_rectangle_t src_tmp, mask_tmp;
    cairo_rectangle_t *src_rectangle = NULL;
    cairo_rectangle_t *mask_rectangle = NULL;
  
    /* The RENDER/libpixman operators are clipped to the bounds of the untransformed,
     * non-repeating sources and masks. Other sources and masks can be ignored.
     */
    if (_cairo_matrix_is_integer_translation (&src_attr->matrix, NULL, NULL) &&
	src_attr->extend == CAIRO_EXTEND_NONE)
    {
	src_tmp.x = (dst_x - (src_x + src_attr->x_offset));
	src_tmp.y = (dst_y - (src_y + src_attr->y_offset));
	src_tmp.width = src_width;
	src_tmp.height = src_height;

	src_rectangle = &src_tmp;
    }

    if (mask_attr &&
	_cairo_matrix_is_integer_translation (&mask_attr->matrix, NULL, NULL) &&
	mask_attr->extend == CAIRO_EXTEND_NONE)
    {
	mask_tmp.x = (dst_x - (mask_x + mask_attr->x_offset));
	mask_tmp.y = (dst_y - (mask_y + mask_attr->y_offset));
	mask_tmp.width = mask_width;
	mask_tmp.height = mask_height;

	mask_rectangle = &mask_tmp;
    }

    return _cairo_surface_composite_fixup_unbounded_internal (dst, src_rectangle, mask_rectangle,
							      dst_x, dst_y, width, height);
}

/**
 * _cairo_surface_composite_shape_fixup_unbounded:
 * @dst: the destination surface
 * @src_attr: source surface attributes (from _cairo_pattern_acquire_surface())
 * @src_width: width of source surface
 * @src_height: height of source surface
 * @mask_width: width of mask surface
 * @mask_height: height of mask surface
 * @src_x: @src_x from _cairo_surface_composite()
 * @src_y: @src_y from _cairo_surface_composite()
 * @mask_x: @mask_x from _cairo_surface_composite()
 * @mask_y: @mask_y from _cairo_surface_composite()
 * @dst_x: @dst_x from _cairo_surface_composite()
 * @dst_y: @dst_y from _cairo_surface_composite()
 * @width: @width from _cairo_surface_composite()
 * @height: @height_x from _cairo_surface_composite()
 * 
 * Like _cairo_surface_composite_fixup_unbounded(), but instead of
 * handling the case where we have a source pattern and a mask
 * pattern, handle the case where we are compositing a source pattern
 * using a mask we create ourselves, as in
 * _cairo_surface_composite_glyphs() or _cairo_surface_composite_trapezoids()
 **/
cairo_status_t
_cairo_surface_composite_shape_fixup_unbounded (cairo_surface_t            *dst,
						cairo_surface_attributes_t *src_attr,
						int                         src_width,
						int                         src_height,
						int                         mask_width,
						int                         mask_height,
						int			    src_x,
						int			    src_y,
						int			    mask_x,
						int			    mask_y,
						int			    dst_x,
						int			    dst_y,
						unsigned int		    width,
						unsigned int		    height)
{
    cairo_rectangle_t src_tmp, mask_tmp;
    cairo_rectangle_t *src_rectangle = NULL;
    cairo_rectangle_t *mask_rectangle = NULL;
  
    /* The RENDER/libpixman operators are clipped to the bounds of the untransformed,
     * non-repeating sources and masks. Other sources and masks can be ignored.
     */
    if (_cairo_matrix_is_integer_translation (&src_attr->matrix, NULL, NULL) &&
	src_attr->extend == CAIRO_EXTEND_NONE)
    {
	src_tmp.x = (dst_x - (src_x + src_attr->x_offset));
	src_tmp.y = (dst_y - (src_y + src_attr->y_offset));
	src_tmp.width = src_width;
	src_tmp.height = src_height;

	src_rectangle = &src_tmp;
    }

    mask_tmp.x = dst_x - mask_x;
    mask_tmp.y = dst_y - mask_y;
    mask_tmp.width = mask_width;
    mask_tmp.height = mask_height;
    
    mask_rectangle = &mask_tmp;

    return _cairo_surface_composite_fixup_unbounded_internal (dst, src_rectangle, mask_rectangle,
							      dst_x, dst_y, width, height);
}
