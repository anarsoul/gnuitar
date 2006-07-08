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
 * Revision 1.28  2006/07/08 18:11:33  alankila
 * - reduce overdrive effect cpu drain by implementing low-pass filtering
 *   in resampler and reusing the static 720 Hz lowpass filter as decimating
 *   filter. Should be 10-20 % faster.
 *
 * Revision 1.27  2006/07/08 16:28:16  alankila
 * - extend hilbert transform with channel information for effects that could
 *   be used on channels separately. We've already allocated space in biquads
 *   for them.
 *
 * Revision 1.26  2006/06/16 14:44:14  alankila
 * - use full precision for allpass constants
 * - remove SSE version of biquad code, it was buggy.
 *
 * Revision 1.25  2006/05/29 23:46:02  alankila
 * - move _GNU_SOURCE into Makefile
 * - align memory for x86-32; x86-64 already aligned memory for us in glibc
 *   so we didn't crash. This is done through new gnuitar_memalign().
 * - cater to further restrictions in SSE instructions for x86 arhictecture:
 *   it appears that mulps memory must be aligned to 16 too. This crashed
 *   all biquad-using functions and tubeamp. :-(
 *
 * Revision 1.24  2006/05/26 13:45:54  alankila
 * - check that we can use movaps and use it for SSE.
 *
 * Revision 1.23  2006/05/25 16:54:12  alankila
 * - use 12 dB/oct lowpass filter between stages to help with our treble
 *   problem. Sounds less aliased now.
 *
 * Revision 1.22  2006/05/25 10:30:41  alankila
 * - use SSE for biquad computations.
 *
 * Revision 1.21  2006/05/25 09:10:54  alankila
 * - move biquad arithmetic to floats to obtain small performance gain
 *
 * Revision 1.20  2006/05/25 09:03:05  alankila
 * - replace the SSE code with even faster version. Tubeamp effect now runs
 *   20 % faster on my computer. Add some alignment directives to make future
 *   use of SSE potentially easier.
 *
 * Revision 1.19  2006/05/24 20:17:05  alankila
 * - make inlining actually possible / working
 *
 * Revision 1.18  2006/05/20 14:28:04  alankila
 * - restore mono-phaser back to earlier design
 * - fix hilbert transform's allpass delay
 * - need to figure out what is the proper name for the phasor allpass
 *
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
#include "utils.h"

/* empty are used to align the structure to 16 byte boundary */
#ifdef __SSE__
typedef struct {
    float       b1, b2, a1, a2;
    float       mem[MAX_CHANNELS][4];
    float       b0, empty1, empty2, empty3;
} Biquad_t __attribute__((aligned(16)));
#else
typedef struct {
    float       b0, b1, b2, a1, a2;
    float       mem[MAX_CHANNELS][4];
} Biquad_t;
#endif

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
extern void     set_lpf_biquad(double Fs, double Fc, double BW,
			       Biquad_t *f);
extern void     set_phaser_biquad(double delay, Biquad_t *f);
extern void     set_2nd_allpass_biquad(double delay, Biquad_t *f);
extern void     set_rc_lowpass_biquad(double fs, double fc, Biquad_t *f);
extern void     set_rc_highpass_biquad(double fs, double fc, Biquad_t *f);
extern void     set_lsh_biquad(double Fs, double Fc, double G, Biquad_t *f);

extern void     hilbert_transform(DSP_SAMPLE in, DSP_SAMPLE *x0, DSP_SAMPLE *x1, Hilbert_t *h, int curr_channel);
extern void     hilbert_init(Hilbert_t *h);
extern void     fir_interpolate_2x(DSP_SAMPLE *mem, DSP_SAMPLE in, DSP_SAMPLE *o1, DSP_SAMPLE *o2);
extern DSP_SAMPLE fir_decimate_2x(DSP_SAMPLE *mem, DSP_SAMPLE in1, DSP_SAMPLE in2);

/*
 * Sampling rate, Center frequency, Ripple %, Lowpass?
 */
extern void     set_chebyshev1_biquad(double Fs, double Fc, double ripple,
			              int lowpass, Biquad_t *f);

/*
 * computes one sample ; because inline declared, this will be compiled in 
 * place where called works with MS Visual C . Other compilers ? 
 */

/* check if the compiler is Visual C or GCC so we can use inline function in C,
 * declared here */
#if defined(__SSE__) && defined(FLOAT_DSP)
#include <xmmintrin.h>

/*
static inline float
do_biquad(float x, Biquad_t *f, int c)
{
    __m128          r;
    float           y;
    float           *mem = f->mem[c];
    float           *b1 = &f->b1;

    // use SSE to calculate 4 of the biquad terms
    asm("movaps     (%%edx), %%xmm0             \n"
"        mulps      (%%ecx), %%xmm0             \n"
"        movaps     %%xmm0, %[r]                \n"
        :
        : "d"(b1), "c"(mem), [r]"m"(r)
        : "cc", "xmm0");
    
    r = _mm_add_ps(_mm_movehl_ps(r, r), r);
    r = _mm_add_ss(_mm_shuffle_ps(r, r, 1), r);
    _mm_store_ss(&y, r);

    // add the last term
    y += f->b0 * x;
    
    mem[1] = mem[0];
    mem[0] = x;
    mem[3] = mem[2];
    mem[2] = y;

    return y;
}
*/

static inline float
convolve(const float *a, const float *b, int len) {
    __m128 r;
    float dot = 0.0;
    int i;
    
    i = (len / 4) * 2;
    if (i) {
        /* The assembly code does the convolution as fast as possible. Modelled after
         * the algorithm in Mmmath library by Ville Tuulos, GPL license. */

        asm("   xorps  %%xmm0, %%xmm0               \n"
            ".Lloop%=:                              \n"
            "   decl   %[i]                         \n"
            "   decl   %[i]                         \n"
            "   movups (%%ecx, %[i], 8), %%xmm2     \n"
            "   mulps  (%%edx, %[i], 8), %%xmm2     \n"
            "   addps  %%xmm2, %%xmm0               \n"
            "   cmpl   $0, %[i]                     \n"
            "   jnz    .Lloop%=                     \n"
            "   movaps %%xmm0, %[r]                 \n"
            : 
            : "d"(a), "c"(b), [r]"m"(r), [i]"a"(i)
            : "cc", "xmm0", "xmm2");

        r = _mm_add_ps(_mm_movehl_ps(r, r), r);
        r = _mm_add_ss(_mm_shuffle_ps(r, r, 1), r);
        _mm_store_ss(&dot, r);
    }
    
    switch (len % 4) {
        case 3: dot += a[len - 3] * b[len - 3];
        case 2: dot += a[len - 2] * b[len - 2];
        case 1: dot += a[len - 1] * b[len - 1];
    }
    
    return dot;
}
#else

static inline DSP_SAMPLE
convolve(const DSP_SAMPLE *a, const DSP_SAMPLE *b, int len) {
    int i, dot = 0;
    for (i = 0; i < len; i += 1)
            dot += a[i] * b[i];
    return dot;
}

#endif


/* Denormals are small numbers that force FPU into slow mode.
 * Denormals tend to occur in all low-pass filters, but a DC offset can remove them. */
#define DENORMAL_BIAS   1E-5

__inline float static
do_biquad(float x, Biquad_t *f, int c)
{				
				 
    float          y;
    if(isnan(x))
	x=0;
    y = x * f->b0 + f->mem[c][0] * f->b1 + f->mem[c][1] * f->b2
        + f->mem[c][2] * f->a1 + f->mem[c][3] * f->a2 + DENORMAL_BIAS;
    if(isnan(y))
	y=0;
    f->mem[c][1] = f->mem[c][0];
    f->mem[c][0] = x;
    f->mem[c][3] = f->mem[c][2];
    f->mem[c][2] = y;
    return y;
}

#endif
