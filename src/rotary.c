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
#include "backbuf.h"

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
    float fftbuf[FFT_SIZE * 2];
    float pha, sinval, cosval;
    
    if (db->channels != 1)
        return;
    
    params->phase += (float) db->len / db->channels / sample_rate * 1000.0 / params->speed;
    if (params->phase >= 1.0)
        params->phase -= 1.0;

    pha = params->phase;
    sinval = sin_lookup(pha);
    pha += 0.25;
    if (pha >= 1.0)
        pha -= 1.0;
    cosval = sin_lookup(pha);

    for (i = 0; i < db->len; i += 1) {
        params->history->add(params->history, db->data[i]);
        if (params->time_to_next_fft > 0) {
            params->time_to_next_fft -= 1;
            continue;
        }
        /* we do four FFTs on every input sample */
        params->time_to_next_fft = FFT_SIZE / 2 - 1;

        /* fill FFT buffer with data from history */
        for (j = 0; j < FFT_SIZE; j += 1) {
            fftbuf[j * 2] = params->history->get(params->history, FFT_SIZE - j);
            fftbuf[j * 2 + 1] = 0;
        }
        /* this is rather dumb because it's same as history -- I'll optimize it later */
        for (j = 0; j < FFT_SIZE / 2; j += 1)
            params->norm->add(params->norm, fftbuf[(j + FFT_SIZE/2 - FFT_SIZE/4) * 2]);
        
        /* window it with a flat top window -- I should use a better window for this */
        for (j = 0; j < FFT_SIZE / 4; j += 1) {
            fftbuf[j * 2] *= (float) j / (FFT_SIZE / 4);
            fftbuf[(FFT_SIZE - j - 1) * 2] *= (float) j / (FFT_SIZE / 4);
        }

        /* compute spectrum */
        do_fft(fftbuf, FFT_SIZE, FFT_FORWARD);
            
        /* Hilbert transform:
         * Multiply positive frequences with -i and negatives with +i.
         *
         * We could probably do this with carefully tuned allpass delays instead,
         * or maybe some FIR that also does hilbert transform. Anyway, let's do
         * it like this for now. */
        for (j = 0; j < FFT_SIZE / 2; j += 1) {
            float re = fftbuf[j * 2];
            float im = fftbuf[j * 2 + 1];

            fftbuf[j * 2] = im;
            fftbuf[j * 2 + 1] = -re;
        }
        for (j = FFT_SIZE / 2; j < FFT_SIZE; j += 1) {
            float re = fftbuf[j * 2];
            float im = fftbuf[j * 2 + 1];
                
            fftbuf[j * 2] = -im;
            fftbuf[j * 2 + 1] = re;
        }
            
        /* now we have hilbert transformed signal */
        do_fft(fftbuf, FFT_SIZE, FFT_INVERSE);

        /* obtain the 1/4th of the inverse for output */
        for (j = 0; j < FFT_SIZE / 2; j += 1)
            params->hilb->add(params->hilb, fftbuf[(j + FFT_SIZE/2 - FFT_SIZE/4) * 2] / FFT_SIZE);
        params->unread_output += FFT_SIZE / 2;
    }
    /* this is a hack -- we probably could compute the right buffer offset. This only
     * hurts us on the first few buffer runs, later this condition will not trigger */
    if (params->unread_output < db->len)
        params->unread_output = db->len;
    
    /* Rotary effect
     * We obtain two outputs and shift their frequency bands like this: */
    db->channels = 2;
    db->len *= 2;

    for (i = 0; i < db->len/2; i += 1) {
        DSP_SAMPLE x0 = params->norm->get(params->norm, params->unread_output - i);
        DSP_SAMPLE x1 = params->hilb->get(params->hilb, params->unread_output - i);
        DSP_SAMPLE y0 = cosval * x0 + sinval * x1;
        DSP_SAMPLE y1 = cosval * x0 - sinval * x1;

        /* factor and biquad estimate hrtf */
        db->data[i*2+0] = 0.68 * y0 + 0.32 * do_biquad(y1, &params->ld, 0);
        db->data[i*2+1] = 0.68 * y1 + 0.32 * do_biquad(y0, &params->rd, 0);
        
        /* this code would implement stereo phaser
        db->data[i*2+0] = (sinval * y0 + cosval * y1);
        db->data[i*2+1] = (cosval * y0 + sinval * y1);
         */
    }

    
    params->unread_output -= db->len/2;

    return;
}

static void
rotary_done(struct effect *p)
{
    struct rotary_params *params = p->params;
    
    del_Backbuf(params->history);
    del_Backbuf(params->hilb);
    del_Backbuf(params->norm);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
rotary_save(struct effect *p, SAVE_ARGS)
{
    //struct rotary_params *params = p->params;

    return;
}

static void
rotary_load(struct effect *p, LOAD_ARGS)
{
    //struct rotary_params *params = p->params;

    return;
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
    params->history = new_Backbuf(FFT_SIZE);
    params->hilb = new_Backbuf(MAX_BUFFER_SIZE);
    params->norm = new_Backbuf(MAX_BUFFER_SIZE);
    params->unread_output = 0;

    set_rc_lowpass_biquad(sample_rate, 7000, &params->ld);
    set_rc_lowpass_biquad(sample_rate, 7000, &params->rd);
    
    return p;
}
