#define WIDTH 50
#define HEIGHT 50

#include "cairo-tutorial.h"

static void
draw (cairo_t *cr)
{
    cairo_translate (cr, 24.5, 25);
    cairo_move_to (cr, 5, -2.5);
    cairo_line_to (cr, 0, 2.5);
    cairo_line_to (cr, -5, -2.5);
    cairo_close_path (cr);
    cairo_set_line_width (cr, 1);
    cairo_stroke (cr);
}
