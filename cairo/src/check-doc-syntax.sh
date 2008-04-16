#!/bin/sh

LANG=C

if ! grep --version 2>/dev/null | grep GNU >/dev/null; then
	echo "GNU grep not found; skipping test"
	exit 0
fi

test -z "$srcdir" && srcdir=.
status=0

echo Checking documentation for incorrect syntax

# Note: this test is also run from doc/public/ to check the SGML files

if test "x$SGML_DOCS" = x; then
    FILES=`find "$srcdir" -name '*.h' -or -name '*.c' -or -name '*.cpp'`
fi

# Note: This test reports false positives on non-gtk-doc comments and
# non-public enum values, (such as CAIRO_FIXED_FRAC_BITS in the comment
# for _cairo_output_stream_init). I'm opposed to uglifying those comments
# with % just to shut this warning up. So instead, I'm turning this check
# off. (cworth 2008-03-02)
#
# Meanwhile, I'd love to see a system that would just link things like
# enums up without any decoration.
#
#enum_regexp='\([^%@]\|^\)\<\(FALSE\|TRUE\|NULL\|CAIRO_[0-9A-Z_]*[^(0-9A-Z_]\)'
#if test "x$SGML_DOCS" = x; then
#	enum_regexp='^[/ ][*] .*'$enum_regexp
#fi
#if grep "$enum_regexp" $FILES | grep -v '#####'; then
#	status=1
#	echo Error: some macros in the docs are not prefixed by percent sign.
#	echo Fix this by searching for the following regexp in the above files:
#	echo "	'$enum_regexp'"
#fi

type_regexp='\( .*[^#]\| \|^\)\<cairo[0-9a-z_]*_t\>\($\|[^:]$\|[^:].\)'
if test "x$SGML_DOCS" = x; then
	type_regexp='^[/ ][*]'$type_regexp
else
	type_regexp='\(.'$type_regexp'\)\|\('$type_regexp'.\)'
fi
if grep "$type_regexp" $FILES | grep -v '#####'; then
	status=1
	echo Error: some type names in the docs are not prefixed by hash sign,
	echo neither are the only token in the doc line followed by colon.
	echo Fix this by searching for the following regexp in the above files:
	echo "	'$type_regexp'"
fi

func_regexp='\([^#]\|^\)\<\(cairo_[][<>/0-9a-z_]*\> \?[^][ <>(]\)'
if test "x$SGML_DOCS" = x; then
	func_regexp='^[/ ][*] .*'$func_regexp
fi
if grep "$func_regexp" $FILES | grep -v '#####'; then
	status=1
	echo Error: some function names in the docs are not followed by parentheses.
	echo Fix this by searching for the following regexp in the above files:
	echo "	'$func_regexp'"
fi

note_regexp='NOTE'
if grep "$note_regexp" $FILES; then
	status=1
	echo Error: some source files contain the string 'NOTE'.
	echo Be civil and replace it by 'Note' please.
fi

exit $status
