dnl -*- mode: autoconf -*-

# serial 1

dnl Usage:
dnl   GTK_DOC_CHECK([minimum-gtk-doc-version])
AC_DEFUN([GTK_DOC_CHECK],
[
  AC_BEFORE([AC_PROG_LIBTOOL],[$0])dnl setup libtool first
  AC_BEFORE([AM_PROG_LIBTOOL],[$0])dnl setup libtool first
  dnl for overriding the documentation installation directory
  AC_ARG_WITH(html-dir,
    AC_HELP_STRING([--with-html-dir=PATH], [path to installed docs]),,
    [with_html_dir='${datadir}/gtk-doc/html'])
  HTML_DIR="$with_html_dir"
  AC_SUBST(HTML_DIR)

  dnl enable/disable documentation building
  AC_ARG_ENABLE(gtk-doc,
    AC_HELP_STRING([--enable-gtk-doc],
                   [use gtk-doc to build documentation [default=no]]),,
    enable_gtk_doc=no)

  have_gtk_doc=no
  if test x$enable_gtk_doc = xyes; then
    if test -z "$PKG_CONFIG"; then
      AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
    fi
    if test "$PKG_CONFIG" != "no" && $PKG_CONFIG --exists gtk-doc; then
      have_gtk_doc=yes
    fi

  dnl do we want to do a version check?
ifelse([$1],[],,
    [gtk_doc_min_version=$1
    if test "$have_gtk_doc" = yes; then
      AC_MSG_CHECKING([gtk-doc version >= $gtk_doc_min_version])
      if $PKG_CONFIG --atleast-version $gtk_doc_min_version gtk-doc; then
        AC_MSG_RESULT(yes)
      else
        AC_MSG_RESULT(no)
        have_gtk_doc=no
      fi
    fi
])
    if test "$have_gtk_doc" != yes; then
      enable_gtk_doc=no
    fi
  fi

  AM_CONDITIONAL(ENABLE_GTK_DOC, test x$enable_gtk_doc = xyes)
  AM_CONDITIONAL(GTK_DOC_USE_LIBTOOL, test -n "$LIBTOOL")
])

# AX_C_FLOAT_WORDS_BIGENDIAN ([ACTION-IF-TRUE], [ACTION-IF-FALSE],
#                             [ACTION-IF-UNKNOWN])
#
# Checks the ordering of words within a multi-word float. This check
# is necessary because on some systems (e.g. certain ARM systems), the
# float word ordering can be different from the byte ordering. In a
# multi-word float context, "big-endian" implies that the word containing
# the sign bit is found in the memory location with the lowest address.
# This implemenation was inspired by the AC_C_BIGENDIAN macro in autoconf.
# -------------------------------------------------------------------------
AC_DEFUN([AX_C_FLOAT_WORDS_BIGENDIAN],
  [AC_CACHE_CHECK(whether float word ordering is bigendian,
                  ax_cv_c_float_words_bigendian, [

# The endianess is detected by first compiling C code that contains a special
# double float value, then grepping the resulting object file for certain
# strings of ascii values. The double is specially crafted to have a
# binary representation that corresponds with a simple string. In this
# implementation, the string "noonsees" was selected because the individual
# word values ("noon" and "sees") are palindromes, thus making this test
# byte-order agnostic. If grep finds the string "noonsees" in the object
# file, the target platform stores float words in big-endian order. If grep
# finds "seesnoon", float words are in little-endian order. If neither value
# is found, the user is instructed to specify the ordering.

ax_cv_c_float_words_bigendian=unknown
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[

double d = 90904234967036810337470478905505011476211692735615632014797120844053488865816695273723469097858056257517020191247487429516932130503560650002327564517570778480236724525140520121371739201496540132640109977779420565776568942592.0;

]])], [

if strings - conftest.$ac_objext | grep noonsees >/dev/null ; then
  ax_cv_c_float_words_bigendian=yes
fi
if strings - conftest.$ac_objext | grep seesnoon >/dev/null ; then
  if test "$ax_cv_c_float_words_bigendian" = unknown; then
    ax_cv_c_float_words_bigendian=no
  else
    ax_cv_c_float_words_bigendian=unknown
  fi
fi

])])

case $ax_cv_c_float_words_bigendian in
  yes)
    m4_default([$1],
      [AC_DEFINE([FLOAT_WORDS_BIGENDIAN], 1,
                 [Define to 1 if your system stores words within floats
                  with the most significant word first])]) ;;
  no)
    $2 ;;
  *)
    m4_default([$3],
      [AC_MSG_ERROR([

Unknown float word ordering. You need to manually preset
ax_cv_c_float_words_bigendian=no (or yes) according to your system.

    ])]) ;;
esac

])# AX_C_FLOAT_WORDS_BIGENDIAN
