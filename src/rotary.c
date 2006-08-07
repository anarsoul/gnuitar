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
#include "gui.h"
#include <stdlib.h>

#include "biquad.h"
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
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->speed, 200, 5000, 1, 1, 0);
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
rotary_filter(struct effect *p, data_block_t *db)
{
    struct rotary_params *params = p->params;
    int i;
    float pha, sinval = 0, cosval = 0;
    DSP_SAMPLE *tmp;
    
    if (db->channels != 1 || n_output_channels < 2)
        return;
    
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
        DSP_SAMPLE x0, x1, y0, y1;

        /* update the approximation of sin and cos values to avoid
         * discontinuities between audio blocks */
        if (i % 8 == 0) {
            sinval = sin_lookup(pha);
            cosval = cos_lookup(pha);
            pha += (float) 16 / sample_rate * 1000.0 / params->speed;
            // params->speed;
            if (pha >= 1.0)
                pha -= 1.0;
        }

        /* compute separate f + fc and f - fc outputs */
        hilbert_transform(db->data[i], &x0, &x1, &params->hilb, 0);
        y0 = cosval * x0 + sinval * x1;
        y1 = cosval * x0 - sinval * x1;

        /* factor and biquad estimate hrtf */
        db->data_swap[i*2+0] = 0.60 * y0 + 0.40 * do_biquad(y1, &params->ld, 0);
        db->data_swap[i*2+1] = 0.60 * y1 + 0.40 * do_biquad(y0, &params->rd, 0);
    }

    params->phase += (float) db->len / sample_rate * 1000.0 / params->speed;
    if (params->phase >= 1.0)
        params->phase -= 1.0;

    tmp = db->data;
    db->data = db->data_swap;
    db->data_swap = tmp;
}

static void
rotary_done(struct effect *p)
{
    gnuitar_free(p->params);
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
    params = p->params = gnuitar_memalign(1, sizeof(struct rotary_params));
    p->proc_init = rotary_init;
    p->proc_filter = rotary_filter;
    p->proc_save = rotary_save;
    p->proc_load = rotary_load;
    p->toggle = 0;
    p->proc_done = rotary_done;

    params->speed = 1000;
    params->unread_output = 0;

    set_rc_lowpass_biquad(sample_rate, 2000, &params->ld);
    set_rc_lowpass_biquad(sample_rate, 2000, &params->rd);

    hilbert_init(&params->hilb);
    
    return p;
}
