/*
 * GNUitar
 * Vibrato effect
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

#ifndef _VIBRATO_H_
#define _VIBRATO_H_ 1

#include "effect.h"
#include "biquad.h"

#define MAX_VIBRATO_BUFSIZE 16384

effect_t *   vibrato_create(void);

struct vibrato_params {
    Hilbert_t       hilbert;
    float           vibrato_amplitude, vibrato_base,
       		    vibrato_speed,
                    vibrato_phase,
                    phase;
};

#endif
