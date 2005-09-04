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
 * Revision 1.21  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.20  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.19  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.18  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.17  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.16  2005/09/01 22:23:27  alankila
 * - missed one nchannels
 *
 * Revision 1.15  2005/09/01 22:07:18  alankila
 * - multichannel ready
 *
 * Revision 1.14  2005/08/24 10:49:47  fonin
 * Minor change to compile on windows (#include utils.h for isnan)
 *
 * Revision 1.13  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.12  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.11  2005/08/11 17:57:21  alankila
 * - add some missing headers & fix all compiler warnings on gcc 4.0.1+ -Wall
 *
 * Revision 1.10  2005/08/10 11:28:14  alankila
 * - redesigned eqbank UI. This change may be a bit early as UI changes are
 *   to be postponed to 0.4.0, but the old one was simply too gruesome, imo
 * - removed the use of snprintf instead of sprintf. There's no point because
 *   user can't control those values.
 *
 * Revision 1.9  2005/08/10 11:06:26  alankila
 * - sync to biquad interface
 * - change storage types of boosts and volume to double to keep fractions
 * - use sizeof(params->foo) instead of hardcoded sizes
 *
 * Revision 1.8  2005/08/07 19:38:24  alankila
 * - symmetrize Volume range from -30 to +30 and set individual gains
 *   from -9.9 to +9.9 (only 2 digits required to display, not 3 as with 10.0)
 * - if GTK2, allow flipping the range with gtk_range_set_inverted
 * - set some packing parameters so that it looks nicer in GTK2
 *
 * Revision 1.7  2005/07/31 10:23:49  fonin
 * Tiny code lickup
 *
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
#define FB_MIN -9.9
/* Maximal Value in Decibels */
#define FB_MAX 9.9

/* Array with the center frequencies of the filters in Hertz
 * Beware, to large values for the ends, may result in instability
 */

const int fb_cf[FB_NB] =
    {40,100,200,320,640,1000,1600,2200,3000,4000,6000,8000,12000,16000};
/* Array with the bandwidths of each filter in Hertz */
const int fb_bw[FB_NB] =
    {30,120,160,320,640,800,1200,1200,1400,1800,2600,4000,6000,8000};


#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#    include "utils.h"
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
    p->par->boosts[p->slider_id] = adj->value;
    set_peq_biquad(sample_rate, fb_cf[p->slider_id], fb_bw[p->slider_id],
		   adj->value, &p->par->filters[p->slider_id]);
}

void
update_eqbank_volume(GtkAdjustment * adj, struct eqbank_params *p)
{
    p->volume = adj->value;
}

void
toggle_eqbank(void *bullshit, struct effect *p)
{
    p->toggle = !p->toggle;
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

    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(3, FB_NB + 2, FALSE);

    Hzlabel = gtk_label_new("Frequency (Hz)");
    Dblabel = gtk_label_new("Boost (dB)");

    gtk_table_attach(GTK_TABLE(parmTable), Hzlabel, 1, FB_NB, 2, 3,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK), 0, 0);
    gtk_table_attach(GTK_TABLE(parmTable), Dblabel, 0, 1, 0, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK), 0, 0);
    for (i = 0; i < FB_NB; i++) {
	adj_boost[i] = gtk_adjustment_new(peq->boosts[i],
					  FB_MIN, FB_MAX, 1.0, 5.0, 0.0);
	boost[i] = gtk_vscale_new(GTK_ADJUSTMENT(adj_boost[i]));
	gtk_widget_set_size_request(GTK_WIDGET(boost[i]),32,100);
	gtk_range_set_inverted(GTK_RANGE(boost[i]), TRUE);

	sl_wrappers[i].par = peq;
	sl_wrappers[i].slider_id = i;
	gtk_signal_connect(GTK_OBJECT(adj_boost[i]), "value_changed",
			   GTK_SIGNAL_FUNC(update_eqbank_eq),
			   (void *) &sl_wrappers[i]);
	gtk_table_attach(GTK_TABLE(parmTable), boost[i], i + 1, i + 2, 0,
			 1,
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					    GTK_SHRINK),
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					    GTK_SHRINK), 3, 3);
        
        if (fb_cf[i] < 1000) {
            sprintf(s, "%i", fb_cf[i]);
        } else if (fb_cf[i] % 1000 == 0) {
            sprintf(s, "%ik", fb_cf[i] / 1000);
        } else {
            sprintf(s, "%.1fk", fb_cf[i] / 1000.0);
        }
	
        /* Other function on other compilers ? */
	boost_label[i] = gtk_label_new(s);
	gtk_table_attach(GTK_TABLE(parmTable), boost_label[i], i + 1,
			 i + 2, 1, 2,
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
			 __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK), 3, 3);

    }
    Olabel = gtk_label_new("Master");
    gtk_table_attach(GTK_TABLE(parmTable), Olabel, FB_NB + 1, FB_NB + 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK), 3, 3);
    adj_output = gtk_adjustment_new(peq->volume, -30, 30, 1.0, 5.0, 0.0);
    output = gtk_vscale_new(GTK_ADJUSTMENT(adj_output));
    gtk_widget_set_size_request(output,55,100);
    gtk_range_set_inverted(GTK_RANGE(output), TRUE);
    gtk_signal_connect(GTK_OBJECT(adj_output), "value_changed",
		       GTK_SIGNAL_FUNC(update_eqbank_volume),
		       (void *) peq);
    gtk_table_attach(GTK_TABLE(parmTable), output, FB_NB + 1, FB_NB + 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 3);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_eqbank), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK), 0, 0);

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
    double		  t, ocoeff;
    int			  cchannel=0;

    ep = p->params;
	
    count = db->len;
    s = db->data;

    ocoeff = pow(10, ep->volume / 20.0);
    while (count) {
	t = *s;
	for (i = 0; i < FB_NB; i++)
	    t = do_biquad(t, &ep->filters[i], cchannel);
	t *= ocoeff;
#ifdef CLIP_EVERYWHERE
	if (t > MAX_SAMPLE)
	    t = MAX_SAMPLE;
	if (t < -MAX_SAMPLE)
	    t = -MAX_SAMPLE;
#endif
	*s = t;
	cchannel = (cchannel + 1) % db->channels;
	  
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
    	free(ep->filters[i].mem);

    free(ep->filters);
    free(ep->boosts);

    gtk_widget_destroy(p->control);

    free(p->params);
    free(p);
}

void
eqbank_save(struct effect *p, SAVE_ARGS)
{
    struct eqbank_params *params = p->params;
    gchar *label;
    int i;

    for (i = 0; i < FB_NB; i += 1) {
	label = g_strdup_printf("boost%d", i);
	SAVE_DOUBLE(label, params->boosts[i]);
	free(label);
    }
    SAVE_DOUBLE("volume", params->volume);
}

void
eqbank_load(struct effect *p, LOAD_ARGS)
{
    struct eqbank_params *params = p->params;
    gchar *label;
    int i;

    for (i = 0; i < FB_NB; i += 1) {
	label = g_strdup_printf("boost%d", i);
	LOAD_DOUBLE(label, params->boosts[i]);
	free(label);
    }
    LOAD_DOUBLE("volume", params->volume);

    for (i = 0; i < FB_NB; i++) {
	set_peq_biquad(sample_rate, fb_cf[i], fb_bw[i], params->boosts[i], &params->filters[i]);
    }
}

effect_t *
eqbank_create()
{
    effect_t       *p;
    struct eqbank_params *peq;
    int             i;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct eqbank_params));
    p->proc_init = eqbank_init;
    p->proc_filter = eqbank_filter;
    p->proc_save = eqbank_save;
    p->proc_load = eqbank_load;
    p->toggle = 0;
    p->id = EQBANK;
    p->proc_done = eqbank_done;

    peq = p->params;
    peq->filters = calloc(FB_NB, sizeof(peq->filters[0]));
    peq->boosts  = calloc(FB_NB, sizeof(peq->boosts[0]));
    for (i = 0; i < FB_NB; i++) {
    	peq->filters[i].mem = calloc(MAX_CHANNELS, sizeof(double) * 4);
	peq->boosts[i] = 0;
	set_peq_biquad(sample_rate, fb_cf[i], fb_bw[i], peq->boosts[i], &peq->filters[i]);
    }
    peq->volume=0;

    return p;
}
