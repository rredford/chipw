#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Initialisation.
AC_PREREQ(2.69)
AC_INIT([Chips Workshop], 2.0.7, chipw@chrise.me.uk, chipw, https://www.microstupidity.com/chipw/)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR(src/level.cpp)
AC_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.6.0)
if test "x$WX_LIBS" = x; then
    AC_MSG_ERROR([Failed to detect suitable wxWidgets])
fi
RESCOMP="$($WX_CONFIG_WITH_ARGS --rescomp 2>/dev/null)"
AC_SUBST(RESCOMP)
AC_ARG_ENABLE([windres], [AS_HELP_STRING([--enable-windres], [enable the pesky Windows resource file thing])])
AM_CONDITIONAL(ENABLE_WINDRES, [test "x$enable_windres" != "x"])
AC_ARG_ENABLE([autodatadir], [AS_HELP_STRING([--enable-autodatadir], [use wxWidgets function to determine runtime data directory])])
AM_CONDITIONAL(ENABLE_AUTODATADIR, [test "x$enable_autodatadir" != "x"])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Output.
if test "x$enable_debug" = xyes; then
    AC_DEFINE(DEBUG, 1, [Define to 1 to enable debugging features])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
