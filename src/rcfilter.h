/*
 * $Id$
 */

#ifndef RCFILTER
#define RCFILTER 1

#include "pump.h"

#define HIGHPASS 0
#define LOWPASS 1

#define MAX_FILTERS 10

struct filter_data {
    double          i[MAX_FILTERS][2],
                    last_sample[MAX_FILTERS][2];
    double          max,
                    amplify,
                    R,
                    C,
                    invR,
                    dt_div_C;
    double          di[MAX_FILTERS][2];
};

extern void     LC_filter(int *, int, int, double, struct filter_data *);
extern void     RC_setup(int, double, struct filter_data *);
extern void     RC_set_freq(double, struct filter_data *);
extern void     RC_bandpass(int *, int, struct filter_data *);
extern void     RC_highpass(int *, int, struct filter_data *);
extern void     RC_lowpass(int *, int, struct filter_data *);

#endif
