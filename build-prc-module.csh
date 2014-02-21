#!/bin/csh -f
## build-prc-module.csh
 #
 # Copyright 2006-2012 David G. Barnes, Paul Bourke, Christopher Fluke
 #
 # This file is part of S2PLOT.
 #
 # S2PLOT is free software: you can redistribute it and/or modify it
 # under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # S2PLOT is distributed in the hope that it will be useful, but
 # WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with S2PLOT.  If not, see <http://www.gnu.org/licenses/>. 
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
 # $Id: build-prc-module.csh 5798 2012-10-09 02:22:00Z dbarnes $
 #

echo
echo ====================================================================
echo S2PLOT PRC export module
echo ====================================================================
echo

if (!(${?S2PATH})) then
  echo "S2PATH environment variable MUST be set ... please fix and retry."
  exit(-1);
endif
if (! -d $S2PATH || ! -e ${S2PATH}/scripts/s2plot.csh) then
  echo "S2PATH is set but invalid: ${S2PATH} ... please fix and retry."
  exit(-1);
endif
source ${S2PATH}/scripts/s2plot.csh
if ($status) then
  exit(-1)
endif

if (!(${?S2PLOT_PRCDRIVER})) then
  echo "S2PLOT_PRCDRIVER environment variable MUST be set ... please fix and retry."
  echo " [recommended value: s2prcwriter]"
  exit(-1);
endif

set thisdir=`pwd`
if ($thisdir != $S2PATH) then
  echo "You must be in directory ${S2PATH} to build this module."
  exit(-1);
endif

if (! -d ${S2PATH}/${S2KERNEL}) then
  echo "Directory ${S2PATH}/${S2KERNEL} does not exist!  Cannot build!  Try running"
  echo "build-lib.csh script first, or re-installing your distribution."
  exit(-1);
endif

if (! -e ${S2PATH}/${S2KERNEL}/${S2LIBNAME}) then
  echo "Library ${S2LIBNAME} is missing!  Cannot build!  Try running "
  echo "build-lib.csh script first, or re-installing your distribution."
  exit(-1);
endif

if ($1 != "-n") then
  echo "Building PRC support code ..."
  cd $S2PATH/s2prc/newprc
  make clean
  if ($S2OSTYPE == linux) then
    make CFLAGS="$BASEFLAGS -fPIC -DS2LINUX"
  else
    make CFLAGS="$BASEFLAGS"
  endif
endif

cd $S2PATH/s2prc
echo "Compiling PRC module ..."
$S2MODCMPLR -I./newprc -I${S2HARUDIR}/include s2prc.cc

echo "Linking PRC module ..."
#$S2MODMAKER -o ${S2PLOT_PRCDRIVER}.so s2prc.o ./newprc/*.o -L${S2HARUDIR}/lib${S2LBITS} -lhpdf -lz

$S2MODMAKER -o ${S2PLOT_PRCDRIVER}.so s2prc.o ./newprc/*.o \
-L${S2PATH}/${S2KERNEL} -ls2plot -L${S2HARUDIR}/lib${S2LBITS} -lhpdf -lz \
-lstdc++

echo "Copying module to $S2PATH/$S2ARCH ..."
/bin/mv -f ${S2PLOT_PRCDRIVER}.so ${S2PATH}/${S2KERNEL}

if ($S2KERNEL == darwin && $S2SHARED == yes) then
  echo "Modifying dynamic library path ..."
  install_name_tool -change ${S2PATH}/${S2KERNEL}/${S2LIBNAME} @executable_path/${S2LIBNAME} ${S2PATH}/${S2KERNEL}/${S2PLOT_PRCDRIVER}.so
endif

#echo "Copying support JavaScript to $S2PATH/ ..."
#/bin/cp -f s2plot-prc.js $S2PATH


echo "Removing objects ..."
/bin/rm s2prc.o

echo "Cleaning up ..."
cd newprc
make clean
rm -f s2direct.map s2direct.prc s2plotprc.pdf
cd ..
rm -f s2direct.map s2direct.prc s2plotprc.pdf

echo Done!
