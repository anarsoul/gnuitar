/*
 * GNUitar
 * Backbuf - circular buffer for delay
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
 *
 * $Log$
 * Revision 1.12  2006/08/10 18:52:07  alankila
 * - declare prototypes properly
 * - hide some accidentally global methods
 *
 * Revision 1.11  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.10  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.9  2005/09/16 20:40:18  alankila
 * - increase backbuf performance by allocating to nearest exponent of 2
 *
 * Revision 1.8  2005/09/01 14:09:56  alankila
 * - multichannel work: delay independent of nchannels; uses backbuf instead
 *   of doing it all on its own. Also fixes bugs with delay load/save.
 *
 * Revision 1.7  2005/09/01 13:36:23  alankila
 * Objectify backbuf, correct naming and make it a typedef.
 *
 * Revision 1.6  2005/08/10 18:37:54  alankila
 * - add function for fetching interpolated samples
 *
 * Revision 1.5  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.4  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.3  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.2  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:06  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "backbuf.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static void
backbuf_add(Backbuf_t *b, const BUF_TYPE d)
{
    b->curpos += 1;
    b->storage[b->curpos & b->mask] = d;
}

static BUF_TYPE
backbuf_get(Backbuf_t *b, const unsigned int delay)
{
    assert(delay < b->nstor);
    return b->storage[(b->curpos - delay) & b->mask];
}

/* XXX optimize this a bit */
static BUF_TYPE
backbuf_get_interpolated(Backbuf_t *b, float delay)
{
    unsigned int delay_int = delay;
    unsigned int getpos;
    
    delay -= delay_int;
    getpos = b->curpos - delay_int;
    return b->storage[getpos & b->mask] * (1 - delay) + b->storage[(getpos - 1) & b->mask] * delay;
}

static void
backbuf_clear(Backbuf_t *b)
{
    memset(b->storage, 0, (b->mask + 1) * sizeof(b->storage[0]));
}

Backbuf_t *
new_Backbuf(const unsigned int max_delay)
{
    unsigned int size;
    
    Backbuf_t *b = calloc(1, sizeof(Backbuf_t));

    b->nstor = max_delay + 1;
    size = 1;
    while (size < b->nstor)
        size <<= 1;
    b->storage = calloc(size, sizeof(BUF_TYPE));
    b->mask = size - 1;
    b->curpos = 0;
    b->add = backbuf_add;
    b->get = backbuf_get;
    b->get_interpolated = backbuf_get_interpolated;
    b->clear = backbuf_clear;
    return b;
}

void
del_Backbuf(Backbuf_t *b)
{
    free(b->storage);
    free(b);
}
