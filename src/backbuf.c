/*
 * GNUitar
 * Backbuf - circular buffer for delay
 * Copyright (C) 2000,2001 Max Rudensky		<fonin@ziet.zhitomir.ua>
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

BUF_TYPE
backbuff_get(struct backBuf *b, unsigned int Delay)
{
    int             getpos;
    assert(Delay < b->nstor);
    getpos = (int) b->curpos;
    getpos -= Delay;
    if (getpos < 0)
	getpos += b->nstor;

    assert(getpos >= 0 && getpos < (int) b->nstor);

    return b->storage[getpos];
}
