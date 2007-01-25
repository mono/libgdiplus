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

#include <stdio.h>
#include <stdlib.h>
#include <poppler.h>

#define FAIL(msg)							\
    do { fprintf (stderr, "FAIL: %s\n", msg); exit (-1); } while (0)

#define PIXELS_PER_POINT 1

int main (int argc, char *argv[])
{
    PopplerDocument *document;
    PopplerPage *page;
    GdkPixbuf *pixbuf;
    double width, height;
    GError *error;
    const char *filename = argv[1];
    const char *output_filename = argv[2];
    const char *page_label = argv[3];
    gchar *absolute, *uri;

    if (argc != 4)
	FAIL ("usage: pdf2png input_file.pdf output_file.png page");

    g_type_init ();

    error = NULL;

    if (g_path_is_absolute(filename)) {
	absolute = g_strdup (filename);
    } else {
	gchar *dir = g_get_current_dir ();
	absolute = g_build_filename (dir, filename, (gchar *) 0);
	free (dir);
    }

    uri = g_filename_to_uri (absolute, NULL, &error);
    free (absolute);
    if (uri == NULL)
	FAIL (error->message);

    document = poppler_document_new_from_file (uri, NULL, &error);
    if (document == NULL)
	FAIL (error->message);

    page = poppler_document_get_page_by_label (document, page_label);
    if (page == NULL)
	FAIL ("page not found");

    poppler_page_get_size (page, &width, &height);

    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
			     width * PIXELS_PER_POINT,
			     height * PIXELS_PER_POINT);
    gdk_pixbuf_fill (pixbuf, 0xffffffff);
    poppler_page_render_to_pixbuf (page, 0, 0, width , height,
				   PIXELS_PER_POINT, 0, pixbuf);

    gdk_pixbuf_save (pixbuf, output_filename, "png", &error, NULL);
    if (error != NULL)
	FAIL (error->message);

    return 0;
}
