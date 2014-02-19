#!/usr/bin/env python
# ns2v3a.py
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
    vertexA = []
    vertexB = []
    colA = {'r': 1.0, 'g': 1.0, 'b': 0.1 }
    colB = {'r': 1.0, 'g': 0.0, 'b': 0.9 }
    trans = 's'
    alphaA = 0.3
    alphaB = 0.8
    
    randoms = numpy.random.uniform(-1.0, 1.0, 18)
    for i in range(3):
        vertexA.append({'x': randoms[6*i], 'y': randoms[6*i+1], 'z': randoms[i+2]})
        vertexB.append({'x': randoms[6*i+3], 'y': randoms[6*i+4], 'z': randoms[6*i+5]})
    
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)

    ns2vf3a(vertexA, colA, trans, alphaA)
    ns2vf3a(vertexB, colB, trans, alphaB)

    # draw a thick dot at the origin
    ns2vpa({'x':0.0, 'y':0.0, 'z':0.0}, colA, 10.0, trans, 0.0)
    
    s2disp(-1,1)


if __name__ == '__main__':
	main()

