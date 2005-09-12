/*
 * GNUitar
 * Pitch Shifter effect
 * Copyright (c) 2005 Antti S. Lankila  <alankila@bel.fi>
 *
 * See COPYING for details about license.
 *
 * $Id$
 */

#ifndef _PITCH_H_
#define _PITCH_H_ 1

#include "backbuf.h"

#define PITCH_GAIN_CORRECTION_RESOLUTION 4

extern effect_t *   pitch_create();

struct pitch_params {
    Backbuf_t      *history[MAX_CHANNELS];
    
    /* user tunables */
    int             halfnote;
    double          finetune, drywet;
    
    /* current modulation phase */
    double          phase;

    /* output power correction */
    double          input[PITCH_GAIN_CORRECTION_RESOLUTION],
                    output[PITCH_GAIN_CORRECTION_RESOLUTION];
    int             inout_n[PITCH_GAIN_CORRECTION_RESOLUTION];
};

#endif
