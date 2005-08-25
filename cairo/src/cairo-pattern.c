/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2004 David Reveman
 * Copyright © 2005 Red Hat, Inc.
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

typedef struct _cairo_shader_color_stop {
    cairo_fixed_t	offset;
    cairo_fixed_48_16_t scale;
    int			id;
    unsigned char	color_char[4];
} cairo_shader_color_stop_t;

typedef struct _cairo_shader_op {
    cairo_shader_color_stop_t *stops;
    int			      n_stops;
    cairo_extend_t	      extend;
} cairo_shader_op_t;

#define MULTIPLY_COLORCOMP(c1, c2) \
    ((unsigned char) \
     ((((unsigned char) (c1)) * (int) ((unsigned char) (c2))) / 0xff))

const cairo_solid_pattern_t cairo_pattern_nil = {
    { CAIRO_PATTERN_SOLID, 	/* type */
      (unsigned int)-1,		/* ref_count */
      CAIRO_STATUS_NO_MEMORY,	/* status */
      { 1., 0., 0., 1., 0., 0., }, /* matrix */
      CAIRO_FILTER_DEFAULT,	/* filter */
      CAIRO_EXTEND_DEFAULT },	/* extend */
};

static const cairo_solid_pattern_t cairo_pattern_nil_null_pointer = {
    { CAIRO_PATTERN_SOLID, 	/* type */
      (unsigned int)-1,		/* ref_count */
      CAIRO_STATUS_NULL_POINTER,/* status */
      { 1., 0., 0., 1., 0., 0., }, /* matrix */
      CAIRO_FILTER_DEFAULT,	/* filter */
      CAIRO_EXTEND_DEFAULT },	/* extend */
};

static const cairo_solid_pattern_t cairo_pattern_nil_file_not_found = {
    { CAIRO_PATTERN_SOLID, 	/* type */
      (unsigned int)-1,		/* ref_count */
      CAIRO_STATUS_FILE_NOT_FOUND, /* status */
      { 1., 0., 0., 1., 0., 0., }, /* matrix */
      CAIRO_FILTER_DEFAULT,	/* filter */
      CAIRO_EXTEND_DEFAULT },	/* extend */
};

static const cairo_solid_pattern_t cairo_pattern_nil_read_error = {
    { CAIRO_PATTERN_SOLID, 	/* type */
      (unsigned int)-1,		/* ref_count */
      CAIRO_STATUS_READ_ERROR,	/* status */
      { 1., 0., 0., 1., 0., 0., }, /* matrix */
      CAIRO_FILTER_DEFAULT,	/* filter */
      CAIRO_EXTEND_DEFAULT },	/* extend */
};

static const cairo_pattern_t *
_cairo_pattern_nil_for_status (cairo_status_t status)
{
    switch (status) {
    case CAIRO_STATUS_NULL_POINTER:
	return &cairo_pattern_nil_null_pointer.base;
    case CAIRO_STATUS_FILE_NOT_FOUND:
	return &cairo_pattern_nil_file_not_found.base;
    case CAIRO_STATUS_READ_ERROR:
	return &cairo_pattern_nil_read_error.base;
    default:
    case CAIRO_STATUS_NO_MEMORY:
	return &cairo_pattern_nil.base;
    }
}

/**
 * _cairo_pattern_set_error:
 * @pattern: a pattern
 * @status: a status value indicating an error, (eg. not
 * CAIRO_STATUS_SUCCESS)
 * 
 * Sets pattern->status to @status and calls _cairo_error;
 *
 * All assignments of an error status to pattern->status should happen
 * through _cairo_pattern_set_error() or else _cairo_error() should be
 * called immediately after the assignment.
 *
 * The purpose of this function is to allow the user to set a
 * breakpoint in _cairo_error() to generate a stack trace for when the
 * user causes cairo to detect an error.
 **/
static void
_cairo_pattern_set_error (cairo_pattern_t *pattern,
			  cairo_status_t status)
{
    /* Don't overwrite an existing error. This preserves the first
     * error, which is the most significant. It also avoids attempting
     * to write to read-only data (eg. from a nil pattern). */
    if (pattern->status == CAIRO_STATUS_SUCCESS)
	pattern->status = status;

    _cairo_error (status);
}

static void
_cairo_pattern_init (cairo_pattern_t *pattern, cairo_pattern_type_t type)
{
    pattern->type      = type; 
    pattern->ref_count = 1;
    pattern->status    = CAIRO_STATUS_SUCCESS;
    pattern->extend    = CAIRO_EXTEND_DEFAULT;
    pattern->filter    = CAIRO_FILTER_DEFAULT;

    cairo_matrix_init_identity (&pattern->matrix);
}

static void
_cairo_gradient_pattern_init_copy (cairo_gradient_pattern_t	  *pattern,
				   const cairo_gradient_pattern_t *other)
{
    if (other->base.type == CAIRO_PATTERN_LINEAR)
    {
	cairo_linear_pattern_t *dst = (cairo_linear_pattern_t *) pattern;
	cairo_linear_pattern_t *src = (cairo_linear_pattern_t *) other;
	
	*dst = *src;
    }
    else
    {
	cairo_radial_pattern_t *dst = (cairo_radial_pattern_t *) pattern;
	cairo_radial_pattern_t *src = (cairo_radial_pattern_t *) other;
	
	*dst = *src;
    }

    if (other->n_stops)
    {
	pattern->stops = malloc (other->n_stops * sizeof (cairo_color_stop_t));
	if (pattern->stops == NULL) {
	    _cairo_pattern_set_error (&pattern->base, CAIRO_STATUS_NO_MEMORY);
	    return;
	}
	
	memcpy (pattern->stops, other->stops,
		other->n_stops * sizeof (cairo_color_stop_t));
    }
}

void
_cairo_pattern_init_copy (cairo_pattern_t	*pattern,
			  const cairo_pattern_t *other)
{
    if (other->status) {
	_cairo_pattern_set_error (pattern, other->status);
	return;
    }

    switch (other->type) {
    case CAIRO_PATTERN_SOLID: {
	cairo_solid_pattern_t *dst = (cairo_solid_pattern_t *) pattern;
	cairo_solid_pattern_t *src = (cairo_solid_pattern_t *) other;

	*dst = *src;
    } break;
    case CAIRO_PATTERN_SURFACE: {
	cairo_surface_pattern_t *dst = (cairo_surface_pattern_t *) pattern;
	cairo_surface_pattern_t *src = (cairo_surface_pattern_t *) other;
	
	*dst = *src;
	cairo_surface_reference (dst->surface);
    } break;
    case CAIRO_PATTERN_LINEAR:
    case CAIRO_PATTERN_RADIAL: {
	cairo_gradient_pattern_t *dst = (cairo_gradient_pattern_t *) pattern;
	cairo_gradient_pattern_t *src = (cairo_gradient_pattern_t *) other;
	
	_cairo_gradient_pattern_init_copy (dst, src);
    } break;
    }
    
    pattern->ref_count = 1;
}

void
_cairo_pattern_fini (cairo_pattern_t *pattern)
{
    switch (pattern->type) {
    case CAIRO_PATTERN_SOLID:
	break;
    case CAIRO_PATTERN_SURFACE: {
	cairo_surface_pattern_t *surface_pattern =
	    (cairo_surface_pattern_t *) pattern;
	
	cairo_surface_destroy (surface_pattern->surface);
    } break;
    case CAIRO_PATTERN_LINEAR:
    case CAIRO_PATTERN_RADIAL: {
	cairo_gradient_pattern_t *gradient =
	    (cairo_gradient_pattern_t *) pattern;
	
	if (gradient->stops)
	    free (gradient->stops);
    } break;
    }
}

void
_cairo_pattern_init_solid (cairo_solid_pattern_t *pattern,
			   const cairo_color_t	 *color)
{
    _cairo_pattern_init (&pattern->base, CAIRO_PATTERN_SOLID);
    pattern->color = *color;
}

void
_cairo_pattern_init_for_surface (cairo_surface_pattern_t *pattern,
				 cairo_surface_t	 *surface)
{
    if (surface->status) {
	/* Force to solid to simplify the pattern_fini process. */
	pattern->base.type = CAIRO_PATTERN_SOLID;
	_cairo_pattern_set_error (&pattern->base, surface->status);
	return;
    }

    _cairo_pattern_init (&pattern->base, CAIRO_PATTERN_SURFACE);
    
    pattern->surface = cairo_surface_reference (surface);
}

static void
_cairo_pattern_init_gradient (cairo_gradient_pattern_t *pattern,
			      cairo_pattern_type_t     type)
{
    _cairo_pattern_init (&pattern->base, type);

    pattern->stops   = NULL;
    pattern->n_stops = 0;
}

void
_cairo_pattern_init_linear (cairo_linear_pattern_t *pattern,
			    double x0, double y0, double x1, double y1)
{
    _cairo_pattern_init_gradient (&pattern->base, CAIRO_PATTERN_LINEAR);
    
    pattern->point0.x = x0;
    pattern->point0.y = y0;
    pattern->point1.x = x1;
    pattern->point1.y = y1;
}

void
_cairo_pattern_init_radial (cairo_radial_pattern_t *pattern,
			    double cx0, double cy0, double radius0,
			    double cx1, double cy1, double radius1)
{
    _cairo_pattern_init_gradient (&pattern->base, CAIRO_PATTERN_RADIAL);

    pattern->center0.x = cx0;
    pattern->center0.y = cy0;
    pattern->radius0   = fabs (radius0);
    pattern->center1.x = cx1;
    pattern->center1.y = cy1;
    pattern->radius1   = fabs (radius1);
}

cairo_pattern_t *
_cairo_pattern_create_solid (const cairo_color_t *color)
{
    cairo_solid_pattern_t *pattern;

    pattern = malloc (sizeof (cairo_solid_pattern_t));
    if (pattern == NULL)
	return (cairo_pattern_t *) &cairo_pattern_nil.base;

    _cairo_pattern_init_solid (pattern, color);

    return &pattern->base;
}

/**
 * cairo_pattern_create_rgb:
 * @red: red component of the color
 * @green: green component of the color
 * @blue: blue component of the color
 * 
 * Creates a new cairo_pattern_t corresponding to an opaque color.  The
 * color components are floating point numbers in the range 0 to 1.
 * If the values passed in are outside that range, they will be
 * clamped.
 * 
 * Return value: the newly created #cairo_pattern_t if succesful, or
 * an error pattern in case of no memory.  The caller owns the
 * returned object and should call cairo_pattern_destroy() when
 * finished with it.
 *
 * This function will always return a valid pointer, but if an error
 * occurred the pattern status will be set to an error.  To inspect
 * the status of a pattern use cairo_pattern_status().
 **/
cairo_pattern_t *
cairo_pattern_create_rgb (double red, double green, double blue)
{
    cairo_pattern_t *pattern;
    cairo_color_t color;

    _cairo_restrict_value (&red,   0.0, 1.0);
    _cairo_restrict_value (&green, 0.0, 1.0);
    _cairo_restrict_value (&blue,  0.0, 1.0);

    _cairo_color_init_rgb (&color, red, green, blue);

    pattern = _cairo_pattern_create_solid (&color);
    if (pattern->status)
	_cairo_error (pattern->status);

    return pattern;
}

/**
 * cairo_pattern_create_rgba:
 * @red: red component of the color
 * @green: green component of the color
 * @blue: blue component of the color
 * @alpha: alpha component of the color
 * 
 * Creates a new cairo_pattern_t corresponding to a translucent color.
 * The color components are floating point numbers in the range 0 to
 * 1.  If the values passed in are outside that range, they will be
 * clamped.
 * 
 * Return value: the newly created #cairo_pattern_t if succesful, or
 * an error pattern in case of no memory.  The caller owns the
 * returned object and should call cairo_pattern_destroy() when
 * finished with it.
 *
 * This function will always return a valid pointer, but if an error
 * occurred the pattern status will be set to an error.  To inspect
 * the status of a pattern use cairo_pattern_status().
 **/
cairo_pattern_t *
cairo_pattern_create_rgba (double red, double green, double blue,
			   double alpha)
{
    cairo_pattern_t *pattern;
    cairo_color_t color;

    _cairo_restrict_value (&red,   0.0, 1.0);
    _cairo_restrict_value (&green, 0.0, 1.0);
    _cairo_restrict_value (&blue,  0.0, 1.0);
    _cairo_restrict_value (&alpha, 0.0, 1.0);

    _cairo_color_init_rgba (&color, red, green, blue, alpha);

    pattern = _cairo_pattern_create_solid (&color);
    if (pattern->status)
	_cairo_error (pattern->status);

    return pattern;
}

/**
 * cairo_pattern_create_for_surface:
 * @surface: the surface 
 * 
 * Create a new cairo_pattern_t for the given surface.
 * 
 * Return value: the newly created #cairo_pattern_t if succesful, or
 * an error pattern in case of no memory.  The caller owns the
 * returned object and should call cairo_pattern_destroy() when
 * finished with it.
 *
 * This function will always return a valid pointer, but if an error
 * occurred the pattern status will be set to an error.  To inspect
 * the status of a pattern use cairo_pattern_status().
 **/
cairo_pattern_t *
cairo_pattern_create_for_surface (cairo_surface_t *surface)
{
    cairo_surface_pattern_t *pattern;

    if (surface == NULL)
	return (cairo_pattern_t*) _cairo_pattern_nil_for_status (CAIRO_STATUS_NULL_POINTER);

    if (surface->status)
	return (cairo_pattern_t*) _cairo_pattern_nil_for_status (surface->status);

    pattern = malloc (sizeof (cairo_surface_pattern_t));
    if (pattern == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_pattern_t *)&cairo_pattern_nil.base;
    }

    _cairo_pattern_init_for_surface (pattern, surface);

    return &pattern->base;
}

/**
 * cairo_pattern_create_linear:
 * @x0: x coordinate of the start point 
 * @y0: y coordinate of the start point 
 * @x1: x coordinate of the end point 
 * @y1: y coordinate of the end point 
 * 
 * Create a new linear gradient cairo_pattern_t along the line defined
 * by (x0, y0) and (x1, y1).  Before using the gradient pattern, a
 * number of color stops should be defined using
 * cairo_pattern_add_color_stop_rgb() or
 * cairo_pattern_add_color_stop_rgba().
 *
 * Note: The coordinates here are in pattern space. For a new pattern,
 * pattern space is identical to user space, but the relationship
 * between the spaces can be changed with cairo_pattern_set_matrix().
 * 
 * Return value: the newly created #cairo_pattern_t if succesful, or
 * an error pattern in case of no memory.  The caller owns the
 * returned object and should call cairo_pattern_destroy() when
 * finished with it.
 *
 * This function will always return a valid pointer, but if an error
 * occurred the pattern status will be set to an error.  To inspect
 * the status of a pattern use cairo_pattern_status().
 **/
cairo_pattern_t *
cairo_pattern_create_linear (double x0, double y0, double x1, double y1)
{
    cairo_linear_pattern_t *pattern;

    pattern = malloc (sizeof (cairo_linear_pattern_t));
    if (pattern == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_pattern_t *) &cairo_pattern_nil.base;
    }

    _cairo_pattern_init_linear (pattern, x0, y0, x1, y1);

    return &pattern->base.base;
}

/**
 * cairo_pattern_create_radial:
 * @cx0: x coordinate for the center of the start circle
 * @cy0: y coordinate for the center of the start circle
 * @radius0: radius of the start cirle
 * @cx1: x coordinate for the center of the end circle
 * @cy1: y coordinate for the center of the end circle
 * @radius1: radius of the end cirle
 * 
 * Creates a new radial gradient cairo_pattern_t between the two
 * circles defined by (x0, y0, c0) and (x1, y1, c0).  Before using the
 * gradient pattern, a number of color stops should be defined using
 * cairo_pattern_add_color_stop_rgb() or
 * cairo_pattern_add_color_stop_rgba().
 *
 * Note: The coordinates here are in pattern space. For a new pattern,
 * pattern space is identical to user space, but the relationship
 * between the spaces can be changed with cairo_pattern_set_matrix().
 * 
 * Return value: the newly created #cairo_pattern_t if succesful, or
 * an error pattern in case of no memory.  The caller owns the
 * returned object and should call cairo_pattern_destroy() when
 * finished with it.
 *
 * This function will always return a valid pointer, but if an error
 * occurred the pattern status will be set to an error.  To inspect
 * the status of a pattern use cairo_pattern_status().
 **/
cairo_pattern_t *
cairo_pattern_create_radial (double cx0, double cy0, double radius0,
			     double cx1, double cy1, double radius1)
{
    cairo_radial_pattern_t *pattern;
    
    pattern = malloc (sizeof (cairo_radial_pattern_t));
    if (pattern == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return (cairo_pattern_t *) &cairo_pattern_nil.base;
    }

    _cairo_pattern_init_radial (pattern, cx0, cy0, radius0, cx1, cy1, radius1);

    return &pattern->base.base;
}

/**
 * cairo_pattern_reference:
 * @pattern: a #cairo_pattern_t
 * 
 * Increases the reference count on @pattern by one. This prevents
 * @pattern from being destroyed until a matching call to
 * cairo_pattern_destroy() is made.
 *
 * Return value: the referenced #cairo_pattern_t.
 **/
cairo_pattern_t *
cairo_pattern_reference (cairo_pattern_t *pattern)
{
    if (pattern == NULL)
	return NULL;

    if (pattern->ref_count == (unsigned int)-1)
	return pattern;

    assert (pattern->ref_count > 0);

    pattern->ref_count++;

    return pattern;
}

/**
 * cairo_pattern_status:
 * @pattern: a #cairo_pattern_t
 * 
 * Checks whether an error has previously occurred for this
 * pattern.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS, %CAIRO_STATUS_NO_MEMORY, or
 * %CAIRO_STATUS_PATTERN_TYPE_MISMATCH.
 **/
cairo_status_t
cairo_pattern_status (cairo_pattern_t *pattern)
{
    return pattern->status;
}

/**
 * cairo_pattern_destroy:
 * @pattern: a #cairo_pattern_t
 * 
 * Decreases the reference count on @pattern by one. If the result is
 * zero, then @pattern and all associated resources are freed.  See
 * cairo_pattern_reference().
 **/
void
cairo_pattern_destroy (cairo_pattern_t *pattern)
{
    if (pattern == NULL)
	return;

    if (pattern->ref_count == (unsigned int)-1)
	return;

    assert (pattern->ref_count > 0);

    pattern->ref_count--;
    if (pattern->ref_count)
	return;

    _cairo_pattern_fini (pattern);
    free (pattern);
}

static void
_cairo_pattern_add_color_stop (cairo_gradient_pattern_t *pattern,
			       double			 offset,
			       cairo_color_t		*color)
{
    cairo_color_stop_t *stop;
    cairo_color_stop_t *new_stops;

    pattern->n_stops++;
    new_stops = realloc (pattern->stops,
			 pattern->n_stops * sizeof (cairo_color_stop_t));
    if (new_stops == NULL) {
	_cairo_pattern_set_error (&pattern->base, CAIRO_STATUS_NO_MEMORY);
	return;
    }
    
    pattern->stops = new_stops;

    stop = &pattern->stops[pattern->n_stops - 1];

    stop->offset = _cairo_fixed_from_double (offset);
    stop->color = *color;
}

/**
 * cairo_pattern_add_color_stop_rgb:
 * @pattern: a #cairo_pattern_t
 * @offset: an offset in the range [0.0 .. 1.0]
 * @red: red component of color
 * @green: green component of color
 * @blue: blue component of color
 * 
 * Adds an opaque color stop to a gradient pattern. The offset
 * specifies the location along the gradient's control vector. For
 * example, a linear gradient's control vector is from (x0,y0) to
 * (x1,y1) while a radial gradient's control vector is from any point
 * on the start circle to the corresponding point on the end circle.
 *
 * The color is specified in the same way as in cairo_set_source_rgb().
 *
 * Note: If the pattern is not a gradient pattern, (eg. a linear or
 * radial pattern), then the pattern will be put into an error status
 * with a status of %CAIRO_STATUS_PATTERN_TYPE_MISMATCH.
 **/
void
cairo_pattern_add_color_stop_rgb (cairo_pattern_t *pattern,
				  double	   offset,
				  double	   red,
				  double	   green,
				  double	   blue)
{
    cairo_color_t color;

    if (pattern->status)
	return;

    if (pattern->type != CAIRO_PATTERN_LINEAR &&
	pattern->type != CAIRO_PATTERN_RADIAL)
    {
	_cairo_pattern_set_error (pattern, CAIRO_STATUS_PATTERN_TYPE_MISMATCH);
	return;
    }

    _cairo_restrict_value (&offset, 0.0, 1.0);
    _cairo_restrict_value (&red,    0.0, 1.0);
    _cairo_restrict_value (&green,  0.0, 1.0);
    _cairo_restrict_value (&blue,   0.0, 1.0);

    _cairo_color_init_rgb (&color, red, green, blue);
    _cairo_pattern_add_color_stop ((cairo_gradient_pattern_t *) pattern,
				   offset,
				   &color);
}

/**
 * cairo_pattern_add_color_stop_rgba:
 * @pattern: a #cairo_pattern_t
 * @offset: an offset in the range [0.0 .. 1.0]
 * @red: red component of color
 * @green: green component of color
 * @blue: blue component of color
 * @alpha: alpha component of color
 * 
 * Adds a translucent color stop to a gradient pattern. The offset
 * specifies the location along the gradient's control vector. For
 * example, a linear gradient's control vector is from (x0,y0) to
 * (x1,y1) while a radial gradient's control vector is from any point
 * on the start circle to the corresponding point on the end circle.
 *
 * The color is specified in the same way as in cairo_set_source_rgba().
 *
 * Note: If the pattern is not a gradient pattern, (eg. a linear or
 * radial pattern), then the pattern will be put into an error status
 * with a status of %CAIRO_STATUS_PATTERN_TYPE_MISMATCH.
 */
void
cairo_pattern_add_color_stop_rgba (cairo_pattern_t *pattern,
				   double	   offset,
				   double	   red,
				   double	   green,
				   double	   blue,
				   double	   alpha)
{
    cairo_color_t color;

    if (pattern->status)
	return;

    if (pattern->type != CAIRO_PATTERN_LINEAR &&
	pattern->type != CAIRO_PATTERN_RADIAL)
    {
	_cairo_pattern_set_error (pattern, CAIRO_STATUS_PATTERN_TYPE_MISMATCH);
	return;
    }

    _cairo_restrict_value (&offset, 0.0, 1.0);
    _cairo_restrict_value (&red,    0.0, 1.0);
    _cairo_restrict_value (&green,  0.0, 1.0);
    _cairo_restrict_value (&blue,   0.0, 1.0);
    _cairo_restrict_value (&alpha,  0.0, 1.0);

    _cairo_color_init_rgba (&color, red, green, blue, alpha);
    _cairo_pattern_add_color_stop ((cairo_gradient_pattern_t *) pattern,
				   offset,
				   &color);
}

/**
 * cairo_pattern_set_matrix:
 * @pattern: a #cairo_pattern_t
 * @matrix: a #cairo_matrix_t
 * 
 * Sets the pattern's transformation matrix to @matrix. This matrix is
 * a transformation from user space to pattern space.
 *
 * When a pattern is first created it always has the identity matrix
 * for its transformation matrix, which means that pattern space is
 * initially identical to user space.
 *
 * Important: Please note that the direction of this transformation
 * matrix is from user space to pattern space. This means that if you
 * imagine the flow from a pattern to user space (and on to device
 * space), then coordinates in that flow will be transformed by the
 * inverse of the pattern matrix.
 *
 * For example, if you want to make a pattern appear twice as large as
 * it does by default the correct code to use is:
 *
 * <informalexample><programlisting>
 * cairo_matrix_init_scale (&amp;matrix, 0.5, 0.5);
 * cairo_pattern_set_matrix (pattern, &amp;matrix);
 * </programlisting></informalexample>
 *
 * Meanwhile, using values of 2.0 rather than 0.5 in the code above
 * would cause the pattern to appear at half of its default size.
 *
 * Also, please note the discussion of the user-space locking
 * semantics of cairo_set_source().
 **/
void
cairo_pattern_set_matrix (cairo_pattern_t      *pattern,
			  const cairo_matrix_t *matrix)
{
    if (pattern->status)
	return;

    pattern->matrix = *matrix;
}

/**
 * cairo_pattern_get_matrix:
 * @pattern: a #cairo_pattern_t
 * @matrix: return value for the matrix
 * 
 * Stores the pattern's transformation matrix into @matrix.
 **/
void
cairo_pattern_get_matrix (cairo_pattern_t *pattern, cairo_matrix_t *matrix)
{
    *matrix = pattern->matrix;
}

void
cairo_pattern_set_filter (cairo_pattern_t *pattern, cairo_filter_t filter)
{
    if (pattern->status)
	return;

    pattern->filter = filter;
}

cairo_filter_t
cairo_pattern_get_filter (cairo_pattern_t *pattern)
{
    return pattern->filter;
}

void
cairo_pattern_set_extend (cairo_pattern_t *pattern, cairo_extend_t extend)
{
    if (pattern->status)
	return;

    pattern->extend = extend;
}

cairo_extend_t
cairo_pattern_get_extend (cairo_pattern_t *pattern)
{
    return pattern->extend;
}

void
_cairo_pattern_transform (cairo_pattern_t	*pattern,
			  const cairo_matrix_t  *ctm_inverse)
{
    assert (pattern->status == CAIRO_STATUS_SUCCESS);

    cairo_matrix_multiply (&pattern->matrix, ctm_inverse, &pattern->matrix);
}

#define INTERPOLATE_COLOR_LINEAR(c1, c2, factor) \
  (((c2 * factor) + (c1 * (65536 - factor))) / 65536)

static void
_cairo_pattern_shader_linear (unsigned char *color0,
			      unsigned char *color1,
			      cairo_fixed_t factor,
			      uint32_t	    *pixel)
{
    *pixel = ((INTERPOLATE_COLOR_LINEAR (color0[3], color1[3], factor) << 24) |
	      (INTERPOLATE_COLOR_LINEAR (color0[0], color1[0], factor) << 16) |
	      (INTERPOLATE_COLOR_LINEAR (color0[1], color1[1], factor) << 8) |
	      (INTERPOLATE_COLOR_LINEAR (color0[2], color1[2], factor) << 0));
}

#undef INTERPOLATE_COLOR_LINEAR

static int
_cairo_shader_color_stop_compare (const void *elem1, const void *elem2)
{
    cairo_shader_color_stop_t *s1 = (cairo_shader_color_stop_t *) elem1;
    cairo_shader_color_stop_t *s2 = (cairo_shader_color_stop_t *) elem2;
	
    return
        (s1->offset == s2->offset) ?
        /* equal offsets, sort on id */
        ((s1->id < s2->id) ? -1 : 1) :
        /* sort on offset */
        ((s1->offset < s2->offset) ? -1 : 1);
}

static cairo_status_t
_cairo_pattern_shader_init (cairo_gradient_pattern_t *pattern,
			    cairo_shader_op_t	     *op)
{
    int i;

    op->stops = malloc (pattern->n_stops * sizeof (cairo_shader_color_stop_t));
    if (!op->stops)
	return CAIRO_STATUS_NO_MEMORY;

    for (i = 0; i < pattern->n_stops; i++)
    {
	op->stops[i].color_char[0] = pattern->stops[i].color.red * 0xff;
	op->stops[i].color_char[1] = pattern->stops[i].color.green * 0xff;
	op->stops[i].color_char[2] = pattern->stops[i].color.blue * 0xff;
	op->stops[i].color_char[3] = pattern->stops[i].color.alpha * 0xff;
	op->stops[i].offset = pattern->stops[i].offset;
	op->stops[i].id = i;
    }

    /* sort stops in ascending order */
    qsort (op->stops, pattern->n_stops, sizeof (cairo_shader_color_stop_t),
	   _cairo_shader_color_stop_compare);

    /* this scale value is used only when computing gradient values
     * before the defined range, in which case stop 0 is used for both
     * ends of the interpolation, making the value of 'scale' not
     * actually matter, except that valgrind notices we're using
     * an undefined value.
     */
    op->stops[0].scale = 0;
    for (i = 0; i < pattern->n_stops - 1; i++)
    {
	op->stops[i + 1].scale = op->stops[i + 1].offset - op->stops[i].offset;
	if (op->stops[i + 1].scale == 65536)
	    op->stops[i + 1].scale = 0;
    }

    op->n_stops = pattern->n_stops;
    op->extend = pattern->base.extend;

    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_pattern_shader_fini (cairo_shader_op_t *op)
{
    if (op->stops)
	free (op->stops);
}

/* Find two color stops bounding the given offset. If the given offset
 * is before the first or after the last stop offset, the nearest
 * offset is returned twice.
 */
static void
_cairo_shader_op_find_color_stops (cairo_shader_op_t	     *op,
				   cairo_fixed_t	     offset,
				   cairo_shader_color_stop_t *stops[2])
{
    int i;

    /* Before first stop. */
    if (offset <= op->stops[0].offset) {
	stops[0] = &op->stops[0];
	stops[1] = &op->stops[0];
	return;
    }

    /* Between two stops. */
    for (i = 0; i < op->n_stops - 1; i++) {
	if (offset <= op->stops[i + 1].offset) {
	    stops[0] = &op->stops[i];
	    stops[1] = &op->stops[i + 1];
	    return;
	}
    }

    /* After last stop. */
    stops[0] = &op->stops[op->n_stops - 1];
    stops[1] = &op->stops[op->n_stops - 1];
}

static void
_cairo_pattern_calc_color_at_pixel (cairo_shader_op_t *op,
				    cairo_fixed_t     factor,
				    uint32_t	      *pixel)
{
    cairo_shader_color_stop_t *stops[2];

    switch (op->extend) {
    case CAIRO_EXTEND_REPEAT:
	factor -= factor & 0xffff0000;
	break;
    case CAIRO_EXTEND_REFLECT:
	if (factor < 0 || factor > 65536) {
	    if ((factor >> 16) % 2)
		factor = 65536 - (factor - (factor & 0xffff0000));
	    else
		factor -= factor & 0xffff0000;
	}
	break;
    case CAIRO_EXTEND_NONE:
	break;
    }

    _cairo_shader_op_find_color_stops (op, factor, stops);

    /* take offset as new 0 of coordinate system */
    factor -= stops[0]->offset;
	    
    /* difference between two offsets == 0, abrubt change */
    if (stops[1]->scale)
	factor = ((cairo_fixed_48_16_t) factor << 16) /
	    stops[1]->scale;

    _cairo_pattern_shader_linear (stops[0]->color_char,
				  stops[1]->color_char,
				  factor, pixel);
	    
    /* multiply alpha */
    if (((unsigned char) (*pixel >> 24)) != 0xff) {
	*pixel = (*pixel & 0xff000000) |
	    (MULTIPLY_COLORCOMP (*pixel >> 16, *pixel >> 24) << 16) |
	    (MULTIPLY_COLORCOMP (*pixel >> 8, *pixel >> 24) << 8) |
	    (MULTIPLY_COLORCOMP (*pixel >> 0, *pixel >> 24) << 0);
    }
}

static cairo_status_t
_cairo_image_data_set_linear (cairo_linear_pattern_t *pattern,
			      double		     offset_x,
			      double		     offset_y,
			      uint32_t		     *pixels,
			      int		     width,
			      int		     height)
{
    int x, y;
    cairo_point_double_t point0, point1;
    double a, b, c, d, tx, ty;
    double scale, start, dx, dy, factor;
    cairo_shader_op_t op;
    cairo_status_t status;

    status = _cairo_pattern_shader_init (&pattern->base, &op);
    if (status)
	return status;

    /* We compute the position in the linear gradient for
     * a point q as:
     *
     *  [q . (p1 - p0) - p0 . (p1 - p0)] / (p1 - p0) ^ 2
     *
     * The computation is done in pattern space. The 
     * calculation could be heavily optimized by using the
     * fact that 'factor' increases linearly in both
     * directions.
     */
    point0.x = pattern->point0.x;
    point0.y = pattern->point0.y;
    point1.x = pattern->point1.x;
    point1.y = pattern->point1.y;

    _cairo_matrix_get_affine (&pattern->base.base.matrix,
			      &a, &b, &c, &d, &tx, &ty);

    dx = point1.x - point0.x;
    dy = point1.y - point0.y;
    scale = dx * dx + dy * dy;
    scale = (scale) ? 1.0 / scale : 1.0;

    start = dx * point0.x + dy * point0.y;

    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
	    double qx_device = x + offset_x;
	    double qy_device = y + offset_y;
		
	    /* transform fragment into pattern space */
	    double qx = a * qx_device + c * qy_device + tx;
	    double qy = b * qx_device + d * qy_device + ty;
	    
	    factor = ((dx * qx + dy * qy) - start) * scale;

	    _cairo_pattern_calc_color_at_pixel (&op, _cairo_fixed_from_double (factor), pixels++);
	}
    }

    _cairo_pattern_shader_fini (&op);
    
    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_linear_pattern_classify (cairo_linear_pattern_t *pattern,
				double		       offset_x,
				double		       offset_y,
				int		       width,
				int		       height,
				cairo_bool_t           *is_horizontal,
				cairo_bool_t           *is_vertical)
{
    cairo_point_double_t point0, point1;
    double a, b, c, d, tx, ty;
    double scale, start, dx, dy;
    cairo_fixed_t factors[3];
    int i;

    /* To classidy a pattern as horizontal or vertical, we first
     * compute the (fixed point) factors at the corners of the
     * pattern. We actually only need 3/4 corners, so we skip the
     * fourth.
     */
    point0.x = pattern->point0.x;
    point0.y = pattern->point0.y;
    point1.x = pattern->point1.x;
    point1.y = pattern->point1.y;

    _cairo_matrix_get_affine (&pattern->base.base.matrix,
			      &a, &b, &c, &d, &tx, &ty);

    dx = point1.x - point0.x;
    dy = point1.y - point0.y;
    scale = dx * dx + dy * dy;
    scale = (scale) ? 1.0 / scale : 1.0;

    start = dx * point0.x + dy * point0.y;

    for (i = 0; i < 3; i++) {
	double qx_device = (i % 2) * (width - 1) + offset_x;
	double qy_device = (i / 2) * (height - 1) + offset_y;

	/* transform fragment into pattern space */
	double qx = a * qx_device + c * qy_device + tx;
	double qy = b * qx_device + d * qy_device + ty;
	
	factors[i] = _cairo_fixed_from_double (((dx * qx + dy * qy) - start) * scale);
    }

    /* We consider a pattern to be vertical if the fixed point factor
     * at the two upper corners is the same. We could accept a small
     * change, but determining what change is acceptable would require
     * sorting the stops in the pattern and looking at the differences.
     *
     * Horizontal works the same way with the two left corners.
     */

    *is_vertical = factors[1] == factors[0];
    *is_horizontal = factors[2] == factors[0];
}

static cairo_status_t
_cairo_image_data_set_radial (cairo_radial_pattern_t *pattern,
			      double		     offset_x,
			      double		     offset_y,
			      uint32_t		     *pixels,
			      int		     width,
			      int		     height)
{
    int x, y, aligned_circles;
    cairo_point_double_t c0, c1;
    double px, py, ex, ey;
    double a, b, c, d, tx, ty;
    double r0, r1, c0_e_x, c0_e_y, c0_e, c1_e_x, c1_e_y, c1_e,
	c0_c1_x, c0_c1_y, c0_c1, angle_c0, c1_y, y_x, c0_y, c0_x, r1_2,
	denumerator, fraction, factor;
    cairo_shader_op_t op;
    cairo_status_t status;

    status = _cairo_pattern_shader_init (&pattern->base, &op);
    if (status)
	return status;

    c0.x = pattern->center0.x;
    c0.y = pattern->center0.y;
    r0 = pattern->radius0;
    c1.x = pattern->center1.x;
    c1.y = pattern->center1.y;
    r1 =  pattern->radius1;

    if (c0.x != c1.x || c0.y != c1.y) {
	aligned_circles = 0;
	c0_c1_x = c1.x - c0.x;
	c0_c1_y = c1.y - c0.y;
	c0_c1 = sqrt (c0_c1_x * c0_c1_x + c0_c1_y * c0_c1_y);
	r1_2 = r1 * r1;
    } else {
	aligned_circles = 1;
	r1 = 1.0 / (r1 - r0);
	r1_2 = c0_c1 = 0.0; /* shut up compiler */
    }

    _cairo_matrix_get_affine (&pattern->base.base.matrix,
			      &a, &b, &c, &d, &tx, &ty);

    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
	    px = x + offset_x;
	    py = y + offset_y;
		
	    /* transform fragment */
	    ex = a * px + c * py + tx;
	    ey = b * px + d * py + ty;

	    if (aligned_circles) {
		ex = ex - c1.x;
		ey = ey - c1.y;

		factor = (sqrt (ex * ex + ey * ey) - r0) * r1;
	    } else {
	    /*
	                y         (ex, ey)
               c0 -------------------+---------- x
                  \     |                  __--
                   \    |              __--
                    \   |          __--
                     \  |      __-- r1
                      \ |  __--
                      c1 --

	       We need to calulate distance c0->x; the distance from
	       the inner circle center c0, through fragment position
	       (ex, ey) to point x where it crosses the outer circle.

	       From points c0, c1 and (ex, ey) we get angle C0. With
	       angle C0 we calculate distance c1->y and c0->y and by
	       knowing c1->y and r1, we also know y->x. Adding y->x to
	       c0->y gives us c0->x. The gradient offset can then be
	       calculated as:
	       
	       offset = (c0->e - r0) / (c0->x - r0)
	       
	       */

		c0_e_x = ex - c0.x;
		c0_e_y = ey - c0.y;
		c0_e = sqrt (c0_e_x * c0_e_x + c0_e_y * c0_e_y);

		c1_e_x = ex - c1.x;
		c1_e_y = ey - c1.y;
		c1_e = sqrt (c1_e_x * c1_e_x + c1_e_y * c1_e_y);

		denumerator = -2.0 * c0_e * c0_c1;
		
		if (denumerator != 0.0) {
		    fraction = (c1_e * c1_e - c0_e * c0_e - c0_c1 * c0_c1) /
			denumerator;

		    if (fraction > 1.0)
			fraction = 1.0;
		    else if (fraction < -1.0)
			fraction = -1.0;
		    
		    angle_c0 = acos (fraction);
		    
		    c0_y = cos (angle_c0) * c0_c1;
		    c1_y = sin (angle_c0) * c0_c1;
		    
		    y_x = sqrt (r1_2 - c1_y * c1_y);
		    c0_x = y_x + c0_y;
		    
		    factor = (c0_e - r0) / (c0_x - r0);
		} else {
		    factor = -r0;
		}
	    }

	    _cairo_pattern_calc_color_at_pixel (&op, _cairo_fixed_from_double (factor), pixels++);
	}
    }

    _cairo_pattern_shader_fini (&op);

    return CAIRO_STATUS_SUCCESS;
}

static cairo_int_status_t
_cairo_pattern_acquire_surface_for_gradient (cairo_gradient_pattern_t *pattern,
					     cairo_surface_t	        *dst,
					     int			x,
					     int			y,
					     unsigned int		width,
					     unsigned int	        height,
					     cairo_surface_t	        **out,
					     cairo_surface_attributes_t *attr)
{
    cairo_image_surface_t *image;
    cairo_status_t status;
    uint32_t *data;
    cairo_bool_t repeat = FALSE;

    if (pattern->base.type == CAIRO_PATTERN_LINEAR) {
	cairo_bool_t is_horizontal;
	cairo_bool_t is_vertical;

	_cairo_linear_pattern_classify ((cairo_linear_pattern_t *)pattern,
					x, y, width, height,
					&is_horizontal, &is_vertical);
	if (is_horizontal) {
	    height = 1;
	    repeat = TRUE;
	}
	if (is_vertical) {
	    width = 1;
	    repeat = TRUE;
	}
    }
    
    data = malloc (width * height * 4);
    if (data == NULL)
	return CAIRO_STATUS_NO_MEMORY;
    
    if (pattern->base.type == CAIRO_PATTERN_LINEAR)
    {
	cairo_linear_pattern_t *linear = (cairo_linear_pattern_t *) pattern;
	
	status = _cairo_image_data_set_linear (linear, x, y, data,
					       width, height);
    }
    else
    {
	cairo_radial_pattern_t *radial = (cairo_radial_pattern_t *) pattern;
	
	status = _cairo_image_data_set_radial (radial, x, y, data,
					       width, height);
    }

    if (status) {
	free (data);
	return status;
    }

    image = (cairo_image_surface_t *)
	cairo_image_surface_create_for_data ((unsigned char *) data,
					     CAIRO_FORMAT_ARGB32,
					     width, height,
					     width * 4);
    
    if (image->base.status) {
	free (data);
	return CAIRO_STATUS_NO_MEMORY;
    }

    _cairo_image_surface_assume_ownership_of_data (image);
    
    status = _cairo_surface_clone_similar (dst, &image->base, out);
	
    cairo_surface_destroy (&image->base);

    attr->x_offset = -x;
    attr->y_offset = -y;
    cairo_matrix_init_identity (&attr->matrix);
    attr->extend = repeat ? CAIRO_EXTEND_REPEAT : CAIRO_EXTEND_NONE;
    attr->filter = CAIRO_FILTER_NEAREST;
    attr->acquired = FALSE;
    
    return status;
}

static cairo_int_status_t
_cairo_pattern_acquire_surface_for_solid (cairo_solid_pattern_t	     *pattern,
					  cairo_surface_t	     *dst,
					  int			     x,
					  int			     y,
					  unsigned int		     width,
					  unsigned int		     height,
					  cairo_surface_t	     **out,
					  cairo_surface_attributes_t *attribs)
{
    *out = _cairo_surface_create_similar_solid (dst,
						CAIRO_CONTENT_COLOR_ALPHA,
						1, 1,
						&pattern->color);
    if ((*out)->status)
	return CAIRO_STATUS_NO_MEMORY;

    attribs->x_offset = attribs->y_offset = 0;
    cairo_matrix_init_identity (&attribs->matrix);
    attribs->extend = CAIRO_EXTEND_REPEAT;
    attribs->filter = CAIRO_FILTER_NEAREST;
    attribs->acquired = FALSE;
    
    return CAIRO_STATUS_SUCCESS;
}


/**
 * _cairo_pattern_is_opaque_solid
 *
 * Convenience function to determine whether a pattern is an opaque
 * (alpha==1.0) solid color pattern. This is done by testing whether
 * the pattern's alpha value when converted to a byte is 255, so if a
 * backend actually supported deep alpha channels this function might
 * not do the right thing.
 *
 * Return value: %TRUE if the pattern is an opaque, solid color.
 **/
cairo_bool_t 
_cairo_pattern_is_opaque_solid (cairo_pattern_t *pattern)
{
    cairo_solid_pattern_t *solid;

    if (pattern->type != CAIRO_PATTERN_SOLID)
	return FALSE;

    solid = (cairo_solid_pattern_t *) pattern;

    return CAIRO_ALPHA_IS_OPAQUE (solid->color.alpha);
}

static cairo_int_status_t
_cairo_pattern_acquire_surface_for_surface (cairo_surface_pattern_t   *pattern,
					    cairo_surface_t	       *dst,
					    int			       x,
					    int			       y,
					    unsigned int	       width,
					    unsigned int	       height,
					    cairo_surface_t	       **out,
					    cairo_surface_attributes_t *attr)
{
    cairo_int_status_t status;
    int tx, ty;

    attr->acquired = FALSE;
	    
    if (_cairo_surface_is_image (dst))
    {
	cairo_image_surface_t *image;
	
	status = _cairo_surface_acquire_source_image (pattern->surface,
						      &image,
						      &attr->extra);
	if (status)
	    return status;

	*out = &image->base;
	attr->acquired = TRUE;
    }
    else
    {
	status = _cairo_surface_clone_similar (dst, pattern->surface, out);
    }
    
    attr->extend = pattern->base.extend;
    attr->filter = pattern->base.filter;
    if (_cairo_matrix_is_integer_translation (&pattern->base.matrix,
					      &tx, &ty))
    {
	cairo_matrix_init_identity (&attr->matrix);
	attr->x_offset = tx;
	attr->y_offset = ty;
	attr->filter = CAIRO_FILTER_NEAREST;
    }
    else
    {
	attr->matrix = pattern->base.matrix;
	attr->x_offset = attr->y_offset = 0;
    }
    
    return status;
}

/**
 * _cairo_pattern_acquire_surface:
 * @pattern: a #cairo_pattern_t
 * @dst: destination surface
 * @x: X coordinate in source corresponding to left side of destination area
 * @y: Y coordinate in source corresponding to top side of destination area
 * @width: width of destination area
 * @height: height of destination area
 * @surface_out: location to store a pointer to a surface
 * @attributes: surface attributes that destination backend should apply to
 * the returned surface
 * 
 * A convenience function to obtain a surface to use as the source for
 * drawing on @dst.
 * 
 * Return value: %CAIRO_STATUS_SUCCESS if a surface was stored in @surface_out.
 **/
cairo_int_status_t
_cairo_pattern_acquire_surface (cairo_pattern_t		   *pattern,
				cairo_surface_t		   *dst,
				int			   x,
				int			   y,
				unsigned int		   width,
				unsigned int		   height,
				cairo_surface_t		   **surface_out,
				cairo_surface_attributes_t *attributes)
{
    cairo_status_t status;
    
    if (pattern->status) {
	*surface_out = NULL;
	attributes->acquired = FALSE;
	return pattern->status;
    }

    switch (pattern->type) {
    case CAIRO_PATTERN_SOLID: {
	cairo_solid_pattern_t *src = (cairo_solid_pattern_t *) pattern;
	
	status = _cairo_pattern_acquire_surface_for_solid (src, dst,
							   x, y, width, height,
							   surface_out,
							   attributes);
	} break;
    case CAIRO_PATTERN_LINEAR:
    case CAIRO_PATTERN_RADIAL: {
	cairo_gradient_pattern_t *src = (cairo_gradient_pattern_t *) pattern;

	/* fast path for gradients with less than 2 color stops */
	if (src->n_stops < 2)
	{
	    const cairo_color_t *color;
	    cairo_solid_pattern_t solid;

	    if (src->n_stops)
		color = &src->stops->color;
	    else
		color = CAIRO_COLOR_TRANSPARENT;

	    _cairo_pattern_init_solid (&solid, color);

	    status = _cairo_pattern_acquire_surface_for_solid (&solid, dst,
							       x, y,
							       width, height,
							       surface_out,
							       attributes);
	}
	else
	{
	    status = _cairo_pattern_acquire_surface_for_gradient (src, dst,
								  x, y,
								  width, height,
								  surface_out,
								  attributes);
	}
    } break;
    case CAIRO_PATTERN_SURFACE: {
	cairo_surface_pattern_t *src = (cairo_surface_pattern_t *) pattern;
	
	status = _cairo_pattern_acquire_surface_for_surface (src, dst,
							     x, y, width, height,
							     surface_out,
							     attributes);
    } break;
    default:
	status = CAIRO_INT_STATUS_UNSUPPORTED;
    }

    return status;
}

/**
 * _cairo_pattern_release_surface:
 * @pattern: a #cairo_pattern_t
 * @surface: a surface obtained by _cairo_pattern_acquire_surface
 * @attributes: attributes obtained by _cairo_pattern_acquire_surface
 * 
 * Releases resources obtained by _cairo_pattern_acquire_surface.
 **/
void
_cairo_pattern_release_surface (cairo_pattern_t		   *pattern,
				cairo_surface_t		   *surface,
				cairo_surface_attributes_t *attributes)
{
    if (attributes->acquired)
    {
	cairo_surface_pattern_t *surface_pattern;

	assert (pattern->type == CAIRO_PATTERN_SURFACE);
	surface_pattern = (cairo_surface_pattern_t *) pattern;

	_cairo_surface_release_source_image (surface_pattern->surface,
					     (cairo_image_surface_t *) surface,
					     attributes->extra);
    }
    else
    {
	cairo_surface_destroy (surface);
    }
}

cairo_int_status_t
_cairo_pattern_acquire_surfaces (cairo_pattern_t	    *src,
				 cairo_pattern_t	    *mask,
				 cairo_surface_t	    *dst,
				 int			    src_x,
				 int			    src_y,
				 int			    mask_x,
				 int			    mask_y,
				 unsigned int		    width,
				 unsigned int		    height,
				 cairo_surface_t	    **src_out,
				 cairo_surface_t	    **mask_out,
				 cairo_surface_attributes_t *src_attributes,
				 cairo_surface_attributes_t *mask_attributes)
{
    cairo_int_status_t	  status;
    cairo_pattern_union_t src_tmp, mask_tmp;

    if (src->status)
	return src->status;
    if (mask && mask->status)
	return mask->status;

    /* If src and mask are both solid, then the mask alpha can be
     * combined into src and mask can be ignored. */

    /* XXX: This optimization assumes that there is no color
     * information in mask, so this will need to change when we
     * support RENDER-style 4-channel masks. */
    if (src->type == CAIRO_PATTERN_SOLID &&
	mask && mask->type == CAIRO_PATTERN_SOLID)
    {
	cairo_color_t combined;
	cairo_solid_pattern_t *src_solid = (cairo_solid_pattern_t *) src;
	cairo_solid_pattern_t *mask_solid = (cairo_solid_pattern_t *) mask;

	combined = src_solid->color;
	_cairo_color_multiply_alpha (&combined, mask_solid->color.alpha);

	_cairo_pattern_init_solid (&src_tmp.solid, &combined);

	mask = NULL;
    }
    else
    {
	_cairo_pattern_init_copy (&src_tmp.base, src);
    }

    status = _cairo_pattern_acquire_surface (&src_tmp.base, dst,
					     src_x, src_y,
					     width, height,
					     src_out, src_attributes);
    if (status) {
	_cairo_pattern_fini (&src_tmp.base);
	return status;
    }

    if (mask == NULL)
    {
	_cairo_pattern_fini (&src_tmp.base);
	*mask_out = NULL;
	return CAIRO_STATUS_SUCCESS;
    }

    _cairo_pattern_init_copy (&mask_tmp.base, mask);
	
    status = _cairo_pattern_acquire_surface (&mask_tmp.base, dst,
					     mask_x, mask_y,
					     width, height,
					     mask_out, mask_attributes);
    
    if (status)
	_cairo_pattern_release_surface (&src_tmp.base,
					*src_out, src_attributes);

    _cairo_pattern_fini (&src_tmp.base);
    _cairo_pattern_fini (&mask_tmp.base);

    return status;
}
