/* cairo - a vector graphics library with display and print output
 *
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
 * The Initial Developer of the Original Code is Red Hat, Inc.
 *
 * Contributor(s):
 *	Carl D. Worth <cworth@redhat.com>
 */

#ifndef CAIRO_GSTATE_PRIVATE_H
#define CAIRO_GSTATE_PRIVATE_H

#include "cairo-path-fixed-private.h"

struct _cairo_clip_path {
    unsigned int	ref_count;
    cairo_path_fixed_t	path;
    cairo_fill_rule_t	fill_rule;
    double		tolerance;
    cairo_clip_path_t	*prev;
};

typedef struct _cairo_clip {
    cairo_clip_mode_t mode;

    /*
     * Mask-based clipping for cases where the backend 
     * clipping isn't sufficiently able.
     *
     * The rectangle here represents the
     * portion of the destination surface that this
     * clip surface maps to, it does not
     * represent the extents of the clip region or
     * clip paths
     */
    cairo_surface_t *surface;
    cairo_rectangle_t surface_rect;
    /*
     * Surface clip serial number to store
     * in the surface when this clip is set
     */
    unsigned int serial;
    /*
     * A clip region that can be placed in the surface
     */
    pixman_region16_t *region;
    /*
     * If the surface supports path clipping, we store the list of
     * clipping paths that has been set here as a linked list.
     */
    cairo_clip_path_t *path;
} cairo_clip_t;

struct _cairo_gstate {
    cairo_operator_t operator;
    
    double tolerance;

    /* stroke style */
    double line_width;
    cairo_line_cap_t line_cap;
    cairo_line_join_t line_join;
    double miter_limit;

    cairo_fill_rule_t fill_rule;

    double *dash;
    int num_dashes;
    double dash_offset;

    cairo_font_face_t *font_face;
    cairo_scaled_font_t *scaled_font;	/* Specific to the current CTM */
    cairo_matrix_t font_matrix;
    cairo_font_options_t font_options;

    cairo_clip_t clip;

    cairo_matrix_t ctm;
    cairo_matrix_t ctm_inverse;

    cairo_pen_t pen_regular;

    cairo_surface_t *target;

    cairo_pattern_t *source;

    struct _cairo_gstate *next;
};

#endif /* CAIRO_GSTATE_PRIVATE_H */
