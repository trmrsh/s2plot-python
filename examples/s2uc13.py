#!/usr/bin/env python
# s2uc13.py
#
# Copyright 2008 Swinburne University of Technology.
#
# This file is part of the S2PLOT Python module.
#
# The S2PLOT Python module is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.  
#
# The S2PLOT Python module is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the S2PLOT Python module.  If not, see
# <http://www.gnu.org/licenses/>.
#
# We would appreciate it if research outcomes using S2PLOT would
# provide the following acknowledgement:
#
# "Three-dimensional visualisation was conducted with the S2PLOT
# progamming library"
#
# and a reference to 
#
# D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
# of the Astronomical Society of Australia, 23(2), 82-93.
#
# original version: Nick Jones, October 2007.

import sys
import os
import getopt
import numpy
from s2plot import *

XLIMIT=100.0
YLIMIT=100.0
ZLIMIT=10.0
NPOINTS=75

def main(argv=None):
    if argv is None:
        argv = sys.argv
    
    try:
        opts, args = getopt.getopt(argv[1:], "l", [])
    except getopt.error, msg:
        print "Unrecognised option: %s" % msg
        return 1
    
    linkcameras = False
    # option processing
    for option, value in opts:
        if option == "-l":
            linkcameras = True
    
    fullscreen = 0 # not full-screen mode
    stereo = 0 # passive stereo mode (1 for active st, 0 for mono)
    
    x1 = -XLIMIT; x2 = XLIMIT # x world coord range
    y1 = -YLIMIT; y2 = YLIMIT # y world coord range
    z1 = -ZLIMIT; z2 = ZLIMIT # z world coord range
    np = NPOINTS              # point population size
    symbol = 0  # -2 = infinitesimally small dot
    
    # B = draw front bottom (-Y,-Z) edge 
    # C = draw front top (-Y,+Z) edge
    # D = draw back bottom (+Y,-Z) edge
    # E = draw back top (+Y,+Z) edge
    # T = draw major tick marks
    # S = draw minor tick marks
    xopt = "BCDETSMNQ"
    # B = draw bottom left (-X,-Z) edge
    # C = draw bottom right (+X,-Z) edge
    # D = draw top left (-X,+Z) edge
    # E = draw top right (+X,+Z) edge
    yopt = "BCDETSMNQ"
    # B = draw left front (-X,-Y) edge
    # C = draw right front (+X,-Y) edge
    # D = draw left back (-X,+Y) edge
    # E = draw right back (+X,+Y) edge
    zopt = "BCDETSMNQ"
    
    # major tick increments on all axes
    xtick = 0.0; ytick = 0.0; ztick = 0.0;
    
    # number of subdivisions on all axes
    nxsub = 4; nysub = 1; nzsub = 10;
    
    interactive = 1; # 0 = just draw; 1 = draw and go interactive
    
    if not s2opend("/s2mono", len(argv), argv):
        sys.stderr.write("Failed to open S2PLOT device - exiting!\n")
        os._exit(1)
    
    # move the default (zeroth) panel to left two thirds of window
    xs2mp(0, 0., 0., 0.67, 1.)
    
    # set world coordinate range
    s2swin(x1, x2, y1, y2, z1, z2)
    
    # generate a set of points extending over world coordinate space
    
    xpts = numpy.random.uniform(x1, x2, np)
    ypts = numpy.random.uniform(y1, y2, np)
    zpts = numpy.random.uniform(z1, z2, np)
    expts = numpy.random.uniform(0, 0.05*(x2-x1), np)
    eypts = []
    for i in range(np):
        r_max = 0.1*abs(ypts[i])
        eypts.append(numpy.random.uniform(0, r_max))
    eypts = numpy.array(eypts)
    ezpts = numpy.random.uniform(0, 0.1*(z2-z1), np)
    
    # plot red points
    s2sci(S2_PG_RED)
    s2pt(np, xpts, ypts, zpts, symbol)
    
    # plot various error bars
    s2sci(S2_PG_WHITE)
    s2errb(4, np, xpts, ypts, zpts, expts, 1)
    s2sci(S2_PG_GREEN)
    s2errb(2, np, xpts, ypts, zpts, eypts, 2)
    s2sci(S2_PG_ORANGE)
    s2errb(9, np, xpts, ypts, zpts, ezpts, 3)
    
    # make a new color
    s2scr(354, 1.0, 0.3, 0.5) # pinkish?
    
    # draw and label a box around the points, in white
    s2sci(354)
    s2box(xopt, xtick, nxsub, yopt, ytick, nysub, zopt, ztick, nzsub)
    s2lab("X-axis", "Y-axis", "Z-axis", "S2PLOT Demonstration program 13")
    
    cs2scb(callback1)
    
    # add a new panel on trc of screen
    panel2 = xs2ap(0.68, 0.38, 1., 1.)
    # set world coordinate range
    print 'current:', xs2qsp()
    xs2cp(panel2)
    print 'current:', xs2qsp()
    s2swin(x1, x2, y1, y2, z1, z2)
    s2sci(S2_PG_BLUE)
    s2slw(3.)
    s2pt(np, xpts, ypts, zpts, 1)
    s2box(xopt, xtick, nxsub, yopt, ytick, nysub, zopt, ztick, nzsub)
    cs2scb(callback2)
    
    # make panel zero active for camera things
    xs2cp(0)
    
    # slave camera on panel 1 to that on panel 0
    if linkcameras:
        xs2lpc(0, 1)
    
    # flush graphics and enter interactive mode
    s2show(interactive)
    
    # successful run
    return

def callback1(t, kc, static=[-1, '']):
    if static[0] < 0:
        static[0] = t
    
    static[1] = "time=%6.2f" % (t-static[0])
    s2sci(S2_PG_YELLOW)
    s2sch(2.)
    s2textxy(-40., 0., 0., static[1])

def callback2(t, kc, static=['']):
    static[0] = "keycount=%03d" % kc
    s2sci(S2_PG_GREEN)
    s2sch(3.)
    s2textxy(-70., 25., 10., static[0])

if __name__ == '__main__':
    sys.exit(main())

