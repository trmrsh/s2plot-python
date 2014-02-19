#!/usr/bin/env python
# cs2scb.py
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

from s2plot import *
import numpy, random

# Spacebar: toggle between two different colours 
# Press 1:  to turn off callback 
# Press 2:  to turn on callback 
# Press 3:  to select a different colour set to toggle between 
# Press 4:  to select a different colour set to toggle between
# Press 5:  toggle callback - same as pressing 1 then 2 

colour = 1

def ncb(N):
    global colour
    print 'The cursor is showing: ', ss2qxh()
    if N == 1:
        cs2dcb()
    elif N == 2:
        cs2ecb()
    elif N == 3:
        colour = 3
    elif N == 4:
        colour = 5
    elif N == 5:
        cs2tcb()

def kcb(key):
    try:
        print "You pressed '%s'" % str(key)
    except Exception, e:
        print e
    return 0

def cb(t, kc, x=[], y=[], z=[]):
    """A dynamic callback - jitter particle positions each time through"""
    global colour
    N = 20
    try:
        if len(x) == 0:
            for i in range(0,N):
                x.append(random.uniform(-1.0,1.0))
                y.append(random.uniform(-1.0,1.0))
                z.append(random.uniform(-1.0,1.0))
            
        for i in range(0,N):
            x[i] += random.uniform(-0.05,0.05)
            y[i] += random.uniform(-0.05,0.05)
            z[i] += random.uniform(-0.05,0.05)
        
        s2sci((kc % 2) + colour)
    
        s2slw(3)
        s2pt(N, numpy.array(x), numpy.array(z), numpy.array(y), 1)
    except Exception, e:
        print e

def main():
    N = 20
    
    x = numpy.array([random.uniform(-2.0,2.0) for i in range(0,N)])
    y = numpy.array([random.uniform(-2.0,2.0) for i in range(0,N)])
    z = numpy.array([random.uniform(-2.0,2.0) for i in range(0,N)])
    
    s2opend("/S2MONO",0, [])
    s2swin(-2.0,2.0, -2.0,2.0, -2.0,2.0)
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0)
    
    print 'current pane:', xs2qsp()
    cs2scb(cb)
    cs2sncb(ncb)
    cs2skcb(kcb)
    
    s2slw(4)
    s2pt(N,x,y,z,1)
    s2slw(1)
    
    s2disp(-1,1)

if __name__ == '__main__':
	main()

