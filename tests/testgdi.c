/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*-
 * 
 * testgdi.c : simple test hack
 *
 * Authors:
 *	Vladimir Vukicevic <vladimir@pobox.com>
 *
 * Based on cairo test code.
 *
 * Modify the body of win_draw() to test your own code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <cairo.h>


#include "gdip.h"

typedef struct win {
    Display *dpy;
    int scr;
    Window win;
    GC gc;
    int width, height;
    KeyCode quit_code;
} win_t;

static void win_init(win_t *win);
static void win_deinit(win_t *win);
static void win_draw(win_t *win);
static void win_handle_events(win_t *win);

static void
win_draw(win_t *win)
{
    GpGraphics *gp;
    GpStatus st;
    GpImage *img;
    gunichar2 *unis;

    XClearWindow(win->dpy, win->win);

    gp = gdip_graphics_new ();

    cairo_set_target_drawable (gp->ct, win->dpy, win->win);

    unis = g_utf8_to_utf16 ("test.jpg", -1, NULL, NULL, NULL);
    st = GdipLoadImageFromFile (unis, &img);
    if (st != Ok) {
        printf ("st after load: %d\n", st);
        exit(-1);
    }
    g_free (unis);

    st = GdipDrawImage (gp, img, 0, 0);
    if (st != Ok) {
        printf ("st after draw: %d\n", st);
        exit(-1);
    }

    unis = g_utf8_to_utf16 ("test.tif", -1, NULL, NULL, NULL);
    st = GdipLoadImageFromFile (unis, &img);
    st = GdipDrawImage (gp, img, 100, 0);
}

int
main(int argc, char *argv[])
{
    win_t win;

    win.dpy = XOpenDisplay(0);

    if (win.dpy == NULL) {
	fprintf(stderr, "Failed to open display\n");
	return 1;
    }

    win_init(&win);

    win_draw(&win);

    win_handle_events(&win);

    win_deinit(&win);

    XCloseDisplay(win.dpy);

    return 0;
}


static void
win_init(win_t *win)
{
    Window root;

    win->width = 400;
    win->height = 400;

    root = DefaultRootWindow(win->dpy);
    win->scr = DefaultScreen(win->dpy);

    win->win = XCreateSimpleWindow(win->dpy, root, 0, 0,
				   win->width, win->height, 0,
				   BlackPixel(win->dpy, win->scr), BlackPixel(win->dpy, win->scr));

    win->quit_code = XKeysymToKeycode(win->dpy, XStringToKeysym("Q"));

    XSelectInput(win->dpy, win->win,
		 KeyPressMask
		 |StructureNotifyMask
		 |ExposureMask);

    XMapWindow(win->dpy, win->win);
}

static void
win_deinit(win_t *win)
{
    XDestroyWindow(win->dpy, win->win);
}

static void
win_handle_events(win_t *win)
{
    XEvent xev;

    while (1) {
	XNextEvent(win->dpy, &xev);
	switch(xev.type) {
	case KeyPress:
	{
	    XKeyEvent *kev = &xev.xkey;
	    
	    if (kev->keycode == win->quit_code) {
		return;
	    }
	}
	break;
	case ConfigureNotify:
	{
	    XConfigureEvent *cev = &xev.xconfigure;

	    win->width = cev->width;
	    win->height = cev->height;
	}
	break;
	case Expose:
	{
	    XExposeEvent *eev = &xev.xexpose;

	    if (eev->count == 0)
		win_draw(win);
	}
	break;
	}
    }
}
