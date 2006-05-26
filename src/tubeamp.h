/*
 * GNUitar
 * Distortion effect
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
 */

#ifndef _TUBEAMP_H_
#define _TUBEAMP_H_ 1

#include "pump.h"
#include "biquad.h"

extern effect_t *tubeamp_create();

#define MAX_STAGES 4
#define MAX_IMPULSE_LENGTH 512

struct tubeamp_params {
    int         stages;
    float       gain, middlefreq;
    float       asymmetry, biasfactor;
    
    /* internal state variables */
    Biquad_t    highpass[MAX_STAGES];
    Biquad_t    lowpass[MAX_STAGES];
    Biquad_t    biaslowpass[MAX_STAGES];
    Biquad_t    middlecut[MAX_STAGES];
    float       bias[MAX_STAGES];
    float       r_i[MAX_STAGES], r_k_p[MAX_STAGES];
    
    float       in[MAX_CHANNELS];
    /* convolution buffer */
    DSP_SAMPLE  buf[MAX_CHANNELS][MAX_IMPULSE_LENGTH * 2];
    int         bufidx[MAX_CHANNELS];
    
    Biquad_t    decimation_filter;
};

#endif
