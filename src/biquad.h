// Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
 
#include "pump.h"
    struct Biquad {
    double         a0,
                    a1,
                    a2,
                    b1,
                    b2;		// coefficients
    double          m0,
                    m1,
                    m2,
                    m3;		// memory
    double          m0c,
                    m1c,
                    m2c,
                    m3c;	// memory for second channel
};

    // Sampling rate, Center frequency, Bandwidth, Gain (decibels)
extern void     SetEqBiquad(double Fs, double Fc, double BW, double G,
			    struct Biquad *f);

    // computes one sample ; because inline declared, this will be
    // compiled in place where called
    // works with MS Visual C . Other compilers ?
extern double   doBiquad(double x, struct Biquad *f) ;

    // does one sample for the second channel if stereo 
extern double   doBiquadC(double x, struct Biquad *f) ;
