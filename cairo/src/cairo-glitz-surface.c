/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2004 David Reveman
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of David
 * Reveman not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. David Reveman makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * DAVID REVEMAN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL DAVID REVEMAN BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: David Reveman <davidr@novell.com>
 */

#include "cairoint.h"
#include "cairo-glitz.h"

typedef struct _cairo_glitz_surface {
    cairo_surface_t   base;

    glitz_surface_t   *surface;
    glitz_format_t    *format;
    pixman_region16_t *clip;
} cairo_glitz_surface_t;

static cairo_status_t
_cairo_glitz_surface_finish (void *abstract_surface)
{
    cairo_glitz_surface_t *surface = abstract_surface;

    if (surface->clip)
    {
	glitz_surface_set_clip_region (surface->surface, 0, 0, NULL, 0);
	pixman_region_destroy (surface->clip);
    }
    
    glitz_surface_destroy (surface->surface);

    return CAIRO_STATUS_SUCCESS;
}

static glitz_format_name_t
_glitz_format_from_content (cairo_content_t content)
{
    switch (content) {
    case CAIRO_CONTENT_COLOR:
	return GLITZ_STANDARD_RGB24;
    case CAIRO_CONTENT_ALPHA:
	return GLITZ_STANDARD_A8;
    case CAIRO_CONTENT_COLOR_ALPHA:
	return GLITZ_STANDARD_ARGB32;
    }

    ASSERT_NOT_REACHED;
    return GLITZ_STANDARD_ARGB32;
}

static cairo_surface_t *
_cairo_glitz_surface_create_similar (void	    *abstract_src,
				     cairo_content_t content,
				     int	     width,
				     int	     height)
{
    cairo_glitz_surface_t *src = abstract_src;
    cairo_surface_t	  *crsurface;
    glitz_drawable_t	  *drawable;
    glitz_surface_t	  *surface;
    glitz_format_t	  *gformat;

    drawable = glitz_surface_get_drawable (src->surface);
    
    gformat = glitz_find_standard_format (drawable,
					  _glitz_format_from_content (content));
    if (!gformat) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_surface_t*) &_cairo_surface_nil;
    }
    
    surface = glitz_surface_create (drawable, gformat, width, height, 0, NULL);
    if (surface == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_surface_t*) &_cairo_surface_nil;
    }

    crsurface = cairo_glitz_surface_create (surface);
    
    glitz_surface_destroy (surface);

    return crsurface;
}

static cairo_status_t
_cairo_glitz_surface_get_image (cairo_glitz_surface_t *surface,
				cairo_rectangle_t     *interest,
				cairo_image_surface_t **image_out,
				cairo_rectangle_t     *rect_out)
{
    cairo_image_surface_t *image;
    int			  x1, y1, x2, y2;
    int			  width, height;
    unsigned char	  *pixels;
    cairo_format_masks_t  format;
    glitz_buffer_t	  *buffer;
    glitz_pixel_format_t  pf;

    x1 = 0;
    y1 = 0;
    x2 = glitz_surface_get_width (surface->surface);
    y2 = glitz_surface_get_height (surface->surface);

    if (interest)
    {
	if (interest->x > x1)
	    x1 = interest->x;
	if (interest->y > y1)
	    y1 = interest->y;
	if (interest->x + interest->width < x2)
	    x2 = interest->x + interest->width;
	if (interest->y + interest->height < y2)
	    y2 = interest->y + interest->height;

	if (x1 >= x2 || y1 >= y2)
	{
	    *image_out = NULL;
	    return CAIRO_STATUS_SUCCESS;
	}
    }

    width  = x2 - x1;
    height = y2 - y1;

    if (rect_out)
    {
	rect_out->x = x1;
	rect_out->y = y1;
	rect_out->width = width;
	rect_out->height = height;
    }
    
    if (surface->format->type == GLITZ_FORMAT_TYPE_COLOR) {
	if (surface->format->color.red_size > 0) {
	    format.bpp = 32;
	    
	    if (surface->format->color.alpha_size > 0)
		format.alpha_mask = 0xff000000;
	    else
		format.alpha_mask = 0x0;
	    
	    format.red_mask = 0xff0000;
	    format.green_mask = 0xff00;
	    format.blue_mask = 0xff;
	} else {
	    format.bpp = 8;
	    format.blue_mask = format.green_mask = format.red_mask = 0x0;
	    format.alpha_mask = 0xff;
	}
    } else {
	format.bpp = 32;
	format.alpha_mask = 0xff000000;
	format.red_mask = 0xff0000;
	format.green_mask = 0xff00;
	format.blue_mask = 0xff;
    }

    pf.masks.bpp = format.bpp;
    pf.masks.alpha_mask = format.alpha_mask;
    pf.masks.red_mask = format.red_mask;
    pf.masks.green_mask = format.green_mask;
    pf.masks.blue_mask = format.blue_mask;
    pf.xoffset = 0;
    pf.skip_lines = 0;

    /* XXX: we should eventually return images with negative stride,
       need to verify that libpixman have no problem with this first. */
    pf.bytes_per_line = (((width * format.bpp) / 8) + 3) & -4;
    pf.scanline_order = GLITZ_PIXEL_SCANLINE_ORDER_TOP_DOWN;

    pixels = malloc (height * pf.bytes_per_line);
    if (!pixels)
	return CAIRO_STATUS_NO_MEMORY;

    buffer = glitz_buffer_create_for_data (pixels);
    if (!buffer) {
	free (pixels);
	return CAIRO_STATUS_NO_MEMORY;
    }
    
    glitz_get_pixels (surface->surface,
		      x1, y1,
		      width, height,
		      &pf,
		      buffer);

    glitz_buffer_destroy (buffer);
    
    image = (cairo_image_surface_t *)
        _cairo_image_surface_create_with_masks (pixels,
						&format,
						width, height,
						pf.bytes_per_line);
    if (image->base.status)
    {
	free (pixels);
	return CAIRO_STATUS_NO_MEMORY;
    }

    _cairo_image_surface_assume_ownership_of_data (image);

    *image_out = image;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_glitz_surface_set_image (void		      *abstract_surface,
				cairo_image_surface_t *image,
				int		      x_dst,
				int		      y_dst)
{
    cairo_glitz_surface_t *surface = abstract_surface;
    glitz_buffer_t	  *buffer;
    glitz_pixel_format_t  pf;
    pixman_format_t	  *format;
    int			  am, rm, gm, bm;
    char		  *data;
    
    format = pixman_image_get_format (image->pixman_image);
    if (!format)
	return CAIRO_STATUS_NO_MEMORY;

    pixman_format_get_masks (format, &pf.masks.bpp, &am, &rm, &gm, &bm);

    pf.masks.alpha_mask = am;
    pf.masks.red_mask = rm;
    pf.masks.green_mask = gm;
    pf.masks.blue_mask = bm;
    pf.xoffset = 0;
    pf.skip_lines = 0;

    /* check for negative stride */
    if (image->stride < 0)
    {
	pf.bytes_per_line = -image->stride;
	pf.scanline_order = GLITZ_PIXEL_SCANLINE_ORDER_BOTTOM_UP;
	data = (char *) image->data + image->stride * (image->height - 1);
    }
    else
    {
	pf.bytes_per_line = image->stride;
	pf.scanline_order = GLITZ_PIXEL_SCANLINE_ORDER_TOP_DOWN;
	data = (char *) image->data;
    }

    buffer = glitz_buffer_create_for_data (data);
    if (!buffer)
	return CAIRO_STATUS_NO_MEMORY;
    
    glitz_set_pixels (surface->surface,
		      x_dst, y_dst,
		      image->width, image->height,
		      &pf,
		      buffer);
    
    glitz_buffer_destroy (buffer);
    
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_glitz_surface_acquire_source_image (void              *abstract_surface,
					   cairo_image_surface_t **image_out,
					   void                  **image_extra)
{
    cairo_glitz_surface_t *surface = abstract_surface;

    *image_extra = NULL;
    
    return _cairo_glitz_surface_get_image (surface, NULL, image_out, NULL);
}

static void
_cairo_glitz_surface_release_source_image (void              *abstract_surface,
					   cairo_image_surface_t *image,
					   void                  *image_extra)
{
    cairo_surface_destroy (&image->base);
}

static cairo_status_t
_cairo_glitz_surface_acquire_dest_image (void                *abstract_surface,
					 cairo_rectangle_t     *interest_rect,
					 cairo_image_surface_t **image_out,
					 cairo_rectangle_t     *image_rect_out,
					 void                  **image_extra)
{
    cairo_glitz_surface_t *surface = abstract_surface;
    cairo_image_surface_t *image;
    cairo_status_t	  status;

    status = _cairo_glitz_surface_get_image (surface, interest_rect, &image,
					     image_rect_out);
    if (status)
	return status;

    *image_out   = image;
    *image_extra = NULL;

    return status;
}

static void
_cairo_glitz_surface_release_dest_image (void                *abstract_surface,
					 cairo_rectangle_t     *interest_rect,
					 cairo_image_surface_t *image,
					 cairo_rectangle_t     *image_rect,
					 void                  *image_extra)
{
    cairo_glitz_surface_t *surface = abstract_surface;

    _cairo_glitz_surface_set_image (surface, image,
				    image_rect->x, image_rect->y);

    cairo_surface_destroy (&image->base);
}


static cairo_status_t
_cairo_glitz_surface_clone_similar (void	    *abstract_surface,
				    cairo_surface_t *src,
				    cairo_surface_t **clone_out)
{
    cairo_glitz_surface_t *surface = abstract_surface;
    cairo_glitz_surface_t *clone;

    if (surface->base.status)
	return surface->base.status;

    if (src->backend == surface->base.backend)
    {
	*clone_out = cairo_surface_reference (src);	
	
	return CAIRO_STATUS_SUCCESS;
    }
    else if (_cairo_surface_is_image (src))
    {
	cairo_image_surface_t *image_src = (cairo_image_surface_t *) src;
	cairo_content_t content = _cairo_content_from_format (image_src->format);
    
	clone = (cairo_glitz_surface_t *)
	    _cairo_glitz_surface_create_similar (surface, content,
						 image_src->width,
						 image_src->height);
	if (clone->base.status)
	    return CAIRO_STATUS_NO_MEMORY;

	_cairo_glitz_surface_set_image (clone, image_src, 0, 0);
	
	*clone_out = &clone->base;

	return CAIRO_STATUS_SUCCESS;
    }
    
    return CAIRO_INT_STATUS_UNSUPPORTED;
}

static void
_cairo_glitz_surface_set_matrix (cairo_glitz_surface_t *surface,
				 cairo_matrix_t	       *matrix)
{
    glitz_transform_t transform;

    transform.matrix[0][0] = _cairo_fixed_from_double (matrix->xx);
    transform.matrix[0][1] = _cairo_fixed_from_double (matrix->xy);
    transform.matrix[0][2] = _cairo_fixed_from_double (matrix->x0);

    transform.matrix[1][0] = _cairo_fixed_from_double (matrix->yx);
    transform.matrix[1][1] = _cairo_fixed_from_double (matrix->yy);
    transform.matrix[1][2] = _cairo_fixed_from_double (matrix->y0);

    transform.matrix[2][0] = 0;
    transform.matrix[2][1] = 0;
    transform.matrix[2][2] = _cairo_fixed_from_double (1);

    glitz_surface_set_transform (surface->surface, &transform);
}

static glitz_operator_t
_glitz_operator (cairo_operator_t op)
{
    switch (op) {
    case CAIRO_OPERATOR_CLEAR:
	return GLITZ_OPERATOR_CLEAR;

    case CAIRO_OPERATOR_SOURCE:
	return GLITZ_OPERATOR_SRC;
    case CAIRO_OPERATOR_OVER:
	return GLITZ_OPERATOR_OVER;
    case CAIRO_OPERATOR_IN:
	return GLITZ_OPERATOR_IN;
    case CAIRO_OPERATOR_OUT:
	return GLITZ_OPERATOR_OUT;
    case CAIRO_OPERATOR_ATOP:
	return GLITZ_OPERATOR_ATOP;

    case CAIRO_OPERATOR_DEST:
	return GLITZ_OPERATOR_DST;
    case CAIRO_OPERATOR_DEST_OVER:
	return GLITZ_OPERATOR_OVER_REVERSE;
    case CAIRO_OPERATOR_DEST_IN:
	return GLITZ_OPERATOR_IN_REVERSE;
    case CAIRO_OPERATOR_DEST_OUT:
	return GLITZ_OPERATOR_OUT_REVERSE;
    case CAIRO_OPERATOR_DEST_ATOP:
	return GLITZ_OPERATOR_ATOP_REVERSE;

    case CAIRO_OPERATOR_XOR:
	return GLITZ_OPERATOR_XOR;
    case CAIRO_OPERATOR_ADD:
	return GLITZ_OPERATOR_ADD;
    case CAIRO_OPERATOR_SATURATE:
	/* XXX: OVER is definitely not the right thing here, (but it
	 * is what the original glitz backend code has always
	 * done). Cairo's SATURATE operator is the native GL
	 * compositing mode, (from my understanding). So why isn't
	 * there a GLITZ_OPERATOR_SATURATE for us to use here? */
	return GLITZ_OPERATOR_OVER;
    }

    ASSERT_NOT_REACHED;

    /* Something's very broken if this line of code can be reached, so
       we want to return something that would give a noticeably
       incorrect result. The XOR operator seems so rearely desired
       that it should fit the bill here. */
    return CAIRO_OPERATOR_XOR;
}

#define CAIRO_GLITZ_FEATURE_OK(surface, name)				  \
    (glitz_drawable_get_features (glitz_surface_get_drawable (surface)) & \
     (GLITZ_FEATURE_ ## name ## _MASK))

static glitz_status_t
_glitz_ensure_target (glitz_surface_t *surface)
{
    if (glitz_surface_get_attached_drawable (surface) ||
	CAIRO_GLITZ_FEATURE_OK (surface, FRAMEBUFFER_OBJECT))
	return CAIRO_STATUS_SUCCESS;

    return CAIRO_INT_STATUS_UNSUPPORTED;
}

typedef struct _cairo_glitz_surface_attributes {
    cairo_surface_attributes_t	base;
    
    glitz_fill_t		fill;
    glitz_filter_t		filter;
    glitz_fixed16_16_t		*params;
    int				n_params;
    cairo_bool_t		acquired;
} cairo_glitz_surface_attributes_t;

static cairo_int_status_t
_cairo_glitz_pattern_acquire_surface (cairo_pattern_t	              *pattern,
				      cairo_glitz_surface_t	       *dst,
				      int			       x,
				      int			       y,
				      unsigned int		       width,
				      unsigned int		       height,
				      cairo_glitz_surface_t	 **surface_out,
				      cairo_glitz_surface_attributes_t *attr)
{
    cairo_glitz_surface_t *src = NULL;

    attr->acquired = FALSE;

    switch (pattern->type) {
    case CAIRO_PATTERN_LINEAR:
    case CAIRO_PATTERN_RADIAL: {
	cairo_gradient_pattern_t    *gradient =
	    (cairo_gradient_pattern_t *) pattern;
	char			    *data;
	glitz_fixed16_16_t	    *params;
	int			    n_params;
	unsigned int		    *pixels;
	int			    i;
	unsigned char		    alpha;
	glitz_buffer_t		    *buffer;
	static glitz_pixel_format_t format = {
            {
                32,
                0xff000000,
                0x00ff0000,
                0x0000ff00,
                0x000000ff
            },
            0, 0, 0,
            GLITZ_PIXEL_SCANLINE_ORDER_BOTTOM_UP
        };
	
	/* XXX: the current color gradient acceleration provided by glitz is
	 * experimental, it's been proven inappropriate in a number of ways,
	 * most importantly, it's currently implemented as filters and
	 * gradients are not filters. eventually, it will be replaced with
	 * something more appropriate.
	 */

	if (gradient->n_stops < 2)
	    break;

	/* glitz doesn't support inner and outer circle with different
	   center points. */
	if (pattern->type == CAIRO_PATTERN_RADIAL)
	{
	    cairo_radial_pattern_t *grad = (cairo_radial_pattern_t *) pattern;
	    
	    if (grad->center0.x != grad->center1.x ||
		grad->center0.y != grad->center1.y)
		break;
	}

	if (!CAIRO_GLITZ_FEATURE_OK (dst->surface, FRAGMENT_PROGRAM))
	    break;
	
	if (pattern->filter != CAIRO_FILTER_BILINEAR &&
	    pattern->filter != CAIRO_FILTER_GOOD &&
	    pattern->filter != CAIRO_FILTER_BEST)
	    break;

	alpha = gradient->stops[0].color.alpha * 0xff;
	for (i = 1; i < gradient->n_stops; i++)
	{
	    unsigned char a;
	    
	    a = gradient->stops[i].color.alpha * 0xff;
	    if (a != alpha)
		break;
	}

	/* we can't have color stops with different alpha as gradient color
	   interpolation should be done to unpremultiplied colors. */
	if (i < gradient->n_stops)
	    break;

	n_params = gradient->n_stops * 3 + 4;

	data = malloc (sizeof (glitz_fixed16_16_t) * n_params +
		       sizeof (unsigned int) * gradient->n_stops);
	if (!data)
	    return CAIRO_STATUS_NO_MEMORY;

	params = (glitz_fixed16_16_t *) data;
	pixels = (unsigned int *)
	    (data + sizeof (glitz_fixed16_16_t) * n_params);

	buffer = glitz_buffer_create_for_data (pixels);
	if (!buffer)
	{
	    free (data);
	    return CAIRO_STATUS_NO_MEMORY;
	}

	src = (cairo_glitz_surface_t *)
	    _cairo_surface_create_similar_scratch (&dst->base,
						   CAIRO_CONTENT_COLOR_ALPHA,
						   gradient->n_stops, 1);
	if (src->base.status)
	{
	    glitz_buffer_destroy (buffer);
	    free (data);
	    return CAIRO_STATUS_NO_MEMORY;
	}

	for (i = 0; i < gradient->n_stops; i++)
	{
	    pixels[i] =
                (((int) alpha) << 24)                                  |
                (((int) gradient->stops[i].color.red   * alpha) << 16) |
                (((int) gradient->stops[i].color.green * alpha) << 8)  |
                (((int) gradient->stops[i].color.blue  * alpha));
	    
	    params[4 + 3 * i] = gradient->stops[i].offset;
	    params[5 + 3 * i] = i << 16;
	    params[6 + 3 * i] = 0;
	}

	glitz_set_pixels (src->surface, 0, 0, gradient->n_stops, 1,
			  &format, buffer);

	glitz_buffer_destroy (buffer);

	if (pattern->type == CAIRO_PATTERN_LINEAR)
	{
	    cairo_linear_pattern_t *grad = (cairo_linear_pattern_t *) pattern;
	    
	    params[0] = _cairo_fixed_from_double (grad->point0.x);
	    params[1] = _cairo_fixed_from_double (grad->point0.y);
	    params[2] = _cairo_fixed_from_double (grad->point1.x);
	    params[3] = _cairo_fixed_from_double (grad->point1.y);
	    attr->filter = GLITZ_FILTER_LINEAR_GRADIENT;
	}
	else
	{
	    cairo_radial_pattern_t *grad = (cairo_radial_pattern_t *) pattern;
	    
	    params[0] = _cairo_fixed_from_double (grad->center0.x);
	    params[1] = _cairo_fixed_from_double (grad->center0.y);
	    params[2] = _cairo_fixed_from_double (grad->radius0);
	    params[3] = _cairo_fixed_from_double (grad->radius1);
	    attr->filter = GLITZ_FILTER_RADIAL_GRADIENT;
	}

	switch (pattern->extend) {
	case CAIRO_EXTEND_NONE:
	    attr->fill = GLITZ_FILL_NEAREST;
	    break;
	case CAIRO_EXTEND_REPEAT:
	    attr->fill = GLITZ_FILL_REPEAT;
	    break;
	case CAIRO_EXTEND_REFLECT:
	    attr->fill = GLITZ_FILL_REFLECT;
	    break;
	}

	attr->params	    = params;
	attr->n_params	    = n_params;
	attr->base.matrix   = pattern->matrix;
	attr->base.x_offset = 0;
	attr->base.y_offset = 0;
    } break;
    default:
	break;
    }

    if (!src)
    {
	cairo_int_status_t status;

	status = _cairo_pattern_acquire_surface (pattern, &dst->base,
						 x, y, width, height,
						 (cairo_surface_t **) &src,
						 &attr->base);
	if (status)
	    return status;
	
	if (src)
	{
	    switch (attr->base.extend) {
	    case CAIRO_EXTEND_NONE:
		attr->fill = GLITZ_FILL_TRANSPARENT;
		break;
	    case CAIRO_EXTEND_REPEAT:
		attr->fill = GLITZ_FILL_REPEAT;
		break;
	    case CAIRO_EXTEND_REFLECT:
		attr->fill = GLITZ_FILL_REFLECT;
		break;
	    }

	    switch (attr->base.filter) {
	    case CAIRO_FILTER_FAST:
	    case CAIRO_FILTER_NEAREST:
		attr->filter = GLITZ_FILTER_NEAREST;
		break;
	    case CAIRO_FILTER_GOOD:
	    case CAIRO_FILTER_BEST:
	    case CAIRO_FILTER_BILINEAR:
	    default:
		attr->filter = GLITZ_FILTER_BILINEAR;
		break;
	    }
	    
	    attr->params   = NULL;
	    attr->n_params = 0;
	    attr->acquired = TRUE;
	}
    }

    *surface_out = src;
    
    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_glitz_pattern_release_surface (cairo_pattern_t		      *pattern,
				      cairo_glitz_surface_t	      *surface,
				      cairo_glitz_surface_attributes_t *attr)
{
    if (attr->acquired)
	_cairo_pattern_release_surface (pattern, &surface->base, &attr->base);
    else
	cairo_surface_destroy (&surface->base);
}

static cairo_int_status_t
_cairo_glitz_pattern_acquire_surfaces (cairo_pattern_t	                *src,
				       cairo_pattern_t	                *mask,
				       cairo_glitz_surface_t	        *dst,
				       int			        src_x,
				       int			        src_y,
				       int			        mask_x,
				       int			        mask_y,
				       unsigned int		        width,
				       unsigned int		        height,
				       cairo_glitz_surface_t	    **src_out,
				       cairo_glitz_surface_t	    **mask_out,
				       cairo_glitz_surface_attributes_t *sattr,
				       cairo_glitz_surface_attributes_t *mattr)
{
    cairo_int_status_t	  status;
    cairo_pattern_union_t tmp;

    /* If src and mask are both solid, then the mask alpha can be
     * combined into src and mask can be ignored. */

    /* XXX: This optimization assumes that there is no color
     * information in mask, so this will need to change when we
     * support RENDER-style 4-channel masks. */

    if (src->type == CAIRO_PATTERN_SOLID &&
	mask->type == CAIRO_PATTERN_SOLID)
    {
	cairo_color_t combined;
	cairo_solid_pattern_t *src_solid = (cairo_solid_pattern_t *) src;
	cairo_solid_pattern_t *mask_solid = (cairo_solid_pattern_t *) mask;

	combined = src_solid->color;
	_cairo_color_multiply_alpha (&combined, mask_solid->color.alpha);

	_cairo_pattern_init_solid (&tmp.solid, &combined);

	mask = NULL;
    } else {
	_cairo_pattern_init_copy (&tmp.base, src);
    }
	
    status = _cairo_glitz_pattern_acquire_surface (&tmp.base, dst,
						   src_x, src_y,
						   width, height,
						   src_out, sattr);
    
    _cairo_pattern_fini (&tmp.base);

    if (status)
	return status;

    if (mask)
    {
	_cairo_pattern_init_copy (&tmp.base, mask);
	
	status = _cairo_glitz_pattern_acquire_surface (&tmp.base, dst,
						       mask_x, mask_y,
						       width, height,
						       mask_out, mattr);
    
	if (status)
	    _cairo_glitz_pattern_release_surface (&tmp.base, *src_out, sattr);

	_cairo_pattern_fini (&tmp.base);

	return status;
    }
    else
    {
	*mask_out = NULL;
    }

    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_glitz_surface_set_attributes (cairo_glitz_surface_t	      *surface,
				     cairo_glitz_surface_attributes_t *a)
{
    _cairo_glitz_surface_set_matrix (surface, &a->base.matrix);
    glitz_surface_set_fill (surface->surface, a->fill);
    glitz_surface_set_filter (surface->surface, a->filter,
			      a->params, a->n_params);
}

static cairo_int_status_t
_cairo_glitz_surface_composite (cairo_operator_t op,
				cairo_pattern_t  *src_pattern,
				cairo_pattern_t  *mask_pattern,
				void		 *abstract_dst,
				int		 src_x,
				int		 src_y,
				int		 mask_x,
				int		 mask_y,
				int		 dst_x,
				int		 dst_y,
				unsigned int	 width,
				unsigned int	 height)
{
    cairo_glitz_surface_attributes_t	src_attr, mask_attr;
    cairo_glitz_surface_t		*dst = abstract_dst;
    cairo_glitz_surface_t		*src;
    cairo_glitz_surface_t		*mask;
    cairo_int_status_t			status;

    if (op == CAIRO_OPERATOR_SATURATE)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (_glitz_ensure_target (dst->surface))
	return CAIRO_INT_STATUS_UNSUPPORTED;

    status = _cairo_glitz_pattern_acquire_surfaces (src_pattern, mask_pattern,
						    dst,
						    src_x, src_y,
						    mask_x, mask_y,
						    width, height,
						    &src, &mask,
						    &src_attr, &mask_attr);
    if (status)
	return status;

    _cairo_glitz_surface_set_attributes (src, &src_attr);
    if (mask)
    {
	_cairo_glitz_surface_set_attributes (mask, &mask_attr);
	glitz_composite (_glitz_operator (op),
			 src->surface,
			 mask->surface,
			 dst->surface,
			 src_x + src_attr.base.x_offset,
			 src_y + src_attr.base.y_offset,
			 mask_x + mask_attr.base.x_offset,
			 mask_y + mask_attr.base.y_offset,
			 dst_x, dst_y,
			 width, height);
	
	if (mask_attr.n_params)
	    free (mask_attr.params);
	
	_cairo_glitz_pattern_release_surface (mask_pattern, mask, &mask_attr);
    }
    else
    {    
	glitz_composite (_glitz_operator (op),
			 src->surface,
			 NULL,
			 dst->surface,
			 src_x + src_attr.base.x_offset,
			 src_y + src_attr.base.y_offset,
			 0, 0,
			 dst_x, dst_y,
			 width, height);
    }

    if (src_attr.n_params)
	free (src_attr.params);

    _cairo_glitz_pattern_release_surface (src_pattern, src, &src_attr);

    if (glitz_surface_get_status (dst->surface) == GLITZ_STATUS_NOT_SUPPORTED)
	return CAIRO_INT_STATUS_UNSUPPORTED;
    
    return CAIRO_STATUS_SUCCESS;
}

static cairo_int_status_t
_cairo_glitz_surface_fill_rectangles (void		  *abstract_dst,
				      cairo_operator_t	  op,
				      const cairo_color_t *color,
				      cairo_rectangle_t	  *rects,
				      int		  n_rects)
{
    cairo_glitz_surface_t *dst = abstract_dst;

    if (op == CAIRO_OPERATOR_SOURCE)
    {
	glitz_color_t glitz_color;

	glitz_color.red = color->red_short;
	glitz_color.green = color->green_short;
	glitz_color.blue = color->blue_short;
	glitz_color.alpha = color->alpha_short;

	glitz_set_rectangles (dst->surface, &glitz_color,
			      (glitz_rectangle_t *) rects, n_rects);
    }
    else
    {
	cairo_glitz_surface_t *src;
	
	if (op == CAIRO_OPERATOR_SATURATE)
	    return CAIRO_INT_STATUS_UNSUPPORTED;

	if (_glitz_ensure_target (dst->surface))
	    return CAIRO_INT_STATUS_UNSUPPORTED;

	src = (cairo_glitz_surface_t *)
	    _cairo_surface_create_similar_solid (&dst->base,
						 CAIRO_CONTENT_COLOR_ALPHA,
						 1, 1,
						 (cairo_color_t *) color);
	if (src->base.status)
	    return CAIRO_STATUS_NO_MEMORY;

	glitz_surface_set_fill (src->surface, GLITZ_FILL_REPEAT);
	
	while (n_rects--)
	{
	    glitz_composite (_glitz_operator (op),
			     src->surface,
			     NULL,
			     dst->surface,
			     0, 0,
			     0, 0,
			     rects->x, rects->y,
			     rects->width, rects->height);
	    rects++;
	}
	
	cairo_surface_destroy (&src->base);
    }

    if (glitz_surface_get_status (dst->surface) == GLITZ_STATUS_NOT_SUPPORTED)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_int_status_t
_cairo_glitz_surface_composite_trapezoids (cairo_operator_t  op,
					   cairo_pattern_t   *pattern,
					   void		     *abstract_dst,
					   cairo_antialias_t antialias,
					   int		     src_x,
					   int		     src_y,
					   int		     dst_x,
					   int		     dst_y,
					   unsigned int	     width,
					   unsigned int	     height,
					   cairo_trapezoid_t *traps,
					   int		     n_traps)
{
    cairo_pattern_union_t tmp_src_pattern;
    cairo_pattern_t *src_pattern;
    cairo_glitz_surface_attributes_t attributes;
    cairo_glitz_surface_t	     *dst = abstract_dst;
    cairo_glitz_surface_t	     *src;
    cairo_glitz_surface_t	     *mask = NULL;
    glitz_buffer_t		     *buffer = NULL;
    void			     *data = NULL;
    cairo_int_status_t		     status;
    unsigned short		     alpha;

    if (dst->base.status)
	return dst->base.status;

    if (op == CAIRO_OPERATOR_SATURATE)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (_glitz_ensure_target (dst->surface))
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (pattern->type == CAIRO_PATTERN_SURFACE)
    {
	_cairo_pattern_init_copy (&tmp_src_pattern.base, pattern);
	
	status = _cairo_glitz_pattern_acquire_surface (&tmp_src_pattern.base, dst,
						       src_x, src_y,
						       width, height,
						       &src, &attributes);
	src_pattern = &tmp_src_pattern.base;
    }
    else
    {
	status = _cairo_glitz_pattern_acquire_surface (pattern, dst,
						       src_x, src_y,
						       width, height,
						       &src, &attributes);
	src_pattern = pattern;
    }
    alpha = 0xffff;

    if (status)
	return status;

    if (op == CAIRO_OPERATOR_ADD || n_traps <= 1)
    {
	static glitz_color_t	clear_black = { 0, 0, 0, 0 };
	glitz_color_t		color;
	glitz_geometry_format_t	format;
	int			n_trap_added;
	int			offset = 0;
	int			data_size = 0;
	int			size = 30 * n_traps; /* just a guess */

	format.vertex.primitive = GLITZ_PRIMITIVE_QUADS;
	format.vertex.type = GLITZ_DATA_TYPE_FLOAT;
	format.vertex.bytes_per_vertex = 3 * sizeof (glitz_float_t);
	format.vertex.attributes = GLITZ_VERTEX_ATTRIBUTE_MASK_COORD_MASK;
	format.vertex.mask.type = GLITZ_DATA_TYPE_FLOAT;
	format.vertex.mask.size = GLITZ_COORDINATE_SIZE_X;
	format.vertex.mask.offset = 2 * sizeof (glitz_float_t);

	mask = (cairo_glitz_surface_t *)
	    _cairo_glitz_surface_create_similar (&dst->base,
						 CAIRO_CONTENT_ALPHA,
						 2, 1);
	if (mask->base.status)
	{
	    _cairo_glitz_pattern_release_surface (src_pattern, src, &attributes);
	    if (src_pattern == &tmp_src_pattern.base)
		_cairo_pattern_fini (&tmp_src_pattern.base);

	    return CAIRO_STATUS_NO_MEMORY;
	}

	color.red = color.green = color.blue = color.alpha = 0xffff;

	glitz_set_rectangle (mask->surface, &clear_black, 0, 0, 1, 1);
	glitz_set_rectangle (mask->surface, &color, 1, 0, 1, 1);

	glitz_surface_set_fill (mask->surface, GLITZ_FILL_NEAREST);
	glitz_surface_set_filter (mask->surface,
				  GLITZ_FILTER_BILINEAR,
				  NULL, 0);
	
	size *= format.vertex.bytes_per_vertex;
	
	while (n_traps)
	{
	    if (data_size < size)
	    {
		data_size = size;
		data = realloc (data, data_size);
		if (!data)
		{
		    _cairo_glitz_pattern_release_surface (src_pattern, src,
							  &attributes);
		    if (src_pattern == &tmp_src_pattern.base)
			_cairo_pattern_fini (&tmp_src_pattern.base);
		    return CAIRO_STATUS_NO_MEMORY;
		}

		if (buffer)
		    glitz_buffer_destroy (buffer);
		
		buffer = glitz_buffer_create_for_data (data);
		if (!buffer) {
		    free (data);
		    _cairo_glitz_pattern_release_surface (src_pattern, src,
							  &attributes);
		    if (src_pattern == &tmp_src_pattern.base)
			_cairo_pattern_fini (&tmp_src_pattern.base);
		    return CAIRO_STATUS_NO_MEMORY;
		}
	    }
    
	    offset +=
		glitz_add_trapezoids (buffer,
				      offset, size - offset,
				      format.vertex.type, mask->surface,
				      (glitz_trapezoid_t *) traps, n_traps,
				      &n_trap_added);
		
	    n_traps -= n_trap_added;
	    traps   += n_trap_added;
	    size    *= 2;
	}
    
	glitz_set_geometry (dst->surface,
			    GLITZ_GEOMETRY_TYPE_VERTEX,
			    &format, buffer);
	glitz_set_array (dst->surface, 0, 3,
			 offset / format.vertex.bytes_per_vertex,
			 0, 0);
    }
    else
    {
	cairo_image_surface_t *image;
	unsigned char	      *ptr;
	int		      stride;

	stride = (width + 3) & -4;
	data = malloc (stride * height);
	if (!data)
	{
	    _cairo_glitz_pattern_release_surface (src_pattern, src, &attributes);
	    if (src_pattern == &tmp_src_pattern.base)
		_cairo_pattern_fini (&tmp_src_pattern.base);
	    return CAIRO_STATUS_NO_MEMORY;
	}

	memset (data, 0, stride * height);

	/* using negative stride */
	ptr = (unsigned char *) data + stride * (height - 1);
	
	image = (cairo_image_surface_t *)
	    cairo_image_surface_create_for_data (ptr,
						 CAIRO_FORMAT_A8,
						 width, height,
						 -stride);
	if (image->base.status)
	{
	    cairo_surface_destroy (&src->base);
	    free (data);
	    return CAIRO_STATUS_NO_MEMORY;
	}

	pixman_add_trapezoids (image->pixman_image, -dst_x, -dst_y,
			       (pixman_trapezoid_t *) traps, n_traps);

	mask = (cairo_glitz_surface_t *)
	    _cairo_surface_create_similar_scratch (&dst->base,
						   CAIRO_CONTENT_ALPHA,
						   width, height);
	if (mask->base.status)
	{
	    _cairo_glitz_pattern_release_surface (src_pattern, src, &attributes);
	    free (data);
	    cairo_surface_destroy (&image->base);
	    return CAIRO_STATUS_NO_MEMORY;
	}

	_cairo_glitz_surface_set_image (mask, image, 0, 0);
    }

    _cairo_glitz_surface_set_attributes (src, &attributes);
    
    glitz_composite (_glitz_operator (op),
		     src->surface,
		     mask->surface,
		     dst->surface,
		     src_x + attributes.base.x_offset,
		     src_y + attributes.base.y_offset,
		     0, 0,
		     dst_x, dst_y,
		     width, height);

    if (attributes.n_params)
	free (attributes.params);

    glitz_set_geometry (dst->surface,
			GLITZ_GEOMETRY_TYPE_NONE,
			NULL, NULL);

    if (buffer)
	glitz_buffer_destroy (buffer);
    
    free (data);

    _cairo_glitz_pattern_release_surface (src_pattern, src, &attributes);
    if (src_pattern == &tmp_src_pattern.base)
	_cairo_pattern_fini (&tmp_src_pattern.base);

    if (mask)
	cairo_surface_destroy (&mask->base);

    if (glitz_surface_get_status (dst->surface) == GLITZ_STATUS_NOT_SUPPORTED)
	return CAIRO_INT_STATUS_UNSUPPORTED;
    
    return CAIRO_STATUS_SUCCESS;
}

static cairo_int_status_t
_cairo_glitz_surface_set_clip_region (void		*abstract_surface,
				      pixman_region16_t *region)
{
    cairo_glitz_surface_t *surface = abstract_surface;

    if (region)
    {
	glitz_box_t *box;
	int	    n;
	
	if (!surface->clip)
	{
	    surface->clip = pixman_region_create ();
	    if (!surface->clip)
		return CAIRO_STATUS_NO_MEMORY;
	}
	pixman_region_copy (surface->clip, region);

	box = (glitz_box_t *) pixman_region_rects (surface->clip);
	n = pixman_region_num_rects (surface->clip);
	glitz_surface_set_clip_region (surface->surface, 0, 0, box, n);
    }
    else
    {
	glitz_surface_set_clip_region (surface->surface, 0, 0, NULL, 0);
	
	if (surface->clip)
	    pixman_region_destroy (surface->clip);

	surface->clip = NULL;
    }
    
    return CAIRO_STATUS_SUCCESS;
}

static cairo_int_status_t
_cairo_glitz_surface_get_extents (void		    *abstract_surface,
				  cairo_rectangle_t *rectangle)
{
    cairo_glitz_surface_t *surface = abstract_surface;

    rectangle->x = 0;
    rectangle->y = 0;
    rectangle->width = glitz_surface_get_width  (surface->surface);
    rectangle->height = glitz_surface_get_height (surface->surface);

    return CAIRO_STATUS_SUCCESS;
}

#define CAIRO_GLITZ_GLYPH_CACHE_MEMORY_DEFAULT 0x100000

#define CAIRO_GLITZ_AREA_AVAILABLE 0
#define CAIRO_GLITZ_AREA_DIVIDED   1
#define CAIRO_GLITZ_AREA_OCCUPIED  2

typedef struct _cairo_glitz_root_area cairo_glitz_root_area_t;

typedef struct _cairo_glitz_area {
    int			     state;
    int			     level;
    int			     x, y;
    int			     width, height;
    struct _cairo_glitz_area *area[4];
    cairo_glitz_root_area_t  *root;
    void		     *closure;
} cairo_glitz_area_t;

static cairo_glitz_area_t _empty_area = {
    0, 0, 0, 0, 0, 0,
    { NULL, NULL, NULL, NULL },
    NULL,
    NULL
};

typedef struct _cairo_glitz_area_funcs {
    cairo_status_t (*move_in)	    (cairo_glitz_area_t *area,
				     void		*closure);
    
    void	   (*move_out)	    (cairo_glitz_area_t *area,
				     void		*closure);
    
    int		   (*compare_score) (cairo_glitz_area_t *area,
				     void		*closure1,
				     void		*closure2);
} cairo_glitz_area_funcs_t;

struct _cairo_glitz_root_area {
    int				   max_level;
    int				   width, height;
    cairo_glitz_area_t		   *area;
    const cairo_glitz_area_funcs_t *funcs;
};

static cairo_status_t
_cairo_glitz_area_move_in (cairo_glitz_area_t *area,
			   void		      *closure)
{
    area->closure = closure;
    area->state   = CAIRO_GLITZ_AREA_OCCUPIED;
    
    return (*area->root->funcs->move_in) (area, area->closure);
}

static void
_cairo_glitz_area_move_out (cairo_glitz_area_t *area)
{
    if (area->root)
    {
	(*area->root->funcs->move_out) (area, area->closure);

	area->closure = NULL;
	area->state   = CAIRO_GLITZ_AREA_AVAILABLE;
    }
}

static cairo_glitz_area_t *
_cairo_glitz_area_create (cairo_glitz_root_area_t *root,
			  int			  level,
			  int			  x,
			  int			  y,
			  int			  width,
			  int			  height)
{
    cairo_glitz_area_t *area;
    int		       n = 4;
    
    area = malloc (sizeof (cairo_glitz_area_t));
    if (!area)
	return NULL;

    area->level   = level;
    area->x	  = x;
    area->y	  = y;
    area->width   = width;
    area->height  = height;
    area->root    = root;
    area->closure = NULL;
    area->state   = CAIRO_GLITZ_AREA_AVAILABLE;
    
    while (n--)
	area->area[n] = NULL;

    return area;
}

static void
_cairo_glitz_area_destroy (cairo_glitz_area_t *area)
{   
    if (area == NULL)
	return;

    if (area->state == CAIRO_GLITZ_AREA_OCCUPIED)
    {
	_cairo_glitz_area_move_out (area);
    }
    else
    {
	int n = 4;
	
	while (n--)
	    _cairo_glitz_area_destroy (area->area[n]);
    }
    
    free (area);
}

static cairo_glitz_area_t *
_cairo_glitz_area_get_top_scored_sub_area (cairo_glitz_area_t *area)
{
    if (!area)
	return NULL;
		
    switch (area->state) {
    case CAIRO_GLITZ_AREA_OCCUPIED:
	return area;
    case CAIRO_GLITZ_AREA_AVAILABLE:
	break;
    case CAIRO_GLITZ_AREA_DIVIDED: {
	cairo_glitz_area_t *tmp, *top = NULL;
	int		   i;
	
	for (i = 0; i < 4; i++)
	{
	    tmp = _cairo_glitz_area_get_top_scored_sub_area (area->area[i]);
	    if (tmp && top)
	    {
		if ((*area->root->funcs->compare_score) (tmp,
							 tmp->closure,
							 top->closure) > 0)
		    top = tmp;
	    }
	    else if (tmp)
	    {
		top = tmp;
	    }
	}
	return top;
    }
    }
    
    return NULL;
}

static cairo_int_status_t
_cairo_glitz_area_find (cairo_glitz_area_t *area,
			int		   width,
			int		   height,
			cairo_bool_t	   kick_out,
			void		   *closure)
{
    cairo_status_t status;

    if (area->width < width || area->height < height)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    switch (area->state) {
    case CAIRO_GLITZ_AREA_OCCUPIED:
	if (kick_out)
	{
	    if ((*area->root->funcs->compare_score) (area,
						     area->closure,
						     closure) >= 0)
		return CAIRO_INT_STATUS_UNSUPPORTED;
	
	    _cairo_glitz_area_move_out (area);
	} else {
	    return CAIRO_INT_STATUS_UNSUPPORTED;
	}
		
    /* fall-through */
    case CAIRO_GLITZ_AREA_AVAILABLE: {
	if (area->level == area->root->max_level ||
	    (area->width == width && area->height == height))
	{
	    return _cairo_glitz_area_move_in (area, closure);
	}
	else
	{
	    int dx[4], dy[4], w[4], h[4], i;
	    
	    dx[0] = dx[2] = dy[0] = dy[1] = 0;
	    
	    w[0] = w[2] = dx[1] = dx[3] = width;
	    h[0] = h[1] = dy[2] = dy[3] = height;

	    w[1] = w[3] = area->width - width;
	    h[2] = h[3] = area->height - height;
	    
	    for (i = 0; i < 2; i++)
	    {
		if (w[i])
		    area->area[i] =
			_cairo_glitz_area_create (area->root,
						  area->level + 1,
						  area->x + dx[i],
						  area->y + dy[i],
						  w[i], h[i]);
	    }

	    for (; i < 4; i++)
	    {
		if (w[i] && h[i])
		    area->area[i] =
			_cairo_glitz_area_create (area->root,
						  area->level + 1,
						  area->x + dx[i],
						  area->y + dy[i],
						  w[i], h[i]);
	    }

	    area->state = CAIRO_GLITZ_AREA_DIVIDED;
	    
	    status = _cairo_glitz_area_find (area->area[0],
					     width, height,
					     kick_out, closure);
	    if (status == CAIRO_STATUS_SUCCESS)
                return CAIRO_STATUS_SUCCESS;
	}
    } break;
    case CAIRO_GLITZ_AREA_DIVIDED: {
	cairo_glitz_area_t *to_area;
	int		   i, rejected = FALSE;

	for (i = 0; i < 4; i++)
	{
	    if (area->area[i])
	    {
		if (area->area[i]->width >= width &&
		    area->area[i]->height >= height)
		{
		    status = _cairo_glitz_area_find (area->area[i],
						     width, height,
						     kick_out, closure);
		    if (status == CAIRO_STATUS_SUCCESS)
			return CAIRO_STATUS_SUCCESS;
		    
		    rejected = TRUE;
		}
	    }
	}

	if (rejected)
	    return CAIRO_INT_STATUS_UNSUPPORTED;

	to_area = _cairo_glitz_area_get_top_scored_sub_area (area);
	if (to_area)
	{
	    if (kick_out)
	    {
		if ((*area->root->funcs->compare_score) (to_area,
							 to_area->closure,
							 closure) >= 0)
		    return CAIRO_INT_STATUS_UNSUPPORTED;
	    } else {
		return CAIRO_INT_STATUS_UNSUPPORTED;
	    }
	}

	for (i = 0; i < 4; i++)
	{
	    _cairo_glitz_area_destroy (area->area[i]);
	    area->area[i] = NULL;
	}
	
	area->closure = NULL;
	area->state   = CAIRO_GLITZ_AREA_AVAILABLE;

	status = _cairo_glitz_area_find (area, width, height,
					 TRUE, closure);
	if (status == CAIRO_STATUS_SUCCESS)
	    return CAIRO_STATUS_SUCCESS;
	
    } break;
    }

    return CAIRO_INT_STATUS_UNSUPPORTED;
}

static cairo_status_t
_cairo_glitz_root_area_init (cairo_glitz_root_area_t	    *root,
			     int			    max_level,
			     int			    width,
			     int			    height,
			     const cairo_glitz_area_funcs_t *funcs)
{
    root->max_level = max_level;
    root->funcs     = funcs;

    root->area = _cairo_glitz_area_create (root, 0, 0, 0, width, height);
    if (!root->area)
	return CAIRO_STATUS_NO_MEMORY;
    
    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_glitz_root_area_fini (cairo_glitz_root_area_t *root)
{
    _cairo_glitz_area_destroy (root->area);
}

#define GLYPH_CACHE_TEXTURE_SIZE 512
#define GLYPH_CACHE_MAX_LEVEL     64
#define GLYPH_CACHE_MAX_HEIGHT    72
#define GLYPH_CACHE_MAX_WIDTH     72

#define WRITE_VEC2(ptr, _x, _y) \
    *(ptr)++ = (_x);		\
    *(ptr)++ = (_y)

#define WRITE_BOX(ptr, _vx1, _vy1, _vx2, _vy2, p1, p2) \
    WRITE_VEC2 (ptr, _vx1, _vy1);		       \
    WRITE_VEC2 (ptr, (p1)->x, (p2)->y);		       \
    WRITE_VEC2 (ptr, _vx2, _vy1);		       \
    WRITE_VEC2 (ptr, (p2)->x, (p2)->y);		       \
    WRITE_VEC2 (ptr, _vx2, _vy2);		       \
    WRITE_VEC2 (ptr, (p2)->x, (p1)->y);		       \
    WRITE_VEC2 (ptr, _vx1, _vy2);		       \
    WRITE_VEC2 (ptr, (p1)->x, (p1)->y)

typedef struct _cairo_glitz_glyph_cache {
    cairo_cache_t	    base;
    cairo_glitz_root_area_t root;
    glitz_surface_t	    *surface;
} cairo_glitz_glyph_cache_t;

typedef struct {
    cairo_glyph_cache_key_t key;
    int			    ref_count;
    cairo_glyph_size_t	    size;
    cairo_glitz_area_t	    *area;
    cairo_bool_t	    locked;
    cairo_point_double_t    p1, p2;
} cairo_glitz_glyph_cache_entry_t;

static cairo_status_t
_cairo_glitz_glyph_move_in (cairo_glitz_area_t *area,
			    void	       *closure)
{
    cairo_glitz_glyph_cache_entry_t *entry = closure;
    
    entry->area = area;

    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_glitz_glyph_move_out (cairo_glitz_area_t	*area,
			     void	        *closure)
{
    cairo_glitz_glyph_cache_entry_t *entry = closure;
    
    entry->area = NULL;
}

static int
_cairo_glitz_glyph_compare (cairo_glitz_area_t *area,
			    void	       *closure1,
			    void	       *closure2)
{
    cairo_glitz_glyph_cache_entry_t *entry = closure1;
    
    if (entry->locked)
	return 1;

    return -1;
}

static const cairo_glitz_area_funcs_t _cairo_glitz_area_funcs = {
    _cairo_glitz_glyph_move_in,
    _cairo_glitz_glyph_move_out,
    _cairo_glitz_glyph_compare
};

static cairo_status_t 
_cairo_glitz_glyph_cache_create_entry (void *abstract_cache,
				       void *abstract_key,
				       void **return_entry)
{
    cairo_glitz_glyph_cache_entry_t *entry;
    cairo_glyph_cache_key_t	    *key = abstract_key;
   
    cairo_status_t status;
    cairo_cache_t *im_cache;
    cairo_image_glyph_cache_entry_t *im;

    unsigned long entry_memory = 0;

    entry = malloc (sizeof (cairo_glitz_glyph_cache_entry_t));
    if (!entry)
	return CAIRO_STATUS_NO_MEMORY;

    _cairo_lock_global_image_glyph_cache ();

    im_cache = _cairo_get_global_image_glyph_cache ();
    if (im_cache == NULL) {
	_cairo_unlock_global_image_glyph_cache ();
	free (entry);
	return CAIRO_STATUS_NO_MEMORY;
    }

    status = _cairo_cache_lookup (im_cache, key, (void **) (&im), NULL);
    if (status != CAIRO_STATUS_SUCCESS || im == NULL) {
	_cairo_unlock_global_image_glyph_cache ();
	free (entry);
	return CAIRO_STATUS_NO_MEMORY;
    }

    if (im->image)
	entry_memory = im->image->width * im->image->stride;

    _cairo_unlock_global_image_glyph_cache ();

    entry->ref_count = 1;
    entry->key	    = *key;
    entry->key.base.memory = entry_memory;
    entry->area	    = NULL;
    entry->locked   = FALSE;

    _cairo_unscaled_font_reference (entry->key.unscaled);
    
    *return_entry = entry;

    return CAIRO_STATUS_SUCCESS;
}

static void 
_cairo_glitz_glyph_cache_destroy_entry (void *abstract_cache,
					void *abstract_entry)
{
    cairo_glitz_glyph_cache_entry_t *entry = abstract_entry;

    entry->ref_count--;
    if (entry->ref_count)
	return;

    if (entry->area)
	_cairo_glitz_area_move_out (entry->area);
    
    _cairo_unscaled_font_destroy (entry->key.unscaled);

    free (entry);	
}

static void 
_cairo_glitz_glyph_cache_entry_reference (void *abstract_entry)
{
    cairo_glitz_glyph_cache_entry_t *entry = abstract_entry;

    entry->ref_count++;	
}

static void 
_cairo_glitz_glyph_cache_destroy_cache (void *abstract_cache)
{
    cairo_glitz_glyph_cache_t *cache = abstract_cache;

    _cairo_glitz_root_area_fini (&cache->root);
	
    glitz_surface_destroy (cache->surface);
}

static const cairo_cache_backend_t _cairo_glitz_glyph_cache_backend = {
    _cairo_glyph_cache_hash,
    _cairo_glyph_cache_keys_equal,
    _cairo_glitz_glyph_cache_create_entry,
    _cairo_glitz_glyph_cache_destroy_entry,
    _cairo_glitz_glyph_cache_destroy_cache
};

static cairo_glitz_glyph_cache_t *_cairo_glitz_glyph_caches = NULL;

static cairo_glitz_glyph_cache_t *
_cairo_glitz_get_glyph_cache (cairo_glitz_surface_t *surface)
{
    cairo_glitz_glyph_cache_t *cache;
    glitz_drawable_t	      *drawable;
    glitz_format_t	      *format;

    /* 
     * FIXME: caches should be thread specific, display specific and screen
     * specific. 
     */
    
    if (_cairo_glitz_glyph_caches)
	return _cairo_glitz_glyph_caches;

    drawable = glitz_surface_get_drawable (surface->surface);
    
    format = glitz_find_standard_format (drawable, GLITZ_STANDARD_A8);
    if (!format)
	return NULL;
    
    cache = malloc (sizeof (cairo_glitz_glyph_cache_t));
    if (!cache)
	return NULL;

    cache->surface =
	glitz_surface_create (drawable, format,
			      GLYPH_CACHE_TEXTURE_SIZE,
			      GLYPH_CACHE_TEXTURE_SIZE,
			      0, NULL);
    if (cache->surface == NULL)
    {
	free (cache);
	return NULL;
    }

    if (_cairo_glitz_root_area_init (&cache->root,
				     GLYPH_CACHE_MAX_LEVEL,
				     GLYPH_CACHE_TEXTURE_SIZE,
				     GLYPH_CACHE_TEXTURE_SIZE,
				     &_cairo_glitz_area_funcs))
    {
	glitz_surface_destroy (cache->surface);
	free (cache);
	return NULL;
    }

    if (_cairo_cache_init (&cache->base,
			   &_cairo_glitz_glyph_cache_backend,
			   CAIRO_GLITZ_GLYPH_CACHE_MEMORY_DEFAULT))
    {
	_cairo_glitz_root_area_fini (&cache->root);
	glitz_surface_destroy (cache->surface);
	free (cache);
	return NULL;
    }
    
    _cairo_glitz_glyph_caches = cache;
    
    return cache;
}

#define FIXED_TO_FLOAT(f) (((glitz_float_t) (f)) / 65536)

static cairo_status_t
_cairo_glitz_cache_glyph (cairo_glitz_glyph_cache_t	  *cache,
			  cairo_glitz_glyph_cache_entry_t *entry,
			  cairo_image_glyph_cache_entry_t *image_entry)
{
    glitz_point_fixed_t  p1, p2;
    glitz_pixel_format_t pf;
    glitz_buffer_t	 *buffer;
    pixman_format_t	 *format;
    int			 am, rm, gm, bm;

    entry->size = image_entry->size;
    
    if (entry->size.width  > GLYPH_CACHE_MAX_WIDTH ||
	entry->size.height > GLYPH_CACHE_MAX_HEIGHT)
	return CAIRO_STATUS_SUCCESS;

    if ((entry->size.width  == 0 && entry->size.height == 0) ||
        !image_entry->image)
    {
	entry->area = &_empty_area;
	return CAIRO_STATUS_SUCCESS;
    }
    
    format = pixman_image_get_format (image_entry->image->pixman_image);
    if (!format)
	return CAIRO_STATUS_NO_MEMORY;
	
    if (_cairo_glitz_area_find (cache->root.area,
				entry->size.width,
				entry->size.height,
				FALSE, entry))
    {
	if (_cairo_glitz_area_find (cache->root.area,
				    entry->size.width,
				    entry->size.height,
				    TRUE, entry))
	    return CAIRO_STATUS_SUCCESS;
    }
    
    buffer = glitz_buffer_create_for_data (image_entry->image->data);
    if (!buffer)
    {
	_cairo_glitz_area_move_out (entry->area);
	return CAIRO_STATUS_NO_MEMORY;
    }

    pixman_format_get_masks (format, &pf.masks.bpp, &am, &rm, &gm, &bm);

    pf.masks.alpha_mask = am;
    pf.masks.red_mask   = rm;
    pf.masks.green_mask = gm;
    pf.masks.blue_mask  = bm;
    
    pf.bytes_per_line = image_entry->image->stride;
    pf.scanline_order = GLITZ_PIXEL_SCANLINE_ORDER_BOTTOM_UP;
    pf.xoffset	      = 0;
    pf.skip_lines     = 0;
    
    glitz_set_pixels (cache->surface,
		      entry->area->x,
		      entry->area->y,
		      entry->size.width,
		      entry->size.height,
		      &pf, buffer);

    glitz_buffer_destroy (buffer);
	
    p1.x = entry->area->x << 16;
    p1.y = entry->area->y << 16;
    p2.x = (entry->area->x + entry->size.width)  << 16;
    p2.y = (entry->area->y + entry->size.height) << 16;
    
    glitz_surface_translate_point (cache->surface, &p1, &p1);
    glitz_surface_translate_point (cache->surface, &p2, &p2);
    
    entry->p1.x = FIXED_TO_FLOAT (p1.x);
    entry->p1.y = FIXED_TO_FLOAT (p1.y);
    entry->p2.x = FIXED_TO_FLOAT (p2.x);
    entry->p2.y = FIXED_TO_FLOAT (p2.y);
    
    return CAIRO_STATUS_SUCCESS;
}

#define N_STACK_BUF 256

static cairo_int_status_t
_cairo_glitz_surface_show_glyphs (cairo_scaled_font_t *scaled_font,
				  cairo_operator_t    op,
				  cairo_pattern_t     *pattern,
				  void		      *abstract_surface,
				  int		      src_x,
				  int		      src_y,
				  int		      dst_x,
				  int		      dst_y,
				  unsigned int	      width,
				  unsigned int	      height,
				  const cairo_glyph_t *glyphs,
				  int		      num_glyphs)
{
    cairo_glitz_surface_attributes_t attributes;
    cairo_glitz_surface_t	     *dst = abstract_surface;
    cairo_glitz_surface_t	     *src;
    cairo_glitz_glyph_cache_t	     *cache;
    cairo_glitz_glyph_cache_entry_t  *stack_entries[N_STACK_BUF];
    cairo_glitz_glyph_cache_entry_t  **entries;
    cairo_glyph_cache_key_t	     key;
    glitz_float_t		     stack_vertices[N_STACK_BUF * 16];
    glitz_float_t		     *vertices;
    glitz_buffer_t		     *buffer;
    cairo_int_status_t		     status;
    int				     i, cached_glyphs = 0;
    int				     remaining_glyps = num_glyphs;
    glitz_float_t		     x1, y1, x2, y2;
    static glitz_vertex_format_t     format = {
	GLITZ_PRIMITIVE_QUADS,
	GLITZ_DATA_TYPE_FLOAT,
	sizeof (glitz_float_t) * 4,
	GLITZ_VERTEX_ATTRIBUTE_MASK_COORD_MASK,
	{ 0 },
	{
	    GLITZ_DATA_TYPE_FLOAT,
	    GLITZ_COORDINATE_SIZE_XY,
	    sizeof (glitz_float_t) * 2,
	}
    };

    if (op == CAIRO_OPERATOR_SATURATE)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (_glitz_ensure_target (dst->surface))
	return CAIRO_INT_STATUS_UNSUPPORTED;

    status = _cairo_glitz_pattern_acquire_surface (pattern, dst,
						   src_x, src_y,
						   width, height,
						   &src, &attributes);
    if (status)
	return status;

    _cairo_glitz_surface_set_attributes (src, &attributes);

    if (num_glyphs > N_STACK_BUF)
    {
	char *data;
	
	data = malloc (num_glyphs * sizeof (void *) +
		       num_glyphs * sizeof (glitz_float_t) * 16);
	if (!data)
	    goto FAIL1;
	
	entries  = (cairo_glitz_glyph_cache_entry_t **) data;
	vertices = (glitz_float_t *) (data + num_glyphs * sizeof (void *));
    }
    else
    {
	entries  = stack_entries;
	vertices = stack_vertices;
    }

    buffer = glitz_buffer_create_for_data (vertices);
    if (!buffer)
	goto FAIL2;
    
    cache = _cairo_glitz_get_glyph_cache (dst);
    if (!cache)
    {
	num_glyphs = 0;
	goto UNLOCK;
    }

    status = _cairo_scaled_font_get_glyph_cache_key (scaled_font, &key);
    if (status)
	goto UNLOCK;

    for (i = 0; i < num_glyphs; i++)
    {
	key.index = glyphs[i].index;
	
	status = _cairo_cache_lookup (&cache->base,
				      &key,
				      (void **) &entries[i],
				      NULL);
	if (status)
	{
	    num_glyphs = i;
	    goto UNLOCK;
	}

	_cairo_glitz_glyph_cache_entry_reference (entries[i]);

	if (entries[i]->area)
	{
	    remaining_glyps--;

	    if (entries[i]->area->width)
	    {
		x1 = floor (glyphs[i].x + 0.5) + entries[i]->size.x;
		y1 = floor (glyphs[i].y + 0.5) + entries[i]->size.y;
		x2 = x1 + entries[i]->size.width;
		y2 = y1 + entries[i]->size.height;
	    
		WRITE_BOX (vertices, x1, y1, x2, y2,
			   &entries[i]->p1, &entries[i]->p2);
	    
		entries[i]->locked = TRUE;
		cached_glyphs++;
	    }
	}
    }

    if (remaining_glyps)
    {
	cairo_cache_t			*image_cache;
	cairo_image_glyph_cache_entry_t *image_entry;
	cairo_surface_t			*image;
	cairo_glitz_surface_t		*clone;
	
	_cairo_lock_global_image_glyph_cache ();

	image_cache = _cairo_get_global_image_glyph_cache ();
	if (!image_cache)
	{
	    _cairo_unlock_global_image_glyph_cache ();
	    status = CAIRO_STATUS_NO_MEMORY;
	    goto UNLOCK;
	}
	
	for (i = 0; i < num_glyphs; i++)
	{
	    if (!entries[i]->area)
	    {
		key.index = glyphs[i].index;
		
		status = _cairo_cache_lookup (image_cache,
					      &key,
					      (void **) &image_entry,
					      NULL);
		if (status)
		{
		    _cairo_unlock_global_image_glyph_cache ();
		    goto UNLOCK;
		}

		status = _cairo_glitz_cache_glyph (cache,
						   entries[i],
						   image_entry);
		if (status)
		{
		    _cairo_unlock_global_image_glyph_cache ();
		    goto UNLOCK;
		}
	    }
	    
	    x1 = floor (glyphs[i].x + 0.5);
	    y1 = floor (glyphs[i].y + 0.5);

	    if (entries[i]->area)
	    {
		if (entries[i]->area->width)
		{
		    x1 += entries[i]->size.x;
		    y1 += entries[i]->size.y;
		    x2 = x1 + entries[i]->size.width;
		    y2 = y1 + entries[i]->size.height;
		    
		    WRITE_BOX (vertices, x1, y1, x2, y2,
			       &entries[i]->p1, &entries[i]->p2);
		    
		    entries[i]->locked = TRUE;
		    cached_glyphs++;
		}
	    }
	    else
	    {
		x1 += image_entry->size.x;
		y1 += image_entry->size.y;

		if (!image_entry->image)
		    continue;
		
		image = &image_entry->image->base;
		status =
		    _cairo_glitz_surface_clone_similar (abstract_surface,
							image,
							(cairo_surface_t **)
							&clone);
		if (status)
		{
		    _cairo_unlock_global_image_glyph_cache ();
		    goto UNLOCK;
		}

		glitz_composite (_glitz_operator (op), 
				 src->surface, 
				 clone->surface,
				 dst->surface,
				 src_x + attributes.base.x_offset + x1,
				 src_y + attributes.base.y_offset + y1,
				 0, 0,
				 x1, y1,
				 image_entry->size.width,
				 image_entry->size.height);

		cairo_surface_destroy (&clone->base);

		if (glitz_surface_get_status (dst->surface) ==
		    GLITZ_STATUS_NOT_SUPPORTED)
		{
		    status = CAIRO_INT_STATUS_UNSUPPORTED;
		    _cairo_unlock_global_image_glyph_cache ();
		    goto UNLOCK;
		}
	    }
	}
	
	_cairo_unlock_global_image_glyph_cache ();
    }

    if (cached_glyphs)
    {
	glitz_set_geometry (dst->surface,
			    GLITZ_GEOMETRY_TYPE_VERTEX,
			    (glitz_geometry_format_t *) &format,
			    buffer);

	glitz_set_array (dst->surface, 0, 4, cached_glyphs * 4, 0, 0);

	glitz_composite (_glitz_operator (op),
			 src->surface,
			 cache->surface,
			 dst->surface,
			 src_x + attributes.base.x_offset,
			 src_y + attributes.base.y_offset,
			 0, 0,
			 dst_x, dst_y,
			 width, height);

	glitz_set_geometry (dst->surface,
			    GLITZ_GEOMETRY_TYPE_NONE,
			    NULL, NULL);
    }
    
UNLOCK:
    if (cached_glyphs)
    {
	for (i = 0; i < num_glyphs; i++)
	    entries[i]->locked = FALSE;
    }
    
    for (i = 0; i < num_glyphs; i++)
	_cairo_glitz_glyph_cache_destroy_entry (cache, entries[i]);

    glitz_buffer_destroy (buffer);

 FAIL2:
    if (num_glyphs > N_STACK_BUF)
	free (entries);

 FAIL1:
    if (attributes.n_params)
	free (attributes.params);

    _cairo_glitz_pattern_release_surface (pattern, src, &attributes);

    if (status)
	return status;
    
    if (glitz_surface_get_status (dst->surface) == GLITZ_STATUS_NOT_SUPPORTED)
	return CAIRO_INT_STATUS_UNSUPPORTED;
    
    return CAIRO_STATUS_SUCCESS;
}

static const cairo_surface_backend_t cairo_glitz_surface_backend = {
    _cairo_glitz_surface_create_similar,
    _cairo_glitz_surface_finish,
    _cairo_glitz_surface_acquire_source_image,
    _cairo_glitz_surface_release_source_image,
    _cairo_glitz_surface_acquire_dest_image,
    _cairo_glitz_surface_release_dest_image,
    _cairo_glitz_surface_clone_similar,
    _cairo_glitz_surface_composite,
    _cairo_glitz_surface_fill_rectangles,
    _cairo_glitz_surface_composite_trapezoids,
    NULL, /* copy_page */
    NULL, /* show_page */
    _cairo_glitz_surface_set_clip_region,
    NULL, /* intersect_clip_path */
    _cairo_glitz_surface_get_extents,
    _cairo_glitz_surface_show_glyphs
};

cairo_surface_t *
cairo_glitz_surface_create (glitz_surface_t *surface)
{
    cairo_glitz_surface_t *crsurface;

    if (surface == NULL)
	return (cairo_surface_t*) &_cairo_surface_nil;

    crsurface = malloc (sizeof (cairo_glitz_surface_t));
    if (crsurface == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_surface_t*) &_cairo_surface_nil;
    }

    _cairo_surface_init (&crsurface->base, &cairo_glitz_surface_backend);

    glitz_surface_reference (surface);
    
    crsurface->surface = surface;
    crsurface->format  = glitz_surface_get_format (surface);
    crsurface->clip    = NULL;
    
    return (cairo_surface_t *) crsurface;
}
