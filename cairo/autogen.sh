#!/bin/sh
# Run this to generate all the initial makefiles, etc.
set -e

PACKAGE=cairo

LIBTOOLIZE=${LIBTOOLIZE-libtoolize}
LIBTOOLIZE_FLAGS="--copy --force"
ACLOCAL=${ACLOCAL-aclocal}
AUTOHEADER=${AUTOHEADER-autoheader}
AUTOMAKE=${AUTOMAKE-automake}
AUTOMAKE_FLAGS="--add-missing"
AUTOCONF=${AUTOCONF-autoconf}

# automake 1.8 requires autoconf 2.58
# automake 1.7 requires autoconf 2.54
# I don't know what automake 1.4 wants, but the following seems to work...
automake_min_vers=1.4
aclocal_min_vers=$automake_min_vers
autoconf_min_vers=2.54
libtoolize_min_vers=1.4

# The awk-based string->number conversion we use needs a C locale to work as expected.
LANG=C
LC_NUMERIC=C

ARGV0=$0

if ($AUTOCONF --version) < /dev/null > /dev/null 2>&1 ; then
    if ($AUTOCONF --version | head -n 1 | awk 'NR==1 { if( $(NF) >= '$autoconf_min_vers') \
			       exit 1; exit 0; }');
    then
       echo "$ARGV0: ERROR: \`$AUTOCONF' is too old."
       $AUTOCONF --version
       echo "           (version $autoconf_min_vers or newer is required)"
       DIE="yes"
    fi
else
    echo $AUTOCONF: command not found
    echo
    echo "$ARGV0: ERROR: You must have \`autoconf' installed to compile $PACKAGE."
    echo "           (version $autoconf_min_vers or newer is required)"
    DIE="yes"
fi

if ($AUTOMAKE --version) < /dev/null > /dev/null 2>&1 ; then
  if ($AUTOMAKE --version | head -n 1 | awk 'NR==1 { if( $(NF) >= '$automake_min_vers') \
			     exit 1; exit 0; }');
     then
     echo "$ARGV0: ERROR: \`$AUTOMAKE' is too old."
     $AUTOMAKE --version
     echo "           (version $automake_min_vers or newer is required)"
     DIE="yes"
  fi
  if ($ACLOCAL --version) < /dev/null > /dev/null 2>&1; then
    if ($ACLOCAL --version | head -n 1 | awk 'NR==1 { if( $(NF) >= '$aclocal_min_vers' ) \
						exit 1; exit 0; }' );
    then
      echo "$ARGV0: ERROR: \`$ACLOCAL' is too old."
      $ACLOCAL --version
      echo "           (version $aclocal_min_vers or newer is required)"
      DIE="yes"
    fi
  else
    echo $ACLOCAL: command not found
    echo
    echo "$ARGV0: ERROR: Missing \`$ACLOCAL'"
    echo "           The version of $AUTOMAKE installed doesn't appear recent enough."
    DIE="yes"
  fi
else
    echo $AUTOMAKE: command not found
    echo
    echo "$ARGV0: ERROR: You must have \`automake' installed to compile $PACKAGE."
    echo "           (version $automake_min_vers or newer is required)"
    DIE="yes"
fi

if ($LIBTOOLIZE --version) < /dev/null > /dev/null 2>&1 ; then
    if ($LIBTOOLIZE --version | awk 'NR==1 { if( $4 >= '$libtoolize_min_vers') \
			       exit 1; exit 0; }');
    then
       echo "$ARGV0: ERROR: \`$LIBTOOLIZE' is too old."
       echo "           (version $libtoolize_min_vers or newer is required)"
       DIE="yes"
    fi
else
    echo $LIBTOOLIZE: command not found
    echo
    echo "$ARGV0: ERROR: You must have \`libtoolize' installed to compile $PACKAGE."
    echo "           (version $libtoolize_min_vers or newer is required)"
    DIE="yes"
fi

if test -z "$ACLOCAL_FLAGS"; then
    acdir=`$ACLOCAL --print-ac-dir`
    if [ ! -f $acdir/pkg.m4 ]; then
	echo "$ARGV0: Error: Could not find pkg-config macros."
	echo "        (Looked in $acdir/pkg.m4)"
	echo "        If pkg.m4 is available in /another/directory, please set"
	echo "        ACLOCAL_FLAGS=\"-I /another/directory\""
	echo "        Otherwise, please install pkg-config."
	echo ""
	echo "pkg-config is available from:"
	echo "http://www.freedesktop.org/software/pkgconfig/"
	DIE=yes
    fi
fi

if test "X$DIE" != X; then
  exit 1
fi


if test -z "$*"; then
  echo "$ARGV0:	Note: \`./configure' will be run with no arguments."
  echo "		If you wish to pass any to it, please specify them on the"
  echo "		\`$0' command line."
  echo
fi

do_cmd() {
    echo "$ARGV0: running \`$@'"
    $@
}

do_cmd $LIBTOOLIZE $LIBTOOLIZE_FLAGS

do_cmd $ACLOCAL $ACLOCAL_FLAGS

do_cmd $AUTOHEADER

do_cmd $AUTOMAKE $AUTOMAKE_FLAGS

do_cmd $AUTOCONF

do_cmd ./configure --enable-maintainer-mode --enable-gtk-doc ${1+"$@"} && echo "Now type \`make' to compile" || exit 1
