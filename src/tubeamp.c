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
    o = gtk_adjustment_new(params->gain, 3.0, 20.0, 0.1, 1, 0);
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

static void
tubeamp_filter(struct effect *p, struct data_block *db)
{
    int i, j, k, curr_channel = 0;
    struct tubeamp_params *params = p->params;
    float gain;

    for (i = 0; i < MAX_STAGES; i += 1) {
        set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 10, &params->highpass[i]);
        set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, params->treblefreq, &params->lowpass[i]);
        set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, params->biasfreq, &params->biaslowpass[i]);
        set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, params->lsfreq, &params->lowshelf[i]);
        set_peq_biquad(sample_rate * UPSAMPLE_RATIO, 800, 500.0, params->middlecut, &params->middlecut_bq[i]);
    }

    gain = pow(10, params->gain / 20);
    
    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < db->len; i += 1) {
        float result;
        for (k = 0; k < UPSAMPLE_RATIO; k += 1) {
            /* IIR interpolation */
            params->in[curr_channel] = (db->data[i] + params->in[curr_channel] * 3) / 4.0;
#define DISTORTION_AMOUNT (MAX_SAMPLE * 2.0)
            result = params->in[curr_channel] / DISTORTION_AMOUNT * gain;
            for (j = 0; j < params->stages; j += 1) {
                /* highpass filter to remove offset from earlier pass */
//                result = do_biquad(result, &params->highpass[j], curr_channel);
                /* middlecut for user tone control, for the "metal crunch" sound */
                result = do_biquad(result, &params->middlecut_bq[j], curr_channel);
                /* low shelve to remove bass to avoid saturating the sound */
                result = 0.5 * result + 0.5 * do_biquad(result, &params->lowshelf[j], curr_channel);
                /* waveshaper adds high-frequency components */
                result += do_biquad(tanh(result + params->bias[j]), &params->highpass[j], curr_channel) * gain;
                /* bias calculation creates a feedback loop with the distortion, making
                 * the distort react more to sound dynamics. */
                params->bias[j] = do_biquad((0.25 + 0.5 * (j+1) / params->stages) - 0.25 * result, &params->biaslowpass[j], curr_channel);
                /* lowpass filter keeps the added high-frequency components in control */
                result = do_biquad(result, &params->lowpass[j], curr_channel);
                /* each stage inverts in a real tube amp */
                result = -result;
            }
            /* final tone control, a poor man's cabinet simulation */
            result = do_biquad(result, &params->final_highpass, curr_channel);
            result = do_biquad(result, &params->final_lowpass, curr_channel);
        }
        db->data[i] = result * DISTORTION_AMOUNT / gain / params->stages / 4;
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
    struct tubeamp_params *params = p->params;
    SAVE_INT("stages", params->stages);
    SAVE_DOUBLE("gain", params->gain);
    SAVE_DOUBLE("lsfreq", params->lsfreq);
    SAVE_DOUBLE("treblefreq", params->treblefreq);
    SAVE_DOUBLE("middlecut", params->middlecut);
}

static void
tubeamp_load(struct effect *p, LOAD_ARGS)
{
    struct tubeamp_params *params = p->params;
    LOAD_INT("stages", params->stages);
    LOAD_DOUBLE("gain", params->gain);
    LOAD_DOUBLE("lsfreq", params->lsfreq);
    LOAD_DOUBLE("treblefreq", params->treblefreq);
    LOAD_DOUBLE("middlecut", params->middlecut);
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

    params->stages = 5;
    params->gain = 15.0;
    params->lsfreq = 120;
    params->treblefreq = 5000;
    params->biasfreq = 10;
    params->middlecut = -4.0;

    /* low-end cabinet simulation: 6 kHz cut and 20 Hz cut */ 
    set_chebyshev1_biquad(sample_rate * UPSAMPLE_RATIO, 6000, 1.0, TRUE, &params->final_lowpass);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 20, &params->final_highpass);
    
    return p;
}
