import config

WIDTH = 209.
HEIGHT = 125.

outline(0, 0, WIDTH, HEIGHT)


##################################### MODE #####################################

mode_button_x = 18.5
mode_center_y = HEIGHT * .5
mode_delta_y = 23.

cutout(mode_button_x, mode_center_y - 2 * mode_delta_y, 20, 20)
cutout(mode_button_x, mode_center_y - 1 * mode_delta_y, 20, 20)
cutout(mode_button_x, mode_center_y,                    20, 20)
cutout(mode_button_x, mode_center_y + 1 * mode_delta_y, 20, 20)
cutout(mode_button_x, mode_center_y + 2 * mode_delta_y, 20, 20)

mode_key_x = 43.
mode_key_size = 12.
mode_key_ratio = 1.777
mode_sbse_ratio = .5
mode_pip_ratio = .7
mode_text_ofs_y = 1.2

w = mode_key_size * mode_key_ratio
h = mode_key_size
x = mode_key_x - w * .5

# off

y = mode_center_y - 2 * mode_delta_y - h * .5
line(x, y, x + w, y + h)
line(x, y + h, x + w, y)
del y

# side-by-side equal

y = mode_center_y - 1 * mode_delta_y - h * .5
ax = config.se_ax; ay = config.se_ay; aw = config.se_aw; ah = config.se_ah
bx = config.se_bx; by = config.se_by; bw = config.se_bw; bh = config.se_bh

rect(x, y, w, h)
rect(x + ax * w, y + ay * h, aw * w, ah * h)
rect(x + bx * w, y + by * h, bw * w, bh * h)
text_tiny(x + (ax + aw * .5) * w,
          y + (ay + ah * .5) * h + mode_text_ofs_y, 'a')
text_tiny(x + (bx + bw * .5) * w,
          y + (by + bh * .5) * h + mode_text_ofs_y, 'b')

del ax, ay, aw, ah
del bx, by, bw, bh
del y

# side-by-side preview

y = mode_center_y - h * .5
ax = config.sp_ax; ay = config.sp_ay; aw = config.sp_aw; ah = config.sp_ah
bx = config.sp_bx; by = config.sp_by; bw = config.sp_bw; bh = config.sp_bh

rect(x, y, w, h)
rect(x + ax * w, y + ay * h, aw * w, ah * h)
rect(x + bx * w, y + by * h, bw * w, bh * h)
text_tiny(x + (ax + aw * .5) * w,
          y + (ay + ah * .5) * h + mode_text_ofs_y, 'a')
text_tiny(x + (bx + bw * .5) * w,
          y + (by + bh * .5) * h + mode_text_ofs_y, 'b')

del ax, ay, aw, ah
del bx, by, bw, bh
del y

# picture-in-picture

y = mode_center_y + 1 * mode_delta_y - h * .5
bx = config.pp_bx; by = config.pp_by; bw = config.pp_bw; bh = config.pp_bh

rect(x, y, w, h)
rect(x + bx * w, y + by * h, bw * w, bh * h)
text_tiny(x + w * .5, y + h * .5 + mode_text_ofs_y, 'a')
text_tiny(x + (bx + bw * .5) * w,
          y + (by + bh * .5) * h + mode_text_ofs_y, 'b')

del bx, by, bw, bh
del y

# fullscreen

y = mode_center_y + 2 * mode_delta_y - h * .5
rect(x, y, w, h)
text_tiny(x + w * .5, y + h * .5 + mode_text_ofs_y, 'a')

del x, y, w, h


################################## PROJECTION ##################################

proj_center_x = 132.
proj_delta_x = 26.
proj_caption_y = 12.5 + 1.
proj_button_y = 27.5 + 1.
proj_key_y = 44.5 + 1.

proj_rect_width = 140.
proj_rect_top_y = 4. + 1.
proj_rect_bottom_y = 50. + 1.

rounded_rect(proj_center_x - proj_rect_width * .5, proj_rect_top_y,
             proj_rect_width, proj_rect_bottom_y - proj_rect_top_y, 3)

text_large(proj_center_x, proj_caption_y, 'PROJECTION')

cutout(proj_center_x - 2 * proj_delta_x, proj_button_y, 20, 20)
cutout(proj_center_x - 1 * proj_delta_x, proj_button_y, 20, 20)
cutout(proj_center_x,                    proj_button_y, 20, 20)
cutout(proj_center_x + 1 * proj_delta_x, proj_button_y, 20, 20)
cutout(proj_center_x + 2 * proj_delta_x, proj_button_y, 20, 20)

text_small(proj_center_x - 2 * proj_delta_x, proj_key_y, 'INFO')
text_small(proj_center_x - 1 * proj_delta_x, proj_key_y, 'SLIDES')
text_small(proj_center_x,                    proj_key_y, 'DOC CAM')
text_small(proj_center_x + 1 * proj_delta_x, proj_key_y, 'EXTRA')
text_small(proj_center_x + 2 * proj_delta_x, proj_key_y, 'BLACK')


#################################### SOURCE ####################################

src_key_x = 64.
src_center_x = 122.
src_delta_x = 42.
src_take_x = 190.5
src_key_ofs_y = 2.
src_caption_y = 58.5 + 3.
src_button0_y = 73.5 + 3.
src_button1_y = 99.5 + 3.
src_key_y = 116.5 + 3.

line(src_center_x - src_delta_x, proj_rect_bottom_y,
     src_center_x - src_delta_x, src_button1_y)

text_large((src_center_x - src_delta_x + src_take_x) * .5, src_caption_y,
           'STREAM/RECORDING')

text_large(src_key_x, src_button0_y + src_key_ofs_y, 'b')
text_large(src_key_x, src_button1_y + src_key_ofs_y, 'a')

cutout(src_center_x - src_delta_x, src_button0_y, 20, 20)
cutout(src_center_x,               src_button0_y, 58, 20)
cutout(src_center_x + src_delta_x, src_button0_y, 20, 20)

cutout(src_center_x - src_delta_x, src_button1_y, 20, 20)
cutout(src_center_x,               src_button1_y, 58, 20)
cutout(src_center_x + src_delta_x, src_button1_y, 20, 20)
cutout(src_take_x,                 src_button1_y, 20, 20)

text_small(src_center_x - src_delta_x, src_key_y, 'PROJECTION')
text_small(src_center_x - 19.,         src_key_y, 'CAM1')
text_small(src_center_x,               src_key_y, 'CAM2')
text_small(src_center_x + 19.,         src_key_y, 'CAM3')
text_small(src_center_x + src_delta_x, src_key_y, 'EXTRA')
text_small(src_take_x,                 src_key_y, 'TAKE')
