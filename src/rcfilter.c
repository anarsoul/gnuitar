/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/01/11 13:22:04  fonin
 * Initial revision
 *
 */

#include "rcfilter.h"
#include "pump.h"
#include <math.h>

void
LC_filter(int *sound, int count, int filter_no, double freq,
	  struct filter_data *pp)
{
    double          R,
                    L,
                    C,
                    dt_div_L,
                    dt_div_C;
    double          du,
                    d2i;
    int             t;

    L = 50e-3;			/*
				 * like original crybaby wahwah, hehehe 
				 */
    C = 1.0 / (4.0 * pow(M_PI * freq, 2.0) * L);
    R = 300.0;

    dt_div_C = 1.0 / (C * SAMPLE_RATE);
    dt_div_L = 1.0 / (L * SAMPLE_RATE);

    for (t = 0; t < count; t++) {
	du = (double) *sound - pp->last_sample[filter_no][0];
	pp->last_sample[filter_no][0] = (double) *sound;

	d2i =
	    dt_div_L * (du - pp->i[filter_no][0] * dt_div_C -
			R * pp->di[filter_no][0]);
	pp->di[filter_no][0] += d2i;
	pp->i[filter_no][0] += pp->di[filter_no][0];

	/*
	 * *sound=(int)(pp->i[filter_no][0]*pp->amplify); 
	 */
	*sound = (int) (pp->i[filter_no][0] * 500.0);

	sound++;
    }
}

double
other(double f, double x)
{
    return 1.0 / (2 * M_PI * f * x);
}

void
RC_setup(int max, double amplify, struct filter_data *pp)
{
    int             c,
                    d;

    pp->max = max;
    pp->amplify = amplify;

    for (c = 0; c < max; c++)
	for (d = 0; d < 2; d++)
	    pp->i[c][d] = pp->di[c][d] = pp->last_sample[c][d] = 0;
}

void
RC_set_freq(double f, struct filter_data *pp)
{
    pp->R = 1000.0;
    pp->C = other(f, pp->R);
    pp->invR = 1.0 / pp->R;
    pp->dt_div_C = (1.0 / SAMPLE_RATE) / pp->C;
}

void
RC_filter(int *sound, int count, int mode, int filter_no,
	  struct filter_data *pp)
{
    double          du,
                    di;
    int             t;

    for (t = 0; t < count; t++) {
	du = (double) *sound - pp->last_sample[filter_no][mode];
	pp->last_sample[filter_no][mode] = (double) *sound;

	di = pp->invR * (du - pp->i[filter_no][mode] * pp->dt_div_C);
	pp->i[filter_no][mode] += di;
	if (mode == HIGHPASS)
	    *sound =
		(int) ((pp->i[filter_no][mode] * pp->R) * pp->amplify);
	else
	    *sound =
		(int) (((double) *sound - pp->i[filter_no][mode] * pp->R) *
		       pp->amplify);

	sound++;
    }
}

void
RC_bandpass(int *sound, int count, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++) {
	RC_filter(sound, count, HIGHPASS, a, pp);
	RC_filter(sound, count, LOWPASS, a, pp);
    }
}

void
RC_highpass(int *sound, int count, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(sound, count, HIGHPASS, a, pp);
}

void
RC_lowpass(int *sound, int count, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(sound, count, LOWPASS, a, pp);
}
