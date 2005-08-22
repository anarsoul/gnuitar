/*
 * GNUitar
 * RC-filter emulation
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

#ifndef RCFILTER
#define RCFILTER 1

#include "pump.h"

#define HIGHPASS 0
#define LOWPASS 1

#define MAX_FILTERS 10

struct filter_data {
    double          i[MAX_FILTERS][2][MAX_CHANNELS],
                    last_sample[MAX_FILTERS][2][MAX_CHANNELS];
    double          max,
                    amplify,
                    R,
                    C,
                    invR,
                    dt_div_C;
    double          di[MAX_FILTERS][2][MAX_CHANNELS];
};

extern void     LC_filter(struct data_block *db, int, double, struct filter_data *);
extern void     RC_setup(int, double, struct filter_data *);
extern void     RC_set_freq(double, struct filter_data *);
extern void     RC_bandpass(struct data_block *db, struct filter_data *);
extern void     RC_highpass(struct data_block *db, struct filter_data *);
extern void     RC_lowpass(struct data_block *db, struct filter_data *);

#endif
