#!/usr/bin/env python
import re, sys
from common import HEIGHT

RE_COMMAND = re.compile('([A-Za-z]) +(.*)$')
RE_ARG1 = re.compile('([-.0-9]+) +(.*)$')
RE_ARG2 = re.compile('([-.0-9]+),([-.0-9]+) +(.*)$')

def convert(s):
    return float(s) * .264583  # Inkscape px<->mm factor

def read_path(f):
    data = []

    for line in f:
        line = line.lstrip(' ')
        if not line.startswith('d="'):
            continue
        assert line.endswith('"\n')
        d = line[3:-2] + ' '

        while d:
            m = RE_COMMAND.match(d)
            assert m is not None
            cmd = m.group(1)
            d = m.group(2)

            if cmd in 'HhVv':
                # single numbers
                args = []
                while True:
                    m = RE_ARG1.match(d)
                    if m is None:
                        break
                    args.append(convert(m.group(1)))
                    d = m.group(2)
                assert args is not None
            elif cmd in 'MmLlTt':
                # x,y pairs
                args = []
                while True:
                    m = RE_ARG2.match(d)
                    if m is None:
                        break
                    args.append((convert(m.group(1)), convert(m.group(2))))
                    d = m.group(3)
                assert args is not None
            elif cmd in 'SsQq':
                # two x,y pairs
                args = []
                while True:
                    m = RE_ARG2.match(d)
                    if m is None:
                        break
                    args.append((convert(m.group(1)), convert(m.group(2))))
                    d = m.group(3)
                assert args is not None
                assert len(args) % 2 == 0
            elif cmd in 'Cc':
                # three x,y pairs
                args = []
                while True:
                    m = RE_ARG2.match(d)
                    if m is None:
                        break
                    args.append((convert(m.group(1)), convert(m.group(2))))
                    d = m.group(3)
                assert args is not None
                assert len(args) % 3 == 0
            elif cmd in 'Aa':
                raise NotImplementedError
            elif cmd in 'Zz':
                args = None
            else:
                sys.stderr.write("Invalid command '%s'" % cmd)
                sys.exit(1)

            data.append((cmd, args))

        data.append((None, None))

    return data

ELEMENT_LINE, ELEMENT_CUBIC_BEZIER, ELEMENT_QUADRATIC_BEZIER = xrange(3)

def parse_path(data):
    elements = []
    pen_x, pen_y = 0., 0.
    ss_x, ss_y = 0., 0.
    start_subpath = True

    for cmd, args in data:
        if cmd is None:
            pen_x, pen_y = 0., 0.
            ss_x, ss_y = 0., 0.
            start_subpath = True
            continue

        is_relative = cmd.upper() != cmd

        if cmd in 'Mm':
            x, y = args[0]
            if is_relative:
                x += pen_x
                y += pen_y
            pen_x = x
            pen_y = y
            if start_subpath:
                ss_x = x
                ss_y = y

            for x, y in args[1:]:
                if is_relative:
                    x += pen_x
                    y += pen_y
                elements.append((ELEMENT_LINE, pen_x, pen_y, x, y))
                pen_x = x
                pen_y = y
        elif cmd in 'Ll':
            for x, y in args:
                if is_relative:
                    x += pen_x
                    y += pen_y
                elements.append((ELEMENT_LINE, pen_x, pen_y, x, y))
                pen_x = x
                pen_y = y
        elif cmd in 'Hh':
            for x in args:
                if is_relative:
                    x += pen_x
                elements.append((ELEMENT_LINE, pen_x, pen_y, x, pen_y))
                pen_x = x
        elif cmd in 'Vv':
            for y in args:
                if is_relative:
                    y += pen_y
                elements.append((ELEMENT_LINE, pen_x, pen_y, pen_x, y))
                pen_y = y
        elif cmd in 'Zz':
            #elements.append((ELEMENT_LINE, pen_x, pen_y, ss_x, ss_y))
            pen_x = ss_x
            pen_y = ss_y

        elif cmd in 'Cc':
            while args:
                (x1, y1), (x2, y2), (x, y) = args[0:3]
                del args[0:3]
                if is_relative:
                    x1 += pen_x; y1 += pen_y
                    x2 += pen_x; y2 += pen_y
                    x += pen_x; y += pen_y

                elements.append((ELEMENT_CUBIC_BEZIER, pen_x, pen_y,
                                 x1, y1, x2, y2, x, y))
                pen_x = x
                pen_y = y
        elif cmd in 'Ss':
            while args:
                (x2, y2), (x, y) = args[0:2]
                del args[0:2]
                if is_relative:
                    x2 += pen_x; y2 += pen_y
                    x += pen_x; y += pen_y

                if elements and elements[-1][0] == ELEMENT_CUBIC_BEZIER:
                    x1 = pen_x * 2 - elements[-1][5]
                    y1 = pen_y * 2 - elements[-1][6]
                else:
                    x1 = pen_x
                    y1 = pen_y

                elements.append((ELEMENT_CUBIC_BEZIER, pen_x, pen_y,
                                 x1, y1, x2, y2, x, y))
                pen_x = x
                pen_y = y

        elif cmd in 'Qq':
            while args:
                (x1, y1), (x, y) = args[0:2]
                del args[0:2]
                if is_relative:
                    x1 += pen_x; y1 += pen_y
                    x += pen_x; y += pen_y

                elements.append((ELEMENT_QUADRATIC_BEZIER, pen_x, pen_y,
                                 x1, y1, x, y))
                pen_x = x
                pen_y = y
        elif cmd in 'Tt':
            for x, y in args:
                if is_relative:
                    x += pen_x; y += pen_y

                if elements and elements[-1][0] == ELEMENT_QUADRATIC_BEZIER:
                    x1 = pen_x * 2 - elements[-1][3]
                    y1 = pen_y * 2 - elements[-1][4]
                else:
                    x1 = pen_x
                    y1 = pen_y

                elements.append((ELEMENT_CUBIC_BEZIER, pen_x, pen_y,
                                 x1, y1, x, y))
                pen_x = x
                pen_y = y

        else:
            raise NotImplementedError

        start_subpath = cmd in 'Zz'

    return elements

def group_elements(elements):
    drawn_elements = set()
    segments = []

    for element in elements:
        if element in drawn_elements:
            continue

        current_segment = []
        segments.append(current_segment)

        while element is not None:
            current_segment.append(element)
            drawn_elements.add(element)
            drawn_elements.add((element[0], ) + tuple(
                element[(i % 2 - 1) * 2 - i]
                for i in xrange(len(element) - 1)))

            next_element = None
            for e in elements:
                if e not in drawn_elements and e[1] == element[-2] \
                                           and e[2] == element[-1]:
                    next_element = e
                    break

            element = next_element

    return segments

def write_data(f, segments):
    for i, segment in enumerate(segments):
        if i != 0:
            print

        print 'M', segment[0][1], HEIGHT - segment[0][2]
        pen = segment[0][1], segment[0][2]

        for element in segment:
            assert pen == (element[1], element[2])
            if element[0] == ELEMENT_LINE:
                print 'L', element[3], HEIGHT - element[4]
                pen = element[3], element[4]
            elif element[0] == ELEMENT_CUBIC_BEZIER:
                print 'C', element[3], HEIGHT - element[4], \
                           element[5], HEIGHT - element[6], \
                           element[7], HEIGHT - element[8]
                pen = element[7], element[8]
            elif element[0] == ELEMENT_QUADRATIC_BEZIER:
                x0, y0, x1, y1, x2, y2 = element[1:7]
                pen = x2, y2
                #print 'Q', x1, HEIGHT - y1, x2, HEIGHT - y2
                #print 'C', 1/3. * x0 + 2/3. * x1, 1/3. * y0 + 2/3. * y1, \
                #           2/3. * x1 + 1/3. * x2, 2/3. * y1 + 1/3. * y2, x2, y2

                y0 = HEIGHT - y0
                y1 = HEIGHT - y1
                y2 = HEIGHT - y2
                steps = 10
                for i in xrange(steps):
                    t = 1. / steps * (i + 1)
                    print 'L', (1-t)*(1-t) * x0 + 2*(1-t)*t * x1 + t*t * x2, \
                               (1-t)*(1-t) * y0 + 2*(1-t)*t * y1 + t*t * y2
            else:
                raise NotImplementedError

def main():
    data = read_path(sys.stdin)
    elements = parse_path(data)
    segments = group_elements(elements)
    sys.stderr.write(repr(segments))
    write_data(sys.stdout, segments)

if __name__ == '__main__':
    main()
