/*
 * $Id$
 */

#ifndef _TREMOLO_H_
#define _TREMOLO_H_ 1

#include "pump.h"

extern void     tremolo_create(struct effect *);

struct tremolo_params {
    int             tremolo_size,
                    tremolo_amplitude,
                    tremolo_speed,
                    tremolo_phase_buffer_size;
    int            *history,
                    tremolo_index,
                    tremolo_phase,
                    index,
                   *phase_buffer;
};

#endif
