/* _s2plot.h
 *
 * Copyright 2008 Swinburne University of Technology.
 *
 * This file is part of the S2PLOT Python module.
 *
 * The S2PLOT Python module is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.  
 *
 * The S2PLOT Python module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the S2PLOT Python module.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * We would appreciate it if research outcomes using S2PLOT would
 * provide the following acknowledgement:
 *
 * "Three-dimensional visualisation was conducted with the S2PLOT
 * progamming library"
 *
 * and a reference to 
 *
 * D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
 * of the Astronomical Society of Australia, 23(2), 82-93.
 *
 * original version: Nick Jones, October 2007.
 */

#include "Python.h"
#include "numpy/arrayobject.h"
#include "s2plot.h"

#ifdef macintosh
#include "mymath.h"
#else
#include <math.h>
#endif

// helper functions
float   *numpy1D_to_float(PyArrayObject *);
int     *numpy1D_to_int(PyArrayObject *);
float  **numpy2D_to_float(PyArrayObject *);
float ***numpy3D_to_float(PyArrayObject *);

static PyMethodDef S2PlotMethods[];

// OPENING, CLOSING AND SELECTING DEVICES
static PyObject *s2plot_s2open(PyObject *self, PyObject *args);
static PyObject *s2plot_s2opend(PyObject *self, PyObject *args);
static PyObject *s2plot_s2opendo(PyObject *self, PyObject *args);
static PyObject *s2plot_s2ldev(PyObject *self, PyObject *args);
static PyObject *s2plot_s2show(PyObject *self, PyObject *args);
static PyObject *s2plot_s2disp(PyObject *self, PyObject *args);
static PyObject *s2plot_s2eras(PyObject *self, PyObject *args);
// WINDOWS AND VIEWPORTS
static PyObject *s2plot_s2svp(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qvp(PyObject *self, PyObject *args);
static PyObject *s2plot_s2swin(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qwin(PyObject *self, PyObject *args);
static PyObject *s2plot_s2env(PyObject *self, PyObject *args);
// PRIMITIVES
static PyObject *s2plot_s2line(PyObject *self, PyObject *args);
static PyObject *s2plot_s2circxy(PyObject *self, PyObject *args);
static PyObject *s2plot_s2circxz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2circyz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2diskxy(PyObject *self, PyObject *args);
static PyObject *s2plot_s2diskxz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2diskyz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2rectxy(PyObject *self, PyObject *args);
static PyObject *s2plot_s2rectxz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2rectyz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2wcube(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_s2pt1(PyObject *self, PyObject *args);
static PyObject *s2plot_s2pt(PyObject *self, PyObject *args);
static PyObject *s2plot_s2pnts(PyObject *self, PyObject *args);
static PyObject *s2plot_s2textxy(PyObject *self, PyObject *args);
static PyObject *s2plot_s2textxz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2textyz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2textxyf(PyObject *self, PyObject *args);
static PyObject *s2plot_s2textxzf(PyObject *self, PyObject *args);
static PyObject *s2plot_s2textyzf(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qtxtxy(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qtxtxz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qtxtyz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qtxtxyf(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qtxtxzf(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qtxtyzf(PyObject *self, PyObject *args);
static PyObject *s2plot_s2arro(PyObject *self, PyObject *args);
// ATTRIBUTES
static PyObject *s2plot_s2sci(PyObject *self, PyObject *args);
static PyObject *s2plot_s2scr(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qcr(PyObject *self, PyObject *args);
static PyObject *s2plot_s2slw(PyObject *self, PyObject *args);
static PyObject *s2plot_s2sls(PyObject *self, PyObject *args);
static PyObject *s2plot_s2sch(PyObject *self, PyObject *args);
static PyObject *s2plot_s2sah(PyObject *self, PyObject *args);
// AXES, BOXES & LABELS
static PyObject *s2plot_s2box(PyObject *self, PyObject *args);
static PyObject *s2plot_s2lab(PyObject *self, PyObject *args);
static PyObject *s2plot_s2help(PyObject *self, PyObject *args); /* NEW */
// XY(Z) PLOTS
static PyObject *s2plot_s2errb(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funt(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funtc(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funxy(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funxz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funyz(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funxyr(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funxzr(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funyzr(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funuv(PyObject *self, PyObject *args);
static PyObject *s2plot_s2funuva(PyObject *self, PyObject *args);
// IMAGES/SURFACES
static PyObject *s2plot_s2surp(PyObject *self, PyObject *args);
static PyObject *s2plot_s2surpa(PyObject *self, PyObject *args);
static PyObject *s2plot_s2scir(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qcir(PyObject *self, PyObject *args);
static PyObject *s2plot_s2icm(PyObject *self, PyObject *args);
// VECTOR PLOTS
static PyObject *s2plot_s2vect3(PyObject *self, PyObject *args);
// MISCELLANEOUS ROUTINES
static PyObject *s2plot_s2iden(PyObject *self, PyObject *args);
// S2 "NATIVE" ROUTINES
static PyObject *s2plot_ns2sphere(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vsphere(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2spheret(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vspheret(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2spherex(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vspherex(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vplanett(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2vplanetx(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2disk(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vdisk(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2arc(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2varc(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2erc(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2verc(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2text(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vtext(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2point(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vpoint(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vnpoint(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2thpoint(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vthpoint(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2i(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vi(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2line(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vline(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2thline(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vthline(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2thwcube(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2vthwcube(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2cline(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vcline(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2thcline(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2vthcline(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2vf3(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf3n(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf3c(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf3nc(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4n(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4c(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4nc(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4t(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4x(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf4xt(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2scube(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2vscube(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ns2m(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vm(PyObject *self, PyObject *args);
// DYNAMIC-ONLY GEOMETRY FUNCTIONS
static PyObject *s2plot_ds2bb(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2vbb(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2vbbr(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2vbbp(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ds2tb(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2vtb(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2protect(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2unprotect(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2isprotected(PyObject *self, PyObject *args);
// CALLBACK AND HANDLE SYSTEM
static PyObject *s2plot_cs2scb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2ecb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2dcb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2tcb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2skcb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2sncb(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2ah(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2shcb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2th(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2qhv(PyObject *self, PyObject *args);
// ADVANCED TEXTURE AND COLORMAP HANDLING
static PyObject *s2plot_ss2lt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2gt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2pt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2ptt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2lcm(PyObject *self, PyObject *args);
// ENVIRONMENT AND RENDERING ATTRIBUTES
static PyObject *s2plot_ss2ssr(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2srm(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qrm(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sl(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sbc(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sfra(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qpt(PyObject *self, PyObject *args);
// ADVANCED CAMERA CONTROL
static PyObject *s2plot_ss2sc(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qc(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sas(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2scf(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2ucf(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sca(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ss2qca(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ss2sss(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qss(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2scs(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ss2qcs(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ss2tc(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qess(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sess(PyObject *self, PyObject *args);
// ADDITIONAL PRIMITIVES
static PyObject *s2plot_s2chromapts(PyObject *self, PyObject *args);
static PyObject *s2plot_s2chromacpts(PyObject *self, PyObject *args);
// FUNCTIONS IN TESTING/DEVELOPMENT
static PyObject *s2plot_ss2ltt(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_s2latexture(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vf3a(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2vpa(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2cis(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2cisc(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2dis(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2sisl(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2sisa(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2sisc(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2cvr(PyObject *self, PyObject *args);
static PyObject *s2plot_ds2dvr(PyObject *self, PyObject *args);
static PyObject *s2plot_ns2svrl(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2ct(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2ctt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2dt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2txh(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qxh(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2thv(PyObject *self, PyObject *args);
static PyObject *s2plot_s2skypa(PyObject *self, PyObject *args);
static PyObject *s2plot_s2impa(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2spt(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2sfc(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2ap(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2tp(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2cp(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2mp(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2lpc(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2qpa(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2qcpa(PyObject *self, PyObject *args);
static PyObject *s2plot_xs2spp(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_xs2qsp(PyObject *self, PyObject *args);
//static PyObject *s2plot_xs2lpg(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2tsc(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qci(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qlw(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qls(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qch(PyObject *self, PyObject *args);
static PyObject *s2plot_s2qah(PyObject *self, PyObject *args);
static PyObject *s2plot_s2twc(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_s2qwc(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ss2qsr(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qbc(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qfc(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qfra(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qas(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qcf(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qar(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qsa(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qnfp(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2scbx(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2qsd(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2sdhcb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2spcb(PyObject *self, PyObject *args);
static PyObject *s2plot_cs2sptxy(PyObject *self, PyObject *args); /* NEW */
static PyObject *s2plot_ds2ahx(PyObject *self, PyObject *args);
//static PyObject *s2plot_font2tex(PyObject *self, PyObject *args);
static PyObject *s2plot_pushVRMLname(PyObject *self, PyObject *args);
//static PyObject *s2plot_s2show_thr(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2wtga(PyObject *self, PyObject *args);
static PyObject *s2plot_ss2gpix(PyObject *self, PyObject *args);
