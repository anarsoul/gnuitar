/*
 * GNUitar
 * Reverberation effect
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include "reverb.h"
#include "gui.h"

#define MAX_REVERB_SIZE  3000 /* ms */

static void
update_reverb_drywet(GtkAdjustment *adj, struct reverb_params *params)
{
    params->drywet = adj->value;
}

static void
update_reverb_delay(GtkAdjustment *adj, struct reverb_params *params)
{
    params->delay = adj->value;
}

static void
update_reverb_regen(GtkAdjustment *adj, struct reverb_params *params)
{
    params->regen = adj->value;
}

static void
reverb_init(struct effect *p)
{
    struct reverb_params *preverb;

    GtkWidget      *drywet;
    GtkWidget      *drywet_label;
    GtkObject      *adj_drywet;

    GtkWidget      *delay;
    GtkWidget      *delay_label;
    GtkObject      *adj_delay;

    GtkWidget      *regen;
    GtkWidget      *regen_label;
    GtkObject      *adj_regen;

    GtkWidget      *button;

    GtkWidget      *parmTable;

    preverb = (struct reverb_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(4, 3, FALSE);

    adj_delay = gtk_adjustment_new(preverb->delay, 1.0, MAX_REVERB_SIZE, 1.0, 1.0, 0.0);
    delay_label = gtk_label_new("delay\nms");
    gtk_table_attach(GTK_TABLE(parmTable), delay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_delay), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_delay), preverb);

    delay = gtk_vscale_new(GTK_ADJUSTMENT(adj_delay));
    gtk_widget_set_size_request(GTK_WIDGET(delay),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), delay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_drywet = gtk_adjustment_new(preverb->drywet, 0.0, 100.0, 1.0, 1.0, 0.0);
    drywet_label = gtk_label_new("Dry/Wet\n%");
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_drywet), preverb);

    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));

    gtk_table_attach(GTK_TABLE(parmTable), drywet, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_regen = gtk_adjustment_new(preverb->regen, 0.0, 100.0, 1.0, 1.0, 0.0);
    regen_label = gtk_label_new("regen\n%");
    gtk_table_attach(GTK_TABLE(parmTable), regen_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_regen), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_regen), preverb);

    regen = gtk_vscale_new(GTK_ADJUSTMENT(adj_regen));

    gtk_table_attach(GTK_TABLE(parmTable), regen, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS
		     (GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Reverberator"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

/* backbuf-based allpass filter for longer than 1 or 2 sample delays.
 * This */
static double
allpass_filter(double input, double factor, int delay, Backbuf_t *history)
{
    double tmp, output;

    // delay - 1 because 0th is already the previous, so someone asking
    // for the previous sample is really asking for the 0th sample until
    // add() is performed
    tmp = input - factor * history->get(history, delay - 1);
    output = history->get(history, delay - 1) + factor * tmp;
    history->add(history, tmp);
    return output;
}

static double
comb_filter(double input, double factor, int delay, Backbuf_t *history)
{
    double output;
    history->add(history, input); // 0th is current, -1 is previous
    output = input + factor * history->get(history, delay);
    return output;
}

static void
reverb_filter(struct effect *p, data_block_t *db)
{
    struct reverb_params *params = p->params;
    DSP_SAMPLE     *s;
    int             count,
                    c = 0; /* curr_channel */
    float           input, a, mono, Dry, Wet, Rgn, Delay, fsr;

    s = db->data;
    count = db->len;

    Delay = params->delay / 1000.0 * sample_rate;
    Wet = params->drywet / 100.0;
    Dry = 1 - Wet;
    Rgn = params->regen / 100.0;
    
    /* This is a John Chowning reverberator, explained here:
     * http://www-ccrma.stanford.edu/~jos/waveguide/Schroeder_Reverberator_called_JCRev.html
     *
     * Signal block diagram is as follows:
     *
     * input--[ ap(0.7, 1051) ]--[ ap(0.7, 337)]--[ ap(0.7, 113) ]--a
     *
     *    +--[ comb(0.742, 4799) ]--+
     *    |                         |
     *    +--[ comb(0.733, 4999) ]--+
     * a--+                        (+)--mono
     *    +--[ comb(0.715, 5399) ]--+
     *    |                         |
     *    +--[ comb(0.697, 5801) ]--+
     * 
     * In case multichannel output is desired, the output is further processed
     * with:
     *
     *       +--[ delay(0.046 * sample_rate) ]--ch1
     *       |                                  
     *       +--[ delay(0.057 * sample_rate) ]--ch2
     * mono--+
     *       +--[ delay(0.041 * sample_rate) ]--ch3
     *       |
     *       +--[ delay(0.054 * sample_rate) ]--ch4
     *
     * For stereo output, the channels 1, 2 and 3, 4 are averaged.
     */

    fsr = sample_rate / 44100;
    while (count) {
        input = *s;

        /* change from original: infinite reverb through history */
        input += params->history[c]->get(params->history[c], Delay) * Rgn;
        
        a = allpass_filter(input, 0.7, 1051 * fsr, params->ap[c][0]);
        a = allpass_filter(a,     0.7,  337 * fsr, params->ap[c][1]);
        a = allpass_filter(a,     0.7,  113 * fsr, params->ap[c][2]);
        
        mono  = comb_filter(a, 0.742, 4799 * fsr, params->comb[c]);
        mono += comb_filter(a, 0.733, 4999 * fsr, params->comb[c]);
        mono += comb_filter(a, 0.715, 5399 * fsr, params->comb[c]);
        mono += comb_filter(a, 0.697, 5801 * fsr, params->comb[c]);
        /* change from original: some more reverb terms */
        mono += comb_filter(a, 0.75, 3533 * fsr, params->comb[c]);
        mono += comb_filter(a, 0.65, 6521 * fsr, params->comb[c]);
        
        params->history[c]->add(params->history[c], mono / 6.0);
        
        *s = *s * Dry + mono * Wet;

        c = (c + 1) % db->channels;
        s++;
        count--;
    }
}

static void
reverb_done(struct effect *p)
{
    struct reverb_params *dr;
    int             i;
    
    dr = (struct reverb_params *) p->params;

    for (i = 0; i < MAX_CHANNELS; i += 1) {
        free(dr->history[i]);
        del_Backbuf(dr->ap[i][0]);
        del_Backbuf(dr->ap[i][1]);
        del_Backbuf(dr->ap[i][2]);
        del_Backbuf(dr->comb[i]);
    }
    
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
reverb_save(struct effect *p, SAVE_ARGS)
{
    struct reverb_params *params = p->params;

    SAVE_DOUBLE("drywet", params->drywet);
    SAVE_DOUBLE("regen", params->regen);
    SAVE_DOUBLE("delay", params->delay);
}

static void
reverb_load(struct effect *p, LOAD_ARGS)
{
    struct reverb_params *params = p->params;

    LOAD_DOUBLE("drywet", params->drywet);
    LOAD_DOUBLE("regen", params->regen);
    LOAD_DOUBLE("delay", params->delay);
}

effect_t *
reverb_create()
{
    effect_t       *p;
    struct reverb_params *dr;
    int             i;
    
    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct reverb_params));

    p->proc_init = reverb_init;
    p->proc_filter = reverb_filter;
    p->toggle = 0;
    p->proc_done = reverb_done;
    p->proc_load = reverb_load;
    p->proc_save = reverb_save;
    dr = (struct reverb_params *) p->params;
    for (i = 0; i < MAX_CHANNELS; i += 1) {
        dr->history[i] = new_Backbuf(MAX_REVERB_SIZE / 1000.0 * MAX_SAMPLE_RATE);
        dr->ap[i][0] = new_Backbuf(2048);
        dr->ap[i][1] = new_Backbuf(2048);
        dr->ap[i][2] = new_Backbuf(2048);
        dr->comb[i] = new_Backbuf(8192);
    }
    dr->delay  = 130;    /* ms */
    dr->drywet = 25.0;
    dr->regen  = 25.0;
    return p;
}
