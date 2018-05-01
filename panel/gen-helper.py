#!/usr/bin/env python
import sys
from common import read_data

def main():
    for segment in read_data(sys.stdin):
        x, y = segment[0][0]
        print 'M %f %f' % (x + .5, y + .5)
        print 'L %f %f' % (x + .5, y - .5)
        print 'L %f %f' % (x - .5, y - .5)
        print 'L %f %f' % (x - .5, y + .5)
        print 'L %f %f' % (x + .5, y + .5)
        print

if __name__ == '__main__':
    main()
