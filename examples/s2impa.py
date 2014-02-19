#!/usr/bin/env python
# s2impa.py
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

XLIMIT = 10.0
YLIMIT = 10.0
ZLIMIT = 10.0

NX = 32
NY = 32

def main():
    x1 = -XLIMIT; x2 = XLIMIT
    y1 = -YLIMIT; y2 = YLIMIT
    z1 = -ZLIMIT; z2 = ZLIMIT
    
    walls = True
    lcolor = S2_PG_LTGREY
    fcolor = S2_PG_DKGREY
    trunk = 0
    symbol = 4
    
    s2opendo("/s2mono")
    
    dx = (x2-x1)/(NX)
    dy = (y2-y1)/(NY)
    
    s2swin(x1-0.5*dx,x2+0.5*dx,y1-0.5*dy,y2+0.5*dy,z1,z2)
    
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    image = [[x**2+y**2 for y in numpy.arange(y1,y2,dy)] for x in numpy.arange(x1,x2,dx)]
    image = -numpy.sqrt(image) + numpy.random.uniform(0.0, 1.0, (NX, NY))
    minz = image.min()
    maxz = image.max()
    
    s2icm("rainbow", 1000, 1500)
    s2scir(1000, 1500)
    
    # Set up the transformation mapping from grid to world coordinatee
    tr = numpy.array([
        x1, dx, 0.0, 0.0,
        y1, 0.0, dy, 0.0,
        z1, 0.0, 0.0, 0.0
    ])
    
    # Draw the data mapped to the z-plane
    s2surpa(image, NX, NY, 0, NX-1, 0, NY-1, minz, maxz, tr)
    
    tr[8] = z1 - minz * (z2 - z1) / (maxz - minz) # Fix up z minimum
    tr[11] = (z2 - z1) / (maxz - minz) # Fix up z range
    
    # Draw the data as impulses
    s2impa(image, NX, NY, 0, NX-1, 0, NY-1, minz, maxz, tr, trunk, symbol);
    
    print 'ss2qnfp says:', ss2qnfp()
    print 'ds2isprotected says:', ds2isprotected()
    ds2protect()
    print 'ds2isprotected says:', ds2isprotected()
    ds2unprotect()
    print 'ds2isprotected says:', ds2isprotected()
    
    s2disp(-1,1)

if __name__ == '__main__':
    main()
    

