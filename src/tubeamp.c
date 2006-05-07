/*
 * GNUitar
 * Distortion effect 3 -- Tube amplifier
 * Copyright (C) 2006 Antti S. Lankila  <alankila@bel.fi>
 *
 * GPL license.
 *
 * $Id$
 *
 * $Log$
 * Revision 1.1  2006/05/07 13:22:12  alankila
 * - new bare bones distortion effect: tubeamp
 *
 *
 */

#include <math.h>
#include <gui.h>
#include <stdlib.h>

#include "biquad.h"
#include "tubeamp.h"

static void
tubeamp_init(struct effect *p)
{
    GtkWidget      *button;
    GtkWidget      *parmTable;

    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(1, 1, FALSE);

    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Overdrive"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

#define DISTORTION_AMOUNT (MAX_SAMPLE * 0.5)
/* asymmetric waveshaper based on offseted tan */
static float
waveshaper(float input, int idx) {
    //input += idx * DISTORTION_AMOUNT / 8;
    //return input - powf(input / DISTORTION_AMOUNT, 3) / 3 * input;
    return tanh(input / DISTORTION_AMOUNT + 0.5 + (idx-4) / 10.0) * DISTORTION_AMOUNT;
}

static void
tubeamp_filter(struct effect *p, struct data_block *db)
{
    int i, j, curr_channel = 0;
    struct tubeamp_params *params = p->params;

    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < db->len; i += 1) {
        float result = db->data[i];
        for (j = 0; j < MAX_STAGES; j += 1) {
            result = do_biquad(result, &params->highpass[j], curr_channel);
            result = do_biquad(result, &params->lowshelf[j], curr_channel);
            result = do_biquad(result, &params->lowpass[j], curr_channel);
            result = waveshaper(result, j);
            result = -result * 1.80; /* compensate for lost gain in lsh and waveshaper */
        }
        result = do_biquad(result, &params->final_highpass, curr_channel);
        db->data[i] = result / 2.0;
        curr_channel = (curr_channel + 1) % db->channels;
    }
    
    return;
}

static void
tubeamp_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
tubeamp_save(struct effect *p, SAVE_ARGS)
{
    //struct tubeamp_params *params = p->params;

    return;
}

static void
tubeamp_load(struct effect *p, LOAD_ARGS)
{
    //struct tubeamp_params *params = p->params;

    return;
}

effect_t *
tubeamp_create()
{
    effect_t   *p;
    int i;
    struct tubeamp_params *params;

    p = calloc(1, sizeof(effect_t)); 
    params = p->params = calloc(1, sizeof(struct tubeamp_params));
    p->proc_init = tubeamp_init;
    p->proc_filter = tubeamp_filter;
    p->proc_save = tubeamp_save;
    p->proc_load = tubeamp_load;
    p->toggle = 0;
    p->proc_done = tubeamp_done;

    for (i = 0; i < MAX_STAGES; i += 1) {
        set_rc_highpass_biquad(sample_rate, 10, &params->highpass[i]);
        set_rc_lowpass_biquad(sample_rate, 5000, &params->lowpass[i]);
        set_lsh_biquad(sample_rate, 80, -3, &params->lowshelf[i]);
    }
    set_rc_highpass_biquad(sample_rate, 10, &params->final_highpass);
    
    return p;
}
