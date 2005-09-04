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

#include "backbuf.h"

extern effect_t *   tuner_create();

/* frequency measurements history buffer size */
#define FREQ_SIZE 16

/* data defined in order of progressive refinements */
struct tuner_params {
    /* backlog of sample data */
    Backbuf_t	    *history;

    /* signal processing helpers */
    double	    power;
    DSP_SAMPLE	    oldval[4];
    
    /* raw measurements */
    double	    freq_history[FREQ_SIZE];
    int		    freq_index;
    double	    sorted_freq_history[FREQ_SIZE];

    /* final frequency */
    double	    freq;
    
    /* GUI */
    GtkWidget	    *label_current;
    GtkWidget	    *label_ideal;
    GtkWidget	    *ruler;
    char	    freq_str_buf[80];
    gboolean	    quitting;
};

#endif
