/*
 * GNUitar
 * Phasor effect
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

#ifndef _PHASOR_H_
#define _PHASOR_H_ 1

#include "effect.h"
#include "biquad.h"

#define MAX_PHASOR_FILTERS  24

extern effect_t *   phasor_create(void);

struct phasor_params {
    double          depth, sweep_time, drywet, f;
    int             stereo;
    Biquad_t        allpass[MAX_PHASOR_FILTERS];
    Hilbert_t       hilb;
};

#endif
