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

extern effect_t *   pitch_create();

struct pitch_params {
    Backbuf_t      *history[MAX_CHANNELS];
    int             halfnote;
    double          finetune, drywet;
    double          phase;
};

#endif
