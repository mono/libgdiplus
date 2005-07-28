#define WIDTH 50
#define HEIGHT 50

#include "cairo-tutorial.h"

static void
draw (cairo_t *cr)
{
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_paint (cr);

    cairo_move_to (cr, 47.5, 25);
    cairo_arc (cr, 25, 25, 22.5,
	       0.0, 2 * M_PI);

    cairo_set_source_rgb (cr, 0.6, 0.8, 1.0);
    cairo_fill_preserve (cr);

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke (cr);
}
