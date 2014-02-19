#!/usr/bin/env python
# volrender.py
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
from numpy import *

BWIDTH = 40

def fGrid(i,j,k):
    global nx, ny, nz
    
    x = 1.0*(i - nx/2.0)/(nx - 1.0)
    y = 1.0*(j - ny/2.0)/(ny - 1.0)
    z = 1.0*(k - nz/2.0)/(nz - 1.0)
    f = x**3 + y**2 - z**4
    f += 8.*exp(-16. * x**2 - 18.*y**2 - 20.2 *z**2)
    
    # when all these conditions are met, add 6 to f: draw a box
    f += 6.0*(x > -0.3)*(x < 0.0)*(y > 0.2)*(y < 0.4)*(z > -0.3)*(z < -0.2)
    
    # highlight the edges of the volume
    f[0,0,:]        = 12.0
    f[0,ny-1,:]     = 12.0
    f[0,:,0]        = 12.0
    f[0,:,nz-1]     = 12.0
    f[nx-1,0,:]     = 12.0
    f[nx-1,ny-1,:]  = 12.0
    f[nx-1,:,0]     = 12.0
    f[nx-1,:,nz-1]  = 12.0
    f[:,0,0]        = 12.0
    f[:,0,nz-1]     = 12.0
    f[:,ny-1,0]     = 12.0
    f[:,ny-1,nz-1]  = 12.0
    
    return f

def main():
    global grid, nx, ny, nz, vrid, isoid
    
    nx = BWIDTH
    ny = BWIDTH
    nz = BWIDTH
    
    # transpose matrix for volume rendering and isosurface
    # offsets
    tr = zeros((12))
    tr[1] = tr[6] = tr[11] = 1.0
    
    s2opendo("/s2monof")
    
    # set up coordinate system
    s2svp(-0.7, 0.7, -0.7, 0.7, -0.7, 0.7)
    s2swin(0, nx-1, 0, ny-1, 0, nz-1)
    opt = "BCDETS"
    s2sci(S2_PG_CYAN)
    s2sch(0.5)
    s2slw(1)
    s2box(opt, 0.0, 0, opt, 0.0, 0, opt, 0.0, 0)
    
    grid = fromfunction(fGrid,(nx,ny,nz), dtype=float32)
    
    # create the volume rendering "object"
    vrid = ns2cvr(grid, nx, ny, nz,\
	    0, nx-1, 0, ny-1, 0, nz-1,\
		tr, 's', 0.6, 9., 0.0, 0.6)
    
    # create the isosurface "object"
    isoid = ns2cis(grid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
        tr, 0., 1, 't', 1.0, 0.3, 0.3, 0.3);
    
    cs2scb(callback)
    
    ss2srm(SHADE_FLAT)
    amb = {'r':0.8, 'g':0.8, 'b':0.8}
    ss2sl(amb, 0, None, None, 0)
    
    s2icm("rainbow", 2000, 3000)
    s2scir(2000, 3000)
    
    ss2txh(1)
    
    s2disp(-1,1)
    grid = zeros((nx,ny,nz))
    
    s2disp(-1,1)

def callback(t, key, static=[0,0,0]):
    """The dynamic geometry callback.  This must be as efficient as possible since
    its called once per frame: use numpy rather than python loops."""
    try:
        global grid, vrid, isoid, nx, ny, nz
        if static[0] is 0:
            # store these as static variables to speed things up
            i = zeros((2,nx,ny,nz), dtype=float32)
            j = zeros((2,nx,ny,nz), dtype=float32)
            k = zeros((2,nx,ny,nz), dtype=float32)
            
            for n in range(0,nx):
                i[0,n,:,:] = sin(2*pi*n/nx)
                i[1,n,:,:] = cos(2*pi*n/nx)
            for m in range(0,ny):
                j[0,:,m,:] = sin(2*pi*m/ny)
                j[1,:,m,:] = cos(2*pi*m/ny)
            for l in range(0,nz):
                k[0,:,:,l] = sin(2*pi*l/nz)
                k[1,:,:,l] = cos(2*pi*l/nz)
            static[0] = i
            static[1] = j
            static[2] = k
        
        s0t = sin(t);       c0t = cos(t)
        s1t = sin(0.9*t);   c1t = cos(0.9*t)
        s2t = sin(2.3*t);   c2t = cos(2.3*t)
        #            grid[i][j][k] += 0.03 * (s0t*static[0][1,i,j,k]+c0t*static[0][0,i,j,k])\
        #                + 0.4 * (c1t*static[1][1,i,j,k] - s1t*static[1][0,i,j,k])\
        #                - 0.02 * (s2t*static[2][1,i,j,k] + c2t*static[2][0,i,j,k])
        grid += 0.01 * (s0t*static[0][1]+c0t*static[0][0])\
            + 0.01 * (c1t*static[1][1] - s1t*static[1][0])\
            - 0.01 * (s2t*static[2][1] + c2t*static[2][0])
    
        #draw the volume rendering
        ds2dvr(vrid, 1)
    
        #update the isosurface level and draw the isosurface
        level = (key % 9) + 1. - 1.0e-5
        if level > 1:
            ns2sisl(isoid, 11. - level)
            ns2dis(isoid, 1)
    except Exception, e:
        print e

if __name__ == '__main__':
    main()

