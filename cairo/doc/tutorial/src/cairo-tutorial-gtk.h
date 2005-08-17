/* cairo-tutorial-gtk.h - a tutorial framework for cairo with gtk+
 *
 * Copyright © 2005, Carl Worth
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>
#include <cairo.h>

#ifndef WIDTH
#define WIDTH 400
#endif

#ifndef HEIGHT
#define HEIGHT 400
#endif

static void
draw (cairo_t *cr);

#if ! GTK_CHECK_VERSION(2,7,0)
/* copied from gtk+/gdk/gdkcairo.c and gtk+/gdk/x11/gdkdrawable-x11.c
 * gdk_cairo_create() which is available in 2.7.0 and later.
 */
static cairo_t *
gdk_cairo_create (GdkDrawable *drawable)
{
    int width, height;
    cairo_t *cr = NULL;
    cairo_surface_t *surface = NULL;
    GdkVisual *visual = gdk_drawable_get_visual (drawable);

    gdk_drawable_get_size (drawable, &width, &height);
    if (visual) 
	surface = cairo_xlib_surface_create (GDK_DRAWABLE_XDISPLAY (drawable),
					     GDK_DRAWABLE_XID (drawable),
					     GDK_VISUAL_XVISUAL (visual),
					     width, height);
    else if (gdk_drawable_get_depth (drawable) == 1)
	surface = cairo_xlib_surface_create_for_bitmap 
	    (GDK_PIXMAP_XDISPLAY (drawable),
	     GDK_PIXMAP_XID (drawable),
	     GDK_SCREEN_XSCREEN (gdk_drawable_get_screen (drawable)),
	     width, height);
    else {
	g_warning ("Using Cairo rendering requires the drawable argument to\n"
		   "have a specified colormap. All windows have a colormap,\n"
		   "however, pixmaps only have colormap by default if they\n"
		   "were created with a non-NULL window argument. Otherwise\n"
		   "a colormap must be set on them with "
		   "gdk_drawable_set_colormap");
	return NULL;
    }
    if (surface) {
	cr = cairo_create (surface);
	cairo_surface_destroy (surface);
    }
    return cr;
}
#endif

static gboolean
handle_expose (GtkWidget      *widget,
	       GdkEventExpose *event,
	       gpointer	       data)
{
    cairo_t *cr;

    cr = gdk_cairo_create (widget->window);

    draw (cr);

    cairo_destroy (cr);

    return FALSE;
}

int 
main (int argc, char **argv)
{
    GtkWidget *window, *drawing_area;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_window_set_title (GTK_WINDOW (window), "cairo demo");

    g_signal_connect (window, "destroy-event",
		      G_CALLBACK (gtk_main_quit), NULL);

    drawing_area = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (window), drawing_area);

    g_signal_connect (drawing_area, "expose-event",
		      G_CALLBACK (handle_expose), NULL);

    gtk_widget_show_all (window);

    gtk_main ();
    
    return 0;
}
