ref_LTLIBRARIES = libref_softsdl.la

include ../Makefile.inc

libref_softsdl_la_SOURCES = $(commonsources) \
                            @top_srcdir@/platform/sdl/ref/rw_sdl.c \
                            @top_srcdir@/platform/sdl/ref/input.c
libref_softsdl_la_CFLAGS = @SDL2_CFLAGS@ -I../ -D_INPUT_SOFT
libref_softsdl_la_LIBADD = @SDL2_LIBS@

if REF_SOFT_ASM
libref_softsdl_la_SOURCES += r_aclipa.asm r_draw16.asm r_drawa.asm r_edgea.asm \
                             r_polysa.asm r_scana.asm r_spr8.asm r_surf8.asm \
                             r_varsa.asm

# These aren't actually tied to linux, but rather gcc
if !REF_SOFT_ASM_INLINE
libref_softsdl_la_SOURCES += $(gccasmsources)
endif
libref_softsdl_la_CCASFLAGS = -DELF -x assembler-with-cpp
endif

