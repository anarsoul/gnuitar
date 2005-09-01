/*
 * GNUitar
 * Noise effect
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
 * Revision 1.10  2005/09/01 22:48:20  alankila
 * - factor nchannels out of noise, but should treat every channel
 *   separately. This effect is not yet multichannel ready but broken.
 *
 * Revision 1.9  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.8  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.7  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.6  2005/04/24 19:11:04  fonin
 * Added comments
 *
 * Revision 1.5  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.4  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.3  2003/04/18 18:22:04  fonin
 * Cleanup.
 *
 * Revision 1.2  2003/04/16 14:00:07  fonin
 * - fix with updating the time controls;
 * - all features work now including fade out time, fade in time,
 *   hysteresis etc.
 * - better presets.
 *
 * Revision 1.1  2003/04/12 20:01:48  fonin
 * New noise gate effect.
 *
 */

#include "noise.h"
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "gui.h"


void            noise_filter(struct effect *p, struct data_block *db);

void
update_noise_threshold(GtkAdjustment * adj, struct noise_params *params)
{
    params->threshold = (int) adj->value * 10;
}

void
update_noise_hold(GtkAdjustment * adj, struct noise_params *params)
{
    params->hold_time = (int) adj->value * sample_rate / 1000;
}

void
update_noise_release(GtkAdjustment * adj, struct noise_params *params)
{
    params->release_time =
	(int) adj->value * sample_rate / 1000;
}

void
update_noise_hyst(GtkAdjustment * adj, struct noise_params *params)
{
    params->hysteresis = (int) adj->value * 10;
}

void
update_noise_attack(GtkAdjustment * adj, struct noise_params *params)
{
    params->attack = (int) adj->value * sample_rate / 1000;
}

void
toggle_noise(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = noise_filter;
	p->toggle = 1;
    }
}

void
noise_init(struct effect *p)
{
    struct noise_params *pnoise;

    GtkWidget      *threshold;
    GtkWidget      *threshold_label;
    GtkObject      *adj_threshold;

    GtkWidget      *release;
    GtkWidget      *release_label;
    GtkObject      *adj_release;

    GtkWidget      *hold;
    GtkWidget      *hold_label;
    GtkObject      *adj_hold;

    GtkWidget      *attack;
    GtkWidget      *attack_label;
    GtkObject      *adj_attack;

    GtkWidget      *hyst;
    GtkWidget      *hyst_label;
    GtkObject      *adj_hyst;

    GtkWidget      *button;

    GtkWidget      *parmTable;


    pnoise = (struct noise_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    gtk_window_set_position(GTK_WINDOW(p->control), GTK_WIN_POS_CENTER);

    parmTable = gtk_table_new(4, 11, FALSE);

    adj_threshold = gtk_adjustment_new(pnoise->threshold / 10,
				       0.0, 101.0, 1.0, 1.0, 1.0);
    threshold_label = gtk_label_new("Threshold\n%");
    gtk_table_attach(GTK_TABLE(parmTable), threshold_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_threshold), "value_changed",
		       GTK_SIGNAL_FUNC(update_noise_threshold), pnoise);

    threshold = gtk_vscale_new(GTK_ADJUSTMENT(adj_threshold));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(threshold),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), threshold, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_hold =
	gtk_adjustment_new(pnoise->hold_time * 1000 /
			   sample_rate, 0.0, 201.0, 1.0, 1.0,
			   1.0);
    hold_label = gtk_label_new("Hold\nms");
    gtk_table_attach(GTK_TABLE(parmTable), hold_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_hold), "value_changed",
		       GTK_SIGNAL_FUNC(update_noise_hold), pnoise);

    hold = gtk_vscale_new(GTK_ADJUSTMENT(adj_hold));

    gtk_table_attach(GTK_TABLE(parmTable), hold, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_release =
	gtk_adjustment_new(pnoise->release_time * 1000 /
			   sample_rate, 0.0, 15001.0, 1.0,
			   1.0, 1.0);
    release_label = gtk_label_new("Release\nms");
    gtk_table_attach(GTK_TABLE(parmTable), release_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_release), "value_changed",
		       GTK_SIGNAL_FUNC(update_noise_release), pnoise);

    release = gtk_vscale_new(GTK_ADJUSTMENT(adj_release));

    gtk_table_attach(GTK_TABLE(parmTable), release, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_attack =
	gtk_adjustment_new(pnoise->attack * 1000 /
			   sample_rate, 0.0, 4001.0, 1.0,
			   1.0, 1.0);
    attack_label = gtk_label_new("Attack\nms");
    gtk_table_attach(GTK_TABLE(parmTable), attack_label, 7, 8, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_attack), "value_changed",
		       GTK_SIGNAL_FUNC(update_noise_attack), pnoise);

    attack = gtk_vscale_new(GTK_ADJUSTMENT(adj_attack));

    gtk_table_attach(GTK_TABLE(parmTable), attack, 7, 8, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_hyst =
	gtk_adjustment_new(pnoise->hysteresis / 10, 0.0, 101.0, 1.0, 1.0,
			   1.0);
    hyst_label = gtk_label_new("Hysteresis\n%");
    gtk_table_attach(GTK_TABLE(parmTable), hyst_label, 9, 10, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_hyst), "value_changed",
		       GTK_SIGNAL_FUNC(update_noise_hyst), pnoise);

    hyst = gtk_vscale_new(GTK_ADJUSTMENT(adj_hyst));

    gtk_table_attach(GTK_TABLE(parmTable), hyst, 9, 10, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_noise), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("noise"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
noise_filter(struct effect *p, struct data_block *db)
{

    int             	 count;
    DSP_SAMPLE     	 *s;
    struct noise_params  *dn;
    static unsigned int  hold_counter=0;    /* how much longer before we start 
					     * to supress the signal */
    static unsigned int  release_counter=0; /* how much longer before we 
					     * fade out to nothing - 
					     * fadeout counter */
    static float    	 release_amp = 1.0;
    static float	 attack_amp = 1.0;
    static unsigned int  attack_counter = 0;
    static short         fadeout = 0;	/* if non-zero, we use hysteresis to
					 * suppress the sound.
					 * Otherwise, we use the threshold.  */

    dn = (struct noise_params *) p->params;

    count = db->len;
    s = db->data;

    while (count) {
	/* signal is below the threshold, we're not already fading out */
	if ((((*s >= 0 && *s < (dn->threshold << 8))
	      || (*s <= 0 && *s > -(dn->threshold << 8))) && !fadeout)
	    ||
	    /* or signal is below the hysteresis (stop threshold),
	     * and we're already fading out */
	    (((*s >= 0 && *s < (dn->hysteresis << 8))
	      || (*s <= 0 && *s > -(dn->hysteresis << 8))) && fadeout)) {

	    /* When the signal is near the zero for the hold time long,
	     * we do the fadeout  */
	    hold_counter++;
	    if (hold_counter >= dn->hold_time * db->channels) {
		/* we're within the hysteresis - init the fadein attack vars,
		 * also we'll now react on threshold instead of hysteresis
		 * (fadeout = 0) */
		if ((*s >= 0 && *s < (dn->hysteresis << 8))
		    || (*s <= 0 && *s > -(dn->hysteresis << 8))) {
		    attack_counter = 0;
		    attack_amp = 1;
		    fadeout = 0;
		}

		/* we're fading out - adjust the fadeout amplify coefficient */
		if (dn->release_time && release_counter < dn->release_time * db->channels) {
		    release_counter++;
		    release_amp =
			((float) dn->release_time -
			 (float) release_counter) /
			(float) dn->release_time;
		/* otherwise, cut off the signal immediately */
		} else if (!dn->release_time)
		    release_amp = 0;
	    }
	/* signal is above the threshold/hysteresis */
	} else {
	    /* Init vars. Don't be confused by setting up a fadeout.
	     * It only will start if we'll become lower than hysteresis. */
	    hold_counter = 0;
	    release_counter = 0;
	    release_amp = 1.0;
	    fadeout = 1;

	    /* if fadein is setup, we adjust the attack amp.coeff. */
	    if (dn->attack && attack_counter < dn->attack * db->channels) {
		attack_counter++;
		attack_amp = (float) attack_counter / (float) dn->attack;
	    /* otherwise, it's always 1 */
	    } else
		attack_amp = 1;
	}
	*s *= attack_amp * release_amp;
	s++;
	count--;
    }
}

void
noise_save(struct effect *p, int fd)
{
    struct noise_params *sp;

    sp = (struct noise_params *) p->params;

    write(fd, &sp->threshold, sizeof(sp->threshold));
    write(fd, &sp->hold_time, sizeof(sp->hold_time));
    write(fd, &sp->release_time, sizeof(sp->release_time));
    write(fd, &sp->attack, sizeof(sp->attack));
    write(fd, &sp->hysteresis, sizeof(sp->hysteresis));
}

void
noise_load(struct effect *p, int fd)
{
    struct noise_params *sp;

    sp = (struct noise_params *) p->params;

    read(fd, &sp->threshold, sizeof(sp->threshold));
    read(fd, &sp->hold_time, sizeof(sp->hold_time));
    read(fd, &sp->release_time, sizeof(sp->release_time));
    read(fd, &sp->attack, sizeof(sp->attack));
    read(fd, &sp->hysteresis, sizeof(sp->hysteresis));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = noise_filter;
    }
}

void
noise_done(struct effect *p)
{
    struct noise_params *dp;

    dp = (struct noise_params *) p->params;

    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
noise_create(struct effect *p)
{
    struct noise_params *pnoise;

    p->params =
	(struct noise_params *) malloc(sizeof(struct noise_params));
    p->proc_init = noise_init;
    p->proc_filter = passthru;
    p->proc_load = noise_load;
    p->proc_save = noise_save;
    p->toggle = 0;
    p->proc_done = noise_done;
    p->id = NOISE;

    pnoise = (struct noise_params *) p->params;

    pnoise->threshold = 500;
    pnoise->hold_time = 2 * sample_rate / 1000;
    pnoise->release_time = 500 * sample_rate / 1000;
    pnoise->attack = 0 * sample_rate / 1000;
    pnoise->hysteresis = 410;
}
