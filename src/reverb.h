/*
 * $Id$
 */

#ifndef _REVERB_H_
#define _REVERB_H_ 1

#include "pump.h"

extern void     reverb_create(struct effect *);

struct reverbBuffer {
    int            *data;
    int             nChunks;
    int             nCursor;
};

struct reverb_params {
    struct reverbBuffer *history;
    int             dry,
                    wet,
                    regen,
                    delay;
};

#endif
