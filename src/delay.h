/*
 * $Id$
 */

#ifndef _DELAY_H_
#define _DELAY_H_ 1

#include "pump.h"

#define MAX_STEP 65000
#define MAX_COUNT 10
#define MAX_SIZE (MAX_STEP*MAX_COUNT)

extern void     delay_create(struct effect *);

struct delay_params {
    int            *history,
                   *idelay,
                    index;
    int             delay_size,
                    delay_decay,
                    delay_start,
                    delay_step,
                    delay_count;
};

#endif
