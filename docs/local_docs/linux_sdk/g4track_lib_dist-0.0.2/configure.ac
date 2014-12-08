#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([G4Track_lib_dist], [0.0.2], [help@polhemus.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_LIBTOOL


have_usblibs=no
AC_SEARCH_LIBS([libusb_open_device_with_vid_pid],[usb-1.0],[have_usblibs=yes])

if test "x${have_usblibs}" = xno; then
   AC_MSG_ERROR([
--------------------------------------------------------------------------
The G4Track library requires libusb-1.0 lib to run.  Please install before
continuing.     Stopping....
Check 'config.log' for more information.
--------------------------------------------------------------------------])
fi



AC_CONFIG_FILES([Makefile
		 include/Makefile
		 udev_rules/Makefile
		 bin/Makefile
		 man/Makefile])


AC_OUTPUT

