/*
 * $Id$
 */

#ifndef _VIBRATO_H_
#define _VIBRATO_H_ 1

#include "pump.h"

#define MAX_VIBRATO_BUFSIZE 20000

extern void     vibrato_create(struct effect *);

struct vibrato_params {
    int             vibrato_amplitude,
                    vibrato_speed,
                    vibrato_phase_buffer_size;
    int             vibrato_phase,
                   *phase_buffer;
};

#endif
