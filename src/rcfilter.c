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
 * Revision 1.13  2006/08/10 18:52:07  alankila
 * - declare prototypes properly
 * - hide some accidentally global methods
 *
 * Revision 1.12  2006/08/06 20:14:55  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.11  2005/09/01 16:09:54  alankila
 * - make rcfilter and autowah multichannel ready. In addition, autowah
 *   now performs linear sweep in logarithmic domain rather than exponential.
 *
 * Revision 1.10  2005/08/22 22:27:34  alankila
 * - erase the nchannel parts from constant computations. These can't be
 *   correct, because all state variables are per channel. In case of RC_filter
 *   the nchannels got cancelled out, but not so lucky with LC_filter. This
 *   will change the sound for some effects on 2 channels and more.
 *
 * Revision 1.9  2005/08/22 22:11:59  alankila
 * - change RC filters to accept data_block
 * - LC filters have no concept of "LOWPASS" or "HIGHPASS" filtering, there's
 *   just filter_no.
 * - remove unused SAMPLE8 typedef
 *
 * Revision 1.8  2005/07/31 10:22:54  fonin
 * Check for NaN values on input and output
 *
 * Revision 1.7  2005/04/06 19:34:20  fonin
 * Code lickup
 *
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
#include <math.h>
#include <string.h>
#ifdef _WIN32
#    include "utils.h"		/* for M_PI */
#endif

void
LC_filter(data_block_t *db, int filter_no, double freq,
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
    DSP_SAMPLE     *sound = db->data;

    L = 50e-3;			/* 
				 * like original crybaby wahwah, hehehe 
				 */
    C = 1.0 / (4.0 * pow(M_PI * freq, 2.0) * L);
    R = 300.0;

    dt_div_C = 1.0 / (C * sample_rate);
    dt_div_L = 1.0 / (L * sample_rate);

    for (t = 0; t < db->len; t++) {
	if(isnan(*sound))
	    *sound=0;
	du = (double) *sound - pp->last_sample[filter_no][0][currchannel];
	pp->last_sample[filter_no][0][currchannel] = (double) *sound;

	d2i =
	    dt_div_L * (du - pp->i[filter_no][0][currchannel] * dt_div_C -
			R * pp->di[filter_no][0][currchannel]);
	pp->di[filter_no][0][currchannel] += d2i;
	pp->i[filter_no][0][currchannel] +=
	    pp->di[filter_no][0][currchannel];

	*sound = (int) (pp->i[filter_no][0][currchannel] * 500.0);
	if(isnan(*sound))
	    *sound=0;
	currchannel = (currchannel + 1) % db->channels;

	sound++;
    }
}

static double
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
    pp->C = other(f, pp->R);
    pp->invR = 1.0 / pp->R;
    pp->dt_div_C = 1.0 / (sample_rate * pp->C);
}

static void
RC_filter(data_block_t *db, int mode, int filter_no,
	  struct filter_data *pp)
{
    double          du,
                    di;
    int             t,
                    currchannel = 0;
    DSP_SAMPLE     *sound = db->data;
    
    for (t = 0; t < db->len; t++) {
	if(isnan(*sound))
	    *sound=0;
	du = (double) *sound -
	    pp->last_sample[filter_no][mode][currchannel];
	pp->last_sample[filter_no][mode][currchannel] = (double) *sound;

	di = pp->invR * (du -
			 pp->i[filter_no][mode][currchannel] *
			 pp->dt_div_C);
	pp->i[filter_no][mode][currchannel] += di;

	if (mode == HIGHPASS)
	    *sound =
		((pp->i[filter_no][mode][currchannel] * pp->R) *
		       pp->amplify);
	else
	    *sound =
		(((double) *sound -
			pp->i[filter_no][mode][currchannel] * pp->R) *
		       pp->amplify);
	currchannel = (currchannel + 1) % db->channels;
	if(isnan(*sound))
	    *sound=0;

	sound++;
    }
}

void
RC_bandpass(data_block_t *db, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++) {
	RC_filter(db, HIGHPASS, a, pp);
	RC_filter(db, LOWPASS, a, pp);
    }
}

void
RC_highpass(data_block_t *db, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(db, HIGHPASS, a, pp);
}

void
RC_lowpass(data_block_t *db, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(db, LOWPASS, a, pp);
}
