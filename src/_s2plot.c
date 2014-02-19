/* _s2plot.c
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
#include "_s2plot.h"

#ifdef macintosh
#include <stdio.h>
#include "mymath.h"
#else
#include <math.h>
#endif

static PyMethodDef S2PlotMethods[] = {
    // OPENING, CLOSING AND SELECTING DEVICES
    {"s2open", s2plot_s2open, METH_VARARGS,"s2open(fullscreen, stereo, argc, argv)\n\nOpen the S2PLOT device. If fullscreen = 0, use windowed mode, else make best effort at going fullscreen. If stereo = 0, use mono view, else use stereo view. For stereo = 1, attempt active stereo mode, or for stereo = 2, attempt passive stereo mode. The commandline arguments are needed for the creation of GLUT contexts."},
    {"s2opend", s2plot_s2opend, METH_VARARGS,"s2opend(device, argc, argv)\n\nOpen the S2PLOT device (device string version). This is like the PGPlot open function, which allows for setting the device via the environment, via standard input when the program is run, or via an explicit setting in the code. Options are:\n\n        * /S2MONO - mono device, windowed\n        * /S2MONOF - mono device, full screen if possible\n        * /S2PASSV - passive stereo device, windowed (not for projection)\n        * /S2PASSVF - passive stereo device, full screen if possible\n        * /S2ACTIV - active stereo device, windowed\n        * /S2ACTIVF - active stereo device, full screen if possible\n        * /S2FISH - fisheye projection, windowed\n        * /S2FISHF - fisheye projection, full screen if possible\n        * /S2TRUNCB - truncated-base fisheye, windowed display\n        * /S2TRUNCBF - truncated-base fisheye, full screen if possible\n        * /S2TRUNCT - truncated-top fisheye, windowed display\n        * /S2TRUNCTF - truncated-top fisheye, full-screen if possible\n        * /S2ANA - anaglyph stereo (red(L), blue(R)), windowed\n        * /S2ANAF - anaglyph stereo (red(L), blue(R)), full screen if possible\n        * /S2DSANA - anaglyph stereo (red(L), blue(R)), pre-desaturated, windowed\n        * /S2DSANAF - anaglyph stereo (red(L), blue(R)), pre-desaturated, full-screen \n\n    If the device string is given as blank or empty, then the value of the environment variable S2PLOT_ENV will be used, and should be set to one of the above. If this value is not found, then the behaviour is as for \"/?\" described below.\n\n    If the device string is given as \"/?\" then the user will be prompted for their choice when the program is run. The default choice will be the value of S2PLOT_ENV if it contains a valid device."},
    {"s2opendo",s2plot_s2opendo, METH_VARARGS,"s2opendo(device)\n\nOpen the S2PLOT device (device string version, ignoring command line arguments)."},
    {"s2show", s2plot_s2show, METH_VARARGS,"s2show(interactive)\n\nDraw the scene and enter interactive mode if interactive is non-zero. This function never returns. If you need to regain control after displaying graphics, consider using s2disp."},
    {"s2disp", s2plot_s2disp, METH_VARARGS,"s2disp(idelay, irestorecamera)\n\nDraw the scene, but return control when a timeout occurs or when the user hits the 'TAB' key. In some distributions, this capability is not available - a warning will be issued and s2show will be called implicitly.\n\n    # If idelay = 0, the function returns immediately the event buffer is clear.\n    # If idelay > 0 the function will return after this many seconds or when key is pressed.\n    # If idelay < 0 there is no timeout, and the function returns when the 'TAB' key is pressed.\n\n    # If irestorecamera > 0, the camera will be returned to its \"home\" position, otherwise it is left in the current position."},
    {"s2ldev", s2plot_s2ldev, METH_VARARGS,"s2ldev()\n\nList the available S2PLOT devices on stdout."},
    {"s2eras", s2plot_s2eras, METH_VARARGS,"s2eras()\n\nErase the geometry. If called in the main program flow, this will erase all geometry. It is generally only used if you are using the s2disp(...) function to regain control after showing some geometry. If called from a callback function registered with cs2scb(...), this will erase the dynamic geometry, however this is generally unnecessary as the dynamic geometry is implicitly erased prior to the callback function being called! "},
    // WINDOWS AND VIEWPORTS
    {"s2svp", s2plot_s2svp, METH_VARARGS, "s2svp(x1, x2, y1, y2, z1, z2)\n\nSet up the region of 3D space that is used for plotting. This is the \"viewport\" where graphics are drawn. The default values are [-1,+1] for all coordinates. A \"fatal\" error will occur if x1 > x2 or y1 > y2 or z1 > z2."},
    {"s2qvp", s2plot_s2qvp, METH_VARARGS, "s2qvp()\n\nQuery the device viewport range.  Returns a tuple of (x0, x1, y0, y1, z0, z1)."},
    {"s2swin", s2plot_s2swin, METH_VARARGS,"s2swin(x1, x2, y1, y2, z1, z2)\n\nSet up the world coordinate range for the device. This configures the mapping of the user's coordinates to the device coordinates. x1, x2 define the range of the X-axis; y1, y2 the Y-axis, and z1, z2 the Z-axis. We use a standard right-handed coordinate system with X negative to left, positive to right; Y negative out of the screen, positive into the screen; and Z negative down, positive up. A \"fatal\" error will occur if x1 > x2, or y1 > y2, or z1 > z2."},
    {"s2qwin", s2plot_s2qwin, METH_VARARGS, "s2qwin()\n\nQuery the world coordinate range.  Returns a tuple of (x0, x1, y0, y1, z0, z1)."},
    {"s2env", s2plot_s2env, METH_VARARGS, "s2env(xmin, xmax, ymin, ymax, zmin, zmax, just, axis)\n\nConvenience function which sets the plotting environment:\n\n        * Sets the viewport to the default;\n        * Sets the world coordinate range as requested; and\n        * Optionally labels the plot with a call to s2box. \n\n    If just = 1, the scales of the x, y and z axes (in something like world coordinates per inch) will be equal and the plotting viewport may be a generic rectangular prism rather than a cube (the case for just = 0). Argument axis controls labelling as follows:\n\n        * axis = -2: no box, axes or labels\n        * axis = -1: draw box only\n        * axis = 0: draw box and label it with coordinates\n        * axis = 1: same as axis=0, but also draw the coordinate axes\n        * axis = 2: same as axis=1, but also draw grid lines at major intervals \n\n    It is a requirement that xmin < xmax, ymin < ymax and zmin < zmax.\n\n    A call to s2sci can preceed the call to s2env so that the axis labels are drawn in the desired colour."},
    // PRIMITIVES
    {"s2line", s2plot_s2line, METH_VARARGS, "s2line(n, xpts, ypts, zpts)\n\nDraw a poly line, n vertices at (xpts, ypts, zpts) which should all be numpy arrays. The line is drawn in the current colour and with the current line width."},
    {"s2circxy", s2plot_s2circxy, METH_VARARGS, "s2circxy(px, py, pz, r, nseg, asp)\n\nDraw a circle in the xy plane at a given z coordinate. Use nseg to control how many line segments are used to draw the circle. Low values of nseg (>= 3) can be used to draw n-sided polygons!\n\n    Argument asp controls the aspect ratio and whether a circle or ellipse is drawn.\n\n        * asp<0 will produce a circle by calculating the radius as the functions s2disk?? do.\n        * asp=1.0 will produce a circle when the x and y axes have the same device scale and an ellipse otherwise;\n        * asp < 1.0 will squash the ellipse further in y (z, x);\n        * asp > 1.0 stretch in the y (z, x) direction."},
    {"s2circxz", s2plot_s2circxz, METH_VARARGS, "s2circxz(px, py, pz, r, nseg, asp)\n\nDraw a circle in the xz plane at a given y coordinate. Use nseg to control how many line segments are used to draw the circle. Low values of nseg (>= 3) can be used to draw n-sided polygons!\n\n    Argument asp controls the aspect ratio and whether a circle or ellipse is drawn.\n\n        * asp<0 will produce a circle by calculating the radius as the functions s2disk?? do.\n        * asp=1.0 will produce a circle when the x and y axes have the same device scale and an ellipse otherwise;\n        * asp < 1.0 will squash the ellipse further in y (z, x);\n        * asp > 1.0 stretch in the y (z, x) direction."},
    {"s2circyz", s2plot_s2circyz, METH_VARARGS, "s2circyz(px, py, pz, r, nseg, asp)\n\nDraw a circle in the yz plane at a given x coordinate. Use nseg to control how many line segments are used to draw the circle. Low values of nseg (>= 3) can be used to draw n-sided polygons!\n\n    Argument asp controls the aspect ratio and whether a circle or ellipse is drawn.\n\n        * asp<0 will produce a circle by calculating the radius as the functions s2disk?? do.\n        * asp=1.0 will produce a circle when the x and y axes have the same device scale and an ellipse otherwise;\n        * asp < 1.0 will squash the ellipse further in y (z, x);\n        * asp > 1.0 stretch in the y (z, x) direction."},
    {"s2diskxy", s2plot_s2diskxy, METH_VARARGS, "s2diskxy(px, py, pz, r1, r2)\n\nDraw a disk in the xy plane, at a given z coordinate. The disk has an inner and outer radius and the annulus is filled. Thus you can draw solid circles (r1 = 0, r2 > 0), circle outlines (r1 > 0, r2 = r1+eps) and all things in between. The radius is given in world coordinates, and the actual radius used is the quadratic mean of the radius converted to x and y normalised device units. Note that the disk is infinitely thin and can vanish when viewed edge on. If this is problem, consider using s2circxy to provide an outline for the disk/s."},
    {"s2diskxz", s2plot_s2diskxz, METH_VARARGS, "s2diskxz(px, py, pz, r1, r2)\n\nDraw a disk in the xz plane, at a given y coordinate. The disk has an inner and outer radius and the annulus is filled. Thus you can draw solid circles (r1 = 0, r2 > 0), circle outlines (r1 > 0, r2 = r1+eps) and all things in between. The radius is given in world coordinates, and the actual radius used is the quadratic mean of the radius converted to x and z normalised device units. Note that the disk is infinitely thin and can vanish when viewed edge on. If this is problem, consider using s2circxz to provide an outline for the disk/s."},
    {"s2diskyz", s2plot_s2diskyz, METH_VARARGS, "s2diskyz(px, py, pz, r1, r2)\n\nDraw a disk in the yz plane, at a given x coordinate. The disk has an inner and outer radius and the annulus is filled. Thus you can draw solid circles (r1 = 0, r2 > 0), circle outlines (r1 > 0, r2 = r1+eps) and all things in between. The radius is given in world coordinates, and the actual radius used is the quadratic mean of the radius converted to y and z normalised device units. Note that the disk is infinitely thin and can vanish when viewed edge on. If this is problem, consider using s2circyz to provide an outline for the disk/s."},
    {"s2rectxy", s2plot_s2rectxy, METH_VARARGS, "s2rectxy(xmin, xmax, ymin, ymax, z)\n\nDraw a rectangle in the xy plane, at a given z coordinate. The rectangle is a filled quadrangle. The edge of the rectangle is NOT drawn. Use s2line if you need a border to your rectangle/s."},
    {"s2rectxz", s2plot_s2rectxz, METH_VARARGS, "s2rectxz(xmin, xmax, zmin, zmax, y)\n\nDraw a rectangle in the xz plane, at a given y coordinate. The rectangle is a filled quadrangle. The edge of the rectangle is NOT drawn. Use s2line if you need a border to your rectangle/s."},
    {"s2rectyz", s2plot_s2rectyz, METH_VARARGS, "s2rectyz(ymin, ymax, zmin, zmax, x)\n\nDraw a rectangle in the yz plane, at a given x coordinate. The rectangle is a filled quadrangle. The edge of the rectangle is NOT drawn. Use s2line if you need a border to your rectangle/s."},
    {"s2wcube", s2plot_s2wcube, METH_VARARGS, "s2wcube(xmin, xmax, ymin, ymax, zmin, zmax)\n\nDraw a wireframe cube with edges parallel to the main coordinate axes.  Thickness and colour controlled by standard s2 functions/s."}, /* NEW */
    {"s2pt1", s2plot_s2pt1, METH_VARARGS, "s2pt1(x, y, z, symbol)\n\nDraw a single point at location (x,y,z) using symbol:\n\n        * 0 = wireframe box\n        * 1 = point\n        * 2 = wireframe 3D cross\n        * 4 = shaded sphere\n        * 6 = shaded box"},
    {"s2pt", s2plot_s2pt, METH_VARARGS,"s2pt(np, xpts, ypts, zpts, symbol)\n\nDraw a set of npts points.  The points must be numpy arrays. Symbol value 1 produces a single pixel, independent of distance to the point. Symbols are drawn in the current colour. Markers taking finite size are scaled by the current character height. The current linewidth does not affect markers.\n\n    Argument symbol should be one of:\n\n        * 0 = wireframe box\n        * 1 = point\n        * 2 = wireframe 3D cross\n        * 4 = shaded sphere\n        * 6 = shaded box"},
    {"s2pnts", s2plot_s2pnts, METH_VARARGS, "s2pnts(np, xpts, ypts, zpts, symbols, ns)\n\nDraw a set of points with different markers.  Points and markers must be numpy arrays.  The markers are selected from the array symbols of length ns. If ns == n, then the ith point is drawn with the ith symbol. If ns < n, then for the first ns points, the ith symbol is used for the ith point, thereafter the first symbol is used for all remaining points.\n\n    Available values for symbols are:\n\n        * 0 = wireframe box\n        * 1 = point\n        * 2 = wireframe 3D cross\n        * 4 = shaded sphere\n        * 6 = shaded box"},
    {"s2textxy", s2plot_s2textxy, METH_VARARGS, "s2textxy(x, y, z, text)\n\nDraw text in the xy plane, at a fixed z coordinate. Text is drawn with its bottom left corner at the given coordinate. For text extending in the positive direction along an axis, use the s2textxyf versions and give the flipx and/or flipy argument as +1, use -1 for flipped text, or any other value for arbitrary scaling."},
    {"s2textxz", s2plot_s2textxz, METH_VARARGS, "s2textxz(x, y, z, text)\n\nDraw text in the xz plane, at a fixed y coordinate. Text is drawn with its bottom left corner at the given coordinate. For text extending in the positive direction along an axis, use the s2textxzf versions and give the flipx and/or flipz argument as +1, use -1 for flipped text, or any other value for arbitrary scaling."},
    {"s2textyz", s2plot_s2textyz, METH_VARARGS, "s2textyz(x, y, z, text)\n\nDraw text in the yz plane, at a fixed x coordinate. Text is drawn with its bottom left corner at the given coordinate. For text extending in the positive direction along an axis, use the s2textyzf versions and give the flipy and/or flipz argument as +1, use -1 for flipped text, or any other value for arbitrary scaling."},
    {"s2textxyf", s2plot_s2textxyf, METH_VARARGS, "s2textxyf(x, y, z, flipx, flipy, text)\n\nDraw flipped text in the xy plane, at a fixed z coordinate. Text is drawn with its bottom left corner at the given coordinate. For text extending in the positive direction along an axis, give the flipx and/or flipy argument as +1, use -1 for flipped text, or any other value for arbitrary scaling."},
    {"s2textxzf", s2plot_s2textxzf, METH_VARARGS, "s2textxzf(x, y, z, flipx, flipz, text)\n\nDraw flipped text in the xz plane, at a fixed y coordinate. Text is drawn with its bottom left corner at the given coordinate. For text extending in the positive direction along an axis, give the flipx and/or flipz argument as +1, use -1 for flipped text, or any other value for arbitrary scaling."},
    {"s2textyzf", s2plot_s2textyzf, METH_VARARGS, "s2textyzf(x, y, z, flipy, flipz, text)\n\nDraw flipped text in the yz plane, at a fixed x coordinate. Text is drawn with its bottom left corner at the given coordinate. For text extending in the positive direction along an axis, give the flipy and/or flipz argument as +1, use -1 for flipped text, or any other value for arbitrary scaling."},
    {"s2qtxtxy", s2plot_s2qtxtxy, METH_VARARGS, "s2qtxtxy(x, y, z, text, pad)\n\nFind the bounding box of text drawn with the s2textxy function. For convenience, pad can be given > 0.0 to obtain padding around the text of pad percent.  Returns an tuple: ((xmin, xmax),(ymin, ymax))"},
    {"s2qtxtxz", s2plot_s2qtxtxz, METH_VARARGS, "s2qtxtxz(x, y, z, text, pad)\n\nFind the bounding box of text drawn with the s2textxz function. For convenience, pad can be given > 0.0 to obtain padding around the text of pad percent.  Returns an tuple: ((xmin, xmax),(zmin, zmax))"},
    {"s2qtxtyz", s2plot_s2qtxtyz, METH_VARARGS, "s2qtxtyz(x, y, z, text, pad)\n\nFind the bounding box of text drawn with the s2textyz function. For convenience, pad can be given > 0.0 to obtain padding around the text of pad percent.  Returns an tuple: ((ymin, ymax),(zmin, zmax))"},
    {"s2qtxtxyf", s2plot_s2qtxtxyf, METH_VARARGS, "s2qtxtxyf(x, y, z, text, pad)\n\nFind the bounding box of text drawn with the s2textxyf function. For convenience, pad can be given > 0.0 to obtain padding around the text of pad percent.  Returns an tuple: ((xmin, xmax),(ymin, ymax))"},
    {"s2qtxtxzf", s2plot_s2qtxtxzf, METH_VARARGS, "s2qtxtxzf(x, y, z, text, pad)\n\nFind the bounding box of text drawn with the s2textxzf function. For convenience, pad can be given > 0.0 to obtain padding around the text of pad percent.  Returns an tuple: ((xmin, xmax),(ymin, ymax))"},
    {"s2qtxtyzf", s2plot_s2qtxtyzf, METH_VARARGS, "s2qtxtyzf(x, y, z, text, pad)\n\nFind the bounding box of text drawn with the s2textyzf function. For convenience, pad can be given > 0.0 to obtain padding around the text of pad percent.  Returns an tuple: ((xmin, xmax),(ymin, ymax))"},
    {"s2arro", s2plot_s2arro, METH_VARARGS, "s2arro(x1, y1, z1, x2, y2, z2)\n\nDraw an arrow from the point with world-coordinates (x1,y1,z1) to (x2,y2,z2). The size of the arrowhead at (x2,y2,z2) is controlled by the current character size (s2sch). The arrow style is set with s2sah."},
    // ATTRIBUTES
    {"s2sci", s2plot_s2sci, METH_VARARGS, "s2sci(idx)\n\nSet the pen colour by index. If it lies outside the defined range, it will be set to the default colour (white)."},
    {"s2scr", s2plot_s2scr, METH_VARARGS, "s2scr(idx, r, g, b)\n\nSet colour representation associated with index idx. Once defined this colour can be selected and used. The default colours (indices 0..15) can be redefined.\n\n    Note that for historical reasons, the variable type (float) used in setting and querying the colour index is not the same as the variable type of the COLOUR data structure (double)."},
    {"s2qcr", s2plot_s2qcr, METH_VARARGS, "s2qcr(idx)\n\nQuery colour representation, ie. obtain the red, green and blue components of the given colour index as a dictionary with keys (r,g,b).\n\n    Note that for historical reasons, the variable type (float) used in setting and querying the colour index is not the same as the variable type of the COLOUR data structure (double)."},
    {"s2slw", s2plot_s2slw, METH_VARARGS,"s2slw(width)\n\nSet the linewidth in pixels. The default is 1. Changing the line width affects lines and vector symbols. Unlike PGPLOT, text is not affected."},
    {"s2sls", s2plot_s2sls, METH_VARARGS, "s2sls(style)\n\nSet the character height in \"arbitrary\" units.  The default character size is 1.0, corresponding to a character about 15 pixels in size.  Changing the character height also changes the scale of tick marks and symbols."},
    {"s2sch", s2plot_s2sch, METH_VARARGS,"s2sch(size)\n\nSet the character height in \"arbitrary\" units. The default character size is 1.0, corresponding to a character about 15 pixels in size. Changing the character height also changes the scale of tick marks and symbols."},
    {"s2sah", s2plot_s2sah, METH_VARARGS, "s2sah(fs, angle, barb)\n\nSet the style to be used for arrowheads drawn with s2arro. Currently, all arrowheads are drawn as cones, with the rendering mode (wireframe or shaded) affecting the look. Line-based arrowheads may be added at a later stage. Argument fs is the fill style:\n\n        * fs = 1: filled\n        * fs = 2: outline [not yet implemented] \n\n    Argument angle [5.0, 135.0] is the angle of the arrow point in degrees, with default value 45.0 (a cone of semi-vertex angle 22.5 degrees). Argument barb [0.05, 1.0] is the fraction of the cone that is cut away from the back, such that 0.05 gives an open cone and 1.0 gives a cone with a solid base, with default value 0.3."},
// AXES, BOXES & LABELS
    {"s2box", s2plot_s2box, METH_VARARGS,"s2box(xopt, xtick, nxsub, yopt, ytick, nysub, zopt, ztick, nzsub)\n\nDraw a labelled box around the world space. Arguments xtick, ytick and ztick specify the major tick intervals on each axis (0.0 means let S2PLOT figure out the \"best\" value/s); nxsub, nysub and nzsub indicate how many subdivisions the major intervals should comprise and are ignored when S2PLOT is determining major intervals itself. If nxsub, nysub or nzsub are 0, then they are automatically determined.\n\n    xopt is a character string, with any of the following flags:\n    B = draw front bottom (-Y,-Z) edge\n    C = draw front top (-Y,+Z) edge\n    D = draw back bottom (+Y,-Z) edge\n    E = draw back top (+Y,+Z) edge\n    T = draw major tick marks\n    S = draw minor tick marks [not yet implemented]\n    M = numeric labels in conventional location/s\n    N = numeric labels in non-conventional location/s\n    O = draw labels on an opaque panel so they are never seen in reverse - this is always done if both M and N options are requested\n    Q = make panel larger to fit axis titles drawn by s2lab function\n    G = draw grid lines at major tick intervals\n    L = written label is 10^(coordval), ie. logarithmic labels\n    1 = force decimal labels (this is default: redundant option)\n    2 = force exponential (scientific notation) labels\n\n    yopt is a character string as for xopt, but with following changes:\n    B = draw bottom left (-Z,-X) edge\n    C = draw bottom right (-Z,+X) edge\n    D = draw top left (+Z,-X) edge\n    E = draw top right (+Z,+X) edge\n\n    zopt is a character string as for xopt, but with following changes:\n    B = draw left front (-X,-Y) edge\n    C = draw right front (-X,+Y) edge\n    D = draw left back (+X,-Y) edge\n    E = draw right back (+X,+Y) edge"},
    {"s2lab", s2plot_s2lab, METH_VARARGS, "s2lab(xlab, ylab, zlab, title)\n\nDraw labels along the edges of the viewport. Note that this routine expects s2box to have been called with options to produce panels for drawing axis text on. If not, then the function will not fail, but the resultant labels will be overlaid on back-to-front versions of same."},
    {"s2help", s2plot_s2help, METH_VARARGS, "s2help(helpstr)\n\nSet the custom help string.  This is shown on the second press of the F1 key for S2PLOT programs, if it is set."}, /* NEW */
    // XY(Z) PLOTS
    {"s2errb", s2plot_s2errb, METH_VARARGS, "s2errb(dir, n, xpts, ypts, zpts, edelt, termsize)\n\nDraw error bars at the coordinates (xpts[i], ypts[i], zpts[i]) which should be numpy arrays. Error bars are drawn in the direction indicated by argument dir as described below. One-sided error bar lengths are given by edelt, such that for error bars in eg. the x direction, error bars are drawn to xpts[i]+edelt[i], to xpts[i]-edelt[i], or to both. Argument termsize gives the size of the terminals to draw for each error bar; it is given in an integer increment of the current linewidth. Eg. if t=1, then end points are one pixel larger than the line width used to draw the bars.\n\n    dir: 1	for error bar in	+x	direction\n    2	...	+y	...\n    3	...	+z	...\n    4	...	-x	...\n    5	...	-y	...\n    6	...	-z	...\n    7	...	+/-x	...\n    8	...	+/-y	...\n    9	...	+/-z	..."},
    {"s2funt", s2plot_s2funt, METH_VARARGS, "s2funt(fx, fy, fz, n, tmin, tmax)\n\nDraw a curve defined by parametric equations fx(t), fy(t) and fz(t), which must all be callables taking a float and returning a float.  N points are constructed, uniformly spaced from tmin to tmax."},
    {"s2funtc", s2plot_s2funtc, METH_VARARGS, "s2funtc(fx, fy, fz, fc, n, tmin, tmax)\n\nLike s2funt, but an additional function fc (whose return value is clipped to the range [0,1]) controls the colour of the line, according to the colour index range currently set.  fc must also be a callable taking a float and returning a float."},
    {"s2funxy", s2plot_s2funxy, METH_VARARGS, "s2funxy(fxy, nx, ny, xmin, xmax, ymin, ymax, ctl)\n\nDraw the surface described by the function fxy which must return a float given 2 floats. The function is evaluated on a nx * ny grid whose world coordinates extend from (xmin,ymin) to (xmax,ymax). The ctl argument has the following effect:\n\n        * ctl = 0: curve plotted in current window and viewport. Caller is responsible for setting the viewport and world coordinate system suitably.\n        * ctl = 1: s2env is called automatically to fit the plot in the current viewport. \n\n    Beware that these functions consume memory to store all the function evaluations prior to triangulating the surface."},
    {"s2funxz", s2plot_s2funxz, METH_VARARGS, "s2funxz(fxz, nx, nz, xmin, xmax, zmin, zmax, ctl)\n\nDraw the surface described by the function fxz which must return a float given 2 floats. The function is evaluated on a nx * nz grid whose world coordinates extend from (xmin,zmin) to (xmax,zmax). The ctl argument has the following effect:\n\n        * ctl = 0: curve plotted in current window and viewport. Caller is responsible for setting the viewport and world coordinate system suitably.\n        * ctl = 1: s2env is called automatically to fit the plot in the current viewport. \n\n    Beware that these functions consume memory to store all the function evaluations prior to triangulating the surface."},
    {"s2funyz", s2plot_s2funyz, METH_VARARGS, "s2funyz(fyz, ny, nz, ymin, ymax, zmin, zmax, ctl)\n\nDraw the surface described by the function fyz which must return a float given 2 floats. The function is evaluated on a ny * nz grid whose world coordinates extend from (ymin,zmin) to (ymax,zmax). The ctl argument has the following effect:\n\n        * ctl = 0: curve plotted in current window and viewport. Caller is responsible for setting the viewport and world coordinate system suitably.\n        * ctl = 1: s2env is called automatically to fit the plot in the current viewport. \n\n    Beware that these functions consume memory to store all the function evaluations prior to triangulating the surface."},
    {"s2funxyr", s2plot_s2funxyr, METH_VARARGS, "s2funxyr(fxy, nx, ny, xmin, xmax, ymin, ymax, ctl, rmin, rmax)\n\nDraw surface as per s2funxy, but with explicit settings for the colour range mapping between rmin and rmax."},
    {"s2funxzr", s2plot_s2funxzr, METH_VARARGS, "s2funxzr(fxz, nx, nz, xmin, xmax, zmin, zmax, ctl, rmin, rmax)\n\nDraw surface as per s2funxz, but with explicit settings for the colour range mapping between rmin and rmax."},
    {"s2funyzr", s2plot_s2funyzr, METH_VARARGS, "s2funyzr(fyz, ny, nz, ymin, ymax, zmin, zmax, ctl, rmin, rmax)\n\nDraw surface as per s2funyz, but with explicit settings for the colour range mapping between rmin and rmax."},
    {"s2funuv", s2plot_s2funuv, METH_VARARGS, "s2funuv(fx, fy, fz, fcol, umin, umax, uDIV, vmin, vmax, vDIV)\n\nPlot the parametric function (generally a surface) defined by { fx(u,v), fy(u,v), fz(u,v) } (each return a float given 2 floats), coloured by fcol(u,v) with fcol required to fall in the range [0,1].  fcol is then mapped to the current colormap index range (set with s2scir). The range of u and v values are specified by umin,umax and vmin, vmax, and the number of divisions by uDIV and vDIV."},
    // IMAGES/SURFACES
    {"s2surp", s2plot_s2surp, METH_VARARGS,"s2surp(data, nx, ny, i1, i2, j1, j2, datamin, datamax,  tr)\n\nDraw a colour surface representation of the 2-dimensional numpy-array, data, containing nx * ny values. A sub-section only of the array is drawn, viz. data[i1:i2][j1:j2]. Data values <= datamin  are mapped to the first colour in the colour map (see s2scir), while values >= datamax are mapped to the last entry in the colour map. The mapping is linear at this stage. The final argument, tr, defines the transformation of the data cell locations to world coordinates in the X-Y space, and the transformation of data values to the Z ordinate, as follows:\n\n    x = tr[0] + tr[1] * i + tr[2] * j\n    y = tr[3] + tr[4] * i + tr[5] * j\n    z = tr[6] + tr[7] * dataval"},
    {"s2surpa", s2plot_s2surpa, METH_VARARGS,"s2surpa(data, nx, ny, i1, i2, j1, j2, datamin, datamax, tr)\n\nDraw a colour surface representation of the 2-dimensional numpy-array, data, containing nx * ny values. A sub-section only of the array is drawn, viz. data[i1:i2][j1:j2]. Data values <= datamin are mapped to the first colour in the colour map (see s2scir), while values >= datamax  are mapped to the last entry in the colour map. The mapping is linear at this stage. This function differs to the simpler s2surp in that the tranformation array provides an arbitrary transform, allowing the surface plot to be placed anywhere in the space oriented at any angle, etc. The transformation is as follows:\n\n    x = tr[0] + tr[1] * i + tr[2] * j + tr[3] * dataval\n    y = tr[4] + tr[5] * i + tr[6] * j + tr[7] * dataval\n    z = tr[8] + tr[9] * i + tr[10]* j + tr[11]* dataval"},
    {"s2scir", s2plot_s2scir, METH_VARARGS, "s2scir(col1, col2)\n\nSet the range of colour indices used for shading."},
    {"s2qcir", s2plot_s2qcir, METH_VARARGS, "s2qcir()\n\nQuery the colour index range.  Returns a tuple: (colMin, colMax)"},
    {"s2icm", s2plot_s2icm, METH_VARARGS,"s2icm(mapname, idx1, idx2)\n\nInstall various colour maps. Give map name as a string, and index range where you want the map installed. Available maps are: 'rainbow', 'grey'|'gray', 'terrain', 'topo', 'iron', 'heated, 'hot', 'astro', 'alt', 'zebra', 'mgreen'; and may be preceeded by the exact string \"inverse \" (note the space is significant) to reverse the map direction."},
    // VECTOR PLOTS
    {"s2vect3", s2plot_s2vect3, METH_VARARGS, "s2vect3(a, b, c, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, scale, nc, tr, minlength, colbylength, minl, maxl)\n\nDraw a vector map of a 3D data array, with data blanking.\n\n        * a, b and c are 3D numpy-arrays indexed by [0..(adim-1)][0..(bdim-1)][0..(cdim-1)] holding the components of the vectors in three orthogonal directions.\n        * The slice of data actually plotted is indexed by [a1..a2][b1..b2][c1..c2].\n        * Vector lengths are scaled by the value scale. There is no auto-set as per PGPlot's pgvect function.\n        * nc controls positioning of the vectors. nc<0 places the head of the vector on the coordinates; nc>0 places the vector base on the coords, and nc == 0 centres the vector on the coords.\n        * tr is the transformation matrix which maps indexes into the arrays onto the x, y and z axes of the 3D space. NOTE: this transformation IS NOT APPLIED to the vector components!!! The standard (non-rotated, non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9]and tr[10] all zero.\n          x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic\n          y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic\n          z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic\n        * Vectors with length (sqrt(a[...]^2 + b[...]^2 + c[...]^2)) less than minlength are not drawn.\n        * If colbylength > 0, the the vectors will be coloured by mapping those of length minl or smaller to the start of the current colour index range, and those of length maxl or greater to the end of the current colour index range."},
    // MISCELLANEOUS ROUTINES
    {"s2iden",s2plot_s2iden, METH_VARARGS, "s2iden(textra)\n\nWrite username, date, time and optional string at bottom of plot."},
    // S2 "NATIVE" ROUTINES
    {"ns2sphere", s2plot_ns2sphere, METH_VARARGS, "ns2sphere(x, y, z, r, red, green, blue)\n\nDraw a sphere, with a given centre (x,y,z), radius (r) and RGB colour (red, green, blue)."},
    {"ns2vsphere", s2plot_ns2vsphere, METH_VARARGS, "ns2vsphere(P, r, col)\n\nDraw a sphere, with a given centre (P, an {xyz} dict), radius (r) and RGB colour (col, an {rgb} dict)."},
    {"ns2spheret", s2plot_ns2spheret, METH_VARARGS, "ns2spheret(x, y, z, r, red, green, blue, texturefn)\n\nDraw a textured sphere, with a given centre (x,y,z), radius (r) and RGB colour (red, green, blue). The texture file is specified by texturefn, and must be a .TGA file that has length and width as powers of 2. The utility program texturise.csh can be used to convert your textures to the appropriate format. This is an OpenGL restriction."},
    {"ns2vspheret", s2plot_ns2vspheret, METH_VARARGS, "ns2vspheret(P, r, col, texturefn)\n\nDraw a textured sphere, with a given centre (P, an {xyz} dict), radius (r) and colour (col, an {rgb} dict), using vector data structures. The texture file is specified by texturefn, and must be a .TGA file that has length and width as powers of 2. The utility program texturise.csh can be used to convert your textures to the appropriate format. This is an OpenGL restriction."},
    {"ns2spherex", s2plot_ns2spherex, METH_VARARGS, "ns2spherex(x, y, z, r, red, green, blue, texid)\n\nDraw a textured sphere, with given centre, radius, colour and using a texture id as returned by ss2lt."},
    {"ns2vspherex", s2plot_ns2vspherex, METH_VARARGS, "ns2vspherex(P, r, col, texid)\n\nDraw a textured sphere, with given centre, radius, colour and using a texture id as returned by ss2lt. P is an {xyz} dict and col is a {rgb} dict."},
    {"ns2vplanett", s2plot_ns2vplanett, METH_VARARGS, "ns2vplanett(iP, ir, icol, itexturefn, texture_phase, axis, rotation.\n\nDraw a planet, which is a textured sphere whose texture can be 'slid' around the planet, and the planet then rotated about an arbitrary axis."}, /* NEW */
    {"ns2vplanetx", s2plot_ns2vplanetx, METH_VARARGS, "ns2vplanetx(iP, ir, icol, itextureid, texture_phase, axis, rotation.\n\nDraw a planet, which is a textured sphere whose texture can be 'slid' around the planet, and the planet then rotated about an arbitrary axis."}, /* NEW */
    {"ns2disk", s2plot_ns2disk, METH_VARARGS, "ns2disk(x, y, z, nx, ny, nz, r1, r2, red, green, blue)\n\nDraw an annulus with given centre (x,y,z), inner and outer radii (r1, r2) and RGB colour (red, green, blue). The annulus is oriented with normal vector (nx,ny,nz)."},
    {"ns2vdisk", s2plot_ns2vdisk, METH_VARARGS, "ns2vdisk(P, N, r1, r2, col)\n\nDraw an annulus with given centre (P, an {xyz} dict), inner and outer radii (r1, r2) and RGB colour (col, an {rgb} dict). The annulus is oriented with normal vector (nx,ny,nz)."},
    {"ns2arc", s2plot_ns2arc, METH_VARARGS, "ns2arc(px, py, pz, nx, ny, nz, sx, sy, sz, deg, nseg)\n\nDraw an arc at (px,py,pz) with normal (nx,ny,nz) using the current pen thickness and colour. Vector (sx,sy,sz) gives starting vector from (px,py,pz) and is rotated around the normal in steps (deg/(nseg-1)).\n\n    This function is an \"extension\", and its behaviour for world coordinates which have unequal scales in x, y and/or z is not particularly well defined.  For equal x,y,z world scales, these arcs have major/minor axis ratio of 1.0."},
    {"ns2varc", s2plot_ns2varc, METH_VARARGS, "ns2varc(P, N, S, deg, nseg)\n\nDraw an arc at position P, with normal N using the current pen thickness and colour. Vector S gives starting vector from P and is rotated around the normal in steps (deg/(nseg-1)).  P, N and S are {xyz} dicts.\n\n    This function is an \"extension\", and its behaviour for world coordinates which have unequal scales in x, y and/or z is not particularly well defined."},
    {"ns2erc", s2plot_ns2erc, METH_VARARGS, "ns2erc(px, py, pz, nx, ny, nz, sx, sy, sz, deg, nseg, axratio)\n\nDraw an arc at (px,py,pz) with normal (nx,ny,nz) using the current pen thickness and colour. Vector (sx,sy,sz) gives starting vector from (px,py,pz) and is rotated around the normal in steps (deg/(nseg-1)).\n\n    This function is an \"extension\", and its behaviour for world coordinates which have unequal scales in x, y and/or z is not particularly well defined.  Uses specified axis ratio, axratio."}, /* NEW */
    {"ns2verc", s2plot_ns2verc, METH_VARARGS, "ns2verc(P, N, S, deg, nseg, axratio)\n\nDraw an arc at position P, with normal N using the current pen thickness and colour. Vector S gives starting vector from P and is rotated around the normal in steps (deg/(nseg-1)).  P, N and S are {xyz} dicts, axratio is a float.\n\n"}, /* NEW */
    {"ns2text", s2plot_ns2text, METH_VARARGS, "ns2text(x, y, z, rx, ry, rz, ux, uy, uz, red, green, blue, text)\n\nDraw text at a given position (x,y,z), with right vector (rx,ry,rz) and up vector (ux,uy,uz), RGB colour (red,greeen,blue), and text string (text)."},
    {"ns2vtext", s2plot_ns2vtext, METH_VARARGS, "ns2vtext(P, R, U, col, text)\n\nDraw text at a given position (P), with right vector (R) and up vector (U), RGB colour (col), and text string (text). P, R and U are {xyz} dicts and col is a {rgb} dict."},
    {"ns2point", s2plot_ns2point, METH_VARARGS, "ns2point(x, y, z, red, green, blue)\n\nDraw a point at the position (x,y,z) with RBG colour (red,green,blue)."},
    {"ns2vpoint", s2plot_ns2vpoint, METH_VARARGS, "ns2vpoint(P, col)\n\nDraw a point at position P with RBG colour (col), where P is an {xyz} dict and col is a {rgb} dict."},
    {"ns2vnpoint", s2plot_ns2vnpoint, METH_VARARGS, "ns2vnpoint(P, col, n)\n\nDraw n points at positions P with RBG colour (col), where P is a List of {xyz} dicts and col is a {rgb} dict."},
    {"ns2thpoint", s2plot_ns2thpoint, METH_VARARGS, "ns2thpoint(x, y, z, red, green, blue, size)\n\nDraw a thick point at the position (x,y,z) with RBG colour (red,green,blue) and thickness size pixels (not world coordinates)."},
    {"ns2vthpoint", s2plot_ns2vthpoint, METH_VARARGS, "ns2vthpoint(P, col, size)\n\nDraw a thick point at position P with RBG colour (col), and thickness size pixels (not world coordinates) where P is an {xyz} dict and col is a {rgb} dict."},
    {"ns2i", s2plot_ns2i, METH_VARARGS, "ns2i(x, y, z, red, green, blue)\n\nPlace an OpenGL light at location (x,y,z) with RGB colour (red,green,blue)."},
    {"ns2vi", s2plot_ns2vi, METH_VARARGS, "ns2vi(P, col)\n\nPlace an OpenGL light at location (P, an {xyz} dict) with RGB colour col, a {rgb} dict."},
    {"ns2line", s2plot_ns2line, METH_VARARGS, "ns2line(x1, y1, z1, x2, y2, z2, red, green, blue)\n\nDraw a line from (x1,y1,z1) to (x2,y2,z2) using RGB colour (red,green,blue)."},
    {"ns2vline", s2plot_ns2vline, METH_VARARGS, "ns2vline(P1, P2, col)\n\nDraw a line from (P1, an {xyz} dict) to (P2) with RGB colour (col, an {rgb} dict), using vector data structures."},
    {"ns2thline", s2plot_ns2thline, METH_VARARGS, "ns2thline(x1, y1, z1, x2, y2, z2, red, green, blue, width)\n\nDraw a thick line (width) from (x1,y1,z1) to (x2,y2,z2) using RGB colour (red,green,blue)."},
    {"ns2vthline", s2plot_ns2vthline, METH_VARARGS, "ns2vthline(P1, P2, col, width)\n\nDraw a thick (width) line from (P1) to (P2, {xyz} dicts) with RGB colour (col, an {rgb} dict), using vector data structures."},
    {"ns2thwcube", s2plot_ns2thwcube, METH_VARARGS, "ns2thwcube(x1, y1, z1, x2, y2, z2, red, green, blue, width)\n\nDraw a wireframe cube (with axes parallel to the coordinate axes) in a specific color and thickness."}, /* NEW */
    {"ns2vthwcube", s2plot_ns2vthwcube, METH_VARARGS, "ns2vthwcube(P1, P2, col, width)\n\nDraw a wireframe cube (with axes parallel to the coordinate axes) in a specific color and thickness, using vector data structures."}, /* NEW */
    {"ns2cline", s2plot_ns2cline, METH_VARARGS, "ns2cline(x1, y1, z1, x2, y2, z2, red1, green1, blue1, red2, green2, blue2)\n\nDraw a coloured line from (x1,y1,z1) to (x2,y2,z2). The colour is blended along the line between the two input RGB colours (red1,green1,blue1) and (red2,green2,blue2)."},
    {"ns2vcline", s2plot_ns2vcline, METH_VARARGS, "ns2vcline(P1, P2, col1, col2)\n\nDraw a coloured line from (P1) to (P2, {xyz} dicts), using vector data structures. The colour is blended along the line between the two input RGB colours (col1) and (col2, {rgb} dicts)."},
    {"ns2thcline", s2plot_ns2thcline, METH_VARARGS, "ns2thcline(x1, y1, z1, x2, y2, z2, red1, green1, blue1, red2, green2, blue2, width)\n\nDraw a thick coloured line, with clolour blended between the two given colours along the line, and given width."}, /* NEW */
    {"ns2vthcline", s2plot_ns2vthcline, METH_VARARGS, "ns2vthcline(P1, P2, col1, col2, width)\n\nDraw a thick coloured line, with clolour blended between the two given colours along the line, and given width, using vector datastructures."}, /* NEW */
    {"ns2vf3", s2plot_ns2vf3, METH_VARARGS, "ns2vf3(P, col)\n\nDraw a 3-vertex facet with a single colour. The vertices are given by the list P of 3 {xyz} dicts, normals are calculated automatically, and the RGB colour is col, a {rgb} dict."},
    {"ns2vf3n", s2plot_ns2vf3n, METH_VARARGS, " ns2vf3n(P, N, col)\n\nDraw a 3-vertex facet with a single colour. The vertices are given by the list P, of 3 {xyz} dicts, normals are specified in the list N of 3 {xyz} dicts, and the RGB colours is col, an {rgb} dict."},
    {"ns2vf3c", s2plot_ns2vf3c, METH_VARARGS, "ns2vf3c(P, col)\n\nDraw a 3-vertex facet with a coloured vertices. The vertices are given by the list P of 3 {xyz} dicts, normals are calculated automatically, and the RGB colours for each vertex are stored in the list col, of 3 {rgb} dicts."},
    {"ns2vf3nc", s2plot_ns2vf3nc, METH_VARARGS, "ns2vf3nc(P, N, col)\n\nThe most general function for drawing a 3-vertex facet with coloured vertices. The vertices are given by the list, P, of 3 {xyz} dicts, normals are in the list, N, of 3 {xyz} dicts, and the RGB colours for vertices are in col, 3 {rgb} dicts. The other 3-vertex functions call this function."},
    {"ns2vf4", s2plot_ns2vf4, METH_VARARGS, "ns2vf4(P, col)\n\nDraw a 4-vertex facet with a single colour. The vertices are given by the list, P, of 4 {xyz} dicts, normals are calculated automatically, and the RGB colour is col, an {rgb} dict."},
    {"ns2vf4n", s2plot_ns2vf4n, METH_VARARGS, "ns2vf4n(P, N, col)\n\nDraw a 4-vertex facet with a single colour. The vertices are given by the list P, normals are specified in the list N ( both 4-lists of {xyz} dicts), and the RGB colours is col, a {rgb} dict."},
    {"ns2vf4c", s2plot_ns2vf4c, METH_VARARGS, "ns2vf4c(P, col)\n\nDraw a 4-vertex facet with a coloured vertices. The vertices are given by the list, P, of 4 {xyz} dicts, normals are calculated automatically, and the RGB colours for each vertex are stored in the list col, of 4 {rgb} dicts."},
    {"ns2vf4nc", s2plot_ns2vf4nc, METH_VARARGS, "ns2vf4nc(P, N, col)\n\nThe most general function for drawing a 4-vertex facet with coloured vertices. The vertices are given by the list, P, of 4 {xyz} dicts, normals are in the list, N, of 4 {xyz} dicts, and the RGB colours for vertices are in col, 4 {rgb} dicts. The other 4-vertex functions call this function."},
    {"ns2vf4t", s2plot_ns2vf4t, METH_VARARGS, "ns2vf4t(P, col, texturefn, scale, trans)\n\nDraw a 4-vertex facet, and apply the texture file texturefn. The vertices are given by the list, P, of 4 {xyz} dicts, normals are calculated automatically, and the underlying RGB colour is col, a {rgb} dict. scale should be in the range [0,1] and transparency trans = 'o' (opaque) or 't' (transparent).\n\n    texturefn must be a .TGA file that has length and width as powers of 2. The utility program texturise.csh can be used to convert your textures to the appropriate format. This is an OpenGL restriction."},
    {"ns2vf4x", s2plot_ns2vf4x, METH_VARARGS, "ns2vf4x(P, col, textureid, scale, trans)\n\nDraw a textured 4-vertex facet (P is a 4-list of {xyz} dicts specifying the corners and col is the {rgb} dict colour), using a texture id as returned by ss2lt; scale should be in the range [0,1] and trans = 'o' or 't' for opaque or transparent textures."},
    {"ns2vf4xt", s2plot_ns2vf4xt, METH_VARARGS, "ns2vf4xt(iP, icol, itextureid, iscale, itrans, ialpha)\n\nDraw a textured 4-vertex facet, using a texture id as returned by ss2lt; scale should be in the range [0,1] and trans = 'o' or 't' for opaque or transparent textures. The alpha channel is set by parameter ialpha.  iP is a 4-list of {xyz} dicts specifying the corners, and icol is the {rgb} dict colour."},
    {"ns2scube", s2plot_ns2scube, METH_VARARGS, "ns2scube(x1, y1, z1, x2, y2, z2, red, green, blue, alpha)\n\nDraw a solid cube with filled but transparent faces."}, /* NEW */
    {"ns2vscube", s2plot_ns2vscube, METH_VARARGS, "ns2vscube(P1, P2, col, alpha)\n\nDraw a solid cube with filled but transparent faces, using vector structures."}, /* NEW */
    {"ns2m", s2plot_ns2m, METH_VARARGS, "ns2m(type, size, x, y, z, red, green, blue)\n\nDraw a marker at (x,y,z) with size and RGB colour (red,green,blue). Argument type should be one of:\n\n        * 0 = tetrahedron (pyramid)\n        * 1 = wireframe 3D cross\n        * 2 = shaded box\n        * 4 = octahedron (diamond)"},
    {"ns2vm", s2plot_ns2vm, METH_VARARGS, "ns2vm(type, size, P, col)\n\nDraw a marker at position P, an {xyz} dict, with size and RGB colour col, a {rgb} dict. Argument type should be one of:\n\n        * 0 = tetrahedron (pyramid)\n        * 1 = wireframe 3D cross\n        * 2 = shaded box\n        * 4 = octahedron (diamond)"},
    // DYNAMIC-ONLY GEOMETRY FUNCTIONS
    {"ds2bb", s2plot_ds2bb, METH_VARARGS, "ds2bb(x, y, z, str_x, str_y, str_z, isize, r, g, b, itextid, alpha, trans)\n\nDraw a (dynamic) billboard. This is a textured facet (4 vertex) that sits at a given location and always faces the camera. By using small, rotationally symmetric texture maps, the appearance of soft, 3d objects can be produced at low frame-rate cost. This function should only be called from a dynamic callback: billboards cannot be cached static geometry as they typically change with every refresh.\n\n    The billboard texture is at coordinates (x,y,z), and can be stretched towards the coordinates (str_x, str_y, str_z) (for no stretching, set these coordinates to 0.0). The overall scale of the billboard texture is controlled by isize.\n\n    The RGB texture colour is specified by parameters (r, g, b), and the texture to use is identified by itextid.\n\n    Transparency is controlled by the alpha channel, with value in the range [0,1] and trans:\n\n        * trans = 'o' opaque;\n        * trans = 't' transparent; and\n        * trans = 's' transparency + absorption."},
    {"ds2vbb", s2plot_ds2vbb, METH_VARARGS, "ds2vbb(iP, iStretch, isize, iC, itexid, alpha, trans)\n\nDraw a (dynamic) billboard using vector inputs. This is a textured facet (4 vertex) that sits at a given location and always faces the camera. By using small, rotationally symmetric texture maps, the appearance of soft, 3d objects can be produced at low frame-rate cost. This function should only be called from a dynamic callback: billboards cannot be cached static geometry as they typically change with every refresh.\n\n    The billboard texture is at coordinates iP, an {xyz} dict, and can be stretched towards the coordinates given by iStretch, an {xyz} dict (for no stretching, set these coordinates to 0.0). The overall scale of the billboard texture is controlled by isize.\n\n    The RGB texture colour is specified by iC, an {rgb} dict, and the texture to use is identified by itextid.\n\n    Transparency is controlled by the alpha channel, with value in the range [0,1] and trans:\n\n        * trans = 'o' opaque;\n        * trans = 't' transparent; and\n        * trans = 's' transparency + absorption."},
    {"ds2vbbr", s2plot_ds2vbbr, METH_VARARGS, "ds2vbbr(iP, iStretch, isize, ipangle, iC, itexid, alpha, trans)\n\nDraw a (dynamic) \"billboard\", with rotation ipangle (in degrees) of the texture about the view direction."},
    {"ds2vbbp", s2plot_ds2vbbp, METH_VARARGS, "ds2vbbp(iP, offset, aspect, isize, iC, itexid, alpha, trans)\n\nDraw a (dynamic) \"billboard\", with aspect ratio width:height instead of stretch, and offset in screen coords (x,y) [offset.z ignored]"}, /* NEW */
    {"ds2tb", s2plot_ds2tb, METH_VARARGS, "ds2tb(x, y, z, x_off, y_off, text, scaletext)\n\nDraw text at position (x,y,z) that always faces the camera. Use this function only from within a dynamic callback. offset is in character units along Right and Up vectors - use this to displace labels in screen coordinates. If scaletext is true (non-zero), then text further from the camera will be made larger so that labels are equally legible for all distances.\n\n    NOTE: this function uses the S2PLOT attributes for character height (size) and colour."},
    {"ds2vtb", s2plot_ds2vtb, METH_VARARGS, "ds2vtb(iP, ioff, text, scaletext)\n\nDraw text at position iP, an {xyz} dict, that faces the camera with vector inputs. Use this function only from within a dynamic callback. The components of iOff, an {xyz} dict, are in character units along Right and Up vectors - use this to displace labels in screen coordinates. If scaletext is true (non-zero), then text further from the camera will be made larger so that labels are equally legible for all distances.\n\n    NOTE: this function uses the S2PLOT attributes for character height (size) and colour. The z component of ioff is ignored."},
    {"ds2protect", s2plot_ds2protect, METH_VARARGS, "ds2protect()\n\nProtect the dynamic goemetry.  Typically use this by setting ds2protect() at the end of your standard dynamic callback, then then call ds2unprotect() if key presses / events processed by other callbacks will result in a changed geometry state.  Use with caution ... incorrect use can result in Total Memory Consumption."},
    {"ds2unprotect", s2plot_ds2unprotect, METH_VARARGS, "ds2unprotect()\n\nUnprotect the dynamic goemetry.  Typically use this by setting ds2protect() at the end of your standard dynamic callback, then then call ds2unprotect() if key presses / events processed by other callbacks will result in a changed geometry state.  Use with caution ... incorrect use can result in Total Memory Consumption."},
    {"ds2isprotected", s2plot_ds2isprotected, METH_VARARGS, "ds2isprotected()\n\nEnquire protected state with of the dynamic geometry."},
    // CALLBACK AND HANDLE SYSTEM
    {"cs2scb", s2plot_cs2scb, METH_VARARGS,"cs2scb(cbfn)\n\nSet the dynamic geometry callback function; use None as an argument to cancel callback. The callback function must be of the form: cbfn(time, keycount) where time is the current system time and keycount records the number of times that the space bar has been pressed.  cbfn has no return value.\n\n    The callback function can be toggled on/off by pressing the z key."},
    {"cs2ecb", s2plot_cs2ecb, METH_VARARGS,"cs2ecb()\n\nEnable the previously disabled dynamic geometry callback."},
    {"cs2dcb", s2plot_cs2dcb, METH_VARARGS,"cs2dcb()\n\nDisable the previously enabled dynamic geometry callback."},
    {"cs2tcb", s2plot_cs2tcb, METH_VARARGS,"cs2tcb()\n\nToggle the animation state of the current dynamic geometry callback. Has the same outcome as pressing the z key."},
    {"cs2skcb", s2plot_cs2skcb, METH_VARARGS, "cs2skcb(cb)\n\nSet the user-installed keyboard callback function.  Use None arg to disable the keyboard callback.  When registered, this function gets to process normal keyboard (ascii) presses before S2PLOT's internal handling.  It can consume the event (by returning 1) and prevent the default S2PLOT handling.  A return value of 0 should be used when the key was not processed. The callback function should have the signature:\n int kbdcallback(string key)\nShift-ESC cannot be over-ridden."},
    {"cs2sncb", s2plot_cs2sncb, METH_VARARGS,"cs2sncb(cbfn)\n\nSet the function to call when number-keys pressed.  Use None argument to disable callback.\nWhen registered, a callback overrides the default s2plot handling of keys 1-7 (camera views).\nThe callback must be of the form: cbfn(N) where N is an integer, the number-key that was pressed."},
    {"ds2ah", s2plot_ds2ah, METH_VARARGS, "ds2ah(iP, size, icol, ihilite, iid, iselected)\n\nAdd a handle to object at location iP, an {xyz} dict. The handle size is set by size, with colour icol when the handle is not selected and ihilite when it is selected (both are {rgb} dicts). iid is the id of the current handle and iselected gives its selection status.\n\n    To use handles, press Shift-s (show handles) and Shift-c (show cross-hairs). Note that selecting handles in a stereoscopic environment requires you to click on the point between the left and right eye views."},
    {"cs2shcb", s2plot_cs2shcb, METH_VARARGS, "cs2shcb(cbfn)\n\nSet the handle callback function; use None argument to cancel the callback. The handle callback function must be of the form: void cbfn(id) where id is the index of the selected handle."},
    {"cs2th", s2plot_cs2th, METH_VARARGS, "cs2th(iid)\n\nToggle the state of the named (dynamic) handle."},
    {"cs2qhv", s2plot_cs2qhv, METH_VARARGS, "cs2qhv()\n\nQuery the current state of the selection handle visibility. Returns 0 if handles are disabled or 1 if handles are enabled."},
    // ADVANCED TEXTURE AND COLORMAP HANDLING
    {"ss2lt", s2plot_ss2lt, METH_VARARGS, "ss2lt(itexturefn)\n\nLoad a texture for future (generally repeated) use. The texture file should be stored in the directory pointed to by environment variable S2PLOT_TEXPATH.  Returns an int, the id of the texture."},
    {"ss2gt", s2plot_ss2gt, METH_VARARGS, "ss2gt(textureID)\n\nGet an identified texture as a numpy array. The array is 3D, with indices: [width, height, {rgba}].  The 4-byte 3rd index r, g, b and alpha values [0,255]. If the texture is not found, None is returned."},
    {"ss2pt", s2plot_ss2pt, METH_VARARGS, "ss2pt(itextureID)\n\nReinstall a texture, eg. after modifying the map returned by ss2gt.  The texture must be numpy 3D of indices [width, height, {rgba}] of type Unsigned Bytes."},
    {"ss2ptt", s2plot_ss2ptt, METH_VARARGS, "ss2ptt(itextureID)\n\nReinstall the texture, but for a \"transient\" texture: this routine is considerably faster, but multiresolution versions are not created."},
    {"ss2lcm", s2plot_ss2lcm, METH_VARARGS, "ss2lcm(imapfile, startidx, maxn)\n\nLoad a colourmap into memory, starting at index startidx, read a maximum of maxn colours. Returns the number of entries read and stored. Map file format is per line:\n\n    index red green blue\n\n    with all integer values. Colour components are in the range [0,255]. The imapfile containing the colourmap should be stored in the directory pointed to by the environment variable S2PLOT_TEXPATH The index column is currently ignored. After calling this function, it is usual to call s2scir(startidx, startidx+retval-1) to activate this colormap for subsequent use."},
    // ENVIRONMENT AND RENDERING ATTRIBUTES
    {"ss2ssr", s2plot_ss2ssr, METH_VARARGS, "ss2ssr(res)\n\nSet sphere resolution. Spheres are drawn with (res*res) flat surfaces. Larger spheres (or spheres that will be viewed closer-up) require higher sphere resolutions. Be warned that rendering time takes a severe hit with resolutions much larger than about 12."},
    {"ss2srm", s2plot_ss2srm, METH_VARARGS, "ss2srm(mode)\n\nSet the rendering mode. Options are:\n    * WIREFRAME\n    * SHADE_FLAT\n    * SHADE_DIFFUSE and\n    * SHADE_SPECULAR"},
    {"ss2qrm", s2plot_ss2qrm, METH_VARARGS, "ss2qrm()\n\nGet the rendering mode. Returns values of the constants:\n    * WIREFRAME\n    * SHADE_FLAT\n    * SHADE_DIFFUSE and\n    * SHADE_SPECULAR"},
    {"ss2sl", s2plot_ss2sl, METH_VARARGS, "ss2sl(ambient, nlights, lightpos, lightcol, worldcoords)\n\nSet the entire lighting environment. The nlights in the scene are at positions lightpos (lists of {xyz} dicts) with colours lightcol (list of {rgb} dicts), and ambient lighting colour, as an {rgb} dict. If worldcoords > 0 then the caller has given world coordinates, otherwise they are viewport-relative coordinates."},
    {"ss2sbc", s2plot_ss2sbc, METH_VARARGS, "ss2sbc(r, g, b)\n\nSet the background colour. This call should almost always be followed by calls to s2scr to set the 0th colour index to be the same as the background, and the 1st colour index to be the opposite. Some S2PLOT internals always use white to draw text, and setting the background colour to a light value might result in some text being difficult or impossible to read."},
    {"ss2sfra", s2plot_ss2sfra, METH_VARARGS, "ss2sfra(rot)\n\nSet the fisheye rotation angle (degrees). This is only functional if the projection is in use is a fisheye, and it has the effect of rotating the projection \"pole\" away from the centre of the \"screen\", towards the bottom of the screen, by rot degrees."},
    {"ss2qpt", s2plot_ss2qpt, METH_VARARGS, "ss2qpt()\n\nFetch the projection type of the device in use. Return values are:\n    * 0 = perspective\n    * 1 = orthographic\n    * 2 = fisheye"},
    // ADVANCED CAMERA CONTROL
    {"ss2sc", s2plot_ss2sc, METH_VARARGS, "ss2sc(position, up, vdir, worldcoords)\n\nSet the camera position, up vector and view direction (vdir); all are {xyz} dicts. If worldcoords > 0 then caller has given world coordinates, otherwise they are viewport-relative coordinates."},
    {"ss2qc", s2plot_ss2qc, METH_VARARGS, "ss2qc(worldcoords)\n\nQuery the camera position, up vector and view direction (vdir), which are returned as a tuple of {xyz} dicts. If worldcoords > 0 then return world coordinates, otherwise return viewport-relative coordinates.\n\n    Please note that this function returns camera position of LAST update. Calling it immediately after ss2sc without allowing a refresh or redraw will not return identical arguments as given to ss2sc. The return value can be checked for this possibility: if non-zero, then programmed changes to the camera are still pending."},
    {"ss2sas", s2plot_ss2sas, METH_VARARGS, "ss2sas(startstop)\n\nStart (startstop = 1) or stop (startstop = 0) the camera rotation."},
    {"ss2scf", s2plot_ss2scf, METH_VARARGS, "ss2scf(position, worldcoords)\n\nSet the camera mid/focus point. If this is set, then rotation will be about the provided point (position, an {xyz} dict). Use ss2ucf to revert to \"auto\" mid point. If worldcoords > 0 then caller has given world coordinates, otherwise they are viewport-relative coordinates."},
    {"ss2ucf", s2plot_ss2ucf, METH_VARARGS, "ss2ucf()\n\nReset the camera mid/focus point so that an automatic mid-point is calculated. To set the focus manually, use ss2scf."},
    {"ss2sca", s2plot_ss2sca, METH_VARARGS, "ss2sca(aperture)\n\nSet the camera apaerture in degrees."}, /* NEW */
    {"ss2qca", s2plot_ss2qca, METH_VARARGS, "ss2qca()\n\nReturn the camera aperture in degrees."}, /* NEW */
    {"ss2sss", s2plot_ss2sss, METH_VARARGS, "ss2sss(spd)\n\nSet the spin/interaction speed to spd. Values between 0.01 and 20.0 are reasonably sensible."},
    {"ss2qss", s2plot_ss2qss, METH_VARARGS, "ss2qss()\n\nGet the current spin/interaction speed."},
    {"ss2scs", s2plot_ss2scs, METH_VARARGS, "ss2scs(spd)\n\nSet the camera speed to spd. Values between 0.01 and 20.0 are reasonably sensible."}, /* NEW */
    {"ss2qcs", s2plot_ss2qcs, METH_VARARGS, "ss2qcs()\n\nGet the current camera speed."}, /* NEW */
    {"ss2tc", s2plot_ss2tc, METH_VARARGS, "ss2tc(enabledisable)\n\nEnable (enabledisable = 1) or disable (enabledisable = 0) camera translation."},
    {"ss2qess", s2plot_ss2qess, METH_VARARGS, "ss2qess()\n\nQuery camera eye separation multiplier."},
    {"ss2sess", s2plot_ss2sess, METH_VARARGS, "ss2sess(sep)\n\nSet camera eye separation multiplier to sep (a float)."},
    // ADDITIONAL PRIMITIVES
    {"s2chromapts", s2plot_s2chromapts, METH_VARARGS, "s2chromapts(n, ilong, lat, dist, size, radius, dmin, dmax)\n\nPlot points on a sphere of given radius, at given longitude ilong and latitude ilat (both numpy arrays, in degrees), coloured by the current colormap. The numpy array, dist, gives the distance to each point. Index into map is calculated linearly between dmin and dmax. This function is so called because with the right colormap, a chromastereoscopic view will be produced for observing from the origin of the coordinate system - especially useful when using a fisheye projection. The numpy array, size, contains the desired sizes of the points."},
    {"s2chromacpts", s2plot_s2chromacpts, METH_VARARGS, "s2chromacpts(n, ix, iy, iz, dist, size, dmin, dmax)\n\nPlot points on a Cartesian grid at given locations (ix,iy,iz; all numpy arrays), coloured by the current colormap. The numpy array, dist, gives the distance to each point from the camera. Index into map is calculated linearly between dmin and dmax. This function is so called because with the right colormap, a chromastereoscopic view will be produced. The numpy array size contains the desired sizes of the points."},
// FUNCTIONS IN TESTING/DEVELOPMENT
    {"ss2ltt", s2plot_ss2ltt, METH_VARARGS, "ss2ltt(latex_command)\n\nCreate a texture with LaTeX commands.  The return value is a dict containing keys:\n'texture_id' - the texture handle (as used by eg. ns2vf4x etc)\n'aspect' - the x:y aspect ratio of the texture map."}, /* NEW */
    {"ns2vf3a", s2plot_ns2vf3a, METH_VARARGS, "ns2vf3a(P, col, trans, alpha)\n\nDraw a transparent 3-vertex facet with a single colour. The vertices are given by the 3-list, P, of {xyz} dicts, normals are calculated automatically, and the RGB colour is col, a {rgb} dict. Transparency is controlled by the alpha channel, with value in the range [0,1] and trans:\n\n        * trans = 'o' opaque vertex;\n        * trans = 't' addition blending - never gets dimmer; and\n        * trans = 's' standard blending - can get dimmer."},
    {"ns2vpa", s2plot_ns2vpa, METH_VARARGS, "ns2vpa(P, col, size, trans, alpha)\n\nDraw a transparent thick dot.\n* P - the location of the dot as an (x,y,z) dict.\n* col - the dot colour as an (r,g,b) dict.\n* size - the size of the dot as a float.\n* trans - 'o', 't' or 's' for opaque, additional blending, or standard blending respectively\n* alpha - the alpha in [0,1]."},
    {"ns2cis", s2plot_ns2cis, METH_VARARGS, "ns2cis(grid, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, tr, level, resolution, trans, alpha, red, green, blue)\n\nDraw an isosurface of a data volume, at given level. The data in grid is a 3D numpy array volume; in order to be able to dynamically modify the grid, it should be of numpy dtype: numpy.float32.  grid is indexed by:\n    grid[0..(adim-1)][0..(bdim-1)][0..(cdim-1)].\n\n    The slice of data actually plotted is indexed by [a1..a2][b1..b2][c1..c2] (as for s2vect3, for example).\n\n    tr is the transformation matrix (as a numpy array) which maps indices into the grid onto the x, y and z axes of the 3d world space. The standard (non-rotated, non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9] and tr[10] all zero.\n\n    The transformation matrix is:\n       x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic\n       y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic\n       z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic\n\n    Provide tr = NULL to use the default, unit transformation.\n\n    level gives the level at which the isosurface should be drawn. Skip every nth cell as given by resolution. If n = 1 then no skipping occurs.\n\n    Transparency of the surface is controlled by the alpha-channel parameter, alpha, between 0 and 1, and trans:\n\n        * trans = 'o' for opaque;\n        * trans = 't' for \"piling up\"; and\n        * trans = 's' for transparency that can occlude. \n\n    The RGB colour of the isosurface is provided by red, green and blue in the range [0,1]."},
    {"ns2cisc", s2plot_ns2cisc, METH_VARARGS, "ns2cisc(grid, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, tr, level, resolution, trans, alpha, fcol)\n\nDraw an isosurface of a data volume, at given level, using a function to calculate the colour over the surface. The data in grid is a 3D numpy array volume; in order to be able to dynamically modify the grid, it should be of numpy dtype: numpy.float32.  grid is indexed by:\n    grid[0..(adim-1)][0..(bdim-1)][0..(cdim-1)].\n\n    The slice of data actually plotted is indexed by [a1..a2][b1..b2][c1..c2] (as for s2vect3, for example).\n\n    tr is the transformation matrix (a numpy array) which maps indices into the grid onto the x, y and z axes of the 3d world space. The standard (non-rotated, non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9] and tr[10] all zero.\n\n    The transformation matrix is:\n       x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic\n       y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic\n       z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic\n\n    Provide tr = NULL to use the default, unit transformation.\n\n    level gives the level at which the isosurface should be drawn. Skip every nth cell as given by resolution. If n = 1 then no skipping occurs.\n\n    Transparency of the surface is controlled by the alpha-channel parameter, alpha, between 0 and 1, and trans:\n\n        * trans = 'o' for opaque;\n        * trans = 't' for \"piling up\"; and\n        * trans = 's' for transparency that can occlude. \n\n    The colour of the isosurface is specified by the function:\n\n       fcol(x, y, z)\n\n    which returns a colour dict with keys {rgb}."},
    {"ns2dis", s2plot_ns2dis, METH_VARARGS, "ns2dis(isid, force)\n\nDraw an isosurface object. Set force = 1 to force the surface to be recalculated, e.g. the grid or isosurface level has been changed. Using force = 0 will redisplay previously calculated version of isosurface."},
    {"ns2sisl", s2plot_ns2sisl, METH_VARARGS, "ns2sisl(isid, level)\n\nSelect the isosurface level for the isosurface identified by isid. Changes to the isosurface level will not be displayed until the next call to ns2dis with force = 1."},
    {"ns2sisa", s2plot_ns2sisa, METH_VARARGS, "ns2sisa(isid, alpha, trans)\n\nSet alpha and transparency of isosurface with id isid.  alpha is a float, and trans is a character."},
    {"ns2sisc", s2plot_ns2sisc, METH_VARARGS, "ns2sisc(isid, r, g, b)\n\nSet r, g, b colour of isosurface with id isid."},
    {"ns2cvr", s2plot_ns2cvr, METH_VARARGS, "ns2cvr(grid, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, tr, trans, datamin, datamax, alphamin, alphamax)\n\nCreate a volume rendering object. To display a volume render object you must use the function ds2dvr from within a dynamic callback.\n\n    The data in grid is a 3D numpy array volume; in order to be able to dynamically modify the grid, it should be of numpy dtype: numpy.float32. grid is indexed by:\n    grid[0..(adim-1)][0..(bdim-1)][0..(cdim-1)].\n\n    The slice of data actually plotted is indexed by [a1..a2][b1..b2][c1..c2].\n\n    tr is the transformation matrix (a numpy array) which maps indices into the grid onto the x, y and z axes of the 3d world space. The standard (non-rotated, non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9] and tr[10] all zero.\n\n    The transformation matrix is:\n       x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic\n       y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic\n       z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic\n\n    Provide tr = NULL to use the default, unit transformation.\n\n    Note that the voxels are pixel centred, so care must be taken with drawing bounding boxes (see the example code below for a solution).\n\n    datamin and datamax indicate the range of data values which are mapped to alphamin and alphamax. alpha is the transparency, with 0.0 corresponding to completely transparent (invisible) and 1.0 is opaque. Ordinarily, set datamin and datamax to bracket the signal region of your data, set alphamin to 0.0 and alphamax to something like 0.7.\n\n    There are three transparency modes, controlled by the parameter trans:\n\n        * trans = 'o' for opaque regardless of alpha settings;\n        * trans = 't' for transparency only; and\n        * trans = 's' is transparent allowing absoprtion. \n\n    The RGB colour of the isosurface is provided by red, green and blue in the range [0,1].\n\n    Set datamin > datamax to request auto-scaling.\n\n    Volume rendering works best when the render mode is set to SHADE_FLAT, and only light ambiently. See the example below."},
    {"ds2dvr", s2plot_ds2dvr, METH_VARARGS, "ds2dvr(vrid, force)\n\nDraw a volume rendering object (dynamic only). Set force to true to make the textures reload, e.g. if you have changed the values of the grid elements."},
    {"ns2svrl", s2plot_ns2svrl, METH_VARARGS, "ns2svrl(vrid, datamin, datamax, alphamin, alphamax)\n\nChange the volume rendering data and alpha range (\"level\") for vol rendering object with id, vrid.  After changing, be sure to call ds2dvr with force=1.  No protection is provided against datamin > datamax!"},
    {"ss2ct", s2plot_ss2ct, METH_VARARGS, "ss2ct(width, height)\n\nCreate a texture for the user to fill in as they see fit. Typical use is to call this function, then ss2gt and ss2pt to modify the texture as desired. Function returns the ID of the newly created texture."},
    {"ss2ctt", s2plot_ss2ctt, METH_VARARGS, "ss2ctt(width, height)\n\nCreate a texture as per ss2ct, but texture is for \"transient\" use: this means the texture is much faster to create, but multi-resolution versions are not constructed/used."},
    {"ss2dt", s2plot_ss2dt, METH_VARARGS, "ss2dt(texid)\n\nDelete a texture which is no longer required."},
    {"ss2txh", s2plot_ss2txh, METH_VARARGS, "ss2txh(enabledisable)\n\nEnable, disable or toggle the visibility of the cursor cross-hair depending on the value of enabledisable:\n\n        * enabledisable = 1 Enable cross-hair\n        * enabledisable = 0 Disable cross-hair\n        * enabledisable = -1 Toggle current state of cross-hair visibility \n\n    The cross-hair can also be toggled by using the key combination Cntrl-C."},
    {"ss2qxh", s2plot_ss2qxh, METH_VARARGS, "ss2qxh()\n\nQuery the current state of the cross-hair visibility. Returns False if cross-hair is disabled or True if cross-hair is enabled."},
    {"cs2thv", s2plot_cs2thv, METH_VARARGS, "cs2thv(enabledisable)\n\nEnable, disable or toggle the visibility of the selection handles depending on the value of enabledisable:\n\n        * enabledisable = 1 Enable selection handles\n        * enabledisable = 0 Disable selection handles\n        * enabledisable = -1 Toggle current state of selection handles \n\n    The selection handles can also be toggled by using the key combination Ctrl-S."},
    // DOCUSTRINGS NEED WORK HERE
    {"s2skypa", s2plot_s2skypa, METH_VARARGS, "s2skypa(data, nx, ny, i1, i2, j1, j2, datamin, datamax, tr, walls, idx_left, idx_front)\n\nCreate a \"skyscraper\" plot with arbitrary rotation / skew / translation.\n* data - a 2D numpy data array\n* nx, ny - length of data array in x and y directions\n* i1, i2 - first and last index on x-axis\n* j1, j2 - first and last index on y-axis\n* datamin/max - range of data to consider\n* tr - transformation matrix from datapoints to s2plot worldspace\n* walls - whether to draw the walls or not\n* idx_left/front - "},
    {"s2impa", s2plot_s2impa, METH_VARARGS, "s2impa(data, nx, ny, i1, i2, j1, j2, datamin, datamax, tr, trunk, symbol)\n\nAn \"impulse\" plot with arbitrary rotation / skew / translation.  Use point types as for s2pt.\n* data - a 2D numpy data array\n* nx, ny - length of data array in x and y directions\n* i1, i2 - first and last index on x-axis\n* j1, j2 - first and last index on y-axis\n* datamin/max - range of data to consider\n* tr - transformation matrix from datapoints to s2plot worldspace\n* trunk - boolean: whether to draw trunks of points or not."},
    {"ss2spt", s2plot_ss2spt, METH_VARARGS, "ss2spt(proj_type)\n\nSet the projection type: only certain changes are allowed."},
    {"ss2sfc", s2plot_ss2sfc, METH_VARARGS, "ss2sfc(red, green, blue)\n\nSet the foreground color.  This is only used for debugging (d) and key information (F1).  This should normally be followed by a call to s2scr to set the 1st colour index to a similar colour."},
    {"xs2ap", s2plot_xs2ap, METH_VARARGS, "xs2ap(x0, y0, x1, y1)\n\nAdd a new panel to the S2PLOT window.  The panel goes from (x0,y0) to (x1,y1) where these are fractions of the window coordinates.  Individual panels can be activated and deactivated by providing the panel id to the toggle function."},
    {"xs2tp", s2plot_xs2tp, METH_VARARGS, "xs2tp(panel_id)\n\nToggle whether a particular panel is active: panel_id = 1 for active, 0 for inactive.  Inactive panels are not drawn."},
    {"xs2cp", s2plot_xs2cp, METH_VARARGS, "xs2cp(panel_id)\n\nSelect the panel with id, panel_id, for subsequent geometry calls."},
    {"xs2mp", s2plot_xs2mp, METH_VARARGS, "xs2mp(panel_id, x0, y0, x1, y1)\n\nMovethe panel with id, panel_id, to (x0,y0), (x1,y1)."},
    {"xs2lpc", s2plot_xs2lpc, METH_VARARGS, "xs2lpc(master_id, slave_id)\n\nLink camera of panel with id \"slave_id\" to the camera of panel with id \"master_id\".  This *cannot* be undone!"},
    {"xs2qpa", s2plot_xs2qpa, METH_VARARGS, "xs2qpa(panel_id)\n\nQuery whether the specified panel active"},
    {"xs2qcpa", s2plot_xs2qcpa, METH_VARARGS, "xs2qcpa()\n\nReturns whether the currently selected panel is active"},
    {"xs2spp", s2plot_xs2spp, METH_VARARGS, "xs2spp(active, inactive, width)\n\nSet panel frame properties.  (COLOUR, COLOUR, float)"}, /* NEW */
    {"xs2qsp", s2plot_xs2qsp, METH_VARARGS, "xs2qsp()\n\nReturns the id of the currently selected panel."},
    //{"xs2lpg", s2plot_xs2lpg, METH_VARARGS, "xs2lpg(master_id, slave_id)\n\nLink geometry of panel with id \"slave_id\" to the geometry of panel with id \"master_id\".  This *cannot* be undone!"},
    {"ss2tsc", s2plot_ss2tsc, METH_VARARGS, "ss2tsc(screens)\n\nSet which screen coordinates to draw in, can be any of 'c', 'l', or 'r', or a combination of same.  Empty string means not screen coordinates."},
    {"s2qci", s2plot_s2qci, METH_VARARGS, "s2qci()\n\nQuery the color index."},
    {"s2qlw", s2plot_s2qlw, METH_VARARGS, "s2qlw()\n\nQuery the line width."},
    {"s2qls", s2plot_s2qls, METH_VARARGS, "s2qls()\n\nQuery the line style."},
    {"s2qch", s2plot_s2qch, METH_VARARGS, "s2qch()\n\nQuery character height."},
    {"s2qah", s2plot_s2qah, METH_VARARGS, "s2qah()\n\nQuery arrow-head style.  Returns a dict containing keys (fs, angle, barb)."},
    {"s2twc", s2plot_s2twc, METH_VARARGS, "s2twc(enabledisable)\n\nSet whether clipping is being applied to geometry.\n\nWhen enabled, clipping means points not within the world coordinate bounds as defined by the most recent call to s2win, will not be drawn.  The initial implementation applies to points only, future work may apply the clipping to lines and facets as well.  Enable/disable/toggle with 1,0,-1."}, /* NEW */
    {"s2qwc", s2plot_s2qwc, METH_VARARGS, "s2qwc()\n\nQuery whether clipping is being applied to geometry.  (See s2twc for more information).  Returns True or False."}, /* NEW */
    {"ss2qsr", s2plot_ss2qsr, METH_VARARGS, "ss2qsr()\n\nQuery sphere resolution."},
    {"ss2qbc", s2plot_ss2qbc, METH_VARARGS, "ss2qbc()\n\nQuery background colour.  Returns a dict with keys (r, g, b)."},
    {"ss2qfc", s2plot_ss2qfc, METH_VARARGS, "ss2qfc()\n\nQuery foreground colour.  Returns a dict with keys (r, g, b)."},
    {"ss2qfra", s2plot_ss2qfra, METH_VARARGS, "ss2qfra()\n\nQuery fisheye rotation angle (in degrees)."},
    {"ss2qas", s2plot_ss2qas, METH_VARARGS, "ss2qas()\n\nQuery autospin state."},
    {"ss2qcf", s2plot_ss2qcf, METH_VARARGS, "ss2qcf(world_coords)\n\nQuery the camera focus point, and whether it is explicitly set.  The results are in world coordinates if world_coords is True.  The return value is a dict with keys:\n* set - whether the focus \"set\" or \"unset\" - see functions ss2scf and ss2ucf.\n* 'position' - a dict with (x,y,z) keys denoting the camena position."},
    {"ss2qar", s2plot_ss2qar, METH_VARARGS, "ss2qar()\n\nQuery the aspect ratio: returned value is ratio of width to height\nie. it is normally > 1."},
    {"ss2qsa", s2plot_ss2qsa, METH_VARARGS, "ss2qsa()\n\nQuery the \"screen\" type, including stereo setting, fullscreen and dome status/type.  Returns a dict with keys:\n* stereo\n* fullscreen\n* dome\nGlobal module constants contain possible return values."},
    {"ss2qsd", s2plot_ss2qsd, METH_VARARGS, "ss2qsd()\n\nQuery screen (panel) width and height in PIXELS.\nReturn a dict with keys (x,y)."},
    {"ss2qnfp", s2plot_ss2qnfp, METH_VARARGS, "ss2qnfp()\n\nGet the actual near and far clip plane distances from the camera in DEVICE coordinates.  Returns a tuple of (near, far)."},
    {"cs2sdhcb", s2plot_cs2sdhcb, METH_VARARGS, "cs2sdhcb(cb)\n\nSet the drag handle callback function; use NULL argument to cancel callback.  The callback function must have the signature: cb(id, pt), where id is the integer id of the handle, and pt is the dict with (x,y,z) keys specifying a position."},
    {"cs2spcb", s2plot_cs2spcb, METH_VARARGS, "cs2spcb(cb, data)\n\nSet the callback function for the prompt functionality.  When the users presses '~', a prompt is placed on screen and they can type input.  When ENTER is pressed, the registered callback is called with the command string as argument.  If ESC is pressed instead, the prompt is dropped.  The signature of the callback must be cb(text, data) if data is not None, cb(text) otherwise"},
    {"cs2sptxy", s2plot_cs2sptxy, METH_VARARGS, "cs2sptxy(prompt, xfrac, yfrac)\n\nSet the text and position of the prompt.  xfrac and yfrac are fractions of the viewport from bottom left."}, /* NEW */
    {"cs2scbx", s2plot_cs2scbx, METH_VARARGS, "cs2scbx(cb, data)\n\nSet the dynamic callback function - this one takes a void data ptr which gets handed off to the callback when it is invoked.  Thus the callback has the signature cb(t, kc, data) or cb(t, kc) if data is None or unspecified."},
    {"ds2ahx", s2plot_ds2ahx, METH_VARARGS, "ds2ahx(pos, size, tex_id, ihitex, icol, ihilite, iid,  iselected))\n\nAdd a handle - with specified textures for unselected and selected."},
    //{"ss2ftt", s2plot_ss2ftt, METH_VARARGS, "ss2ftt()\n\n"},
    {"pushVRMLname", s2plot_pushVRMLname, METH_VARARGS, "pushVRMLname(name)\n\nPush the given name onto the VRML \"name stack\".  When VRML data is written, it is written in \"groups\" with these names."},
    //Q{"s2show_thr", s2plot_s2show_thr, METH_VARARGS, "s2show_thr()\n\nDraw the scene.  Argument interactive should be non-zero.  If zero, a warning will be issued.  Presence of this argument is historical.  This function never returns.  If you need to regain control after displaying graphics, consider using s2disp."},
    {"ss2wtga", s2plot_ss2wtga, METH_VARARGS, "ss2wtga(fname)\n\nWrite the current frame image to a named TGA file.  This can be used immediately after a call to s2disp to save a rendered image.  For effective use, s2disp must display the image long enough to go beyond the S2PLOT \"fade-in\" time; the best way to do this is to set the S2PLOT_FADETIME environment variable to 0.0, and then call s2disp(0,1).   Do not include the \".tga\" extension - this is added for you."},
    {"ss2gpix", s2plot_ss2gpix, METH_VARARGS, "ss2gpix()\n\nFetch the current frame image to an RGB buffer.  The buffer is a 3D numpy array with indices: [width, height, {r,g,b}].  THIS FUNCTION SHOULD ONLY BE USED IN non-stereo MODES."},
    {NULL, NULL, 0, NULL}     // Sentinel - marks the end of this structure
};

// helper functions
float   *numpy1D_to_float(PyArrayObject *numpyArray){
    // convert the numpy arrays into C arrays
    int n, i;
    float *result;

    if (PyArray_NDIM(numpyArray) != 1) {
        PyErr_SetString(PyExc_ValueError,
            "In numpy1D_to_float: array must be 1 dimensional.");
        return NULL;
    }
    n = PyArray_DIM(numpyArray, 0);

    if (PyArray_TYPE(numpyArray) == PyArray_FLOAT){
      result = (float *) PyArray_DATA(numpyArray);

        // add a python reference to the numpy array in case the user dels their ref to it
        Py_XINCREF(numpyArray);
        return result;
    } else if (PyArray_TYPE(numpyArray) == PyArray_DOUBLE){
        double *_result;

        _result = (double *) PyArray_DATA(numpyArray);
        result = (float *) malloc((size_t) (n*sizeof(float)));
        for(i = 0; i < n; i++){
            result[i] = (float) _result[i];
        }
        return result;
    } else {
        PyErr_SetString(PyExc_ValueError,
            "In numpy1D_to_float: array must be of type Float or Double.");
        return NULL;
    }
}
int     *numpy1D_to_int(PyArrayObject *numpyArray){
    // convert the numpy arrays into C arrays
    int n, i;
    int *result;

    if (PyArray_NDIM(numpyArray) != 1) {
        PyErr_SetString(PyExc_ValueError,
            "In numpy1D_to_int: array must be 1 dimensional.");
        return NULL;
    }
    n = PyArray_DIM(numpyArray, 0);

    if (PyArray_TYPE(numpyArray) == PyArray_INT){
        result = (int *) PyArray_DATA(numpyArray);

        // add a python reference to the numpy array in case the user dels their ref to it
        Py_XINCREF(numpyArray);
        return result;
    } else if (PyArray_TYPE(numpyArray) == PyArray_LONG){
        long *_result;

        _result = (long *) PyArray_DATA(numpyArray);
        result = (int *) malloc((size_t) (n*sizeof(int)));
        for(i = 0; i < n; i++){
            result[i] = (int) _result[i];
        }
        return result;
    } else {
        PyErr_SetString(PyExc_ValueError,
            "In numpy1D_to_float: array must be of type Float or Double.");
        return NULL;
    }
}
float  **numpy2D_to_float(PyArrayObject *numpyArray){
    int n, m, i, j, strides[2];
    float **result;
    
    if(PyArray_NDIM(numpyArray) != 2){
        PyErr_SetString(PyExc_ValueError,
            "In numpy2D_to_float: array must be 2 dimensional.");
        return NULL;
    }
    n = PyArray_DIM(numpyArray, 0);
    m = PyArray_DIM(numpyArray, 1);
    strides[0] = (int) PyArray_STRIDE(numpyArray, 0);
    strides[1] = (int) PyArray_STRIDE(numpyArray, 1);

    if (PyArray_TYPE(numpyArray) == PyArray_FLOAT){
        char *_ro;
        
        result = (float **) malloc((size_t) (n*sizeof(float*)));
        _ro = PyArray_DATA(numpyArray);
        for (i = 0; i < n; i++) {
            result[i] = (float *) (_ro + i*strides[0]);
        }
        // add a python reference to the numpy array in case the user dels their ref to it
        Py_XINCREF(numpyArray);
        return result;
    } else if (PyArray_TYPE(numpyArray) == PyArray_DOUBLE)  {
        char *_ro;
        double *ptr;
        
        result = (float **) malloc((size_t) (n*sizeof(float*)));
        _ro = PyArray_DATA(numpyArray);
        for (i = 0; i < n; i++) {
            result[i] = (float *) malloc((size_t) (m*sizeof(float)));
            for(j = 0; j < m; j++){
                ptr = (double *) (_ro + i*strides[0] + j*strides[1]);
                result[i][j] = (float) *ptr;
            }
        }
        return result;
    } else {
        PyErr_SetString(PyExc_ValueError,
            "In numpy2D_to_float: array must be of type Float or Double.");
        return NULL;
    }
}
float ***numpy3D_to_float(PyArrayObject *numpyArray){
    // convert the numpy matrices into C arrays
    int n,m,l, i,j,k, strides[3];
    float ***result;

    if (PyArray_NDIM(numpyArray) != 3){
        PyErr_SetString(PyExc_ValueError,
            "In numpy3D_to_float: array must be 3 dimensional.");
        return NULL;        
    }
    n = PyArray_DIM(numpyArray, 0);
    m = PyArray_DIM(numpyArray, 1);
    l = PyArray_DIM(numpyArray, 2);
    strides[0] = (int) PyArray_STRIDE(numpyArray, 0);
    strides[1] = (int) PyArray_STRIDE(numpyArray, 1);
    strides[2] = (int) PyArray_STRIDE(numpyArray, 2);

    if (PyArray_TYPE(numpyArray) == PyArray_FLOAT)  {
        char *dataPtr;
        
        result = (float ***) malloc((size_t) (n*sizeof(float**)));
        dataPtr = PyArray_DATA(numpyArray);
        for (i = 0; i < n; i++) {
            result[i] = (float **) malloc((size_t) (m*sizeof(float*)));
            for(j = 0; j < m; j++){
                result[i][j] = (float *) (dataPtr + i*strides[0] + j*strides[1]);
            }
        }
        // add a python reference to the numpy array in case the user deletes it
        Py_XINCREF(numpyArray);
        return result;
    } else if(PyArray_TYPE(numpyArray) == PyArray_DOUBLE){
        char *dataPtr;
        double *doublePtr;
        
        result = (float ***) malloc((size_t) (n*sizeof(float**)));
        dataPtr = PyArray_DATA(numpyArray);
        for (i = 0; i < n; i++) {
            result[i] = (float **) malloc((size_t) (m*sizeof(float*)));
            for(j = 0; j < m; j++){
                result[i][j] = (float *) malloc((size_t) (l*sizeof(float)));
                for(k = 0; k < l; k++){
                    doublePtr = (double *) (dataPtr +i*strides[0] + j*strides[1] + k*strides[2]);
                    result[i][j][k] = (float) *doublePtr;
                }
            }
        }
        return result;
    } else {
        PyErr_SetString(PyExc_ValueError,
            "In numpy3D_to_float: array must be of type Float or Double.");
        return NULL;
    }
}
void numpy_free(PyArrayObject *numpyArray, void *data){
    // just checks before freeing whether the memory is in use by a particular numpy object
    // if so, do nothing
  if(PyArray_DATA(numpyArray) != data){
        free(data);
    }
}
XYZ Dict_to_XYZ(PyObject *dict){
    XYZ out;
    
    out.x = (float) PyFloat_AsDouble(PyDict_GetItemString(dict,"x"));
    out.y = (float) PyFloat_AsDouble(PyDict_GetItemString(dict,"y"));
    out.z = (float) PyFloat_AsDouble(PyDict_GetItemString(dict,"z"));

    return out;
}
COLOUR Dict_to_COLOUR(PyObject *dict){
    COLOUR out;
    
    out.r = (float) PyFloat_AsDouble(PyDict_GetItemString(dict,"r"));
    out.g = (float) PyFloat_AsDouble(PyDict_GetItemString(dict,"g"));
    out.b = (float) PyFloat_AsDouble(PyDict_GetItemString(dict,"b"));

    return out;
}
PyObject *XYZ_to_Dict(XYZ xyz){
    PyObject *out;
    
    out = (PyObject *) PyDict_New();
    if(out == NULL) return NULL;
    
    PyDict_SetItemString(out,"x",PyFloat_FromDouble((double) xyz.x));
    PyDict_SetItemString(out,"y",PyFloat_FromDouble((double) xyz.y));
    PyDict_SetItemString(out,"z",PyFloat_FromDouble((double) xyz.z));
    
    return out;
}
PyObject *COLOUR_to_Dict(COLOUR colour){
    PyObject *out;
    
    out = (PyObject *) PyDict_New();
    if(out == NULL) return NULL;
    
    PyDict_SetItemString(out,"r",PyFloat_FromDouble((double) colour.r));
    PyDict_SetItemString(out,"g",PyFloat_FromDouble((double) colour.g));
    PyDict_SetItemString(out,"b",PyFloat_FromDouble((double) colour.b));
    
    return out;
}

// init the module definition...
PyMODINIT_FUNC init_s2plot(void){
    (void) Py_InitModule3("_s2plot", S2PlotMethods,"A literal implementation of the s2plot library.");
    // the following line necessary for numpy
    import_array();
}

// OPENING, CLOSING AND SELECTING DEVICES
static PyObject *s2plot_s2open(PyObject *self, PyObject *args){
    int fullscreen, stereo, argc, result;
    PyObject *argvList, *pyResult;
    const char **argv;
                 
    if(!PyArg_ParseTuple(args,"iiiO:s2open",&fullscreen,&stereo,&argc,&argvList)){
        return NULL;
    }
    // convert th PyObject_List to a char**
    int len = (int) PyList_Size(argvList);
    argv = (const char **) malloc(sizeof(char *)*len);
    
    int i;
    for(i = 0; i < len; i++){
        argv[i] = PyString_AsString(PyList_GetItem(argvList,i));
    }
    
    result = s2open(fullscreen,stereo,argc, (char **) argv);
    
    free(argv);
    
    pyResult = PyInt_FromLong((long) result);
    
    Py_INCREF(pyResult);
    return pyResult;
}                
static PyObject *s2plot_s2opend(PyObject *self, PyObject *args){
    const char *device;
    int argc, result;
    PyObject *argvList, *pyResult;
    const char **argv;
    
    if(!PyArg_ParseTuple(args,"siO:s2opend",&device,&argc,&argvList)){
        return NULL;
    }            
    // convert th PyObject_List to a char**
    int len = (int) PyList_Size(argvList);
    argv = (const char **) malloc(sizeof(char *)*len);
    
    int i;
    for(i = 0; i < len; i++){
        argv[i] = PyString_AsString(PyList_GetItem(argvList,i));
    }            
    
    result = s2opend((char *) device, argc, (char **) argv);
    
    free(argv);  
    
    pyResult = PyInt_FromLong((long) result);
    
    Py_INCREF(pyResult);
    return pyResult;
}                
static PyObject *s2plot_s2opendo(PyObject *self, PyObject *args){
    const char *device;
    int result;
    PyObject * pyResult;
                 
    if(!PyArg_ParseTuple(args,"s:s2opendo",&device)){
        return NULL;
    }
    
    result = s2opendo((char *) device);
    
    pyResult = PyInt_FromLong((long) result);
    
    Py_INCREF(pyResult);
    return pyResult;
}                
static PyObject *s2plot_s2show(PyObject *self, PyObject *args){
    int interactive;
    
    if(!PyArg_ParseTuple(args,"i:s2show",&interactive)){
        return NULL;
    }
    
    s2show(interactive);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2disp(PyObject *self, PyObject *args){
    int idelay, irestorecamera;
    
    if(!PyArg_ParseTuple(args,"ii:s2disp",&idelay,&irestorecamera)){
        return NULL;
    }
    
    s2disp(idelay,irestorecamera);
    
    Py_INCREF(Py_None);
    
    return Py_None;
}
static PyObject *s2plot_s2ldev(PyObject *self, PyObject *args){
    s2ldev();
    
    Py_INCREF(Py_None);
    return Py_None;
}                
static PyObject *s2plot_s2eras(PyObject *self, PyObject *args){
    s2eras();
    
    Py_INCREF(Py_None);
    return Py_None;
}                
// WINDOWS AND VIEWPORTS
static PyObject *s2plot_s2svp(PyObject *self, PyObject *args){
    float x[2],y[2],z[2];
                 
    if(!PyArg_ParseTuple(args,"ffffff:s2svp",&x[0],&x[1],&y[0],&y[1],&z[0],&z[1])){
        return NULL;
    }
    
    s2svp(x[0],x[1],y[0],y[1],z[0],z[1]);
                 
    Py_INCREF(Py_None);
    return Py_None;
}                
static PyObject *s2plot_s2qvp(PyObject *self, PyObject *args){
    float x[2],y[2],z[2];
    PyObject *resultList;
    
    s2qvp(&x[0],&x[1],&y[0],&y[1],&z[0],&z[1]);
    
    resultList = (PyObject *) PyTuple_New(6);
    if(resultList == NULL)
        return NULL;
    
    PyTuple_SetItem(resultList,0,PyFloat_FromDouble((double) x[0]));
    PyTuple_SetItem(resultList,1,PyFloat_FromDouble((double) x[1]));
    PyTuple_SetItem(resultList,2,PyFloat_FromDouble((double) y[0]));
    PyTuple_SetItem(resultList,3,PyFloat_FromDouble((double) y[1]));
    PyTuple_SetItem(resultList,4,PyFloat_FromDouble((double) z[0]));
    PyTuple_SetItem(resultList,5,PyFloat_FromDouble((double) z[1]));
    Py_INCREF(resultList);
    
    return resultList;
}
static PyObject *s2plot_s2swin(PyObject *self, PyObject *args){
    float x[2],y[2],z[2];
                 
    if(!PyArg_ParseTuple(args,"ffffff:s2swin",&x[0],&x[1],&y[0],&y[1],&z[0],&z[1])){
        return NULL;
    }
    
    s2swin(x[0],x[1],y[0],y[1],z[0],z[1]);
    
    Py_INCREF(Py_None);
    return Py_None;
}                
static PyObject *s2plot_s2qwin(PyObject *self, PyObject *args){
    float x[2],y[2],z[2];
    PyObject *resultList;
    
    s2qwin(&x[0],&x[1],&y[0],&y[1],&z[0],&z[1]);
    
    resultList = (PyObject *) PyTuple_New(6);
    if(resultList == NULL)
        return NULL;
    
    PyTuple_SetItem(resultList,0,PyFloat_FromDouble((double) x[0]));
    PyTuple_SetItem(resultList,1,PyFloat_FromDouble((double) x[1]));
    PyTuple_SetItem(resultList,2,PyFloat_FromDouble((double) y[0]));
    PyTuple_SetItem(resultList,3,PyFloat_FromDouble((double) y[1]));
    PyTuple_SetItem(resultList,4,PyFloat_FromDouble((double) z[0]));
    PyTuple_SetItem(resultList,5,PyFloat_FromDouble((double) z[1]));
    Py_INCREF(resultList);
    
    return resultList;
}
static PyObject *s2plot_s2env(PyObject *self, PyObject *args){
    float x[2], y[2], z[2];
    int just, axis;
    
    if(!PyArg_ParseTuple(args,"ffffffii:s2env",&x[0], &x[1], &y[0], &y[1], &z[0], &z[1], &just, &axis)){
        return NULL;
    }
    
    s2env(x[0],x[1],y[0],y[1],z[0],z[1],just,axis);
    
    Py_INCREF(Py_None);
    return Py_None;
}
// PRIMITIVES
static PyObject *s2plot_s2line(PyObject *self, PyObject *args){
    int n;
    PyArrayObject *xIn, *yIn, *zIn;
    float *xPts, *yPts, *zPts;
    int symbol;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"iO!O!O!:s2line",&n,&PyArray_Type,&xIn,&PyArray_Type,&yIn,&PyArray_Type,&zIn,&symbol) || NULL == &xIn || NULL == &yIn || NULL == &zIn){
        return NULL;
    }
    if(!(xPts = numpy1D_to_float(xIn))) return NULL;
    if(!(yPts = numpy1D_to_float(yIn))) return NULL;
    if(!(zPts = numpy1D_to_float(zIn))) return NULL;
    
    s2line(n, xPts, yPts, zPts);
    
    numpy_free(xIn, xPts);
    numpy_free(yIn, yPts);
    numpy_free(zIn, zPts);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2circxy(PyObject *self, PyObject *args){
    float px, py, pz, r, asp;
    int nseg;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"ffffif:s2circxy",&px, &py, &pz, &r, &nseg, &asp)){
        return NULL;
    }

    s2circxy(px, py, pz, r, nseg, asp);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2circxz(PyObject *self, PyObject *args){
    float px, py, pz, r, asp;
    int nseg;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"ffffif:s2circxz",&px, &py, &pz, &r, &nseg, &asp)){
        return NULL;
    }

    s2circxz(px, py, pz, r, nseg, asp);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2circyz(PyObject *self, PyObject *args){
    float px, py, pz, r, asp;
    int nseg;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"ffffif:s2circyz",&px, &py, &pz, &r, &nseg, &asp)){
        return NULL;
    }

    s2circyz(px, py, pz, r, nseg, asp);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2diskxy(PyObject *self, PyObject *args){
    float px, py, pz, r1, r2;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffff:s2diskxy",&px, &py, &pz, &r1, &r2)){
        return NULL;
    }

    s2diskxy(px, py, pz, r1, r2);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2diskxz(PyObject *self, PyObject *args){
    float px, py, pz, r1, r2;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffff:s2diskxz",&px, &py, &pz, &r1, &r2)){
        return NULL;
    }

    s2diskxz(px, py, pz, r1, r2);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2diskyz(PyObject *self, PyObject *args){
    float px, py, pz, r1, r2;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffff:s2diskyz",&px, &py, &pz, &r1, &r2)){
        return NULL;
    }

    s2diskyz(px, py, pz, r1, r2);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2rectxy(PyObject *self, PyObject *args){
    float xmin, xmax, ymin, ymax, z;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffff:s2rectxy", &xmin, &xmax, &ymin, &ymax, &z)){
        return NULL;
    }

    s2rectxy(xmin, xmax, ymin, ymax, z);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2rectxz(PyObject *self, PyObject *args){
    float xmin, xmax, zmin, zmax, y;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffff:s2rectxz", &xmin, &xmax, &zmin, &zmax, &y)){
        return NULL;
    }

    s2rectxz(xmin, xmax, zmin, zmax, y);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2rectyz(PyObject *self, PyObject *args){
    float ymin, ymax, zmin, zmax, x;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffff:s2rectyz", &ymin, &ymax, &zmin, &zmax, &x)){
        return NULL;
    }

    s2rectyz(ymin, ymax, zmin, zmax, x);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2wcube(PyObject *self, PyObject *args){ /* NEW */
    float xmin, xmax, ymin, ymax, zmin, zmax;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"ffffff:s2wcube", &xmin, &xmax, &ymin, &ymax, &zmin, &zmax)){
        return NULL;
    }

    s2wcube(xmin, xmax, ymin, ymax, zmin, zmax);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2pt1(PyObject *self, PyObject *args){
    float x, y, z;
    int symbol;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffi:s2pt1",&x, &y, &z, &symbol)){
        return NULL;
    }

    s2pt1(x, y, z, symbol);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2pt(PyObject *self, PyObject *args){
    int N;
    PyArrayObject *xIn, *yIn, *zIn;
    float *xCoord, *yCoord, *zCoord;
    int symbol;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"iO!O!O!i:s2pt",&N,&PyArray_Type,&xIn,&PyArray_Type,&yIn,&PyArray_Type,&zIn,&symbol) || NULL == &xIn || NULL == &yIn || NULL == &zIn){
        return NULL;
    }
    if(!(xCoord = numpy1D_to_float(xIn))) return NULL;
    if(!(yCoord = numpy1D_to_float(yIn))) return NULL;
    if(!(zCoord = numpy1D_to_float(zIn))) return NULL;
    
    s2pt(N,xCoord,yCoord,zCoord,symbol);
    
    numpy_free(xIn, xCoord);
    numpy_free(yIn, yCoord);
    numpy_free(zIn, zCoord);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2pnts(PyObject *self, PyObject *args){
    int np, ns, *symbols;
    PyArrayObject *xIn, *yIn, *zIn, *symbolsIn;
    float *xPts, *yPts, *zPts;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"iO!O!O!O!i:s2pnts", &np, &PyArray_Type, &xIn, &PyArray_Type, &yIn, &PyArray_Type, &zIn, &PyArray_Type, &symbolsIn, &ns) || NULL == &xIn || NULL == &yIn || NULL == &zIn || NULL == &symbolsIn){
        return NULL;
    }
    if(!(xPts = numpy1D_to_float(xIn))) return NULL;
    if(!(yPts = numpy1D_to_float(yIn))) return NULL;
    if(!(zPts = numpy1D_to_float(zIn))) return NULL;
    if(!(symbols = numpy1D_to_int(symbolsIn))) return NULL;
        
    s2pnts(np, xPts, yPts, zPts, symbols, ns);    

    numpy_free(xIn, xPts);
    numpy_free(yIn, yPts);
    numpy_free(zIn, zPts);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2textxy(PyObject *self, PyObject *args){
    float x, y, z;
    char *text;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffs:s2textxy", &x, &y, &z, &text)){
        return NULL;
    }

    s2textxy(x, y, z, text);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2textxz(PyObject *self, PyObject *args){
    float x, y, z;
    char *text;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffs:s2textxz", &x, &y, &z, &text)){
        return NULL;
    }

    s2textxz(x, y, z, text);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2textyz(PyObject *self, PyObject *args){
    float x, y, z;
    char *text;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffs:s2textyz", &x, &y, &z, &text)){
        return NULL;
    }

    s2textyz(x, y, z, text);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2textxyf(PyObject *self, PyObject *args){
    float x, y, z, flipx, flipy;
    char *text;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffffs:s2textxyf", &x, &y, &z, &flipx, &flipy, &text)){
        return NULL;
    }

    s2textxyf(x, y, z, flipx, flipy, text);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2textxzf(PyObject *self, PyObject *args){
    float x, y, z, flipx, flipz;
    char *text;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffffs:s2textxzf", &x, &y, &z, &flipx, &flipz, &text)){
        return NULL;
    }

    s2textxzf(x, y, z, flipx, flipz, text);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2textyzf(PyObject *self, PyObject *args){
    float x, y, z, flipy, flipz;
    char *text;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffffs:s2textyzf", &x, &y, &z, &flipy, &flipz, &text)){
        return NULL;
    }

    s2textyzf(x, y, z, flipy, flipz, text);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2qtxtxy(PyObject *self, PyObject *args){
    float x, y, z, pad;
    float x1, x2, y1, y2;
    char *text;
    PyObject *result, *resultX, *resultY;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffsf:s2qtxtxy", &x, &y, &z, &text, &pad)){
        return NULL;
    }

    s2qtxtxy(&x1, &x2, &y1, &y2, x, y, z, text, pad);

    result = (PyObject *) PyTuple_New(2);
    resultX = (PyObject *) PyTuple_New(2);
    resultY = (PyObject *) PyTuple_New(2);

    if(result == NULL || resultX == NULL || resultY == NULL)
        return NULL;
    
    PyTuple_SetItem(resultX, 0, PyFloat_FromDouble((double) x1));
    PyTuple_SetItem(resultX, 1, PyFloat_FromDouble((double) x2));
    PyTuple_SetItem(resultY, 0, PyFloat_FromDouble((double) y1));
    PyTuple_SetItem(resultY, 1, PyFloat_FromDouble((double) y2));
    PyTuple_SetItem(result, 0, resultX);
    PyTuple_SetItem(result, 1, resultY);
    Py_INCREF(resultX);
    Py_INCREF(resultY);
    Py_INCREF(result);
    
    return result;
}
static PyObject *s2plot_s2qtxtxz(PyObject *self, PyObject *args){
    float x, y, z, pad;
    float x1, x2, z1, z2;
    char *text;
    PyObject *result, *resultX, *resultZ;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffsf:s2qtxtxz", &x, &y, &z, &text, &pad)){
        return NULL;
    }

    s2qtxtxz(&x1, &x2, &z1, &z2, x, y, z, text, pad);

    result = (PyObject *) PyTuple_New(2);
    resultX = (PyObject *) PyTuple_New(2);
    resultZ = (PyObject *) PyTuple_New(2);

    if(result == NULL || resultX == NULL || resultZ == NULL)
        return NULL;
    
    PyTuple_SetItem(resultX, 0, PyFloat_FromDouble((double) x1));
    PyTuple_SetItem(resultX, 1, PyFloat_FromDouble((double) x2));
    PyTuple_SetItem(resultZ, 0, PyFloat_FromDouble((double) z1));
    PyTuple_SetItem(resultZ, 1, PyFloat_FromDouble((double) z2));
    PyTuple_SetItem(result, 0, resultX);
    PyTuple_SetItem(result, 1, resultZ);
    Py_INCREF(resultX);
    Py_INCREF(resultZ);
    Py_INCREF(result);
    
    return result;
}
static PyObject *s2plot_s2qtxtyz(PyObject *self, PyObject *args){
    float x, y, z, pad;
    float y1, y2, z1, z2;
    char *text;
    PyObject *result, *resultY, *resultZ;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffsf:s2qtxtyz", &x, &y, &z, &text, &pad)){
        return NULL;
    }

    s2qtxtyz(&y1, &y2, &z1, &z2, x, y, z, text, pad);

    result = (PyObject *) PyTuple_New(2);
    resultY = (PyObject *) PyTuple_New(2);
    resultZ = (PyObject *) PyTuple_New(2);

    if(result == NULL || resultY == NULL || resultZ == NULL)
        return NULL;
    
    PyTuple_SetItem(resultY, 0, PyFloat_FromDouble((double) y1));
    PyTuple_SetItem(resultY, 1, PyFloat_FromDouble((double) y2));
    PyTuple_SetItem(resultZ, 0, PyFloat_FromDouble((double) z1));
    PyTuple_SetItem(resultZ, 1, PyFloat_FromDouble((double) z2));
    PyTuple_SetItem(result, 0, resultY);
    PyTuple_SetItem(result, 1, resultZ);
    Py_INCREF(resultY);
    Py_INCREF(resultZ);
    Py_INCREF(result);
    
    return result;
}
static PyObject *s2plot_s2qtxtxyf(PyObject *self, PyObject *args){
    float x, y, z, flipx, flipy, pad;
    float x1, x2, y1, y2;
    char *text;
    PyObject *result, *resultX, *resultY;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffffsf:s2qtxtxyf", &x, &y, &z, &flipx, &flipy, &text, &pad)){
        return NULL;
    }

    s2qtxtxyf(&x1, &x2, &y1, &y2, x, y, z, flipx, flipy, text, pad);

    result = (PyObject *) PyTuple_New(2);
    resultX = (PyObject *) PyTuple_New(2);
    resultY = (PyObject *) PyTuple_New(2);

    if(result == NULL || resultX == NULL || resultY == NULL)
        return NULL;
    
    PyTuple_SetItem(resultX, 0, PyFloat_FromDouble((double) x1));
    PyTuple_SetItem(resultX, 1, PyFloat_FromDouble((double) x2));
    PyTuple_SetItem(resultY, 0, PyFloat_FromDouble((double) y1));
    PyTuple_SetItem(resultY, 1, PyFloat_FromDouble((double) y2));
    PyTuple_SetItem(result, 0, resultX);
    PyTuple_SetItem(result, 1, resultY);
    Py_INCREF(resultX);
    Py_INCREF(resultY);
    Py_INCREF(result);
    
    return result;
}
static PyObject *s2plot_s2qtxtxzf(PyObject *self, PyObject *args){
    float x, y, z, flipx, flipz, pad;
    float x1, x2, z1, z2;
    char *text;
    PyObject *result, *resultX, *resultZ;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffffsf:s2qtxtxzf", &x, &y, &z, &flipx, &flipz, &text, &pad)){
        return NULL;
    }

    s2qtxtxzf(&x1, &x2, &z1, &z2, x, y, z, flipx, flipz, text, pad);

    result = (PyObject *) PyTuple_New(2);
    resultX = (PyObject *) PyTuple_New(2);
    resultZ = (PyObject *) PyTuple_New(2);

    if(result == NULL || resultX == NULL || resultZ == NULL)
        return NULL;
    
    PyTuple_SetItem(resultX, 0, PyFloat_FromDouble((double) x1));
    PyTuple_SetItem(resultX, 1, PyFloat_FromDouble((double) x2));
    PyTuple_SetItem(resultZ, 0, PyFloat_FromDouble((double) z1));
    PyTuple_SetItem(resultZ, 1, PyFloat_FromDouble((double) z2));
    PyTuple_SetItem(result, 0, resultX);
    PyTuple_SetItem(result, 1, resultZ);
    Py_INCREF(resultX);
    Py_INCREF(resultZ);
    Py_INCREF(result);
    
    return result;
}
static PyObject *s2plot_s2qtxtyzf(PyObject *self, PyObject *args){
    float x, y, z, flipy, flipz, pad;
    float y1, y2, z1, z2;
    char *text;
    PyObject *result, *resultY, *resultZ;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"fffffsf:s2qtxtyzf", &x, &y, &z, &flipy, &flipz, &text, &pad)){
        return NULL;
    }

    s2qtxtyzf(&y1, &y2, &z1, &z2, x, y, z, flipy, flipz, text, pad);

    result = (PyObject *) PyTuple_New(2);
    resultY = (PyObject *) PyTuple_New(2);
    resultZ = (PyObject *) PyTuple_New(2);

    if(result == NULL || resultY == NULL || resultZ == NULL)
        return NULL;
    
    PyTuple_SetItem(resultY, 0, PyFloat_FromDouble((double) y1));
    PyTuple_SetItem(resultY, 1, PyFloat_FromDouble((double) y2));
    PyTuple_SetItem(resultZ, 0, PyFloat_FromDouble((double) z1));
    PyTuple_SetItem(resultZ, 1, PyFloat_FromDouble((double) z2));
    PyTuple_SetItem(result, 0, resultY);
    PyTuple_SetItem(result, 1, resultZ);
    Py_INCREF(resultY);
    Py_INCREF(resultZ);
    Py_INCREF(result);
    
    return result;
}
static PyObject *s2plot_s2arro(PyObject *self, PyObject *args){
    float x[2], y[2], z[2];
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"ffffff:s2arro",&x[0], &y[0], &z[0], &x[1], &y[1], &z[1])){
        return NULL;
    }

    s2arro(x[0], y[0], z[0], x[1], y[1], z[1]);

    Py_INCREF(Py_None);
    return Py_None;
}
// ATTRIBUTES
static PyObject *s2plot_s2sci(PyObject *self, PyObject *args){
    int idx;
    
    if(!PyArg_ParseTuple(args,"i:s2sci",&idx)){
        return NULL;
    }
    
    s2sci(idx);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2scr(PyObject *self, PyObject *args){
    int idx;
    float r, g, b;
    
    if(!PyArg_ParseTuple(args,"ifff:s2scr",&idx,&r,&g,&b)){
        return NULL;
    }
    
    s2scr(idx, r, g, b);

    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2qcr(PyObject *self, PyObject *args){
    int idx;
    float r, g, b;
    PyObject *resultList;
    
    if(!(PyArg_ParseTuple(args, "i:s2qcr", &idx))){
        return NULL;
    }
    
    s2qcr(idx, &r, &g, &b);

    resultList = (PyObject *) PyDict_New();
    if(resultList == NULL)
        return NULL;
    
    PyDict_SetItemString(resultList,"r",PyFloat_FromDouble((double) r));
    PyDict_SetItemString(resultList,"g",PyFloat_FromDouble((double) g));
    PyDict_SetItemString(resultList,"b",PyFloat_FromDouble((double) b));
    
    Py_INCREF(resultList);
    return resultList;
}
static PyObject *s2plot_s2slw(PyObject *self, PyObject *args){
    float width;
    
    if(!PyArg_ParseTuple(args,"f:s2slw",&width)){
        return NULL;
    }
    
    s2slw(width);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2sls(PyObject *self, PyObject *args){
    int style;
    
    if(!PyArg_ParseTuple(args,"i:s2sls",&style)){
        return NULL;
    }
    
    s2sls(style);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2sch(PyObject *self, PyObject *args){
    float size;
    
    if(!PyArg_ParseTuple(args,"f:s2sch",&size)){
        return NULL;
    }
    
    s2sch(size);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2sah(PyObject *self, PyObject *args){
    int fs;
    float angle, barb;
    
    if(!PyArg_ParseTuple(args,"iff:s2sah",&fs, &angle, &barb)){
        return NULL;
    }
    
    s2sah(fs, angle, barb);
    
    Py_INCREF(Py_None);
    return Py_None;
}
// AXES, BOXES & LABELS
static PyObject *s2plot_s2box(PyObject *self, PyObject *args){
    char *xopt, *yopt, *zopt;
    float xtick, ytick, ztick;
    int nxsub, nysub, nzsub;
    
    if(!PyArg_ParseTuple(args,"sfisfisfi:s2box",&xopt,&xtick,&nxsub,&yopt,&ytick,&nysub,&zopt,&ztick,&nzsub)){
        return NULL;
    }
    
    s2box(xopt,xtick,nxsub,yopt,ytick,nysub,zopt,ztick,nzsub);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2lab(PyObject *self, PyObject *args){
    char *xlab, *ylab, *zlab, *title;
    
    if(!PyArg_ParseTuple(args,"ssss:s2lab",&xlab, &ylab, &zlab, &title)){
        return NULL;
    }
    
    s2lab(xlab, ylab, zlab, title);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2help(PyObject *self, PyObject *args){ /* NEW */
    char *helpstr;

    if(!PyArg_ParseTuple(args,"s:s2help",&helpstr)){
        return NULL;
    }
    
    s2help(helpstr);
    
    Py_INCREF(Py_None);
    return Py_None;
}
// XY(Z) PLOTS
static PyObject *s2plot_s2errb(PyObject *self, PyObject *args){
    int dir, n, termsize;
    PyArrayObject *xPtsIn, *yPtsIn, *zPtsIn, *eDeltIn;
    float *xPts, *yPts, *zPts, *eDelt;
    
    if(!PyArg_ParseTuple(args,"iiO!O!O!O!i:s2errb",&dir, &n, &PyArray_Type, &xPtsIn, &PyArray_Type, &yPtsIn, &PyArray_Type, &zPtsIn, &PyArray_Type, &eDeltIn, &termsize) || NULL == xPtsIn || NULL == yPtsIn || NULL == zPtsIn || NULL == eDeltIn){
        return NULL;
    }
    
    if(!(xPts = numpy1D_to_float(xPtsIn))) return NULL;
    if(!(yPts = numpy1D_to_float(yPtsIn))) return NULL;
    if(!(zPts = numpy1D_to_float(zPtsIn))) return NULL;
    if(!(eDelt = numpy1D_to_float(eDeltIn))) return NULL;
    
    s2errb(dir, n, xPts, yPts, zPts, eDelt, termsize);
    
    numpy_free(xPtsIn, xPts);
    numpy_free(yPtsIn, yPts);
    numpy_free(zPtsIn, zPts);
    numpy_free(eDeltIn, eDelt);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *pyParametric_fx = NULL;
static PyObject *pyParametric_fy = NULL;
static PyObject *pyParametric_fz = NULL;
static PyObject *pyParametric_fc = NULL;
float cParametricFunctions_fx(float *t){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("(f)", *t);
    pyResult = PyEval_CallObject(pyParametric_fx, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
float cParametricFunctions_fy(float *t){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("(f)", *t);
    
    pyResult = PyEval_CallObject(pyParametric_fy, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
        
    return result;
}
float cParametricFunctions_fz(float *t){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("(f)", *t);
    
    pyResult = PyEval_CallObject(pyParametric_fz, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
float cParametricFunctions_fc(float *t){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("(f)", *t);
    
    pyResult = PyEval_CallObject(pyParametric_fc, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
static PyObject *s2plot_s2funt(PyObject *self, PyObject *args){
    int N;
    float tmin, tmax;
    PyObject *tempX, *tempY, *tempZ;
    
    if(!PyArg_ParseTuple(args, "OOOiff:s2funt", &tempX, &tempY, &tempZ, &N, &tmin, &tmax)) {
        return NULL;
    }
    if (!PyCallable_Check(tempX) || !PyCallable_Check(tempY) || !PyCallable_Check(tempZ)) {
        PyErr_SetString(PyExc_TypeError, "parameters must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempX);
    Py_XINCREF(tempY);
    Py_XINCREF(tempZ);
    Py_XDECREF(pyParametric_fx);
    Py_XDECREF(pyParametric_fy);
    Py_XDECREF(pyParametric_fz);
    
    pyParametric_fx = tempX;
    pyParametric_fy = tempY;
    pyParametric_fz = tempZ;

    s2funt(cParametricFunctions_fx, cParametricFunctions_fy, cParametricFunctions_fz, N, tmin, tmax);        

    Py_INCREF(Py_None);
    return Py_None; 
}
static PyObject *s2plot_s2funtc(PyObject *self, PyObject *args){
    int N;
    float tmin, tmax;
    PyObject *tempX, *tempY, *tempZ, *tempC;
    
    if(!PyArg_ParseTuple(args, "OOOOiff:s2funtc", &tempX, &tempY, &tempZ, &tempC, &N, &tmin, &tmax)) {
        return NULL;
    }
    if (!PyCallable_Check(tempX) || !PyCallable_Check(tempY) || !PyCallable_Check(tempZ) || !PyCallable_Check(tempC)) {
        PyErr_SetString(PyExc_TypeError, "parameters must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempX);
    Py_XINCREF(tempY);
    Py_XINCREF(tempZ);
    Py_XINCREF(tempC);
    Py_XDECREF(pyParametric_fx);
    Py_XDECREF(pyParametric_fy);
    Py_XDECREF(pyParametric_fz);
    Py_XDECREF(pyParametric_fc);
    
    pyParametric_fx = tempX;
    pyParametric_fy = tempY;
    pyParametric_fz = tempZ;
    pyParametric_fc = tempC;

    s2funtc(cParametricFunctions_fx, cParametricFunctions_fy, cParametricFunctions_fz, cParametricFunctions_fc, N, tmin, tmax);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *pyParametricSurface = NULL;
float cParametricSurface(float *u, float *v){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("ff", *u, *v);
    pyResult = PyEval_CallObject(pyParametricSurface, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
static PyObject *s2plot_s2funxy(PyObject *self, PyObject *args){
    int nx, ny, ctl;
    float xmin, xmax, ymin, ymax;
    PyObject *tempXY;
    
    if(!PyArg_ParseTuple(args, "Oiiffffi:s2funxy", &tempXY, &nx, &ny, &xmin, &xmax, &ymin, &ymax, &ctl)) {
        return NULL;
    }
    if (!PyCallable_Check(tempXY)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempXY);
    Py_XDECREF(pyParametricSurface);
    
    pyParametricSurface = tempXY;

    s2funxy(cParametricSurface, nx, ny, xmin, xmax, ymin, ymax, ctl);

    Py_INCREF(Py_None);
    return Py_None; 
}
static PyObject *s2plot_s2funxz(PyObject *self, PyObject *args){
    int nx, nz, ctl;
    float xmin, xmax, zmin, zmax;
    PyObject *tempXZ;
    
    if(!PyArg_ParseTuple(args, "Oiiffffi:s2funxz", &tempXZ, &nx, &nz, &xmin, &xmax, &zmin, &zmax, &ctl)) {
        return NULL;
    }
    if (!PyCallable_Check(tempXZ)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempXZ);
    Py_XDECREF(pyParametricSurface);
    
    pyParametricSurface = tempXZ;

    s2funxz(cParametricSurface, nx, nz, xmin, xmax, zmin, zmax, ctl);

    Py_INCREF(Py_None);
    return Py_None; 
}
static PyObject *s2plot_s2funyz(PyObject *self, PyObject *args){
    int ny, nz, ctl;
    float ymin, ymax, zmin, zmax;
    PyObject *tempYZ;
    
    if(!PyArg_ParseTuple(args, "Oiiffffi:s2funyz", &tempYZ, &ny, &nz, &ymin, &ymax, &zmin, &zmax, &ctl)) {
        return NULL;
    }
    if (!PyCallable_Check(tempYZ)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempYZ);
    Py_XDECREF(pyParametricSurface);
    
    pyParametricSurface = tempYZ;

    s2funyz(cParametricSurface, ny, nz, ymin, ymax, zmin, zmax, ctl);

    Py_INCREF(Py_None);
    return Py_None; 
    
}
static PyObject *s2plot_s2funxyr(PyObject *self, PyObject *args){
    int nx, ny, ctl;
    float xmin, xmax, ymin, ymax, rmin, rmax;
    PyObject *tempXY;
    
    if(!PyArg_ParseTuple(args, "Oiiffffiff:s2funxyr", &tempXY, &nx, &ny, &xmin, &xmax, &ymin, &ymax, &ctl, &rmin, &rmax)) {
        return NULL;
    }
    if (!PyCallable_Check(tempXY)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempXY);
    Py_XDECREF(pyParametricSurface);
    
    pyParametricSurface = tempXY;

    s2funxyr(cParametricSurface, nx, ny, xmin, xmax, ymin, ymax, ctl, rmin, rmax);

    Py_INCREF(Py_None);
    return Py_None; 
}
static PyObject *s2plot_s2funxzr(PyObject *self, PyObject *args){
    int nx, nz, ctl;
    float xmin, xmax, zmin, zmax, rmin, rmax;
    PyObject *tempXZ;
    
    if(!PyArg_ParseTuple(args, "Oiiffffiff:s2funxzr", &tempXZ, &nx, &nz, &xmin, &xmax, &zmin, &zmax, &ctl, &rmin, &rmax)) {
        return NULL;
    }
    if (!PyCallable_Check(tempXZ)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempXZ);
    Py_XDECREF(pyParametricSurface);
    
    pyParametricSurface = tempXZ;

    s2funxzr(cParametricSurface, nx, nz, xmin, xmax, zmin, zmax, ctl, rmin, rmax);

    Py_INCREF(Py_None);
    return Py_None; 
}
static PyObject *s2plot_s2funyzr(PyObject *self, PyObject *args){
    int ny, nz, ctl;
    float ymin, ymax, zmin, zmax, rmin, rmax;
    PyObject *tempYZ;
    
    if(!PyArg_ParseTuple(args, "Oiiffffi:s2funyzr", &tempYZ, &ny, &nz, &ymin, &ymax, &zmin, &zmax, &ctl, &rmin, &rmax)) {
        return NULL;
    }
    if (!PyCallable_Check(tempYZ)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempYZ);
    Py_XDECREF(pyParametricSurface);
    
    pyParametricSurface = tempYZ;

    s2funyzr(cParametricSurface, ny, nz, ymin, ymax, zmin, zmax, ctl, rmin, rmax);

    Py_INCREF(Py_None);
    return Py_None; 
    
}
float cParametricSurface_fx(float *u, float *v){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("ff", *u, *v);
    
    pyResult = PyEval_CallObject(pyParametric_fx, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
float cParametricSurface_fy(float *u, float *v){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("ff", *u, *v);
    
    pyResult = PyEval_CallObject(pyParametric_fy, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
float cParametricSurface_fz(float *u, float *v){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("ff", *u, *v);
    
    pyResult = PyEval_CallObject(pyParametric_fz, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
float cParametricSurface_fc(float *u, float *v){
    float result;
    PyObject *arg, *pyResult;
    
    arg = Py_BuildValue("ff", *u, *v);
    
    pyResult = PyEval_CallObject(pyParametric_fc, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL){
        return 0;
    }
    result = (float) PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    
    return result;
}
static PyObject *s2plot_s2funuv(PyObject *self, PyObject *args){
    int uDIV, vDIV;
    float umin, umax, vmin, vmax;
    PyObject *tempX, *tempY, *tempZ, *tempC;
    
    if(!PyArg_ParseTuple(args, "OOOOffiffi:s2funuv", &tempX, &tempY, &tempZ, &tempC, &umin, &umax, &uDIV, &vmin, &vmax, &vDIV)) {
        return NULL;
    }
    if (!PyCallable_Check(tempX) || !PyCallable_Check(tempY) || !PyCallable_Check(tempZ) || !PyCallable_Check(tempC)) {
        PyErr_SetString(PyExc_TypeError, "parameters must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempX);
    Py_XINCREF(tempY);
    Py_XINCREF(tempZ);
    Py_XINCREF(tempC);
    Py_XDECREF(pyParametric_fx);
    Py_XDECREF(pyParametric_fy);
    Py_XDECREF(pyParametric_fz);
    Py_XDECREF(pyParametric_fc);
    
    pyParametric_fx = tempX;
    pyParametric_fy = tempY;
    pyParametric_fz = tempZ;
    pyParametric_fc = tempC;

    s2funuv(cParametricSurface_fx, cParametricSurface_fy, cParametricSurface_fz, cParametricSurface_fc, umin, umax, uDIV, vmin, vmax, vDIV);

    Py_INCREF(Py_None);
    return Py_None;
}
// IMAGES/SURFACES
static PyObject *s2plot_s2surp(PyObject *self, PyObject *args){

    float **data, *tr;
    PyArrayObject *dataObject, *trIn;
    int n[2], i[2], j[2];
    float dataRange[2];
    
    // parse the python into C
    if(!PyArg_ParseTuple(args, "O!iiiiiiffO!:s2surp", &PyArray_Type, &dataObject, &n[0], &n[1], &i[0], &i[1], &j[0], &j[1], &dataRange[0], &dataRange[1], &PyArray_Type, &trIn) || NULL == &dataObject || NULL == &trIn){
        return NULL;
    }
    
    if(!(data = numpy2D_to_float(dataObject))) return NULL;
    if(!(tr = numpy1D_to_float(trIn))) return NULL;
    
    s2surp(data, n[0], n[1], i[0], i[1], j[0], j[1], dataRange[0], dataRange[1], tr);

    numpy_free(dataObject, data);
    numpy_free(trIn, tr);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2surpa(PyObject *self, PyObject *args){

    float **data, *tr;
    PyArrayObject *dataObject, *trIn;
    int n[2], i[2], j[2];
    float dataRange[2];
    
    // parse the python into C
    if(!PyArg_ParseTuple(args, "O!iiiiiiffO!:s2surpa", &PyArray_Type, &dataObject, &n[0], &n[1], &i[0], &i[1], &j[0], &j[1], &dataRange[0], &dataRange[1], &PyArray_Type, &trIn) || NULL == &dataObject || NULL == &trIn){
        return NULL;
    }
    
    if(!(data = numpy2D_to_float(dataObject))) return NULL;
    if(!(tr = numpy1D_to_float(trIn))) return NULL;
    
    s2surpa(data, n[0], n[1], i[0], i[1], j[0], j[1], dataRange[0], dataRange[1], tr);

    numpy_free(dataObject, data);
    numpy_free(trIn, tr);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2scir(PyObject *self, PyObject *args){
    int col1, col2;
    
    if(!PyArg_ParseTuple(args,"ii:s2scir",&col1,&col2)){
        return NULL;
    }
    
    s2scir(col1, col2);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2qcir(PyObject *self, PyObject *args){
    int col1, col2;
    PyObject *resultList;
    
    s2qcir(&col1,&col2);
    
    resultList = (PyObject *) PyTuple_New(2);
    if(resultList == NULL)
        return NULL;
    
    PyTuple_SetItem(resultList,0,PyInt_FromLong((long) col1));
    PyTuple_SetItem(resultList,1,PyInt_FromLong((long) col2));
    Py_INCREF(resultList);
    
    return resultList;
}
static PyObject *s2plot_s2icm(PyObject *self, PyObject *args){
    char *mapname;
    int idx1, idx2;

    if(!PyArg_ParseTuple(args,"sii:s2icm",&mapname,&idx1,&idx2)){
        return NULL;
    }
    
    return Py_BuildValue("i",s2icm(mapname, idx1, idx2));
}
// VECTOR PLOTS
static PyObject *s2plot_s2vect3(PyObject *self, PyObject *args){
    int adim, bdim, cdim, a1, a2, b1, b2, c1, c2;
    PyArrayObject *aIn, *bIn, *cIn, *trIn;
    float ***a, ***b, ***c, *tr, scale, nc, minlength, minl, maxl;
    int colbylength;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"O!O!O!iiiiiiiiifiO!fiff:s2vect3", &PyArray_Type, &aIn, &PyArray_Type, &bIn, &PyArray_Type, &cIn, &adim, &bdim, &cdim, &a1, &a2, &b1, &b2, &c1, &c2, &scale, &nc, &PyArray_Type, &trIn, &minlength, &colbylength, &minl, &maxl) || NULL == &aIn || NULL == &bIn || NULL == &cIn || NULL == &trIn){
        return NULL;
    }
    if(!(a = numpy3D_to_float(aIn))) {return NULL;}
    if(!(b = numpy3D_to_float(bIn))) {numpy_free(aIn, a); return NULL;}
    if(!(c = numpy3D_to_float(cIn))) {numpy_free(aIn, a); numpy_free(bIn, b); return NULL;}
    if(!(tr = numpy1D_to_float(trIn))) {numpy_free(aIn, a); numpy_free(bIn, b); numpy_free(cIn, c); return NULL;}
    
    s2vect3(a, b, c, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, scale, nc, tr, minlength, colbylength, minl, maxl);

    numpy_free(aIn, a);
    numpy_free(bIn, b);
    numpy_free(cIn, c);
    numpy_free(trIn, tr);

    Py_INCREF(Py_None);
    return Py_None;
}
// MISCELLANEOUS ROUTINES
static PyObject *s2plot_s2iden(PyObject *self, PyObject *args){
    char *textra;

    if(!PyArg_ParseTuple(args,"s:s2iden",&textra)){
        return NULL;
    }
    
    s2iden(textra);
    
    Py_INCREF(Py_None);
    return Py_None;
}
// S2 "NATIVE" ROUTINES
static PyObject *s2plot_ns2sphere(PyObject *self, PyObject *args){
    float x, y, z, r, red, green, blue;
    
    if(!PyArg_ParseTuple(args, "fffffff:ns2sphere", &x, &y, &z, &r, &red, &green, &blue)){
        return NULL;
    }
    
    ns2sphere(x, y, z, r, red, green, blue);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vsphere(PyObject *self, PyObject *args){
    XYZ P;
    float r;
    COLOUR col;
    PyObject *PIn, *colIn;
    
    if(!PyArg_ParseTuple(args, "OfO:ns2vsphere", &PIn, &r, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    
    P = Dict_to_XYZ(PIn);
    col = Dict_to_COLOUR(colIn);

    ns2vsphere(P, r, col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2spheret(PyObject *self, PyObject *args){
    float x, y, z, r, red, green, blue;
    char * texturefn;

    if(!PyArg_ParseTuple(args, "fffffffs:ns2spheret", &x, &y, &z, &r, &red, &green, &blue, &texturefn)){
        return NULL;
    }
    
    ns2spheret(x, y, z, r, red, green, blue, texturefn);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vspheret(PyObject *self, PyObject *args){
    XYZ P;
    float r;
    COLOUR col;
    char *texturefn;
    PyObject *PIn, *colIn;

    if(!PyArg_ParseTuple(args, "OfOs:ns2vspheret", &PIn, &r, &colIn, &texturefn)){
        return NULL;
    }
    
    P = Dict_to_XYZ(PIn);
    col = Dict_to_COLOUR(colIn);

    ns2vspheret(P, r, col, texturefn);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2spherex(PyObject *self, PyObject *args){
    float x, y, z, r, red, green, blue;
    unsigned int textureid;
    
    if(!(PyArg_ParseTuple(args, "fffffffI:ns2spherex", &x, &y, &z, &r, &red, &green, &blue, &textureid))){
        return NULL;
    }
    
    ns2spherex(x, y, z, r, red, green, blue, textureid);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vspherex(PyObject *self, PyObject *args){
    XYZ P;
    float r;
    COLOUR col;
    unsigned int textureid;
    PyObject *PIn, *colIn;
    
    if(!(PyArg_ParseTuple(args, "OfOI:ns2vspherex", &PIn, &r, &colIn, &textureid)) || NULL == PIn || NULL == colIn){
        return NULL;
    }

    P = Dict_to_XYZ(PIn);
    col = Dict_to_COLOUR(colIn);

    ns2vspherex(P, r, col, textureid);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vplanett(PyObject *self, PyObject *args){ /* NEW */
    XYZ iP;
    float ir;
    COLOUR icol;
    char *itexturefn;
    float texture_phase, rotation;
    XYZ axis;
    PyObject *iPIn, *icolIn, *axisIn;
    
    if(!(PyArg_ParseTuple(args, "OfOsfOf:ns2vspherex", &iPIn, &ir, &icolIn,
                          &itexturefn, &texture_phase, &axisIn, &rotation))
            || NULL == iPIn || NULL == icolIn || NULL == axisIn ){
        return NULL;
    }

    iP = Dict_to_XYZ(iPIn);
    icol = Dict_to_COLOUR(icolIn);
    axis = Dict_to_XYZ(axisIn);

    ns2vplanett(iP, ir, icol, itexturefn, texture_phase, axis, rotation);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vplanetx(PyObject *self, PyObject *args){ /* NEW */
    XYZ iP;
    float ir;
    COLOUR icol;
    unsigned int itextureid;
    float texture_phase, rotation;
    XYZ axis;
    PyObject *iPIn, *icolIn, *axisIn;
    
    if(!(PyArg_ParseTuple(args, "OfOsfOf:ns2vspherex", &iPIn, &ir, &icolIn,
                          &itextureid, &texture_phase, &axisIn, &rotation))
            || NULL == iPIn || NULL == icolIn || NULL == axisIn ){
        return NULL;
    }

    iP = Dict_to_XYZ(iPIn);
    icol = Dict_to_COLOUR(icolIn);
    axis = Dict_to_XYZ(axisIn);

    ns2vplanetx(iP, ir, icol, itextureid, texture_phase, axis, rotation);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2disk(PyObject *self, PyObject *args){
    float x, y, z, nx, ny, nz, r1, r2, red, green, blue;
    
    if(!PyArg_ParseTuple(args, "fffffffffff:ns2disk", &x, &y, &z, &nx, &ny, &nz, &r1, &r2, &red, &green, &blue)){
        return NULL;
    }
    
    ns2disk(x, y, z, nx, ny, nz, r1, r2, red, green, blue);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vdisk(PyObject *self, PyObject *args){
    XYZ P, N;
    float r1, r2;
    COLOUR col;
    PyObject *PIn, *NIn, *colIn;
    
    if(!PyArg_ParseTuple(args, "OOffO:ns2vdisk", &PIn, &NIn, &r1, &r2, &colIn) || PIn == NULL || colIn == NULL){
        return NULL;
    }
    
    P = Dict_to_XYZ(PIn);
    N = Dict_to_XYZ(NIn);
    col = Dict_to_COLOUR(colIn);
    
    ns2vdisk(P, N, r1, r2, col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2arc(PyObject *self, PyObject *args){
    float px, py, pz, nx, ny, nz, sx, sy, sz, deg;
    int nseg;

    if(!PyArg_ParseTuple(args, "ffffffffffi:ns2arc", &px, &py, &pz, &nx, &ny, &nz, &sx, &sy, &sz, &deg, &nseg)){
        return NULL;
    }
    
    ns2arc(px, py, pz, nx, ny, nz, sx, sy, sz, deg, nseg);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2varc(PyObject *self, PyObject *args){
    XYZ P, N, S;
    float deg;
    int nseg;
    PyObject *PIn, *NIn, *SIn; 
    
    if(!PyArg_ParseTuple(args, "OOOfi:ns2varc", &PIn, &NIn, &SIn, &deg, &nseg) || NULL == PIn || NULL == NIn || NULL == SIn){
        return NULL;
    }
    
    P = Dict_to_XYZ(PIn);
    N = Dict_to_XYZ(NIn);
    S = Dict_to_XYZ(SIn);
    
    ns2varc(P, N, S, deg, nseg);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2erc(PyObject *self, PyObject *args){ /* NEW */
    float px, py, pz, nx, ny, nz, sx, sy, sz, deg, axratio;
    int nseg;

    if(!PyArg_ParseTuple(args, "ffffffffffif:ns2erc", &px, &py, &pz, &nx, &ny,
                        &nz, &sx, &sy, &sz, &deg, &nseg, &axratio)){
        return NULL;
    }
    
    ns2erc(px, py, pz, nx, ny, nz, sx, sy, sz, deg, nseg, axratio);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2verc(PyObject *self, PyObject *args){ /* NEW */
    XYZ P, N, S;
    float deg, axratio;
    int nseg;
    PyObject *PIn, *NIn, *SIn; 
    
    if(!PyArg_ParseTuple(args, "OOOfif:ns2verc", 
            &PIn, &NIn, &SIn, &deg, &nseg, &axratio) || NULL == PIn || NULL == NIn || NULL == SIn){
        return NULL;
    }
    
    P = Dict_to_XYZ(PIn);
    N = Dict_to_XYZ(NIn);
    S = Dict_to_XYZ(SIn);
    
    ns2verc(P, N, S, deg, nseg, axratio);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2text(PyObject *self, PyObject *args){
    float x, y, z, rx, ry, rz, ux, uy, uz, red, green, blue;
    char *text;

    if(!PyArg_ParseTuple(args, "ffffffffffffs:ns2text", &x, &y, &z, &rx, &ry, &rz, &ux, &uy, &uz, &red, &green, &blue, &text)){
        return NULL;
    }
    
    ns2text(x, y, z, rx, ry, rz, ux, uy, uz, red, green, blue, text);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vtext(PyObject *self, PyObject *args){
    XYZ P, R, U;
    COLOUR col;
    char *text;
    PyObject *PIn, *RIn, *UIn, *colIn;

    if(!PyArg_ParseTuple(args, "OOOOs:ns2vtext", &PIn, &RIn, &UIn, &colIn, &text) || NULL == PIn || NULL == RIn || NULL == UIn || NULL == colIn){
        return NULL;
    }
    P = Dict_to_XYZ(PIn);
    R = Dict_to_XYZ(RIn);
    U = Dict_to_XYZ(UIn);
    col = Dict_to_COLOUR(colIn);
    
    ns2vtext(P, R, U, col, text);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2point(PyObject *self, PyObject *args){
    float x, y, z, r, g, b;
    
    if(!PyArg_ParseTuple(args, "ffffff:ns2point", &x, &y, &z, &r, &g, &b)){
        return NULL;
    }
    
    ns2point(x, y, z, r, g, b);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vpoint(PyObject *self, PyObject *args){
    XYZ P;
    COLOUR col;
    PyObject *PIn, *colIn;
    
    if(!PyArg_ParseTuple(args, "OO:ns2vpoint", &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    P = Dict_to_XYZ(PIn);
    col = Dict_to_COLOUR(colIn);
    
    ns2vpoint(P, col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vnpoint(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR col;
    int n, i;
    PyObject *PIn, *colIn;
    
    if(!PyArg_ParseTuple(args, "OOi:ns2vpoint", &PIn, &colIn, &n) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    
    if((int) PyList_Size(PIn) != n){
        PyErr_SetString(PyExc_IndexError, "the list of positions must have length n");
        return NULL;
    }
    
    P = (XYZ *) calloc(n, sizeof(XYZ));
    for(i = 0; i < n; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
    }

    
    ns2vnpoint(P, col, n);
    
    free(P);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2thpoint(PyObject *self, PyObject *args){
    float x, y, z, r, g, b, size;

    if(!PyArg_ParseTuple(args, "fffffff:ns2thpoint", &x, &y, &z, &r, &g, &b, &size)){
        return NULL;
    }
    
    ns2thpoint(x, y, z, r, g, b, size);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vthpoint(PyObject *self, PyObject *args){
    PyObject *_P, *_col;
    float size;
    // convention XYZ and COLOUR structs are python dictionaries
    XYZ P;
    COLOUR col;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"OOf:ns2vthpoint",&_P, &_col, &size) || NULL == _P || NULL == _col){
        return NULL;
    }

    // unpack lists into structs; XYZ struct...
    P = Dict_to_XYZ(_P);
    // COLOUR structs...
    col = Dict_to_COLOUR(_col);

    ns2vthpoint(P, col, size);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2i(PyObject *self, PyObject *args){
    float x, y, z, r, g, b;

    if(!PyArg_ParseTuple(args, "ffffff:ns2i", &x, &y, &z, &r, &g, &b)){
        return NULL;
    }
    
    ns2i(x, y, z, r, g, b);
    
    Py_INCREF(Py_None);
    return Py_None;   
}
static PyObject *s2plot_ns2vi(PyObject *self, PyObject *args){
    XYZ P;
    COLOUR col;
    PyObject *PIn, *colIn;

    if(!PyArg_ParseTuple(args, "OO:ns2vi", &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    
    P = Dict_to_XYZ(PIn);
    col = Dict_to_COLOUR(colIn);
    
    ns2vi(P, col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2line(PyObject *self, PyObject *args){
    float x1, y1, z1, x2, y2, z2, r, g, b;
    
    if(!PyArg_ParseTuple(args, "fffffffff:ns2line", &x1, &y2, &z1, &x2, &y2, &z2, &r, &g, &b)){
        return NULL;
    }
    
    ns2line(x1, y1, z1, x2, y2, z2, r, g, b);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vline(PyObject *self, PyObject *args){
    XYZ P1, P2;
    COLOUR col;
    PyObject *P1In, *P2In, *colIn;

    if(!PyArg_ParseTuple(args, "OOO:ns2vline", &P1In, &P2In, &colIn) || NULL == P1In || NULL == P2In || NULL == colIn){
        return NULL;
    }
    P1 = Dict_to_XYZ(P1In);
    P2 = Dict_to_XYZ(P2In);
    col = Dict_to_COLOUR(colIn);
    
    ns2vline(P1, P2, col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2thline(PyObject *self, PyObject *args){
    float x1, y1, z1, x2, y2, z2, r, g, b, w;

    if(!PyArg_ParseTuple(args, "ffffffffff:ns2thline", &x1, &y1, &z1, &x2, &y2, &z2, &r, &g, &b, &w)){
        return NULL;
    }
    
    ns2thline(x1, y1, z1, x2, y2, z2, r, g, b, w);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vthline(PyObject *self, PyObject *args){
    XYZ P1, P2;
    COLOUR col;
    float width;
    PyObject *P1In, *P2In, *colIn;

    if(!PyArg_ParseTuple(args, "OOOf:ns2vthline", &P1In, &P2In, &colIn, &width) || NULL == P1In || NULL == P2In || NULL == colIn){
        return NULL;
    }
    P1 = Dict_to_XYZ(P1In);
    P2 = Dict_to_XYZ(P2In);
    col = Dict_to_COLOUR(colIn);
    
    ns2vthline(P1, P2, col, width);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2thwcube(PyObject *self, PyObject *args){ /* NEW */
    float x1, y1, z1, x2, y2, z2, red, green, blue, width;
        if(!PyArg_ParseTuple(args, "ffffffffffff:ns2thwcube", &x1, &y1, &z1, &x2, &y2, &z2, &red, &green, &blue, &width)){
        return NULL;
    }
    
    ns2thwcube(x1, y1, z1, x2, y2, z2, red, green, blue, width);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vthwcube(PyObject *self, PyObject *args){ /* NEW */
    XYZ P1, P2;
    COLOUR col;
    float width;
    PyObject *P1In, *P2In, *colIn;

    if(!PyArg_ParseTuple(args, "OOOf:ns2vthwcube", &P1In, &P2In, &colIn, &width) || NULL == P1In || NULL == P2In || NULL == colIn){
        return NULL;
    }
    P1 = Dict_to_XYZ(P1In);
    P2 = Dict_to_XYZ(P2In);
    col = Dict_to_COLOUR(colIn);
    
    ns2vthwcube(P1, P2, col, width);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2cline(PyObject *self, PyObject *args){
    float x1, y1, z1, x2, y2, z2, red, green, blue, red1, green1, blue1;
        if(!PyArg_ParseTuple(args, "ffffffffffff:ns2cline", &x1, &y1, &z1, &x2, &y2, &z2, &red, &green, &blue, &red1, &green1, &blue1)){
        return NULL;
    }
    
    ns2cline(x1, y1, z1, x2, y2, z2, red, green, blue, red1, green1, blue1);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vcline(PyObject *self, PyObject *args){
    XYZ P1, P2;
    COLOUR col1, col2;
    PyObject *P1In, *P2In, *col1In, *col2In;

    if(!PyArg_ParseTuple(args, "OOOO:ns2vcline", &P1In, &P2In, &col1In, &col2In) || NULL == P1In || NULL == P2In || NULL == col1In || NULL == col2In){
        return NULL;
    }
    P1 = Dict_to_XYZ(P1In);
    P2 = Dict_to_XYZ(P2In);
    col1 = Dict_to_COLOUR(col1In);
    col2 = Dict_to_COLOUR(col2In);
    
    ns2vcline(P1, P2, col1, col2);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2thcline(PyObject *self, PyObject *args){ /* NEW */
    float x1, y1, z1, x2, y2, z2, red1, green1, blue1, red2, green2, blue2, width;
        if(!PyArg_ParseTuple(args, "fffffffffffff:ns2thcline", &x1, &y1, &z1, &x2, &y2, &z2,
                &red1, &green1, &blue1, &red2, &green2, &blue2, &width)){
        return NULL;
    }
    
    ns2thcline(x1, y1, z1, x2, y2, z2, red1, green1, blue1, red2, green2, blue2, width);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vthcline(PyObject *self, PyObject *args){ /* NEW */
    XYZ P1, P2;
    COLOUR col1, col2;
    PyObject *P1In, *P2In, *col1In, *col2In;
    float width;

    if(!PyArg_ParseTuple(args, "OOOOf:ns2vthcline", &P1In, &P2In, &col1In, &col2In, &width)
            || NULL == P1In || NULL == P2In || NULL == col1In || NULL == col2In){
        return NULL;
    }
    P1 = Dict_to_XYZ(P1In);
    P2 = Dict_to_XYZ(P2In);
    col1 = Dict_to_COLOUR(col1In);
    col2 = Dict_to_COLOUR(col2In);
    
    ns2vthcline(P1, P2, col1, col2, width);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf3(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR col;
    PyObject *PIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OO:ns2vf3", &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 3){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 3 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(3, sizeof(XYZ));
    for(i = 0; i < 3; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf3(P, col);
    
    free(P);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf3n(PyObject *self, PyObject *args){
    XYZ *P, *N;
    COLOUR col;
    PyObject *PIn, *NIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OOO:ns2vf3n", &PIn, &NIn, &colIn) || NULL == PIn || NULL == NIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 3 || PyList_Size(NIn) != 3){
        PyErr_SetString(PyExc_IndexError, "vertices and normals must be lists of 3 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(3, sizeof(XYZ));
    N = (XYZ *) calloc(3, sizeof(XYZ));    
    for(i = 0; i < 3; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
        N[i] = Dict_to_XYZ(PyList_GetItem(NIn, i));        
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf3n(P, N, col);

    free(P);
    free(N);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf3c(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR *col;
    PyObject *PIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OO:ns2vf3c", &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 3 || PyList_Size(colIn) != 3){
        PyErr_SetString(PyExc_IndexError, "vertices and colours must be lists of 3 dictionaries");
        return NULL;
    }
    
    P = (XYZ *) calloc(3, sizeof(XYZ));
    col = (COLOUR *) calloc(3, sizeof(COLOUR));    
    for(i = 0; i < 3; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
        col[i] = Dict_to_COLOUR(PyList_GetItem(colIn, i));        
    }
    
    ns2vf3c(P, col);
    
    free(P);
    free(col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf3nc(PyObject *self, PyObject *args){
    XYZ *P, *N;
    COLOUR *col;
    PyObject *PIn, *NIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OOO:ns2vf3nc", &PIn, &NIn, &colIn) || NULL == PIn || NULL == NIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 3 || PyList_Size(NIn) != 3 || PyList_Size(colIn) != 3){
        PyErr_SetString(PyExc_IndexError, "vertices, normals and colours must be lists of 3 dictionaries");
        return NULL;
    }
    
    P = (XYZ *) calloc(3, sizeof(XYZ));
    N = (XYZ *) calloc(3, sizeof(XYZ));   
    col = (COLOUR *) calloc(3, sizeof(COLOUR)); 
    for(i = 0; i < 3; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
        N[i] = Dict_to_XYZ(PyList_GetItem(NIn, i));
        col[i] = Dict_to_COLOUR(PyList_GetItem(colIn, i));
    }
    
    ns2vf3nc(P, N, col);
    
    free(P);
    free(N);
    free(col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR col;
    PyObject *PIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OO:ns2vf4", &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 4){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    for(i = 0; i < 4; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf4(P, col);
    
    free(P);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4n(PyObject *self, PyObject *args){
    XYZ *P, *N;
    COLOUR col;
    PyObject *PIn, *NIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OOO:ns2vf4n", &PIn, *NIn, &colIn) || NULL == PIn || NULL == NIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 3 || PyList_Size(NIn) != 3){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    N = (XYZ *) calloc(4, sizeof(XYZ));
    for(i = 0; i < 4; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
        N[i] = Dict_to_XYZ(PyList_GetItem(NIn, i));
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf4n(P, N, col);
    
    free(P);
    free(N);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4c(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR *col;
    PyObject *PIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OO:ns2vf4c", &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 4 || PyList_Size(colIn) != 4){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    col = (COLOUR *) calloc(4, sizeof(COLOUR));
    for(i = 0; i < 3; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
        col[i] = Dict_to_COLOUR(PyList_GetItem(colIn, i));
    }
    
    ns2vf4c(P, col);
    
    free(P);
    free(col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4nc(PyObject *self, PyObject *args){
    XYZ *P, *N;
    COLOUR *col;
    PyObject *PIn, *NIn, *colIn;
    int i;

    if(!PyArg_ParseTuple(args, "OOO:ns2vf4nc", &PIn, &NIn, &colIn) || NULL == PIn || NULL == NIn ||NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 4 || PyList_Size(NIn) != 4 || PyList_Size(colIn) != 4){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    N = (XYZ *) calloc(4, sizeof(XYZ));
    col = (COLOUR *) calloc(4, sizeof(COLOUR));
    for(i = 0; i < 4; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
        N[i] = Dict_to_XYZ(PyList_GetItem(NIn, i));
        col[i] = Dict_to_COLOUR(PyList_GetItem(colIn, i));
    }
    
    ns2vf4nc(P, N, col);
    
    free(P);
    free(N);
    free(col);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4t(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR col;
    PyObject *PIn, *colIn;
    float scale;
    char *texturefn, *trans;
    int i;

    if(!PyArg_ParseTuple(args, "OOsfs:ns2vf4t", &PIn, &colIn, &texturefn, &scale, &trans) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 4){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    for(i = 0; i < 4; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf4t(P, col, texturefn, scale, trans[0]);
    
    free(P);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4x(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR col;
    PyObject *PIn, *colIn;
    unsigned int textureid;
    float scale;
    char *trans;
    int i;

    if(!PyArg_ParseTuple(args, "OOIfs:ns2vf4x", &PIn, &colIn, &textureid, &scale, &trans) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 4){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    for(i = 0; i < 4; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf4x(P, col, textureid, scale, trans[0]);
    
    free(P);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vf4xt(PyObject *self, PyObject *args){
    XYZ *P;
    COLOUR col;
    PyObject *PIn, *colIn;
    unsigned int textureid;
    float iscale, ialpha;
    char *itrans;
    int i;

    if(!PyArg_ParseTuple(args, "OOIfsf:ns2vf4xt", &PIn, &colIn, &textureid, &iscale, &itrans, &ialpha) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    if(PyList_Size(PIn) != 4){
        PyErr_SetString(PyExc_IndexError, "vertices must be a list of 4 dictionaries of {x,y,z}");
        return NULL;
    }
    
    P = (XYZ *) calloc(4, sizeof(XYZ));
    for(i = 0; i < 4; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn, i));
    }
    col = Dict_to_COLOUR(colIn);
    
    ns2vf4xt(P, col, textureid, iscale, itrans[0], ialpha);
    
    free(P);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2scube(PyObject *self, PyObject *args){ /* NEW */
    float x1, y1, z1, x2, y2, z2, red, green, blue, alpha;

    if(!PyArg_ParseTuple(args, "ffffffffff:ns2scube", &x1, &y1, &z1, &x2, &y2, &z2,
            &red, &green, &blue, &alpha))
        return NULL;
    
    ns2scube(x1, y1, z1, x2, y2, z2, red, green, blue, alpha);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vscube(PyObject *self, PyObject *args){ /* NEW */
    XYZ P1, P2;
    COLOUR col;
    PyObject *P1In, *P2In, *colIn;
    float alpha;

    if(!PyArg_ParseTuple(args, "OOOf:ns2vscube", &P1In, &P2In, &colIn, &alpha) || NULL == P1In || NULL == P2In || NULL == colIn){
        return NULL;
    }
    P1 = Dict_to_XYZ(P1In);
    P2 = Dict_to_XYZ(P2In);
    col = Dict_to_COLOUR(colIn);
    
    ns2vscube(P1, P2, col, alpha);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2m(PyObject *self, PyObject *args){
    int type;
    float size, x, y, z, r, g, b;

    if(!PyArg_ParseTuple(args, "ifffffff:ns2m", &type, &size, &x, &y, &z, &r, &g, &b)){
        return NULL;
    }
    
    ns2m(type, size, x, y, z, r, g, b);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2vm(PyObject *self, PyObject *args){
    int type;
    float size;
    XYZ P;
    COLOUR col;
    PyObject *PIn, *colIn;

    if(!PyArg_ParseTuple(args, "ifOO:ns2vm", &type, &size, &PIn, &colIn) || NULL == PIn || NULL == colIn){
        return NULL;
    }
    P = Dict_to_XYZ(PIn);
    col = Dict_to_COLOUR(colIn);
    
    ns2vm(type, size, P, col);
        
    Py_INCREF(Py_None);
    return Py_None;
    
}
// DYNAMIC-ONLY GEOMETRY FUNCTIONS
static PyObject *s2plot_ds2bb(PyObject *self, PyObject *args){
    float x, y, z, str_x, str_y, str_z, isize, r, g, b, alpha;
    unsigned int itextid;
    char *trans;
    
    if(!PyArg_ParseTuple(args, "ffffffffffIfs:ds2bb", &x, &y, &z, &str_x, &str_y, &str_z, &isize, &r, &g, &b, &itextid, &alpha, &trans)){
        return NULL;
    }
    
    ds2bb(x, y, z, str_x, str_y, str_z, isize, r, g, b, itextid, alpha, trans[0]);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2vbb(PyObject *self, PyObject *args){
    XYZ iP, iStretch;
    float isize, alpha;
    COLOUR iC;
    unsigned int itextid;
    char *trans;
    PyObject *iPIn, *iStretchIn, *iCIn;
    
    if(!PyArg_ParseTuple(args, "OOfOIfs:ds2vbb", &iPIn, &iStretchIn, &isize, &iCIn, &itextid, &alpha, &trans) || NULL == iPIn || NULL == iStretchIn || NULL == iCIn){
        return NULL;
    }
    
    iP = Dict_to_XYZ(iPIn);
    iStretch = Dict_to_XYZ(iStretchIn);
    iC = Dict_to_COLOUR(iCIn);
    
    ds2vbb(iP, iStretch, isize, iC, itextid, alpha, trans[0]);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2vbbr(PyObject *self, PyObject *args){
    XYZ iP, iStretch;
    float isize, ipangle, alpha;
    COLOUR iC;
    unsigned int itextid;
    char *trans;
    PyObject *iPIn, *iStretchIn, *iCIn;
    
    if(!PyArg_ParseTuple(args, "OOffOIfs:ds2vbbr", &iPIn, &iStretchIn, &isize, &ipangle, &iCIn, &itextid, &alpha, &trans) || NULL == iPIn || NULL == iStretchIn || NULL == iCIn){
        return NULL;
    }
    
    iP = Dict_to_XYZ(iPIn);
    iStretch = Dict_to_XYZ(iStretchIn);
    iC = Dict_to_COLOUR(iCIn);
    
    ds2vbbr(iP, iStretch, isize, ipangle, iC, itextid, alpha, trans[0]);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2vbbp(PyObject *self, PyObject *args){ /* NEW */
    XYZ iP, offset;
    float aspect, isize, alpha;
    COLOUR iC;
    unsigned int itextid;
    char *trans;
    PyObject *iPIn, *ioffsetIn, *iCIn;
    
    if(!PyArg_ParseTuple(args, "OOffOIfs:ds2vbbp", &iPIn, &ioffsetIn, &aspect, &isize, &iCIn, &itextid, &alpha, &trans) || NULL == iPIn || NULL == ioffsetIn || NULL == iCIn){
        return NULL;
    }
    
    iP = Dict_to_XYZ(iPIn);
    offset = Dict_to_XYZ(ioffsetIn);
    iC = Dict_to_COLOUR(iCIn);
    
    ds2vbbr(iP, offset, aspect, isize, iC, itextid, alpha, trans[0]);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2tb(PyObject *self, PyObject *args){
    float x, y, z, x_off, y_off;
    char *text;
    int scaletext;
    
    if(!PyArg_ParseTuple(args, "fffffsi:ds2tb", &x, &y, &z, &x_off, &y_off, &text, &scaletext)){
        return NULL;
    }
    
    ds2tb(x, y, z, x_off, y_off, text, scaletext);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2vtb(PyObject *self, PyObject *args){
    XYZ iP, ioff;
    char *text;
    int scaletext;
    PyObject *iPIn, *ioffIn;
    
    if(!PyArg_ParseTuple(args, "OOsi:ds2vtb", &iPIn, &ioffIn, &text, &scaletext) || NULL == iPIn || NULL == ioffIn){
        return NULL;
    }
    
    iP = Dict_to_XYZ(iPIn);
    ioff = Dict_to_XYZ(ioffIn);

    ds2vtb(iP, ioff, text, scaletext);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2protect(PyObject *self, PyObject *args){

    ds2protect();

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2unprotect(PyObject *self, PyObject *args){
    
    ds2unprotect();

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ds2isprotected(PyObject *self, PyObject *args){
    int protected;
    PyObject *pyResult;

    protected = ds2isprotected();

    pyResult = (PyObject *) PyBool_FromLong((long) protected);
    Py_INCREF(pyResult);
    
    return pyResult;

}
// CALLBACK AND HANDLE SYSTEM
static PyObject *pyCallbackDict = NULL;
void   cCallBackFunction(double *time, int *keycount){
    PyObject *arg;
    PyObject *currentCallback, *callable, *callData;
    PyObject *pyResult;

    if(pyCallbackDict == NULL){
        // cannot set exception - in callback
        return;
    }
    currentCallback = PyDict_GetItem(pyCallbackDict, PyInt_FromLong((long) xs2qsp()));
    if(currentCallback == NULL || (PyTuple_Check(currentCallback) && PyTuple_Size(currentCallback) != 2) || (!PyTuple_Check(currentCallback) && !PyCallable_Check(currentCallback))){
        // cannot set exception - in callback
        return;
    }
    if(PyTuple_Check(currentCallback) && PyTuple_Size(currentCallback) == 2){
        callable = PyTuple_GetItem(currentCallback, 0);
        callData = PyTuple_GetItem(currentCallback, 1);
        if(callData == Py_None){
            arg = Py_BuildValue("di", *time, *keycount);
        } else {
            arg = Py_BuildValue("diO", *time, *keycount, callData);
        }
    } else {
        callable = currentCallback;
        arg = Py_BuildValue("di", *time, *keycount);
    }
    pyResult = PyEval_CallObject(callable, arg);
    Py_DECREF(arg);

    if (pyResult != NULL){
        Py_DECREF(pyResult);
    }
}
static PyObject *s2plot_cs2scb(PyObject *self, PyObject *args){
    PyObject *result = NULL;
    PyObject *temp;
    if (PyArg_ParseTuple(args, "O:cs2scb", &temp)) {
        if (!PyCallable_Check(temp)) {
            if(temp == Py_None){
                cs2scb(NULL);
                Py_RETURN_NONE;
            } else {
                PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                return NULL;
            }
        }
        
        // store a reference to the new callback in the global dict:
        /// the key will be the id of this panel
        if(pyCallbackDict == NULL){
            pyCallbackDict = PyDict_New();
            Py_XINCREF(pyCallbackDict);
        }
        Py_XINCREF(temp);
        
        PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
        // decref the current callback if it exists
        PyObject *currentCallback = PyDict_GetItem(pyCallbackDict, pyKey);
        if(currentCallback != NULL){
            Py_XDECREF(currentCallback);
        }
        // set the new callback for this pane
        PyDict_SetItem(pyCallbackDict, pyKey, temp);
        
        cs2scb(&cCallBackFunction);

        Py_INCREF(Py_None); 
        result = Py_None; 
    } 
    return result;
}
static PyObject *s2plot_cs2ecb(PyObject *dummy, PyObject *args){

    cs2ecb();

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_cs2dcb(PyObject *dummy, PyObject *args){

    cs2dcb();

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_cs2tcb(PyObject *dummy, PyObject *args){

    cs2tcb();

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *pyKCallbackDict = NULL;
int    cKCallBackFunction(unsigned char *key){
    PyObject *argList = NULL;
    PyObject *currentCallback = NULL;
    PyObject *result = NULL;
    PyObject *keyString = NULL;
    int cResult = 0;
    
    // build a unit length string
    keyString = PyString_FromStringAndSize((const char *)key, 1);
        
    // get the callback for the current pane
    if(pyKCallbackDict == NULL){
        // cannot set an error - in a callback
        return 0;
    }
    currentCallback = PyDict_GetItem(pyKCallbackDict, PyInt_FromLong((long) xs2qsp()));
    if(currentCallback == NULL){
        // cannot set an error - in a callback
        return 0;
    }
    argList = Py_BuildValue("(S)", keyString);
    result = PyEval_CallObject(currentCallback, argList);
    Py_DECREF(argList);
    
    if (result != NULL){
        cResult = (int) PyInt_AsLong(result);
        Py_DECREF(result);
    }
    
    return cResult;
}
static PyObject *s2plot_cs2skcb(PyObject *self, PyObject *args){
    PyObject *result = NULL;
    PyObject *temp;
    if (PyArg_ParseTuple(args, "O:cs2skcb", &temp)) {
        if (!PyCallable_Check(temp)) {
            if(temp == Py_None){
                cs2skcb(NULL);
                Py_RETURN_NONE;
            } else {
                PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                return NULL;
            }
        }
        
        // store a reference to the new callback in the global dict:
        /// the key will be the id of this panel
        if(pyKCallbackDict == NULL){
            pyKCallbackDict = PyDict_New();
            Py_XINCREF(pyKCallbackDict);
        }
        Py_XINCREF(temp);
        
        PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
        // decref the current callback if it exists
        PyObject *currentCallback = PyDict_GetItem(pyKCallbackDict, pyKey);
        if(currentCallback != NULL){
            Py_XDECREF(currentCallback);
        }
        // set the new callback for this pane
        PyDict_SetItem(pyKCallbackDict, pyKey, temp);
        
        cs2skcb(&cKCallBackFunction);
        
        Py_INCREF(Py_None); 
        result = Py_None; 
    } 
    return result;
}
static PyObject *pyNCallbackDict = NULL;
void   cNCallBackFunction(int *N){
    PyObject *argList;
    PyObject *currentCallback;
    PyObject *result;
    
    if(pyNCallbackDict == NULL){
        // cannot set an error - in a callback
        return;
    }
    currentCallback = PyDict_GetItem(pyNCallbackDict, PyInt_FromLong((long) xs2qsp()));
    if(currentCallback == NULL){
        // cannot set an error - in a callback
        return;
    }
    argList = Py_BuildValue("(i)", *N);
    result = PyEval_CallObject(currentCallback, argList);
    Py_DECREF(argList);
    
    if (result != NULL){
        Py_DECREF(result);
    }
}
static PyObject *s2plot_cs2sncb(PyObject *self, PyObject *args){
    
    PyObject *result = NULL;
    PyObject *temp;
    if (PyArg_ParseTuple(args, "O:cs2sncb", &temp)) {
        if (!PyCallable_Check(temp)) {
            if(temp == Py_None){
                cs2sncb(NULL);
                Py_RETURN_NONE;
            } else {
                PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                return NULL;
            }
        }
        
        // store a reference to the new callback in the global dict:
        /// the key will be the id of this panel
        if(pyNCallbackDict == NULL){
            pyNCallbackDict = PyDict_New();
            Py_XINCREF(pyNCallbackDict);
        }
        Py_XINCREF(temp);
        
        PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
        PyObject *currentCallback = PyDict_GetItem(pyNCallbackDict, pyKey);
        if(currentCallback != NULL){
            Py_XDECREF(currentCallback);
        }
        
        PyDict_SetItem(pyNCallbackDict, pyKey, temp);
        
        cs2sncb(&cNCallBackFunction);
        
        Py_INCREF(Py_None); 
        result = Py_None; 
    } 
    return result;
}
static PyObject *s2plot_ds2ah(PyObject *self, PyObject *args){
    PyObject *_iP, *_icol, *_ihilite;
    float size;
    XYZ iP;
    COLOUR icol, ihilite;
    unsigned int iid;
    int iselected;

    if(!PyArg_ParseTuple(args,"OfOOIi:ds2ah",&_iP, &size, &_icol,&_ihilite,&iid, &iselected) || NULL == _iP || NULL == _icol || NULL == _ihilite){
        return NULL;
    }

    // unpack lists into structs; XYZ struct...
    iP = Dict_to_XYZ(_iP);
    // COLOUR structs...
    icol = Dict_to_COLOUR(_icol);
    ihilite = Dict_to_COLOUR(_ihilite);
    
    ds2ah(iP, size, icol, ihilite, iid, iselected);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *pyHCallbackDict = NULL;
void   cHCallBackFunction(int *id){
    PyObject *argList;
    PyObject * currentCallback;
    PyObject *result;
    
    if(pyHCallbackDict == NULL){
        // cannot set an error - in a callback
        return;
    }
    currentCallback = PyDict_GetItem(pyHCallbackDict, PyInt_FromLong((long) xs2qsp()));
    if(currentCallback == NULL){
        // cannot set an error - in a callback
        return;
    }
    argList = Py_BuildValue("(i)", *id);
    result = PyEval_CallObject(currentCallback, argList);
    Py_DECREF(argList);
    
    if (result != NULL){
        Py_DECREF(result);
    }
}
static PyObject *s2plot_cs2shcb(PyObject *self, PyObject *args){
    
    PyObject *result = NULL;
    PyObject *temp;
    if (PyArg_ParseTuple(args, "O:cs2shcb", &temp)) {
        if (!PyCallable_Check(temp)) {
            if(temp == Py_None){
                cs2shcb(NULL);
                Py_RETURN_NONE;
            } else {
                PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                return NULL;
            }
        }
        
        // store a reference to the new callback in the global dict:
        // the key will be the id of this panel
        if(pyHCallbackDict == NULL){
            pyHCallbackDict = PyDict_New();
            Py_XINCREF(pyHCallbackDict);
        }
        Py_XINCREF(temp);
        
        PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
        PyObject *currentCallback = PyDict_GetItem(pyHCallbackDict, pyKey);
        if(currentCallback != NULL){
            Py_XDECREF(currentCallback);
        }
        
        PyDict_SetItem(pyHCallbackDict, pyKey, temp);
        
        cs2shcb(&cHCallBackFunction);

        Py_INCREF(Py_None); 
        result = Py_None; 
    } 
    return result;
}
static PyObject *s2plot_cs2th(PyObject *self, PyObject *args){
    unsigned int iid;

    if(!PyArg_ParseTuple(args,"I:cs2th",&iid)){
        return NULL;
    }
    
    cs2th(iid);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_cs2qhv(PyObject *self, PyObject *args){
    PyObject *result;

    result = Py_BuildValue("i:cs2qhv",cs2qhv());

    Py_INCREF(result);
    return result;
}
// ADVANCED TEXTURE AND COLORMAP HANDLING
static PyObject *s2plot_ss2lt(PyObject *self, PyObject *args){
    char *itexturefn;
    
    if(!PyArg_ParseTuple(args,"s:ss2lt",&itexturefn)){
        return NULL;
    }
    
    return Py_BuildValue("I",ss2lt(itexturefn));
}
static PyObject *s2plot_ss2gt(PyObject *self, PyObject *args){
    unsigned int textureID;
    int width, height, i, j, k, dims[3], strides[3];
    unsigned char *textureData;
    char *matrixData;
    unsigned char *index;
    PyArrayObject *textureMatrix;
    
    if(!PyArg_ParseTuple(args,"I:ss2gt",&textureID)){
        return NULL;
    }
    
    textureData = ss2gt(textureID, &width, &height);
    
    if(!textureData){
        Py_INCREF(Py_None);
        return Py_None;
    }
    dims[0] = width;
    dims[1] = height;
    dims[2] = 4;
    textureMatrix = (PyArrayObject *) PyArray_FromDims(3,dims,PyArray_UBYTE);
    strides[0] = (int) PyArray_STRIDE(textureMatrix, 0);
    strides[1] = (int) PyArray_STRIDE(textureMatrix, 1);
    strides[2] = (int) PyArray_STRIDE(textureMatrix, 2);
    matrixData = PyArray_DATA(textureMatrix);
    
    // copy the texture's data over to the numpy array
    for (i = 0; i < height; i++) {
        for(j = 0; j < width; j++){
            for(k = 0; k < 3; k++){
                index = (unsigned char*) (matrixData + i*strides[1] + j*strides[0] + k*strides[2]);
                *index = (unsigned char) textureData[4*(i*width + j) + k];
            }
        }
    }
    
    return (PyObject *) textureMatrix;
}
static PyObject *s2plot_ss2pt(PyObject *self, PyObject *args){
    // different interface to that of it's ancestor by necessity
    PyArrayObject *textureIn;
    unsigned int itextureID;
    int width, height, i, j, k;
    unsigned char *textureData;
    int strides[3];
    unsigned char *index;
    char *matrixData;

    if(!PyArg_ParseTuple(args,"IO!:ss2pt", &itextureID, &PyArray_Type, &textureIn) || textureIn == NULL){
        return NULL;
    }
    // check we've been given the correct type of array
    if(PyArray_NDIM(textureIn) != 3 || PyArray_DIM(textureIn, 2) != 4 || PyArray_TYPE(textureIn) != PyArray_UBYTE){
        PyErr_SetString(PyExc_ValueError, "texture array must be 3D byte array: width*height*(r,g,b,a)");
        return NULL;
    }
    strides[0] = (int) PyArray_STRIDE(textureIn, 0);
    strides[1] = (int) PyArray_STRIDE(textureIn, 1);
    strides[2] = (int) PyArray_STRIDE(textureIn, 2);
    
    // also need to load the texture with the id we've got
    textureData = ss2gt(itextureID, &width, &height);
    
    if(PyArray_DIM(textureIn, 0) != width || PyArray_DIM(textureIn, 1) != height){
        PyErr_SetString(PyExc_ValueError, "texture supplied must have the same dimensions as the texture in memory");
        return NULL;
    }
    
    //here need to take textureIn and place its values at the memory at textureData
    matrixData = PyArray_DATA(textureIn);
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            for(k = 0; k < 4; k++){
                index = (unsigned char*) (matrixData + i*strides[1] + j*strides[0] + k*strides[2]);
                textureData[4*(i*width + j) + k] = (unsigned char) *index;
            }
        }
    }
    
    ss2pt(itextureID);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2ptt(PyObject *self, PyObject *args){
    // different interface to that of it's ancestor by necessity
    PyArrayObject *textureIn;
    unsigned int itextureID;
    int width, height, i, j, k, strides[3];
    char *matrixData;
    unsigned char *index, *textureData;

    if(!PyArg_ParseTuple(args,"IO!:ss2ptt", &itextureID, &PyArray_Type, &textureIn) || textureIn == NULL){
        return NULL;
    }
    // check we've been given the correct type of array
    if(PyArray_NDIM(textureIn) != 3 || PyArray_DIM(textureIn, 2) != 4 || PyArray_TYPE(textureIn) != PyArray_UBYTE){
        PyErr_SetString(PyExc_ValueError, "texture array must be 3D byte array: width*height*(r,g,b,a)");
        return NULL;
    }
    
    // also need to load the texture with the id we've got
    textureData = ss2gt(itextureID, &width, &height);
    
    if(PyArray_DIM(textureIn, 0) != width || PyArray_DIM(textureIn, 1) != height){
        PyErr_SetString(PyExc_ValueError, "texture supplied must have the same dimensions as the texture in memory");
        return NULL;
    }
    
    //here need to take textureIn and place its values at the memory at textureData
    matrixData = PyArray_DATA(textureIn);
    strides[0] = (int) PyArray_STRIDE(textureIn, 0);
    strides[1] = (int) PyArray_STRIDE(textureIn, 1);
    strides[2] = (int) PyArray_STRIDE(textureIn, 2);
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            for(k = 0; k < 4; k++){
                index = (unsigned char*) (matrixData + i*strides[1] + j*strides[0] + k*strides[2]);
                textureData[4*(i*width + j) + k] = (unsigned char) *index;
            }
        }
    }
    
    ss2ptt(itextureID);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2lcm(PyObject *self, PyObject *args){
    char * imapfile;
    int startidx, maxn;

    if(!PyArg_ParseTuple(args,"sii:ss2lcm",&imapfile, &startidx, &maxn)){
        return NULL;
    }
    
    return Py_BuildValue("i",ss2lcm(imapfile, startidx, maxn));

}
// ENVIRONMENT AND RENDERING ATTRIBUTES
static PyObject *s2plot_ss2ssr(PyObject *self, PyObject *args){
    int res;
    
    if(!PyArg_ParseTuple(args,"i:ss2ssr",&res)){
        return NULL;
    }
    
    ss2ssr(res);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2srm(PyObject *self, PyObject *args){
    int mode;
    
    if(!PyArg_ParseTuple(args,"i:ss2srm",&mode)){
        return NULL;
    }
    
    ss2srm(mode);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qrm(PyObject *self, PyObject *args){
    PyObject *result;

    result = PyInt_FromLong((long) ss2qrm());
    
    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ss2sl(PyObject *self, PyObject *args){
    COLOUR ambient, *lightcol;
    XYZ *lightpos;
    int nlights, worldcoords;
    PyObject *ambientIn;
    PyObject *lightposIn, *lightcolIn;
    int i;
    
    if(!PyArg_ParseTuple(args,"OiOOi:ss2sl", &ambientIn, &nlights, &lightposIn, &lightcolIn, &worldcoords) || NULL == ambientIn || NULL == lightposIn || NULL == lightcolIn){
        return NULL;
    }

    // check the arrays - make sure None was not given for listIns (if only ambient light)
    int nPos = (lightposIn != Py_None) ? (int) PyList_Size(lightposIn) : 0;
    int nCol = (lightcolIn != Py_None) ? (int) PyList_Size(lightcolIn) : 0;
    
    if(nlights != nPos || nlights!= nCol){
        PyErr_SetString(PyExc_ValueError,"lights and colours must be lists of length nlights");
        return NULL;
    }
    
    lightpos = (XYZ *) calloc(nlights, sizeof(XYZ));
    lightcol = (COLOUR *) calloc(nlights, sizeof(COLOUR));
    
    for(i = 0; i< nlights; i++){
        lightpos[i] = Dict_to_XYZ(PyList_GetItem(lightposIn,i));
        lightcol[i] = Dict_to_COLOUR(PyList_GetItem(lightcolIn,i));
    }
    ambient = Dict_to_COLOUR(ambientIn);

    ss2sl(ambient, nlights, lightpos, lightcol, worldcoords);    

    free(lightpos);
    free(lightcol);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2sbc(PyObject *self, PyObject *args){
    float r, g, b;
    
    if(!PyArg_ParseTuple(args,"fff:ss2sbc", &r, &g, &b)){
        return NULL;
    }
    
    ss2sbc(r, g, b);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2sfra(PyObject *self, PyObject *args){
    float rot;
    
    if(!PyArg_ParseTuple(args,"f:ss2sfra", &rot)){
        return NULL;
    }
    
    ss2sfra(rot);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qpt(PyObject *self, PyObject *args){
    PyObject *result;
    
    result = PyInt_FromLong((long) ss2qpt());
    
    Py_INCREF(result);
    return result;
}
// ADVANCED CAMERA CONTROL
static PyObject *s2plot_ss2sc(PyObject *self, PyObject *args){
    PyObject *positionIn, *upIn, *vdirIn;
    XYZ position, up, vdir;
    int worldcoords;

    if(!PyArg_ParseTuple(args,"OOOi:ss2sc",&positionIn, &upIn, &vdirIn, &worldcoords) || NULL == positionIn || NULL == upIn || NULL == vdirIn){
        return NULL;
    }
    position = Dict_to_XYZ(positionIn);
    up = Dict_to_XYZ(upIn);
    vdir = Dict_to_XYZ(vdirIn);
    
    ss2sc(position, up, vdir, worldcoords);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qc(PyObject *self, PyObject *args){
    PyObject *resultTuple, *positionOut, *upOut, *vdirOut;
    int worldcoords;
    XYZ position, up, vdir;
    
    if(!PyArg_ParseTuple(args,"i:ss2qc",&worldcoords)){
        return NULL;
    }
    
    ss2qc(&position, &up, &vdir, worldcoords);
    
    resultTuple = (PyObject *) PyTuple_New(3);
    if(resultTuple == NULL) return NULL;
    
    if(!(positionOut = XYZ_to_Dict(position))) return NULL;
    if(!(upOut = XYZ_to_Dict(up))) return NULL;
    if(!(vdirOut = XYZ_to_Dict(vdir))) return NULL;
        
    PyTuple_SetItem(resultTuple,0,positionOut);
    PyTuple_SetItem(resultTuple,1,upOut);
    PyTuple_SetItem(resultTuple,2,vdirOut);
    Py_INCREF(resultTuple);
    
    return resultTuple;
}
static PyObject *s2plot_ss2sas(PyObject *self, PyObject *args){
    int startstop;
    
    if(!PyArg_ParseTuple(args, "i:ss2sas", &startstop)){
        return NULL;
    }
    
    ss2sas(startstop);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2scf(PyObject *self, PyObject *args){
    XYZ position;
    int worldcoords;
    PyObject *positionIn;
    
    if(!PyArg_ParseTuple(args, "Oi:ss2scf", &positionIn, &worldcoords) || NULL == positionIn){
        return NULL;
    }
    
    position = Dict_to_XYZ(positionIn);
    
    ss2scf(position, worldcoords);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2ucf(PyObject *self, PyObject *args){
    ss2ucf();
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2sca(PyObject *self, PyObject *args){ /* NEW */
    float aperture;
    
    if(!PyArg_ParseTuple(args, "f:ss2sca", &aperture)){
        return NULL;
    }
    
    ss2sca(aperture);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qca(PyObject *self, PyObject *args){ /* NEW */
    PyObject *result;

    result = PyFloat_FromDouble((double) ss2qca());

    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ss2sss(PyObject *self, PyObject *args){
    float spd;
    
    if(!PyArg_ParseTuple(args, "f:ss2sss", &spd)){
        return NULL;
    }
    
    ss2sss(spd);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qss(PyObject *self, PyObject *args){
    PyObject *result;

    result = PyFloat_FromDouble((double) ss2qss());

    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ss2scs(PyObject *self, PyObject *args){ /* NEW */
    float spd;
    
    if(!PyArg_ParseTuple(args, "f:ss2scs", &spd)){
        return NULL;
    }
    
    ss2scs(spd);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qcs(PyObject *self, PyObject *args){ /* NEW */
    PyObject *result;

    result = PyFloat_FromDouble((double) ss2qcs());

    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ss2tc(PyObject *self, PyObject *args){
    int enabledisable;

    if(!PyArg_ParseTuple(args,"i:ss2tc",&enabledisable)){
        return NULL;
    }
    
    ss2tc(enabledisable);
    
    Py_INCREF(Py_None);
    return Py_None;

}
static PyObject *s2plot_ss2qess(PyObject *self, PyObject *args){
    PyObject *result;

    result = PyFloat_FromDouble((double) ss2qess());

    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ss2sess(PyObject *self, PyObject *args){
    float sep;
    
    if(!PyArg_ParseTuple(args, "f:ss2sess", &sep)){
        return NULL;
    }
    
    ss2sess(sep);
    
    Py_INCREF(Py_None);
    return Py_None;
}
// ADDITIONAL PRIMITIVES
static PyObject *s2plot_s2chromapts(PyObject *self, PyObject *args){
    int n;
    float *ilong, *lat, *dist, *size, radius, dmin, dmax;
    PyArrayObject *ilongIn, *latIn, *distIn, *sizeIn;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"iO!O!O!O!fff:s2chromapts",&n, &PyArray_Type, &ilongIn, &PyArray_Type, &latIn, &PyArray_Type, &distIn, &PyArray_Type, &sizeIn, &radius, &dmin, &dmax) || NULL == ilongIn || NULL == latIn || NULL == distIn || NULL == sizeIn){
        return NULL;
    }

    if(!(ilong = numpy1D_to_float(ilongIn))) {return NULL;}
    if(!(lat = numpy1D_to_float(latIn))) {numpy_free(ilongIn, ilong); return NULL;}
    if(!(dist = numpy1D_to_float(distIn))){
        numpy_free(ilongIn, ilongIn);
        numpy_free(latIn, lat);
        return NULL;
    }
    if(!(size = numpy1D_to_float(sizeIn))){
        numpy_free(ilongIn, ilongIn);
        numpy_free(latIn, lat);
        numpy_free(distIn, dist);
        return NULL;
    }

    s2chromapts(n, ilong, lat, dist, size, radius, dmin, dmax);

    numpy_free(ilongIn, ilong);
    numpy_free(latIn, lat);
    numpy_free(distIn, dist);
    numpy_free(sizeIn, size);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2chromacpts(PyObject *self, PyObject *args){
    int n;
    float *ix, *iy, *iz, *dist, *size, dmin, dmax;
    PyArrayObject *ixIn, *iyIn, *izIn, *distIn, *sizeIn;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"iO!O!O!O!O!ff:s2chromacpts",&n, &PyArray_Type, &ixIn, &PyArray_Type, &iyIn, &PyArray_Type, &izIn, &PyArray_Type, &distIn, &PyArray_Type, &sizeIn, &dmin, &dmax) || NULL == ixIn || NULL == iyIn || NULL == izIn || NULL == distIn || NULL == sizeIn){
        return NULL;
    }

    if(!(ix = numpy1D_to_float(ixIn))) {return NULL;}
    if(!(iy = numpy1D_to_float(iyIn))){
        numpy_free(ixIn, ix);
        return NULL;
    }
    if(!(iz = numpy1D_to_float(izIn))) {
        numpy_free(ixIn, ix);
        numpy_free(iyIn, iy);
        return NULL;
    }
    if(!(dist = numpy1D_to_float(distIn))) {
        numpy_free(ixIn, ix);
        numpy_free(iyIn, iy);
        numpy_free(izIn, iz);
        return NULL;
    }
    if(!(size = numpy1D_to_float(sizeIn))) {
        numpy_free(ixIn, ix);
        numpy_free(iyIn, iy);
        numpy_free(izIn, iz);
        numpy_free(distIn, dist);
        return NULL;
    }

    s2chromacpts(n, ix, iy, iz, dist, size, dmin, dmax);

    numpy_free(ixIn, ix);
    numpy_free(iyIn, iy);
    numpy_free(izIn, iz);
    numpy_free(distIn, dist);
    numpy_free(sizeIn, size);

    Py_INCREF(Py_None);
    return Py_None;
}
// **FUNCTIONS IN TESTING/DEVELOPMENT
static PyObject *s2plot_ss2ltt(PyObject *self, PyObject *args){
    float aspect;
    char *command;
    unsigned int tex_id;
    PyObject *pyResult = NULL;

    if(!PyArg_ParseTuple(args, "s:ss2ltt", &command)){
        return NULL;
    }
    
    tex_id = ss2ltt(command, &aspect);
    
    pyResult = (PyObject *) PyDict_New();
    if(pyResult == NULL) return NULL;
    
    PyDict_SetItemString(pyResult, "texture_id", PyInt_FromLong((long) tex_id));
    PyDict_SetItemString(pyResult, "aspect", PyFloat_FromDouble((double) aspect));
    
    Py_INCREF(pyResult);
    return pyResult;
    
}
static PyObject *s2plot_ns2vf3a(PyObject *self, PyObject *args){
    XYZ P[3];
    COLOUR col;
    char trans;
    float alpha;
    PyObject *PIn, *colIn;
    int i;
    
    if(!(PyArg_ParseTuple(args, "OOsf:ns2vf3a", &PIn, &colIn, &trans, &alpha)) || NULL == PIn || NULL == colIn){
        return NULL;
    }

    for(i = 0; i < 3; i++){
        P[i] = Dict_to_XYZ(PyList_GetItem(PIn,i));
    }
    col = Dict_to_COLOUR(colIn);

    ns2vf3a(P, col, trans, alpha);
    
    Py_INCREF(Py_None);
    return Py_None;    
}
static PyObject *s2plot_ns2vpa(PyObject *self, PyObject *args){
    XYZ P;
    COLOUR icol;
    char itrans;
    float isize, ialpha;
    PyObject *PIn, *colIn;
    
    if(!(PyArg_ParseTuple(args, "OOfsf:ns2vpa", &PIn, &colIn, &isize, &itrans, &ialpha)) || NULL == PIn || NULL == colIn){
        return NULL;
    }

    P = Dict_to_XYZ(PIn);
    icol = Dict_to_COLOUR(colIn);
    
    ns2vpa(P, icol, isize, itrans, ialpha);
    
    Py_INCREF(Py_None);
    return Py_None;    
}
static PyObject *s2plot_ns2cis(PyObject *self, PyObject *args){
    float ***grid, *tr, level, alpha, red, green, blue;
    int adim, bdim, cdim, a1, a2, b1, b2, c1, c2, resolution, id;
    char *trans;
    PyArrayObject *gridIn, *trIn;
    PyObject *result;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"O!iiiiiiiiiOfisffff:ns2cis", &PyArray_Type, &gridIn, &adim, &bdim, &cdim, &a1, &a2, &b1, &b2, &c1, &c2, &trIn, &level, &resolution, &trans, &alpha, &red, &green, &blue) || NULL == &gridIn || NULL == &trIn){
        return NULL;
    }
    if(!(grid = numpy3D_to_float(gridIn))) {return NULL;}
    // None is allowed as input - for standard tr matrix
    if((PyObject *) trIn == Py_None) {tr = NULL;}
    else if(!(tr = numpy1D_to_float(trIn))){
        numpy_free(gridIn, grid);
        return NULL;
    }
    
    id = ns2cis(grid, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, tr, level, resolution, *trans, alpha, red, green, blue);

    // don't free grid: the memory is used by the surface drawer
    numpy_free(trIn, tr);

    result = PyInt_FromLong((long) id);
    Py_INCREF(result);
    return result;
}
static PyObject *pyActiveColourCallback = NULL;
static PyObject *pyColourCallbackDict = NULL;
void cColourCallback(float *x, float *y, float *z, float *r, float *g, float *b){ 
    PyObject *arg, *pyResult;
    COLOUR result;
    // python signature is: takes x, y, z: returns rgb dictionary
    
    arg = Py_BuildValue("fff", *x, *y, *z);
    pyResult = PyEval_CallObject(pyActiveColourCallback, arg);
    Py_DECREF(arg);

    // errors?
    if(pyResult == NULL) return;

    result = Dict_to_COLOUR(pyResult);
    *r = result.r;
    *g = result.g;
    *b = result.b;

    Py_XDECREF(pyResult);
}
static PyObject *s2plot_ns2cisc(PyObject *self, PyObject *args){
  float ***grid, *tr, level, alpha;
    int adim, bdim, cdim, a1, a2, b1, b2, c1, c2, resolution, id;
    char *trans;
    PyArrayObject *gridIn, *trIn;
    PyObject *result, *tempColCall;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"O!iiiiiiiiiOfisfO:ns2cisc", &PyArray_Type, &gridIn, &adim, &bdim, &cdim, &a1, &a2, &b1, &b2, &c1, &c2, &trIn, &level, &resolution, &trans, &alpha, &tempColCall) || NULL == &gridIn || NULL == &trIn || NULL == &tempColCall){
        return NULL;
    }
    if (!PyCallable_Check(tempColCall)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    
    // store a reference to the new callback
    Py_XINCREF(tempColCall);
    // create the callback list if it does not exist
    if(pyColourCallbackDict == NULL){
        pyColourCallbackDict = PyDict_New();
        Py_XINCREF(pyColourCallbackDict);
    }
    
    if(!(grid = numpy3D_to_float(gridIn))) {return NULL;}
    // None is allowed as input - for standard tr matrix
    if((PyObject *) trIn == Py_None) {tr = NULL;}
    else if(!(tr = numpy1D_to_float(trIn))){
        numpy_free(gridIn, grid);
        return NULL;
    }
    
    id = ns2cisc(grid, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, tr, level, resolution, *trans, alpha, cColourCallback);

    // add the new callback to the dict with id as the key
    PyDict_SetItem(pyColourCallbackDict, PyInt_FromLong((long) id), tempColCall);

    numpy_free(trIn, tr);

    result = PyInt_FromLong((long) id);
    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ns2dis(PyObject *self, PyObject *args){
    int isid, force;
    
    if(!PyArg_ParseTuple(args, "ii:ns2dis", &isid, &force)){
        return NULL;
    }

    // we don't know whether this is a colour callback or not
    // in any case, set the active colour callback to the isid'th one
    // if the list exists
    if(pyColourCallbackDict != NULL){
        // will return NULL if key doesn't exist
        pyActiveColourCallback = PyDict_GetItem(pyColourCallbackDict, PyInt_FromLong((long) isid));
    }

    ns2dis(isid, force);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2sisl(PyObject *self, PyObject *args){
    int isid;
    float level;
    
    if(!PyArg_ParseTuple(args, "if:ns2sisl", &isid, &level)){
        return NULL;
    }

    ns2sisl(isid, level);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2sisa(PyObject *self, PyObject *args){
    int isid;
    float alpha;
    char *trans;
    
    if(!PyArg_ParseTuple(args, "ifs:ns2sisa", &isid, &alpha, &trans)){
        return NULL;
    }

    ns2sisa(isid, alpha, trans[0]);
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2sisc(PyObject *self, PyObject *args){
    int isid;
    float r, g, b;
    
    if(!PyArg_ParseTuple(args, "ifff:ns2sisc", &isid, &r, &g, &b)){
        return NULL;
    }

    ns2sisc(isid, r, g, b);
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2cvr(PyObject *self, PyObject *args){
    float ***grid, *tr, datamin, datamax, alphamin, alphamax;
    int adim, bdim, cdim, a1, a2, b1, b2, c1, c2, id;
    char *trans;
    PyArrayObject *gridIn, *trIn;
    PyObject *result;
    
    // parse the args into numpy array objects
    if(!PyArg_ParseTuple(args,"O!iiiiiiiiiOsffff:ns2cvr", &PyArray_Type, &gridIn, &adim, &bdim, &cdim, &a1, &a2, &b1, &b2, &c1, &c2, &trIn, &trans, &datamin, &datamax, &alphamin, &alphamax) || NULL == &gridIn || NULL == &trIn){
        return NULL;
    }
    if(!(grid = numpy3D_to_float(gridIn))) {return NULL;}
    // None is allowed as input - for standard tr matrix
    if((PyObject *) trIn == Py_None) {tr = NULL;}
    else if(!(tr = numpy1D_to_float(trIn))){
        numpy_free(gridIn, grid);
        return NULL;
    }
            
    id = ns2cvr(grid, adim, bdim, cdim, a1, a2, b1, b2, c1, c2, tr, *trans, datamin, datamax, alphamin, alphamax);
    
    // don't free grid: the memory is used by the surface drawer
    numpy_free(trIn, tr);

    result = PyInt_FromLong((long) id);
    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_ds2dvr(PyObject *self, PyObject *args){
    int vrid, force;
    
    if(!PyArg_ParseTuple(args, "ii:ds2dvr", &vrid, &force)){
        return NULL;
    }

    ds2dvr(vrid, force);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ns2svrl(PyObject *self, PyObject *args){
    int vrid;
    float datamin, datamax, alphamin, alphamax;
    
    if(!PyArg_ParseTuple(args, "iffff:ns2svrl", &vrid, &datamin, &datamax, &alphamin, &alphamax)){
        return NULL;
    }            
    
    ns2svrl(vrid, datamin, datamax, alphamin, alphamax);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2ct(PyObject *self, PyObject *args){
    int width, height;
    unsigned int id;
    PyObject *result;
    
    if(!PyArg_ParseTuple(args, "ii:ss2ct", &width, &height)){
        return NULL;
    }
    
    id = ss2ct(width, height);    
    
    result = PyInt_FromLong((long) id);
    
    Py_XINCREF(result);
    return result;
}
static PyObject *s2plot_ss2ctt(PyObject *self, PyObject *args){
    int width, height;
    unsigned int id;
    PyObject *result;
    
    if(!PyArg_ParseTuple(args, "ii:ss2ctt", &width, &height)){
        return NULL;
    }
    
    id = ss2ctt(width, height);
    
    result = PyInt_FromLong((long) id);
    
    Py_XINCREF(result);
    return result;
}
static PyObject *s2plot_ss2dt(PyObject *self, PyObject *args){
    unsigned int id;
    
    if(!PyArg_ParseTuple(args, "i:ss2dt", &id)){
        return NULL;
    }
    
    ss2dt(id);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2txh(PyObject *self, PyObject *args){
    int enabledisable;
    
    if(!PyArg_ParseTuple(args, "i:ss2txh", &enabledisable)){
        return NULL;
    }
    
    ss2txh(enabledisable);
    
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2qxh(PyObject *self, PyObject *args){
    PyObject *result;

    result = PyBool_FromLong((long) ss2qxh());

    Py_INCREF(result);
    return result;
}
static PyObject *s2plot_cs2thv(PyObject *self, PyObject *args){
    int enabledisable;
    
    if(!PyArg_ParseTuple(args, "i:cs2thv", &enabledisable)){
        return NULL;
    }
    
    cs2thv(enabledisable);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2skypa(PyObject *self, PyObject *args){
    float **data, *tr;
    PyArrayObject *dataObject, *trIn;
    int n[2], i[2], j[2], walls, idx_left, idx_front;
    float dataRange[2];
    
    // parse the python into C
    if(!PyArg_ParseTuple(args, "O!iiiiiiffO!iii:s2skypa", &PyArray_Type, &dataObject, &n[0], &n[1], &i[0], &i[1], &j[0], &j[1], &dataRange[0], &dataRange[1], &PyArray_Type, &trIn, &walls, &idx_left, &idx_front) || NULL == &dataObject || NULL == &trIn){
        return NULL;
    }
    
    if(!(data = numpy2D_to_float(dataObject))) return NULL;
    if(!(tr = numpy1D_to_float(trIn))) return NULL;
    
    s2skypa(data, n[0], n[1], i[0], i[1], j[0], j[1], dataRange[0], dataRange[1], tr, walls, idx_left, idx_front);

    numpy_free(dataObject, data);
    numpy_free(trIn, tr);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2impa(PyObject *self, PyObject *args){
    float **data, *tr;
    PyArrayObject *dataObject, *trIn;
    int n[2], i[2], j[2], trunk, symbol;
    float dataRange[2];
    
    // parse the python into C
    if(!PyArg_ParseTuple(args, "O!iiiiiiffO!ii:s2impa", &PyArray_Type, &dataObject, &n[0], &n[1], &i[0], &i[1], &j[0], &j[1], &dataRange[0], &dataRange[1], &PyArray_Type, &trIn, &trunk, &symbol) || NULL == &dataObject || NULL == &trIn){
        return NULL;
    }
    
    if(!(data = numpy2D_to_float(dataObject))) return NULL;
    if(!(tr = numpy1D_to_float(trIn))) return NULL;
    
    s2impa(data, n[0], n[1], i[0], i[1], j[0], j[1], dataRange[0], dataRange[1], tr, trunk, symbol);

    numpy_free(dataObject, data);
    numpy_free(trIn, tr);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_ss2spt(PyObject *self, PyObject *args){
    int projtype;
    
    if(!PyArg_ParseTuple(args, "i:ss2spt", &projtype)){
        return NULL;
    }
    
    ss2spt(projtype);

    Py_INCREF(Py_None);
    return Py_None;
}
/* Non-standard function. Do not document.  Set the expansion scale of 
 * near and far clipping planes.  Use with great caution!
 */
//void ss2snfe(float expand);
static PyObject *s2plot_ss2sfc(PyObject *self, PyObject *args){
    float r, g, b;
    
    if(!PyArg_ParseTuple(args, "fff:ss2sfc", &r, &g, &b)){
        return NULL;
    }
    
    ss2sfc(r, g, b);

    Py_INCREF(Py_None);
    return Py_None;
}
// **** NO EXAMPLES FROM HERE **** //
static PyObject *s2plot_xs2ap(PyObject *self, PyObject *args){
    int result;
    PyObject * pyResult = NULL;
    float x[2], y[2];
    
    if(!PyArg_ParseTuple(args, "ffff:xs2ap", &x[0], &y[0], &x[1], &y[1])){
        return NULL;
    }
    
    result = xs2ap(x[0], y[0], x[1], y[1]);

    pyResult = PyInt_FromLong((long) result);

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_xs2tp(PyObject *self, PyObject *args){
    int panel_id;
    
    if(!PyArg_ParseTuple(args, "i:xs2tp", &panel_id)){
        return NULL;
    }
    
    xs2tp(panel_id);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_xs2cp(PyObject *self, PyObject *args){
    int panel_id;
    
    if(!PyArg_ParseTuple(args, "i:xs2cp", &panel_id)){
        return NULL;
    }
    
    xs2cp(panel_id);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_xs2mp(PyObject *self, PyObject *args){
    int panel_id;
    float x[2], y[2];
    
    if(!PyArg_ParseTuple(args, "iffff:xs2mp", &panel_id, &x[0], &y[0], &x[1], &y[1])){
        return NULL;
    }
    
    xs2mp(panel_id, x[0], y[0], x[1], y[1]);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_xs2lpc(PyObject *self, PyObject *args){
    int master_id, slave_id;
    
    if(!PyArg_ParseTuple(args, "ii:xs2lpc", &master_id, &slave_id)){
        return NULL;
    }
    
    xs2lpc(master_id, slave_id);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_xs2qpa(PyObject *self, PyObject *args){
    int panel_id, result;
    PyObject * pyResult = NULL;
    
    if(!PyArg_ParseTuple(args, "i:xs2qpa", &panel_id)){
        return NULL;
    }
    
    result = xs2qpa(panel_id);
    pyResult = PyInt_FromLong((long) result);

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_xs2qcpa(PyObject *self, PyObject *args){
    int result;
    PyObject * pyResult = NULL;
    
    result = xs2qcpa();
    pyResult = PyBool_FromLong((long) result);
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_xs2spp(PyObject *self, PyObject *args){ /* NEW */
    COLOUR active, inactive;
    float width;
    PyObject *activeIn, *inactiveIn;

    if(!PyArg_ParseTuple(args, "OOf:xs2spp", &activeIn, &inactiveIn, &width) || NULL == activeIn || NULL == inactiveIn ){
        return NULL;
    }
    
    active = Dict_to_COLOUR(activeIn);
    inactive = Dict_to_COLOUR(inactiveIn);

    xs2spp(active, inactive, width);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_xs2qsp(PyObject *self, PyObject *args){
    int result;
    PyObject * pyResult = NULL;
    
    result = xs2qsp();
    
    pyResult = PyInt_FromLong((long) result);
    
    Py_INCREF(pyResult);
    return pyResult;
}
// Not Yet Implemented in S2PLOT CORE
//static PyObject *s2plot_xs2lpg(PyObject *self, PyObject *args){
//    int master_id, slave_id;
//    float x[2], y[2];
//    
//    if(!PyArg_ParseTuple(args, "ii", &master_id, &slave_id)){
//        return NULL;
//    }
//    
//    xs2lpg(master_id, slave_id);
//
//    Py_INCREF(Py_None);
//    return Py_None;
//}
static PyObject *s2plot_ss2tsc(PyObject *self, PyObject *args){
    char *screens;
    
    if(!PyArg_ParseTuple(args, "s:ss2tsc", &screens)){
        return NULL;
    }
    
    ss2tsc(screens);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2qci(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyInt_FromLong((long) s2qci());
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_s2qlw(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyInt_FromLong((long) s2qlw());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_s2qls(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyInt_FromLong((long) s2qls());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_s2qch(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyFloat_FromDouble((double) s2qch());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_s2qah(PyObject *self, PyObject *args){
    int fs;
    float angle, barb;
    PyObject * pyResult = NULL;
    
    s2qah(&fs, &angle, &barb);
    
    pyResult = (PyObject *) PyDict_New();
    if(pyResult == NULL) return NULL;
    
    PyDict_SetItemString(pyResult, "fs", PyInt_FromLong((long) fs));
    PyDict_SetItemString(pyResult, "angle", PyFloat_FromDouble((double) angle));
    PyDict_SetItemString(pyResult, "barb", PyFloat_FromDouble((double) barb));
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_s2twc(PyObject *self, PyObject *args){ /* NEW */
    int enabledisable;

    if(!PyArg_ParseTuple(args,"i:s2twc",&enabledisable)){
        return NULL;
    }
    
    s2twc(enabledisable);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *s2plot_s2qwc(PyObject *self, PyObject *args){ /* NEW */
    int clipping_state;

    clipping_state = s2qwc();

    if(clipping_state == 1){Py_RETURN_TRUE;}
    else if(clipping_state == 0){Py_RETURN_FALSE;}

    Py_INCREF(Py_None); /* Should raise exception */
    return Py_None;
}
static PyObject *s2plot_ss2qsr(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyInt_FromLong((long) ss2qsr());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_ss2qbc(PyObject *self, PyObject *args){
    float r, g, b;
    COLOUR rgb;
    PyObject * pyResult = NULL;
    
    ss2qbc(&r, &g, &b);
    rgb.r = r; rgb.g = g; rgb.b = b;
    
    pyResult = COLOUR_to_Dict(rgb);
    
    Py_INCREF(pyResult);
    return pyResult;
}  
static PyObject *s2plot_ss2qfc(PyObject *self, PyObject *args){
    float r, g, b;
    COLOUR rgb;
    PyObject * pyResult = NULL;
    
    ss2qfc(&r, &g, &b);
    rgb.r = r; rgb.g = g; rgb.b = b;
    
    pyResult = COLOUR_to_Dict(rgb);
    
    Py_INCREF(pyResult);
    return pyResult;
} 
static PyObject *s2plot_ss2qfra(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyFloat_FromDouble((double) ss2qfra());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_ss2qas(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyInt_FromLong((long) ss2qas());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_ss2qcf(PyObject *self, PyObject *args){
    int set, worldcoords;
    XYZ position;
    PyObject *pyPosition = NULL;
    PyObject *pySet = NULL;
    PyObject *pyResult = NULL;
    
    
    if(!PyArg_ParseTuple(args, "i:ss2qcf", &worldcoords)){
        return NULL;
    }
    
    ss2qcf(&set, &position, worldcoords);
    
    pySet = (set == 0 ? Py_False : Py_True);
    Py_INCREF(pySet);
    pyPosition = XYZ_to_Dict(position);
    Py_INCREF(pyPosition);
    
    pyResult = (PyObject *) PyDict_New();
    if(pyResult == NULL) return NULL;
    PyDict_SetItemString(pyResult, "focus_set", pySet);
    PyDict_SetItemString(pyResult, "position", pyPosition);    
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_ss2qar(PyObject *self, PyObject *args){
    PyObject * pyResult = NULL;
    
    pyResult = PyFloat_FromDouble((double) ss2qar());

    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_ss2qsa(PyObject *self, PyObject *args){
    int stereo, fullscreen, dome;
    PyObject *pyStereo = NULL;
    PyObject *pyFullscreen = NULL;
    PyObject *pyDome = NULL;
    PyObject *pyResult = NULL;
    
    ss2qsa(&stereo, &fullscreen, &dome);
    
    pyStereo = PyInt_FromLong((long) stereo);
    Py_INCREF(pyStereo);
    pyFullscreen = PyInt_FromLong((long) fullscreen);
    Py_INCREF(pyFullscreen);
    pyDome = PyInt_FromLong((long) dome);
    Py_INCREF(pyDome);
    
    pyResult = (PyObject *) PyDict_New();
    if(pyResult == NULL) return NULL;
    PyDict_SetItemString(pyResult, "stereo", pyStereo);
    PyDict_SetItemString(pyResult, "fullscreen", pyFullscreen);
    PyDict_SetItemString(pyResult, "dome", pyDome);
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_cs2scbx(PyObject *self, PyObject *args){
   PyObject *result = NULL;
   PyObject *temp, *data = NULL, *tuple;
   if (PyArg_ParseTuple(args, "O|O:cs2scbx", &temp, &data)) {
       if (!PyCallable_Check(temp)) {
           if(temp == Py_None){
               cs2scbx(NULL, NULL);
               Py_RETURN_NONE;
           } else {
               PyErr_SetString(PyExc_TypeError, "parameter must be callable");
               return NULL;
           }
       }
       if(!data){
           result = s2plot_cs2scb(self, args);
           return result;
       }
       // store a reference to the new callback in the global dict:
       /// the key will be the id of this panel
       if(pyCallbackDict == NULL){
           pyCallbackDict = PyDict_New();
           Py_XINCREF(pyCallbackDict);
       }
       Py_XINCREF(temp);
       Py_XINCREF(data);
       // create a new tuple containing the callback/data
       tuple = PyTuple_Pack(2, temp, data);
       Py_XINCREF(tuple);

       PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
       // decref the current callback if it exists
       // in this case the pyObjects stored are tuples of (callback, dataObject)
       PyObject *currentCallback = PyDict_GetItem(pyCallbackDict, pyKey);
       if(currentCallback != NULL){
           if(PyTuple_Check(currentCallback) > 0 && PyTuple_Size(currentCallback) == 2){
               Py_XDECREF(PyTuple_GET_ITEM(currentCallback, 0));
               Py_XDECREF(PyTuple_GET_ITEM(currentCallback, 1));
           }
           Py_XDECREF(currentCallback);
       }
       // set the new callback for this pane
       PyDict_SetItem(pyCallbackDict, pyKey, tuple);

       cs2scb(&cCallBackFunction);

       Py_INCREF(Py_None); 
       result = Py_None; 
   } 
   return result;
}
static PyObject *s2plot_ss2qsd(PyObject *self, PyObject *args){
    int x, y;
    PyObject *xOut, *yOut;
    PyObject *pyResult;
    
    ss2qsd(&x, &y);
    
    xOut = PyInt_FromLong((long) x);
    Py_INCREF(xOut);
    yOut = PyInt_FromLong((long) y);
    Py_INCREF(yOut);
    pyResult = (PyObject *) PyDict_New();
    if(pyResult == NULL) return NULL;
    PyDict_SetItemString(pyResult, "x", xOut);
    PyDict_SetItemString(pyResult, "y", yOut);
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *s2plot_ss2qnfp(PyObject *self, PyObject *args){
    double near, far;
    PyObject *nearOut, *farOut, *pyResult;
    
    ss2qnfp(&near, &far);
    
    nearOut = PyFloat_FromDouble(near);
    Py_INCREF(nearOut);
    farOut = PyFloat_FromDouble(far);
    Py_INCREF(farOut);
    pyResult = (PyObject *) PyTuple_New(2);
    if(pyResult == NULL) return NULL;
    PyTuple_SetItem(pyResult, 0, nearOut);
    PyTuple_SetItem(pyResult, 1, farOut);
    
    Py_INCREF(pyResult);
    return pyResult;
}
static PyObject *pyDHCallbackDict = NULL;
void cDHCallBackFunction(int *id, XYZ *pt){ 
    PyObject *arg;
    PyObject *currentCallback;
    PyObject *pyResult;
    PyObject *pyPt = NULL;
    
    pyPt = XYZ_to_Dict(*pt);
    
    if(pyDHCallbackDict == NULL){
        // cannot set exception - in callback
        return;
    }
    currentCallback = PyDict_GetItem(pyDHCallbackDict, PyInt_FromLong((long) xs2qsp()));
    if(currentCallback == NULL){
        // cannot set exception - in callback
        return;
    }
    arg = Py_BuildValue("iO", id, pyPt);
    pyResult = PyEval_CallObject(currentCallback, arg);
    Py_DECREF(arg);
    
    if (pyResult != NULL){
        Py_DECREF(pyResult);
    }
}
static PyObject *s2plot_cs2sdhcb(PyObject *self, PyObject *args){
    PyObject *result = NULL;
    PyObject *temp;
    if (PyArg_ParseTuple(args, "O:cs2sdhcb", &temp)) {
        if (!PyCallable_Check(temp)) {
            if(temp == Py_None){
                cs2sdhcb(NULL);
                Py_RETURN_NONE;
            } else {
                PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                return NULL;
            }
        }
        
        // store a reference to the new callback in the global dict:
        /// the key will be the id of this panel
        if(pyDHCallbackDict == NULL){
            pyDHCallbackDict = PyDict_New();
            Py_XINCREF(pyDHCallbackDict);
        }
        Py_XINCREF(temp);
        
        PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
        // decref the current callback if it exists
        PyObject *currentCallback = PyDict_GetItem(pyDHCallbackDict, pyKey);
        if(currentCallback != NULL){
            Py_XDECREF(currentCallback);
        }
        // set the new callback for this pane
        PyDict_SetItem(pyDHCallbackDict, pyKey, temp);
        
        cs2sdhcb(&cDHCallBackFunction);
        
        Py_INCREF(Py_None); 
        result = Py_None; 
    } 
    return result;
}
static PyObject *pyPCallbackDict = NULL;
void cPCallBackFunction(char *string){ 
    PyObject *arg;
    PyObject *currentCallback, *callable, *callData;
    PyObject *pyResult;
    
    if(pyPCallbackDict == NULL){
        // cannot set exception - in callback
        return;
    }
    currentCallback = PyDict_GetItem(pyPCallbackDict, PyInt_FromLong((long) xs2qsp()));
    if(currentCallback == NULL || !PyTuple_Check(currentCallback) || PyTuple_Size(currentCallback) != 2){
        // cannot set exception - in callback
        return;
    }
    callable = PyTuple_GetItem(currentCallback, 0);
    callData = PyTuple_GetItem(currentCallback, 1);
    if(callData == Py_None){
        arg = Py_BuildValue("(s)", string);
    } else {
        arg = Py_BuildValue("sO", string, callData);
    }
    pyResult = PyEval_CallObject(callable, arg);
    Py_DECREF(arg);
    
    if (pyResult != NULL){
        Py_DECREF(pyResult);
    }
}
static PyObject *s2plot_cs2spcb(PyObject *self, PyObject *args){
    PyObject *result = NULL;
    PyObject *temp, *data = NULL, *tuple;
    if (PyArg_ParseTuple(args, "O|O:cs2spcb", &temp, &data)) {
        if (!PyCallable_Check(temp)) {
            if(temp == Py_None){
                cs2spcb(NULL, NULL);
                Py_RETURN_NONE;
            } else {
                PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                return NULL;
            }
        }
        if(!data){
            data = Py_None;
        }
        // store a reference to the new callback in the global dict:
        /// the key will be the id of this panel
        if(pyPCallbackDict == NULL){
            pyPCallbackDict = PyDict_New();
            Py_XINCREF(pyPCallbackDict);
        }
        Py_XINCREF(temp);
        Py_XINCREF(data);
        // create a new tuple containing the callback/data
        tuple = PyTuple_Pack(2, temp, data);
        Py_XINCREF(tuple);
        
        PyObject *pyKey = PyInt_FromLong((long) xs2qsp());
        // decref the current callback if it exists
        // in this case the pyObjects stored are tuples of (callback, dataObject)
        PyObject *currentCallback = PyDict_GetItem(pyPCallbackDict, pyKey);
        if(currentCallback != NULL){
            if(PyTuple_Check(currentCallback) > 0 && PyTuple_Size(currentCallback) == 2){
                Py_XDECREF(PyTuple_GET_ITEM(currentCallback, 0));
                Py_XDECREF(PyTuple_GET_ITEM(currentCallback, 1));
            }
            Py_XDECREF(currentCallback);
        }
        // set the new callback for this pane
        PyDict_SetItem(pyPCallbackDict, pyKey, tuple);
        
        cs2spcb(&cPCallBackFunction, NULL);
        
        Py_INCREF(Py_None); 
        result = Py_None; 
    } 
    return result;
}
static PyObject *s2plot_cs2sptxy(PyObject *self, PyObject *args){ /* NEW */
    char *prompt;
    float xfrac, yfrac;

    if(!PyArg_ParseTuple(args, "sff:cs2sptxy", &prompt, &xfrac, &yfrac)){
        return NULL;
    }

    cs2sptxy(prompt, xfrac, yfrac);

    Py_RETURN_NONE;
}
static PyObject *s2plot_ds2ahx(PyObject *self, PyObject *args){
    XYZ iP;
    COLOUR icol, ihilite;
    float size;
    int itex, ihitex, iselected;;
    unsigned int iid;
    PyObject *iPIn, *icolIn, *ihiliteIn;

    if(!PyArg_ParseTuple(args, "OfiiOOIi:ds2ahx", &iPIn, &size, &itex, &ihitex, &icolIn, &ihiliteIn, &iid, &iselected) || iPIn == NULL || icolIn == NULL || ihiliteIn == NULL){
        return NULL;
    }
    
    iP = Dict_to_XYZ(iPIn);
    icol = Dict_to_COLOUR(icolIn);
    ihilite = Dict_to_COLOUR(ihiliteIn);
    
    ds2ahx(iP, size, itex, ihitex, icol, ihilite, iid, iselected);
    
    Py_RETURN_NONE;
}
//static PyObject *s2plot_ss2ftt(PyObject *self, PyObject *args){
//    const char *fontfilename, *text;
//    int fontsizepx, border;
//    unsigned int result;
//    PyObject *pyResult;
//
//    if(!PyArg_ParseTuple(args, "ssii", &fontfilename, &text, &fontsizepx, &border)){
//        return NULL;
//    }
//    
//    result = ss2ftt((char *)fontfilename, (char *)text, fontsizepx, border);
//    
//    pyResult = PyInt_FromLong((long) result);
//    Py_XINCREF(pyResult);
//    
//    return pyResult;
//}
static PyObject *s2plot_pushVRMLname(PyObject *self, PyObject *args){
    const char *iname;

    if(!PyArg_ParseTuple(args, "s:pushVRMLname", &iname)){
        return NULL;
    }
    
    pushVRMLname((char *)iname);
    
    Py_RETURN_NONE;
}
//static PyObject *s2plot_s2show_thr(PyObject *self, PyObject *args){
//    int interactive = 1;
//
//    if(!PyArg_ParseTuple(args, "|i", &interactive)){
//        return NULL;
//    }
//    
//    s2show_thr(interactive);
//    
//    Py_RETURN_NONE;
//}
static PyObject *s2plot_ss2wtga(PyObject *self, PyObject *args){
    const char *fname;

    if(!PyArg_ParseTuple(args, "s:ss2wtga", &fname)){
        return NULL;
    }
    
    ss2wtga((char *)fname);
    
    Py_RETURN_NONE;
}
static PyObject *s2plot_ss2gpix(PyObject *self, PyObject *args){
    unsigned int width, height;
    unsigned char *result;
    char *matrixData;
    unsigned char *index;
    PyArrayObject *pyMatrix;
    int dims[3], i, j, k, strides[3];
    
    result = ss2gpix(&width, &height);
    
    if(!result){
        PyErr_SetString(PyExc_RuntimeError,
            "ss2gpix failed to return the screen image");
        return NULL;
    }
    dims[0] = width;
    dims[1] = height;
    dims[2] = 3;
    pyMatrix = (PyArrayObject *) PyArray_FromDims(3, dims, PyArray_UBYTE);
    strides[0] = (int) PyArray_STRIDE(pyMatrix, 0);
    strides[1] = (int) PyArray_STRIDE(pyMatrix, 1);
    strides[2] = (int) PyArray_STRIDE(pyMatrix, 2);
    matrixData = PyArray_DATA(pyMatrix);
    
    // copy the texture's data over to the numpy array
    for (i = 0; i < height; i++) {
        for(j = 0; j < width; j++){
            for(k = 0; k < 3; k++){
                index = (unsigned char*) (matrixData + i*strides[1] + j*strides[0] + k*strides[2]);
                *index = (char) result[k + j*3 + i*width*3];
            }
        }
    }
    // we're responsible to free the memory returned by s2plot
    free(result);
    
    pyMatrix->data = (char *) matrixData;
    Py_XINCREF(pyMatrix);

    return (PyObject *) pyMatrix;
}
