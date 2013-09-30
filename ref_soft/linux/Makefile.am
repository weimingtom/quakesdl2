ref_LTLIBRARIES = libref_softx.la

include ../Makefile.inc

libref_softx_la_SOURCES = $(commonsources) \
                          @top_srcdir@/platform/linux/rw_x11.c
libref_softx_la_CFLAGS = -I../ -D_INPUT_SOFT
libref_softx_la_LIBADD = -lX11 -lXxf86dga

if REF_SOFT_ASM
libref_softx_la_SOURCES += r_aclipa.asm r_draw16.asm r_drawa.asm r_edgea.asm \
                           r_polysa.asm r_scana.asm r_spr8.asm r_surf8.asm \
                           r_varsa.asm

# These aren't actually tied to linux, but rather gcc
if !REF_SOFT_ASM_INLINE
libref_softx_la_SOURCES += $(gccasmsources)
endif
libref_softx_la_CCASFLAGS = -DELF -x assembler-with-cpp
endif

