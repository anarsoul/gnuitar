/*
 * GNUitar
 * Pitch shifter effect
 * Copyright (c) 2005 Antti S. Lankila  <alankila@bel.fi>
 *
 * See COPYING about details for license.
 *
 * This work is based on Tom Szilagyi's tap-plugin's pitch shifter effect.
 *
 * In short, the pitch shifter works by compressing or expanding the audio data
 * over short intervals (interval length defined by PITCH_MODULATION_FREQUENCY)
 * and summing these fragments into output signal modulated with a windowing
 * function that in this case is simply sin(x) over 0 <= x <= pi.
 *
 * To control phase effects, some number of these fragments is required, and
 * the optimum seems to be 3.
 *
 * When shifting up, the algorithm produces echoing artifacts. These echoes
 * would be reduced by increasing the modulation frequency, but this cuts
 * into the bass. An alternative solution might be found with a different
 * windowing function.
 *
 * $Id$
 */

#include "pitch.h"
#include "glib12-compat.h"
#include "gui.h"
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#define PITCH_PHASES                3
#define PITCH_MODULATION_FREQUENCY  6 /* Hz */
#define PITCH_BUFFER_SIZE           (MAX_SAMPLE_RATE / PITCH_MODULATION_FREQUENCY)

void
update_pitch_halfnote(GtkAdjustment *adj, struct pitch_params *params)
{
    params->halfnote = adj->value;
}

void
update_pitch_finetune(GtkAdjustment *adj, struct pitch_params *params)
{
    params->finetune = adj->value;
}

void
update_pitch_drywet(GtkAdjustment *adj, struct pitch_params *params)
{
    params->drywet = adj->value;
}


void
pitch_init(struct effect *p)
{
    struct pitch_params *params = p->params;

    GtkWidget      *halfnote_label;
    GtkWidget      *halfnote;
    GtkObject      *adj_halfnote;

    GtkWidget      *finetune_label;
    GtkWidget      *finetune;
    GtkObject      *adj_finetune;

    GtkWidget      *drywet_label;
    GtkWidget      *drywet;
    GtkObject      *adj_drywet;

    GtkWidget      *parmTable, *button;
    
    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(3, 3, FALSE);

    adj_halfnote = gtk_adjustment_new(params->halfnote, -12.0,
                               12.0, 1.0, 1.0, 0.0);
    halfnote_label = gtk_label_new("Halfnotes");
    gtk_table_attach(GTK_TABLE(parmTable), halfnote_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_halfnote), "value_changed",
		       GTK_SIGNAL_FUNC(update_pitch_halfnote), params);

    halfnote = gtk_vscale_new(GTK_ADJUSTMENT(adj_halfnote));
    gtk_scale_set_digits(GTK_SCALE(halfnote), 0);
    gtk_widget_set_size_request(GTK_WIDGET(halfnote),0,100);
    gtk_table_attach(GTK_TABLE(parmTable), halfnote, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_finetune = gtk_adjustment_new(params->finetune,
				     -0.5, 0.5, 0.1, 0.1, 0.0);
    finetune_label = gtk_label_new("Finetune");
    gtk_table_attach(GTK_TABLE(parmTable), finetune_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_finetune), "value_changed",
		       GTK_SIGNAL_FUNC(update_pitch_finetune), params);

    finetune = gtk_vscale_new(GTK_ADJUSTMENT(adj_finetune));
    gtk_scale_set_digits(GTK_SCALE(finetune), 2);
    gtk_table_attach(GTK_TABLE(parmTable), finetune, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_drywet = gtk_adjustment_new(params->drywet,
				     0, 100.0, 1.0, 1.0, 0.0);
    drywet_label = gtk_label_new("Dry / Wet");
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_pitch_drywet), params);

    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));
    gtk_table_attach(GTK_TABLE(parmTable), drywet, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Pitch shift"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
pitch_filter(effect_t *p, data_block_t *db)
{
    struct pitch_params *params = p->params;
    DSP_SAMPLE     *s, tmp;
    double          phase_inc, phase_tmp, Dry, Wet;
    double          depth = 0;
    int             count, c = 0, dir = 0, i;

    s = db->data;
    count = db->len;

    depth = pow(2, (params->halfnote + params->finetune) / 12) - 1;
    if (depth < -0.5)
       depth = -0.5;
    if (depth > 1.0)
       depth = 1.0;
    depth = depth * sample_rate / PITCH_MODULATION_FREQUENCY;
    if (depth < 0) {
        depth = -depth;
        dir = 1;
    }

    Dry = 1 - params->drywet / 100.0;
    Wet =     params->drywet / 100.0;
 
    phase_inc = (double) PITCH_MODULATION_FREQUENCY / sample_rate;
    while (count) {
        params->history[c]->add(params->history[c], *s);

        tmp = 0;
        phase_tmp = params->phase;
        for (i = 0; i < PITCH_PHASES; i += 1) {
            /* repeatedly weigh pieces of "accelerated" history through
             * a windowing function */
            tmp += sin_lookup(phase_tmp / 2) *
                params->history[c]->get_interpolated(params->history[c], 
                        depth * (dir ? phase_tmp : 1 - phase_tmp));
            phase_tmp += 1.0 / PITCH_PHASES;
            if (phase_tmp >= 1.0)
                phase_tmp -= 1.0;
        }
        tmp /= sqrt(PITCH_PHASES);

        /* we take the sample midpoint between accelerated history in order
         * to reduce phase decorrelation effects, but this gives awful
         * latency to contend with */    
        *s = params->history[c]->get(params->history[c], depth / 2) * Dry + tmp * Wet;
        c = (c + 1) % db->channels;
        if (c == 0) {
            params->phase += phase_inc;
            if (params->phase >= 1.0)
                params->phase -= 1.0;
        }
        s++;
        count--;
    }

}

void
pitch_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

void
pitch_save(effect_t *p, SAVE_ARGS)
{
    struct pitch_params *params = p->params;

    SAVE_INT("halfnote", params->halfnote);
    SAVE_DOUBLE("finetune", params->finetune);
    SAVE_DOUBLE("drywet", params->drywet);
}

void
pitch_load(effect_t *p, LOAD_ARGS)
{
    struct pitch_params *params = p->params;
    
    LOAD_INT("halfnote", params->halfnote);
    LOAD_DOUBLE("finetune", params->finetune);
    LOAD_DOUBLE("drywet", params->drywet);
}

effect_t *
pitch_create()
{
    effect_t           *p;
    struct pitch_params *params;
    int                 i;
    
    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct pitch_params));
    p->proc_init = pitch_init;
    p->proc_filter = pitch_filter;
    p->toggle = 0;
    p->proc_done = pitch_done;
    p->proc_save = pitch_save;
    p->proc_load = pitch_load;

    params = p->params;
    for (i = 0; i < MAX_CHANNELS; i += 1) {
        params->history[i] = new_Backbuf(PITCH_BUFFER_SIZE);
    }
    params->drywet = 50;
    return p;
}
