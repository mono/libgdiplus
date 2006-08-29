#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

#define SIZE 100
#define OFFSET 10

const char	png_filename[]	= "romedalen.png";

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "composite-integer-translate-source",
    "Test simple compositing: integer-translation 32->32 SOURCE",
    SIZE, SIZE,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *image;

    image = cairo_test_create_surface_from_png (png_filename);

    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_rectangle (cr, 0, 0, SIZE, SIZE);
    cairo_fill (cr);

    cairo_translate (cr, OFFSET, OFFSET);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface (cr, image, 0, 0);
    cairo_rectangle (cr, 0, 0, SIZE - OFFSET, SIZE - OFFSET);
    cairo_fill (cr);

    cairo_surface_destroy (image);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
