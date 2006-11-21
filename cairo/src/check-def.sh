#!/bin/sh

LANG=C

if ! which nm 2>/dev/null >/dev/null; then
	echo "'nm' not found; skipping test"
	exit 0
fi

test -z "$srcdir" && srcdir=.
status=0

defs="cairo.def"
make $defs
for def in $defs; do
	lib=${def%.def}
	lib=${lib##*/}
	so=.libs/lib${lib}.so

	test -f $so || continue
	echo Checking $def

	{
		echo EXPORTS
		nm $so | grep ' T ' | cut -d' ' -f3 | grep -v '^_cairo_.*_test_\|^_fini\|^_init' | sort -u
		# cheat: copy the last line from the def file!
		tail -n1 $def
	} | diff $def - || status=1
done

exit $status
