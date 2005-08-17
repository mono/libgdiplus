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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <fontconfig/fontconfig.h>

#include "cairo-test.h"

#include "buffer-diff.h"
#include "read-png.h"
#include "write-png.h"
#include "xmalloc.h"

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#endif

#define CAIRO_TEST_LOG_SUFFIX ".log"
#define CAIRO_TEST_PNG_SUFFIX "-out.png"
#define CAIRO_TEST_REF_SUFFIX "-ref.png"
#define CAIRO_TEST_DIFF_SUFFIX "-diff.png"

/* Static data is messy, but we're coding for tests here, not a
 * general-purpose library, and it keeps the tests cleaner to avoid a
 * context object there, (though not a whole lot). */
FILE *cairo_test_log_file;

void
cairo_test_log (const char *fmt, ...)
{
    va_list va;

    va_start (va, fmt);
    vfprintf (cairo_test_log_file, fmt, va);
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
	cairo_test_log ("  Error: Cannot remove %s: %s\n",
			pathname, strerror (errno));
	exit (1);
    }
}

typedef cairo_surface_t *
(*cairo_test_create_target_surface_t) (int width, int height, void **closure);

typedef void
(*cairo_test_cleanup_target_t) (void *closure);

typedef struct _cairo_test_target
{
    const char		       	       *name;
    cairo_test_create_target_surface_t	create_target_surface;
    cairo_test_cleanup_target_t		cleanup_target;
    void		       	       *closure;
} cairo_test_target_t;

static cairo_surface_t *
create_image_surface (int width, int height, void **closure)
{
    int stride = 4 * width;
    unsigned char *buf;

    *closure = buf = xcalloc (stride * height, 1);

    return cairo_image_surface_create_for_data (buf,
						CAIRO_FORMAT_ARGB32,
						width, height, stride);
}

static void
cleanup_image (void *closure)
{
    unsigned char *buf = closure;

    free (buf);
}

/* XXX: Someone who knows glitz better than I do should fix this up to
 * work. */
#if 0 /* #ifdef CAIRO_HAS_GLITZ_SURFACE */
static cairo_surface_t *
create_glitz_surface (int width, int height, void **closure)
{
#error Not yet implemented
}

static void
cleanup_glitz (cairo_t *cr)
{
#error Not yet implemented
}
#endif

#if CAIRO_HAS_QUARTZ_SURFACE
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
#if 0 && CAIRO_HAS_WIN32_SURFACE
static cairo_surface_t *
create_win32_surface (int width, int height, void **closure)
{
#error Not yet implemented
}

static void
cleanup_win32 (void *closure)
{
#error Not yet implemented
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
create_xcb_surface (int width, int height, void **closure)
{
    XCBSCREEN *root;
    xcb_target_closure_t *xtc;
    cairo_surface_t *surface;
    XCBConnection *c;
    XCBRenderPICTFORMINFO render_format;

    *closure = xtc = xmalloc (sizeof (xcb_target_closure_t));

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

    xtc->c = c = XCBConnectBasic();
    if (c == NULL) {
	cairo_test_log ("Failed to connect to X server through XCB\n");
	return NULL;
    }

    root = XCBConnSetupSuccessRepRootsIter(XCBGetSetup(c)).data;

    xtc->drawable.pixmap = XCBPIXMAPNew (c);
    {
	XCBDRAWABLE root_drawable;
	root_drawable.window = root->root;
	XCBCreatePixmap (c, 32, xtc->drawable.pixmap, root_drawable,
			 width, height);
    }

    render_format = _format_from_cairo (c, CAIRO_FORMAT_ARGB32);
    if (render_format.id.xid == 0)
	return NULL;
    surface = cairo_xcb_surface_create_with_xrender_format (c, xtc->drawable,
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
create_xlib_surface (int width, int height, void **closure)
{
    xlib_target_closure_t *xtc;
    cairo_surface_t *surface;
    Display *dpy;
    XRenderPictFormat *xrender_format;

    *closure = xtc = xmalloc (sizeof (xlib_target_closure_t));

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

    xtc->dpy = dpy = XOpenDisplay (0);
    if (xtc->dpy == NULL) {
	cairo_test_log ("Failed to open display: %s\n", XDisplayName(0));
	return NULL;
    }

    xrender_format = XRenderFindStandardFormat (dpy, PictStandardARGB32);
    
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

static cairo_test_status_t
cairo_test_for_target (cairo_test_t *test,
		       cairo_test_draw_function_t draw,
		       cairo_test_target_t	 *target)
{
    cairo_test_status_t status;
    cairo_surface_t *surface;
    cairo_t *cr;
    char *png_name, *ref_name, *diff_name;
    char *srcdir;
    cairo_test_status_t ret;

    /* Get the strings ready that we'll need. */
    srcdir = getenv ("srcdir");
    if (!srcdir)
	srcdir = ".";
    xasprintf (&png_name, "%s-%s%s", test->name,
	       target->name, CAIRO_TEST_PNG_SUFFIX);
    xasprintf (&ref_name, "%s/%s%s", srcdir, test->name,
	       CAIRO_TEST_REF_SUFFIX);
    xasprintf (&diff_name, "%s-%s%s", test->name,
	       target->name, CAIRO_TEST_DIFF_SUFFIX);

    /* Run the actual drawing code. */
    surface = (target->create_target_surface) (test->width, test->height,
					       &target->closure);
    if (surface == NULL) {
	cairo_test_log ("Error: Failed to set %s target\n", target->name);
	ret = CAIRO_TEST_UNTESTED;
	goto UNWIND_STRINGS;
    }

    cr = cairo_create (surface);

    cairo_save (cr);
    cairo_set_source_rgba (cr, 0, 0, 0, 0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);
    cairo_restore (cr);

    status = (draw) (cr, test->width, test->height);

    /* Then, check all the different ways it could fail. */
    if (status) {
	cairo_test_log ("Error: Function under test failed\n");
	ret = status;
	goto UNWIND_CAIRO;
    }

    if (cairo_status (cr) != CAIRO_STATUS_SUCCESS) {
	cairo_test_log ("Error: Function under test left cairo status in an error state: %s\n",
			cairo_status_to_string (cairo_status (cr)));
	ret = CAIRO_TEST_FAILURE;
	goto UNWIND_CAIRO;
    }

    /* Skip image check for tests with no image (width,height == 0,0) */
    if (test->width != 0 && test->height != 0) {
	int pixels_changed;
	cairo_surface_write_to_png (surface, png_name);
	pixels_changed = image_diff (png_name, ref_name, diff_name);
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
    cairo_surface_destroy (surface);

    cairo_debug_reset_static_data ();

    target->cleanup_target (target->closure);

UNWIND_STRINGS:
    free (png_name);
    free (ref_name);
    free (diff_name);

    return ret;
}

static cairo_test_status_t
cairo_test_expecting (cairo_test_t *test, cairo_test_draw_function_t draw,
		      cairo_test_status_t expectation)
{
    int i;
    cairo_test_status_t status, ret;
    cairo_test_target_t targets[] = 
	{
	    { "image", create_image_surface, cleanup_image}, 
#if 0 /* #ifdef CAIRO_HAS_GLITZ_SURFACE */
	    { "glitz", create_glitz_surface, cleanup_glitz}, 
#endif
#if CAIRO_HAS_QUARTZ_SURFACE
	    { "quartz", create_quartz_surface, cleanup_quartz},
#endif
#if 0 && CAIRO_HAS_WIN32_SURFACE
	    { "win32", create_win32_surface, cleanup_win32},
#endif
#if CAIRO_HAS_XCB_SURFACE
	    { "xcb", create_xcb_surface, cleanup_xcb},
#endif
#if CAIRO_HAS_XLIB_SURFACE
	    { "xlib", create_xlib_surface, cleanup_xlib},
#endif
	};
    char *log_name;

    xasprintf (&log_name, "%s%s", test->name, CAIRO_TEST_LOG_SUFFIX);
    xunlink (log_name);

    cairo_test_log_file = fopen (log_name, "a");
    if (cairo_test_log_file == NULL) {
	fprintf (stderr, "Error opening log file: %s\n", log_name);
	cairo_test_log_file = stderr;
    }
    free (log_name);

    /* The intended logic here is that we return overall SUCCESS
     * iff. there is at least one tested backend and that all tested
     * backends return SUCCESS. In other words:
     *
     *	if      any backend FAILURE
     *		-> FAILURE
     *	else if all backends UNTESTED
     *		-> FAILURE
     *	else    (== some backend SUCCESS)
     *		-> SUCCESS
     */
    ret = CAIRO_TEST_UNTESTED;
    for (i=0; i < sizeof(targets)/sizeof(targets[0]); i++) {
	cairo_test_target_t *target = &targets[i];
	cairo_test_log ("Testing %s with %s target\n", test->name, target->name);
	printf ("%s-%s:\t", test->name, target->name);
	status = cairo_test_for_target (test, draw, target);
	switch (status) {
	case CAIRO_TEST_SUCCESS:
	    printf ("PASS\n");
	    if (ret == CAIRO_TEST_UNTESTED)
		ret = CAIRO_TEST_SUCCESS;
	    break;
	case CAIRO_TEST_UNTESTED:
	    printf ("UNTESTED\n");
	    break;
	default:
	case CAIRO_TEST_FAILURE:
	    if (expectation == CAIRO_TEST_FAILURE)
		printf ("XFAIL\n");
	    else
		printf ("FAIL\n");
	    ret = status;
	    break;
	}
    }
    if (ret == CAIRO_TEST_UNTESTED)
	ret = CAIRO_TEST_FAILURE;

    fclose (cairo_test_log_file);

#if HAVE_FCFINI
    FcFini ();
#endif

    return ret;
}

cairo_test_status_t
cairo_test_expect_failure (cairo_test_t		      *test, 
			   cairo_test_draw_function_t  draw,
			   const char		      *because)
{
    printf ("\n%s is expected to fail:\n\t%s\n", test->name, because);
    return cairo_test_expecting (test, draw, CAIRO_TEST_FAILURE);
}

cairo_test_status_t
cairo_test (cairo_test_t *test, cairo_test_draw_function_t draw)
{
    printf ("\n");
    return cairo_test_expecting (test, draw, CAIRO_TEST_SUCCESS);
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
