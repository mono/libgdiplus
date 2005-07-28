#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

#define SIZE 100
#define SIZE2 20
#define OFFSET 10

cairo_test_t test = {
    "composite-integer-translate-over-repeat",
    "Test simple compositing: integer-translation 32->32 OVER, with repeat",
    SIZE, SIZE
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    cairo_surface_t *image;
    cairo_pattern_t *pat;
    cairo_t *cr2;

    image = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, SIZE2, SIZE2);

    cr2 = cairo_create (image);
    cairo_set_source_rgba (cr2, 1, 0, 0, 1);
    cairo_rectangle (cr2, 0, 0, SIZE2/2, SIZE2/2);
    cairo_fill (cr2);
    cairo_set_source_rgba (cr2, 0, 1, 0, 1);
    cairo_rectangle (cr2, SIZE2/2, 0, SIZE2/2, SIZE2/2);
    cairo_fill (cr2);
    cairo_set_source_rgba (cr2, 0, 0, 1, 1);
    cairo_rectangle (cr2, 0, SIZE2/2, SIZE2/2, SIZE2/2);
    cairo_fill (cr2);
    cairo_set_source_rgba (cr2, 1, 1, 0, 1);
    cairo_rectangle (cr2, SIZE2/2, SIZE2/2, SIZE2/2, SIZE2/2);
    cairo_fill (cr2);
    cairo_destroy (cr2);

    pat = cairo_pattern_create_for_surface (image);
    cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_rectangle (cr, 0, 0, SIZE, SIZE);
    cairo_fill (cr);

    cairo_translate (cr, OFFSET, OFFSET);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source (cr, pat);
    cairo_rectangle (cr, 0, 0, SIZE - OFFSET, SIZE - OFFSET);
    cairo_fill (cr);

    cairo_surface_destroy (image);
    cairo_pattern_destroy (pat);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
