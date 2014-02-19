# __init__.py
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

from _s2plot import *
__doc__ = _s2plot.__doc__

# Colours - based on PGPlot default settings
S2_PG_BLACK     = 0
S2_PG_WHITE     = 1
S2_PG_RED       = 2
S2_PG_GREEN     = 3
S2_PG_BLUE      = 4
S2_PG_CYAN      = 5
S2_PG_MAGENTA   = 6
S2_PG_YELLOW    = 7
S2_PG_ORANGE    = 8
S2_PG_DKGRAY    = 14
S2_PG_DKGREY    = 14
S2_PG_LTGRAY    = 15
S2_PG_LTGREY    = 15

# Rendering modes
WIREFRAME       = 0
SHADE_FLAT      = 1
SHADE_DIFFUSE   = 2	
SHADE_SPECULAR  = 3

# Projections
PERSPECTIVE     = 0
ORTHOGRAPHIC    = 1
FISHEYE         = 2

# Stereo types
NOSTEREO        = 0
ACTIVESTEREO    = 1
DUALSTEREO      = 2
ANAGLYPHSTEREO  = 3
TRIOSTEREO      = 4

# Dome types
NODOME      = 0
TRUNCBOTTOM = 1
TRUNCTOP    = 2
VSPHERICAL  = 3
HSPHERICAL  = 4
WARPMAP     = 5
MIRROR1     = 6
