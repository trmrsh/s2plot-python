#!/usr/bin/env python
# s2vect3.py
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
if os.environ.has_key('S2PATH') and os.environ.has_key('S2ARCH'):
    s2path = os.path.join(os.environ['S2PATH'], 'python', os.environ['S2ARCH'])
    sys.path.append(s2path)
from s2plot import *

XLIMIT = 1.0
YLIMIT = 1.0
ZLIMIT = 1.0
NX = 11
NY = 11
NZ = 11

def main():
    x1 = -XLIMIT; x2 = XLIMIT
    y1 = -YLIMIT; y2 = YLIMIT
    z1 = -ZLIMIT; z2 = ZLIMIT
    
    s2opendo("/s2mono")
    s2swin(x1,x2, y1,y2, z1,z2)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    dtype = numpy.float
    a = numpy.array([[[random.uniform(-1.0,1.0) for k in range(0,NZ)] for j in range(0,NY)] for i in range(0,NX)], dtype=dtype)
    b = numpy.array([[[random.uniform(-1.0,1.0) for k in range(0,NZ)] for j in range(0,NY)] for i in range(0,NX)], dtype=dtype)
    c = numpy.array([[[random.uniform(-1.0,1.0) for k in range(0,NZ)] for j in range(0,NY)] for i in range(0,NX)], dtype=dtype)
    
    minlength = numpy.sqrt((a*a+b*b+c*c).min())
    maxlength = numpy.sqrt((a*a+b*b+c*c).max())
    
    tr = numpy.array([
            x1, 1.0*(x2-x1)/((NX-1)*(NX != 1) + (NX == 1)), 0.0, 0.0,
            y1, 0.0, 1.0*(y2-y1)/((NY-1)*(NY != 1) + (NY == 1)), 0.0,
            z1, 0.0, 0.0, 1.0*(z2-z1)/((NZ-1)*(NZ != 1) + (NZ == 1))
        ])
    
    s2icm("rainbow", 1000, 1500)
    s2scir(1000,1500)
    
    s2slw(2)
    s2sch(2)
    s2sah(1, 30.0, 0.4)
    s2lab("X-axis","Y-axis","Z-axis","Plot")
    s2iden("PY2PLOT is great!")
    scale = 0.2
    nc = 0
    colbylength = 1
    mindraw = -1.0
    
    s2vect3(a,b,c, NX,NY,NZ, 0,NX-1, 0,NY-1, 0,NZ-1,\
    		scale, nc, tr, mindraw, colbylength, minlength, maxlength);
    
    s2disp(-1, 1)

if __name__ == '__main__':
	main()
    

