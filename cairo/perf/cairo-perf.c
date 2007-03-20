/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 8; -*- */
/*
 * Copyright © 2006 Mozilla Corporation
 * Copyright © 2006 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * the authors not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. The authors make no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors: Vladimir Vukicevic <vladimir@pobox.com>
 *          Carl Worth <cworth@cworth.org>
 */

#include "cairo-perf.h"
#include "config.h"

/* For getopt */
#include <unistd.h>
/* For basename */
#include <libgen.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#define CAIRO_PERF_ITERATIONS_DEFAULT	100
#define CAIRO_PERF_LOW_STD_DEV		0.03
#define CAIRO_PERF_STABLE_STD_DEV_COUNT	5

typedef struct _cairo_perf_case {
    CAIRO_PERF_DECL (*run);
    unsigned int min_size;
    unsigned int max_size;
} cairo_perf_case_t;

cairo_perf_case_t perf_cases[];

/* Some targets just aren't that interesting for performance testing,
 * (not least because many of these surface types use a meta-surface
 * and as such defer the "real" rendering to later, so our timing
 * loops wouldn't count the real work, just the recording by the
 * meta-surface. */
static cairo_bool_t
target_is_measurable (cairo_boilerplate_target_t *target)
{
    switch (target->expected_type) {
    case CAIRO_SURFACE_TYPE_IMAGE:
	if (strcmp (target->name, "pdf") == 0 ||
	    strcmp (target->name, "ps") == 0)
	{
	    return FALSE;
	}
	else
	{
	    return TRUE;
	}
    case CAIRO_SURFACE_TYPE_XLIB:
    case CAIRO_SURFACE_TYPE_XCB:
    case CAIRO_SURFACE_TYPE_GLITZ:
    case CAIRO_SURFACE_TYPE_QUARTZ:
    case CAIRO_SURFACE_TYPE_WIN32:
    case CAIRO_SURFACE_TYPE_BEOS:
    case CAIRO_SURFACE_TYPE_DIRECTFB:
#if CAIRO_VERSION_MAJOR > 1 || (CAIRO_VERSION_MAJOR == 1 && CAIRO_VERSION_MINOR > 2)
    case CAIRO_SURFACE_TYPE_OS2:
#endif
	return TRUE;
    case CAIRO_SURFACE_TYPE_PDF:
    case CAIRO_SURFACE_TYPE_PS:
    case CAIRO_SURFACE_TYPE_SVG:
    default:
	return FALSE;
    }
}

static const char *
_content_to_string (cairo_content_t content)
{
    switch (content) {
    case CAIRO_CONTENT_COLOR:
	return "rgb";
    case CAIRO_CONTENT_ALPHA:
	return "a";
    case CAIRO_CONTENT_COLOR_ALPHA:
	return "rgba";
    default:
	return "<unknown_content>";
    }
}

void
cairo_perf_run (cairo_perf_t		*perf,
		const char		*name,
		cairo_perf_func_t	 perf_func)
{
    static cairo_bool_t first_run = TRUE;
    unsigned int i;
    cairo_perf_ticks_t *times;
    cairo_stats_t stats = {0.0, 0.0};
    int low_std_dev_count;

    if (perf->num_names) {
	for (i = 0; i < perf->num_names; i++)
	    if (strstr (name, perf->names[i]))
		goto NAME_FOUND;
	return;
    }
  NAME_FOUND:

    if (perf->list_only) {
	printf ("%s\n", name);
	return;
    }

    if (first_run) {
	if (perf->raw)
	    printf ("[ # ] %s-%-s %s %s %s ...\n",
		    "backend", "content", "test-size", "ticks-per-ms", "time(ticks)");
	else
	    printf ("[ # ] %8s-%-4s %28s %8s %8s %5s %5s %s\n",
		    "backend", "content", "test-size", "min(ticks)", "min(ms)", "median(ms)",
		    "stddev.", "iterations");
	first_run = FALSE;
    }

    times = xmalloc (perf->iterations * sizeof (cairo_perf_ticks_t));

    /* We run one iteration in advance to warm caches, etc. */
    cairo_perf_yield ();
    (perf_func) (perf->cr, perf->size, perf->size);

    low_std_dev_count = 0;
    for (i =0; i < perf->iterations; i++) {
	cairo_perf_yield ();
	times[i] = (perf_func) (perf->cr, perf->size, perf->size);

	if (perf->raw) {
	    if (i == 0)
		printf ("[*] %s-%s %s-%d %g",
			perf->target->name,
			_content_to_string (perf->target->content),
			name, perf->size,
			cairo_perf_ticks_per_second () / 1000.0);
	    printf (" %lld", times[i]);
	} else {
	    if (i > 0) {
		_cairo_stats_compute (&stats, times, i+1);

		if (stats.std_dev <= CAIRO_PERF_LOW_STD_DEV &&
		    ! perf->exact_iterations)
		{
		    low_std_dev_count++;
		    if (low_std_dev_count >= CAIRO_PERF_STABLE_STD_DEV_COUNT)
			break;
		} else {
		    low_std_dev_count = 0;
		}
	    }
	}
    }

    if (perf->raw) {
	printf ("\n");
    } else {
	printf ("[%3d] %8s-%-4s %26s-%-3d ",
		perf->test_number, perf->target->name,
		_content_to_string (perf->target->content),
		name, perf->size);

	printf ("%10lld %#8.3f %#8.3f %#5.2f%% %3d\n",
		stats.min_ticks,
		(stats.min_ticks * 1000.0) / cairo_perf_ticks_per_second (),
		(stats.median_ticks * 1000.0) / cairo_perf_ticks_per_second (),
		stats.std_dev * 100.0, stats.iterations);
    }

    perf->test_number++;
    free (times);
}

static void
usage (const char *argv0)
{
    fprintf (stderr,
	     "Usage: %s [-l] [-r] [-i iterations] [test-names ...]\n"
	     "       %s -l\n"
	     "\n"
	     "Run the cairo performance test suite over the given tests (all by default)\n"
	     "The command-line arguments are interpreted as follows:\n"
	     "\n"
	     "  -r	raw; display each time measurement instead of summary statistics\n"
	     "  -i	iterations; specify the number of iterations per test case\n"
	     "  -l	list only; just list selected test case names without executing\n"
	     "\n"
	     "If test names are given they are used as sub-string matches so a command\n"
	     "such as \"cairo-perf text\" can be used to run all text test cases.\n",
	     argv0, argv0);
}

static void
parse_options (cairo_perf_t *perf, int argc, char *argv[])
{
    int c;
    char *end;

    if (getenv("CAIRO_PERF_ITERATIONS"))
	perf->iterations = strtol(getenv("CAIRO_PERF_ITERATIONS"), NULL, 0);
    else
	perf->iterations = CAIRO_PERF_ITERATIONS_DEFAULT;
    perf->exact_iterations = 0;

    perf->raw = FALSE;
    perf->list_only = FALSE;
    perf->names = NULL;
    perf->num_names = 0;

    while (1) {
	c = getopt (argc, argv, "i:lr");
	if (c == -1)
	    break;

	switch (c) {
	case 'i':
	    perf->exact_iterations = TRUE;
	    perf->iterations = strtoul (optarg, &end, 10);
	    if (*end != '\0') {
		fprintf (stderr, "Invalid argument for -i (not an integer): %s\n",
			 optarg);
		exit (1);
	    }
	    break;
	case 'l':
	    perf->list_only = TRUE;
	    break;
	case 'r':
	    perf->raw = TRUE;
	    break;
	default:
	    fprintf (stderr, "Internal error: unhandled option: %c\n", c);
	    /* fall-through */
	case '?':
	    usage (argv[0]);
	    exit (1);
	}
    }
    if (optind < argc) {
	perf->names = &argv[optind];
	perf->num_names = argc - optind;
    }
}

static int 
check_cpu_affinity(void)
{
#ifdef HAVE_SCHED_GETAFFINITY

    cpu_set_t affinity;
    int i, cpu_count;

    if (sched_getaffinity(0, sizeof(affinity), &affinity)) {
        perror("sched_getaffinity");
        return -1;
    }

    for(i = 0, cpu_count = 0; i < CPU_SETSIZE; ++i) {
        if (CPU_ISSET(i, &affinity))
            ++cpu_count;
    }

    if (cpu_count > 1) {
        fputs(
            "WARNING: cairo-perf has not been bound to a single CPU.\n", 
            stderr);
        return -1;
    }

    return 0;
#else
    fputs(
        "WARNING: Cannot check CPU affinity for this platform.\n", 
        stderr);
    return -1;
#endif
}

int
main (int argc, char *argv[])
{
    int i, j;
    cairo_perf_case_t *perf_case;
    cairo_perf_t perf;
    const char *cairo_test_target = getenv ("CAIRO_TEST_TARGET");
    cairo_boilerplate_target_t *target;
    cairo_surface_t *surface;

    parse_options (&perf, argc, argv);

    if (check_cpu_affinity()) {
        fputs(
            "NOTICE: cairo-perf and the X server should be bound to CPUs (either the same\n"
            "or separate) on SMP systems. Not doing so causes random results when the X\n"
            "server is moved to or from cairo-perf's CPU during the benchmarks:\n"
            "\n"
            "    $ sudo taskset -cp 0 $(pidof X)\n"
            "    $ taskset -cp 1 $$\n"
            "\n"
            "See taskset(1) for information about changing CPU affinity.\n",
            stderr);
    }

    for (i = 0; targets[i].name; i++) {
	perf.target = target = &targets[i];
	perf.test_number = 0;

	if (! target_is_measurable (target))
	    continue;
	if (cairo_test_target && ! strstr (cairo_test_target, target->name))
	    continue;

	for (j = 0; perf_cases[j].run; j++) {

	    perf_case = &perf_cases[j];

	    for (perf.size = perf_case->min_size;
		 perf.size <= perf_case->max_size;
		 perf.size *= 2)
	    {
		surface = (target->create_surface) (NULL,
						    target->content,
						    perf.size, perf.size,
						    CAIRO_BOILERPLATE_MODE_PERF,
						    &target->closure);
		cairo_perf_timer_set_synchronize (target->synchronize,
						  target->closure);

		perf.cr = cairo_create (surface);

		perf_case->run (&perf, perf.cr, perf.size, perf.size);

		if (cairo_status (perf.cr)) {
		    fprintf (stderr, "Error: Test left cairo in an error state: %s\n",
			     cairo_status_to_string (cairo_status (perf.cr)));
		    exit (1);
		}

		cairo_destroy (perf.cr);
		cairo_surface_destroy (surface);

		if (target->cleanup)
		    target->cleanup (target->closure);
	    }
	}
    }

    return 0;
}

cairo_perf_case_t perf_cases[] = {
    { paint,  256, 512},
    { fill,   64, 256},
    { stroke, 64, 256},
    { text,   64, 256},
    { tessellate, 100, 100},
    { subimage_copy, 16, 512},
    { pattern_create_radial, 16, 16},
    { zrusin, 415, 415},
    { world_map, 800, 800},
    { box_outline, 100, 100},
    { mosaic, 800, 800 },
    { long_lines, 100, 100},
    { unaligned_clip, 100, 100},
    { rectangles, 512, 512},
    { NULL }
};
