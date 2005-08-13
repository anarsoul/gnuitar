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
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "reverb.h"
#include "gui.h"

DSP_SAMPLE *
newChunk()
{
    return (DSP_SAMPLE *) malloc(buffer_size * sizeof(DSP_SAMPLE));
}

void
freeChunk(DSP_SAMPLE *p)
{
    free(p);
}

void
reverbBuffer_init(struct reverbBuffer *r, int chunks)
{
    r->data = (DSP_SAMPLE *) malloc(chunks * buffer_size * sizeof(DSP_SAMPLE));
    r->nChunks = chunks;
    r->nCursor = 0;
}

void
reverbBuffer_done(struct reverbBuffer *r)
{
    free(r->data);
    r->data = NULL;
}

void
reverbBuffer_addChunk(struct reverbBuffer *r, DSP_SAMPLE *chunk)
{
    DSP_SAMPLE *addTo;
    addTo = r->data + r->nCursor * buffer_size;
    memcpy(addTo, chunk, buffer_size * sizeof(DSP_SAMPLE));
    r->nCursor++;
    r->nCursor %= r->nChunks;
}

DSP_SAMPLE            *
reverbBuffer_getChunk(struct reverbBuffer *r, int delay)
{
    int             nFrom;
    DSP_SAMPLE     *getFrom;
    DSP_SAMPLE     *giveTo;

    assert((delay >= 0) && (delay < r->nChunks));
    nFrom = r->nCursor - delay;
    while (nFrom < 0)
	nFrom += r->nChunks;
    getFrom = r->data + nFrom * buffer_size;
    giveTo = newChunk();
    memcpy(giveTo, getFrom, buffer_size * sizeof(DSP_SAMPLE));
    return giveTo;
}

void            reverb_filter(struct effect *p, struct data_block *db);

void
update_reverb_wet(GtkAdjustment * adj, struct reverb_params *params)
{
    params->wet = (int) adj->value * 2.56;
}

void
update_reverb_dry(GtkAdjustment * adj, struct reverb_params *params)
{
    params->dry = (int) adj->value * 2.56;
}

void
update_reverb_delay(GtkAdjustment * adj, struct reverb_params *params)
{
    params->delay =
	(int) adj->value * nchannels * sample_rate / (1000 * buffer_size);
}

void
update_reverb_regen(GtkAdjustment * adj, struct reverb_params *params)
{
    params->regen = (int) adj->value * 2.56;
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
    p->control = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_delay =
	gtk_adjustment_new(preverb->delay * buffer_size * 1000 /
			   (sample_rate * nchannels), 1.0,
			   preverb->history->nChunks * buffer_size * 1000 /
			   (sample_rate * nchannels), 1.0, 1.0, 1.0);
    delay_label = gtk_label_new("delay\nms");
    gtk_table_attach(GTK_TABLE(parmTable), delay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


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
	gtk_adjustment_new(preverb->wet / 2.56, 1.0, 101.0, 1.0, 1.0, 1.0);
    wet_label = gtk_label_new("wet\n%");
    gtk_table_attach(GTK_TABLE(parmTable), wet_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_wet), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_wet), preverb);

    wet = gtk_vscale_new(GTK_ADJUSTMENT(adj_wet));

    gtk_table_attach(GTK_TABLE(parmTable), wet, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_dry =
	gtk_adjustment_new(preverb->dry / 2.56, 1.0, 101.0, 1.0, 1.0, 1.0);
    dry_label = gtk_label_new("dry\n%");
    gtk_table_attach(GTK_TABLE(parmTable), dry_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_dry), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_dry), preverb);

    dry = gtk_vscale_new(GTK_ADJUSTMENT(adj_dry));

    gtk_table_attach(GTK_TABLE(parmTable), dry, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_regen = gtk_adjustment_new(preverb->regen / 2.56,
				   0.0, 101, 1.0, 1.0, 1.0);
    regen_label = gtk_label_new("regen\n%");
    gtk_table_attach(GTK_TABLE(parmTable), regen_label, 7, 8, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_regen), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_regen), preverb);

    regen = gtk_vscale_new(GTK_ADJUSTMENT(adj_regen));

    gtk_table_attach(GTK_TABLE(parmTable), regen, 7, 8, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_reverb), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 9, 10, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
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
    int             Dry,
                    Wet,
                    Rgn,
                    dd,
                    count;
    DSP_SAMPLE     *Old,
                   *Old2,
                    Out;
    DSP_SAMPLE      tmp,
                    tot;
    dr = (struct reverb_params *) p->params;

    s = db->data;
    count = db->len;

    /*
     * get delay 
     */
    dd = dr->delay;
    dd = (dd < 1) ? 1 : (dd >=
			 dr->history->nChunks) ? dr->history->nChunks -
	1 : dd;

    /*
     * get parms 
     */
    Dry = dr->dry;
    Wet = dr->wet;
    Rgn = dr->regen;

    Old = (DSP_SAMPLE *) reverbBuffer_getChunk(dr->history, dd);
    Old2 = Old;
    while (count) {
	/*
	 * mix Old and In into Out, based upon Wet/Dry
	 * then mix Out and In back into In, based upon Rgn/1 
	 */
	tmp = *s;
	tmp *= Dry;
	tmp /= 256;
	tot = *Old;
	tot *= Wet;
	tot /= 256;
	tot += tmp;
#ifdef CLIP_EVERYWHERE
	tot =
	    (tot < -MAX_SAMPLE) ? -MAX_SAMPLE : (tot >
						 MAX_SAMPLE) ? MAX_SAMPLE :
	    tot;
#endif
	Out = tot;

	tot *= Rgn;
	tot /= 256;
	tot += *s;
#ifdef CLIP_EVERYWHERE
	tot =
	    (tot < -MAX_SAMPLE) ? -MAX_SAMPLE : (tot >
						 MAX_SAMPLE) ? MAX_SAMPLE :
	    tot;
#endif
	*s = tot;
	s++;
	Old++;
	count--;
    }

    reverbBuffer_addChunk(dr->history, db->data);
    freeChunk(Old2);
}

void
reverb_done(struct effect *p)
{
    struct reverb_params *dr;

    dr = (struct reverb_params *) p->params;

    if (dr->history != NULL)
	reverbBuffer_done(dr->history);
    free(dr->history);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
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
    p->params =
	(struct reverb_params *) malloc(sizeof(struct reverb_params));

    p->proc_init = reverb_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->id = REVERB;
    p->proc_done = reverb_done;
    p->proc_load = reverb_load;
    p->proc_save = reverb_save;
    dr = (struct reverb_params *) p->params;
    dr->history =
	(struct reverbBuffer *) malloc(sizeof(struct reverbBuffer));
    reverbBuffer_init(dr->history,
		      (int) ((sample_rate * sizeof(int) * nchannels /
			      buffer_size) / 4 + 1));

    dr->delay = dr->history->nChunks * 0.3;
    dr->wet = 80.0f;
    dr->dry = 254.0f;
    dr->regen = 100.0f;
}
