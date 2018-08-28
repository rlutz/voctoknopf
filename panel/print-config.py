#!/usr/bin/env python
import sys
import config

if len(sys.argv) == 2:
    config.init(sys.argv[1])
elif len(sys.argv) != 1:
    sys.stderr.write('Usage: %s [CONFIG-FILE]\n' % sys.argv[0])
    sys.exit(1)

print 'width =', config.width
print 'height =', config.height
print
print 'side-by-side equal      ax = %.3f  ay = %.3f  aw = %.3f  ah = %.3f' % (
    config.se_ax, config.se_ay, config.se_aw, config.se_ah)
print '                        bx = %.3f  by = %.3f  bw = %.3f  bh = %.3f' % (
    config.se_bx, config.se_by, config.se_bw, config.se_bh)
print
print 'side-by-side preview    ax = %.3f  ay = %.3f  aw = %.3f  ah = %.3f' % (
    config.sp_ax, config.sp_ay, config.sp_aw, config.sp_ah)
print '                        bx = %.3f  by = %.3f  bw = %.3f  bh = %.3f' % (
    config.sp_bx, config.sp_by, config.sp_bw, config.sp_bh)
print
print 'picture-in-picture      ax = %.3f  ay = %.3f  aw = %.3f  ah = %.3f' % (
    0., 0., 1., 1.)
print '                        bx = %.3f  by = %.3f  bw = %.3f  bh = %.3f' % (
    config.pp_bx, config.pp_by, config.pp_bw, config.pp_bh)
