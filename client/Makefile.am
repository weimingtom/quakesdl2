bin_PROGRAMS = client

client_SOURCES  = cl_cin.c cl_ents.c cl_fx.c cl_input.c cl_inv.c \
                  cl_main.c cl_newfx.c cl_parse.c cl_pred.c cl_scrn.c \
                  cl_tent.c cl_view.c console.c keys.c menu.c qmenu.c \
                  snd_dma.c snd_mem.c snd_mix.c

# Yes, this is actually needed
client_SOURCES += @top_srcdir@/game/m_flash.c

client_LDADD    = @top_builddir@/platform/libplatform.la \
                  @top_builddir@/qcommon/libqcommon.la \
                  @top_builddir@/server/libserver.la \
                  -lm
