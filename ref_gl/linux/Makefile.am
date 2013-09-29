include ../Makefile.inc

ref_LTLIBRARIES = libref_glx.la

libref_glx_la_SOURCES = $(commonsources) \
                        @top_srcdir@/platform/linux/qgl_linux.c \
                        @top_srcdir@/platform/linux/gl_glx.c
libref_glx_la_CFLAGS = -I../
libref_glx_la_LIBADD = -lX11 -lXext -lXxf86dga -lXxf86vm -lm
