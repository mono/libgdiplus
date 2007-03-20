#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

#define SIZE 140

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "surface-pattern",
    "Test transformed repeated surface patterns"
    "\nExhibiting a strange (very minor) failure in ps backend with device-offset",
    SIZE, SIZE,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_t * cr_surface;
    int surface_size = 6;

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

    cairo_scale (cr, 10, 10);
    cairo_rotate (cr, 1.);
    cairo_set_source_surface (cr, surface, 1.5, 1.5);
    cairo_surface_destroy (surface);

    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_REPEAT);
    cairo_paint (cr);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
