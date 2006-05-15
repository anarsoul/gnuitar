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
 * Revision 1.16  2006/05/15 19:08:26  alankila
 * - remove several multiplier terms, collect them under F_tube
 * - some remaining constants such as 250 indicate original tube mains voltage
 * - increase feedback strength by some 10x -- warmer now
 *
 * Revision 1.15  2006/05/15 16:00:59  alankila
 * - move middle control into the inner loop
 *
 * Revision 1.14  2006/05/15 11:32:18  alankila
 * - rebalance bias for "punch"
 *
 * Revision 1.13  2006/05/15 10:55:46  alankila
 * - make it sound sweeter
 * - make initial lowpass IIR stronger
 *
 * Revision 1.12  2006/05/15 09:45:01  alankila
 * - new model according to Antti Huoviala et al. "Virtual Air Guitar",
 *   presented at 117th Audio Engineering Society conference.
 * - still missing: proper F_tube.
 *
 * Revision 1.11  2006/05/13 16:20:23  alankila
 * - further small tuning of the formula and the defaults
 *
 * Revision 1.10  2006/05/13 13:33:38  alankila
 * - new biasfreq, more tubish sound
 *
 * Revision 1.9  2006/05/13 08:53:05  alankila
 * - load/save functions
 *
 * Revision 1.8  2006/05/13 08:04:23  alankila
 * - parameter lickup
 *
 * Revision 1.7  2006/05/08 07:29:14  alankila
 * - improve nonlinearity crunch a bit -- add gain -- get by with less stages
 *
 * Revision 1.6  2006/05/07 22:39:18  alankila
 * - correct some errors in the model
 *
 * Revision 1.5  2006/05/07 21:53:26  alankila
 * - remove some multiplications in the waveshaper path
 *
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

#include "pump.h"
#include "biquad.h"
#include "tubeamp.h"

#define UPSAMPLE_RATIO 4
    
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
update_middlefreq(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->middlefreq = adj->value;
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
    o = gtk_adjustment_new(params->stages, 2, MAX_STAGES, 1, 1, 0);
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
    o = gtk_adjustment_new(params->gain, 20.0, 35.0, 0.1, 1, 0);
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
    o = gtk_adjustment_new(params->lsfreq, 20.0, 500.0, 0.1, 1, 0);
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
    o = gtk_adjustment_new(params->middlefreq, -5.0, 0.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_middlefreq), params);
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

/* no decent waveshaping at the moment */
static float
F_tube(float in, float r_i)
{
    r_i /= 3000;
    return tanh(in / r_i) * r_i;
}

static void
tubeamp_filter(struct effect *p, struct data_block *db)
{
    int i, j, k, curr_channel = 0;
    struct tubeamp_params *params = p->params;
    float gain;

    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, params->lsfreq, &params->final_highpass);
    set_chebyshev1_biquad(sample_rate * UPSAMPLE_RATIO, params->treblefreq, 1.0, TRUE, &params->final_lowpass);
    for (j = 0; j < params->stages; j += 1)
        set_peq_biquad(sample_rate * UPSAMPLE_RATIO, 720, 500.0, params->middlefreq, &params->middlecut[j]);
    gain = pow(10, params->gain / 20);
    
    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < db->len; i += 1) {
        float result;
        for (k = 0; k < UPSAMPLE_RATIO; k += 1) {
            /* IIR interpolation */
            params->in[curr_channel] = (db->data[i] + params->in[curr_channel] * 5) / 6.0;
            result = params->in[curr_channel] / MAX_SAMPLE;
            for (j = 0; j < params->stages; j += 1) {
                /* gain of the block */
                result *= gain;
                /* low-pass filter that mimicks input capacitance */
                result = do_biquad(result, &params->lowpass[j], curr_channel);
                /* add feedback bias current for "punch" simulation */
                result -= params->bias[j];
                /* run waveshaper */
                result = F_tube(result, params->r_i[j]);
                /* feedback bias */
                params->bias[j] = do_biquad((500 - 20 * result) * params->r_k[j] / params->r_p[j], &params->biaslowpass[j], curr_channel);
                /* high pass filter to remove bias from the current stage */
                result = do_biquad(result, &params->highpass[j], curr_channel);
                /* middlecut for user tone control, for the "metal crunch" sound */
                result = do_biquad(result, &params->middlecut[j], curr_channel);
                result = -result;
            }
            /* final tone control, a poor man's cabinet simulation */
            result = do_biquad(result, &params->final_highpass, curr_channel);
            result = do_biquad(result, &params->final_lowpass, curr_channel);
        }
        db->data[i] = result / 250 * MAX_SAMPLE;
        curr_channel = (curr_channel + 1) % db->channels;
    }
    //for (i = 0; i < params->stages; i += 1)
    //    printf("%d. bias=%.1f\n", i, params->bias[i]);
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
    struct tubeamp_params *params = p->params;
    SAVE_INT("stages", params->stages);
    SAVE_DOUBLE("gain", params->gain);
    SAVE_DOUBLE("lsfreq", params->lsfreq);
    SAVE_DOUBLE("treblefreq", params->treblefreq);
    SAVE_DOUBLE("middlefreq", params->middlefreq);
}

static void
tubeamp_load(struct effect *p, LOAD_ARGS)
{
    struct tubeamp_params *params = p->params;
    LOAD_INT("stages", params->stages);
    LOAD_DOUBLE("gain", params->gain);
    LOAD_DOUBLE("lsfreq", params->lsfreq);
    LOAD_DOUBLE("treblefreq", params->treblefreq);
    LOAD_DOUBLE("middlefreq", params->middlefreq);
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

    params->stages = 3;
    params->gain = 30.0;
    params->lsfreq = 20;
    params->treblefreq = 5000;
    params->middlefreq = -1.5;

    /* configure the various stages */
    params->r_i[0] = 68e3;
    params->r_p[0] = 100000;
    params->r_k[0] = 2700;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 22570, &params->lowpass[0]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 86, &params->biaslowpass[0]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[0]);
    
    params->r_i[1] = 250e3;
    params->r_p[1] = 100000;
    params->r_k[1] = 1500;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 6531, &params->lowpass[1]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 132, &params->biaslowpass[1]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[1]);
    
    params->r_i[2] = 250e3;
    params->r_p[2] = 100000;
    params->r_k[2] = 820;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 6531, &params->lowpass[2]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 194, &params->biaslowpass[2]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[2]);
    
    params->r_i[3] = 250e3;
    params->r_p[3] = 100000;
    params->r_k[3] = 410;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 6531, &params->lowpass[3]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 250, &params->biaslowpass[3]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[3]);
    
    return p;
}
