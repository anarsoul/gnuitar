/*
 * Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
 *
 * $Id$
 * $Log$
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
    k = pow(10, G / 40);		// relative gain
	BW = BW / ( Fc - BW /2 );	// bandwidth in octaves 
    om = 2 * PI * Fc / Fs;		// normalized frequency in radians
    fi = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);	// stuff
    x = 1 + fi / k;		// b0
    f->a0 = (1 + fi * k) / x;
    f->a1 = -2 * cos(om) / x;
    f->a2 = (1 - fi * k) / x;
    f->b1 = -f->a1;
    f->b2 = -(1 - fi / k) / x;
}

#ifndef _MSC_VER															/* check if the compiler is not  Visual C */
	double doBiquad(double x, struct Biquad *f, int channel)	/* so we must declare the fuction here  */
	{
		double          y, *mem;
		mem = f->mem + (channel << 2);
		y = x * f->a0 + mem[0] * f->a1 + mem[1] * f->a2 + mem[2] * f->b1 + mem[3] * f->b2;
		mem[1] = mem[0];
		mem[0] = x;
		mem[3] = mem[2];
		mem[2] = y;
		return y;
	}
#endif

