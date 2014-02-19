#!/usr/bin/env python
# cs2shcb.py
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

from s2plot import *
import numpy, random

N = 100
xyz = []
col = []
sel = []

def pick(id):
    print 'handle callback'
    sel[id] = (sel[id]+1)%2
    cs2th(id)

def cb(t, kc):
    global xyz, col, sel
    hilite = {'r':1.0, 'g':1.0, 'b': 1.0}
    hsize = 0.02
    
    try:
        if kc % 2 == 0:
            for i in range(0,N):
                ns2vthpoint(xyz[i], col[i], 3)
                ds2ah(xyz[i], hsize, col[i], hilite, i, sel[i])
        else:
            for i in range(0,N):
                if sel[i]:
                    ns2vthpoint(xyz[i], col[i], 3)
                    ds2ah(xyz[i], hsize, col[i], hilite, i, sel[i])
    except Exception, e:
        print e

def main():
    global xyz, col, sel
    
    print "Shift-s to toggle handles\nShift-c to toggle crosshair\n"
    print "Right mouse select\nSpacebar toggle show all/show sel\n"
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    xyz = [{'x': random.uniform(-1.0,1.0), 'y': random.uniform(-1.0,1.0), 'z': random.uniform(-1.0,1.0)}\
                for i in range(0,N)]
    col = [{'r': random.random(), 'g': random.random(), 'b': random.random()}\
                for i in range(0,N)]
    sel = [0 for i in range(0,N)]
    
    cs2scb(cb)
    cs2shcb(pick)
    
    s2slw(2)
    for i in range(0,N):
        x = random.uniform(-1.0,1.0)
        y = random.uniform(-1.0,1.0)
        z = random.uniform(-1.0,1.0)
        s2sci(int(round(random.uniform(1,16))))
        s2pt1(x,y,z,1);
    
    s2disp(-1,1)


if __name__ == '__main__':
	main()

