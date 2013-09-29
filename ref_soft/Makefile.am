ref_LTLIBRARIES = libref_soft.la

libref_soft_la_SOURCES = r_aclip.c r_alias.c r_bsp.c r_draw.c r_edge.c \
                         r_image.c r_light.c r_main.c r_misc.c r_model.c \
                         r_part.c r_poly.c r_polyse.c r_rast.c r_scan.c \
                         r_sprite.c r_surf.c

if REF_SOFT_ASM
libref_soft_la_SOURCES += r_aclipa.asm r_draw16.asm r_drawa.asm r_edgea.asm \
                          r_polysa.asm r_scana.asm r_spr8.asm r_surf8.asm \
                          r_varsa.asm
if !REF_SOFT_ASM_INLINE
if REF_SOFT_LINUX
libref_soft_la_SOURCES += @top_srcdir@/platform/linux/r_varsa.s \
                          @top_srcdir@/platform/linux/r_aclipa.s \
                          @top_srcdir@/platform/linux/r_surf8.s \
                          @top_srcdir@/platform/linux/d_polysa.s \
                          @top_srcdir@/platform/linux/r_edgea.s \
                          @top_srcdir@/platform/linux/r_draw16.s \
                          @top_srcdir@/platform/linux/r_spr8.s \
                          @top_srcdir@/platform/linux/sys_dosa.s \
                          @top_srcdir@/platform/linux/r_drawa.s \
                          @top_srcdir@/platform/linux/math.s \
                          @top_srcdir@/platform/linux/r_scana.s
endif
endif
libref_soft_la_CCASFLAGS = -DELF -x assembler-with-cpp
endif

if REF_SOFT_LINUX
libref_soft_la_SOURCES += @top_srcdir@/platform/linux/rw_x11.c
libref_soft_la_LIBADD  = -lX11 -lXxf86dga
endif
