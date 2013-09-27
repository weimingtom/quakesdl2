lib_LTLIBRARIES = libplatform.la

if PLATFORM_LINUX
libplatform_la_SOURCES = linux/sys_linux.c linux/q_shlinux.c linux/glob.c \
                         linux/vid_so.c linux/vid_menu.c \
                         linux/cd_linux.c \
                         linux/net_udp.c \
                         linux/snd_linux.c linux/snd_mixa.s
libplatform_la_CCASFLAGS = -DELF -x assembler-with-cpp
libplatform_la_CFLAGS = @refdirdef@
libplatform_la_LIBADD = -ldl
endif

if PLATFORM_SDL2
libplatform_la_SOURCES = sdl/sys.c sdl/sys_dir.c sdl/sys_hunk.c \
                         sdl/vid.c \
                         sdl/net_udpsock.c \
                         sdl/snd_sdl.c sdl/cd_dummy.c \
                         sdl/vid_menu_linux.c \
                         sdl/kbd.c sdl/in.c \
                         sdl/glob_linux.c
libplatform_la_CFLAGS = @SDL2_CFLAGS@ -I@top_srcdir@/platform/sdl @refdirdef@
libplatform_la_LIBADD = @SDL2_LIBS@

libplatform_la_CCASFLAGS = -DELF -x assembler-with-cpp
endif
