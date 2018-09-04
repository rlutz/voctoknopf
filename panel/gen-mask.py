#!/usr/bin/env python
import sys
from common import WIDTH, HEIGHT

LEFTRIGHT_SPACING = 2.
TOPBOTTOM_SPACING = 2.

TUBE_DX = 4.75
TUBE_DY = 5.75
TUBE_R = 3.5

SCREW_DX = 4.25
SCREW_DY = 13.75
SCREW_R = 2

def svg_mm(x):
    x = str(x)
    while x.endswith('0') and len(x) > 1:
        x = x[:-1]
    if x.endswith('.'):
        x = x[:-1]
    if x.startswith('0.'):
        x = x[1:]
    if x.startswith('-0.'):
        x = '-' + x[2:]
    return x + 'mm'

def svg_rect(x, y, width, height):
    print '  <rect x="%s" y="%s" width="%s" height="%s"/>' % (
        svg_mm(x), svg_mm(y), svg_mm(width), svg_mm(height))

def svg_circle(cx, cy, r):
    print '  <circle cx="%s" cy="%s" r="%s"/>' % (
        svg_mm(cx), svg_mm(cy), svg_mm(r))

def outline(x, y, width, height):
    pass

def cutout(cx, cy, width, height):
    if (width - 1.) % 19. != 0:
        raise ValueError, "Invalid width: " + width
    if (height - 1.) % 19. != 0:
        raise ValueError, "Invalid height: " + width

    nx = int((width - 1.) / 19.)
    ny = int((height - 1.) / 19.)

    for x in xrange(0, nx):
        for y in xrange(0, ny):
            svg_rect(cx + (x - (nx - 1) * .5) * 19. - 7.,
                     cy + (y - (ny - 1) * .5) * 19. - 7., 14., 14.)

def special_cutout(sx, sy):
    cutout(sx - 19. * .5, sy - 19. * .5, 20., 20.)
    cutout(sx + 19. * .5, sy + 19. * .5, 20., 20.)

def line(x0, y0, x1, y1):
    pass

def rect(x, y, width, height):
    pass

def rounded_rect(x, y, width, height, r):
    pass

def text_large(x, y, text):
    pass

def text_small(x, y, text):
    pass

def text_tiny(x, y, text):
    pass

def main():
    try:
        fn, = sys.argv[1:]
    except ValueError:
        sys.stderr.write('Usage: %s FILENAME\n' % sys.argv[0])
        sys.exit(1)

    print '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'
    print '<svg xmlns="http://www.w3.org/2000/svg" version="1.1"'
    print '     width="%s" height="%s">' % (svg_mm(WIDTH), svg_mm(HEIGHT))
    print '  <style>'
    print '    rect, circle {'
    print '        opacity: 1;'
    print '        fill: none;'
    print '        stroke: #000000;'
    print '        stroke-width: .1mm;'
    print '        stroke-linecap: round;'
    print '        stroke-linejoin: round;'
    print '        stroke-miterlimit: 4;'
    print '        stroke-dasharray: none;'
    print '        stroke-opacity: 1;'
    print '    }'
    print '  </style>'

    svg_rect(LEFTRIGHT_SPACING, TOPBOTTOM_SPACING,
             WIDTH - LEFTRIGHT_SPACING * 2, HEIGHT - TOPBOTTOM_SPACING * 2)

    svg_circle(TUBE_DX,         HEIGHT - TUBE_DY, TUBE_R)
    svg_circle(TUBE_DX,         TUBE_DY,          TUBE_R)
    svg_circle(WIDTH - TUBE_DX, HEIGHT - TUBE_DY, TUBE_R)
    svg_circle(WIDTH - TUBE_DX, TUBE_DY,          TUBE_R)
    svg_circle(SCREW_DX,         HEIGHT - SCREW_DY, SCREW_R)
    svg_circle(SCREW_DX,         SCREW_DY,          SCREW_R)
    svg_circle(WIDTH - SCREW_DX, HEIGHT - SCREW_DY, SCREW_R)
    svg_circle(WIDTH - SCREW_DX, SCREW_DY,          SCREW_R)

    execfile(fn, globals())

    print '</svg>'

if __name__ == '__main__':
    main()
