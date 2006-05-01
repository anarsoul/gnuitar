/*
 * GNUitar
 * Amp effect: definitions
 */

#ifndef _AMP_H_
#define _AMP_H_ 1

#include "pump.h"


extern effect_t *   amp_create();

struct amp_params {
    float            amp_power; //power of amp
};

#endif
