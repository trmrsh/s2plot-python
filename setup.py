"""
# setup.py
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
# Three-dimensional visualisation was conducted with the S2PLOT
# progamming library
#
# and a reference to 
#
# D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
# of the Astronomical Society of Australia, 23(2), 82-93.
#
# original version: Nick Jones, October 2007.
"""

import os, sys
from distutils.core import setup, Extension

libPath = []
libraries = ['s2plot']

def main():
    try:
        from numpy.distutils.misc_util  import get_numpy_include_dirs
    except:
        print "S2PLOT requires the NUMPY package.  Unable to load numpy."
        return -1
    includeDirs = get_numpy_include_dirs()
    
    if 'linux' in sys.platform.lower():
        dylibType = 'LD_LIBRARY_PATH'
        dylibExt = '.so'
        libraries.extend(['s2winglut', 's2dispstub', 's2meshstub', 'glut', 'GLU', 'GL'])
    elif 'darwin' in sys.platform.lower():
        dylibType = 'DYLD_LIBRARY_PATH'
        dylibExt = '.dylib'
        libraries.extend(['s2meshstub', 's2winglut'])
    else:
        print "Platform %s unsupported.  Exiting."
        return -1
    
    if any([cmd in ['install','build','build_ext'] for cmd in sys.argv]):
        # if we can't guess that s2plot is accounted for...
        # determine the s2plot environment
        if not os.environ.has_key('S2PATH') or not os.environ.has_key('S2ARCH'):
            print "The python S2PLOT module requires a current installation of S2PLOT."
            print "Unable to guess the location of your S2PLOT installation..."
            print "Enter your S2PATH directory (e.g. /usr/local/s2plot/): ",
            s2path = sys.stdin.readline().strip()
            print "\nEnter your S2ARCH architecture (e.g. darwin-gnu-ppc): ",
            s2arch = sys.stdin.readline().strip()
            print ""
        else:
            s2path = os.environ['S2PATH']
            s2arch = os.environ['S2ARCH']
        includeDirs.append(s2path)
        dylibPath = os.path.join(s2path,s2arch)
        dylibFile = os.path.join(dylibPath,'libs2plot'+dylibExt)
        if not os.path.exists(dylibFile):
            print "Unable to find %s.  Exiting." % dylibFile
            return -1
        libPath.append(dylibPath)
    else:
        print "To run the s2plot module, ensure that the s2plot libraries are installed and are in your dynamic library paths."
        print "You may need to set %s to point to your s2plot library directories." % dylibType
    
    
    s2plot_ext = Extension('_s2plot',
        sources = [os.path.join('src','_s2plot.c')],
        include_dirs = includeDirs,
        libraries = libraries,
        library_dirs = libPath)
    
    setup(name = 's2plot-python',
            url = 'http://astronomy.swin.edu.au/s2plot/',
            author = 'Nicholas Jones',
            author_email = 'njones@astro.swin.edu.au',
            version = '0.4',
            description = 'A python wrapper of the s2plot 3D stereo plotting library',
            platforms = ['any'],
            license = 'http://www.gnu.org/copyleft/gpl.html',
            long_description = __doc__,
            packages = ['s2plot'],
            package_dir = {'s2plot': 'src'},
            ext_package='s2plot',
            ext_modules = [s2plot_ext]
        )

if __name__ == '__main__':
    main()
