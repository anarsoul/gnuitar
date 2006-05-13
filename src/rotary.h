/*
 * GNUitar
 * Rotary effect
 * Copyright (C) 2006 Antti S. Lankila  <alankila@bel.fi>
 *
 * $Id$
 */

#ifndef _ROTARY_H_
#define _ROTARY_H_ 1

#include "pump.h"
#include "biquad.h"

#define FFT_SIZE 512

extern effect_t *rotary_create();

struct rotary_params {
    int         speed;
    int         time_to_next_fft, unread_output;
    float       phase;
    DSP_SAMPLE  x0_tmp;
    Biquad_t    ld, rd, a1[4], a2[4];
};

#endif
