/*
 * $Id$
 */

#ifndef _SUSTAIN_H_
#define _SUSTAIN_H_ 1

#include "pump.h"

extern void     sustain_create(struct effect *);

struct sustain_params {
    int             sust,
                    noise,
                    threshold,
                    volaccum;
};

#endif
