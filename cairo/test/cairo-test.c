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
#include "xmalloc.h"

#ifdef _MSC_VER
#include <crtdbg.h>
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

    /* Nor do we care about rendering anomalies in external renderers. */
    "fill-degenerate-sort-order",
    NULL
};

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
cairo_test_fini (void)
{
    fclose (cairo_test_log_file);
    cairo_debug_reset_static_data ();
#if HAVE_FCFINI
    FcFini ();
#endif
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

static void
xunlink (const char *pathname)
{
    if (unlink (pathname) < 0 && errno != ENOENT) {
	cairo_test_log ("Error: Cannot remove %s: %s\n",
			pathname, strerror (errno));
	exit (1);
    }
}

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
cairo_test_for_target (cairo_test_t			 *test,
		       cairo_boilerplate_target_t	 *target,
		       int				  dev_offset)
{
    cairo_test_status_t status;
    cairo_surface_t *surface = NULL;
    cairo_t *cr;
    char *png_name, *ref_name, *diff_name, *offset_str;
    cairo_test_status_t ret = CAIRO_TEST_SUCCESS;
    cairo_content_t expected_content;
    cairo_font_options_t *font_options;
    const char *format;

    /* Get the strings ready that we'll need. */
    format = cairo_boilerplate_content_name (target->content);
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

    if (target->is_vector) {
	int i;

	for (i = 0; vector_ignored_tests[i] != NULL; i++)
	    if (strcmp (test->name, vector_ignored_tests[i]) == 0) {
		cairo_test_log ("Error: Skipping for vector target %s\n", target->name);
		ret = CAIRO_TEST_UNTESTED;
		goto UNWIND_STRINGS;
	    }
    }

    if (ret == CAIRO_TEST_SUCCESS) {
	/* Run the actual drawing code. */

	if (test->width && test->height) {
	    test->width += dev_offset;
	    test->height += dev_offset;
	}

	surface = (target->create_surface) (test->name,
					    target->content,
					    test->width,
					    test->height,
					    CAIRO_BOILERPLATE_MODE_TEST,
					    &target->closure);

	if (test->width && test->height) {
	    test->width -= dev_offset;
	    test->height -= dev_offset;;
	}
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

    if (cairo_status (cr) != CAIRO_STATUS_SUCCESS) {
	cairo_test_log ("Error: Function under test left cairo status in an error state: %s\n",
			cairo_status_to_string (cairo_status (cr)));
	ret = CAIRO_TEST_FAILURE;
	goto UNWIND_CAIRO;
    }

    /* Skip image check for tests with no image (width,height == 0,0) */
    if (test->width != 0 && test->height != 0) {
	buffer_diff_result_t result;
	cairo_status_t diff_status;
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

	cairo_test_log ("Comparing result against reference image: %s\n", ref_name);

	if (target->content == CAIRO_TEST_CONTENT_COLOR_ALPHA_FLATTENED) {
	    diff_status= image_diff_flattened (png_name, ref_name, diff_name,
					       dev_offset, dev_offset, 0, 0, &result);
	} else {
	    diff_status = image_diff (png_name, ref_name, diff_name,
				      dev_offset, dev_offset, 0, 0, &result);
	}
	if (diff_status) {
	    cairo_test_log ("Error: Failed to compare images: %s\n",
			    cairo_status_to_string (diff_status));
	    ret = CAIRO_TEST_FAILURE;
	    goto UNWIND_CAIRO;
	}
	if (result.pixels_changed && result.max_diff > target->error_tolerance) {
	    ret = CAIRO_TEST_FAILURE;
	    goto UNWIND_CAIRO;
	}
    }

UNWIND_CAIRO:
    cairo_destroy (cr);
UNWIND_SURFACE:
    cairo_surface_destroy (surface);

    cairo_debug_reset_static_data ();

    if (target->cleanup)
	target->cleanup (target->closure);

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
#ifdef HAVE_SIGNAL_H
    void (*old_segfault_handler)(int);
#endif
    volatile cairo_test_status_t status, ret;
    cairo_boilerplate_target_t ** volatile targets_to_test;

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

    {
	int tmp_num_targets;
	cairo_bool_t tmp_limited_targets;
	targets_to_test = cairo_boilerplate_get_targets (&tmp_num_targets, &tmp_limited_targets);
	num_targets = tmp_num_targets;
	limited_targets = tmp_limited_targets;
    }

    /* The intended logic here is that we return overall SUCCESS
     * iff. there is at least one tested backend and that all tested
     * backends return SUCCESS, OR, there's backends were manually
     * limited, and none were tested.
     * In other words:
     *
     *  if      backends limited and no backend tested
     *          -> SUCCESS
     *	else if any backend not SUCCESS
     *		-> FAILURE
     *	else if all backends UNTESTED
     *		-> FAILURE
     *	else    (== some backend SUCCESS)
     *		-> SUCCESS
     *
     * Also, on a crash, run no further tests.
     */
    status = ret = CAIRO_TEST_UNTESTED;
    for (i = 0; i < num_targets && status != CAIRO_TEST_CRASHED; i++) {
	for (j = 0; j < NUM_DEVICE_OFFSETS; j++) {
	    cairo_boilerplate_target_t * volatile target = targets_to_test[i];
	    volatile int dev_offset = j * 25;

	    cairo_test_log ("Testing %s with %s target (dev offset %d)\n", test->name, target->name, dev_offset);
	    printf ("%s-%s-%s [%d]:\t", test->name, target->name,
		    cairo_boilerplate_content_name (target->content),
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
			    cairo_boilerplate_content_name (target->content),
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
			 cairo_boilerplate_content_name (target->content), dev_offset,
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
			     cairo_boilerplate_content_name (target->content), dev_offset,
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

    /* if the set of targets to test was limited using CAIRO_TEST_TARGET, we
     * behave slightly differently, to ensure that limiting the targets does
     * not increase the number of tests failing. */
    if (limited_targets) {

	/* if all untested, success */
	if (ret == CAIRO_TEST_UNTESTED) {
	    printf ("None of the tested backends passed, but tested targets are manually limited.\n"
		    "Passing the test, to not fail the suite.\n");
	    ret = CAIRO_TEST_SUCCESS;
	}

	/* if all passed, but expecting failure, return failure to not
	 * trigger an XPASS failure */
	if (expectation == CAIRO_TEST_FAILURE && ret == CAIRO_TEST_SUCCESS) {
	    printf ("All tested backends passed, but tested targets are manually limited\n"
		    "and the test suite expects this test to fail for at least one target.\n"
		    "Intentionally failing the test, to not fail the suite.\n");
	    ret = CAIRO_TEST_FAILURE;
	}

    } else {

	if (ret == CAIRO_TEST_UNTESTED)
	    ret = CAIRO_TEST_FAILURE;

    }

    cairo_test_fini ();

    cairo_boilerplate_free_targets (targets_to_test);

    return ret;
}

cairo_test_status_t
cairo_test (cairo_test_t *test)
{
    cairo_test_status_t expectation = CAIRO_TEST_SUCCESS;
    const char *xfails;

#ifdef _MSC_VER
    /* We don't want an assert dialog, we want stderr */
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

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
    if (status) {
	cairo_surface_destroy (check);
	return status;
    }

    cairo_save (cr);
    cairo_set_source_surface (cr, check, 0, 0);
    cairo_pattern_set_filter (cairo_get_source (cr), CAIRO_FILTER_NEAREST);
    cairo_pattern_set_extend (cairo_get_source (cr), CAIRO_EXTEND_REPEAT);
    cairo_paint (cr);
    cairo_restore (cr);

    cairo_surface_destroy (check);

    return CAIRO_STATUS_SUCCESS;
}
