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

/* 1 second at max sample rate */
#define MAX_REVERB_SIZE  MAX_SAMPLE_RATE

void            reverb_filter(struct effect *p, struct data_block *db);

void
update_reverb_wet(GtkAdjustment * adj, struct reverb_params *params)
{
    params->wet = (int) adj->value;
}

void
update_reverb_dry(GtkAdjustment * adj, struct reverb_params *params)
{
    params->dry = (int) adj->value;
}

void
update_reverb_delay(GtkAdjustment * adj, struct reverb_params *params)
{
    int             i;
    params->delay = adj->value;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        params->history[i]->clear(params->history[i]);
}

void
update_reverb_regen(GtkAdjustment * adj, struct reverb_params *params)
{
    params->regen = (int) adj->value;
}

void
toggle_reverb(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = reverb_filter;
	p->toggle = 1;
    }
}


void
reverb_init(struct effect *p)
{
    struct reverb_params *preverb;

    GtkWidget      *wet;
    GtkWidget      *wet_label;
    GtkObject      *adj_wet;

    GtkWidget      *dry;
    GtkWidget      *dry_label;
    GtkObject      *adj_dry;

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

    adj_delay = gtk_adjustment_new(preverb->delay, 1.0, 1000.0, 1.0, 1.0, 0.0);
    delay_label = gtk_label_new("delay\nms");
    gtk_table_attach(GTK_TABLE(parmTable), delay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_delay), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_delay), preverb);

    delay = gtk_vscale_new(GTK_ADJUSTMENT(adj_delay));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(delay),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), delay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_wet =
	gtk_adjustment_new(preverb->wet, 0.0, 100.0, 1.0, 1.0, 0.0);
    wet_label = gtk_label_new("wet\n%");
    gtk_table_attach(GTK_TABLE(parmTable), wet_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_wet), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_wet), preverb);

    wet = gtk_vscale_new(GTK_ADJUSTMENT(adj_wet));

    gtk_table_attach(GTK_TABLE(parmTable), wet, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_dry =
	gtk_adjustment_new(preverb->dry, 0.0, 100.0, 1.0, 1.0, 0.0);
    dry_label = gtk_label_new("dry\n%");
    gtk_table_attach(GTK_TABLE(parmTable), dry_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_dry), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_dry), preverb);

    dry = gtk_vscale_new(GTK_ADJUSTMENT(adj_dry));

    gtk_table_attach(GTK_TABLE(parmTable), dry, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_regen = gtk_adjustment_new(preverb->regen, 0.0, 95.0, 1.0, 1.0, 0.0);
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
		       GTK_SIGNAL_FUNC(toggle_reverb), p);

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

void
reverb_filter(struct effect *p, struct data_block *db)
{
    struct reverb_params *dr;
    DSP_SAMPLE     *s;
    int             dd,
                    count,
                    curr_channel = 0;
    double          tmp, tot, Dry, Wet, Rgn;
    dr = (struct reverb_params *) p->params;

    s = db->data;
    count = db->len;

    /*
     * get delay 
     */
    dd = dr->delay * sample_rate / 1000.0;
    if (dd < 1)
        dd = 1;
    if (dd > MAX_REVERB_SIZE)
        dd = MAX_REVERB_SIZE;

    /*
     * get parms 
     */
    Dry = dr->dry / 100.0;
    Wet = dr->wet / 100.0;
    Rgn = dr->regen / 100.0;

    while (count) {
	/* the old sample * Rgn */
        tmp = dr->history[curr_channel]->get(dr->history[curr_channel], dd)
                * Rgn;
        /* mix with original and write to backbuf */
        tot = tmp + *s;
#ifdef CLIP_EVERYWHERE
	tot =
	    (tot < -MAX_SAMPLE) ? -MAX_SAMPLE : (tot >
						 MAX_SAMPLE) ? MAX_SAMPLE :
	    tot;
#endif
        dr->history[curr_channel]->add(dr->history[curr_channel], tot);

        /* mix reverb with output with proportions as given by wet & dry % */
        tot = ((double) *s) * Dry + tmp * Wet;
#ifdef CLIP_EVERYWHERE
	tot =
	    (tot < -MAX_SAMPLE) ? -MAX_SAMPLE : (tot >
						 MAX_SAMPLE) ? MAX_SAMPLE :
	    tot;
#endif
	*s = tot;
        
        curr_channel = (curr_channel + 1) % db->channels;
	s++;
	count--;
    }
}

void
reverb_done(struct effect *p)
{
    struct reverb_params *dr;
    int             i;
    
    dr = (struct reverb_params *) p->params;

    for (i = 0; i < MAX_CHANNELS; i += 1)
        del_Backbuf(dr->history[i]);
    
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

void
reverb_save(struct effect *p, int fd)
{
    struct reverb_params *rp;

    rp = (struct reverb_params *) p->params;

    write(fd, &rp->dry, sizeof(rp->dry));
    write(fd, &rp->wet, sizeof(rp->wet));
    write(fd, &rp->regen, sizeof(rp->regen));
    write(fd, &rp->delay, sizeof(rp->delay));
}

void
reverb_load(struct effect *p, int fd)
{
    struct reverb_params *rp;

    rp = (struct reverb_params *) p->params;

    read(fd, &rp->dry, sizeof(rp->dry));
    read(fd, &rp->wet, sizeof(rp->wet));
    read(fd, &rp->regen, sizeof(rp->regen));
    read(fd, &rp->delay, sizeof(rp->delay));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = reverb_filter;
    }
}


void
reverb_create(struct effect *p)
{
    struct reverb_params *dr;
    int             i;
    
    p->params = calloc(1, sizeof(struct reverb_params));

    p->proc_init = reverb_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->id = REVERB;
    p->proc_done = reverb_done;
    p->proc_load = reverb_load;
    p->proc_save = reverb_save;
    dr = (struct reverb_params *) p->params;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        dr->history[i] = new_Backbuf(MAX_REVERB_SIZE); /* 1 second memory */

    dr->delay = 100;    /* ms */
    dr->wet   = 50.0;
    dr->dry   = 50.0;
    dr->regen = 30.0;
}
