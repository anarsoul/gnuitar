/* GNUitar effect interface.
 * Copyright (C) 2006 Antti S. Lankila. Licensed under GPL.
 *
 * $Id$
 *
 * $Log$
 * Revision 1.3  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.2  2006/08/07 20:01:50  alankila
 * - move all modifications of effect list structures into effect.c.
 *
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

static effect_t       *effects[MAX_EFFECTS];
static int             effects_n = 0;
static my_mutex        effectlist_lock;

struct effect_creator {
    const char           *str;
    effect_t *          (*create_f)(void);
};

static const struct effect_creator effect_list[] = {
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

/* for the old STDOUT listing -- need iterators to do these two... */
void
effect_list_print_all(void)
{
    int i = 0;
    for (i = 0; effect_list[i].str != NULL; i += 1) {
        printf("\t%s\n", effect_list[i].str);
    }
}

void
effect_list_add_to_clist(GtkWidget *w)
{
    int i = 0;
    for (i = 0; effect_list[i].str != NULL; i += 1) {
        gtk_clist_append(GTK_CLIST(w), (char **) &effect_list[i].str);
    }
}

int
effect_list_find_by_name(const char *name)
{
    int k = 0;
    while (effect_list[k].str && strcmp(name, effect_list[k].str)) {
	k += 1;
    }
    if (effect_list[k].str == NULL)
        return -1;
    return k;
}

void
effect_start(void)
{
    my_create_mutex(&effectlist_lock);
    effects_n = 0;
    memset(effects, 0, sizeof(effects));
}

void
effect_stop(void)
{
    effect_clear();
    my_close_mutex(effectlist_lock);
}

/* almost lockless effect destroy */
void
effect_clear(void)
{
    int old_n, i;

    my_lock_mutex(effectlist_lock);
    old_n = effects_n;
    effects_n = 0;
    my_unlock_mutex(effectlist_lock);

    for (i = 0; i < old_n; i++)
	effects[i]->proc_done(effects[i]);
}

/* find index by effect identity */
int
effect_find(const effect_t *target)
{
    int i;

    my_lock_mutex(effectlist_lock);
    for (i = 0; i < effects_n; i++) {
        if (effects[i] == target)
            break;
    }
    my_unlock_mutex(effectlist_lock);
    /* no such effect? */
    if (i == effects_n)
        return -1;
    return i;
}

/* remove effect from effectlist */
effect_t *
effect_delete(int i)
{
    effect_t *effect = NULL;
    my_lock_mutex(effectlist_lock);
    if (i < effects_n) {
        effects_n -= 1;
        effect = effects[i];
        for (; i < effects_n; i += 1)
            effects[i] = effects[i+1];
        effects[effects_n] = NULL;
    }
    my_unlock_mutex(effectlist_lock);
    return effect;
}

/* moving is different from just swapping. In move, an effect is dragged
 * from its position to another position, and effects in-between must
 * shift upwards/downwards. */
int
effect_move(const int start, const int end)
{
    int i;
    effect_t *swap;

    //gnuitar_printf("start=%d, end=%d\n", start, end);
    if (start < 0 || start >= effects_n || end < 0 || end >= effects_n || start == end)
        return 0;

    my_lock_mutex(effectlist_lock);
    if (start < end) {
        swap = effects[start];
        for (i = start; i < end; i += 1)
            effects[i] = effects[i+1];
        effects[end] = swap;
    } else if (start > end) {
        swap = effects[start];
        for (i = start; i > end; i -= 1)
            effects[i] = effects[i-1];
        effects[end] = swap;
    }
    my_unlock_mutex(effectlist_lock);
    return 1;
}

/* insert into effect list */
int
effect_insert(effect_t *effect, const int curr_row)
{
    int i, idx;

    my_lock_mutex(effectlist_lock);
    if (curr_row >= 0 && curr_row < effects_n) {
	idx = curr_row + 1;
	for (i = effects_n; i > idx; i--) {
	   effects[i] = effects[i - 1];
	}
	effects_n += 1;
    } else {
	idx = effects_n;
	effects_n += 1;
    }
    effects[idx] = effect;
    my_unlock_mutex(effectlist_lock);

    return idx;
}

/* evaluate a function for all effects */
void
effect_iterate(void (*func)(effect_t *effect, int idx, void *data), void *data)
{
    int i;
    my_lock_mutex(effectlist_lock);
    for (i = 0; i < effects_n; i += 1) {
        func(effects[i], i, data);
    }
    my_unlock_mutex(effectlist_lock);
}

/* construct and destroy effects */
effect_t *
effect_create(const int idx)
{
    effect_t *effect = effect_list[idx].create_f();
    effect->proc_init(effect);
    return effect;
}

effect_t *
effect_create_without_init(const int idx)
{
    return effect_list[idx].create_f();
}

void
effect_destroy(effect_t *effect)
{
    effect->proc_done(effect);
}
