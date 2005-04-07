/*
 * GNUitar
 * Chebyshev filter
 * Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 *
 * $Log$
 * Revision 1.3  2005/04/07 18:36:35  fonin
 * Fix with PI constant
 *
 * Revision 1.2  2005/04/06 19:34:20  fonin
 * Code lickup
 *
 */
#include <math.h>
#include "chebyshev.h"
#include "utils.h"

/* Check if the compiler is Visual C or GCC */
#if defined (_MSC_VER) || defined(__GNUC__)
#   pragma intrinsic (sin,cos,sqrt,tan)
#endif

// const double tt=0.017453735581517578669072396122404;
// const double tt2=3.0463288574953277002597889659763e-4;



void
CalcChebyshev2(double Fs, double Fc, double ripple, int lowpass,
	       struct Biquad *filter)
{
    double          x,
                    y,
                    z,
                    c,
                    v,
                    t,
                    r,
                    om,
                    m,
                    x0,
                    x1,
                    x2,
                    y1p,
                    y2,
                    k,
                    d,
                    tt,
                    tt2;
    // x=-cos(M_PI/4+M_PI/2);
    // y=sin(M_PI/4+M_PI/2);
    // c=-0.99915455413031497832540334286332;
    // v=0.041111761828599317357934264608497;
    c = -cos(M_PI / 4);
    v = sin(M_PI / 4);
    if (ripple > 0) {
	t = 100.0 / (100.0 - ripple);
	x = sqrt(t * t - 1);
	t = 1 / x;
	r = t + sqrt(t / x);
	y = 0.5 * log(r + 1);
	z = 0.5 * log(r - 1);
	t = exp(z);
	z = (t + 1 / t) / 2;
	t = exp(y);
	c *= (t - 1 / t) / 2 / z;
	v *= (t + 1 / t) / 2 / z;
    }
    tt = 2 * tan(0.5);
    tt2 = tt * tt;
    om = 2 * M_PI * Fc / Fs;
    m = c * c + v * v;
    d = 4 - 4 * c * tt + m * tt2;
    x0 = tt2 / d;
    x1 = x0 * 2;
    x2 = x0;
    y1p = (8 - 2 * m * tt2) / d;
    y2 = (-4 - 4 * c * tt - m * tt2) / d;
    if (lowpass)
	k = sin(0.5 - om / 2) / sin(0.5 + om / 2);
    else
	k = -cos(om / 2 + 0.5) / cos(om / 2 - 0.5);
    d = 1 + k * (y1p - y2 * k);
    filter->a0 = (x0 - k * (x1 - x2 * k)) / d;
    // coeff[1]=(k*(-2*(x0+x2)+x1*k)+x1)/d;
    filter->a1 = 2 * filter->a0;
    filter->a2 = filter->a0;
    // coeff[2]=(k*(x0*k-x1)+x2)/d;
    filter->b1 = (k * (2 + y1p * k - 2 * y2) + y1p) / d;
    filter->b2 = (-k * (k + y1p) + y2) / d;
    if (!lowpass) {
	filter->a1 = -filter->a1;
	filter->b1 = -filter->b1;
	// t=(1-coeff[0]+coeff[1]-coeff[2])/(coeff[3]-coeff[4]);
    }
    // else
    // t=(1-coeff[0]-coeff[1]-coeff[2])/(coeff[3]+coeff[4]);

    // for (i=0;i<5;i++)
    // coeff[i]*=t;

}
