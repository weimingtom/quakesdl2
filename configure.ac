#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(quake2, 0.1, [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([game/g_main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign silent-rules])
LT_INIT

# Checks for programs.
AM_PROG_AS
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h float.h limits.h mntent.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_GETMNTENT
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
#AC_CHECK_FUNCS([bzero floor getcwd gethostbyname getmntent getpagesize gettimeofday memmove memset mkdir munmap pow putenv select socket sqrt strcasecmp strchr strdup strerror strrchr strstr])

AM_CONDITIONAL([PLATFORM_LINUX], [true])
AM_CONDITIONAL([PLATFORM_LINUX_ENABLEGL], [true])

CFLAGS+=" -Dstricmp=strcasecmp"

# For some reason, using the software renderer and not having position-independent code crashes somewhere in the mixer assembly code. Go figure...
CFLAGS+=" -fPIC"

AC_CONFIG_FILES([Makefile platform/Makefile qcommon/Makefile ref_soft/Makefile ref_gl/Makefile server/Makefile game/Makefile client/Makefile ])
AC_OUTPUT
