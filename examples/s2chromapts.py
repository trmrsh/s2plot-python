#!/usr/bin/env python
# s2chromapts.py
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
import numpy, math
from s2plot import *


def main():
    NPTS = 500
    D2R = (math.pi / 180.)
    
    pos  = {'x': 0.,'y': 0.,'z': -0.1}
    up   = {'x': 0.,'y': 1.,'z': 0.}
    view = {'x': 0.,'y': 0.,'z': 1.}
    
    print "This demonstration appears in 3D when viewed with "
    print "Chromadepth(TM) glasses\n"
    
    s2opendo("/S2FISH")
    s2swin(-1.,1., -1.,1., -1.,1.)
    
    ss2sc(pos, up, view, 0)
    ss2tc(0)
    
    nentries = ss2lcm("chromapal.txt", 100, 2000)
    s2scir(100, 100+nentries-1)
    
    a = numpy.random.uniform(0.,360.,NPTS)
    b = numpy.arcsin(numpy.random.uniform(-1,1,NPTS))/D2R
    dist = numpy.sqrt(numpy.random.uniform(0,100,NPTS))
    size = 11.0*numpy.ones(NPTS) - 0.8*dist
    
    s2chromapts(NPTS, a, b, dist, size, 1., 0., 10.);
    
    s2disp(-1,1);

if __name__ == '__main__':
	main()

