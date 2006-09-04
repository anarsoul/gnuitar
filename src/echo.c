/*
 * GNUitar
 * Echo effect
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
 * Revision 1.26  2006/09/04 14:25:10  alankila
 * - hide multichannel GUI if input channels != 1
 *
 * Revision 1.25  2006/08/02 19:07:57  alankila
 * - add missing static declarations
 *
 * Revision 1.24  2005/09/16 20:40:51  alankila
 * - reduce total voice count to spare memory and constrain effect
 * - complete circular mixing and increase cross-mixing attenuation a lot
 *
 * Revision 1.23  2005/09/06 23:21:40  alankila
 * - tabularize pow, set some constants, do crossmixing, better defaults
 *
 * Revision 1.22  2005/09/06 14:54:32  alankila
 * - set button states at loadup
 * - make echo multichannel aware. Echo currently can do almost everything
 *   reverb can do, so we could remove reverb.
 *
 * Revision 1.21  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.20  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.19  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.18  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.17  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.16  2005/09/03 16:36:51  alankila
 * - reworked effect to use backbuff
 * - removed some output scaling that made this effect almost inaudible
 * - reinstated a decay based on delay % and echo delay
 * - echo_size adjustment works again
 *
 * Revision 1.15  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.14  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.13  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.12  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.11  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.10  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.9  2003/03/14 19:20:31  fonin
 * Unreferenced variables fixed.
 *
 * Revision 1.8  2003/03/12 20:53:54  fonin
 * - meaningful sliders measure units;
 * - code cleanup.
 *
 * Revision 1.7  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.6  2003/02/01 19:15:12  fonin
 * Use sizeof(variable) instead sizeof(type) in load/save procedures,
 * when reading/writing from file.
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
 * Revision 1.1.1.1  2001/01/11 13:21:41  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "echo.h"
#include "gui.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

#define ECHO_FIRST_PRIME                    20
#define ECHO_NEXT_PRIME_DISTANCE_FACTOR	    1.6
#define ECHO_CROSSMIX_ATTN                  10.0

static void
update_echo_decay(GtkAdjustment *adj, struct echo_params *params)
{
    params->echo_decay = adj->value;
}

static void
update_echo_count(GtkAdjustment *adj, struct echo_params *params)
{
    params->echoes = adj->value;
}

static void
update_echo_size(GtkAdjustment *adj, struct echo_params *params)
{
    params->echo_size = adj->value;
}

static void
toggle_echo_multichannel(void *bullshit, struct echo_params *params)
{
    params->multichannel = !params->multichannel;
}

static int
is_prime(int n)
{
    int             i;

    for (i = 2; i < n; i++)
	if (n % i == 0)
	    return 0;

    return 1;
}

static void
echo_init(struct effect *p)
{
    struct echo_params *params;

    GtkWidget      *decay;
    GtkWidget      *decay_label;
    GtkObject      *adj_decay;

    GtkWidget      *count;
    GtkWidget      *count_label;
    GtkObject      *adj_count;

    GtkWidget      *size;
    GtkWidget      *size_label;
    GtkObject      *adj_size;

    GtkWidget      *button, *mcbutton;
    GtkWidget      *parmTable;

    params = p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_decay = gtk_adjustment_new(params->echo_decay,
				   1.0, 80.0, 1.0, 10.0, 0.0);
    decay_label = gtk_label_new("Decay\n%");
    gtk_table_attach(GTK_TABLE(parmTable), decay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_decay), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_decay), params);

    decay = gtk_vscale_new(GTK_ADJUSTMENT(adj_decay));
    gtk_widget_set_size_request(GTK_WIDGET(decay),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), decay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_count = gtk_adjustment_new(params->echoes,
				   1.0, MAX_ECHO_COUNT, 1.0, 1.0, 0.0);
    count_label = gtk_label_new("Voices\n#");
    gtk_table_attach(GTK_TABLE(parmTable), count_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_count), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_count), params);

    count = gtk_vscale_new(GTK_ADJUSTMENT(adj_count));
    gtk_scale_set_digits(GTK_SCALE(count), 0);

    gtk_table_attach(GTK_TABLE(parmTable), count, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_size = gtk_adjustment_new(params->echo_size, 1.0,
                                  MAX_ECHO_LENGTH, 1.0, 1.0, 0.0);
    size_label = gtk_label_new("Delay\nms");
    gtk_table_attach(GTK_TABLE(parmTable), size_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_size), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_size), params);

    size = gtk_vscale_new(GTK_ADJUSTMENT(adj_size));

    gtk_table_attach(GTK_TABLE(parmTable), size, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    if (n_output_channels > 1 && n_input_channels == 1) {
        mcbutton = gtk_check_button_new_with_label("Multichannel");
        if (params->multichannel)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mcbutton), TRUE);
        gtk_signal_connect(GTK_OBJECT(mcbutton), "toggled",
                           GTK_SIGNAL_FUNC(toggle_echo_multichannel), params);

        gtk_table_attach(GTK_TABLE(parmTable), mcbutton, 1, 3, 2, 3,
                         __GTKATTACHOPTIONS(GTK_SHRINK),
                         __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);
    }
    
    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Echo"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}
    
static void
echo_filter_mono(effect_t *p, data_block_t *db)
{
    int                 i, count, curr_channel = 0;
    DSP_SAMPLE          *s, tmp;
    double              in, out, echo_samples, echo_decay;
    struct echo_params  *params;
    int                 delay_lookup[MAX_ECHO_COUNT];
    float               decay_lookup[MAX_ECHO_COUNT];

    s = db->data;
    count = db->len;

    params = p->params;

    echo_samples = params->echo_size / 1000.0 * sample_rate;
    echo_decay = params->echo_decay / 100.0;
    
    for (i = 0; i < params->echoes; i += 1) {
	delay_lookup[i]   = echo_samples * params->size_factor[i];
	decay_lookup[i]   = pow(echo_decay, params->decay_factor[i]);
    }
    while (count) {
        /* mix current input into the various echo buffers at their
         * expected delays */
        in = *s;
        out = in;
        for (i = 0; i < params->echoes; i += 1) {
	    tmp = params->history[curr_channel][i]->get(params->history[curr_channel][i], delay_lookup[i]) * decay_lookup[i];
            out += tmp;
            if (params->echoes > 1) {
                if (i > 1)
                    tmp += params->history[0][i-1]->get(params->history[0][i-1], delay_lookup[i-1]) * decay_lookup[i-1] / ECHO_CROSSMIX_ATTN;
                else
                    tmp += params->history[0][params->echoes-1]->get(params->history[0][params->echoes-1], delay_lookup[params->echoes-1]) * decay_lookup[params->echoes-1] / ECHO_CROSSMIX_ATTN;
            }
            params->history[curr_channel][i]->add(params->history[curr_channel][i], in + tmp);
        }
        *s = out;
        
        curr_channel = (curr_channel + 1) % db->channels; 
        s++;
        count--;
    }
}

static void
echo_filter_mc(effect_t *p, data_block_t *db)
{
    int                 i, count, curr_channel = 0;
    DSP_SAMPLE          *ins, *outs, tmp;
    double              in, out, echo_samples, echo_decay;
    struct echo_params  *params;
    int                 delay_lookup[MAX_ECHO_COUNT];
    float               decay_lookup[MAX_ECHO_COUNT];

    assert(db->channels == 1);
    ins = db->data;
    outs = db->data_swap;
    db->data = outs;
    db->data_swap = ins;
    
    count = db->len;
    
    params = p->params;
    echo_samples = params->echo_size / 1000.0 * sample_rate;
    echo_decay = params->echo_decay / 100.0;
    
    db->channels = n_output_channels;
    db->len *= n_output_channels;
    
    for (i = 0; i < params->echoes; i += 1) {
	delay_lookup[i]   = echo_samples * params->size_factor[i];
	decay_lookup[i]   = pow(echo_decay, params->decay_factor[i]);
    }
    
    while (count) {
        /* echo buffers are equally divided with all output channels.
	 * otherwise the algorithm is the same. */
        in = *ins++;
        for (curr_channel = 0; curr_channel < db->channels; curr_channel += 1) {
            out = in;
            
	    /* this for loop distributes channels equally between voices */
            for (i = curr_channel; i < params->echoes; i += db->channels) {
                tmp = params->history[0][i]->get(params->history[0][i], delay_lookup[i]) * decay_lookup[i];
                out += tmp;
	
                /* do some cross-mixing */        
                if (params->echoes > 1) {
                    if (i > 1)
                        tmp += params->history[0][i-1]->get(params->history[0][i-1], delay_lookup[i-1]) * decay_lookup[i-1] / ECHO_CROSSMIX_ATTN;
                    else
                        tmp += params->history[0][params->echoes-1]->get(params->history[0][params->echoes-1], delay_lookup[params->echoes-1]) * decay_lookup[params->echoes-1] / ECHO_CROSSMIX_ATTN;
                }
                params->history[0][i]->add(params->history[0][i], in + tmp);
            }

            *outs++ = out;
        }
        
        count--;
    }
}

static void
echo_filter(effect_t *p, data_block_t *db)
{
    struct echo_params *params = p->params;
    if (params->multichannel && db->channels == 1 && n_output_channels > 1) {
        echo_filter_mc(p, db);
    } else {
        echo_filter_mono(p, db);
    }
}

static void
echo_done(struct effect *p)
{
    struct echo_params *params;
    int             i, j;

    params = p->params;
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
        for (j = 0; j < MAX_CHANNELS; j += 1) {
            del_Backbuf(params->history[j][i]);
        }
    }

    gtk_widget_destroy(p->control);
    free(p);
}

static void
echo_save(struct effect *p, SAVE_ARGS)
{
    struct echo_params *params = p->params;

    SAVE_DOUBLE("echo_size", params->echo_size);
    SAVE_DOUBLE("echo_decay", params->echo_decay);
    SAVE_INT("echoes", params->echoes);
    SAVE_INT("multichannel", params->multichannel);
}

static void
echo_load(struct effect *p, LOAD_ARGS)
{
    struct echo_params *params = p->params;

    LOAD_DOUBLE("echo_size", params->echo_size);
    LOAD_DOUBLE("echo_decay", params->echo_decay);
    LOAD_INT("echoes", params->echoes);
    LOAD_INT("multichannel", params->multichannel);
}

effect_t *
echo_create()
{
    effect_t       *p;
    struct echo_params *params;
    int             i, j, k;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(*params));
    p->proc_init = echo_init;
    p->proc_filter = echo_filter;
    p->proc_save = echo_save;
    p->proc_load = echo_load;
    p->toggle = 0;
    p->proc_done = echo_done;

    params = p->params;
    params->multichannel = 0;
    params->echo_size = 200;
    params->echo_decay = 30.0;
    params->echoes = MAX_ECHO_COUNT;

    /* find some primes to base echo times on */
    k = ECHO_FIRST_PRIME;
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
	while (! is_prime(k))
	    k += 1;
        params->primes[i] = k;
        k *= ECHO_NEXT_PRIME_DISTANCE_FACTOR;
    }
    /* scale primes such that largest value is 1.0 in both */
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
        params->size_factor[i] = params->primes[i] / params->primes[0];
        params->decay_factor[i] = params->primes[i] / params->primes[0];
    }
    /* build history buffers, one per channel per echo */
    /* with 20 voices, 0.5 s max buffer, 48 kHz sample rate
     * and 4 bytes per sample we need approx. 1 MB */
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
        for (j = 0; j < MAX_CHANNELS; j += 1) {
            params->history[j][i] = new_Backbuf(MAX_ECHO_LENGTH / 1000.0 * MAX_SAMPLE_RATE * params->size_factor[i]);
        }
    }
    return p;
}
