#include "cairo-test.h"

const char	png_filename[]	= "romedalen.png";

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "extend-repeat-similar",
    "Test CAIRO_EXTEND_REPEAT for surface patterns",
    256 + 32*2, 192 + 32*2,
    draw
};

static cairo_surface_t *
clone_similar_surface (cairo_surface_t * target, cairo_surface_t *surface)
{
    cairo_t *cr;
    cairo_surface_t *similar;

    similar = cairo_surface_create_similar (target,
	                              cairo_surface_get_content (surface),
				      cairo_image_surface_get_width (surface),
				      cairo_image_surface_get_height (surface));
    cr = cairo_create (similar);
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);
    cairo_destroy (cr);

    return similar;
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *surface;
    cairo_surface_t *similar;

    surface = cairo_test_create_surface_from_png (png_filename);
    similar = clone_similar_surface (cairo_get_group_target (cr), surface);
    cairo_set_source_surface (cr, similar, 32, 32);
    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_REPEAT);

    cairo_paint (cr);

    cairo_surface_destroy (similar);
    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
