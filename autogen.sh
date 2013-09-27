#!/bin/bash -e

function autodegen() {
	if [ -f Makefile ]; then
		make distclean
	fi
	
	# Remove generated Makefile.in files
	for i in $(find . -type d  | grep -v '^\./\.git'); do
		if [ -f $i/Makefile.am ]; then
			rm -f $i/Makefile.in
		fi
	done
	
	# Remove top-level autotools files
	rm -f aclocal.m4 config.guess config.h.in config.h.in~ config.sub configure depcomp install-sh ltmain.sh missing
	
	# Remove cache
	rm -Rf autom4te.cache
	
	# Remove m4 directory
	rm -Rf m4
}

function help() {
	cat <<EOF
Usage: autogen.sh [--degen] [--devbuild] [--devclean] [args]
Options:
		--degen          Remove all files generated by autogen
		--devbuild       Auto-configure a tree suitable for development
		--devclean       Remove all files generated by --devbuild
		args             Arguments to pass to configure
EOF
	if [ ! -z "$@" ]; then
		exit $@
	fi
}

CFLAGS=""
CONFIGUREFLAGS=""

case $1 in
	--degen)
		autodegen
		exit 0
		;;
	--devbuild)
		CFLAGS="-O0 -g -Wall" # -Werror
		CONFIGUREFLAGS="--prefix=$PWD/out"
		shift
		;;
	--devclean)
		rm -Rf out
		autodegen
		exit 0
		;;
esac

export CFLAGS
mkdir -p m4
autoreconf -i
./configure $CONFIGUREFLAGS 
