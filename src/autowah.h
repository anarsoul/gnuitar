/*
 * $Id$
 */

#ifndef _AUTOWAH_H_
#define _AUTOWAH_H_ 1

#include "pump.h"
#include "rcfilter.h"

extern void     autowah_create(struct effect *);

struct autowah_params {
    float           freq_low;
    float           freq_high;
    float           speed;
    int             wah_count;
    float           f,
                    df;
    struct filter_data *fd;
    unsigned short  mixx;
};

#endif
