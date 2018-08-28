#!/usr/bin/env python
import math, sys
from common import WIDTH, HEIGHT

def outline(x, y, width, height):
    pass

def cutout(cx, cy, width, height):
    width -= 3;  x = cx - width * .5
    height -= 3; y = cy - height * .5
    print 'M %f %f' % (x + width - 4.5, HEIGHT - y - 4.5)
    print 'L %f %f' % (x + width, HEIGHT - y)
    print 'L %f %f' % (x + width, HEIGHT - y - height)
    print 'L %f %f' % (x,         HEIGHT - y - height)
    print 'L %f %f' % (x,         HEIGHT - y)
    print 'L %f %f' % (x + width, HEIGHT - y)
    print

def special_cutout(sx, sy):
    width = 20. - 3.
    height = 20. - 3.
    ax = sx - 19. * .5 - width * .5
    ay = sy - 19. * .5 - height * .5
    bx = sx + 19. * .5 - width * .5
    by = sy + 19. * .5 - height * .5

    def arc(x, y, r, start, end):
        phi = start
        while True:
            phi += math.pi * .01
            if phi >= end - math.pi * .001:
                break
            print 'L %f %f' % (x + math.cos(phi) * r, y + math.sin(phi) * r)

    assert bx - (ax + width) == by - (ay + height)
    r = (bx - (ax + width)) / (2 - math.sqrt(2))

    print 'M %f %f' % (ax + width - 4.5, HEIGHT - ay - 4.5)
    print 'L %f %f' % (ax + width, HEIGHT - ay)
    print 'L %f %f' % (ax + width, HEIGHT - by + r)

    arc(ax + width + r, HEIGHT - by + r, r, math.pi, math.pi * 1.5)

    print 'L %f %f' % (ax + width + r, HEIGHT - by)
    print 'L %f %f' % (bx + width, HEIGHT - by)
    print 'L %f %f' % (bx + width, HEIGHT - by - height)
    print 'L %f %f' % (bx,         HEIGHT - by - height)
    print 'L %f %f' % (bx,         HEIGHT - ay - height - r)

    arc(bx - r, HEIGHT - ay - height - r, r, 0, math.pi * .5)

    print 'L %f %f' % (bx - r, HEIGHT - ay - height)
    print 'L %f %f' % (ax,         HEIGHT - ay - height)
    print 'L %f %f' % (ax,         HEIGHT - ay)
    print 'L %f %f' % (ax + width, HEIGHT - ay)
    print

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

    execfile(fn, globals())

if __name__ == '__main__':
    main()
