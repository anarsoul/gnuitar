/* $Id
 * Copyright 2006 Antti S. Lankila. All rights reserved.
 * Licensed under the GPL. See http://www.gnu.org/ for details. */

#include <stdio.h>

#include "audio-midi.h"

typedef struct {
    float cc_latest;
    float cc[128];
    int key;
    short keyevent;
} midictrl_t;

static volatile midictrl_t midictrl = {
    .cc_latest = 0,
    .keyevent = 0,
};

void midi_set_continuous_control(int type, float value) {
    if (type < 0 || type > 0x7f) {
        fprintf(stderr, "Invalid continuous control: %d => %f\n", type, value);
        return;
    }

    midictrl.cc_latest = value;
    midictrl.cc[type] = value;
}

float midi_get_continuous_control(int type) {
    if (type == -1)
        return midictrl.cc_latest;

    return midictrl.cc[type];
}

void midi_set_program(int program) {
    midictrl.key = program;
    midictrl.keyevent = 1;
}

int midi_get_program() {
    if (! midictrl.keyevent)
        return -1;
    midictrl.keyevent = 0;
    return midictrl.key;
}

