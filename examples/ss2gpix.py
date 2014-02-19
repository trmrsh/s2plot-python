#!/usr/bin/env python
# ss2gpix.py
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

def cb(t, kc, static=[False]):
    try:
        col = {'r': 1, 'g': 1, 'b': 1} # Billboard texture colour
        pos = {'x': 0, 'y': 0, 'z': 0 } # Billboard texture location
        iS  = {'x': 0, 'y': 0, 'z': 0 } # Billboard stretch factor
        
        screen = None
        texture = None
        
        if not static[0]:
            screen = ss2gpix() # Capture the screen
            width = screen.shape[0]
            height = screen.shape[1]
            
            if width > height:
                w = height
                h = height
            else:
                w = width
                h = width
            
            dx = (width - w)/2
            dy = (height - h)/2
            
            iD = ss2ct(w, h) # Create a new texture
            
            texture = ss2gt(iD) # Get memory for this texture
            
            # numpy copy the screenshot data over to the texture's data array
            texture[:,:,0:3] = screen[dx:dx+w, dy:dy+h]
            #texture += 5
            
            ss2pt(iD, texture) # Restore the texture for use
        else:
            iD = static[0]
        
        if kc % 2 == 1:
            static[0] = iD
        else:
            static[0] = False
        
        ss2tsc("c") # Change to screen coordinates
        xmin = 0.01; xmax = 0.99
        ymin = 0.01; ymax = 0.99
        
        ar = ss2qar() # Get screen aspect ratio
        if ar > 1.0: # Fiddle coordinates to get
            xmin = 0.5-(0.49)/ar # square region in window centre
            xmax = 0.5+(0.49)/ar
        else:
            ymin = 0.5-(0.49)*ar
            ymax = 0.5+(0.49)*ar
        # Draw square with thick lines
        ns2thline(xmin,ymin,0.01, xmax,ymin,0.01, 1,0,0, 3)
        ns2thline(xmin,ymax,0.01, xmax,ymax,0.01, 0,1,0, 3)
        ns2thline(xmin,ymin,0.01, xmin,ymax,0.01, 0,0,1, 3)
        ns2thline(xmax,ymin,0.01, xmax,ymax,0.01, 1,0,1, 3)
        ss2tsc("") # Return to world coordinates
        
        ds2vbb(pos, iS, 0.1, col, iD, 1.0, 'o')# Draw billboard texture
        
        del screen
        del texture
    except Exception, e:
        print e

def main():
    print "Use +/- to stare infinity in the face...\n"
    
    s2opendo("/s2mono")
    s2swin(-1.,1., -1.,1., -1.,1.)
    #s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    s2pt1(0, 0, 1, 6)
    
    cs2scb(cb)
    
    s2show(1)

if __name__ == '__main__':
    main()

