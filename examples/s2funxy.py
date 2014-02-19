#!/usr/bin/env python
# s2funxy.py
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

XLIMIT = 1.0
YLIMIT = 1.0
ZLIMIT = 1.0

def fxy(x,y):
   return (x**2 + 0.25*y**2)

def fxz(x, z):
   return (0.1*x**2 + 0.1*z**2)

def fyz(y, z):
   return (-0.3*y**2 - 0.3*z**2)

def main():
    nx = 64; ny = 64; nz = 64
    ctl = 0
    
    x1 = -XLIMIT*1.5; x2 = +XLIMIT*1.5
    y1 = -YLIMIT*1.5; y2 = +YLIMIT*1.5
    z1 = -ZLIMIT*1.5; z2 = +ZLIMIT*1.5
    
    s2opendo("/s2mono")
    s2swin(x1,x2,y1,y2,z1,z2)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    s2icm("rainbow",1000,2000)
    s2scir(1000,2000)
    s2funxy(fxy, nx, ny, -XLIMIT,XLIMIT, -YLIMIT,YLIMIT, ctl)
    
    s2icm("astro",2000,3000);
    s2scir(2000,3000);
    s2funxz(fxz, nx, nz, -XLIMIT,XLIMIT, -ZLIMIT,ZLIMIT, ctl);
    
    s2icm("mgreen",3000,4000);
    s2scir(3000,4000);
    s2funyz(fyz, ny, nz, -YLIMIT,YLIMIT, -ZLIMIT,ZLIMIT, ctl);
    
    s2disp(-1,1)

if __name__ == '__main__':
	main()

