commonsources = ../r_aclip.c ../r_alias.c ../r_bsp.c ../r_draw.c ../r_edge.c \
                ../r_image.c ../r_light.c ../r_main.c ../r_misc.c ../r_model.c \
                ../r_part.c ../r_poly.c ../r_polyse.c ../r_rast.c ../r_scan.c \
                ../r_sprite.c ../r_surf.c
gccasmsources = @top_srcdir@/platform/linux/r_varsa.s \
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
