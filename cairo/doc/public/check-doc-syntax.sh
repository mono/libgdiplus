#!/bin/sh

LANG=C

test -z "$srcdir" && srcdir=.
test -z "$top_srcdir" && top_srcdir=$srcdir/../..

SGML_DOCS=true
FILES=$srcdir/tmpl/*.sgml

. "$top_srcdir/src/check-doc-syntax.sh"
