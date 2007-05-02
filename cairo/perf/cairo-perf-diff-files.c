/*
 * Copyright © 2006 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the
 * copyright holders not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission. The copyright holders make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * Authors: Carl Worth <cworth@cworth.org>
 */

#include "cairo-perf.h"

/* We use _GNU_SOURCE for getline and strndup if available. */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

typedef struct _test_report {
    int id;
    const char *configuration;
    char *backend;
    char *content;
    char *name;
    int size;

    /* The samples only exists for "raw" reports */
    cairo_perf_ticks_t *samples;
    unsigned int samples_size;
    unsigned int samples_count;

    /* The stats are either read directly or computed from samples.
     * If the stats have not yet been computed from samples, then
     * iterations will be 0. */
    cairo_stats_t stats;
} test_report_t;

typedef struct _test_diff {
    test_report_t **tests;
    int num_tests;
    double min;
    double max;
    double change;
} test_diff_t;

typedef struct _cairo_perf_report {
    char *configuration;
    const char *name;
    test_report_t *tests;
    int tests_size;
    int tests_count;
} cairo_perf_report_t;

typedef enum {
    TEST_REPORT_STATUS_SUCCESS,
    TEST_REPORT_STATUS_COMMENT,
    TEST_REPORT_STATUS_ERROR
} test_report_status_t;

typedef struct _cairo_perf_report_options {
    double min_change;
    int use_utf;
    int print_change_bars;
    int use_ms;
} cairo_perf_report_options_t;

typedef struct _cairo_perf_diff_files_args {
    const char **filenames;
    int num_filenames;
    cairo_perf_report_options_t options;
} cairo_perf_diff_files_args_t;

#ifndef __USE_GNU
static ssize_t
getline (char **lineptr, size_t *n, FILE *stream);

static char *
strndup (const char *s, size_t n);
#endif

/* Ad-hoc parsing, macros with a strong dependence on the calling
 * context, and plenty of other ugliness is here.  But at least it's
 * not perl... */
#define parse_error(...) fprintf(stderr, __VA_ARGS__); return TEST_REPORT_STATUS_ERROR;
#define skip_char(c)							\
do {									\
    if (*s && *s == (c)) {						\
	s++;								\
    } else {								\
	 parse_error ("expected '%c' but found '%c'", c, *s);		\
    }									\
} while (0)
#define skip_space() while (*s && (*s == ' ' || *s == '\t')) s++;
#define parse_int(result)						\
do {									\
    (result) = strtol (s, &end, 10);					\
    if (*s && end != s) {						\
	s = end;							\
    } else {								\
	parse_error("expected integer but found %s", s);		\
    }									\
} while (0)
#define parse_long_long(result)						\
do {									\
    (result) = strtoll (s, &end, 10);					\
    if (*s && end != s) {						\
	s = end;							\
    } else {								\
	parse_error("expected integer but found %s", s);		\
    }									\
} while (0)
#define parse_double(result)						\
do {									\
    (result) = strtod (s, &end);					\
    if (*s && end != s) {						\
	s = end;							\
    } else {								\
	parse_error("expected floating-point value but found %s", s);	\
    }									\
} while (0)
/* Here a string is simply a sequence of non-whitespace */
#define parse_string(result)						\
do {									\
    for (end = s; *end; end++)						\
	if (isspace (*end))						\
	    break;							\
    (result) = strndup (s, end - s);					\
    if ((result) == NULL) {						\
	fprintf (stderr, "Out of memory.\n");				\
	exit (1);							\
    }									\
    s = end;								\
} while (0)

static test_report_status_t
test_report_parse (test_report_t *report, char *line, char *configuration)
{
    char *end;
    char *s = line;
    cairo_bool_t is_raw = FALSE;
    double min_time, median_time;

    /* The code here looks funny unless you understand that these are
     * all macro calls, (and then the code just looks sick). */
    if (*s == '\n')
	return TEST_REPORT_STATUS_COMMENT;

    skip_char ('[');
    skip_space ();
    if (*s == '#')
	return TEST_REPORT_STATUS_COMMENT;
    if (*s == '*') {
	s++;
	is_raw = TRUE;
    } else {
	parse_int (report->id);
    }
    skip_char (']');

    skip_space ();

    report->configuration = configuration;
    parse_string (report->backend);
    end = strrchr (report->backend, '-');
    if (*end)
	*end++ = '\0';
    report->content = end;

    skip_space ();

    parse_string (report->name);
    end = strrchr (report->name, '-');
    if (*end)
	*end++ = '\0';
    report->size = atoi (end);

    skip_space ();

    report->samples = NULL;
    report->samples_size = 0;
    report->samples_count = 0;

    if (is_raw) {
	parse_double (report->stats.ticks_per_ms);
	skip_space ();

	report->samples_size = 5;
	report->samples = xmalloc (report->samples_size * sizeof (cairo_perf_ticks_t));
	do {
	    if (report->samples_count == report->samples_size) {
		report->samples_size *= 2;
		report->samples = xrealloc (report->samples,
					    report->samples_size * sizeof (cairo_perf_ticks_t));
	    }
	    parse_long_long (report->samples[report->samples_count++]);
	    skip_space ();
	} while (*s && *s != '\n');
	report->stats.iterations = 0;
	skip_char ('\n');
    } else {
	parse_double (report->stats.min_ticks);
	skip_space ();

	parse_double (min_time);
	report->stats.ticks_per_ms = report->stats.min_ticks / min_time;

	skip_space ();

	parse_double (median_time);
	report->stats.median_ticks = median_time * report->stats.ticks_per_ms;

	skip_space ();

	parse_double (report->stats.std_dev);
	report->stats.std_dev /= 100.0;
	skip_char ('%');

	skip_space ();

	parse_int (report->stats.iterations);

	skip_space ();
	skip_char ('\n');
    }

    return TEST_REPORT_STATUS_SUCCESS;
}

/* We conditionally provide a custom implementation of getline and strndup
 * as needed. These aren't necessary full-fledged general purpose
 * implementations. They just get the job done for our purposes.
 */
#ifndef __USE_GNU
#define POORMANS_GETLINE_BUFFER_SIZE (65536)
static ssize_t
getline (char **lineptr, size_t *n, FILE *stream)
{
    if (!*lineptr)
    {
        *n = POORMANS_GETLINE_BUFFER_SIZE;
        *lineptr = (char *) malloc (*n);
    }

    if (!fgets (*lineptr, *n, stream))
        return -1;

    if (!feof (stream) && !strchr (*lineptr, '\n'))
    {
        fprintf (stderr, "The poor man's implementation of getline in "
                          __FILE__ " needs a bigger buffer. Perhaps it's "
                         "time for a complete implementation of getline.\n");
        exit (0);
    }

    return strlen (*lineptr);
}
#undef POORMANS_GETLINE_BUFFER_SIZE

static char *
strndup (const char *s, size_t n)
{
    size_t len;
    char *sdup;

    if (!s)
        return NULL;

    len = strlen (s);
    len = (n < len ? n : len);
    sdup = (char *) malloc (len + 1);
    if (sdup)
    {
        memcpy (sdup, s, len);
        sdup[len] = '\0';
    }

    return sdup;
}
#endif /* ifndef __USE_GNU */


static int
test_report_cmp_backend_then_name (const void *a, const void *b)
{
    const test_report_t *a_test = a;
    const test_report_t *b_test = b;

    int cmp;

    cmp = strcmp (a_test->backend, b_test->backend);
    if (cmp)
	return cmp;

    cmp = strcmp (a_test->content, b_test->content);
    if (cmp)
	return cmp;

    /* A NULL name is a list-termination marker, so force it last. */
    if (a_test->name == NULL)
	if (b_test->name == NULL)
	    return 0;
	else
	    return 1;
    else if (b_test->name == NULL)
	return -1;

    cmp = strcmp (a_test->name, b_test->name);
    if (cmp)
	return cmp;

    if (a_test->size < b_test->size)
	return -1;
    if (a_test->size > b_test->size)
	return 1;

    return 0;
}

static void
cairo_perf_report_sort_and_compute_stats (cairo_perf_report_t *report)
{
    test_report_t *base, *next, *last, *t;

    /* First we sort, since the diff needs both lists in the same
     * order */
    qsort (report->tests, report->tests_count, sizeof (test_report_t),
	   test_report_cmp_backend_then_name);

    /* The sorting also brings all related raw reports together so we
     * can condense them and compute the stats.
     */
    base = &report->tests[0];
    last = &report->tests[report->tests_count - 1];
    while (base <= last) {
	next = base+1;
	if (next <= last) {
	    while (next <= last &&
		   test_report_cmp_backend_then_name (base, next) == 0)
	    {
		next++;
	    }
	    if (next != base) {
		unsigned int new_samples_count = base->samples_count;
		for (t = base + 1; t < next; t++)
		    new_samples_count += t->samples_count;
		if (new_samples_count > base->samples_size) {
		    base->samples_size = new_samples_count;
		    base->samples = xrealloc (base->samples,
					      base->samples_size * sizeof (cairo_perf_ticks_t));
		}
		for (t = base + 1; t < next; t++) {
		    memcpy (&base->samples[base->samples_count], t->samples,
			    t->samples_count * sizeof (cairo_perf_ticks_t));
		    base->samples_count += t->samples_count;
		}
	    }
	}
	if (base->samples)
	    _cairo_stats_compute (&base->stats, base->samples, base->samples_count);
	base = next;
    }
}

static void
cairo_perf_report_load (cairo_perf_report_t *report, const char *filename)
{
    FILE *file;
    test_report_status_t status;
    int line_number = 0;
    char *line = NULL;
    size_t line_size = 0;
    char *configuration;
    char *dot;

    configuration = strdup (filename);
    report->configuration = strdup (basename (configuration));
    free (configuration);
    dot = strrchr (report->configuration, '.');
    if (dot)
	*dot = '\0';

    report->name = filename;
    report->tests_size = 16;
    report->tests = xmalloc (report->tests_size * sizeof (test_report_t));
    report->tests_count = 0;

    file = fopen (filename, "r");
    if (file == NULL) {
	fprintf (stderr, "Failed to open %s: %s\n",
		 filename, strerror (errno));
	exit (1);
    }

    while (1) {
	if (report->tests_count == report->tests_size) {
	    report->tests_size *= 2;
	    report->tests = xrealloc (report->tests,
				      report->tests_size * sizeof (test_report_t));
	}

	line_number++;
	if (getline (&line, &line_size, file) == -1)
	    break;

	status = test_report_parse (&report->tests[report->tests_count],
				    line, report->configuration);
	if (status == TEST_REPORT_STATUS_ERROR)
	    fprintf (stderr, "Ignoring unrecognized line %d of %s:\n%s",
		     line_number, filename, line);
	if (status == TEST_REPORT_STATUS_SUCCESS)
	    report->tests_count++;
	/* Do nothing on TEST_REPORT_STATUS_COMMENT */
    }

    if (line)
	free (line);

    fclose (file);

    cairo_perf_report_sort_and_compute_stats (report);

    /* Add one final report with a NULL name to terminate the list. */
    if (report->tests_count == report->tests_size) {
	report->tests_size *= 2;
	report->tests = xrealloc (report->tests,
				  report->tests_size * sizeof (test_report_t));
    }
    report->tests[report->tests_count].name = NULL;
}

static int
test_diff_cmp_speedup_before_slowdown (const void *a, const void *b)
{
    const test_diff_t *a_diff = a;
    const test_diff_t *b_diff = b;

    /* First make all speedups come before all slowdowns. */
    if (a_diff->change > 1.0 && b_diff->change < 1.0)
	return -1;
    if (a_diff->change < 1.0 && b_diff->change > 1.0)
	return 1;

    /* Reverse sort by magnitude of change so larger changes come
     * first */
    if (fabs (a_diff->change) > fabs (b_diff->change))
	return -1;

    if (fabs (a_diff->change) < fabs (b_diff->change))
	return 1;

    return 0;
}

static int
test_diff_cmp (const void *a, const void *b)
{
    const test_diff_t *a_diff = a;
    const test_diff_t *b_diff = b;

    /* Reverse sort by magnitude of change so larger changes come
     * first */
    if (a_diff->change > b_diff->change)
	return -1;

    if (a_diff->change < b_diff->change)
	return 1;

    return 0;
}

#define CHANGE_BAR_WIDTH 70
static void
print_change_bar (double change, double max_change, int use_utf)
{
    int units_per_cell = (int) ceil (max_change / CHANGE_BAR_WIDTH);
    static char const *ascii_boxes[8] = {
	"****","***" ,"***", "**",
	"**",  "*",   "*",   ""
    };
    static char const *utf_boxes[8] = {
	"█", "▉", "▊", "▋",
	"▌", "▍", "▎", "▏"
    };
    char const **boxes = use_utf ? utf_boxes : ascii_boxes;

    /* For a 1.0x speedup we want a zero-size bar to show "no
     * change". */
    change -= 1.0;

    while (change > units_per_cell) {
	printf(boxes[0]);
	change -= units_per_cell;
    }

    change /= units_per_cell;

    if (change > 7.5/8.0)
	printf(boxes[0]);
    else if (change > 6.5/8.0)
	printf(boxes[1]);
    else if (change > 5.5/8.0)
	printf(boxes[2]);
    else if (change > 4.5/8.0)
	printf(boxes[3]);
    else if (change > 3.5/8.0)
	printf(boxes[4]);
    else if (change > 2.5/8.0)
	printf(boxes[5]);
    else if (change > 1.5/8.0)
	printf(boxes[6]);
    else if (change > 0.5/8.0)
	printf(boxes[7]);

    printf ("\n");
}

static void
test_diff_print_binary (test_diff_t			*diff,
			double				 max_change,
			cairo_perf_report_options_t	*options)
{
    printf ("%5s-%-4s %26s-%-3d  %6.2f %4.2f%% -> %6.2f %4.2f%%: %5.2fx ",
	    diff->tests[0]->backend, diff->tests[0]->content,
	    diff->tests[0]->name, diff->tests[0]->size,
	    diff->tests[0]->stats.median_ticks / diff->tests[0]->stats.ticks_per_ms,
	    diff->tests[0]->stats.std_dev * 100,
	    diff->tests[1]->stats.median_ticks / diff->tests[1]->stats.ticks_per_ms,
	    diff->tests[1]->stats.std_dev * 100,
	    fabs (diff->change));

    if (diff->change > 1.0)
	printf ("speedup\n");
    else
	printf ("slowdown\n");

    if (options->print_change_bars)
	print_change_bar (fabs (diff->change), max_change,
			  options->use_utf);
}

static void
test_diff_print_multi (test_diff_t			*diff,
		       double				 max_change,
		       cairo_perf_report_options_t	*options)
{
    int i;
    double test_time;
    double change;

    printf ("%s (backend: %s-%s, size: %d)\n",
	    diff->tests[0]->name,
	    diff->tests[0]->backend,
	    diff->tests[0]->content,
	    diff->tests[0]->size);

    for (i = 0; i < diff->num_tests; i++) {
	test_time = diff->tests[i]->stats.min_ticks;
	if (options->use_ms)
	    test_time /= diff->tests[i]->stats.ticks_per_ms;
	change = diff->max / test_time;
	printf ("%8s %6.2f: %5.2fx ",
		diff->tests[i]->configuration,
		diff->tests[i]->stats.min_ticks / diff->tests[i]->stats.ticks_per_ms,
		change);

	if (options->print_change_bars)
	    print_change_bar (change, max_change, options->use_utf);
    }

    printf("\n");
}

#define MAX(a,b) ((a) > (b) ? (a) : (b))
static void
cairo_perf_reports_compare (cairo_perf_report_t		*reports,
			    int				 num_reports,
			    cairo_perf_report_options_t	*options)
{
    int i;
    test_report_t **tests, *min_test;
    test_diff_t *diff, *diffs;
    int num_diffs, max_diffs;
    double max_change;
    double test_time;
    cairo_bool_t seen_non_null;
    cairo_bool_t printed_speedup = FALSE;
    cairo_bool_t printed_slowdown = FALSE;

    assert (num_reports >= 2);

    tests = xmalloc (num_reports * sizeof (test_report_t *));

    max_diffs = reports[0].tests_count;
    for (i = 0; i < num_reports; i++) {
	tests[i] = reports[i].tests;
	if (reports[i].tests_count > max_diffs)
	    max_diffs = reports[i].tests_count;
    }

    diff = diffs = xmalloc (max_diffs * sizeof (test_diff_t));

    num_diffs = 0;
    while (1) {
	/* We expect iterations values of 0 when multiple raw reports
	 * for the same test have been condensed into the stats of the
	 * first. So we just skip these later reports that have no
	 * stats. */
	seen_non_null = 0;
	for (i = 0; i < num_reports; i++) {
	    while (tests[i]->name && tests[i]->stats.iterations == 0)
		tests[i]++;
	    if (tests[i]->name)
		seen_non_null = 1;
	}

	if (! seen_non_null)
	    break;

	/* Find the minimum of all current tests, (we have to do this
	 * in case some reports don't have a particular test). */
	min_test = tests[0];
	for (i = 1; i < num_reports; i++)
	    if (test_report_cmp_backend_then_name (tests[i], min_test) < 0)
		min_test = tests[i];

	/* For each report that has the current test, record it into
	 * the diff structure. */
	diff->num_tests = 0;
	diff->tests = xmalloc (num_reports * sizeof (test_diff_t));
	for (i = 0; i < num_reports; i++) {
	    if (test_report_cmp_backend_then_name (tests[i], min_test) == 0) {
		test_time = tests[i]->stats.min_ticks;
		if (options->use_ms)
		    test_time /= tests[i]->stats.ticks_per_ms;
		if (diff->num_tests == 0) {
		    diff->min = test_time;
		    diff->max = test_time;
		} else {
		    if (test_time < diff->min)
			diff->min = test_time;
		    if (test_time > diff->max)
			diff->max = test_time;
		}
		diff->tests[diff->num_tests++] = tests[i];
		tests[i]++;
	    }
	}
	diff->change = diff->max / diff->min;

	if (num_reports == 2) {
	    double old_time, new_time;
	    if (diff->num_tests == 1) {
		printf ("Only in %s: %s %s\n",
			diff->tests[0]->configuration,
			diff->tests[0]->backend,
			diff->tests[0]->name);
		continue;
	    }
	    old_time = diff->tests[0]->stats.min_ticks;
	    new_time = diff->tests[1]->stats.min_ticks;
	    if (options->use_ms) {
		old_time /= diff->tests[0]->stats.ticks_per_ms;
		new_time /= diff->tests[1]->stats.ticks_per_ms;
	    }
	    diff->change = old_time / new_time;
	    if (diff->change < 1.0)
		diff->change = - 1.0 / diff->change;
	}

	diff++;
	num_diffs++;
    }

    if (num_reports == 2)
	qsort (diffs, num_diffs, sizeof (test_diff_t),
	       test_diff_cmp_speedup_before_slowdown);
    else
	qsort (diffs, num_diffs, sizeof (test_diff_t), test_diff_cmp);

    max_change = 1.0;
    for (i = 0; i < num_diffs; i++) {
	if (fabs (diffs[i].change) > max_change)
	    max_change = fabs (diffs[i].change);
    }

    if (num_reports == 2 )
	printf ("old: %s\n"
		"new: %s\n",
		diffs->tests[0]->configuration,
		diffs->tests[1]->configuration);

    for (i = 0; i < num_diffs; i++) {
	diff = &diffs[i];

	/* Discard as uninteresting a change which is less than the
	 * minimum change required, (default may be overriden on
	 * command-line). */
	if (fabs (diff->change) - 1.0 < options->min_change)
	    continue;

	if (num_reports == 2) {
	    if (diff->change > 1.0 && ! printed_speedup) {
		printf ("Speedups\n"
			"========\n");
		printed_speedup = TRUE;
	    }
	    if (diff->change < 1.0 && ! printed_slowdown) {
		printf ("Slowdowns\n"
			"=========\n");
		printed_slowdown = TRUE;
	    }
	    test_diff_print_binary (diff, max_change, options);
	} else {
	    test_diff_print_multi (diff, max_change, options);
	}
    }

    for (i = 0; i < num_diffs; i++)
	free (diffs[i].tests);
    free (diffs);
    free (tests);
}

static void
usage (const char *argv0)
{
    char const *basename = strrchr(argv0, '/');
    basename = basename ? basename+1 : argv0;
    fprintf (stderr,
	     "Usage: %s [options] file1 file2 [...]\n\n",
	     basename);
    fprintf (stderr,
	     "Computes significant performance differences for cairo performance reports.\n"
	     "Each file should be the output of the cairo-perf program (or \"make perf\").\n"
	     "The following options are available:\n"
	     "\n"
	     "--no-utf    Use ascii stars instead of utf-8 change bars.\n"
	     "            Four stars are printed per factor of speedup.\n"
	     "\n"
	     "--no-bars   Don't display change bars at all.\n\n"
	     "\n"
	     "--use-ms    Use milliseconds to calculate differences.\n"
	     "            (instead of ticks which are hardware dependant)\n"
	     "\n"
	     "--min-change threshold[%%]\n"
	     "            Suppress all changes below the given threshold.\n"
	     "            The default threshold of 0.05 or 5%% ignores any\n"
	     "            speedup or slowdown of 1.05 or less. A threshold\n"
	     "            of 0 will cause all output to be reported.\n"
	);
    exit(1);
}

static void
parse_args(int				  argc,
	   char const			**argv,
	   cairo_perf_diff_files_args_t  *args)
{
    int i;

    for (i = 1; i < argc; i++) {
	if (strcmp (argv[i], "--no-utf") == 0) {
	    args->options.use_utf = 0;
	}
	else if (strcmp (argv[i], "--no-bars") == 0) {
	    args->options.print_change_bars = 0;
	}
	else if (strcmp (argv[i], "--use-ms") == 0) {
	    args->options.use_ms = 1;
	}
	else if (strcmp (argv[i], "--min-change") == 0) {
	    char *end = NULL;
	    i++;
	    if (i >= argc)
		usage (argv[0]);
	    args->options.min_change = strtod (argv[i], &end);
	    if (*end) {
		if (*end == '%') {
		    args->options.min_change /= 100;
		} else {
		    usage (argv[0]);
		}
	    }
	}
	else {
	    args->num_filenames++;
	    args->filenames = xrealloc (args->filenames,
					args->num_filenames * sizeof (char *));
	    args->filenames[args->num_filenames - 1] = argv[i];
	}
    }
}

int
main (int argc, const char *argv[])
{
    cairo_perf_diff_files_args_t args = {
	NULL,			/* filenames */
	0,			/* num_filenames */
	{
	    0.05,		/* min change */
	    1,			/* use UTF-8? */
	    1,			/* display change bars? */
	}
    };
    cairo_perf_report_t *reports;
    test_report_t *t;
    int i;

    parse_args (argc, argv, &args);

    if (args.num_filenames < 2)
	usage (argv[0]);

    reports = xmalloc (args.num_filenames * sizeof (cairo_perf_report_t));

    for (i = 0; i < args.num_filenames; i++ )
	cairo_perf_report_load (&reports[i], args.filenames[i]);

    cairo_perf_reports_compare (reports, args.num_filenames, &args.options);

    /* Pointless memory cleanup, (would be a great place for talloc) */
    free (args.filenames);
    for (i = 0; i < args.num_filenames; i++) {
	for (t = reports[i].tests; t->name; t++) {
	    free (t->samples);
	    free (t->backend);
	    free (t->name);
	}
	free (reports[i].tests);
	free (reports[i].configuration);
    }
    free (reports);

    return 0;
}
