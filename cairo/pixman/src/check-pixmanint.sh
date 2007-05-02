#!/bin/sh

LANG=C

test -z "$srcdir" && srcdir=.
status=0

echo 'Checking source files for missing or misplaced #include "pixmanint.h"'

find "$srcdir" -name '*.c' -or -name '*.cpp' |
while read x; do
	grep '\<include\>' "$x" /dev/null | head -n 1
done |
grep -v '"pixmanint.h"' |
grep . && status = 1

exit $status
