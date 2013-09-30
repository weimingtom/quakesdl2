include ../Makefile.inc

ref_LTLIBRARIES = libref_glsdl.la

libref_glsdl_la_SOURCES = $(commonsources) \
                          @top_srcdir@/platform/sdl/ref/qgl_sdl.c \
                          @top_srcdir@/platform/sdl/ref/gl_sdl.c \
                          @top_srcdir@/platform/sdl/ref/input.c
libref_glsdl_la_CFLAGS = @SDL2_CFLAGS@ -I../ -D_INPUT_GL
libref_glsdl_la_LIBADD = @SDL2_LIBS@
