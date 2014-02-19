#!/usr/bin/env python
# s2latexture.py
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
    s2opendo("/S2MONO")
    s2swin(-1.0,1.0, -1.0,1.0, -1.0,1.0)
    s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0) # Draw coord box
    
    latexcmd = "$f(x) = \Sigma^{\infty}_{i=0} a_i \sin(2\pi x)$"
    col = {'r': 1.0, 'g': 1.0, 'b': 1.0 }
    
    # Must have S2PLOT_LATEXBIN environment variable correctly set
    latexture = s2latexture(latexcmd)
    
    pos = [
        {'x': -1.0, 'y': 1/latexture['aspect'], 'z': 0.0},
        {'x': 1.0, 'y': 1/latexture['aspect'], 'z': 0.0},
        {'x': 1.0, 'y': -1/latexture['aspect'], 'z': 0.0},
        {'x': -1.0, 'y': -1/latexture['aspect'], 'z': 0.0}
    ]
    
    ns2vf4x(pos, col, latexture['texture_id'], 1.0, 'o')
    
    s2disp(-1, 1)
    

if __name__ == '__main__':
	main()

