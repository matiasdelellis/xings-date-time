AC_PREREQ(2.60)
AC_INIT([xings-date-time], [0.2.0], [mati86dl@gmail.com])
AM_INIT_AUTOMAKE([1.9.6 -Wall -Werror dist-bzip2])

AC_PROG_CC
# Compiling sources with per-target flags requires AM_PROG_CC_C_O
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

dnl ---------------------------------------------------------------------------
dnl - Dependencies
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES(GLIB, [glib-2.0])
PKG_CHECK_MODULES(GIO, [gio-2.0])
PKG_CHECK_MODULES(GTK, [gtk+-3.0])

# Translation

AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=xings-date-time
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package.])

# Enable gsettings schema macros
GLIB_GSETTINGS

dnl ---------------------------------------------------------------------------
dnl - Is docbook2man available?
dnl ---------------------------------------------------------------------------
AC_PATH_PROG(DOCBOOK2MAN, docbook2man, no)
if test "$DOCBOOK2MAN" = "no" ; then
	AC_MSG_WARN([docbook2man not found, will not be able to build man documentation])
fi
AM_CONDITIONAL(HAVE_DOCBOOK2MAN, [test "$DOCBOOK2MAN" != "no"])


dnl ---------------------------------------------------------------------------
dnl - Makefiles, etc.
dnl --------------------------------------------------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	data/appdata/Makefile
	man/Makefile
	po/Makefile.in
	src/Makefile
])
AC_OUTPUT

dnl ---------------------------------------------------------------------------
dnl - Output resume
dnl --------------------------------------------------------------------------
echo "
          xings-date-time v$VERSION
        =============================

        prefix:                    ${prefix}
        compiler:                  ${CC}
        cflags:                    ${CFLAGS}
        cppflags:                  ${CPPFLAGS}
"
