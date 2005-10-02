/*
 * GNUitar
 * Autowah effect
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
 * Revision 1.27  2005/10/02 08:25:25  fonin
 * "Mix" checkbox converted to dry/wet slider
 *
 * Revision 1.26  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.25  2005/09/04 19:45:12  alankila
 * - replace "Speed 1/ms" with "Period ms" which is easier to understand
 *
 * Revision 1.24  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.23  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.22  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.21  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.20  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.19  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.18  2005/09/01 16:22:42  alankila
 * - oops, revert gain change
 *
 * Revision 1.17  2005/09/01 16:20:21  alankila
 * - fix comment, and reduce autowah gain
 *
 * Revision 1.16  2005/09/01 16:09:54  alankila
 * - make rcfilter and autowah multichannel ready. In addition, autowah
 *   now performs linear sweep in logarithmic domain rather than exponential.
 *
 * Revision 1.15  2005/08/22 22:11:59  alankila
 * - change RC filters to accept data_block
 * - LC filters have no concept of "LOWPASS" or "HIGHPASS" filtering, there's
 *   just filter_no.
 * - remove unused SAMPLE8 typedef
 *
 * Revision 1.14  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.13  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.12  2005/04/29 11:24:59  fonin
 * Return back the 1.5 amplify coeff
 *
 * Revision 1.11  2005/04/26 13:37:39  fonin
 * Declaring dry[] in the autowah_filter as static fixes the 100% CPU usage on windows; also amplify coefficients changed from 1.5 to 1.0
 *
 * Revision 1.10  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.9  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.8  2003/03/09 20:46:07  fonin
 * - parameter "speed" removed from effect internal structure, df (delta ef)
 *   instead;
 * - cleanup of dead code and variables in autowah_filter();
 * - parameter speed is measured in 1 wave per N msec.
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
 * Revision 1.3  2001/06/02 14:05:58  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.2  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:05  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "autowah.h"
#include "glib12-compat.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include <string.h>
#include <gtk/gtk.h>

void
                autowah_filter(struct effect *p, struct data_block *db);


void
update_wah_speed(GtkAdjustment * adj, struct autowah_params *params)
{
    params->sweep_time = adj->value;
}

void
update_wah_freqlow(GtkAdjustment * adj, struct autowah_params *params)
{
    params->freq_low = (float) adj->value;
}

void
update_wah_freqhi(GtkAdjustment * adj, struct autowah_params *params)
{
    params->freq_high = (float) adj->value;
}

void
update_wah_drywet(GtkAdjustment *adj, struct autowah_params *params)
{
    params->drywet=(float) adj->value;
}

void
autowah_init(struct effect *p)
{
    struct autowah_params *pautowah;

    GtkWidget      *speed_label;
    GtkWidget      *speed;
    GtkObject      *adj_speed;

    GtkWidget      *freq_low;
    GtkWidget      *freqlow_label;
    GtkObject      *adj_freqlow;

    GtkWidget      *freq_high;
    GtkWidget      *freqhi_label;
    GtkObject      *adj_freqhi;

    GtkWidget      *button;
    GtkWidget      *drywet;
    GtkObject	   *adj_drywet;
    GtkWidget      *drywet_label;
    GtkWidget      *parmTable;

    pautowah = (struct autowah_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(4, 3, FALSE);

    adj_speed = gtk_adjustment_new(pautowah->sweep_time, 100.0,
                               20000.0, 1.0, 10.0, 0.0);
    speed_label = gtk_label_new("Period\nms");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_speed), pautowah);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
    gtk_range_set_update_policy(GTK_RANGE(speed), GTK_UPDATE_DELAYED);
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);
    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_freqlow = gtk_adjustment_new(pautowah->freq_low,
				     150.0, 300.0, 1.0, 1.0, 0.0);
    freqlow_label = gtk_label_new("Freq. low\nHz");
    gtk_table_attach(GTK_TABLE(parmTable), freqlow_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_freqlow), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_freqlow), pautowah);

    freq_low = gtk_vscale_new(GTK_ADJUSTMENT(adj_freqlow));
    gtk_range_set_update_policy(GTK_RANGE(freq_low), GTK_UPDATE_DELAYED);
    gtk_table_attach(GTK_TABLE(parmTable), freq_low, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_freqhi = gtk_adjustment_new(pautowah->freq_high,
				    500.0, 3500.0, 1.0, 1.0, 0.0);
    freqhi_label = gtk_label_new("Freq. hi\nHz");
    gtk_table_attach(GTK_TABLE(parmTable), freqhi_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_freqhi), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_freqhi), pautowah);

    freq_high = gtk_vscale_new(GTK_ADJUSTMENT(adj_freqhi));
    gtk_range_set_update_policy(GTK_RANGE(freq_high), GTK_UPDATE_DELAYED);
    gtk_table_attach(GTK_TABLE(parmTable), freq_high, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    drywet_label = gtk_label_new("Dry/Wet\n%");
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);
    adj_drywet = gtk_adjustment_new(pautowah->drywet,
				    0.0, 100.0, 1.0, 5.0, 0.0);
    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));
    gtk_range_set_update_policy(GTK_RANGE(drywet), GTK_UPDATE_DELAYED);
    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_drywet), pautowah);
    gtk_table_attach(GTK_TABLE(parmTable), drywet, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Wah-wah"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
autowah_filter(struct effect *p, struct data_block *db)
{
    struct autowah_params *ap;
    static DSP_SAMPLE      dry[MAX_BUFFER_SIZE];
    int             i;
    double          freq;

    ap = (struct autowah_params *) p->params;


    memcpy(dry, db->data, db->len * sizeof(DSP_SAMPLE));

    if (ap->f > 1.0 && ap->dir > 0) {
	ap->dir = -1;
    }
    if (ap->f < 0.0 && ap->dir < 0) {
	ap->dir = 1;
    }

    /* in order to have audibly linear sweep, we must map
     * [0..1] -> [freq_low, freq_high] linearly in log2, which requires
     * f(x) = a * 2 ^ (b * x)
     *
     * we know that f(0) = freq_low, and f(1) = freq_high. It follows that:
     * a = freq_low, and b = log2(freq_high / freq_low)
     */
    
    freq = ap->freq_low * pow(2, log(ap->freq_high / ap->freq_low)/log(2) * ap->f);
    RC_set_freq(freq, ap->fd);
    RC_bandpass(db, ap->fd);

    ap->f += ap->dir * 1000.0 / ap->sweep_time * db->len / (sample_rate * db->channels) * 2;

    /* mix with dry sound */
    for (i = 0; i < db->len; i++)
        db->data[i] = (db->data[i]*ap->drywet + dry[i]*(100-ap->drywet))/100.0;
}

void
autowah_done(struct effect *p)
{
    struct autowah_params *ap;
    ap = (struct autowah_params *) p->params;

    free(ap->fd);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

void
autowah_save(effect_t *p, SAVE_ARGS)
{
    struct autowah_params *params = p->params;

    SAVE_DOUBLE("sweep_time", params->sweep_time);
    SAVE_DOUBLE("freq_low", params->freq_low);
    SAVE_DOUBLE("freq_high", params->freq_high);
    SAVE_DOUBLE("drywet", params->drywet);
}

void
autowah_load(effect_t *p, LOAD_ARGS)
{
    struct autowah_params *params = p->params;
    
    LOAD_DOUBLE("sweep_time", params->sweep_time);
    LOAD_DOUBLE("freq_low", params->freq_low);
    LOAD_DOUBLE("freq_high", params->freq_high);
    LOAD_DOUBLE("drywet", params->drywet);
}

effect_t *
autowah_create()
{
    effect_t *p;
    struct autowah_params *ap;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct autowah_params));
    ap = p->params;
    p->proc_init = autowah_init;
    p->proc_filter = autowah_filter;
    p->toggle = 0;
    p->proc_done = autowah_done;
    p->proc_save = autowah_save;
    p->proc_load = autowah_load;
    ap->fd = calloc(1, sizeof(struct filter_data));
    RC_setup(10, 1.5, ap->fd);

    ap->freq_low = 150;
    ap->freq_high = 1000;
    ap->sweep_time = 1000;
    ap->dir = 1;
    ap->f = 0.0;
    ap->drywet = 100;

    return p;
}
