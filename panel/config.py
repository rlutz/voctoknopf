import re

config_path = None

def read_config(path):
    re_sec = re.compile('^[\t ]*\[([^][\t =]*)\][\t ]*\n$')
    re_val = re.compile('^[\t ]*([^][\t =]*)[\t ]*=[\t ]*([^][\t ]*)[\t ]*\n$')

    f = open(path)
    c = {}
    s = None

    for l in f:
        match = re_sec.match(l)
        if match:
            s = match.group(1)
        match = re_val.match(l)
        if match:
            c[s, match.group(1)] = match.group(2)

    f.close()
    return c

def get_videocap(name):
    for cap in c['mix', 'videocaps'].split(','):
        if cap.startswith(name + '='):
            return cap[len(name) + 1:]
    raise ValueError

def init(path):
    global config_path, c, width, height
    global se_ax, se_ay, se_aw, se_ah, se_bx, se_by, se_bw, se_bh
    global sp_ax, sp_ay, sp_aw, sp_ah, sp_bx, sp_by, sp_bw, sp_bh
    global pp_bx, pp_by, pp_bw, pp_bh

    config_path = path
    c = read_config(path)

    width = int(get_videocap('width'))
    height = int(get_videocap('height'))

    border = c['side-by-side-equal', 'border']
    gutter = c['side-by-side-equal', 'gutter']
    atop = c['side-by-side-equal', 'atop']
    btop = c['side-by-side-equal', 'btop']
    se_ax = round(int(border) / float(width), 3)
    se_ay = round(int(atop) / float(height), 3)
    se_aw = round((width - int(border) * 2 - int(gutter))
                    / float(width) * .5, 3)
    se_ah = round((width - int(border) * 2 - int(gutter))
                    / float(width) * .5, 3)
    se_bx = round((width + int(gutter)) / float(width) * .5, 3)
    se_by = round(int(btop) / float(height), 3)
    se_bw = round((width - int(border) * 2 - int(gutter))
                    / float(width) * .5, 3)
    se_bh = round((width - int(border) * 2 - int(gutter))
                    / float(width) * .5, 3)

    x, y = c['side-by-side-preview', 'apos'].split('/')
    w, h = c['side-by-side-preview', 'asize'].split('x')
    sp_ax = round(int(x) / float(width), 3)
    sp_ay = round(int(y) / float(height), 3)
    sp_aw = round(int(w) / float(width), 3)
    sp_ah = round(int(h) / float(height), 3)

    x, y = c['side-by-side-preview', 'bpos'].split('/')
    w, h = c['side-by-side-preview', 'bsize'].split('x')
    sp_bx = round(int(x) / float(width), 3)
    sp_by = round(int(y) / float(height), 3)
    sp_bw = round(int(w) / float(width), 3)
    sp_bh = round(int(h) / float(height), 3)

    x, y = c['picture-in-picture', 'pippos'].split('/')
    w, h = c['picture-in-picture', 'pipsize'].split('x')
    pp_bx = round(int(x) / float(width), 3)
    pp_by = round(int(y) / float(height), 3)
    pp_bw = round(int(w) / float(width), 3)
    pp_bh = round(int(h) / float(height), 3)

init('config.ini')
