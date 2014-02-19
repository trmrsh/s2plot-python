#!/usr/bin/env python
# s2pt.py
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


def main():
    N = 50
    M = 10
    symbol = 2
    
    x = numpy.array([random.uniform(-1,1) for i in range(0,N)], dtype=numpy.float32)
    y = numpy.array([random.uniform(-1,1) for i in range(0,N)], dtype=numpy.float32)
    z = numpy.array([random.uniform(-1,1) for i in range(0,N)], dtype=numpy.float32)
    
    xl = numpy.array([random.uniform(-1,1) for i in range(0,M)], dtype=numpy.float32)
    yl = numpy.array([random.uniform(-1,1) for i in range(0,M)], dtype=numpy.float32)
    zl = numpy.array([random.uniform(-1,1) for i in range(0,M)], dtype=numpy.float32)
    
    s2opend("/s2mono",0, [])
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    s2sch(2)
    s2pt(N,x,y,z,symbol)
    symbols = numpy.array([int(round(random.uniform(0,6))) for i in range(0,N)])
    print symbols.__class__, symbols
    s2pnts(N,x,y,z,symbols,N)
    s2slw(4)
    s2line(M, xl, yl, zl)
    
    s2circxy(0,0,-1, 0.5,100,1.0)
    s2diskxz(0,-1,0,0.5,0.9)
    s2rectyz(-0.5,0.5,-0.5,0.5,-1)
    s2textxy(0,0,1,"XY")
    s2textxz(0,0,1,"XZ")
    s2textyz(0,0,1,"YZ")
    xb,yb =  s2qtxtxy(0,0,1,"XY",0.1)
    s2rectxy(xb[0],xb[1],yb[0],yb[1],0.9)
    print s2qtxtxz(0,0,1,"XZ",0)
    print s2qtxtyz(0,0,1,"YZ",0)
    s2textyzf(1,0,0,0.5,3,"YZ")
    print s2qtxtyzf(1,0,0,0.5,3,"YZ",0)
    
    s2disp(-1,1)

if __name__ == '__main__':
	main()

