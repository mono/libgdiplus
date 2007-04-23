#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

const char	png_filename[]	= "romedalen.png";

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "extend-reflect",
    "Test CAIRO_EXTEND_REFLECT for surface patterns",
    256 + 32*2, 192 + 32*2,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;

    surface = cairo_test_create_surface_from_png (png_filename);
    cairo_set_source_surface (cr, surface, 32, 32);
    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_REFLECT);

    cairo_paint (cr);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
