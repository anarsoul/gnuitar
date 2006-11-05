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
 * Revision 1.41  2006/11/05 18:50:59  alankila
 * - add tone controls into tubeamp and change default speaker type
 *
 * Revision 1.40  2006/09/04 14:42:04  alankila
 * - fix hilbert-transform for > 1 channel: forgot x0_tmp
 *
 * Revision 1.39  2006/08/20 10:55:04  alankila
 * - improve whitespace usage / parameter naming consistency
 *
 * Revision 1.38  2006/08/20 10:25:12  alankila
 * - use aligned convolution in the fast scan -- this seems to give almost 2x
 *   performance boost on AMD64
 *
 * Revision 1.37  2006/08/15 15:45:00  alankila
 * - use native operators instead of functions for loop
 *
 * Revision 1.36  2006/08/10 12:54:58  alankila
 * - denormal number avoidance routine contained a double constant instead
 *   of float constant. Ouch. This removes a truckload of unnecessary
 *   datatype conversions.
 * - in addition, SSE gnuitar now emits manual code to turn denormal number
 *   avoidance flags on. This may be unnecessary due to -ffast-math, though.
 *
 * Revision 1.35  2006/08/10 12:24:24  alankila
 * - fix SSE3 algorithm after I managed to misunderstand haddps
 *
 * Revision 1.34  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.33  2006/08/07 21:43:29  alankila
 * - committing a hopefully working version of biquads on SSE now. Had to
 *   rename struct members for this to succeed, though. :-(
 *
 * Revision 1.32  2006/08/07 20:01:07  alankila
 * - gcc attribute aligned shot me in the foot.
 *   Removed that and attempts to use SSE-based biquad implementation.
 *
 * Revision 1.31  2006/08/06 20:57:46  alankila
 * - pepper with const declarations
 *
 * Revision 1.30  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.29  2006/07/27 19:38:14  alankila
 * - disable SSE convolve for mingw32 for now
 * - fix type of non-SSE convolve() routine
 *
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

#include "audio-driver.h"
#include "utils.h"

/* SSE is used to compute convolutions and the biquad summation. */
#ifdef __SSE__
#include <xmmintrin.h>
#endif

/* SSE3 is used to replace shuffle + stuff with the horizontal summation. */
#ifdef __SSE3__
#include <pmmintrin.h>
#endif

#if defined(__SSE__) && defined(FLOAT_DSP)

typedef struct {
    union {
        float b[4]; /* b1, b2, a1, a2 -- sorry about this, especially b0 vs. b[0] :-( */
        __m128 b4;
    };
    union {
        float mem[MAX_CHANNELS][4];
        __m128 mem4[MAX_CHANNELS];
    };
    float b0;
} Biquad_t;

#else

typedef struct {
    float b[4];
    float mem[MAX_CHANNELS][4];
    float b0;
} Biquad_t;

#endif

typedef struct {
    Biquad_t        a1[MAX_CHANNELS], a2[MAX_CHANNELS];
    DSP_SAMPLE      x0_tmp[MAX_CHANNELS];
} Hilbert_t;

/* Fs = sampling rate, Fc = center frequency, BW = bandwidth (octaves),
 * G = gain (dB), ripple = 0=butterw, 1-100=cheb (s-domain ellipticity %),
 * delay = unitless 0 .. 1, lowpass = flag whether cheb is lowpass filter */
void     set_peq_biquad(const double Fs, const double Fc, const double BW, const double G, Biquad_t *f);
void     set_bpf_biquad(const double Fs, const double Fc, const double BW, Biquad_t *f);
void     set_lpf_biquad(const double Fs, const double Fc, const double BW, Biquad_t *f);
void     set_phaser_biquad(const double delay, Biquad_t *f);
void     set_2nd_allpass_biquad(const double delay, Biquad_t *f);
void     set_rc_lowpass_biquad(const double Fs, const double Fc, Biquad_t *f);
void     set_rc_highpass_biquad(const double Fs, const double Fc, Biquad_t *f);
void     set_lsh_biquad(const double Fs, const double Fc, const double G, Biquad_t *f);
void     set_hsh_biquad(const double Fs, const double Fc, const double G, Biquad_t *f);
void     set_chebyshev1_biquad(double Fs, double Fc, double ripple,
			       int lowpass, Biquad_t *f);

void     hilbert_transform(const DSP_SAMPLE in, DSP_SAMPLE *x0, DSP_SAMPLE *x1, Hilbert_t *h, const int curr_channel);
void     hilbert_init(Hilbert_t *h);
void     fir_interpolate_2x(DSP_SAMPLE *mem, const DSP_SAMPLE in, DSP_SAMPLE *o1, DSP_SAMPLE *o2);
DSP_SAMPLE fir_decimate_2x(DSP_SAMPLE *mem, const DSP_SAMPLE in1, const DSP_SAMPLE in2);

#if defined(__SSE__) && defined(FLOAT_DSP)

static inline float
__attribute__ ((nonnull(2)))
do_biquad(const float x, Biquad_t *f, const int c)
{
    __m128          r;
    float          *mem = f->mem[c], y;

    /* struct is arranged so that b1 and a1 terms coincide
     * with the locations of the historic values in *mem.
     * Therefore the multiplication can be performed through SSE. */
    r = f->b4 * f->mem4[c];
    /* sum all the values together */
#ifdef __SSE3__
    /* horizontal add calculates
     * { a, b, c, d }, { e, f, g, h } -> { a + b, c + d, e + f, g + h } */
    r = _mm_hadd_ps(r, r);
    /* r now contains { a + b, c + d, a + b, c + d }.
     * Summing again computes a + b + c + d into all slots. */
    r = _mm_hadd_ps(r, r);
#else
    /* move high quadword of r to low quadword, then add
      * { a, b, c, d } -> { a + c, b + d, 2*c, 2*d } */
    r = _mm_add_ps(_mm_movehl_ps(r, r), r);
    /* move second doubleword to first doubleword, then add
     * { a + c, b + d, 2*c, 2*d } -> { a + b + c + d, b + d, 2*c, 2*d } */
    r = _mm_add_ss(_mm_shuffle_ps(r, r, 1), r);
#endif
    /* store result in y */
    _mm_store_ss(&y, r);
    /* add the final term */
    y += f->b0 * x;
    
    /* update history. This could also be done through _mm_shuffle_ps
     * if x and y were stored in some packed position. */
    mem[1] = mem[0];
    mem[0] = x;
    mem[3] = mem[2];
    mem[2] = y;

    return y;
}

/* important: a is aligned to 16-byte boundary but b is not.
 * Therefore, movups must be used to access that memory. */
static inline float
__attribute__ ((nonnull(1, 2)))
convolve(const float *a, const float *b, const int len)
{
    __m128 r = { 0, 0, 0, 0 };
    __m128 *a4 = (__m128 *) a;
    const float *b4 = b;
    float dot = 0.0;
    int i4 = len / 4;
    if (i4) {
        while (i4 --) {
            r += *a4++ * _mm_loadu_ps(b4);
            b4 += 4;
        }
#ifdef __SSE3__
        r = _mm_hadd_ps(r, r);
        r = _mm_hadd_ps(r, r);
#else
        r = _mm_add_ps(_mm_movehl_ps(r, r), r);
        r = _mm_add_ss(_mm_shuffle_ps(r, r, 1), r);
#endif
        _mm_store_ss(&dot, r);
    }
    
    switch (len % 4) {
        case 3: dot += a[len - 3] * b[len - 3];
        case 2: dot += a[len - 2] * b[len - 2];
        case 1: dot += a[len - 1] * b[len - 1];
    }
    
    return dot;
}

static inline float
__attribute__ ((nonnull(1, 2)))
convolve_aligned (const float *a, const float *b, const int len)
{
    __m128 r = { 0, 0, 0, 0 };
    __m128 *a4 = (__m128 *) a;
    __m128 *b4 = (__m128 *) b;
    float dot = 0.0;
    int i4 = len / 4;
    if (i4) {
        while (i4 --)
            r += *a4++ * *b4++;
#ifdef __SSE3__
        r = _mm_hadd_ps(r, r);
        r = _mm_hadd_ps(r, r);
#else
        r = _mm_add_ps(_mm_movehl_ps(r, r), r);
        r = _mm_add_ss(_mm_shuffle_ps(r, r, 1), r);
#endif
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

/* Denormals are small numbers that force FPU into slow mode.
 * Denormals tend to occur in all low-pass filters, but a DC
 * offset can remove them. SSE code is denormalless due to
 * FPU setting (which should also be enforced by -ffast-math).
 * I'm playing it safe if non-SSE codepaths are used. */
#define DENORMAL_BIAS   1E-5f

static inline float
__attribute__ ((nonnull(1, 2)))
convolve(const DSP_SAMPLE *a, const DSP_SAMPLE *b, const int len)
{
    int i;
    /* a long int type would be needed to hold the value in integer dsp */
    float dot = 0;
    for (i = 0; i < len; i += 1)
            dot += (float) a[i] * (float) b[i];
    return dot;
}

static inline float
__attribute__ ((nonnull(1, 2)))
convolve_aligned(const DSP_SAMPLE *a, const DSP_SAMPLE *b, const int len)
{
    return convolve(a, b, len);
}

static inline float
__attribute__ ((nonnull(2)))
do_biquad(const float x, Biquad_t *f, const int c)
{
    float *mem = f->mem[c], y;
    y = x * f->b0 + mem[0] * f->b[0] + mem[1] * f->b[1]
        + mem[2] * f->b[2] + mem[3] * f->b[3] + DENORMAL_BIAS;
    if (isnan(y))
	y=0;
    mem[1] = mem[0];
    mem[0] = x;
    mem[3] = mem[2];
    mem[2] = y;
    return y;
}

#endif

#endif
