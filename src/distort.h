/*
 * $Id$
 */

#ifndef _DISTORT_H_
#define _DISTORT_H_ 1

#include "pump.h"
#include "rcfilter.h"

extern void     distort_create(struct effect *);

struct distort_params {
    int             sat,
                    level,
                    drive,
                    lastval;
    short           clip;
    int             lowpass;
    struct filter_data fd;
    struct filter_data noise;
};

#endif
