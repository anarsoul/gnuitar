/*
 * GNUitar
 * Rotary speaker simulation -- a stereo effect
 * Copyright (C) 2006 Antti S. Lankila  <alankila@bel.fi>
 *
 * GPL license.
 *
 * $Id$
 */

#include <math.h>
#include <gui.h>
#include <stdlib.h>

#include "biquad.h"
#include "fft.h"
#include "pump.h"
#include "rotary.h"

static void
update_speed(GtkAdjustment *adj, struct rotary_params *params)
{
    params->speed = adj->value;
}

static void
rotary_init(struct effect *p)
{
    GtkWidget      *w;
    GtkObject      *o;
    GtkWidget      *parmTable;
    struct rotary_params *params = p->params;
    
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(1, 2, FALSE);

    w = gtk_label_new("Period\n(ms)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->speed, 250, 5000, 1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_speed), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_scale_set_digits(GTK_SCALE(w), 0);
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 1, 2,
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
			 (gchar *) ("Rotary speaker"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

/* this is a single-channel to two-channel effect */
static void
rotary_filter(struct effect *p, struct data_block *db)
{
    struct rotary_params *params = p->params;
    int i, j;
    float pha, sinval = 0, cosval = 0;
    
    if (db->channels != 1)
        return;
    
    params->phase += (float) db->len / sample_rate * 1000.0 / params->speed;
    if (params->phase >= 1.0)
        params->phase -= 1.0;

    /* The rotary speaker simulation is based on modulating input with subsonic
     * sinuswave and then separating the upwards and downwards shifted frequencies
     * with hilbert transform. The upwards shifted component can be thought to be
     * the horn, and the downwards shifted component the bass speaker. (After
     * a half-turn, the "bass" will shift up and the "horn" will shift down.)
     *
     * After the separation step, a rough approximation of HRTF is used to mix the
     * horn with the bass speaker for both channels. */
    db->channels = 2;
    db->len *= 2;

    pha = params->phase;
    for (i = 0; i < db->len/2; i += 1) {
        /* update the approximation of sin and cos values to avoid
         * discontinuities between audio blocks */
        if (i % 16 == 0) {
            float phatmp;
            sinval = sin_lookup(pha);
            phatmp = pha + 0.25;
            if (phatmp >= 1.0)
                phatmp -= 1.0;
            cosval = sin_lookup(phatmp);
            pha += (float) 16 / sample_rate * 1000.0 / params->speed;
            if (pha >= 1.0)
                pha -= 1.0;
        }

        /* run the input through allpass delay lines */
        DSP_SAMPLE x0 = db->data[i];
        DSP_SAMPLE x1 = x0;
        for (j = 0; j < 4; j += 1) {
            x0 = do_biquad(x0, &params->a1[j], 0);
            x1 = do_biquad(x1, &params->a2[j], 0);
        }
        /* additionally, x0 needs to be delayed by 1 sample */
        DSP_SAMPLE x0_tmp = x0;
        x0 = params->x0_tmp;
        params->x0_tmp = x0_tmp;
        
        /* compute separate f + fc and f - fc outputs */
        DSP_SAMPLE y0 = cosval * x0 + sinval * x1;
        DSP_SAMPLE y1 = cosval * x0 - sinval * x1;

        /* factor and biquad estimate hrtf */
        db->data_swap[i*2+0] = 0.68 * y0 + 0.32 * do_biquad(y1, &params->ld, 0);
        db->data_swap[i*2+1] = 0.68 * y1 + 0.32 * do_biquad(y0, &params->rd, 0);
        
        /* This code would implement cool stereo phaser.
         * Unfortunately it doesn't belong here but in phasor.c... :-/
        db->data_swap[i*2+0] = (cosval * y0 + sinval * y1);
        db->data_swap[i*2+1] = (cosval * y0 + sin_lookup(1.0-pha) * y1);
        */
    }
    
    /* swap to processed buffer for next effect */
    DSP_SAMPLE *tmp = db->data;
    db->data = db->data_swap;
    db->data_swap = tmp;
}

static void
rotary_done(struct effect *p)
{
    //struct rotary_params *params = p->params;
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
rotary_save(struct effect *p, SAVE_ARGS)
{
    struct rotary_params *params = p->params;
    SAVE_INT("speed", params->speed);
}

static void
rotary_load(struct effect *p, LOAD_ARGS)
{
    struct rotary_params *params = p->params;
    LOAD_INT("speed", params->speed);
}

effect_t *
rotary_create()
{
    effect_t   *p;
    struct rotary_params *params;

    p = calloc(1, sizeof(effect_t)); 
    params = p->params = calloc(1, sizeof(struct rotary_params));
    p->proc_init = rotary_init;
    p->proc_filter = rotary_filter;
    p->proc_save = rotary_save;
    p->proc_load = rotary_load;
    p->toggle = 0;
    p->proc_done = rotary_done;

    params->speed = 1000;
    params->unread_output = 0;

    set_rc_lowpass_biquad(sample_rate, 4000, &params->ld);
    set_rc_lowpass_biquad(sample_rate, 4000, &params->rd);

    /* Setup allpass sections to produce hilbert transform.
     * There value were searched with a genetic algorithm by
     * Olli Niemitalo <o@iki.fi>
     * 
     * http://www.biochem.oulu.fi/~oniemita/dsp/hilbert/
     *
     * The difference between the outputs of passing signal through
     * a1 allpass delay + 1 sample delay and a2 allpass delay
     * is shifted by 90 degrees over 99 % of the frequency band.
     */
    set_2nd_allpass_biquad(0.6923878, &params->a1[0]);
    set_2nd_allpass_biquad(0.9306054, &params->a1[1]);
    set_2nd_allpass_biquad(0.9882295, &params->a1[2]);
    set_2nd_allpass_biquad(0.9987488, &params->a1[3]);

    set_2nd_allpass_biquad(0.4021921, &params->a2[0]);
    set_2nd_allpass_biquad(0.8561711, &params->a2[1]);
    set_2nd_allpass_biquad(0.9722910, &params->a2[2]);
    set_2nd_allpass_biquad(0.9952885, &params->a2[3]);
    
    return p;
}