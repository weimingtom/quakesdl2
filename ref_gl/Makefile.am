lib_LTLIBRARIES = libref_glx.la

libref_glx_la_SOURCES = gl_draw.c gl_image.c gl_light.c gl_mesh.c gl_model.c \
                       gl_rmain.c gl_rmisc.c gl_rsurf.c gl_warp.c

if PLATFORM_LINUX
libref_glx_la_SOURCES += @top_srcdir@/platform/linux/qgl_linux.c @top_srcdir@/platform/linux/gl_glx.c
libref_glx_la_LIBADD  = -lX11 -lXext -lXxf86dga -lXxf86vm -lm
endif
