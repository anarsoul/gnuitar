/*
 * circular buffer for delay 
 */

/*
 * $Id$
 */

#ifndef _BACKBUF_H_
#define _BACKBUF_H_ 1

#define BUF_TYPE int

struct backBuf {

    BUF_TYPE       *storage;
    unsigned int    nstor;
    unsigned int    curpos;
};

void            backbuff_init(struct backBuf *b, unsigned int maxDelay);
void            backbuff_done(struct backBuf *b);
void            backbuff_add(struct backBuf *b, BUF_TYPE d);
BUF_TYPE        backbuff_get(struct backBuf *b, unsigned int Delay);

#endif
