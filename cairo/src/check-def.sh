#!/bin/sh

LANG=C

if ! which nm 2>/dev/null >/dev/null; then
	echo "'nm' not found; skipping test"
	exit 0
fi

test -z "$srcdir" && srcdir=.
status=0

get_cairo_syms='nm "$so" | grep " T " | cut -d" " -f3'
if [ "`uname -s`" = "Linux" ]; then
       get_cairo_syms='objdump -t "$so" | sed -n "/.*g *F *\.\(opd\|text\).* \(.*cairo_.*\)$/s//\2/p"'
fi

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
		eval $get_cairo_syms | grep -v '^_cairo_test_\|^_fini\|^_init' | sort -u
		# cheat: copy the last line from the def file!
		tail -n1 $def
	} | diff $def - || status=1
done

exit $status
