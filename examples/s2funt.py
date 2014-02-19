#!/usr/bin/env python
# s2funt.py
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
from math import *
from s2plot import *

XLIMIT = 10.0
YLIMIT = 10.0
ZLIMIT = 50.0

def fx(t):
    return float(XLIMIT*sin(t))

def fy(t):
    return float(YLIMIT*t/(2*pi)*cos(t))

def fz(t):
    return float(20.0 + (ZLIMIT*t/(2*pi) + 4.0*sin(5*t)*sin(3*t))*0.5)

def gz(t):
    return fz(t) + 2.0

def fc(t):
    return t/(4*pi) + 0.5

def main():
    x1 = -XLIMIT; x2 = XLIMIT
    y1 = -YLIMIT; y2 = YLIMIT
    z1 = -ZLIMIT/4.0; z2 = ZLIMIT
    
    s2opendo("/s2mono")
    s2swin(x1,x2,y1,y2,z1,z2)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)

    s2slw(3)

    s2sci(2)
    s2funt(fx, fy, fz, 1000, -2*pi, 2*pi)
    s2icm("rainbow", 3000, 3600)
    s2scir(3000, 3600)
    s2funtc(fx, fy, gz, fc, 1000, -2*pi, 2*pi)
    s2show(1)

if __name__ == '__main__':
	main()

