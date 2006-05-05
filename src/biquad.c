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
 * Revision 1.16  2006/05/05 18:34:32  alankila
 * - handle denormals to avoid slowdowns for digital silence type situations.
 *
 * Revision 1.15  2005/11/01 12:32:16  alankila
 * - fix highpass
 *
 * Revision 1.14  2005/10/30 11:21:05  alankila
 * - more correct and precise output filtering!
 * - real device seems to have some kind of highpass filtering around 50 Hz
 *   maybe or so, because there's too much bass...
 *
 * Revision 1.13  2005/10/07 12:50:12  alankila
 * - move delay shape computation to where it belongs and change it to bit
 *   smoother
 *
 * Revision 1.12  2005/09/12 22:01:56  alankila
 * - swap a0/b0 around to better coincide with available literature
 * - optimize x1 and x2 terms off chebyshev as they are defined as:
 *       x2 = x0
 *       x1 = 2 * x0
 *   and only used once.
 * - introduce a0 into the chebyshev to resemble each other more
 *
 * Revision 1.11  2005/09/12 08:26:51  alankila
 * - flip the signs of b1 and b2 (but not b0) because the mathematical
 *   difference equation is usually written that way.
 *
 * Revision 1.10  2005/09/10 10:53:38  alankila
 * - remove the need to reserve biquad's mem in caller's side
 *
 * Revision 1.9  2005/09/09 20:22:17  alankila
 * - phasor reimplemented according to a popular algorithm that simulates
 *   high-impedance isolated varying capacitors
 *
 * Revision 1.8  2005/08/14 23:31:22  alankila
 * revert earlier "fix" that does * sizeof(double). It was a brainfart.
 *
 * Revision 1.7  2005/08/10 11:01:39  alankila
 * - remove separate chebyshev.c, move the code into biquad.c
 * - fix the copy in .h to agree with DoBiquad's implementation
 * - rename functions:
 *   * DoBiquad       -> do_biquad
 *   * SetEqBiquad    -> set_peq_biquad
 *   * CalcChebyshev2 -> set_chebyshev2_biquad
 * - this change is followed by fixups in effects distort2 & eqbank
 *
 * Revision 1.6  2005/08/07 12:42:05  alankila
 * Do not use << 2 because double can be wider than 4.
 * Better say what you mean.
 *
 * Revision 1.5  2005/07/31 10:22:54  fonin
 * Check for NaN values on input and output
 *
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

/* peaking band equalizer */
void
set_peq_biquad(double Fs, double Fc, double BW, double G, Biquad_t *f)
{
    double          k, om, alpha, a0;
    
    k = pow(10, G / 40);	/* relative gain */
    BW = BW / (Fc - BW / 2);	/* bandwidth in octaves */
    om = 2 * M_PI * Fc / Fs;	/* normalized frequency in radians */
    alpha = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);
    
    a0 = 1 + alpha / k;
    f->b0 = (1 + alpha * k) / a0;
    f->b1 = -2 * cos(om)    / a0;
    f->b2 = (1 - alpha * k) / a0;
    f->a1 = f->b1;
    f->a2 = (1 - alpha / k) / a0;
}

/* band pass filter */
void
set_bpf_biquad(double Fs, double Fc, double BW, Biquad_t *f)
{
    double om, alpha, a0;
    
    om = 2 * M_PI * Fc / Fs;
    alpha = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);
    
    a0 = 1 + alpha;
    f->b0 = alpha        / a0;
    f->b1 = 0;
    f->b2 = -f->b0;
    f->a1 = -2 * cos(om) / a0;
    f->a2 = (1 - alpha)  / a0;
}

/* allpass filter, really 1st order as a2 = b2 = 0,
 * delay can vary from 0 to 1.
 */
void
set_allpass_biquad(double delay, Biquad_t *f)
{
    f->b0 = delay;
    f->b1 = 1.0;
    f->a1 = delay;
    f->a2 = f->b2 = 0;
}

void
set_rc_lowpass_biquad(double sample_rate, double freq, Biquad_t *f)
{
    double rc = 1 / (2 * M_PI * freq);
    double ts = 1.0 / sample_rate;

    f->b0 = ts / (ts + rc);
    f->a1 = -rc / (ts + rc);
    f->b1 = f->b2 = f->a2 = 0;
}

void
set_rc_highpass_biquad(double sample_rate, double freq, Biquad_t *f)
{
    double rc = 1 / (2 * M_PI * freq);
    double ts = 1.0 / sample_rate;

    f->b0 = 1;
    f->b1 = -1;
    f->a1 = -rc / (ts + rc);
    f->b2 = f->a2 = 0;
}

void
set_rc_highboost_biquad(double sample_rate, double freq, Biquad_t *f)
{
    double rc = 1 / (2 * M_PI * freq);
    double ts = 1.0 / sample_rate;

    f->a1 = rc / (ts + rc);
    f->b0 = 1 + f->a1;
    f->b1 = f->b2 = f->a2 = 0;
}

void
set_chebyshev1_biquad(double Fs, double Fc, double ripple, int lowpass, Biquad_t *f)
{
    double          x, y, z, c, v, t, r, om, m, x0, y1p, y2, k, d, tt, tt2, a0;
    
    om = 2 * M_PI * Fc / Fs;
    
    c = -cos(M_PI / 4);
    v =  sin(M_PI / 4);
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
    m = c * c + v * v;
    d = 4 - 4 * c * tt + m * tt2;
    x0 = tt2 / d;
    y1p = (8 - 2 * m * tt2) / d;
    y2 = (-4 - 4 * c * tt - m * tt2) / d;
    if (lowpass)
        k = sin(0.5 - om / 2) / sin(0.5 + om / 2);
    else
        k = -cos(om / 2 + 0.5) / cos(om / 2 - 0.5);
    
    a0 = 1 + k * (y1p - y2 * k);
    f->b0 = (x0 - k * (2 - k) * x0)             / a0;
    f->b1 = 2 * f->b0;
    f->b2 =     f->b0;
    f->a1 = -(k * (2 + y1p * k - 2 * y2) + y1p) / a0;
    f->a2 = -(-k * (k + y1p) + y2)              / a0;
    if (!lowpass) {
        f->b1 = -f->b1;
        f->a1 = -f->a1;
    }
}

#if !defined(_MSC_VER)
#if defined(__GNUC__)
/* check if the compiler is not Visual C so we must declare the fuction here */
__inline double
do_biquad(double x, Biquad_t *f, int c)
#else
double
do_biquad(double x, Biquad_t *f, int c)
#endif
{
    double          y;
    if(isnan(x))
	x=0;
    y = x * f->b0 + f->mem[c][0] * f->b1 + f->mem[c][1] * f->b2
        - f->mem[c][2] * f->a1 - f->mem[c][3] * f->a2 + DENORMAL_BIAS;
    if(isnan(y))
	y=0;
    f->mem[c][1] = f->mem[c][0];
    f->mem[c][0] = x;
    f->mem[c][3] = f->mem[c][2];
    f->mem[c][2] = y;
    return y;
}
#endif
