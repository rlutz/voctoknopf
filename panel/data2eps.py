#!/usr/bin/env python
import math, re, sys, time
from common import WIDTH, HEIGHT, read_data

MODE_PARAMS = {
    'engraving': {
        'line-width': .5,
        'color': True,
        'hairlines': True,
        'fill': False,
        'grow-outline': 0.
    },
    'cutouts': {
        'line-width': 3.,
        'color': True,
        'hairlines': True,
        'fill': False,
        'grow-outline': 0.
    },
    'outline': {
        'line-width': 3.,
        'color': True,
        'hairlines': True,
        'fill': False,
        'grow-outline': 10.
    },
    'helper': {
        'line-width': 3.,
        'color': True,
        'hairlines': True,
        'fill': False,
        'grow-outline': 10.
    },
    'pengraving': {
        'line-width': .5,
        'color': False,
        'hairlines': False,
        'fill': False,
        'grow-outline': 0.
    },
    'pcutouts': {
        'line-width': 3.,
        'color': False,
        'hairlines': False,
        'fill': True,
        'grow-outline': 0.
    }
}

def write_eps(f, segments):
    f.write('%!PS-Adobe-2.0 EPSF-2.0\n')
    f.write('%%BoundingBox: ')
    grow_outline = PARAMS['grow-outline']
    f.write('%d %d %d %d\n' % (round(-grow_outline / 25.4 * 72),
                               round(-grow_outline / 25.4 * 72),
                               round((WIDTH + grow_outline) / 25.4 * 72),
                               round((HEIGHT + grow_outline) / 25.4 * 72)))
    f.write('%%EndComments\n')

    f.write('gsave\n')
    f.write('72 25.4 div dup scale\n')
    f.write('1 setlinecap\n')
    f.write('1 setlinejoin\n')

    last_pos = (0, 0)

    for i, segment in enumerate(segments):
        if PARAMS['color']:
            phase = i * 1.5
            f.write('%f %f %f setrgbcolor\n' % (
                math.sin(phase),
                math.sin(phase + math.pi * .66),
                math.sin(phase - math.pi * .66)))

        if PARAMS['hairlines']:
            f.write('.01 setlinewidth\n')

            f.write('newpath\n')
            f.write('%f %f moveto\n' % last_pos)
            f.write('%f %f lineto\n' % segment[0][0])
            f.write('stroke\n')

        last_pos = segment[-1][-1]

        f.write('%f setlinewidth\n' % PARAMS['line-width'])

        f.write('newpath\n')
        f.write('%f %f moveto\n' % segment[0][0])
        for element in segment:
            if len(element) == 2:
                f.write('%f %f lineto\n' % element[1])
            elif len(element) == 4:
                f.write('%f %f\n' % element[1])
                f.write('%f %f\n' % element[2])
                f.write('%f %f curveto\n' % element[3])
            elif len(element) == 3:
                (x0, y0), (x1, y1), (x2, y2) = element
                f.write('%f %f\n' % (1/3. * x0 + 2/3. * x1,
                                     1/3. * y0 + 2/3. * y1))
                f.write('%f %f\n' % (2/3. * x1 + 1/3. * x2,
                                     2/3. * y1 + 1/3. * y2))
                f.write('%f %f curveto\n' % (x2, y2))
            else:
                assert False
        if PARAMS['fill']:
            f.write('gsave fill grestore\n')
        f.write('stroke\n')

    f.write('grestore\n')

def main():
    global PARAMS

    try:
        mode, = sys.argv[1:]
    except ValueError:
        sys.stderr.write('Usage: %s MODE\n' % sys.argv[0])
        sys.exit(1)

    try:
        PARAMS = MODE_PARAMS[mode]
    except KeyError:
        sys.stderr.write('Valid modes: %s\n' %
                             ', '.join(sorted(MODE_PARAMS.keys())))
        sys.exit(1)

    segments = read_data(sys.stdin)
    write_eps(sys.stdout, segments)

if __name__ == '__main__':
    main()
