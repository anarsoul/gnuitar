/*
 * $Id$
 */

#ifndef _PHASOR_H_
#define _PHASOR_H_ 1

#include "pump.h"
#include "rcfilter.h"

extern void     phasor_create(struct effect *);

struct phasor_params {
    float           freq_low;
    float           freq_high;
    float           speed;
    float           f,
                    df;
    struct filter_data fd;
};

#endif
