/* GNUitar effect interface.
 * Copyright (C) 2006 Antti S. Lankila. Licensed under GPL.
 *
 * $Id$
 *
 * $Log$
 * Revision 1.1  2006/08/07 13:20:42  alankila
 * - group all effects through effect.h rather than enumerating them in
 *   pump.c.
 *
 */

#include "effect.h"

#include "amp.h"
#include "autowah.h"
#include "phasor.h"
#include "chorus.h"
#include "delay.h"
#include "echo.h"
#include "tremolo.h"
#include "vibrato.h"
#include "distort.h"
#include "distort2.h"
#include "tubeamp.h"
#include "sustain.h"
#include "reverb.h"
#include "rotary.h"
#include "noise.h"
#include "eqbank.h"
#include "pitch.h"
#include "tuner.h"

#include "utils.h"

effect_t       *effects[MAX_EFFECTS];
int             effects_n = 0;
my_mutex        effectlist_lock;

/* note that vibrato & tremolo effects are swapped */
struct effect_creator effect_list[] = {
    {"Digital amp", amp_create},
    {"Autowah", autowah_create},
    {"Distort", distort_create},
    {"Delay", delay_create},
    {"Reverb", reverb_create},
    {"Tremolo bar", vibrato_create},
    {"Chorus / Flanger", chorus_create},
    {"Echo", echo_create},
    {"Phaser", phasor_create},
    {"Tremolo", tremolo_create},
    {"Sustain", sustain_create},
    {"Overdrive", distort2_create},
    {"Tube amplifier", tubeamp_create},
    {"Rotary speaker", rotary_create},
    {"Noise gate", noise_create},
    {"Eq bank", eqbank_create},
    {"Pitch shift", pitch_create},
    {"Tuner", tuner_create},
    {NULL, NULL}
};




