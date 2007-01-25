#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

#define SIZE 400
#define OFFSET 50
const char	png_filename[]	= "romedalen.png";

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "extend-reflect",
    "Test CAIRO_EXTEND_REFLECT for surface patterns"
    "\nCAIRO_EXTEND_REFLECT code is broken and corrupts memory",
    SIZE, SIZE,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_pattern_t *pattern;
    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_rectangle (cr, 0, 0, SIZE, SIZE);
    cairo_fill (cr);

    pattern = cairo_test_create_pattern_from_png (png_filename);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REFLECT);
    cairo_set_source (cr, pattern);
    cairo_pattern_destroy (pattern);

    cairo_paint (cr);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
