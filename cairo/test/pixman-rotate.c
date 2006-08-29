#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <cairo.h>

#include "cairo-test.h"

#define WIDTH	32
#define HEIGHT	WIDTH

#define IMAGE_WIDTH	(3 * WIDTH)
#define IMAGE_HEIGHT	IMAGE_WIDTH

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "pixman-rotate",
    "Exposes pixman off-by-one error when rotating",
    IMAGE_WIDTH, IMAGE_HEIGHT,
    draw
};

/* Draw the word cairo at NUM_TEXT different angles */
static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *stamp;
    cairo_t *cr2;

    stamp = cairo_surface_create_similar (cairo_get_target (cr),
					  CAIRO_CONTENT_COLOR_ALPHA,
					  WIDTH, HEIGHT);
    cr2 = cairo_create (stamp);
    {
	cairo_new_path (cr2);
	cairo_rectangle (cr2, WIDTH / 4, HEIGHT / 4, WIDTH / 2, HEIGHT / 2);
	cairo_set_source_rgba (cr2, 1, 0, 0, 0.8);
	cairo_fill (cr2);

	cairo_rectangle (cr2, 0, 0, WIDTH, HEIGHT);
	cairo_set_line_width (cr2, 2);
	cairo_set_source_rgb (cr2, 0, 0, 0);
	cairo_stroke (cr2);
    }
    cairo_destroy (cr2);

    /* Draw a translucent rectangle for reference where the rotated
     * image should be. */
    cairo_new_path (cr);
    cairo_rectangle (cr, WIDTH, HEIGHT, WIDTH, HEIGHT);
    cairo_set_source_rgba (cr, 1, 1, 0, 0.3);
    cairo_fill (cr);

#if 1 /* Set to 0 to generate reference image */
    cairo_translate (cr, 2 * WIDTH, 2 * HEIGHT);
    cairo_rotate (cr, M_PI);
#else
    cairo_translate (cr, WIDTH, HEIGHT);
#endif

    cairo_set_source_surface (cr, stamp, 0, 0);
    cairo_paint (cr);

    cairo_show_page (cr);

    cairo_surface_destroy (stamp);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
