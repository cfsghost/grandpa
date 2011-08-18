AC_PREREQ(2.57)
AC_INIT(grandpa, 0.1)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE
AC_PROG_INTLTOOL
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_SILENT_RULES([yes])

AC_HEADER_STDC
# Checks for libraries.
pkg_modules="x11 \
             xrandr \
             clutter-1.0"

PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug], [Enable debugging]),
              CFLAGS="$CFLAGS -g -D_DEBUG") 

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

AC_OUTPUT([
  Makefile
  src/Makefile
  po/Makefile.in
])
