/*
 * GNUitar
 * Delay effect: definitions
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

#ifndef _DELAY_H_
#define _DELAY_H_ 1

#include "effect.h"

#define MAX_COUNT 10
#define MAX_SECONDS 10
#define MAX_STEP (MAX_SAMPLE_RATE * MAX_SECONDS)
#define MAX_SIZE (MAX_STEP * (MAX_COUNT + 1))

effect_t *   delay_create(void);

struct delay_params {
    Backbuf_t       *history[MAX_CHANNELS];  /* history of samples */
    short            multichannel;
    float            delay_decay,   /* % of decay each time */
                     delay_time;    /* time (ms) between repeats */
    int              delay_count;    /* number of repeats */
};

#endif
