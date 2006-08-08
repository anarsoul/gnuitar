/*
 * GNUitar
 * Pump module - processing sound
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

#ifndef PUMP_H
#define PUMP_H 1

#include "audio-driver.h"

extern volatile unsigned short write_track;

void     pump_sample(data_block_t *db);
void     pump_start(void);
void     pump_stop(void);
void     save_pump(const char *fname);
void     load_pump(const char *fname);
void     load_settings(void);
void     save_settings(void);
void     load_initial_state(char **argv, int argc);
char    *discover_preset_path(void);

#endif

