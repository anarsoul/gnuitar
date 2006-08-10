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

#ifndef _DISTORT2_H_
#define _DISTORT2_H_ 1

#include "effect.h"
#include "biquad.h"

effect_t *distort2_create(void);

struct distort2_params {
    float       drive, clip, treble;
    float 	last[MAX_CHANNELS];

    DSP_SAMPLE  interpolate_firmem[MAX_CHANNELS][8];
    Biquad_t    feedback_minus_loop, output_bass_cut,
                rolloff, treble_highpass;
};

#endif
