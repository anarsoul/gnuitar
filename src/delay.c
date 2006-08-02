/*
 * GNUitar
 * Delay effect
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
 *
 * $Log$
 * Revision 1.30  2006/08/02 19:07:56  alankila
 * - add missing static declarations
 *
 * Revision 1.29  2005/09/06 14:54:31  alankila
 * - set button states at loadup
 * - make echo multichannel aware. Echo currently can do almost everything
 *   reverb can do, so we could remove reverb.
 *
 * Revision 1.28  2005/09/05 20:07:49  alankila
 * - multichannel chorus
 * - add some code to synchronize output volumes regardless of voices #
 *   based on the random walk theorem
 *
 * Revision 1.27  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.26  2005/09/04 16:06:59  alankila
 * - first multichannel effect: delay
 * - need to use surround40 driver in alsa
 * - introduce new buffer data_swap so that effects need not reserve buffers
 * - correct off-by-one error in multichannel adapting
 *
 * Revision 1.25  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.24  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.23  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.22  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.21  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.20  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.19  2005/09/01 23:16:10  alankila
 * - make delay params independent of sampling rate
 *
 * Revision 1.18  2005/09/01 17:31:40  alankila
 * - various small fixes for multichannel / gui
 *
 * Revision 1.17  2005/09/01 14:18:54  alankila
 * - drop fractions in delay count
 *
 * Revision 1.16  2005/09/01 14:09:56  alankila
 * - multichannel work: delay independent of nchannels; uses backbuf instead
 *   of doing it all on its own. Also fixes bugs with delay load/save.
 *
 * Revision 1.15  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.14  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.13  2005/08/11 17:57:21  alankila
 * - add some missing headers & fix all compiler warnings on gcc 4.0.1+ -Wall
 *
 * Revision 1.12  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.11  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.10  2003/03/14 19:20:31  fonin
 * Unreferenced variables fixed.
 *
 * Revision 1.9  2003/03/12 20:53:54  fonin
 * - meaningful sliders measure units;
 * - code cleanup.
 *
 * Revision 1.8  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.7  2003/02/01 19:15:12  fonin
 * Use sizeof(variable) instead sizeof(type) in load/save procedures,
 * when reading/writing from file.
 *
 * Revision 1.6  2003/01/31 19:45:45  fonin
 * Better effect presets.
 *
 * Revision 1.5  2003/01/30 21:35:29  fonin
 * Got rid of rnd_window_pos().
 *
 * Revision 1.4  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.3  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.2  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:26  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "backbuf.h"
#include "delay.h"
#include "gui.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

/* used to swap rear channels */
static const int circular_order[] = { 0, 1, 3, 2 };

static void
update_delay_decay(GtkAdjustment *adj, struct delay_params *params)
{
    int             i;
    params->delay_decay = adj->value;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        params->history[i]->clear(params->history[i]);
}

static void
update_delay_time(GtkAdjustment *adj, struct delay_params *params)
{
    int             i;
    params->delay_time = adj->value;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        params->history[i]->clear(params->history[i]);
}

static void
update_delay_repeat(GtkAdjustment *adj, struct delay_params *params)
{
    params->delay_count = adj->value;
}

static void
toggle_delay_multichannel(void *bullshit, struct delay_params *params)
{
    params->multichannel = !params->multichannel;
}

static void
delay_init(struct effect *p)
{
    struct delay_params *pdelay;

    GtkWidget      *decay;
    GtkWidget      *decay_label;
    GtkObject      *adj_decay;

    GtkWidget      *time;
    GtkWidget      *time_label;
    GtkObject      *adj_time;

    GtkWidget      *repeat;
    GtkWidget      *repeat_label;
    GtkObject      *adj_repeat;

    GtkWidget      *button, *mcbutton;

    GtkWidget      *parmTable;


    pdelay = (struct delay_params *) p->params;


    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(3, 3, FALSE);

    adj_decay = gtk_adjustment_new(pdelay->delay_decay,
				   10.0, 100.0, 1.0, 1.0, 0.0);
    decay_label = gtk_label_new("Decay\n%");
    gtk_table_attach(GTK_TABLE(parmTable), decay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_decay), "value_changed",
		       GTK_SIGNAL_FUNC(update_delay_decay), pdelay);

    decay = gtk_vscale_new(GTK_ADJUSTMENT(adj_decay));
    gtk_widget_set_size_request(GTK_WIDGET(decay),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), decay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_time =
	gtk_adjustment_new(pdelay->delay_time, 1.0,
			   MAX_SECONDS * 1000,
			   1.0, 1.0, 0.0);
    time_label = gtk_label_new("Time\nms");
    gtk_table_attach(GTK_TABLE(parmTable), time_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_time), "value_changed",
		       GTK_SIGNAL_FUNC(update_delay_time), pdelay);
    time = gtk_vscale_new(GTK_ADJUSTMENT(adj_time));

    gtk_table_attach(GTK_TABLE(parmTable), time, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_repeat = gtk_adjustment_new(pdelay->delay_count,
				    1.0, MAX_COUNT, 1.0, 1.0, 0.0);
    repeat_label = gtk_label_new("Repeat\ntimes");
    gtk_table_attach(GTK_TABLE(parmTable), repeat_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_repeat), "value_changed",
		       GTK_SIGNAL_FUNC(update_delay_repeat), pdelay);

    repeat = gtk_vscale_new(GTK_ADJUSTMENT(adj_repeat));
    gtk_scale_set_digits(GTK_SCALE(repeat), 0);

    gtk_table_attach(GTK_TABLE(parmTable), repeat, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    
    if (n_input_channels == 1 && n_output_channels > 1) {
        mcbutton = gtk_check_button_new_with_label("Multichannel");
        if (pdelay->multichannel)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mcbutton), TRUE);
        gtk_signal_connect(GTK_OBJECT(mcbutton), "toggled",
                           GTK_SIGNAL_FUNC(toggle_delay_multichannel), pdelay);
        gtk_table_attach(GTK_TABLE(parmTable), mcbutton, 1, 3, 2, 3,
                         __GTKATTACHOPTIONS(GTK_EXPAND |
                                            GTK_SHRINK),
                         __GTKATTACHOPTIONS(GTK_FILL |
                                            GTK_SHRINK), 0, 0);
    }

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

    gtk_window_set_title(GTK_WINDOW(p->control), "Delay");
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

static void
delay_filter_mono(effect_t *p, data_block_t *db)
{
    struct delay_params *dp = p->params;
    int             i,
                    count,
                    current_delay = 0;
    double          current_decay, delay_inc, decay_fac;
    DSP_SAMPLE     *s, newval;
    int             curr_channel = 0;

    s = db->data;
    count = db->len;

    delay_inc = dp->delay_time / 1000.0 * sample_rate;
    decay_fac = dp->delay_decay / 100.0;
    
    /* this is a simple mono version that treats all channels separately */
    while (count) {
        newval = 0;
        current_delay = 0;
        current_decay = 1.0;
        for (i = 0; i < dp->delay_count; i += 1) {
            current_delay += delay_inc;
            current_decay *= decay_fac;

            newval += dp->history[curr_channel]->get(dp->history[curr_channel], current_delay) * current_decay;
	}
        
        /* write to history, add decay to current sample */
        dp->history[curr_channel]->add(dp->history[curr_channel], *s);
        *s += newval;

        curr_channel = (curr_channel + 1) % db->channels;
	s++;
	count--;
    }
}

/* this is a mono-to-N channel mixer */
static void
delay_filter_mc(effect_t *p, data_block_t *db)
{
    struct delay_params *dp = p->params;
    int             i,
                    count,
                    current_delay = 0;
    double          current_decay, delay_inc, decay_fac;
    DSP_SAMPLE     *ins, *outs, newval;
    int             curr_channel = 0;

    /* this is only good for mono and up to 4 channels */
    assert(db->channels == 1);
    assert(n_output_channels <= 4);
    
    /* fill into provided output buffer */
    ins = db->data;
    outs = db->data_swap;
    /* already swap the buffers for caller */
    db->data = outs;
    db->data_swap = ins;
    
    count = db->len;

    db->channels = n_output_channels;
    db->len *= n_output_channels;
    
    delay_inc = dp->delay_time / 1000.0 * sample_rate;
    decay_fac = dp->delay_decay / 100.0;
    
    while (count) {
        /* delays are distributed across channels from history */
        dp->history[0]->add(dp->history[0], *ins++);

        for (curr_channel = 0; curr_channel < db->channels; curr_channel += 1) {
            current_delay = 0;
            current_decay = 1.0;
            newval = 0;
	    /* +1 because we mix even the current data through history */
            for (i = 0; i < dp->delay_count + 1; i += 1) {
                /* mix only every Nth voice */
                if (circular_order[i % db->channels] == curr_channel)
		    newval += dp->history[0]->get(dp->history[0], current_delay) * current_decay;
                current_delay += delay_inc;
                current_decay *= decay_fac;
            }
            *outs++ = newval;
        }
        
	count--;
    }
}

static void
delay_filter(effect_t *p, data_block_t *db)
{
    struct delay_params *params = p->params;
    if (params->multichannel && db->channels == 1 && n_output_channels > 1) {
        delay_filter_mc(p, db);
    } else {
	delay_filter_mono(p, db);
    }
}

static void
delay_done(struct effect *p)
{
    struct delay_params *dp;
    int i;

    dp = (struct delay_params *) p->params;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        del_Backbuf(dp->history[i]);

    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
delay_save(struct effect *p, SAVE_ARGS)
{
    struct delay_params *params = p->params;

    SAVE_DOUBLE("delay_decay", params->delay_decay);
    SAVE_DOUBLE("delay_time", params->delay_time);
    SAVE_INT("delay_count", params->delay_count);
    SAVE_INT("multichannel", params->multichannel);
}

static void
delay_load(struct effect *p, LOAD_ARGS)
{
    struct delay_params *params = p->params;

    LOAD_DOUBLE("delay_decay", params->delay_decay);
    LOAD_DOUBLE("delay_time", params->delay_time);
    LOAD_INT("delay_count", params->delay_count);
    LOAD_INT("multichannel", params->multichannel);
}

effect_t *
delay_create()
{
    effect_t           *p;
    struct delay_params *pdelay;
    int                 i;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct delay_params));
    pdelay = p->params;
    p->proc_init = delay_init;
    p->proc_filter = delay_filter;
    p->toggle = 0;
    p->proc_done = delay_done;
    p->proc_save = delay_save;
    p->proc_load = delay_load;

    /*
     * 
     * Brian May echo stuff delay_start=delay_step=35000 delay_count=2
     * 
     * Diablo town music delay_start=delay_step=25000 delay_count=3
     * 
     */

    pdelay->multichannel = 0;
    pdelay->delay_decay = 55;
    pdelay->delay_time = 1000;
    pdelay->delay_count = 8;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        pdelay->history[i] = new_Backbuf(MAX_SIZE);
    return p;
}
