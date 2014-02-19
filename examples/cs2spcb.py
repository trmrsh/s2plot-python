#!/usr/bin/env python
# cs2spcb.py
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

# GLOBALS - Background Colour
r = 0.0
g = 0.0
b = 0.0

def pcb(txt, i=None):
    try:
        global r, g, b
        print 'got to the pcb with', txt, i
        rgb = txt.split(' ') # prompt text should be 'r g b'
        if len(rgb) >= 3:
            r = float(rgb[0])
            g = float(rgb[1])
            b = float(rgb[2])
    except Exception, e:
        print 'pcb failed with', e

def cb(t, kc):
    global r, g, b
    try:
        ss2sbc(r, g, b) # Set background colour
        ss2sfc(1-r, 1-g, 1-b) # Choose sensible foreground
        s2scr(32, 1-r, 1-g, 1-b) # Create a new colour index
        s2sci(32) # Set colour to this index
        s2textxy(-1, 0, 0, "Press ~ then enter R G B values e.g 1 1 1") # Write some text
    except Exception, e:
        print 'cb failed with', e

def main():
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    cs2scb(cb)
    
    cs2spcb(pcb)
    
    s2show(1)
    

if __name__ == '__main__':
	main()

