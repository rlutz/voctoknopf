#!/usr/bin/env python
import math, re, sys, time
from common import read_data

MODE_PARAMS = {
    'engraving': {
        'description': 'Engraving',
        'tool': 'engraving tool',
        'speed': 3000,
        'safe_z': -68.,
        'depth': [-70.0],
        'plunge_feedrate': 20,
        'feedrate': 20
    },
    'cutouts': {
        'description': 'Cutouts',
        'tool': 'd=3mm flat',
        'speed': 7000,
        'safe_z': -68.,
        'depth': [-70.6, -71.0, -71.4, -72.0],
        'plunge_feedrate': 30,
        'feedrate': 30
    },
    'outline': {
        'description': 'Outline',
        'tool': 'd=3mm flat',
        'speed': 7000,
        'safe_z': -68.,
        'depth': [-70.6, -71.0, -71.4, -72.0],
        'plunge_feedrate': 30,
        'feedrate': 30
    },
    'helper': {
        'description': 'Drill helper',
        'tool': 'd=3mm drill-tip',
        'speed': 5000,
        'safe_z': -68.,
        'depth': [-70.4, -70.8, -71.2, -72.0],
        'plunge_feedrate': 20,
        'feedrate': 20
    }
}

last_x, last_y, last_z = None, None, None
total_dist, total_time = 0., 0.

def g0_xy(x, y):
    global last_x, last_y
    last_x, last_y = x, y
    print 'G0 X%.3f Y%.3f' % (x, y)

def g0_z(z):
    global last_z
    last_z = z
    print 'G0 Z%.3f' % z

def g1_xy(x, y, feedrate):
    global last_x, last_y, total_dist, total_time
    dist = math.hypot(x - last_x, y - last_y)
    total_dist += dist
    total_time += dist / feedrate
    last_x, last_y = x, y
    print 'G1 X%.3f Y%.3f F%.0f' % (x, y, feedrate)

def g1_z(z, plunge_feedrate):
    global last_z, total_dist, total_time
    dist = math.fabs(z - last_z)
    total_dist += dist
    total_time += dist / plunge_feedrate
    last_z = z
    print 'G1 Z%.3f F%.0f' % (z, plunge_feedrate)

def write_gcode(segments):
    print '(%s)' % time.strftime('%c')
    print '(%s)' % PARAMS['description']
    print '(Tool: %s)' % PARAMS['tool']
    print 'G21'

    plunge_feedrate = PARAMS['plunge_feedrate']
    feedrate = PARAMS['feedrate']
    depths = PARAMS['depth']
    safe_z = PARAMS['safe_z']

    print 'M3 S%.0f' % PARAMS['speed']
    g0_z(safe_z)

    for segment in segments:
        g0_xy(*segment[0][0])

        for i, depth in enumerate(depths):
            if i != 0 and segment[-1][1] != segment[0][0]:
                g0_z(safe_z)
                g0_xy(*segment[0][0])

            g1_z(depth, plunge_feedrate)
            for element in segment:
                g1_xy(*element[1], feedrate = feedrate)

        g0_z(safe_z)

    print 'M5'

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

    write_gcode(segments)

    hours = math.floor(total_time / 60.)
    minutes = math.floor(total_time - hours * 60.)
    seconds = (total_time - hours * 60. - minutes) * 60.
    sys.stderr.write('%s: %.0fmm, %.0fh %.0fm %.0fs total\n' % (
        PARAMS['description'], total_dist, hours, minutes, seconds))

if __name__ == '__main__':
    main()
