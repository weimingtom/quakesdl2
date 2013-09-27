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
AC_PROG_SED
PKG_PROG_PKG_CONFIG

# Checks for libraries.

# Checks for header files.
#AC_PATH_X
#AC_CHECK_HEADERS([fcntl.h float.h limits.h mntent.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
#AC_C_INLINE
#AC_TYPE_UID_T

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_GETMNTENT
#AC_FUNC_MALLOC
#AC_FUNC_MMAP
#AC_FUNC_REALLOC
#AC_CHECK_FUNCS([bzero floor getcwd gethostbyname getmntent getpagesize gettimeofday memmove memset mkdir munmap pow putenv select socket sqrt strcasecmp strchr strdup strerror strrchr strstr])

## Custom stuff follows here
CFLAGS="$CFLAGS -Dstricmp=strcasecmp"

platforms="linux sdl2"
platform="sdl2"

_found="no"
for i in $platforms; do
	if test "$i" == "$platform"; then
		_found="yes"
	fi
done
if test "$_found" == "no"; then
	echo ""
	echo "Specified platform $platform is invalid."
	echo "Use one of these: $platforms"
	exit 1
fi

AM_CONDITIONAL([PLATFORM_LINUX], [test $platform == "linux"])
AM_CONDITIONAL([PLATFORM_SDL2], [test $platform == "sdl2"])

AM_CONDITIONAL([REF_GL_LINUX], [true])
AM_CONDITIONAL([REF_SOFT_LINUX], [true])

if test "$platform" == "linux"; then
	# For some reason, using the software renderer and not having 
	# position-independent code crashes somewhere in the mixer assembly code. Go figure...
	CFLAGS="$CFLAGS -fPIC"
	AC_DEFINE([SNDMIX_ASM], [1], [Use assembler sound mixing routines])
elif test "$platform" == "sdl2"; then
	PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.0])
fi

# Set directories
libdir="$libdir/$PACKAGE-$PACKAGE_VERSION/"
datadir="$datadir/$PACKAGE-$PACKAGE_VERSION/"
datarootdir="$datarootdir/$PACKAGE-$PACKAGE_VERSION/"

# Refresh engine directory
AC_SUBST([refdir], [$libdir/ref])
AC_SUBST([refdirdef], [-DREFRESH_ENGINE_DIRECTORY=\\\"$refdir\\\"])
# Games directory
AC_SUBST([gamesdir], [$datadir/game])
AC_SUBST([gamesdirdef], [-DGAMES_DIRECTORY=\\\"$gamesdir\\\"])

# Defaults
AC_DEFINE([DEFAULT_REFRESH_ENGINE], ["glx"], [Default refresh engine to use])
AC_DEFINE([DEFAULT_GAME], ["baseq2"], [Default game to use])

AC_CONFIG_FILES([Makefile platform/Makefile qcommon/Makefile ref_soft/Makefile ref_gl/Makefile server/Makefile game/Makefile client/Makefile ])
AC_OUTPUT
