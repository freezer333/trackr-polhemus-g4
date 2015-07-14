#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([g4term], [1.0.0], [help@polhemus.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/FilterDlg.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
have_g4lib=no
AC_SEARCH_LIBS([g4_init_sys],[G4Track],[have_g4lib=yes])

if test "x${have_g4lib}" = xno; then
   AC_MSG_ERROR([
--------------------------------------------------------------------------
The G4Term Application requires the G4Track lib to build and run.  Stopping....
Check 'config.log' for more information.
--------------------------------------------------------------------------])
fi



# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h G4TrackIncl.h],[],
			   [AC_MSG_ERROR(["Failure finding required header file -- Terminating configure"])])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memset],[],
	[AC_MSG_ERROR(["Failure finding required function -- Terminating configure"])])

# Check for GTK+
PKG_CHECK_MODULES([GTK],[gtk+-3.0])

# Checks for pthreads
AX_PTHREAD([
	AC_DEFINE([HAVE_PTHREAD],[1],[Define if you have POSIX threads libraries and header files.])
	LIBS="$PTHREAD_LIBS $LIBS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	CXXFLAGS="$CXXFLAGS $PTHREAD_CXXFLAGS"
	CC="$PTHREAD_CC"
	])


AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
