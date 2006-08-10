/*
 * GNUitar
 * Tuner plugin: definitions
 * Copyright (C) 2005 Antti Lankila  <alankila@bel.fi>
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
 * Revision 1.10  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.9  2006/08/10 13:57:48  alankila
 * - use fftw3f instead of fftw3 to avoid slower doubles
 *
 * Revision 1.8  2006/08/06 20:14:55  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.7  2006/07/18 21:35:59  alankila
 * - add optional FFT-based implementation -- it is several times faster
 *   than the time-domain version and nearly as good.
 *
 * Revision 1.6  2005/09/05 19:08:12  alankila
 * - abolish global variables. It's either that, or we forbid opening more than
 *   one tuner at once
 *
 * Revision 1.5  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.4  2005/09/01 13:36:23  alankila
 * Objectify backbuf, correct naming and make it a typedef.
 *
 * Revision 1.3  2005/08/10 17:55:11  alankila
 * - migrate tuner to use the backbuff code
 *
 * Revision 1.2  2005/08/08 16:30:59  alankila
 * - noise-reducing 4-tap FIR. This should kill signal fairly completely
 *   around 11 kHz.
 *
 * Revision 1.1  2005/08/07 12:53:42  alankila
 * - new tuner plugin / effect
 * - some gcc -Wall shutups
 * - added the entry required for gnuitar.vcproj as well but I can't test it
 * - changed pump.h to use enum instead of bunch-of-defines. Hopefully it's
 *   better that way.
 *
 * 
 */

#ifndef _TUNER_H_
#define _TUNER_H_ 1

#include "effect.h"
#ifdef HAVE_FFTW3
#include "fftw3.h"
#endif

#include "backbuf.h"

extern effect_t *   tuner_create(void);

#define FREQ_SIZE 16        /* frequency measurements history buffer size */
#define MAX_STRINGS 6	    /* max.number of strings */

/* data defined in order of progressive refinements */
struct tuner_params {
    /* backlog of sample data */
    Backbuf_t	    *history;

#ifdef HAVE_FFTW3
    /* fftw3 state */
    fftwf_complex  *fftin;
    fftwf_plan      fftfw;
    fftwf_plan      fftbw;
    int             count;
#endif
    
    /* signal processing helpers */
    float	    power;
    DSP_SAMPLE	    oldval[3];
    
    /* raw measurements */
    float	    freq_history[FREQ_SIZE];
    int		    freq_index;
    float	    sorted_freq_history[FREQ_SIZE];

    /* final frequency */
    float	    freq;
    
    /* GUI */
    GtkWidget	    *label_current, *label_ideal, *ruler, *led_table, *layout_combo, *leds[MAX_STRINGS], *note_letters[MAX_STRINGS];
    int             curr_layout;
    int             layout[MAX_STRINGS];
    gboolean	    quitting;
};

#endif
