WIDTH = 209.
HEIGHT = 125.

def read_data(f):
    segments = []
    segment = None
    pos_x = None
    pos_y = None

    for line in f:
        assert line.endswith('\n')
        line = line[:-1]

        if line.startswith('M '):
            assert segment is None
            segment = []
            segments.append(segment)
            pos_x, pos_y = (float(x) for x in line[2:].split(' '))
        elif line.startswith('L '):
            assert segment is not None
            x, y = (float(x) for x in line[2:].split(' '))
            segment.append(((pos_x, pos_y), (x, y)))
            pos_x, pos_y = x, y
        elif line.startswith('C '):
            assert segment is not None
            x1, y1, x2, y2, x, y = (float(x) for x in line[2:].split(' '))
            segment.append(((pos_x, pos_y), (x1, y1), (x2, y2), (x, y)))
            pos_x, pos_y = x, y
        elif line.startswith('Q '):
            assert segment is not None
            x1, y1, x, y = (float(x) for x in line[2:].split(' '))
            segment.append(((pos_x, pos_y), (x1, y1), (x, y)))
            pos_x, pos_y = x, y
        else:
            assert not line
            segment = None
            pos = None

    return segments
