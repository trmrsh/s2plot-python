#!/usr/bin/env python
# cs2scbx.py
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

# Global variales used in callback
master_panel = 0 # Default ID of main panel

def cb(t, kc, value=777):
    message = "I am panel: %d"  % value # Data passed to callback
    s2textxy(0, 0, 0, message)

def main():
    global master_panel, slave_panel
    
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    xs2mp(master_panel, 0.0, 0.5, 0.5, 1.0) # Move to top left
    
    slave_panel = xs2ap(0.5, 0.0, 1.0, 0.5) # Create panel in bottom right
    xs2cp(slave_panel) # Choose this panel
    s2swin(-2.,2., -2.,2., -2.,2.) # Set the window coordinates
    s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0) # Draw coordinate box
    cs2scbx(cb, slave_panel) # Dynamic callback with data
    
    xs2cp(master_panel) # Go back to main panel
    cs2scbx(cb, master_panel) # Dynamic callback with data
    
    s2sch(3) #  Set character height
    
    s2show(1)

if __name__ == '__main__':
    main()

