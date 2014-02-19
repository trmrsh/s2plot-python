#!/usr/bin/env python
# s2chromacpts.py
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

NPOINTS = 500

def cb(t, k, x = [], y = [], z = []):
    if len(x) == 0:
        x = numpy.random.uniform(-1,1,NPOINTS)
        y = numpy.random.uniform(-1,1,NPOINTS)
        z = numpy.random.uniform(-1,1,NPOINTS)
    
    pos, up, view = ss2qc(1)
    
    dist = numpy.sqrt(\
        (x-pos['x']*numpy.ones(NPOINTS))**2 + \
        (y-pos['y']*numpy.ones(NPOINTS))**2 + \
        (z-pos['z']*numpy.ones(NPOINTS))**2\
    )
    dmin = dist.min()
    dmax = dist.max()
    
    size = 8.0*numpy.ones(NPOINTS) - (dist - dmin*numpy.ones(NPOINTS))/(dmax-dmin) * 6.0
    
    s2chromacpts(NPOINTS, x, y, z, dist, size, dmin, dmax);

def main():
    print "This demonstration appears in 3D when viewed with "
    print "Chromadepth(TM) glasses\n"
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    nentries = ss2lcm("chromapal.txt", 100, 2000);
    s2scir(100, 100+nentries-1)
    
    cs2scb(cb)
    
    s2show(1)

if __name__ == '__main__':
	main()

