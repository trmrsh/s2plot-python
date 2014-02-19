#!/usr/bin/env python
# ss2gt.py
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


#include <stdio.h>
#include <stdlib.h>
#include "s2plot.h"

def main():
    texture = "firetile2_pow2_rgb.tga"
    xyz = {'x': 0.5, 'y':0.0, 'z':0.0}
    r = 0.5
    col = {'r': 1.0, 'g':1.0, 'b':1.0}
    
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0, "BCDET",0,0, "BCDET",0,0)
    # crank sphere resolution
    ss2ssr(99)
    print "Current rendering mode: ", ss2qrm()
    ss2srm(3)
    print "Rendering mode reset to: ", ss2qrm()
    
    tex1 = ss2lt(texture)
    tex2 = ss2lt(texture)
    
    ns2spherex(-xyz['x'], xyz['y'], xyz['z'], r, col['r'], col['g'], col['b'], tex1)
    
    tex = ss2gt(tex2)
    print tex
    # turn off the red
    tex[:,:,0] = 0
    #flatten the blue
    tex[:,:,2] = 127
    # blacken the top third
    height = tex.shape[1]
    tex[:,2.0/3*height:,:] = 0
    
    # reload the texture
    ss2pt(tex2,tex)
    
    ns2vspherex(xyz, r, col, tex2)
    
    s2show(1)

if __name__ == '__main__':
	main()

