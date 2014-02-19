#!/usr/bin/env python
# ds2vbb.py
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
import os, random
import numpy
from s2plot import *


def cb(t, kc):
    global tid
    
    try:
        xyz = {"x":0.0, "y":0.3, "z":0.4}
        isize = 0.08
        stri = {"x":0.0, "y":0.0, "z":0.0}
        ioff = {"x":0.01, "y":0.01, "z":0.0}
        col1 = {"r":1.0, "g":1.0, "b":0.0}
        col2 = {"r":0.5, "g":0.5, "b":1.0}
        alpha = 0.9
        trans = 's'
    
        ds2vbb(xyz, stri, isize, col1, tid, alpha, trans)
        ds2vbbr(xyz, stri, 2*isize, 30.0, col2, tid, alpha, trans)
        s2sch(0.2)
        s2sci(1)
        ds2vtb(xyz, ioff, "text", 1);
    except Exception, e:
        print e


def main():
    global tid
    s2opendo("/S2MONO")
    s2svp(-1.0,1.0, -1.0,1.0, -1.0,1.0)
    s2swin(-1.0,1.0, -1.0,1.0, -1.0,1.0)
    s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0)
    
    tid = ss2lt("firetile2_pow2_rgb.tga")
    tex = ss2gt(tid)
    
    # kill all but the red, make 1/2 transparent
    tex[:,:,1:4] = 0,0,127
    
    ss2pt(tid,tex)
    
    cs2scb(cb)
    
    s2disp(-1,1)

if __name__ == '__main__':
	main()

