/*
 * $Id$
 *
 * $Log$
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
