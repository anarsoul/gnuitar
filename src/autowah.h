/*
 * GNUitar
 * Autowah effect: definitions
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

#ifndef _AUTOWAH_H_
#define _AUTOWAH_H_ 1

#include "pump.h"
#include "rcfilter.h"
#include "backbuf.h"

extern effect_t *   autowah_create();

struct autowah_params {
    Backbuf_t       *history;
    
    double          sweep_time;
    double          freq_low;
    double          freq_high;
    double          res;
    double	    drywet;
    int             continuous;

    double          fresh_accum_delta;
    double          fresh_accum_power;
    double          delayed_accum_delta;
    double          delayed_accum_power;
    int             accum_n;
    
    double          f, smoothed_f, freq_vibrato;
    int             dir;
    struct filter_data *fd;
    double          ya[MAX_CHANNELS];
    double          yb[MAX_CHANNELS];
    double          yc[MAX_CHANNELS];
    double          yd[MAX_CHANNELS];
};

#endif
