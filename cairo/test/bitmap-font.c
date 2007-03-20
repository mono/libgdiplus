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
 * Author: Carl D. Worth <cworth@cworth.org>
 */

#include "cairo-test.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cairo-ft.h>
#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>

#define FONT "6x13.pcf"
#define TEXT_SIZE 13

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "bitmap-font",
    "Test drawing with a font consisting only of bitmaps",
    246 + 1, 2 * TEXT_SIZE,
    draw
};

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    FcPattern *pattern;
    cairo_font_face_t *font_face;
    cairo_status_t status;
    const char *srcdir = getenv ("srcdir");
    char *filename;
    int face_count;
    struct stat stat_buf;

    if (! srcdir)
	srcdir = ".";

    xasprintf (&filename, "%s/%s", srcdir, FONT);

    if (stat (filename, &stat_buf) || ! S_ISREG (stat_buf.st_mode)) {
	cairo_test_log ("Error finding font: %s: file not found?\n", filename);
	return CAIRO_TEST_FAILURE;
    }

    pattern = FcFreeTypeQuery ((unsigned char *)filename, 0, NULL, &face_count);
    free (filename);
    if (! pattern) {
	cairo_test_log ("FcFreeTypeQuery failed.\n");
	return CAIRO_TEST_FAILURE;
    }

    font_face = cairo_ft_font_face_create_for_pattern (pattern);

    status = cairo_font_face_status (font_face);
    if (status) {
	cairo_test_log ("Error creating font face for %s: %s\n",
			filename,
			cairo_status_to_string (status));
	return CAIRO_TEST_FAILURE;
    }

    if (cairo_font_face_get_type (font_face) != CAIRO_FONT_TYPE_FT) {
	cairo_test_log ("Unexpected value from cairo_font_face_get_type: %d (expected %d)\n",
			cairo_font_face_get_type (font_face), CAIRO_FONT_TYPE_FT);
	cairo_font_face_destroy (font_face);
	return CAIRO_TEST_FAILURE;
    }

    cairo_set_font_face (cr, font_face);

    FcPatternDestroy (pattern);
    cairo_font_face_destroy (font_face);

    cairo_move_to (cr, 1, TEXT_SIZE - 3);
    cairo_set_source_rgb (cr, 0.0, 0.0, 1.0); /* blue */
    cairo_show_text (cr, "the quick brown fox");

    /* Switch from show_text to text_path/fill to exercise bug #7889 */
    cairo_text_path (cr, " jumps over a lazy dog");
    cairo_fill (cr);

    /* And test it rotated as well for the sake of bug #7888 */

    /* XXX: The math for the vertical positioning here is all wrong,
     * but it is landing where I want it. Someone who understands
     * fonts at all should fix this. */
    cairo_move_to (cr, width -1, 2 * (TEXT_SIZE - 5));
    cairo_rotate (cr, M_PI);
    cairo_show_text (cr, "the quick");
    cairo_show_text (cr, " brown fox");

    cairo_text_path (cr, " jumps over");
    cairo_text_path (cr, " a lazy dog");
    cairo_fill (cr);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    return cairo_test (&test);
}
