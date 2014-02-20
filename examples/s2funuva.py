#!/usr/bin/env python
# s2funuv.py
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
# original version: Tom Marsh, February 2014



import sys
import os, random
import numpy
from math import *
from s2plot import *

LIMIT = 0.7

AMP = 0.4
NTW = 1.5
R1 = 2.0
R2 = 0.5

def tkx(u, v):
    return R2 * cos(v) * cos(u) + R1 * cos(u) * (1. + AMP * cos(u * NTW))

def tky(u, v):
    return R2 * sin(v) + AMP * sin(u * NTW)

def tkz(u, v):
    return R2 * cos(v) * sin(u) + R1 * sin(u) * (1. + AMP * cos(u * NTW))

def fcol(u, v):
    return 0.5 + sin(0.5 * u) * AMP * sin(v)

def falpha(u, v):
   if v < pi:
       return 0.0
   else:
       return 1.0

def main():
    x1 = -LIMIT; x2 = LIMIT
    y1 = -LIMIT; y2 = LIMIT
    z1 = -LIMIT; z2 = LIMIT
    trans = 't'

    s2opendo("/s2mono")
    s2swin(x1, x2, y1, y2, z1, z2)

    s2icm("rainbow", 100, 500)
    s2scir(100, 500)
    s2funuva(tkx, tky, tkz, fcol, trans, falpha, 0., 4.*pi, 320, 0., 2.*pi, 90)

    ss2sas(1)
    ss2ucf()
    ss2sss(1)

    s2disp(-1,1)

if __name__ == '__main__':
	main()


