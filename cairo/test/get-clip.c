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

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "get-clip",
    "Test cairo_copy_clip_rectangle_list and cairo_clip_extents",
    0, 0,
    draw
};

static cairo_bool_t
check_count (const char *message, cairo_bool_t uses_clip_rects,
             cairo_rectangle_list_t *list, int expected)
{
    if (!uses_clip_rects) {
        if (expected == 0 && list->num_rectangles == 0)
            return 1;
	if (list->num_rectangles == expected)
	    return 1;
	if (list->status == CAIRO_STATUS_CLIP_NOT_REPRESENTABLE)
	    return 1;
        cairo_test_log ("Error: %s; cairo_copy_clip_rectangle_list unexpectedly got %d rectangles\n",
                        message, list->num_rectangles);
        return 0;
    }

    if (list->status != CAIRO_STATUS_SUCCESS) {
        cairo_test_log ("Error: %s; cairo_copy_clip_rectangle_list failed with \"%s\"\n",
                        message, cairo_status_to_string(list->status));
        return 0;
    }

    if (list->num_rectangles == expected)
        return 1;
    cairo_test_log ("Error: %s; expected %d rectangles, got %d\n", message,
                    expected, list->num_rectangles);
    return 0;
}

static cairo_bool_t
check_unrepresentable (const char *message, cairo_rectangle_list_t *list)
{
    if (list->status != CAIRO_STATUS_CLIP_NOT_REPRESENTABLE) {
        cairo_test_log ("Error: %s; cairo_copy_clip_rectangle_list got unexpected result \"%s\"\n"
                        " (we expected CAIRO_STATUS_CLIP_NOT_REPRESENTABLE)",
                        message, cairo_status_to_string(list->status));
        return 0;
    }
    return 1;
}

static cairo_bool_t
check_rectangles_contain (const char *message, cairo_bool_t uses_clip_rects,
                          cairo_rectangle_list_t *list,
                          double x, double y, double width, double height)
{
    int i;

    if (!uses_clip_rects)
        return 1;

    for (i = 0; i < list->num_rectangles; ++i) {
        if (list->rectangles[i].x == x && list->rectangles[i].y == y &&
            list->rectangles[i].width == width && list->rectangles[i].height == height)
            return 1;
    }
    cairo_test_log ("Error: %s; rectangle list does not contain rectangle %f,%f,%f,%f\n",
                    message, x, y, width, height);
    return 0;
}

static cairo_bool_t
check_clip_extents (const char *message, cairo_t *cr,
                    double x, double y, double width, double height)
{
    double ext_x1, ext_y1, ext_x2, ext_y2;
    cairo_clip_extents (cr, &ext_x1, &ext_y1, &ext_x2, &ext_y2);
    if (ext_x1 == x && ext_y1 == y && ext_x2 == x + width && ext_y2 == y + height)
        return 1;
    cairo_test_log ("Error: %s; clip extents %f,%f,%f,%f should be %f,%f,%f,%f\n",
                    message, ext_x1, ext_y1, ext_x2 - ext_x1, ext_y2 - ext_y1,
                    x, y, width, height);
    return 0;
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t        *surface;
    cairo_t                *cr2;
    cairo_rectangle_list_t *rectangle_list;
    const char             *phase;
    cairo_bool_t            uses_clip_rects;
    
    surface = cairo_surface_create_similar (cairo_get_target (cr),
                                            CAIRO_CONTENT_COLOR, 100, 100);
    /* don't use cr accidentally */
    cr = NULL;
    cr2 = cairo_create (surface);
    cairo_surface_destroy (surface);

    /* Check the surface type so we ignore cairo_copy_clip_rectangle_list failures
     * on surface types that don't use rectangle lists for clipping.
     * Default to FALSE for the internal surface types, (meta, test-fallback, etc.)
     */
    switch (cairo_surface_get_type (surface)) {
    case CAIRO_SURFACE_TYPE_IMAGE:
    case CAIRO_SURFACE_TYPE_XLIB:
    case CAIRO_SURFACE_TYPE_XCB:
    case CAIRO_SURFACE_TYPE_GLITZ:
    case CAIRO_SURFACE_TYPE_WIN32:
    case CAIRO_SURFACE_TYPE_BEOS:
    case CAIRO_SURFACE_TYPE_DIRECTFB:
        uses_clip_rects = TRUE;
	break;
    case CAIRO_SURFACE_TYPE_QUARTZ:
    case CAIRO_SURFACE_TYPE_PDF:
    case CAIRO_SURFACE_TYPE_PS:
    case CAIRO_SURFACE_TYPE_SVG:
    case CAIRO_SURFACE_TYPE_OS2:
    default:
        uses_clip_rects = FALSE;
        break;
    }

    /* first, test basic stuff. This should not be clipped, it should
       return the surface rectangle. */
    phase = "No clip set";
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_count (phase, uses_clip_rects, rectangle_list, 1) ||
        !check_clip_extents (phase, cr2, 0, 0, 100, 100) ||
        !check_rectangles_contain(phase, uses_clip_rects, rectangle_list, 0, 0, 100, 100)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);

    /* Test simple clip rect. */
    phase = "Simple clip rect";
    cairo_save (cr2);
    cairo_rectangle (cr2, 10, 10, 80, 80);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_count (phase, uses_clip_rects, rectangle_list, 1) ||
        !check_clip_extents (phase, cr2, 10, 10, 80, 80) ||
        !check_rectangles_contain(phase, uses_clip_rects, rectangle_list, 10, 10, 80, 80)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);

    /* Test everything clipped out. */
    phase = "All clipped out";
    cairo_save (cr2);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_count (phase, uses_clip_rects, rectangle_list, 0)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);
    
    /* test two clip rects */
    phase = "Two clip rects";
    cairo_save (cr2);
    cairo_rectangle (cr2, 10, 10, 10, 10);
    cairo_rectangle (cr2, 20, 20, 10, 10);
    cairo_clip (cr2);
    cairo_rectangle (cr2, 15, 15, 10, 10);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_count (phase, uses_clip_rects, rectangle_list, 2) ||
        !check_clip_extents (phase, cr2, 15, 15, 10, 10) ||
        !check_rectangles_contain(phase, uses_clip_rects, rectangle_list, 15, 15, 5, 5) ||
        !check_rectangles_contain(phase, uses_clip_rects, rectangle_list, 20, 20, 5, 5)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);

    /* test non-rectangular clip */
    phase = "Nonrectangular clip";
    cairo_save (cr2);
    cairo_move_to (cr2, 0, 0);
    cairo_line_to (cr2, 100, 100);
    cairo_line_to (cr2, 100, 0);
    cairo_close_path (cr2);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
     /* can't get this in one tight user-space rectangle */
    if (!check_unrepresentable (phase, rectangle_list) ||
        !check_clip_extents (phase, cr2, 0, 0, 100, 100)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);
    
    phase = "User space, simple scale, getting clip with same transform";
    cairo_save (cr2);
    cairo_scale (cr2, 2, 2);
    cairo_rectangle (cr2, 5, 5, 40, 40);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_count (phase, uses_clip_rects, rectangle_list, 1) ||
        !check_clip_extents (phase, cr2, 5, 5, 40, 40) ||
        !check_rectangles_contain(phase, uses_clip_rects, rectangle_list, 5, 5, 40, 40)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);

    phase = "User space, simple scale, getting clip with no transform";
    cairo_save (cr2);
    cairo_save (cr2);
    cairo_scale (cr2, 2, 2);
    cairo_rectangle (cr2, 5, 5, 40, 40);
    cairo_restore (cr2);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_count (phase, uses_clip_rects, rectangle_list, 1) ||
        !check_clip_extents (phase, cr2, 10, 10, 80, 80) ||
        !check_rectangles_contain(phase, uses_clip_rects, rectangle_list, 10, 10, 80, 80)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);

    phase = "User space, rotation, getting clip with no transform";
    cairo_save (cr2);
    cairo_save (cr2);
    cairo_rotate (cr2, 12);
    cairo_rectangle (cr2, 5, 5, 40, 40);
    cairo_restore (cr2);
    cairo_clip (cr2);
    rectangle_list = cairo_copy_clip_rectangle_list (cr2);
    if (!check_unrepresentable (phase, rectangle_list)) {
        cairo_rectangle_list_destroy (rectangle_list);
	return CAIRO_TEST_FAILURE;
    }
    cairo_rectangle_list_destroy (rectangle_list);
    cairo_restore (cr2);

    cairo_destroy (cr2);
    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
