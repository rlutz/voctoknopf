#!/usr/bin/env python
import sys
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
