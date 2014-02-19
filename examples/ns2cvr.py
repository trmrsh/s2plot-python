#!/usr/bin/env python
# ns2cvr.py
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

N = 5000

def cb(t, kc):
    """A dynamic callback"""
    global vid, x, y, z
    ds2dvr(vid, 1)		# Draw the volume render object
    if kc % 2 == 1:
        s2pt(N, x, y, z, 1)		# Plot the points on <space> press

def main():
    global volume, vid, x, y, z
    
    nx = ny = nz = 32
    x1 = -1.0; x2 = +1.0
    y1 = -1.0; y2 = +1.0
    z1 = -1.0; z2 = +1.0
    amb = {'r':0.8, 'g':0.8, 'b':0.8}   # ambient light
    
    tr = numpy.array([
            x1, (x2-x1)/(nx-1.0), 0               , 0,
            y1, 0               , (y2-y1)/(ny-1.0), 0,
            z1, 0               , 0               , (z2-z1)/(nz-1.0)
        ])
    
    dmin = 0.0			# Mininum data value in volume to plot
    dmax = 2.0			# Maximum data value in volume to plot
    amin = 0.0			# Minimum alpha channel value
    amax = 0.8			# Maximum alpha channel value
    trans = 't'			# Transparency type
    
    dx = tr[1]*0.5		# Offsets for window - voxels are pixel
    dy = tr[6]*0.5		# centred
    dz = tr[11]*0.5
    
    s2opendo("/S2MONO")
    s2swin(x1-dx,x2+dx, y1-dy,y2+dy, z1-dz,z2+dz)	# Set window coords
    s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0)		    # Draw coord box
    
    # Create N random (x,y,z) values
    x = numpy.random.uniform(0, nx, N)* tr[1] + tr[0]
    y = numpy.random.uniform(0, ny, N)* tr[6] + tr[4]
    z = numpy.random.uniform(0, nz, N)* tr[11] + tr[8]
    
    # Give a value to volume
    volume = 1.0 - numpy.random.uniform(0, 1, (nx, ny, nz))**2
    
    s2scir(1000,2000)                # Set colour range
    s2icm("mgreen",1000,2000)        # Install colour map
    # Create the volume render object
    vid = ns2cvr(volume, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1, 
	                            tr, trans, dmin, dmax, amin, amax)
    
    cs2scb(cb)                           # Install a dynamic callback
    
    #fprintf(stderr,"Press <space> to toggle showing data points\n");
    
    ss2srm(SHADE_FLAT);			        # Set shading type to FLAT
    ss2sl(amb, 0, None, None, 0)        # Ambient lighting only
    
    s2disp(-1, 1)


if __name__ == '__main__':
	main()

