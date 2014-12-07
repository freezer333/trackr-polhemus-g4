#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([CreateCfgFile], [1.0.0], [help@polhemus.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/Quaternion.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
have_g4lib=no
AC_SEARCH_LIBS([g4_read_source_cfg_file],[G4SrcCfg],[have_g4lib=yes])
AC_SEARCH_LIBS([sqrt],[m])

if test "x${have_g4lib}" = xno; then
   AC_MSG_ERROR([
--------------------------------------------------------------------------
The CreateSrcCfg Application requires the G4SrcCfg lib to build and run.  Stopping....
Check 'config.log' for more information.
--------------------------------------------------------------------------])
fi



# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h G4SrcCfgIncl.h],[],
			   [AC_MSG_ERROR(["Failure finding required header file -- Terminating configure"])])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([sqrt],[],
	[AC_MSG_ERROR(["Failure finding required function -- Terminating configure"])])

# Check for GTK+
PKG_CHECK_MODULES([GTK],[gtk+-3.0])



AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
