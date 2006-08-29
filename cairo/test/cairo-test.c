/*
 * Copyright © 2004 Red Hat, Inc.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <setjmp.h>
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#include <assert.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#if HAVE_FCFINI
#include <fontconfig/fontconfig.h>
#endif

#include "cairo-test.h"

#include "buffer-diff.h"
#include "read-png.h"
#include "write-png.h"
#include "xmalloc.h"

/* This is copied from cairoint.h. That makes it painful to keep in
 * sync, but the slim stuff makes cairoint.h "hard" to include when
 * not actually building the cairo library itself. Fortunately, since
 * we're checking all these values, we do have a safeguard for keeping
 * them in sync.
 */
typedef enum cairo_internal_surface_type {
    CAIRO_INTERNAL_SURFACE_TYPE_META = 0x1000,
    CAIRO_INTERNAL_SURFACE_TYPE_PAGINATED,
    CAIRO_INTERNAL_SURFACE_TYPE_ANALYSIS,
    CAIRO_INTERNAL_SURFACE_TYPE_TEST_META,
    CAIRO_INTERNAL_SURFACE_TYPE_TEST_FALLBACK,
    CAIRO_INTERNAL_SURFACE_TYPE_TEST_PAGINATED
} cairo_internal_surface_type_t;

static const char *vector_ignored_tests[] = {
    /* We can't match the results of tests that depend on
     * CAIRO_ANTIALIAS_NONE/SUBPIXEL for vector backends
     * (nor do we care). */
    "ft-text-antialias-none",
    "rectangle-rounding-error",
    "text-antialias-gray",
    "text-antialias-none",
    "text-antialias-subpixel",
    "unantialiased-shapes",
    NULL
};

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#define access _access
#define F_OK 0
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE !FALSE
#endif

static void
xunlink (const char *pathname);

static const char *fail_face = "", *normal_face = "";

#define CAIRO_TEST_LOG_SUFFIX ".log"
#define CAIRO_TEST_PNG_SUFFIX "-out.png"
#define CAIRO_TEST_REF_SUFFIX "-ref.png"
#define CAIRO_TEST_DIFF_SUFFIX "-diff.png"

#define NUM_DEVICE_OFFSETS 2

/* A fake format we use for the flattened ARGB output of the PS and
 * PDF surfaces. */
#define CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED ((unsigned int) -1)

/* Static data is messy, but we're coding for tests here, not a
 * general-purpose library, and it keeps the tests cleaner to avoid a
 * context object there, (though not a whole lot). */
FILE *cairo_test_log_file = NULL;
const char *srcdir;

/* Used to catch crashes in a test, such that we report it as such and
 * continue testing, although one crasher may already have corrupted memory in
 * an nonrecoverable fashion. */
jmp_buf jmpbuf;

void
cairo_test_init (const char *test_name)
{
    char *log_name;

    xasprintf (&log_name, "%s%s", test_name, CAIRO_TEST_LOG_SUFFIX);
    xunlink (log_name);

    cairo_test_log_file = fopen (log_name, "a");
    if (cairo_test_log_file == NULL) {
	fprintf (stderr, "Error opening log file: %s\n", log_name);
	cairo_test_log_file = stderr;
    }
    free (log_name);

    printf ("\nTESTING %s\n", test_name);
}

void
cairo_test_log (const char *fmt, ...)
{
    va_list va;
    FILE *file = cairo_test_log_file ? cairo_test_log_file : stderr;

    va_start (va, fmt);
    vfprintf (file, fmt, va);
    va_end (va);
}

void
xasprintf (char **strp, const char *fmt, ...)
{
#ifdef HAVE_VASPRINTF
    va_list va;
    int ret;

    va_start (va, fmt);
    ret = vasprintf (strp, fmt, va);
    va_end (va);

    if (ret < 0) {
	cairo_test_log ("Out of memory\n");
	exit (1);
    }
#else /* !HAVE_VASNPRINTF */
#define BUF_SIZE 1024
    va_list va;
    char buffer[BUF_SIZE];
    int ret;

    va_start (va, fmt);
    ret = vsnprintf (buffer, sizeof(buffer), fmt, va);
    va_end (va);

    if (ret < 0) {
	cairo_test_log ("Failure in vsnprintf\n");
	exit (1);
    }

    if (strlen (buffer) == sizeof(buffer) - 1) {
	cairo_test_log ("Overflowed fixed buffer\n");
	exit (1);
    }

    *strp = strdup (buffer);
    if (!*strp) {
	cairo_test_log ("Out of memory\n");
	exit (1);
    }
#endif /* !HAVE_VASNPRINTF */
}

static void
xunlink (const char *pathname)
{
    if (unlink (pathname) < 0 && errno != ENOENT) {
	cairo_test_log ("Error: Cannot remove %s: %s\n",
			pathname, strerror (errno));
	exit (1);
    }
}

typedef cairo_surface_t *
(*cairo_test_create_target_surface_t) (cairo_test_t	 *test,
				       cairo_content_t	  content,
				       void		**closure);

typedef cairo_status_t
(*cairo_test_write_to_png_t) (cairo_surface_t *surface, const char *filename);

typedef void
(*cairo_test_cleanup_target_t) (void *closure);

typedef struct _cairo_test_target
{
    const char		       	       *name;
    cairo_surface_type_t		expected_type;
    cairo_content_t			content;
    cairo_test_create_target_surface_t	create_target_surface;
    cairo_test_write_to_png_t		write_to_png;
    cairo_test_cleanup_target_t		cleanup_target;
    void			       *closure;
} cairo_test_target_t;

static const char *
_cairo_test_content_name (cairo_content_t content)
{
    /* For the purpose of the content name, we don't distinguish the
     * flattened content value.
     */
    if (content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED)
	content = CAIRO_CONTENT_COLOR_ALPHA;

    switch (content) {
    case CAIRO_CONTENT_COLOR:
	return "rgb24";
    case CAIRO_CONTENT_COLOR_ALPHA:
	return "argb32";
    case CAIRO_CONTENT_ALPHA:
    default:
	assert (0); /* not reached */
	return "---";
    }
}

static cairo_surface_t *
create_image_surface (cairo_test_t	 *test,
		      cairo_content_t	  content,
		      void		**closure)
{
    cairo_format_t format;
    *closure = NULL;

    if (content == CAIRO_CONTENT_COLOR_ALPHA) {
	format = CAIRO_FORMAT_ARGB32;
    } else if (content == CAIRO_CONTENT_COLOR) {
	format = CAIRO_FORMAT_RGB24;
    } else {
	assert (0); /* not reached */
	return NULL;
    }

    return cairo_image_surface_create (format, test->width, test->height);
}

#ifdef CAIRO_HAS_TEST_SURFACES

#include "test-fallback-surface.h"
#include "test-meta-surface.h"
#include "test-paginated-surface.h"

static cairo_surface_t *
create_test_fallback_surface (cairo_test_t	 *test,
			      cairo_content_t	  content,
			      void		**closure)
{
    *closure = NULL;
    return _test_fallback_surface_create (content, test->width, test->height);
}

static cairo_surface_t *
create_test_meta_surface (cairo_test_t		 *test,
			  cairo_content_t	  content,
			  void			**closure)
{
    *closure = NULL;
    return _test_meta_surface_create (content, test->width, test->height);
}

static const cairo_user_data_key_t test_paginated_closure_key;

typedef struct {
    unsigned char *data;
    cairo_content_t content;
    int width;
    int height;
    int stride;
} test_paginated_closure_t;

static cairo_surface_t *
create_test_paginated_surface (cairo_test_t	 *test,
			       cairo_content_t	  content,
			       void		**closure)
{
    test_paginated_closure_t *tpc;
    cairo_surface_t *surface;

    *closure = tpc = xmalloc (sizeof (test_paginated_closure_t));

    tpc->content = content;
    tpc->width = test->width;
    tpc->height = test->height;
    tpc->stride = test->width * 4;

    tpc->data = xcalloc (tpc->stride * test->height, 1);

    surface = _test_paginated_surface_create_for_data (tpc->data,
						       tpc->content,
						       tpc->width,
						       tpc->height,
						       tpc->stride);

    cairo_surface_set_user_data (surface, &test_paginated_closure_key,
				 tpc, NULL);

    return surface;
}

/* The only reason we go through all these machinations to write a PNG
 * image is to _really ensure_ that the data actually landed in our
 * buffer through the paginated surface to the test_paginated_surface.
 *
 * If we didn't implement this function then the default
 * cairo_surface_write_to_png would result in the paginated_surface's
 * acquire_source_image function replaying the meta-surface to an
 * intermediate image surface. And in that case the
 * test_paginated_surface would not be involved and wouldn't be
 * tested.
 */
static cairo_status_t
test_paginated_write_to_png (cairo_surface_t *surface,
			     const char	     *filename)
{
    cairo_surface_t *image;
    cairo_format_t format;
    test_paginated_closure_t *tpc;

    tpc = cairo_surface_get_user_data (surface, &test_paginated_closure_key);

    switch (tpc->content) {
    case CAIRO_CONTENT_COLOR:
	format = CAIRO_FORMAT_RGB24;
	break;
    case CAIRO_CONTENT_COLOR_ALPHA:
	format = CAIRO_FORMAT_ARGB32;
	break;
    case CAIRO_CONTENT_ALPHA:
    default:
	assert (0); /* not reached */
	return CAIRO_STATUS_NO_MEMORY;
    }

    image = cairo_image_surface_create_for_data (tpc->data,
						 format,
						 tpc->width,
						 tpc->height,
						 tpc->stride);

    cairo_surface_write_to_png (image, filename);

    cairo_surface_destroy (image);

    return CAIRO_STATUS_SUCCESS;
}

static void
cleanup_test_paginated (void *closure)
{
    test_paginated_closure_t *tpc = closure;

    free (tpc->data);
    free (tpc);
}

#endif

#ifdef CAIRO_HAS_GLITZ_SURFACE
#include <glitz.h>
#include <cairo-glitz.h>

static const cairo_user_data_key_t glitz_closure_key;

typedef struct _glitz_target_closure_base {
    int width;
    int height;
    cairo_content_t content;
} glitz_target_closure_base_t;

#if CAIRO_CAN_TEST_GLITZ_GLX_SURFACE
#include <glitz-glx.h>

typedef struct _glitz_glx_target_closure {
    glitz_target_closure_base_t base;
    Display        *dpy;
    int             scr;
    Window          win;
} glitz_glx_target_closure_t;

static glitz_surface_t *
create_glitz_glx_surface (glitz_format_name_t	      formatname,
			  int			      width,
			  int			      height,
			  glitz_glx_target_closure_t *closure)
{
    Display                 * dpy = closure->dpy;
    int                       scr = closure->scr;
    glitz_drawable_format_t   templ;
    glitz_drawable_format_t * dformat = NULL;
    unsigned long             mask;
    glitz_drawable_t        * drawable = NULL;
    glitz_format_t          * format;
    glitz_surface_t         * sr;

    XSizeHints                xsh;
    XSetWindowAttributes      xswa;
    XVisualInfo             * vinfo;

    memset(&templ, 0, sizeof(templ));
    templ.color.red_size = 8;
    templ.color.green_size = 8;
    templ.color.blue_size = 8;
    templ.color.alpha_size = 8;
    templ.color.fourcc = GLITZ_FOURCC_RGB;
    templ.samples = 1;

    glitz_glx_init (NULL);

    mask = GLITZ_FORMAT_SAMPLES_MASK | GLITZ_FORMAT_FOURCC_MASK |
	GLITZ_FORMAT_RED_SIZE_MASK | GLITZ_FORMAT_GREEN_SIZE_MASK |
	GLITZ_FORMAT_BLUE_SIZE_MASK;
    if (formatname == GLITZ_STANDARD_ARGB32)
	mask |= GLITZ_FORMAT_ALPHA_SIZE_MASK;

    /* Try for a pbuffer first */
    if (!getenv("CAIRO_TEST_FORCE_GLITZ_WINDOW"))
	dformat = glitz_glx_find_pbuffer_format (dpy, scr, mask, &templ, 0);

    if (dformat) {
	closure->win = None;

	drawable = glitz_glx_create_pbuffer_drawable (dpy, scr, dformat,
						      width, height);
	if (!drawable)
	    goto FAIL;
    } else {
	/* No pbuffer, try window */
	dformat = glitz_glx_find_window_format (dpy, scr, mask, &templ, 0);

	if (!dformat)
	    goto FAIL;

	vinfo = glitz_glx_get_visual_info_from_format(dpy,
						      DefaultScreen(dpy),
						      dformat);

	if (!vinfo)
	    goto FAIL;

	xsh.flags = PSize;
	xsh.x = 0;
	xsh.y = 0;
	xsh.width = width;
	xsh.height = height;

	xswa.colormap = XCreateColormap (dpy, RootWindow(dpy, scr),
					 vinfo->visual, AllocNone);
	closure->win = XCreateWindow (dpy, RootWindow(dpy, scr),
				      xsh.x, xsh.y, xsh.width, xsh.height,
				      0, vinfo->depth, CopyFromParent,
				      vinfo->visual, CWColormap, &xswa);
	XFree (vinfo);

	drawable =
	    glitz_glx_create_drawable_for_window (dpy, scr,
						  dformat, closure->win,
						  width, height);

	if (!drawable)
	    goto DESTROY_WINDOW;
    }

    format = glitz_find_standard_format (drawable, formatname);
    if (!format)
	goto DESTROY_DRAWABLE;

    sr = glitz_surface_create (drawable, format, width, height, 0, NULL);
    if (!sr)
	goto DESTROY_DRAWABLE;

    if (closure->win == None || dformat->doublebuffer) {
	glitz_surface_attach (sr, drawable, GLITZ_DRAWABLE_BUFFER_BACK_COLOR);
    } else {
	XMapWindow (closure->dpy, closure->win);
	glitz_surface_attach (sr, drawable, GLITZ_DRAWABLE_BUFFER_FRONT_COLOR);
    }

    glitz_drawable_destroy (drawable);

    return sr;
 DESTROY_DRAWABLE:
    glitz_drawable_destroy (drawable);
 DESTROY_WINDOW:
    if (closure->win)
	XDestroyWindow (dpy, closure->win);
 FAIL:
    return NULL;
}

static cairo_surface_t *
create_cairo_glitz_glx_surface (cairo_test_t   *test,
				cairo_content_t content,
				void    **closure)
{
    int width = test->width;
    int height = test->height;
    glitz_glx_target_closure_t *gxtc;
    glitz_surface_t  * glitz_surface;
    cairo_surface_t  * surface;

    *closure = gxtc = xmalloc (sizeof (glitz_glx_target_closure_t));

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

    gxtc->dpy = XOpenDisplay (getenv("CAIRO_TEST_GLITZ_DISPLAY"));
    if (!gxtc->dpy) {
	cairo_test_log ("Failed to open display: %s\n", XDisplayName(0));
	goto FAIL;
    }

    XSynchronize (gxtc->dpy, 1);

    gxtc->scr = DefaultScreen(gxtc->dpy);

    switch (content) {
    case CAIRO_CONTENT_COLOR:
	glitz_surface = create_glitz_glx_surface (GLITZ_STANDARD_RGB24, width, height, gxtc);
	break;
    case CAIRO_CONTENT_COLOR_ALPHA:
	glitz_surface = create_glitz_glx_surface (GLITZ_STANDARD_ARGB32, width, height, gxtc);
	break;
    default:
	cairo_test_log ("Invalid content for glitz-glx test: %d\n", content);
	goto FAIL_CLOSE_DISPLAY;
    }
    if (!glitz_surface) {
	cairo_test_log ("Failed to create glitz-glx surface\n");
	goto FAIL_CLOSE_DISPLAY;
    }

    surface = cairo_glitz_surface_create (glitz_surface);

    gxtc->base.width = test->width;
    gxtc->base.height = test->height;
    gxtc->base.content = content;
    cairo_surface_set_user_data (surface, &glitz_closure_key,
				 gxtc, NULL);

    return surface;

 FAIL_CLOSE_DISPLAY:
    XCloseDisplay (gxtc->dpy);
 FAIL:
    return NULL;
}

static void
cleanup_cairo_glitz_glx (void *closure)
{
    glitz_glx_target_closure_t *gxtc = closure;

    glitz_glx_fini ();

    if (gxtc->win)
	XDestroyWindow (gxtc->dpy, gxtc->win);

    XCloseDisplay (gxtc->dpy);

    free (gxtc);
}

#endif /* CAIRO_CAN_TEST_GLITZ_GLX_SURFACE */

#if CAIRO_CAN_TEST_GLITZ_AGL_SURFACE
#include <glitz-agl.h>

typedef struct _glitz_agl_target_closure {
    glitz_target_closure_base_t base;
} glitz_agl_target_closure_t;

static glitz_surface_t *
create_glitz_agl_surface (glitz_format_name_t formatname,
			  int width, int height,
			  glitz_agl_target_closure_t *closure)
{
    glitz_drawable_format_t *dformat;
    glitz_drawable_format_t templ;
    glitz_drawable_t *gdraw;
    glitz_format_t *format;
    glitz_surface_t *sr = NULL;
    unsigned long mask;

    memset(&templ, 0, sizeof(templ));
    templ.color.red_size = 8;
    templ.color.green_size = 8;
    templ.color.blue_size = 8;
    templ.color.alpha_size = 8;
    templ.color.fourcc = GLITZ_FOURCC_RGB;
    templ.samples = 1;

    mask = GLITZ_FORMAT_SAMPLES_MASK | GLITZ_FORMAT_FOURCC_MASK |
	GLITZ_FORMAT_RED_SIZE_MASK | GLITZ_FORMAT_GREEN_SIZE_MASK |
	GLITZ_FORMAT_BLUE_SIZE_MASK;
    if (formatname == GLITZ_STANDARD_ARGB32)
	mask |= GLITZ_FORMAT_ALPHA_SIZE_MASK;

    dformat = glitz_agl_find_pbuffer_format (mask, &templ, 0);
    if (!dformat) {
	cairo_test_log ("Glitz failed to find pbuffer format for template.");
	goto FAIL;
    }

    gdraw = glitz_agl_create_pbuffer_drawable (dformat, width, height);
    if (!gdraw) {
	cairo_test_log ("Glitz failed to create pbuffer drawable.");
	goto FAIL;
    }

    format = glitz_find_standard_format (gdraw, formatname);
    if (!format) {
	cairo_test_log ("Glitz failed to find standard format for drawable.");
	goto DESTROY_DRAWABLE;
    }

    sr = glitz_surface_create (gdraw, format, width, height, 0, NULL);
    if (!sr) {
	cairo_test_log ("Glitz failed to create a surface.");
	goto DESTROY_DRAWABLE;
    }

    glitz_surface_attach (sr, gdraw, GLITZ_DRAWABLE_BUFFER_FRONT_COLOR);

 DESTROY_DRAWABLE:
    glitz_drawable_destroy (gdraw);

 FAIL:
    return sr; /* will be NULL unless we create it and attach */
}

static cairo_surface_t *
create_cairo_glitz_agl_surface (cairo_test_t *test,
				cairo_content_t content,
				void **closure)
{
    glitz_surface_t *glitz_surface;
    cairo_surface_t *surface;
    glitz_agl_target_closure_t *aglc;

    glitz_agl_init ();

    *closure = aglc = xmalloc (sizeof (glitz_agl_target_closure_t));

    switch (content) {
    case CAIRO_CONTENT_COLOR:
	glitz_surface = create_glitz_agl_surface (GLITZ_STANDARD_RGB24, test->width, test->height, NULL);
	break;
    case CAIRO_CONTENT_COLOR_ALPHA:
	glitz_surface = create_glitz_agl_surface (GLITZ_STANDARD_ARGB32, test->width, test->height, NULL);
	break;
    default:
	cairo_test_log ("Invalid content for glitz-agl test: %d\n", content);
	goto FAIL;
    }

    if (!glitz_surface)
	goto FAIL;

    surface = cairo_glitz_surface_create (glitz_surface);

    aglc->base.width = test->width;
    aglc->base.height = test->height;
    aglc->base.content = content;
    cairo_surface_set_user_data (surface, &glitz_closure_key, aglc, NULL);

    return surface;

 FAIL:
    return NULL;
}

static void
cleanup_cairo_glitz_agl (void *closure)
{
    free (closure);
    glitz_agl_fini ();
}

#endif /* CAIRO_CAN_TEST_GLITZ_AGL_SURFACE */

#if CAIRO_CAN_TEST_GLITZ_WGL_SURFACE
#include <glitz-wgl.h>

typedef struct _glitz_wgl_target_closure {
    glitz_target_closure_base_t base;
} glitz_wgl_target_closure_t;

static glitz_surface_t *
create_glitz_wgl_surface (glitz_format_name_t formatname,
			  int width, int height,
			  glitz_wgl_target_closure_t *closure)
{
    glitz_drawable_format_t *dformat;
    glitz_drawable_format_t templ;
    glitz_drawable_t *gdraw;
    glitz_format_t *format;
    glitz_surface_t *sr = NULL;
    unsigned long mask;

    memset(&templ, 0, sizeof(templ));
    templ.color.red_size = 8;
    templ.color.green_size = 8;
    templ.color.blue_size = 8;
    templ.color.alpha_size = 8;
    templ.color.fourcc = GLITZ_FOURCC_RGB;
    templ.samples = 1;

    mask = GLITZ_FORMAT_SAMPLES_MASK | GLITZ_FORMAT_FOURCC_MASK |
	GLITZ_FORMAT_RED_SIZE_MASK | GLITZ_FORMAT_GREEN_SIZE_MASK |
	GLITZ_FORMAT_BLUE_SIZE_MASK;
    if (formatname == GLITZ_STANDARD_ARGB32)
	mask |= GLITZ_FORMAT_ALPHA_SIZE_MASK;

    dformat = glitz_wgl_find_pbuffer_format (mask, &templ, 0);
    if (!dformat) {
	cairo_test_log ("Glitz failed to find pbuffer format for template.");
	goto FAIL;
    }

    gdraw = glitz_wgl_create_pbuffer_drawable (dformat, width, height);
    if (!gdraw) {
	cairo_test_log ("Glitz failed to create pbuffer drawable.");
	goto FAIL;
    }

    format = glitz_find_standard_format (gdraw, formatname);
    if (!format) {
	cairo_test_log ("Glitz failed to find standard format for drawable.");
	goto DESTROY_DRAWABLE;
    }

    sr = glitz_surface_create (gdraw, format, width, height, 0, NULL);
    if (!sr) {
	cairo_test_log ("Glitz failed to create a surface.");
	goto DESTROY_DRAWABLE;
    }

    glitz_surface_attach (sr, gdraw, GLITZ_DRAWABLE_BUFFER_FRONT_COLOR);

 DESTROY_DRAWABLE:
    glitz_drawable_destroy (gdraw);

 FAIL:
    return sr; /* will be NULL unless we create it and attach */
}

static cairo_surface_t *
create_cairo_glitz_wgl_surface (cairo_test_t *test,
				cairo_content_t content,
				void **closure)
{
    glitz_surface_t *glitz_surface;
    cairo_surface_t *surface;
    glitz_wgl_target_closure_t *wglc;

    glitz_wgl_init (NULL);

    *closure = wglc = xmalloc (sizeof (glitz_wgl_target_closure_t));

    switch (content) {
    case CAIRO_CONTENT_COLOR:
	glitz_surface = create_glitz_wgl_surface (GLITZ_STANDARD_RGB24, test->width, test->height, NULL);
	break;
    case CAIRO_CONTENT_COLOR_ALPHA:
	glitz_surface = create_glitz_wgl_surface (GLITZ_STANDARD_ARGB32, test->width, test->height, NULL);
	break;
    default:
	cairo_test_log ("Invalid content for glitz-wgl test: %d\n", content);
	goto FAIL;
    }

    if (!glitz_surface)
	goto FAIL;

    surface = cairo_glitz_surface_create (glitz_surface);

    wglc->base.width = test->width;
    wglc->base.height = test->height;
    wglc->base.content = content;
    cairo_surface_set_user_data (surface, &glitz_closure_key, wglc, NULL);

    return surface;

 FAIL:
    return NULL;
}

static void
cleanup_cairo_glitz_wgl (void *closure)
{
    free (closure);
    glitz_wgl_fini ();
}

#endif /* CAIRO_CAN_TEST_GLITZ_WGL_SURFACE */

#endif /* CAIRO_HAS_GLITZ_SURFACE */

#if 0 && CAIRO_HAS_QUARTZ_SURFACE
static cairo_surface_t *
create_quartz_surface (int width, int height, void **closure)
{
#error Not yet implemented
}

static void
cleanup_quartz (void *closure)
{
#error Not yet implemented
}
#endif

/* Testing the win32 surface isn't interesting, since for
 * ARGB images it just chains to the image backend
 */
#if CAIRO_HAS_WIN32_SURFACE
#include "cairo-win32.h"
typedef struct _win32_target_closure
{
  HDC dc;
  HBITMAP bmp;
} win32_target_closure_t;

static cairo_surface_t *
create_win32_surface (cairo_test_t	 *test,
		      cairo_content_t	  content,
		      void		**closure)
{
    int width = test->width;
    int height = test->height;

    BITMAPINFO bmpInfo;
    unsigned char *bits = NULL;
    win32_target_closure_t *data = malloc(sizeof(win32_target_closure_t));
    *closure = data;

    data->dc = CreateCompatibleDC(NULL);

    /* initialize the bitmapinfoheader */
    memset(&bmpInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
    bmpInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = -height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    /* create a DIBSection */
    data->bmp = CreateDIBSection(data->dc, &bmpInfo, DIB_RGB_COLORS, (void**)&bits, NULL, 0);

    /* Flush GDI to make sure the DIBSection is actually created */
    GdiFlush();

    /* Select the bitmap in to the DC */
    SelectObject(data->dc, data->bmp);

    return cairo_win32_surface_create(data->dc);
}

static void
cleanup_win32 (void *closure)
{
  win32_target_closure_t *data = (win32_target_closure_t*)closure;
  DeleteObject(data->bmp);
  DeleteDC(data->dc);

  free(closure);
}
#endif

#if CAIRO_HAS_XCB_SURFACE
#include "cairo-xcb-xrender.h"
typedef struct _xcb_target_closure
{
    XCBConnection *c;
    XCBDRAWABLE drawable;
} xcb_target_closure_t;

/* XXX: This is a nasty hack. Something like this should be in XCB's
 * bindings for Render, not here in this test. */
static XCBRenderPICTFORMINFO
_format_from_cairo(XCBConnection *c, cairo_format_t fmt)
{
    XCBRenderPICTFORMINFO ret = {{ 0 }};
    struct tmpl_t {
	XCBRenderDIRECTFORMAT direct;
	CARD8 depth;
    };
    static const struct tmpl_t templates[] = {
	/* CAIRO_FORMAT_ARGB32 */
	{
	    {
		16, 0xff,
		8,  0xff,
		0,  0xff,
		24, 0xff
	    },
	    32
	},
	/* CAIRO_FORMAT_RGB24 */
	{
	    {
		16, 0xff,
		8,  0xff,
		0,  0xff,
		0,  0x00
	    },
	    24
	},
	/* CAIRO_FORMAT_A8 */
	{
	    {
		0,  0x00,
		0,  0x00,
		0,  0x00,
		0,  0xff
	    },
	    8
	},
	/* CAIRO_FORMAT_A1 */
	{
	    {
		0,  0x00,
		0,  0x00,
		0,  0x00,
		0,  0x01
	    },
	    1
	},
    };
    const struct tmpl_t *tmpl;
    XCBRenderQueryPictFormatsRep *r;
    XCBRenderPICTFORMINFOIter fi;

    if(fmt < 0 || fmt >= (sizeof(templates) / sizeof(*templates)))
	return ret;
    tmpl = templates + fmt;

    r = XCBRenderQueryPictFormatsReply(c, XCBRenderQueryPictFormats(c), 0);
    if(!r)
	return ret;

    for(fi = XCBRenderQueryPictFormatsFormatsIter(r); fi.rem; XCBRenderPICTFORMINFONext(&fi))
    {
	const XCBRenderDIRECTFORMAT *t, *f;
	if(fi.data->type != XCBRenderPictTypeDirect)
	    continue;
	if(fi.data->depth != tmpl->depth)
	    continue;
	t = &tmpl->direct;
	f = &fi.data->direct;
	if(t->red_mask && (t->red_mask != f->red_mask || t->red_shift != f->red_shift))
	    continue;
	if(t->green_mask && (t->green_mask != f->green_mask || t->green_shift != f->green_shift))
	    continue;
	if(t->blue_mask && (t->blue_mask != f->blue_mask || t->blue_shift != f->blue_shift))
	    continue;
	if(t->alpha_mask && (t->alpha_mask != f->alpha_mask || t->alpha_shift != f->alpha_shift))
	    continue;

	ret = *fi.data;
    }

    free(r);
    return ret;
}

static cairo_surface_t *
create_xcb_surface (cairo_test_t	 *test,
		    cairo_content_t	  content,
		    void		**closure)
{
    int width = test->width;
    int height = test->height;
    XCBSCREEN *root;
    xcb_target_closure_t *xtc;
    cairo_surface_t *surface;
    XCBConnection *c;
    XCBRenderPICTFORMINFO render_format;
    cairo_format_t format;

    *closure = xtc = xmalloc (sizeof (xcb_target_closure_t));

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

    xtc->c = c = XCBConnect(NULL,NULL);
    if (c == NULL) {
	cairo_test_log ("Failed to connect to X server through XCB\n");
	return NULL;
    }

    root = XCBSetupRootsIter(XCBGetSetup(c)).data;

    xtc->drawable.pixmap = XCBPIXMAPNew (c);
    {
	XCBDRAWABLE root_drawable;
	root_drawable.window = root->root;
	XCBCreatePixmap (c, 32, xtc->drawable.pixmap, root_drawable,
			 width, height);
    }

    switch (content) {
    case CAIRO_CONTENT_COLOR:
	format = CAIRO_FORMAT_RGB24;
	break;
    case CAIRO_CONTENT_COLOR_ALPHA:
	format = CAIRO_FORMAT_ARGB32;
	break;
    default:
	cairo_test_log ("Invalid content for XCB test: %d\n", content);
	return NULL;
    }

    render_format = _format_from_cairo (c, format);
    if (render_format.id.xid == 0)
	return NULL;
    surface = cairo_xcb_surface_create_with_xrender_format (c, xtc->drawable, root,
							    &render_format,
							    width, height);

    return surface;
}

static void
cleanup_xcb (void *closure)
{
    xcb_target_closure_t *xtc = closure;

    XCBFreePixmap (xtc->c, xtc->drawable.pixmap);
    XCBDisconnect (xtc->c);
    free (xtc);
}
#endif

#if CAIRO_HAS_XLIB_SURFACE
#include "cairo-xlib-xrender.h"
typedef struct _xlib_target_closure
{
    Display *dpy;
    Pixmap pixmap;
} xlib_target_closure_t;

static cairo_surface_t *
create_xlib_surface (cairo_test_t	 *test,
		     cairo_content_t	  content,
		     void		**closure)
{
    int width = test->width;
    int height = test->height;
    xlib_target_closure_t *xtc;
    cairo_surface_t *surface;
    Display *dpy;
    XRenderPictFormat *xrender_format;

    *closure = xtc = xmalloc (sizeof (xlib_target_closure_t));

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

    xtc->dpy = dpy = XOpenDisplay (NULL);
    if (xtc->dpy == NULL) {
	cairo_test_log ("Failed to open display: %s\n", XDisplayName(0));
	return NULL;
    }

    XSynchronize (xtc->dpy, 1);

    /* XXX: Currently we don't do any xlib testing when the X server
     * doesn't have the Render extension. We could do better here,
     * (perhaps by converting the tests from ARGB32 to RGB24). One
     * step better would be to always test the non-Render fallbacks
     * for each test even if the server does have the Render
     * extension. That would probably be through another
     * cairo_test_target which would use an extended version of
     * cairo_test_xlib_disable_render.  */
    switch (content) {
    case CAIRO_CONTENT_COLOR_ALPHA:
	xrender_format = XRenderFindStandardFormat (dpy, PictStandardARGB32);
	break;
    case CAIRO_CONTENT_COLOR:
	xrender_format = XRenderFindStandardFormat (dpy, PictStandardRGB24);
	break;
    case CAIRO_CONTENT_ALPHA:
    default:
	cairo_test_log ("Invalid content for xlib test: %d\n", content);
	return NULL;
    }
    if (xrender_format == NULL) {
	cairo_test_log ("X server does not have the Render extension.\n");
	return NULL;
    }

    xtc->pixmap = XCreatePixmap (dpy, DefaultRootWindow (dpy),
				 width, height, xrender_format->depth);

    surface = cairo_xlib_surface_create_with_xrender_format (dpy, xtc->pixmap,
							     DefaultScreenOfDisplay (dpy),
							     xrender_format,
							     width, height);
    return surface;
}

static void
cleanup_xlib (void *closure)
{
    xlib_target_closure_t *xtc = closure;

    XFreePixmap (xtc->dpy, xtc->pixmap);
    XCloseDisplay (xtc->dpy);
    free (xtc);
}
#endif

#if CAIRO_HAS_BEOS_SURFACE
/* BeOS test functions are external as they need to be C++ */
#include "cairo-test-beos.h"
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
#include "cairo-test-directfb.h"
#endif

#if CAIRO_HAS_PS_SURFACE
#include "cairo-ps.h"

cairo_user_data_key_t	ps_closure_key;

typedef struct _ps_target_closure
{
    char		*filename;
    int			 width;
    int			 height;
    cairo_surface_t	*target;
} ps_target_closure_t;

static cairo_surface_t *
create_ps_surface (cairo_test_t		 *test,
		   cairo_content_t	  content,
		   void			**closure)
{
    int width = test->width;
    int height = test->height;
    ps_target_closure_t	*ptc;
    cairo_surface_t *surface;
    int i;

    for (i = 0; vector_ignored_tests[i] != NULL; i++)
	if (strcmp (test->name, vector_ignored_tests[i]) == 0)
	    return NULL;

    /* Sanitize back to a real cairo_content_t value. */
    if (content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED)
	content = CAIRO_CONTENT_COLOR_ALPHA;

    *closure = ptc = xmalloc (sizeof (ps_target_closure_t));

    xasprintf (&ptc->filename, "%s-ps-%s-out.ps",
	       test->name, _cairo_test_content_name (content));

    ptc->width = width;
    ptc->height = height;

    surface = cairo_ps_surface_create (ptc->filename, width, height);
    if (cairo_surface_status (surface)) {
	free (ptc->filename);
	free (ptc);
	return NULL;
    }
    cairo_surface_set_fallback_resolution (surface, 72., 72.);

    if (content == CAIRO_CONTENT_COLOR) {
	ptc->target = surface;
	surface = cairo_surface_create_similar (ptc->target,
						CAIRO_CONTENT_COLOR,
						width, height);
    } else {
	ptc->target = NULL;
    }

    cairo_surface_set_user_data (surface, &ps_closure_key, ptc, NULL);

    return surface;
}

static cairo_status_t
ps_surface_write_to_png (cairo_surface_t *surface, const char *filename)
{
    ps_target_closure_t *ptc = cairo_surface_get_user_data (surface, &ps_closure_key);
    char    command[4096];

    /* Both surface and ptc->target were originally created at the
     * same dimensions. We want a 1:1 copy here, so we first clear any
     * device offset on surface.
     *
     * In a more realistic use case of device offsets, the target of
     * this copying would be of a different size than the source, and
     * the offset would be desirable during the copy operation. */
    cairo_surface_set_device_offset (surface, 0, 0);

    if (ptc->target) {
	cairo_t *cr;
	cr = cairo_create (ptc->target);
	cairo_set_source_surface (cr, surface, 0, 0);
	cairo_paint (cr);
	cairo_show_page (cr);
	cairo_destroy (cr);

	cairo_surface_finish (surface);
	surface = ptc->target;
    }

    cairo_surface_finish (surface);
    sprintf (command, "gs -q -r72 -g%dx%d -dSAFER -dBATCH -dNOPAUSE -sDEVICE=pngalpha -sOutputFile=%s %s",
	     ptc->width, ptc->height, filename, ptc->filename);
    if (system (command) == 0)
	return CAIRO_STATUS_SUCCESS;
    return CAIRO_STATUS_WRITE_ERROR;
}

static void
cleanup_ps (void *closure)
{
    ps_target_closure_t *ptc = closure;
    if (ptc->target)
	cairo_surface_destroy (ptc->target);
    free (ptc->filename);
    free (ptc);
}
#endif /* CAIRO_HAS_PS_SURFACE */

#if CAIRO_HAS_PDF_SURFACE && CAIRO_CAN_TEST_PDF_SURFACE
#include "cairo-pdf.h"

cairo_user_data_key_t pdf_closure_key;

typedef struct _pdf_target_closure
{
    char		*filename;
    int			 width;
    int			 height;
    cairo_surface_t	*target;
} pdf_target_closure_t;

static cairo_surface_t *
create_pdf_surface (cairo_test_t	 *test,
		    cairo_content_t	  content,
		    void		**closure)
{
    int width = test->width;
    int height = test->height;
    pdf_target_closure_t *ptc;
    cairo_surface_t *surface;
    int i;

    for (i = 0; vector_ignored_tests[i] != NULL; i++)
	if (strcmp (test->name, vector_ignored_tests[i]) == 0)
	    return NULL;

    /* Sanitize back to a real cairo_content_t value. */
    if (content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED)
	content = CAIRO_CONTENT_COLOR_ALPHA;

    *closure = ptc = xmalloc (sizeof (pdf_target_closure_t));

    ptc->width = width;
    ptc->height = height;

    xasprintf (&ptc->filename, "%s-pdf-%s-out.pdf",
	       test->name, _cairo_test_content_name (content));

    surface = cairo_pdf_surface_create (ptc->filename, width, height);
    if (cairo_surface_status (surface)) {
	free (ptc->filename);
	free (ptc);
	return NULL;
    }
    cairo_surface_set_fallback_resolution (surface, 72., 72.);

    if (content == CAIRO_CONTENT_COLOR) {
	ptc->target = surface;
	surface = cairo_surface_create_similar (ptc->target,
						CAIRO_CONTENT_COLOR,
						width, height);
    } else {
	ptc->target = NULL;
    }

    cairo_surface_set_user_data (surface, &pdf_closure_key, ptc, NULL);

    return surface;
}

static cairo_status_t
pdf_surface_write_to_png (cairo_surface_t *surface, const char *filename)
{
    pdf_target_closure_t *ptc = cairo_surface_get_user_data (surface, &pdf_closure_key);
    char    command[4096];

    /* Both surface and ptc->target were originally created at the
     * same dimensions. We want a 1:1 copy here, so we first clear any
     * device offset on surface.
     *
     * In a more realistic use case of device offsets, the target of
     * this copying would be of a different size than the source, and
     * the offset would be desirable during the copy operation. */
    cairo_surface_set_device_offset (surface, 0, 0);

    if (ptc->target) {
	cairo_t *cr;
	cr = cairo_create (ptc->target);
	cairo_set_source_surface (cr, surface, 0, 0);
	cairo_paint (cr);
	cairo_show_page (cr);
	cairo_destroy (cr);

	cairo_surface_finish (surface);
	surface = ptc->target;
    }

    cairo_surface_finish (surface);
    sprintf (command, "./pdf2png %s %s 1",
	     ptc->filename, filename);

    if (system (command) != 0)
	return CAIRO_STATUS_WRITE_ERROR;

    return CAIRO_STATUS_SUCCESS;
}

static void
cleanup_pdf (void *closure)
{
    pdf_target_closure_t *ptc = closure;
    if (ptc->target)
	cairo_surface_destroy (ptc->target);
    free (ptc->filename);
    free (ptc);
}
#endif /* CAIRO_HAS_PDF_SURFACE && CAIRO_CAN_TEST_PDF_SURFACE */

#if CAIRO_HAS_SVG_SURFACE && CAIRO_CAN_TEST_SVG_SURFACE
#include "cairo-svg.h"

cairo_user_data_key_t	svg_closure_key;

typedef struct _svg_target_closure
{
    char    *filename;
    int	    width, height;
    cairo_surface_t	*target;
} svg_target_closure_t;

static cairo_surface_t *
create_svg_surface (cairo_test_t	 *test,
		    cairo_content_t	  content,
		    void		**closure)
{
    int width = test->width;
    int height = test->height;
    int i;
    svg_target_closure_t *ptc;
    cairo_surface_t *surface;

    for (i = 0; vector_ignored_tests[i] != NULL; i++)
	if (strcmp (test->name, vector_ignored_tests[i]) == 0)
	    return NULL;

    *closure = ptc = xmalloc (sizeof (svg_target_closure_t));

    ptc->width = width;
    ptc->height = height;

    xasprintf (&ptc->filename, "%s-svg-%s-out.svg",
	       test->name, _cairo_test_content_name (content));

    surface = cairo_svg_surface_create (ptc->filename, width, height);
    if (cairo_surface_status (surface)) {
	free (ptc->filename);
	free (ptc);
	return NULL;
    }
    cairo_surface_set_fallback_resolution (surface, 72., 72.);

    if (content == CAIRO_CONTENT_COLOR) {
	ptc->target = surface;
	surface = cairo_surface_create_similar (ptc->target,
						CAIRO_CONTENT_COLOR,
						width, height);
    } else {
	ptc->target = NULL;
    }

    cairo_surface_set_user_data (surface, &svg_closure_key, ptc, NULL);

    return surface;
}

static cairo_status_t
svg_surface_write_to_png (cairo_surface_t *surface, const char *filename)
{
    svg_target_closure_t *ptc = cairo_surface_get_user_data (surface, &svg_closure_key);
    char    command[4096];

    /* Both surface and ptc->target were originally created at the
     * same dimensions. We want a 1:1 copy here, so we first clear any
     * device offset on surface.
     *
     * In a more realistic use case of device offsets, the target of
     * this copying would be of a different size than the source, and
     * the offset would be desirable during the copy operation. */
    cairo_surface_set_device_offset (surface, 0, 0);

    if (ptc->target) {
	cairo_t *cr;
	cr = cairo_create (ptc->target);
	cairo_set_source_surface (cr, surface, 0, 0);
	cairo_paint (cr);
	cairo_show_page (cr);
	cairo_destroy (cr);

	cairo_surface_finish (surface);
	surface = ptc->target;
    }

    cairo_surface_finish (surface);
    sprintf (command, "./svg2png %s %s",
	     ptc->filename, filename);

    if (system (command) != 0)
	return CAIRO_STATUS_WRITE_ERROR;

    return CAIRO_STATUS_SUCCESS;
}

static void
cleanup_svg (void *closure)
{
    svg_target_closure_t *ptc = closure;
    if (ptc->target)
	cairo_surface_destroy (ptc->target);
    free (ptc->filename);
    free (ptc);
}
#endif /* CAIRO_HAS_SVG_SURFACE && CAIRO_CAN_TEST_SVG_SURFACE */

static char *
cairo_ref_name_for_test_target_format (const char *test_name,
				       const char *target_name,
				       const char *format)
{
    char *ref_name = NULL;

    /* First look for a target/format-specific reference image. */
    xasprintf (&ref_name, "%s/%s-%s-%s%s", srcdir,
	       test_name,
	       target_name,
	       format,
	       CAIRO_TEST_REF_SUFFIX);
    if (access (ref_name, F_OK) != 0)
	free (ref_name);
    else
	goto done;

    /* Next, look for taget-specifc reference image. */
    xasprintf (&ref_name, "%s/%s-%s%s", srcdir,
	       test_name,
	       target_name,
	       CAIRO_TEST_REF_SUFFIX);
    if (access (ref_name, F_OK) != 0)
	free (ref_name);
    else
	goto done;

    /* Next, look for format-specifc reference image. */
    xasprintf (&ref_name, "%s/%s-%s%s", srcdir,
	       test_name,
	       format,
	       CAIRO_TEST_REF_SUFFIX);
    if (access (ref_name, F_OK) != 0)
	free (ref_name);
    else
	goto done;

    /* Finally, look for the standard reference image. */
    xasprintf (&ref_name, "%s/%s%s", srcdir,
	       test_name,
	       CAIRO_TEST_REF_SUFFIX);
    if (access (ref_name, F_OK) != 0)
	free (ref_name);
    else
	goto done;

    ref_name = NULL;

done:
    return ref_name;
}

static cairo_test_status_t
cairo_test_for_target (cairo_test_t		 *test,
		       cairo_test_target_t	 *target,
		       int			  dev_offset)
{
    cairo_test_status_t status;
    cairo_surface_t *surface;
    cairo_t *cr;
    char *png_name, *ref_name, *diff_name, *offset_str;
    cairo_test_status_t ret;
    cairo_content_t expected_content;
    cairo_font_options_t *font_options;
    const char *format;

    /* Get the strings ready that we'll need. */
    format = _cairo_test_content_name (target->content);
    if (dev_offset)
	xasprintf (&offset_str, "-%d", dev_offset);
    else
	offset_str = strdup("");

    xasprintf (&png_name, "%s-%s-%s%s%s",
	       test->name,
	       target->name,
	       format,
	       offset_str, CAIRO_TEST_PNG_SUFFIX);
    ref_name = cairo_ref_name_for_test_target_format (test->name, target->name, format);
    xasprintf (&diff_name, "%s-%s-%s%s%s",
	       test->name,
	       target->name,
	       format,
	       offset_str, CAIRO_TEST_DIFF_SUFFIX);

    /* Run the actual drawing code. */
    if (test->width && test->height) {
	test->width += dev_offset;
	test->height += dev_offset;
    }

    surface = (target->create_target_surface) (test, target->content, &target->closure);

    if (test->width && test->height) {
	test->width -= dev_offset;
	test->height -= dev_offset;;
    }

    if (surface == NULL) {
	cairo_test_log ("Error: Failed to set %s target\n", target->name);
	ret = CAIRO_TEST_UNTESTED;
	goto UNWIND_STRINGS;
    }

    /* Check that we created a surface of the expected type. */
    if (cairo_surface_get_type (surface) != target->expected_type) {
	cairo_test_log ("Error: Created surface is of type %d (expected %d)\n",
			cairo_surface_get_type (surface), target->expected_type);
	ret = CAIRO_TEST_FAILURE;
	goto UNWIND_SURFACE;
    }

    /* Check that we created a surface of the expected content,
     * (ignore the articifical
     * CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED value).
     */
    expected_content = target->content;
    if (expected_content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED)
	expected_content = CAIRO_CONTENT_COLOR_ALPHA;

    if (cairo_surface_get_content (surface) != expected_content) {
	cairo_test_log ("Error: Created surface has content %d (expected %d)\n",
			cairo_surface_get_content (surface), expected_content);
	ret = CAIRO_TEST_FAILURE;
	goto UNWIND_SURFACE;
    }

    cairo_surface_set_device_offset (surface, dev_offset, dev_offset);

    cr = cairo_create (surface);

    /* Clear to transparent (or black) depending on whether the target
     * surface supports alpha. */
    cairo_save (cr);
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr);
    cairo_restore (cr);

    /* Set all components of font_options to avoid backend differences
     * and reduce number of needed reference images. */
    font_options = cairo_font_options_create ();
    cairo_font_options_set_hint_style (font_options, CAIRO_HINT_STYLE_NONE);
    cairo_font_options_set_hint_metrics (font_options, CAIRO_HINT_METRICS_ON);
    cairo_font_options_set_antialias (font_options, CAIRO_ANTIALIAS_GRAY);
    cairo_set_font_options (cr, font_options);
    cairo_font_options_destroy (font_options);

    status = (test->draw) (cr, test->width, test->height);

    /* Then, check all the different ways it could fail. */
    if (status) {
	cairo_test_log ("Error: Function under test failed\n");
	ret = status;
	goto UNWIND_CAIRO;
    }

    cairo_show_page (cr);

    if (cairo_status (cr) != CAIRO_STATUS_SUCCESS) {
	cairo_test_log ("Error: Function under test left cairo status in an error state: %s\n",
			cairo_status_to_string (cairo_status (cr)));
	ret = CAIRO_TEST_FAILURE;
	goto UNWIND_CAIRO;
    }

    /* Skip image check for tests with no image (width,height == 0,0) */
    if (test->width != 0 && test->height != 0) {
	int pixels_changed;
	xunlink (png_name);
	(target->write_to_png) (surface, png_name);

	if (!ref_name) {
	    cairo_test_log ("Error: Cannot find reference image for %s/%s-%s-%s%s\n",srcdir,
			    test->name,
			    target->name,
			    format,
			    CAIRO_TEST_REF_SUFFIX);
	    ret = CAIRO_TEST_FAILURE;
	    goto UNWIND_CAIRO;
	}

	if (target->content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED)
	    pixels_changed = image_diff_flattened (png_name, ref_name, diff_name, dev_offset, dev_offset, 0, 0);
	else
	    pixels_changed = image_diff (png_name, ref_name, diff_name, dev_offset, dev_offset, 0, 0);
	if (pixels_changed) {
	    if (pixels_changed > 0)
		cairo_test_log ("Error: %d pixels differ from reference image %s\n",
				pixels_changed, ref_name);
	    ret = CAIRO_TEST_FAILURE;
	    goto UNWIND_CAIRO;
	}
    }

    ret = CAIRO_TEST_SUCCESS;

UNWIND_CAIRO:
    cairo_destroy (cr);
UNWIND_SURFACE:
    cairo_surface_destroy (surface);

    cairo_debug_reset_static_data ();

    if (target->cleanup_target)
	target->cleanup_target (target->closure);

UNWIND_STRINGS:
    if (png_name)
      free (png_name);
    if (ref_name)
      free (ref_name);
    if (diff_name)
      free (diff_name);
    if (offset_str)
      free (offset_str);

    return ret;
}

#ifdef HAVE_SIGNAL_H
static void
segfault_handler (int signal)
{
    longjmp (jmpbuf, signal);
}
#endif

static cairo_test_status_t
cairo_test_expecting (cairo_test_t *test,
		      cairo_test_status_t expectation)
{
    /* we use volatile here to make sure values are not clobbered
     * by longjmp */
    volatile size_t i, j, num_targets;
    volatile cairo_bool_t limited_targets = FALSE, print_fail_on_stdout = TRUE;
    const char *tname;
#ifdef HAVE_SIGNAL_H
    void (*old_segfault_handler)(int);
#endif
    volatile cairo_test_status_t status, ret;
    cairo_test_target_t ** volatile targets_to_test;
    cairo_test_target_t targets[] =
	{
	    { "image", CAIRO_SURFACE_TYPE_IMAGE, CAIRO_CONTENT_COLOR_ALPHA,
	      create_image_surface, cairo_surface_write_to_png, NULL},
	    { "image", CAIRO_SURFACE_TYPE_IMAGE, CAIRO_CONTENT_COLOR,
	      create_image_surface, cairo_surface_write_to_png, NULL},
#ifdef CAIRO_HAS_TEST_SURFACES
	    { "test-fallback", CAIRO_INTERNAL_SURFACE_TYPE_TEST_FALLBACK,
	      CAIRO_CONTENT_COLOR_ALPHA,
	      create_test_fallback_surface, cairo_surface_write_to_png, NULL },
	    { "test-fallback", CAIRO_INTERNAL_SURFACE_TYPE_TEST_FALLBACK,
	      CAIRO_CONTENT_COLOR,
	      create_test_fallback_surface, cairo_surface_write_to_png, NULL },
	    { "test-meta", CAIRO_INTERNAL_SURFACE_TYPE_TEST_META,
	      CAIRO_CONTENT_COLOR_ALPHA,
	      create_test_meta_surface, cairo_surface_write_to_png, NULL },
	    { "test-meta", CAIRO_INTERNAL_SURFACE_TYPE_TEST_META,
	      CAIRO_CONTENT_COLOR,
	      create_test_meta_surface, cairo_surface_write_to_png, NULL },
	    { "test-paginated", CAIRO_INTERNAL_SURFACE_TYPE_TEST_PAGINATED,
	      CAIRO_CONTENT_COLOR_ALPHA,
	      create_test_paginated_surface,
	      test_paginated_write_to_png,
	      cleanup_test_paginated },
	    { "test-paginated", CAIRO_INTERNAL_SURFACE_TYPE_TEST_PAGINATED,
	      CAIRO_CONTENT_COLOR,
	      create_test_paginated_surface,
	      test_paginated_write_to_png,
	      cleanup_test_paginated },
#endif
#ifdef CAIRO_HAS_GLITZ_SURFACE
#if CAIRO_CAN_TEST_GLITZ_GLX_SURFACE
	    { "glitz-glx", CAIRO_SURFACE_TYPE_GLITZ,CAIRO_CONTENT_COLOR_ALPHA,
		create_cairo_glitz_glx_surface, cairo_surface_write_to_png,
		cleanup_cairo_glitz_glx },
	    { "glitz-glx", CAIRO_SURFACE_TYPE_GLITZ, CAIRO_CONTENT_COLOR,
		create_cairo_glitz_glx_surface, cairo_surface_write_to_png,
		cleanup_cairo_glitz_glx },
#endif
#if CAIRO_CAN_TEST_GLITZ_AGL_SURFACE
	    { "glitz-agl", CAIRO_SURFACE_TYPE_GLITZ, CAIRO_CONTENT_COLOR_ALPHA,
		create_cairo_glitz_agl_surface, cairo_surface_write_to_png,
		cleanup_cairo_glitz_agl },
	    { "glitz-agl", CAIRO_SURFACE_TYPE_GLITZ, CAIRO_CONTENT_COLOR,
		create_cairo_glitz_agl_surface, cairo_surface_write_to_png,
		cleanup_cairo_glitz_agl },
#endif
#if CAIRO_CAN_TEST_GLITZ_WGL_SURFACE
	    { "glitz-wgl", CAIRO_SURFACE_TYPE_GLITZ, CAIRO_CONTENT_COLOR_ALPHA,
		create_cairo_glitz_wgl_surface, cairo_surface_write_to_png,
		cleanup_cairo_glitz_wgl },
	    { "glitz-wgl", CAIRO_SURFACE_TYPE_GLITZ, CAIRO_CONTENT_COLOR,
		create_cairo_glitz_wgl_surface, cairo_surface_write_to_png,
		cleanup_cairo_glitz_wgl },
#endif
#endif /* CAIRO_HAS_GLITZ_SURFACE */
#if 0 && CAIRO_HAS_QUARTZ_SURFACE
	    { "quartz", CAIRO_SURFACE_TYPE_QUARTZ, CAIRO_CONTENT_COLOR,
		create_quartz_surface, cairo_surface_write_to_png,
		cleanup_quartz },
#endif
#if CAIRO_HAS_WIN32_SURFACE
	    { "win32", CAIRO_SURFACE_TYPE_WIN32, CAIRO_CONTENT_COLOR,
		create_win32_surface, cairo_surface_write_to_png, cleanup_win32 },
#endif
#if CAIRO_HAS_XCB_SURFACE
	    { "xcb", CAIRO_SURFACE_TYPE_XCB, CAIRO_CONTENT_COLOR_ALPHA,
		create_xcb_surface, cairo_surface_write_to_png, cleanup_xcb},
#endif
#if CAIRO_HAS_XLIB_SURFACE
	    { "xlib", CAIRO_SURFACE_TYPE_XLIB, CAIRO_CONTENT_COLOR_ALPHA,
		create_xlib_surface, cairo_surface_write_to_png, cleanup_xlib},
	    { "xlib", CAIRO_SURFACE_TYPE_XLIB, CAIRO_CONTENT_COLOR,
		create_xlib_surface, cairo_surface_write_to_png, cleanup_xlib},
#endif
#if CAIRO_HAS_PS_SURFACE
	    { "ps", CAIRO_SURFACE_TYPE_PS,
	        CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED,
		create_ps_surface, ps_surface_write_to_png, cleanup_ps },

	    /* XXX: We expect type image here only due to a limitation in
	     * the current PS/meta-surface code. A PS surface is
	     * "naturally" COLOR_ALPHA, so the COLOR-only variant goes
	     * through create_similar in create_ps_surface which results
	     * in the similar surface being used as a source. We do not yet
	     * have source support for PS/meta-surfaces, so the
	     * create_similar path for all paginated surfaces currently
	     * returns an image surface.*/
	    { "ps", CAIRO_SURFACE_TYPE_IMAGE, CAIRO_CONTENT_COLOR,
		create_ps_surface, ps_surface_write_to_png, cleanup_ps },
#endif
#if CAIRO_HAS_PDF_SURFACE && CAIRO_CAN_TEST_PDF_SURFACE
	    { "pdf", CAIRO_SURFACE_TYPE_PDF,
	        CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED,
		create_pdf_surface, pdf_surface_write_to_png, cleanup_pdf },

	    /* XXX: We expect type image here only due to a limitation in
	     * the current PDF/meta-surface code. A PDF surface is
	     * "naturally" COLOR_ALPHA, so the COLOR-only variant goes
	     * through create_similar in create_pdf_surface which results
	     * in the similar surface being used as a source. We do not yet
	     * have source support for PDF/meta-surfaces, so the
	     * create_similar path for all paginated surfaces currently
	     * returns an image surface.*/
	    { "pdf", CAIRO_SURFACE_TYPE_IMAGE, CAIRO_CONTENT_COLOR,
		create_pdf_surface, pdf_surface_write_to_png, cleanup_pdf },
#endif
#if CAIRO_HAS_SVG_SURFACE && CAIRO_CAN_TEST_SVG_SURFACE
	    { "svg", CAIRO_SURFACE_TYPE_SVG, CAIRO_CONTENT_COLOR_ALPHA,
		    create_svg_surface, svg_surface_write_to_png, cleanup_svg },
	    { "svg", CAIRO_INTERNAL_SURFACE_TYPE_META, CAIRO_CONTENT_COLOR,
		    create_svg_surface, svg_surface_write_to_png, cleanup_svg },
#endif
#if CAIRO_HAS_BEOS_SURFACE
	    { "beos", CAIRO_SURFACE_TYPE_BEOS, CAIRO_CONTENT_COLOR,
		create_beos_surface, cairo_surface_write_to_png, cleanup_beos},
	    { "beos-bitmap", CAIRO_SURFACE_TYPE_BEOS, CAIRO_CONTENT_COLOR,
		create_beos_bitmap_surface, cairo_surface_write_to_png, cleanup_beos_bitmap},
	    { "beos-bitmap", CAIRO_SURFACE_TYPE_BEOS, CAIRO_CONTENT_COLOR_ALPHA,
		create_beos_bitmap_surface, cairo_surface_write_to_png, cleanup_beos_bitmap},
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
	    { "directfb", CAIRO_SURFACE_TYPE_DIRECTFB, CAIRO_CONTENT_COLOR,
		create_directfb_surface, cairo_surface_write_to_png, cleanup_directfb},
	    { "directfb-bitmap", CAIRO_SURFACE_TYPE_DIRECTFB, CAIRO_CONTENT_COLOR_ALPHA,
		create_directfb_bitmap_surface, cairo_surface_write_to_png,cleanup_directfb},
#endif
	};

#ifdef HAVE_UNISTD_H
    if (isatty (2)) {
	fail_face = "\033[41m\033[37m\033[1m";
	normal_face = "\033[m";
	if (isatty (1))
	    print_fail_on_stdout = FALSE;
    }
#endif

    srcdir = getenv ("srcdir");
    if (!srcdir)
	srcdir = ".";

    cairo_test_init (test->name);
    printf ("%s\n", test->description);

    if (expectation == CAIRO_TEST_FAILURE)
    printf ("Expecting failure\n");


    if ((tname = getenv ("CAIRO_TEST_TARGET")) != NULL && *tname) {

	limited_targets = TRUE;

	num_targets = 0;
	targets_to_test = NULL;

	while (*tname) {
	    int found = 0;
	    const char *end = strpbrk (tname, " \t\r\n;:,");
	    if (!end)
	        end = tname + strlen (tname);

	    for (i = 0; i < sizeof(targets)/sizeof(targets[0]); i++) {
		if (0 == strncmp (targets[i].name, tname, end - tname) &&
		    !isalnum (targets[i].name[end - tname])) {
		    /* realloc isn't exactly the best thing here, but meh. */
		    targets_to_test = realloc (targets_to_test, sizeof(cairo_test_target_t *) * (num_targets+1));
		    targets_to_test[num_targets++] = &targets[i];
		    found = 1;
		}
	    }

	    if (!found) {
		fprintf (stderr, "Cannot test target '%.*s'\n", (int)(end - tname), tname);
		exit(-1);
	    }

	    if (*end)
	      end++;
	    tname = end;
	}
    } else {
	num_targets = sizeof(targets)/sizeof(targets[0]);
	targets_to_test = malloc (sizeof(cairo_test_target_t*) * num_targets);
	for (i = 0; i < num_targets; i++)
	    targets_to_test[i] = &targets[i];
    }

    /* The intended logic here is that we return overall SUCCESS
     * iff. there is at least one tested backend and that all tested
     * backends return SUCCESS, OR, there's no backend to test at all.
     * In other words:
     *
     *  if      no backend to test
     *          -> SUCCESS
     *	else if any backend not SUCCESS
     *		-> FAILURE
     *	else if all backends UNTESTED
     *		-> FAILURE
     *	else    (== some backend SUCCESS)
     *		-> SUCCESS
     */
    ret = CAIRO_TEST_UNTESTED;
    for (i = 0; i < num_targets; i++) {
	for (j = 0; j < NUM_DEVICE_OFFSETS; j++) {
	    cairo_test_target_t * volatile target = targets_to_test[i];
	    volatile int dev_offset = j * 25;

	    cairo_test_log ("Testing %s with %s target (dev offset %d)\n", test->name, target->name, dev_offset);
	    printf ("%s-%s-%s [%d]:\t", test->name, target->name,
		    _cairo_test_content_name (target->content),
		    dev_offset);

#ifdef HAVE_SIGNAL_H
	    /* Set up a checkpoint to get back to in case of segfaults. */
	    old_segfault_handler = signal (SIGSEGV, segfault_handler);
	    if (0 == setjmp (jmpbuf))
#endif
		status = cairo_test_for_target (test, target, dev_offset);
#ifdef HAVE_SIGNAL_H
	    else
	        status = CAIRO_TEST_CRASHED;
	    signal (SIGSEGV, old_segfault_handler);
#endif

	    cairo_test_log ("TEST: %s TARGET: %s FORMAT: %s OFFSET: %d RESULT: ",
			    test->name, target->name,
			    _cairo_test_content_name (target->content),
			    dev_offset);

	    switch (status) {
	    case CAIRO_TEST_SUCCESS:
		printf ("PASS\n");
		cairo_test_log ("PASS\n");
		if (ret == CAIRO_TEST_UNTESTED)
		    ret = CAIRO_TEST_SUCCESS;
		break;
	    case CAIRO_TEST_UNTESTED:
		printf ("UNTESTED\n");
		cairo_test_log ("UNTESTED\n");
		break;
	    case CAIRO_TEST_CRASHED:
		if (print_fail_on_stdout) {
		    printf ("!!!CRASHED!!!\n");
		} else {
		    /* eat the test name */
		    printf ("\r");
		    fflush (stdout);
		}
		cairo_test_log ("CRASHED\n");
		fprintf (stderr, "%s-%s-%s [%d]:\t%s!!!CRASHED!!!%s\n",
			 test->name, target->name,
			 _cairo_test_content_name (target->content), dev_offset,
			 fail_face, normal_face);
		ret = CAIRO_TEST_FAILURE;
		break;
	    default:
	    case CAIRO_TEST_FAILURE:
		if (expectation == CAIRO_TEST_FAILURE) {
		    printf ("XFAIL\n");
		    cairo_test_log ("XFAIL\n");
		} else {
		    if (print_fail_on_stdout) {
			printf ("FAIL\n");
		    } else {
			/* eat the test name */
			printf ("\r");
			fflush (stdout);
		    }
		    fprintf (stderr, "%s-%s-%s [%d]:\t%sFAIL%s\n",
			     test->name, target->name,
			     _cairo_test_content_name (target->content), dev_offset,
			     fail_face, normal_face);
		    cairo_test_log ("FAIL\n");
		}
		ret = status;
		break;
	    }
	}
    }

    if (ret != CAIRO_TEST_SUCCESS)
        printf ("Check %s%s out for more information.\n", test->name, CAIRO_TEST_LOG_SUFFIX);

    /* if no target was requested for test, succeed, otherwise if all
     * were untested, fail. */
    if (ret == CAIRO_TEST_UNTESTED)
	ret = num_targets ? CAIRO_TEST_FAILURE : CAIRO_TEST_SUCCESS;

    /* if targets are limited using CAIRO_TEST_TARGET, and expecting failure,
     * make it fail, such that we can pass test suite by limiting backends
     * to test without triggering XPASS failures. */
    if (limited_targets && expectation == CAIRO_TEST_FAILURE && ret == CAIRO_TEST_SUCCESS) {
	printf ("All tested backends passed, but tested targets are manually limited\n"
		"and the test suite expects this test to fail for at least one target.\n"
		"Intentionally failing the test, to not fail the suite.\n");
	ret = CAIRO_TEST_FAILURE;
    }

    fclose (cairo_test_log_file);

    free (targets_to_test);

#if HAVE_FCFINI
    FcFini ();
#endif

    return ret;
}

cairo_test_status_t
cairo_test (cairo_test_t *test)
{
    cairo_test_status_t expectation = CAIRO_TEST_SUCCESS;
    const char *xfails;

    if ((xfails = getenv ("CAIRO_XFAIL_TESTS")) != NULL) {
	while (*xfails) {
	    const char *end = strpbrk (xfails, " \t\r\n;:,");
	    if (!end)
	        end = xfails + strlen (xfails);

	    if (0 == strncmp (test->name, xfails, end - xfails) &&
		'\0' == test->name[end - xfails]) {
		expectation = CAIRO_TEST_FAILURE;
		break;
	    }

	    if (*end)
	      end++;
	    xfails = end;
	}
    }

    return cairo_test_expecting (test, expectation);
}

cairo_surface_t *
cairo_test_create_surface_from_png (const char *filename)
{
    cairo_surface_t *image;
    char *srcdir = getenv ("srcdir");

    image = cairo_image_surface_create_from_png (filename);
    if (cairo_surface_status(image)) {
        /* expect not found when running with srcdir != builddir
         * such as when 'make distcheck' is run
         */
	if (srcdir) {
	    char *srcdir_filename;
	    xasprintf (&srcdir_filename, "%s/%s", srcdir, filename);
	    image = cairo_image_surface_create_from_png (srcdir_filename);
	    free (srcdir_filename);
	}
	if (cairo_surface_status(image))
	    return NULL;
    }

    return image;
}

cairo_pattern_t *
cairo_test_create_pattern_from_png (const char *filename)
{
    cairo_surface_t *image;
    cairo_pattern_t *pattern;

    image = cairo_test_create_surface_from_png (filename);

    pattern = cairo_pattern_create_for_surface (image);

    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

    cairo_surface_destroy (image);

    return pattern;
}

static cairo_status_t
_draw_check (cairo_surface_t *surface, int width, int height)
{
    cairo_t *cr;
    cairo_status_t status;

    cr = cairo_create (surface);
    cairo_set_source_rgb (cr, 0.75, 0.75, 0.75); /* light gray */
    cairo_paint (cr);

    cairo_set_source_rgb (cr, 0.25, 0.25, 0.25); /* dark gray */
    cairo_rectangle (cr, width / 2,  0, width / 2, height / 2);
    cairo_rectangle (cr, 0, height / 2, width / 2, height / 2);
    cairo_fill (cr);

    status = cairo_status (cr);

    cairo_destroy (cr);

    return status;
}

cairo_status_t
cairo_test_paint_checkered (cairo_t *cr)
{
    cairo_status_t status;
    cairo_surface_t *check;

    check = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 12, 12);
    status = _draw_check (check, 12, 12);
    if (status)
	return status;

    cairo_save (cr);
    cairo_set_source_surface (cr, check, 0, 0);
    cairo_pattern_set_filter (cairo_get_source (cr), CAIRO_FILTER_NEAREST);
    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_REPEAT);
    cairo_paint (cr);
    cairo_restore (cr);

    cairo_surface_destroy (check);

    return CAIRO_STATUS_SUCCESS;
}
