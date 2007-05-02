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

#include "cairo-boilerplate.h"
#include "xmalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void *
xmalloc (size_t size)
{
    void *buf;

    buf = malloc (size);
    if (!buf) {
	CAIRO_BOILERPLATE_LOG ("Error: Out of memory. Exiting.\n");
	exit (1);
    }

    return buf;
}

void *
xcalloc (size_t nmemb, size_t size)
{
    void *buf;

    buf = calloc (nmemb, size);
    if (!buf) {
	CAIRO_BOILERPLATE_LOG ("Error: Out of memory. Exiting\n");
	exit (1);
    }

    return buf;
}

void *
xrealloc (void *buf, size_t size)
{
    buf = realloc (buf, size);
    if (!buf) {
	CAIRO_BOILERPLATE_LOG ("Error: Out of memory. Exiting\n");
	exit (1);
    }

    return buf;
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
	CAIRO_BOILERPLATE_LOG ("Error: Out of memory. Exiting.\n");
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
	CAIRO_BOILERPLATE_LOG ("Failure in vsnprintf\n");
	exit (1);
    }

    if (strlen (buffer) == sizeof(buffer) - 1) {
	CAIRO_BOILERPLATE_LOG ("Overflowed fixed buffer\n");
	exit (1);
    }

    *strp = strdup (buffer);
    if (!*strp) {
	CAIRO_BOILERPLATE_LOG ("Out of memory\n");
	exit (1);
    }
#endif /* !HAVE_VASNPRINTF */
}
