#!/usr/bin/env python
# ns2cis.py
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
import numpy
from s2plot import *

CELLS = 20

def colourfn(x, y, z):
    """Isosurface colouring function"""
    r = 0.2 + 0.5 * x/ (CELLS - 1)
    g = 0.0 + 0.8 * y / (CELLS - 1)
    b = 0.6 + 0.2 * z / (CELLS - 1)
    return {'r': r, 'g': g, 'b': b}

def cb(t, k):
    ns2sisl(dyId1, 2*(k%2 - 0.5)*(t%5)/5.0)
    ns2sisa(dyId1, ((k+5)%10)*0.1, 't')
    if k > 0:
        ns2sisc(dyId2, 0.1*(k%10), 1.0-0.1*(k%10), 0.5+0.1*(k%10))
    ns2dis(dyId1, 0)
    ns2dis(dyId2, 0)

def main():
    nx = CELLS
    ny = CELLS
    nz = CELLS
    
    s2opendo("/s2mono")
    s2swin(0, nx-1, 0, ny-1, 0, nz-1)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    # generate the data grid
    dx = 1.0/nx
    dy = 1.0/ny
    dz = 1.0/nz
    
    grid = numpy.array([[[i**3 + j**2 - k**4 for k in numpy.arange(0,1,dz)]\
        for j in numpy.arange(0,1,dy)] for i in numpy.arange(0,1,dx)])
    
    level       = 0.2	# Set the isosurface level
    resolution  = 1	    # Set the isosurface resolution
    alpha       = 0.5 	# Set the alpha channel
    trans       = 'o' 	# Opaque isosurface

    # Create the isosurface object
    staticId = ns2cis(grid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
        None, level, resolution, trans, alpha, 1.0, 1.0, 0.0)

    level      = 0.7	# Set the isosurface level
    resolution = 1		# Set the isosurface resolution
    alpha      = 0.5	# Set the alpha channel
    trans      = 's'	# Opaque isosurface

    # Create the isosurface object
    global dyId1
    dyId1 = ns2cisc(grid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
        None, level, resolution, trans, alpha, colourfn)
    
    # Create a third iso-object
    sphericalGrid = numpy.array([[[(i-0.5)**2+(j-0.5)**2+(k-0.5)**2 for k in numpy.arange(0,1,dz)]\
        for j in numpy.arange(0,1,dy)] for i in numpy.arange(0,1,dx)])
    level = 0.1
    resolution = 1
    alpha = 0.5
    trans = 's'
    global dyId2
    dyId2 = ns2cis(sphericalGrid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
        None, level, resolution, trans, alpha, 0, 1, 0)

    ns2dis(staticId, 0)          # Draw it

    cs2scb(cb)
    
    s2disp(-1, 1)

if __name__ == '__main__':
	main()

