/*
 * Copyright © 2006 Novell, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Novell, Inc. not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Novell, Inc. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * NOVELL, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Robert O'Callahan <rocallahan@novell.com>
 */

#include "cairo-test.h"
#include <stddef.h>
#include <math.h>

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "get-path-extents",
    "Test cairo_fill_extents and cairo_stroke_extents",
    0, 0,
    draw
};

enum ExtentsType { FILL, STROKE };

enum Relation { EQUALS, CONTAINS };

static cairo_bool_t
check_extents (const char *message, cairo_t *cr, enum ExtentsType type,
               enum Relation relation,
               double x, double y, double width, double height)
{
    double ext_x1, ext_y1, ext_x2, ext_y2;
    const char *type_string;
    const char *relation_string;

    switch (type) {
    default:
    case FILL:
        type_string = "fill";
        cairo_fill_extents (cr, &ext_x1, &ext_y1, &ext_x2, &ext_y2);
        break;
    case STROKE:
        type_string = "stroke";
        cairo_stroke_extents (cr, &ext_x1, &ext_y1, &ext_x2, &ext_y2);
        break;
    }
    
    /* let empty rects match */
    if ((ext_x1 == ext_x2 || ext_y1 == ext_y2) && (width == 0 || height == 0))
        return 1;

    switch (relation) {
    default:
    case EQUALS:
        relation_string = "equal";
        if (ext_x1 == x && ext_y1 == y && ext_x2 == x + width && ext_y2 == y + height)
            return 1;
        break;
    case CONTAINS:
        relation_string = "contain";
        if (width == 0 || height == 0) {
            /* odd test that doesn't really test anything... */
            return 1;
        }
        if (ext_x1 <= x && ext_y1 <= y && ext_x2 >= x + width && ext_y2 >= y + height)
            return 1;
        break;
    }

    cairo_test_log ("Error: %s; %s extents %f,%f,%f,%f should %s %f,%f,%f,%f\n",
                    message, type_string,
                    ext_x1, ext_y1, ext_x2 - ext_x1, ext_y2 - ext_y1,
                    relation_string,
                    x, y, width, height);
    return 0;
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_t         *cr2;
    const char      *phase;
    
    surface = cairo_surface_create_similar (cairo_get_target (cr),
                                            CAIRO_CONTENT_COLOR, 100, 100);
    /* don't use cr accidentally */
    cr = NULL;
    cr2 = cairo_create (surface);
    cairo_surface_destroy (surface);

    cairo_set_line_width (cr2, 10);
    cairo_set_line_join (cr2, CAIRO_LINE_JOIN_MITER);
    cairo_set_miter_limit (cr2, 100);

    phase = "No path";
    if (!check_extents (phase, cr2, FILL, EQUALS, 0, 0, 0, 0) ||
        !check_extents (phase, cr2, STROKE, EQUALS, 0, 0, 0, 0))
	return CAIRO_TEST_FAILURE;

    /* http://bugs.freedesktop.org/show_bug.cgi?id=7965 */
    phase = "A vertical, open path";
    cairo_save (cr2);
    cairo_set_line_cap (cr2, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join (cr2, CAIRO_LINE_JOIN_ROUND);
    cairo_move_to (cr2, 0, 180);
    cairo_line_to (cr2, 750, 180);
    if (!check_extents (phase, cr2, FILL, EQUALS, 0, 0, 0, 0) ||
        !check_extents (phase, cr2, STROKE, EQUALS, -5, 175, 760, 10))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);

    phase = "Simple rect";
    cairo_save (cr2);
    cairo_rectangle (cr2, 10, 10, 80, 80);
    if (!check_extents (phase, cr2, FILL, EQUALS, 10, 10, 80, 80) ||
        !check_extents (phase, cr2, STROKE, EQUALS, 5, 5, 90, 90))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);

    phase = "Two rects";
    cairo_save (cr2);
    cairo_rectangle (cr2, 10, 10, 10, 10);
    cairo_rectangle (cr2, 20, 20, 10, 10);
    if (!check_extents (phase, cr2, FILL, EQUALS, 10, 10, 20, 20) ||
        !check_extents (phase, cr2, STROKE, EQUALS, 5, 5, 30, 30))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);

    phase = "Triangle";
    cairo_save (cr2);
    cairo_move_to (cr2, 10, 10);
    cairo_line_to (cr2, 90, 90);
    cairo_line_to (cr2, 90, 10);
    cairo_close_path (cr2);
    /* miter joins protrude 5*(1+sqrt(2)) above the top-left corner and to
       the right of the bottom-right corner */
    if (!check_extents (phase, cr2, FILL, EQUALS, 10, 10, 80, 80) ||
        !check_extents (phase, cr2, STROKE, CONTAINS, 0, 5, 95, 95))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);
    
    phase = "User space, simple scale, getting extents with same transform";
    cairo_save (cr2);
    cairo_scale (cr2, 2, 2);
    cairo_rectangle (cr2, 5, 5, 40, 40);
    if (!check_extents (phase, cr2, FILL, EQUALS, 5, 5, 40, 40) ||
        !check_extents (phase, cr2, STROKE, EQUALS, 0, 0, 50, 50))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);

    phase = "User space, simple scale, getting extents with no transform";
    cairo_save (cr2);
    cairo_save (cr2);
    cairo_scale (cr2, 2, 2);
    cairo_rectangle (cr2, 5, 5, 40, 40);
    cairo_restore (cr2);
    if (!check_extents (phase, cr2, FILL, EQUALS, 10, 10, 80, 80) ||
        !check_extents (phase, cr2, STROKE, EQUALS, 5, 5, 90, 90))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);

    phase = "User space, rotation, getting extents with transform";
    cairo_save (cr2);
    cairo_rectangle (cr2, -50, -50, 50, 50);
    cairo_rotate (cr2, -M_PI/4);
    /* the path in user space is now (nearly) the square rotated by
       45 degrees about the origin. Thus its x1 and x2 are both nearly 0.
       This should show any bugs where we just transform device-space
       x1,y1 and x2,y2 to get the extents. */
    /* The largest axis-aligned square inside the rotated path has
       side lengths 50*sqrt(2), so a bit over 35 on either side of
       the axes. With the stroke width added to the rotated path,
       the largest axis-aligned square is a bit over 38 on either side of
       the axes. */
    if (!check_extents (phase, cr2, FILL, CONTAINS, -35, -35, 35, 35) ||
        !check_extents (phase, cr2, STROKE, CONTAINS, -38, -38, 38, 38))
	return CAIRO_TEST_FAILURE;
    cairo_new_path (cr2);
    cairo_restore (cr2);

    cairo_destroy (cr2);
    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
