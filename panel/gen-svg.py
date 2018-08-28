#!/usr/bin/env python
import math, sys
from common import WIDTH, HEIGHT

class SVG:
    def __init__(self):
        self.lines = []

    def write(self, f):
        f.write('''\
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1"
     width="%fmm" height="%fmm">
  <style>
    text {
	-inkscape-font-specification: 'Machine Tool Gothic';
	font-family: 'Machine Tool Gothic';
	font-weight: normal;
	font-style: normal;
	font-stretch: normal;
	font-variant: normal;
	font-stretch: normal;		/* condensed/expanded */
	font-variant: normal;		/* small-caps */

	letter-spacing: 0;
	word-spacing: 0;

	text-anchor: middle;

	fill: none;
	stroke: #000000;
	stroke-width: .5mm;
	stroke-linecap: round;
	stroke-linejoin: round;
	stroke-opacity: 1;
    }
    rect {
        opacity: 1;
        fill: none;
        stroke: #000000;
        stroke-width: .5mm;
        stroke-linecap: round;
        stroke-linejoin: round;
        stroke-miterlimit: 4;
        stroke-dasharray: none;
        stroke-opacity: 1;
    }
  </style>
''' % (WIDTH, HEIGHT))
        for line in self.lines:
            f.write('  %s\n' % line)
        f.write('</svg>\n')

    def outline(self, x, y, width, height):
        pass

    def cutout(self, x, y, width, height):
        pass

    def special_cutout(self, sx, sy):
        pass

    def line(self, x0, y0, x1, y1):
        self.lines.append(
            '<path d="M %f,%f %f,%f z" />'
                % (x0 / .264583, y0 / .264583, x1 / .264583, y1 / .264583))
        # Inkscape px<->mm factor

    def rect(self, x, y, width, height):
        self.line(x, y, x + width, y)
        self.line(x + width, y, x + width, y + height)
        self.line(x + width, y + height, x, y + height)
        self.line(x, y + height, x, y)

    def rounded_rect(self, x, y, width, height, r):
        steps = 10
        self.line(x + r,         y,              x + width - r, y)
        for i in xrange(steps):
            a0 = math.pi * .5 * i / steps
            a1 = math.pi * .5 * (i + 1) / steps
            self.line(
                x + width + (math.sin(a0) - 1) * r, y - (math.cos(a0) - 1) * r,
                x + width + (math.sin(a1) - 1) * r, y - (math.cos(a1) - 1) * r)
        self.line(x + width,     y + r,          x + width,     y + height - r)
        for i in xrange(steps):
            a0 = math.pi * .5 * i / steps
            a1 = math.pi * .5 * (i + 1) / steps
            self.line(x + width + (math.cos(a0) - 1) * r,
                      y + height + (math.sin(a0) - 1) * r,
                      x + width + (math.cos(a1) - 1) * r,
                      y + height + (math.sin(a1) - 1) * r)
        self.line(x + width - r, y + height,     x + r,         y + height)
        for i in xrange(steps):
            a0 = math.pi * .5 * i / steps
            a1 = math.pi * .5 * (i + 1) / steps
            self.line(
                x - (math.sin(a0) - 1) * r, y + height + (math.cos(a0) - 1) * r,
                x - (math.sin(a1) - 1) * r, y + height + (math.cos(a1) - 1) * r)
        self.line(x,             y + height - r, x,             y + r)
        for i in xrange(steps):
            a0 = math.pi * .5 * i / steps
            a1 = math.pi * .5 * (i + 1) / steps
            self.line(
                x - (math.cos(a0) - 1) * r, y - (math.sin(a0) - 1) * r,
                x - (math.cos(a1) - 1) * r, y - (math.sin(a1) - 1) * r)

    def text_large(self, x, y, text):
        text = text.replace('&', '&amp;')
        text = text.replace('<', '&lt;')
        text = text.replace('>', '&gt;')
        text = text.replace('"', '&quot;')
        self.lines.append(
            '<text style="font-size: 5.5mm; letter-spacing: 1mm;" '
                  'x="%fmm" y="%fmm">%s</text>' % (x, y, text))

    def text_small(self, x, y, text):
        text = text.replace('&', '&amp;')
        text = text.replace('<', '&lt;')
        text = text.replace('>', '&gt;')
        text = text.replace('"', '&quot;')
        self.lines.append(
            '<text style="font-size: 4mm; letter-spacing: .25mm;" '
                  'x="%fmm" y="%fmm">%s</text>' % (x, y, text))

    def text_tiny(self, x, y, text):
        text = text.replace('&', '&amp;')
        text = text.replace('<', '&lt;')
        text = text.replace('>', '&gt;')
        text = text.replace('"', '&quot;')
        self.lines.append(
            '<text style="font-size: 3.5mm; letter-spacing: .25mm;" '
                  'x="%fmm" y="%fmm">%s</text>' % (x, y, text))

def main():
    try:
        fn, = sys.argv[1:]
    except ValueError:
        sys.stderr.write('Usage: %s FILENAME\n' % sys.argv[0])
        sys.exit(1)

    svg = SVG()
    execfile(fn, {
        'outline': svg.outline,
        'cutout': svg.cutout,
        'special_cutout': svg.special_cutout,
        'line': svg.line,
        'rect': svg.rect,
        'rounded_rect': svg.rounded_rect,
        'text_large': svg.text_large,
        'text_small': svg.text_small,
        'text_tiny': svg.text_tiny
    })
    svg.write(sys.stdout)

if __name__ == '__main__':
    main()
