/*
 * Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
 *
 * $Log$
 * Revision 1.3  2004/10/21 11:13:45  dexterus
 * Fixed calculus error
 * Added support for any numer of channels
 * iniline support delimited to Visual C
 *
 * Revision 1.2  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 * $Id$
 */


#ifndef _BIQUAD_H_
#define _BIQUAD_H_ 1


struct Biquad {
    double          a0,
                    a1,
                    a2,
                    b1,
                    b2;		/* coefficients */
    double          *mem;	/* memory for the filter , must be alocated by the caller, 4 * number of channels*/
};

/* Sampling rate, Center frequency, Bandwidth, Gain (decibels) */
extern void     SetEqBiquad(double Fs, double Fc, double BW, double G,
			    struct Biquad *f);

/* computes one sample ; because inline declared, this will be
 * compiled in place where called
 * works with MS Visual C . Other compilers ? */

#ifdef _MSC_VER															/* check if the compiler is Visual C */
	__inline double doBiquad(double x, struct Biquad *f, int channel)	/* so we can use inline function in C, declared here */
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
#else																	/* otherwise */
	extern double doBiquad(double x, struct Biquad *f, int channel);	/* declare a standar function */
#endif 


#endif
