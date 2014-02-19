#!/usr/bin/env python
# s2sah.py
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
    x = [-0.3, 0.5]
    y = [0.2, 0.1]
    z = [-0.4, 0.8]
    fs = 1
    angle = 30.0
    barb = 0.7

    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    s2sci(2);
    s2sah(fs, angle, barb)
    s2sch(5)
    s2arro(x[0],y[0],z[0], x[1],y[1],z[1])
    s2disp(-1,1)

if __name__ == '__main__':
	main()

