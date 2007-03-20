#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

#define SIZE 200

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "surface-pattern-scale-down",
    "Test scaled-down transformed not-repeated surface patterns"
    "\nFails xlib backend (with argb32) with inexplicable alpha in result",
    SIZE, SIZE,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_t * cr_surface;
    int surface_size = 300;

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_paint (cr);

    /* Create an image surface with my favorite four colors in each
     * quadrant. */
    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
					  surface_size, surface_size);
    cr_surface = cairo_create (surface);
    cairo_set_source_rgb (cr_surface, 1, 1, 1);
    cairo_rectangle (cr_surface,
		     0, 0,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_set_source_rgb (cr_surface, 1, 0, 0);
    cairo_rectangle (cr_surface,
		     surface_size / 2, 0,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_set_source_rgb (cr_surface, 0, 1, 0);
    cairo_rectangle (cr_surface,
		     0, surface_size / 2,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_set_source_rgb (cr_surface, 0, 0, 1);
    cairo_rectangle (cr_surface,
		     surface_size / 2, surface_size / 2,
		     surface_size / 2, surface_size / 2);
    cairo_fill (cr_surface);
    cairo_destroy (cr_surface);

    cairo_scale (cr, 0.2, 0.2);
    cairo_rotate (cr, 1.);
    cairo_set_source_surface (cr, surface, 225, -225);
    cairo_surface_destroy (surface);

    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_NONE);
    cairo_paint (cr);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
