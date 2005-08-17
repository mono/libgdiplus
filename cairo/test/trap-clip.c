/*
 * Copyright © 2005 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Red Hat, Inc. not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Red Hat, Inc. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * RED HAT, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Kristian Høgsberg <krh@redhat.com>
 */

#include <math.h>
#include "cairo-test.h"
#include <stdio.h>

#define WIDTH 64
#define HEIGHT 64
#define PAD 10

const char	png_filename[]	= "romedalen.png";

static void
set_solid_pattern (cairo_t *cr, int x, int y)
{
    cairo_set_source_rgb (cr, 0, 0, 0.6);
}

static void
set_translucent_pattern (cairo_t *cr, int x, int y)
{
    cairo_set_source_rgba (cr, 0, 0, 0.6, 0.5);
}

static void
set_gradient_pattern (cairo_t *cr, int x, int y)
{
    cairo_pattern_t *pattern;

    pattern =
	cairo_pattern_create_linear (x, y, x + WIDTH, y + HEIGHT);
    cairo_pattern_add_color_stop_rgba (pattern, 0, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba (pattern, 1, 0, 0, 0.4, 1);
    cairo_set_source (cr, pattern);
    cairo_pattern_destroy (pattern);
}

static void
set_image_pattern (cairo_t *cr, int x, int y)
{
    cairo_pattern_t *pattern;

    pattern = cairo_test_create_pattern_from_png (png_filename);
    cairo_set_source (cr, pattern);
    cairo_pattern_destroy (pattern);
}

static void
draw_rect (cairo_t *cr, int x, int y)
{
    cairo_new_path (cr);
    cairo_rectangle (cr, x, y, WIDTH, HEIGHT);
    cairo_fill (cr);
}

static void
draw_rects (cairo_t *cr, int x, int y)
{
    int width = WIDTH / 3;
    int height = HEIGHT / 2;
  
    cairo_new_path (cr);
    cairo_rectangle (cr, x, y, width, height);
    cairo_rectangle (cr, x + width, y + height, width, height);
    cairo_rectangle (cr, x + 2 * width, y, width, height);
    cairo_fill (cr);
}

static void
draw_polygon (cairo_t *cr, int x, int y)
{
    cairo_new_path (cr);
    cairo_move_to (cr, x, y);
    cairo_line_to (cr, x, y + HEIGHT);
    cairo_line_to (cr, x + WIDTH / 2, y + 3 * HEIGHT / 4);
    cairo_line_to (cr, x + WIDTH, y + HEIGHT);
    cairo_line_to (cr, x + WIDTH, y);
    cairo_line_to (cr, x + WIDTH / 2, y + HEIGHT / 4);
    cairo_close_path (cr);
    cairo_fill (cr);
}

static void
clip_none (cairo_t *cr, int x, int y)
{
}

static void
clip_rect (cairo_t *cr, int x, int y)
{
    cairo_new_path (cr);
    cairo_rectangle (cr, x + (int)WIDTH / 6, y + (int)HEIGHT / 6,
		     4 * ((int)WIDTH  / 6), 4 * ((int)WIDTH / 6));
    cairo_clip (cr);
    cairo_new_path (cr);
}

static void
clip_rects (cairo_t *cr, int x, int y)
{
    int height = HEIGHT / 3;
  
    cairo_new_path (cr);
    cairo_rectangle (cr, x, y, WIDTH, height);
    cairo_rectangle (cr, x, y + 2 * height, WIDTH, height);
    cairo_clip (cr);
    cairo_new_path (cr);
}

static void
clip_circle (cairo_t *cr, int x, int y)
{
    cairo_new_path (cr);
    cairo_arc (cr, x + WIDTH / 2, y + HEIGHT / 2,
	       WIDTH / 3, 0, 2 * M_PI);
    cairo_clip (cr);
    cairo_new_path (cr);
}

static void (*pattern_funcs[])(cairo_t *cr, int x, int y) = {
    set_solid_pattern,
    set_translucent_pattern,
    set_gradient_pattern,
    set_image_pattern,
};

static void (*draw_funcs[])(cairo_t *cr, int x, int y) = {
    draw_rect,
    draw_rects,
    draw_polygon,
};

static void (*clip_funcs[])(cairo_t *cr, int x, int y) = {
    clip_none,
    clip_rect,
    clip_rects,
    clip_circle,
};

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#define IMAGE_WIDTH (ARRAY_SIZE (pattern_funcs) * (WIDTH + PAD) + PAD)
#define IMAGE_HEIGHT (ARRAY_SIZE (draw_funcs) * ARRAY_SIZE (clip_funcs) * (HEIGHT + PAD) + PAD)

static cairo_test_t test = {
    "trap-clip",
    "Trapezoid clipping",
    IMAGE_WIDTH, IMAGE_HEIGHT
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    int i, j, k, x, y;

    for (k = 0; k < ARRAY_SIZE (clip_funcs); k++) {
	for (j = 0; j < ARRAY_SIZE (draw_funcs); j++) {
	    for (i = 0; i < ARRAY_SIZE (pattern_funcs); i++) {
		x = i * (WIDTH + PAD) + PAD;
		y = (ARRAY_SIZE (draw_funcs) * k + j) * (HEIGHT + PAD) + PAD;

		cairo_save (cr);

		cairo_move_to (cr, x, y);
		clip_funcs[k] (cr, x, y);
		pattern_funcs[i] (cr, x, y);
		draw_funcs[j] (cr, x, y);
		if (cairo_status (cr))
		    cairo_test_log ("%d %d HERE!\n", i, j);

		cairo_restore (cr);
	    }
	}
    }

    if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
	cairo_test_log ("%d %d .HERE!\n", i, j);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test, draw);
}
