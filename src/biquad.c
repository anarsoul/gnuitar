/*
 * Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
 *
 * $Id$
 * $Log$
 * Revision 1.2  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 *
 */

#include "biquad.h"
#include <math.h>

#define PI 3.14159265358979323846E0

/* Sampling rate, Center frequency, Bandwidth, Gain (decibels)
 * Highest frequency possible at 44100 Hz is around 5000, due to an
 * overflow error
 * Will fix that */
void
SetEqBiquad(double Fs, double Fc, double BW, double G, struct Biquad *f)
{
    double          k,
                    om,
                    x;
    double          fi;
    k = pow(10, G / 20);	// relative gain
    om = 2 * PI * Fc / Fs;	// normalized frequency in radians
    fi = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);	// stuff
    x = 1 + fi / k;		// b0
    f->a0 = (1 + fi * k) / x;
    f->a1 = -2 * cos(om) / x;
    f->a2 = (1 - fi * k) / x;
    f->b1 = -f->a1;
    f->b2 = -(1 - fi / k) / x;
}

inline double
doBiquad(double x, struct Biquad *f)
{
    double          y;
    y = x * f->a0 + f->m0 * f->a1 + f->m1 * f->a2 + f->m2 * f->b1 +
	f->m3 * f->b2;
    f->m1 = f->m0;
    f->m0 = x;
    f->m3 = f->m2;
    f->m2 = y;
    return y;
}

inline double
doBiquadC(double x, struct Biquad *f)
{
    double          y;
    y = x * f->a0 + f->m0c * f->a1 + f->m1c * f->a2 + f->m2c * f->b1 +
	f->m3c * f->b2;
    f->m1c = f->m0c;
    f->m0c = x;
    f->m3c = f->m2c;
    f->m2c = y;
    return y;
}
