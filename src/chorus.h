/*
 * $Id$
 */

#ifndef _CHORUS_H_
#define _CHORUS_H_ 1

#include "pump.h"
#include "backbuf.h"

extern void     chorus_create(struct effect *);

struct chorus_params {
    int             wet,
                    dry,
                    depth;
    short           mode;
    int             speed,
                    regen;
    float           ang;

    struct backBuf *memory;
};

#endif
