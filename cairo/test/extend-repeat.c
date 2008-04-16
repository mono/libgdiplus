#include "cairo-test.h"

const char	png_filename[]	= "romedalen.png";

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "extend-repeat",
    "Test CAIRO_EXTEND_REPEAT for surface patterns",
    256 + 32*2, 192 + 32*2,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;

    surface = cairo_test_create_surface_from_png (png_filename);
    cairo_set_source_surface (cr, surface, 32, 32);
    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_REPEAT);

    cairo_paint (cr);

    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
