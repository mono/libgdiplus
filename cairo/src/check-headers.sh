#!/bin/sh

LANG=C

test -z "$srcdir" && srcdir=.
status=0

echo Checking public headers for missing cairo_public decorators

find "$srcdir" -name '*.h' ! -name '*-private.h' ! -name '*-test.h' ! -name 'cairoint.h' |
xargs grep -B 1 '^cairo_.*[ 	]\+(' |
awk '
/^--$/ { context=""; public=0; next; }
/:cairo_.*[ 	]+\(/ { if (!public) {print context; print; print "--";} next; }
/-cairo_public.*[ 	]/ {public=1;}
{ context=$0; }
' |
sed 's/[.]h-/.h:/' |
grep . && status=1

exit $status
