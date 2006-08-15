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

#include "effect.h"
#include "backbuf.h"
#include "biquad.h"

effect_t *   pitch_create(void);

struct pitch_params {
    Backbuf_t      *history[MAX_CHANNELS];
    DSP_SAMPLE     *channel_memory[MAX_CHANNELS], *output_memory[MAX_CHANNELS];
    int memory_index;
    float output_pos, output_buffer_trigger;
    
    /* user tunables */
    int             halfnote;
    double          finetune, drywet;
};

#endif
