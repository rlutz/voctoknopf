import config

WIDTH = 209.
HEIGHT = 125.

outline(0, 0, WIDTH, HEIGHT)


##################################### MODE #####################################

mode_button_x = WIDTH - 20.
mode_key_x = mode_button_x - 24.5
mode_key_size = 12.
mode_key_ratio = 1.777
mode_text_ofs_y = 1.2

def mode_off(x, y):
    w = mode_key_size * mode_key_ratio
    h = mode_key_size
    x -= w * .5
    y -= h * .5

    line(x, y, x + w, y + h)
    line(x, y + h, x + w, y)

def mode_side_by_side_equal(x, y):
    w = mode_key_size * mode_key_ratio
    h = mode_key_size
    x -= w * .5
    y -= h * .5

    ax = config.se_ax; ay = config.se_ay; aw = config.se_aw; ah = config.se_ah
    bx = config.se_bx; by = config.se_by; bw = config.se_bw; bh = config.se_bh

    rect(x, y, w, h)
    rect(x + ax * w, y + ay * h, aw * w, ah * h)
    rect(x + bx * w, y + by * h, bw * w, bh * h)
    text_tiny(x + (ax + aw * .5) * w,
              y + (ay + ah * .5) * h + mode_text_ofs_y, 'a')
    text_tiny(x + (bx + bw * .5) * w,
              y + (by + bh * .5) * h + mode_text_ofs_y, 'b')

def mode_side_by_side_preview(x, y):
    w = mode_key_size * mode_key_ratio
    h = mode_key_size
    x -= w * .5
    y -= h * .5

    ax = config.sp_ax; ay = config.sp_ay; aw = config.sp_aw; ah = config.sp_ah
    bx = config.sp_bx; by = config.sp_by; bw = config.sp_bw; bh = config.sp_bh

    rect(x, y, w, h)
    rect(x + ax * w, y + ay * h, aw * w, ah * h)
    rect(x + bx * w, y + by * h, bw * w, bh * h)
    text_tiny(x + (ax + aw * .5) * w,
              y + (ay + ah * .5) * h + mode_text_ofs_y, 'a')
    text_tiny(x + (bx + bw * .5) * w,
              y + (by + bh * .5) * h + mode_text_ofs_y, 'b')

def mode_picture_in_picture(x, y):
    w = mode_key_size * mode_key_ratio
    h = mode_key_size
    x -= w * .5
    y -= h * .5

    bx = config.pp_bx; by = config.pp_by; bw = config.pp_bw; bh = config.pp_bh

    rect(x, y, w, h)
    rect(x + bx * w, y + by * h, bw * w, bh * h)
    text_tiny(x + w * .5, y + h * .5 + mode_text_ofs_y, 'a')
    text_tiny(x + (bx + bw * .5) * w,
              y + (by + bh * .5) * h + mode_text_ofs_y, 'b')


mode_first_y = 16.

#cutout(mode_button_x, mode_first_y, 20., 20.)
cutout(mode_button_x, mode_first_y + 19., 20., 58.)

#mode_off(mode_key_x, mode_first_y)
mode_side_by_side_equal(mode_key_x, mode_first_y)
mode_side_by_side_preview(mode_key_x, mode_first_y + 19.)
mode_picture_in_picture(mode_key_x, mode_first_y + 19. + 19.)


#################################### SOURCE ####################################

a = 23.

src_button_x = 33.
src_button_y = 104.
src_key_x = src_button_x - 20.
src_key_y = src_button_y + 17.
src_key_dy = 1.44

cutout(src_button_x,                src_button_y, 20., 20.)
cutout(src_button_x,                src_button_y - a - a - 19. * .5, 20., 39.)
cutout(src_button_x,                src_button_y - a - a - 19 - a, 20., 20.)
cutout(src_button_x + a + 19. * .5, src_button_y, 39., 20.)
cutout(src_button_x + a + 19. * .5, src_button_y - a, 39., 20.)
special_cutout(src_button_x + a + 19. * .5, src_button_y - a - a - 19. * .5)
cutout(src_button_x + a + 19. * .5, src_button_y - a - a - 19 - a, 39., 20.)
cutout(src_button_x + a + 19 + a,   src_button_y, 20., 20.)
cutout(src_button_x + a + 19 + a,   src_button_y - a, 20., 20.)
cutout(src_button_x + a + 19 + a,   src_button_y - a - a - 19. * .5, 20., 39.)
cutout(src_button_x + a + 19. + a + a, src_button_y, 20., 20.)

text_small(src_button_x,                   src_key_y, 'PROJECTION')
text_small(src_button_x + a,               src_key_y, 'CAM1')
text_small(src_button_x + a + 19.,         src_key_y, 'CAM2')
text_small(src_button_x + a + 19. + a,     src_key_y, 'EXTRA')
text_small(src_button_x + a + 19. + a + a, src_key_y, 'OFF')

text_small(src_key_x, src_button_y + src_key_dy - a, 'PROJ')
text_small(src_key_x, src_button_y + src_key_dy - a - a, 'CAM1')
text_small(src_key_x, src_button_y + src_key_dy - a - a - 19., 'CAM2')
text_small(src_key_x, src_button_y + src_key_dy - a - a - 19. - a, 'EXTRA')

src_take_x = mode_button_x

cutout(src_take_x, src_button_y, 20., 20.)
text_small(src_take_x, src_key_y, 'TOGGLE')
