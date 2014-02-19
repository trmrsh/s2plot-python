#!/usr/bin/env python
# thread.py
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
import os, threading
import numpy
from s2plot import *
from math import *

A = 0.5
ti = 0.0

class DispThread(threading.Thread):
    def run(self):
        global ti
        while True:
            ti = ti + 0.000001

def cb(t, k):
    global A, ti
    s2pt1(A*sin(3*t),A*cos(3*t),sin(t/2),4)

def main():
    s2opend("/S2MONO",0,[])
    s2swin(-1,1,-1,1,-1,1)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    cs2scb(cb)
    
    s2disp(-1,1)

if __name__ == '__main__':
    #backgroundDisp = DispThread()
    #backgroundDisp.start()
    main()
    #while True:
    #    pass
    
    
    

