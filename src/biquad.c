/*
 * GNUitar
 * Biquad
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
 * $Log$
 * Revision 1.4  2005/04/06 19:34:20  fonin
 * Code lickup
 *
 * Revision 1.3  2004/10/21 11:13:45  dexterus
 * Fixed calculus error
 * Added support for any numer of channels
 * iniline support delimited to Visual C
 *
 * Revision 1.2  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 *
 */

#include "biquad.h"
#include <math.h>
#ifdef _WIN32
#   include "utils.h"
#endif

/*
 * Sampling rate, Center frequency, Bandwidth, Gain (decibels) Highest
 * frequency possible at 44100 Hz is around 5000, due to an overflow error
 * Will fix that 
 */
void
SetEqBiquad(double Fs, double Fc, double BW, double G, struct Biquad *f)
{
    double          k,
                    om,
                    x;
    double          fi;
    k = pow(10, G / 40);	/* relative gain */
    BW = BW / (Fc - BW / 2);	/* bandwidth in octaves */
    om = 2 * M_PI * Fc / Fs;	/* normalized frequency in radians */
    fi = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);	/* stuff */
    x = 1 + fi / k;		/* b0 */
    f->a0 = (1 + fi * k) / x;
    f->a1 = -2 * cos(om) / x;
    f->a2 = (1 - fi * k) / x;
    f->b1 = -f->a1;
    f->b2 = -(1 - fi / k) / x;
}

#if !defined(MSC_VER)
#if defined(__GNUC__)
/* check if the compiler is not Visual C so we must declare the fuction here */
__inline double
doBiquad(double x, struct Biquad *f, int channel)
#else
double
doBiquad(double x, struct Biquad *f, int channel)
#endif
{
    double          y,
                   *mem;
    mem = f->mem + (channel << 2);
    y = x * f->a0 + mem[0] * f->a1 + mem[1] * f->a2 + mem[2] * f->b1 +
	mem[3] * f->b2;
    mem[1] = mem[0];
    mem[0] = x;
    mem[3] = mem[2];
    mem[2] = y;
    return y;
}
#endif
