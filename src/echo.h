/*
 * $Id$
 */

#ifndef _ECHO_H_
#define _ECHO_H_ 1

#include "pump.h"

#define MAX_ECHO_COUNT 20
#define MAX_ECHO_SIZE 1000

extern void     echo_create(struct effect *);

struct echo_params {
    int             echo_size,
                    echo_decay,
                    buffer_count;
    int           **history,
                   *index,
                   *size,
                   *factor;
};

#endif
