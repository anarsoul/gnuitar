/*
 * GNUitar
 * RC-filter emulation
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
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
 *
 * $Log$
 * Revision 1.6  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.5  2003/03/09 21:02:18  fonin
 * Internal redesign for new "change sampling params" feature.
 *
 * Revision 1.4  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.3  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.2  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:04  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "rcfilter.h"
#include "pump.h"
#include <math.h>
#include <string.h>
#ifdef _WIN32
#    define M_PI 3.14159265358979323846E0
#endif

void
LC_filter(DSP_SAMPLE *sound, int count, int filter_no, double freq,
	  struct filter_data *pp)
{
    double          R,
                    L,
                    C,
                    dt_div_L,
                    dt_div_C;
    double          du,
                    d2i;
    int             t,
                    currchannel = 0;

    freq *= nchannels;
    L = 50e-3;			/* 
				 * like original crybaby wahwah, hehehe 
				 */
    C = 1.0 / (4.0 * pow(M_PI * freq, 2.0) * L);
    R = 300.0;

    dt_div_C = 1.0 / (C * sample_rate * nchannels);
    dt_div_L = 1.0 / (L * sample_rate * nchannels);

    for (t = 0; t < count; t++) {
	du = (double) *sound - pp->last_sample[filter_no][0][currchannel];
	pp->last_sample[filter_no][0][currchannel] = (double) *sound;

	d2i =
	    dt_div_L * (du - pp->i[filter_no][0][currchannel] * dt_div_C -
			R * pp->di[filter_no][0][currchannel]);
	pp->di[filter_no][0][currchannel] += d2i;
	pp->i[filter_no][0][currchannel] +=
	    pp->di[filter_no][0][currchannel];

/*
	*sound=(int)(pp->i[filter_no][0][currchannel][currchannel]*pp->amplify); 
*/
	*sound = (int) (pp->i[filter_no][0][currchannel] * 500.0);
	if (nchannels > 1)
	    currchannel = !currchannel;

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
    pp->max = max;
    pp->amplify = amplify;

    memset(pp->i, 0, sizeof(pp->i));
    memset(pp->di, 0, sizeof(pp->di));
    memset(pp->last_sample, 0, sizeof(pp->last_sample));
}

void
RC_set_freq(double f, struct filter_data *pp)
{
    pp->R = 1000.0;
    pp->C = other(f * nchannels, pp->R);
if(isnan(pp->C))
    printf("isnan(pp->C)\n");
    pp->invR = 1.0 / pp->R;
if(isnan(pp->invR))
    printf("isnan(pp->invR)\n");
    pp->dt_div_C = (1.0 / (sample_rate * nchannels)) / pp->C;
if(isnan(pp->dt_div_C))
    printf("isnan(pp->dt_div_C)\n");
}

void
RC_filter(DSP_SAMPLE *sound, int count, int mode, int filter_no,
	  struct filter_data *pp)
{
    double          du,
                    di;
    int             t,
                    currchannel = 0;

    for (t = 0; t < count; t++) {
	du = (double) *sound -
	    pp->last_sample[filter_no][mode][currchannel];
	pp->last_sample[filter_no][mode][currchannel] = (double) *sound;

if(isnan(du))
    printf("isnan(du)!\n");
	di = pp->invR * (du -
			 pp->i[filter_no][mode][currchannel] *
			 pp->dt_div_C);
if(isnan(di))
    printf("isnan(di)!\n");
	pp->i[filter_no][mode][currchannel] += di;
if(isnan(pp->i[filter_no][mode][currchannel]))
    printf("isnan(pp->i[filter_no][mode][currchannel])!\n");

if(isnan(*sound))
    printf("isnan(sound)\n");
	if (mode == HIGHPASS)
	    *sound =
		((pp->i[filter_no][mode][currchannel] * pp->R) *
		       pp->amplify);
	else
	    *sound =
		(((double) *sound -
			pp->i[filter_no][mode][currchannel] * pp->R) *
		       pp->amplify);
	if (nchannels > 1)
	    currchannel = !currchannel;

	sound++;
    }
}

void
RC_bandpass(DSP_SAMPLE *sound, int count, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++) {
	RC_filter(sound, count, HIGHPASS, a, pp);
	RC_filter(sound, count, LOWPASS, a, pp);
    }
}

void
RC_highpass(DSP_SAMPLE *sound, int count, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(sound, count, HIGHPASS, a, pp);
}

void
RC_lowpass(DSP_SAMPLE *sound, int count, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(sound, count, LOWPASS, a, pp);
}
