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
AC_PROG_LN_S
PKG_PROG_PKG_CONFIG

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h float.h limits.h mntent.h netdb.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UID_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_GETMNTENT
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([floor getcwd gethostbyname getmntent getpagesize gettimeofday memmove memset mkdir munmap pow putenv select socket sqrt strcasecmp strchr strdup strerror strrchr strstr])


function listContains() {
	local _found="no"
	local _val="$1"
	shift
	local i
	for i in $@; do
		if test "$i" == "$_val"; then
			_found="yes"
		fi
	done
	test "$_found" == "yes"
}

## Custom stuff follows here
CFLAGS="$CFLAGS -Dstricmp=strcasecmp"

PLATFORMS="linux sdl2"
platform=""
ref_soft_platform=""
ref_gl_platforms=""
ref_soft_asm="no"

all_platforms=""



AC_ARG_WITH([platform], [AS_HELP_STRING([--with-platform=], [Choose main binary platform. One of $PLATFORMS])], [
if test -z "$withval"; then
	AC_MSG_ERROR([you have to give a value for platform])
fi
platform="$withval"
])
AC_ARG_ENABLE([ref-soft-platform], AS_HELP_STRING([--enable-ref-soft-platform=], 
[Choose soft refresh platforms. You can specify multiple platform by seperating them using a comma]), 
[
if test -z "$enableval"; then
	AC_MSG_ERROR([you have to specify a value for platform])
fi
ref_soft_platforms="$(echo $enableval | sed 's/,/ /g')"
])

AC_ARG_ENABLE([ref-gl-platform], AS_HELP_STRING([--enable-ref-gl-platform=], 
[Choose gl refresh platforms. You can specify multiple platform by seperating them using a comma]), 
[
if test -z "$enableval"; then
	AC_MSG_ERROR([you have to specify a value for platform])
fi
ref_gl_platforms="$(echo $enableval | sed 's/,/ /g')"
])

AC_ARG_ENABLE([ref-soft-asm],
AS_HELP_STRING([--enable-ref-soft-asm], [Enable soft refresh assembler routines. x86 only]),
[ref_soft_asm="yes"])

if test -z "$platform"; then
	platform="sdl2"
fi
if test -z "$ref_soft_platforms"; then
	ref_soft_platforms="linux sdl2"
fi
if test -z "$ref_gl_platforms"; then
	ref_gl_platforms="linux sdl2"
fi

# Check given platforms
for i in $platform $ref_soft_platforms $ref_gl_platforms; do
	if listContains $i $PLATFORMS; then
		echo -n
	else
		if test "$i" == "yes"; then
			AC_MSG_ERROR([you have to specify a value for platform])
		else
			echo ""
			echo "Specified platform $platform is invalid."
			echo "Use one of these: $platforms"
			AC_MSG_ERROR([invalid platform specified])
		fi
	fi
done

all_platforms="$platform $ref_soft_platforms $ref_gl_platforms"

AM_CONDITIONAL([PLATFORM_LINUX], [test $platform == "linux"])
AM_CONDITIONAL([PLATFORM_SDL2], [test $platform == "sdl2"])
AM_CONDITIONAL([REF_SOFT_LINUX], [listContains "linux" $ref_soft_platforms])
AM_CONDITIONAL([REF_GL_LINUX], [listContains "linux" $ref_gl_platforms])
AM_CONDITIONAL([REF_SOFT_SDL2], [listContains "sdl2" $ref_soft_platforms])
AM_CONDITIONAL([REF_GL_SDL2], [listContains "sdl2" $ref_gl_platforms])

# REF_SOFT_ASM - Enables x86 assembler routines in ref_soft
AM_CONDITIONAL([REF_SOFT_ASM], [test "$ref_soft_asm" == "yes"])
AM_CONDITIONAL([REF_SOFT_ASM_INLINE], [false]) # Needs to be enabled on non-linux platforms
if test "$ref_soft_asm" == "yes"; then
	AC_DEFINE([REF_SOFT_ASM], [1], [Use assembler soft refresh routines])
	AC_DEFINE([REF_SOFT_ASM_INLINE], [1], [Use inline assembler soft refresh routines])
fi

if test "$platform" == "linux"; then
	# For some reason, using the software renderer and not having 
	# position-independent code crashes somewhere in the mixer assembly code. Go figure...
	CFLAGS="$CFLAGS -fPIC"
	AC_DEFINE([SNDMIX_ASM], [1], [Use assembler sound mixing routines])
fi

if listContains "sdl2" "$all_platforms"; then
	PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.0])
fi

# Set directories
libdir="$libdir/$PACKAGE-$PACKAGE_VERSION/"
datarootdir="$datarootdir/$PACKAGE-$PACKAGE_VERSION/"

# Refresh engine directory
AC_SUBST([refdir], [$libdir/ref])
AC_SUBST([refdirdef], [-DREFRESH_ENGINE_DIRECTORY=\\\"$refdir\\\"])
# Games directory
AC_SUBST([gamesdir], [$datadir/game])
AC_SUBST([gamesdirdef], [-DGAMES_DIRECTORY=\\\"$gamesdir\\\"])

# Defaults
AC_DEFINE([DEFAULT_REFRESH_ENGINE], ["glsdl"], [Default refresh engine to use])
AC_DEFINE([DEFAULT_GAME], ["baseq2"], [Default game to use])
AC_DEFINE([DEFAULT_GL_DRIVER], ["libGL.so"], [Default gl driver to use])
AC_DEFINE([HOMESUBDIRECTORY], [".quake2"], [Subdirectory inside home to use for user files])


AC_CONFIG_FILES([Makefile platform/Makefile qcommon/Makefile
                 ref_soft/Makefile ref_soft/linux/Makefile ref_soft/sdl2/Makefile
                 ref_gl/Makefile ref_gl/linux/Makefile ref_gl/sdl2/Makefile
                 server/Makefile game/Makefile client/Makefile])
AC_OUTPUT

echo ""
echo "Configuration summary"
echo "---------------------"
echo "Prefix:                     $prefix"
echo ""
echo "Engine platform:            $platform"
echo "Soft refresh platforms:     $(echo $ref_soft_platforms | sed 's/^ *//g')"
echo "GL refresh platforms:       $(echo $ref_gl_platforms | sed 's/^ *//g')"
echo ""
echo -n "Use soft refresh assembler: $ref_soft_asm"
if test "x$ref_soft_asm_inline" == "xyes"; then
	echo "(inline)"
else
	echo "(external)"
fi
echo "Default refresh engine:     glsdl"
echo "Default game:               baseq2"
