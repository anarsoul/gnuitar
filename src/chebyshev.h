#ifndef _BIQUAD_H_
	#include "biquad.h"
#endif

#ifndef _CHEBYSHEV_H_
#define _CHEBYSHEV_H_ 1

void CalcChebyshev2(double Fs,double Fc,double ripple
					,int lowpass
					, struct Biquad *filter);



#endif