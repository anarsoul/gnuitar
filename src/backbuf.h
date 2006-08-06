/*
 * GNUitar
 * Backbuf - circular buffer for delay.
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

#ifndef _BACKBUF_H_
#define _BACKBUF_H_ 1

#include "audio-driver.h"   /* for DSP_SAMPLE */

#define BUF_TYPE DSP_SAMPLE
typedef struct Backbuf {
    BUF_TYPE       *storage;
    unsigned int    mask, nstor, curpos;
    /* method slots */
    void	(*add)		    (struct Backbuf *b, const BUF_TYPE value);
    BUF_TYPE	(*get)		    (struct Backbuf *b, const unsigned int pos);
    BUF_TYPE	(*get_interpolated) (struct Backbuf *b, float pos);
    void	(*clear)	    (struct Backbuf *b);
} Backbuf_t;

Backbuf_t * new_Backbuf(const unsigned int size);
void	    del_Backbuf(Backbuf_t *b);

#endif
