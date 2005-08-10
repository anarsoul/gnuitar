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

void
backbuff_init(struct backBuf *b, unsigned int maxDelay)
{
    b->nstor = maxDelay + 1;
    b->storage = (BUF_TYPE *) malloc(sizeof(BUF_TYPE) * b->nstor);
    memset(b->storage, 0, b->nstor * sizeof(BUF_TYPE));
    b->curpos = 0;
}

void
backbuff_done(struct backBuf *b)
{
    free(b->storage);
}

void
backbuff_add(struct backBuf *b, BUF_TYPE d)
{
    b->curpos++;
    if (b->curpos == b->nstor)
	b->curpos = 0;
    b->storage[b->curpos] = d;
}

/* XXX optimize this a bit */
BUF_TYPE
backbuff_get_interpolated(struct backBuf *b, double delay)
{
    BUF_TYPE x, x1, x2;
    unsigned int delay1 = delay;
    unsigned int delay2 = delay + 1;
    
    if (delay2 == b->nstor)
        delay2 = 0;
    
    x1 = backbuff_get(b, delay1);
    x2 = backbuff_get(b, delay2);

    delay = delay - delay1;
    x = x1 * (1 - delay) + x2 * delay;
    
    return x;
}

BUF_TYPE
backbuff_get(struct backBuf *b, unsigned int delay)
{
    int             getpos;
    assert(delay < b->nstor);
    getpos = (int) b->curpos;
    getpos -= delay;
    if (getpos < 0)
	getpos += b->nstor;

    assert(getpos >= 0 && getpos < (int) b->nstor);

    return b->storage[getpos];
}
