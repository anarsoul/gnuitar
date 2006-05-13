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
 * $Log$
 * Revision 1.17  2006/05/13 17:10:06  alankila
 * - move hilbert transform into biquad.c
 * - implement stereo phaser using hilbert transform
 * - clean up remaining struct biquad references and replace them with typedef
 *
 * Revision 1.16  2006/05/13 09:33:16  alankila
 * - more power to phaser, less cpu use, good deal
 *
 * Revision 1.15  2006/05/07 13:22:12  alankila
 * - new bare bones distortion effect: tubeamp
 *
 * Revision 1.14  2006/05/05 18:34:32  alankila
 * - handle denormals to avoid slowdowns for digital silence type situations.
 *
 * Revision 1.13  2005/10/30 11:21:05  alankila
 * - more correct and precise output filtering!
 * - real device seems to have some kind of highpass filtering around 50 Hz
 *   maybe or so, because there's too much bass...
 *
 * Revision 1.12  2005/09/12 22:01:56  alankila
 * - swap a0/b0 around to better coincide with available literature
 * - optimize x1 and x2 terms off chebyshev as they are defined as:
 *       x2 = x0
 *       x1 = 2 * x0
 *   and only used once.
 * - introduce a0 into the chebyshev to resemble each other more
 *
 * Revision 1.11  2005/09/12 09:42:25  fonin
 * - MSVC compatibility fixes
 *
 * Revision 1.10  2005/09/12 08:26:51  alankila
 * - flip the signs of b1 and b2 (but not b0) because the mathematical
 *   difference equation is usually written that way.
 *
 * Revision 1.9  2005/09/10 10:53:38  alankila
 * - remove the need to reserve biquad's mem in caller's side
 *
 * Revision 1.8  2005/09/09 20:22:17  alankila
 * - phasor reimplemented according to a popular algorithm that simulates
 *   high-impedance isolated varying capacitors
 *
 * Revision 1.7  2005/08/14 23:31:22  alankila
 * revert earlier "fix" that does * sizeof(double). It was a brainfart.
 *
 * Revision 1.6  2005/08/10 11:01:39  alankila
 * - remove separate chebyshev.c, move the code into biquad.c
 * - fix the copy in .h to agree with DoBiquad's implementation
 * - rename functions:
 *   * DoBiquad       -> do_biquad
 *   * SetEqBiquad    -> set_peq_biquad
 *   * CalcChebyshev2 -> set_chebyshev2_biquad
 * - this change is followed by fixups in effects distort2 & eqbank
 *
 * Revision 1.5  2005/08/07 12:42:04  alankila
 * Do not use << 2 because double can be wider than 4.
 * Better say what you mean.
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
 * $Id$
 */


#ifndef _BIQUAD_H_
#define _BIQUAD_H_ 1

#include "pump.h"

/* Denormals are small numbers that force FPU into slow mode.
 * Denormals tend to occur in all low-pass filters, but a DC offset can remove them. */
#define DENORMAL_BIAS   1E-5

typedef struct {
    double          b0, b1, b2, a1, a2;
    double          mem[MAX_CHANNELS][4];
} Biquad_t;

typedef struct {
    Biquad_t        a1[4], a2[4];
    DSP_SAMPLE      x0_tmp;
} Hilbert_t;

/*
 * Sampling rate, Center frequency, Bandwidth, Gain (decibels) 
 */
extern void     set_peq_biquad(double Fs, double Fc, double BW, double G,
			       Biquad_t *f);
extern void     set_bpf_biquad(double Fs, double Fc, double BW,
			       Biquad_t *f);
extern void     set_allpass_biquad(double delay, Biquad_t *f);
extern void     set_2nd_allpass_biquad(double delay, Biquad_t *f);
extern void     set_rc_lowpass_biquad(double fs, double fc, Biquad_t *f);
extern void     set_rc_highpass_biquad(double fs, double fc, Biquad_t *f);
extern void     set_rc_highboost_biquad(double fs, double fc, Biquad_t *f);
extern void     set_lsh_biquad(double Fs, double Fc, double G, Biquad_t *f);

extern void     hilbert_transform(DSP_SAMPLE in, DSP_SAMPLE *x0, DSP_SAMPLE *x1, Hilbert_t *h);
extern void     hilbert_init(Hilbert_t *h);

/*
 * Sampling rate, Center frequency, Ripple %, Lowpass?
 */
extern void     set_chebyshev1_biquad(double Fs, double Fc, double ripple,
			              int lowpass, Biquad_t *f);

/*
 * computes one sample ; because inline declared, this will be compiled in 
 * place where called works with MS Visual C . Other compilers ? 
 */

#if defined(_MSC_VER)
#include "utils.h"      /* for isnan() */
/* check if the compiler is Visual C or GCC so we can use inline function in C,
 * declared here */
__inline double
do_biquad(double x, Biquad_t *f, int c)
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
#elif defined(__GNUC__)
extern __inline double do_biquad(double x, Biquad_t *f, int c);
#else
/* otherwise declare a standard function */
extern double do_biquad(double x, Biquad_t *f, int c);
#endif


#endif
