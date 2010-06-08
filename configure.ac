dnl vim: set sw=4 sts=4 ts=4 noet ft=config foldmethod=marker foldmarker={{{,}}} :

dnl {{{ Program, version
AC_PREREQ(2.59)
AC_INIT([src/main.c])

VERSION_MAJOR=0
VERSION_MINOR=1
VERSION_FULL="$VERSION_MAJOR.$VERSION_MINOR"
VERSION="$VERSION_FULL"

AC_SUBST([VERSION_MAJOR])
AC_SUBST([VERSION_MINOR])
AC_SUBST([VERSION_FULL])

AM_INIT_AUTOMAKE(notmuch-reader, [$VERSION_FULL])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
dnl }}}

dnl {{{ Toolchain checks
AC_PROG_CC
AC_PROG_CC_C99
if test x"$ac_cv_prog_cc_c99" = x"no"; then
	AC_MSG_ERROR([notmuch-reader requires a C compiler that supports ISO C99!])
fi
AC_PROG_INSTALL
AC_PROG_MAKE_SET
dnl }}}

dnl {{{ Use system extensions
AC_USE_SYSTEM_EXTENSIONS
dnl }}}

dnl {{{ Ncurses
AC_CHECK_LIB([ncursesw], initscr, [],
			 [AC_CHECK_LIB([ncurses], initscr, [],
						   [AC_MSG_ERROR([notmuch-reader requires ncurses])])])
AC_CHECK_HEADER([ncursesw/ncurses.h], [AC_DEFINE([HAVE_NCURSESW_NCURSES_H], 1, "ncursesw/ncurses.h")],
				[AC_CHECK_HEADER([ncurses/ncurses.h], [AC_DEFINE([HAVE_NCURSES_NCURSES_H], 1, "ncurses/ncurses.h")],
								 [AC_CHECK_HEADER([ncurses.h], [AC_DEFINE([HAVE_NCURSES_H], 1, "ncurses.h")],
												  [AC_MSG_ERROR([ncurses.h not found])])])])
dnl }}}

dnl {{{ Extra CFLAGS
NMR_CFLAGS=
WANTED_CFLAGS="-Wall -W -Wextra -Wvla -Wformat=2 -Wformat-security -Wformat-nonliteral -Winit-self -Wfloat-equal -Wno-deprecated-declarations -Wmissing-declarations -Wmissing-noreturn -Wmissing-prototypes -Wredundant-decls  -Wshadow -Wpointer-arith -Wstrict-prototypes -Wcast-qual -Wwrite-strings -pedantic"
for flag in $WANTED_CFLAGS ; do
	AX_CHECK_COMPILER_FLAGS([$flag], [NMR_CFLAGS="$NMR_CFLAGS $flag"],)
done
AC_SUBST([NMR_CFLAGS])
dnl }}}

dnl {{{ Output
AM_CONFIG_HEADER([config.h])
AC_OUTPUT(
		  Makefile
		  src/Makefile
)
dnl }}}
