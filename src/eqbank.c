/*
 * GNUitar
 * Eq Bank effect
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 * Ciulei Bogdan /Dexterus              <dexterus@hackernetwork.com>
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
 * $Log$
 * Revision 1.6  2005/07/30 18:01:39  fonin
 * Fixed bug in eqbank_load() - loaded values did not get applied
 *
 * Revision 1.5  2005/04/15 14:32:08  fonin
 * Fixed nasty bug with effect saving/loading
 *
 * Revision 1.4  2004/10/21 11:16:26  dexterus
 * Made to work with new biquad.c version (1.3)
 * Overall functional
 *
 * Revision 1.3  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.2  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 *
 */

/*
 * Static control values
 * Change to get diferently configured banks
 */

/* Number of filters in bank */
#define FB_NB 14
/* Minimal value in Decibels ( for UI purpose only) */
#define FB_MIN -10
/* Maximal Value in Decibels */
#define FB_MAX 10

/* Array with the center frequencies of the filters in Hertz
 * Beware, to large values for the ends, may result in instability
 */

const int fb_cf[FB_NB] =
		{40,100,200,320,640,1000,1600,2200,3000,4000,6000,8000,12000,16000};
/* Array with the bandwidths of each filter in Hertz */
const int fb_bw[FB_NB] =
    {30,120,160,320,640,800,1200,1200,1400,1800,2600,4000,6000,8000};


#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

#include "eqbank.h"
#include "gui.h"
#include <math.h>

void            eqbank_filter(struct effect *p, struct data_block *db);

struct slider_wrapper {
    struct eqbank_params *par;
    int             slider_id;
} sl_wrappers[FB_NB];


void
update_eqbank_eq(GtkAdjustment * adj, struct slider_wrapper *p)
{
    p->par->boosts[p->slider_id] = (int) adj->value;
    SetEqBiquad(sample_rate, fb_cf[p->slider_id], fb_bw[p->slider_id],
		(int) adj->value, &p->par->filters[p->slider_id]);
}

void
update_eqbank_volume(GtkAdjustment * adj, struct eqbank_params *p)
{
    p->volume = adj->value;
    p->ocoeff = pow(10, p->volume / 20.0);
}

void
toggle_eqbank(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = eqbank_filter;
	p->toggle = 1;
    }
}

void
eqbank_init(struct effect *p)
{
    struct eqbank_params *peq;

    GtkWidget      *boost[FB_NB],
                   *output;
    GtkObject      *adj_boost[FB_NB],
                   *adj_output;
    GtkWidget      *boost_label[FB_NB];
    GtkWidget      *Hzlabel,
                   *Dblabel,
                   *Olabel;
    GtkWidget      *parmTable;
    GtkWidget      *button;
    char            s[6];
    int             i;

    peq = (struct eqbank_params *) p->params;

#ifdef HAVE_GTK
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
#elif defined HAVE_GTK2
    p->control = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(3, FB_NB + 2, FALSE);

    Hzlabel = gtk_label_new("Frequency (Hz)");
    Dblabel = gtk_label_new("Boost (dB)");

    gtk_table_attach(GTK_TABLE(parmTable), Hzlabel, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    gtk_table_attach(GTK_TABLE(parmTable), Dblabel, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    for (i = 0; i < FB_NB; i++) {
	adj_boost[i] = gtk_adjustment_new(peq->boosts[i],
					  FB_MIN, FB_MAX, 1.0, 5.0, 1.0);
	boost[i] = gtk_vscale_new(GTK_ADJUSTMENT(adj_boost[i]));
#ifdef HAVE_GTK2
	gtk_widget_set_size_request(GTK_WIDGET(boost[i]),0,100);
#endif

	sl_wrappers[i].par = peq;
	sl_wrappers[i].slider_id = i;
	gtk_signal_connect(GTK_OBJECT(adj_boost[i]), "value_changed",
			   GTK_SIGNAL_FUNC(update_eqbank_eq),
			   (void *) &sl_wrappers[i]);
	gtk_table_attach(GTK_TABLE(parmTable), boost[i], i + 1, i + 2, 1,
			 2,
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					    GTK_SHRINK),
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					    GTK_SHRINK), 3, 3);

#ifndef _WIN32
	snprintf(s, 6, "%i", fb_cf[i]);
#else
	sprintf(s, "%i", fb_cf[i]);
#endif
	/* Other function on other compilers ? */
	boost_label[i] = gtk_label_new(s);
	gtk_table_attach(GTK_TABLE(parmTable), boost_label[i], i + 1,
			 i + 2, 0, 1,
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					    GTK_SHRINK),
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					    GTK_SHRINK), 3, 3);

    }
    Olabel = gtk_label_new("Output\n Volume (dB)");
    gtk_table_attach(GTK_TABLE(parmTable), Olabel, FB_NB + 1, FB_NB + 2, 0,
		     1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 3);
    adj_output = gtk_adjustment_new(peq->volume, -20, 30, 1.0, 5.0, 1.0);
    output = gtk_vscale_new(GTK_ADJUSTMENT(adj_output));
    gtk_signal_connect(GTK_OBJECT(adj_output), "value_changed",
		       GTK_SIGNAL_FUNC(update_eqbank_volume),
		       (void *) peq);
    gtk_table_attach(GTK_TABLE(parmTable), output, FB_NB + 1, FB_NB + 2, 1,
		     2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 3);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_eqbank), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, FB_NB >> 1,
		     (FB_NB >> 1) + 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Equalizers Bank"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
eqbank_filter(struct effect *p, struct data_block *db)
{
    int             count,
                    i;

    DSP_SAMPLE         *s;

    struct eqbank_params *ep;
    double          t;
	int cchannel;

    ep = p->params;
	
    count = db->len;
    s = db->data;

    while (count ) {
	
	
	cchannel = 0;
	t = *s;
	for (i = 0; i < FB_NB; i++)
	    t = doBiquad(t, &ep->filters[i], cchannel);
	t *= ep->ocoeff;
	if (t > MAX_SAMPLE)
	    t = MAX_SAMPLE;
	if (t < -MAX_SAMPLE)
	    t = -MAX_SAMPLE;
	*s = t;
	if (cchannel < nchannels - 1) 
	    cchannel++;
	  
	s++;
	count--;
    }
}

void
eqbank_done(struct effect *p)
{
    struct eqbank_params *ep;
    unsigned int i;

    ep = (struct eqbank_params *) p->params;
	
    for (i = 0; i < FB_NB; i++) 
    	free( ep->filters[i].mem);

    free(ep->filters);
    free(ep->boosts);

    gtk_widget_destroy(p->control);

    free(p->params);
    free(p);
    p = NULL;
}

void
eqbank_save(struct effect *p, int fd)
{
    struct eqbank_params *ep;

    ep = (struct eqbank_params *) p->params;

    write(fd, ep->boosts, sizeof(int) * FB_NB);
    write(fd, &ep->volume, sizeof(int));

}

void
eqbank_load(struct effect *p, int fd)
{
    struct eqbank_params *ep;
    int             i;

    ep = (struct eqbank_params *) p->params;
    read(fd, ep->boosts, sizeof(int) * FB_NB);
    read(fd, &ep->volume, sizeof(int));
    for (i = 0; i < FB_NB; i++) {
        ep->filters[i].a0=0;
        ep->filters[i].a1=0;
        ep->filters[i].a2=0;
        ep->filters[i].b1=0;
        ep->filters[i].b2=0;
	SetEqBiquad(sample_rate, fb_cf[i], fb_bw[i], ep->boosts[i], &ep->filters[i]);
    }

    ep->ocoeff = pow(10, ep->volume / 20.0);

    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = eqbank_filter;
    }

}

void
eqbank_create(struct effect *p)
{
    struct eqbank_params *peq;
    int             i;

    p->proc_init = eqbank_init;
    p->proc_filter = passthru;
    p->proc_save = eqbank_save;
    p->proc_load = eqbank_load;
    p->toggle = 0;
    p->id = EQBANK;
    p->proc_done = eqbank_done;

    p->params =
	(struct eqbank_params *) malloc(sizeof(struct eqbank_params));
    peq = (struct eqbank_params *) p->params;
    peq->filters = (struct Biquad *) malloc(sizeof(struct Biquad) * FB_NB );
    peq->boosts = (int *) malloc(sizeof(int) * FB_NB);
    for (i = 0; i < FB_NB; i++) 
    {
    	memset((void *) &peq->filters[i], 0, sizeof(struct Biquad));
    	peq->filters[i].mem = (double*) malloc( (nchannels * sizeof (double)) << 2);
    	memset((void *) peq->filters[i].mem, 0, (nchannels * sizeof (double)) << 2);
	SetEqBiquad(sample_rate, fb_cf[i], fb_bw[i], 0, &peq->filters[i]);
	peq->boosts[i] = 0;
    }
    peq->ocoeff = 1;
    peq->volume=0;
}
