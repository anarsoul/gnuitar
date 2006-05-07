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
 * Revision 1.4  2006/05/07 18:58:53  alankila
 * - knob to tone the midrange distortion fatness
 *
 * Revision 1.3  2006/05/07 18:22:23  alankila
 * - produce controls and more aggressive defaults
 *
 * Revision 1.2  2006/05/07 14:38:15  alankila
 * - reimplement shelve as partial highpass
 * - use less stages
 *
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
update_stages(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->stages = adj->value;
}

static void
update_gain(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->gain = adj->value;
}

static void
update_lsfreq(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->lsfreq = adj->value;
}

static void
update_treblefreq(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->treblefreq = adj->value;
}

static void
update_middlecut(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->middlecut = adj->value;
}

static void
tubeamp_init(struct effect *p)
{
    GtkWidget      *w;
    GtkObject      *o;
    GtkWidget      *parmTable;
    struct tubeamp_params *params = p->params;
    
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(5, 2, FALSE);

    w = gtk_label_new("Stages\n(n)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->stages, 3, MAX_STAGES, 1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_stages), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_scale_set_digits(GTK_SCALE(w), 0);
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    
    w = gtk_label_new("Gain\n(dB)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 1, 2, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->gain, 6.0, 18.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_gain), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 1, 2, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    
    w = gtk_label_new("Bass cut\n(Hz)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 2, 3, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->lsfreq, 20.0, 320.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_lsfreq), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 2, 3, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);

    w = gtk_label_new("Middle cut\n(dB)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 3, 4, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->middlecut, -9.9, 0.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_middlecut), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_range_set_inverted(GTK_RANGE(w), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 3, 4, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    
    w = gtk_label_new("Treble cut\n(Hz)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 4, 5, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->treblefreq, 1600.0, 8000.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_treblefreq), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 4, 5, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    
    w = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
    gtk_signal_connect(GTK_OBJECT(w), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Tube amplifier"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

#define DISTORTION_AMOUNT (MAX_SAMPLE * 2.0)
/* asymmetric waveshaper based on offseted tan */
static float
waveshaper(float input) {
    return tanh(input / DISTORTION_AMOUNT + 0.5) * DISTORTION_AMOUNT;
}

static void
tubeamp_filter(struct effect *p, struct data_block *db)
{
    int i, j, curr_channel = 0;
    struct tubeamp_params *params = p->params;
    float gain;

    for (i = 0; i < MAX_STAGES; i += 1) {
        set_rc_highpass_biquad(sample_rate, 10, &params->highpass[i]);
        set_rc_lowpass_biquad(sample_rate, params->treblefreq, &params->lowpass[i]);
        set_rc_lowpass_biquad(sample_rate, params->biasfreq, &params->biaslowpass[i]);
        set_rc_highpass_biquad(sample_rate, params->lsfreq, &params->lowshelf[i]);
        set_peq_biquad(sample_rate, 720, 500.0, params->middlecut, &params->middlecut_bq[i]);
    }

    gain = pow(10, params->gain / 20);
    
    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < db->len; i += 1) {
        float result = db->data[i];
        for (j = 0; j < params->stages; j += 1) {
            result = do_biquad(result, &params->highpass[j], curr_channel);
            result = do_biquad(result, &params->middlecut_bq[j], curr_channel);
            result = 0.5 * result + 0.5 * do_biquad(result, &params->lowshelf[j], curr_channel);
            result = waveshaper(result - params->bias[j] * (0.25 + j / (MAX_STAGES * 2.0))) * gain;
            params->bias[j] = do_biquad(result, &params->biaslowpass[j], curr_channel);
            result = do_biquad(result, &params->lowpass[j], curr_channel);
            result = -result;
        }
        result = do_biquad(result, &params->final_highpass, curr_channel);
        result = do_biquad(result, &params->final_lowpass, curr_channel);
        db->data[i] = result / gain / params->stages;
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
    struct tubeamp_params *params;

    p = calloc(1, sizeof(effect_t)); 
    params = p->params = calloc(1, sizeof(struct tubeamp_params));
    p->proc_init = tubeamp_init;
    p->proc_filter = tubeamp_filter;
    p->proc_save = tubeamp_save;
    p->proc_load = tubeamp_load;
    p->toggle = 0;
    p->proc_done = tubeamp_done;

    params->stages = 6;
    params->gain = 12.0;
    params->lsfreq = 80;
    params->treblefreq = 4500;
    params->biasfreq = 40;
    params->middlecut = -5.0;
    
    set_rc_lowpass_biquad(sample_rate, 8000, &params->final_lowpass);
    set_rc_highpass_biquad(sample_rate, 10, &params->final_highpass);
    
    return p;
}
