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
