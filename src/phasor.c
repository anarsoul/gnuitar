/*
 * GNUitar
 * Phasor effect
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
 * Revision 1.13  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.12  2005/04/29 11:24:42  fonin
 * Better presets
 *
 * Revision 1.11  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.10  2003/03/13 20:24:30  fonin
 * New parameter "bandpass" - turn on bandpass function.
 *
 * Revision 1.9  2003/03/12 20:55:35  fonin
 * - meaningful measure units;
 * - code cleanup.
 *
 * Revision 1.8  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.7  2003/02/01 19:15:12  fonin
 * Use sizeof(variable) instead sizeof(type) in load/save procedures,
 * when reading/writing from file.
 *
 * Revision 1.6  2003/01/30 21:35:29  fonin
 * Got rid of rnd_window_pos().
 *
 * Revision 1.5  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.4  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.3  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.2  2001/01/13 10:02:12  fonin
 * Initial filter is passthru
 *
 * Revision 1.1.1.1  2001/01/11 13:21:58  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "phasor.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

void            phasor_filter(struct effect *p, struct data_block *db);

void
update_phasor_speed(GtkAdjustment * adj, struct phasor_params *params)
{
    params->df =
	(params->freq_high -
	 params->freq_low) * 1000.0 * buffer_size / (sample_rate *
						     nchannels *
						     (float) adj->value);
}

void
update_phasor_freq_low(GtkAdjustment * adj, struct phasor_params *params)
{
    params->freq_low = (float) adj->value;
    params->f = params->freq_low;
}

void
update_phasor_freq_high(GtkAdjustment * adj, struct phasor_params *params)
{
    params->freq_high = (float) adj->value;
}

void
toggle_phasor(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = phasor_filter;
	p->toggle = 1;
    }
}

void
toggle_bandpass(void *bullshit, struct effect *p)
{
    struct phasor_params *pp;
    pp = (struct phasor_params *) p->params;
    pp->bandpass = !pp->bandpass;
}

void
phasor_init(struct effect *p)
{
    struct phasor_params *pphasor;
    GtkWidget      *freq_low;
    GtkWidget      *freq_low_label;
    GtkObject      *adj_freq_low;

    GtkWidget      *freq_high;
    GtkWidget      *freq_high_label;
    GtkObject      *adj_freq_high;

    GtkWidget      *speed;
    GtkWidget      *speed_label;
    GtkObject      *adj_speed;

    GtkWidget      *button;
    GtkWidget      *bandpass;
    GtkWidget      *parmTable;
    pphasor = p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(4, 8, FALSE);


    adj_speed = gtk_adjustment_new((pphasor->freq_high -
				    pphasor->freq_low) * 1000 *
				   buffer_size / (sample_rate * nchannels *
						  pphasor->df), 5.0, 2000,
				   1.0, 10.0, 1.0);
    speed_label = gtk_label_new("Speed\n1/ms");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_phasor_speed), pphasor);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_freq_low =
	gtk_adjustment_new(pphasor->freq_low, 100.0, 2500.0, 1.0, 1.0,
			   1.0);
    freq_low_label = gtk_label_new("Freq.low\nHz");
    gtk_table_attach(GTK_TABLE(parmTable), freq_low_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_freq_low), "value_changed",
		       GTK_SIGNAL_FUNC(update_phasor_freq_low), pphasor);

    freq_low = gtk_vscale_new(GTK_ADJUSTMENT(adj_freq_low));

    gtk_table_attach(GTK_TABLE(parmTable), freq_low, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_freq_high =
	gtk_adjustment_new(pphasor->freq_high, 250.0, 5000.0, 1.0, 1.0,
			   1.0);
    freq_high_label = gtk_label_new("Freq.high\nHz");
    gtk_table_attach(GTK_TABLE(parmTable), freq_high_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_freq_high), "value_changed",
		       GTK_SIGNAL_FUNC(update_phasor_freq_high), pphasor);

    freq_high = gtk_vscale_new(GTK_ADJUSTMENT(adj_freq_high));

    gtk_table_attach(GTK_TABLE(parmTable), freq_high, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_phasor), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 2, 2, 3,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    bandpass = gtk_check_button_new_with_label("Bandpass");
    gtk_signal_connect(GTK_OBJECT(bandpass), "toggled",
		       GTK_SIGNAL_FUNC(toggle_bandpass), p);

    gtk_table_attach(GTK_TABLE(parmTable), bandpass, 3, 6, 2, 3,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (pphasor->bandpass == 1) {
	pphasor->bandpass = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(bandpass), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Phasor"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
phasor_filter(struct effect *p, struct data_block *db)
{
    struct phasor_params *pp;

    pp = (struct phasor_params *) p->params;

    LC_filter(db->data, db->len, HIGHPASS, pp->f, &(pp->fd));

    if (pp->bandpass)
	RC_bandpass(db->data, db->len, &(pp->fd));

    pp->f += pp->df;
    if (pp->f >= pp->freq_high || pp->f <= pp->freq_low)
	pp->df = -pp->df;

    RC_set_freq(pp->f, &(pp->fd));
}

void
phasor_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
phasor_save(struct effect *p, int fd)
{
    struct phasor_params *pp;

    pp = (struct phasor_params *) p->params;

    write(fd, &pp->freq_low, sizeof(pp->freq_low));
    write(fd, &pp->freq_high, sizeof(pp->freq_high));
    write(fd, &pp->df, sizeof(pp->df));
}

void
phasor_load(struct effect *p, int fd)
{
    struct phasor_params *pp;

    pp = (struct phasor_params *) p->params;

    read(fd, &pp->freq_low, sizeof(pp->freq_low));
    read(fd, &pp->freq_high, sizeof(pp->freq_high));
    read(fd, &pp->df, sizeof(pp->df));
    pp->f = pp->freq_low;

    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = phasor_filter;
    }
}

void
phasor_create(struct effect *p)
{
    struct phasor_params *pphasor;

    p->params =
	(struct phasor_params *) malloc(sizeof(struct phasor_params));
    p->proc_init = phasor_init;
    p->proc_filter = passthru;
    p->proc_done = phasor_done;
    p->proc_load = phasor_load;
    p->proc_save = phasor_save;
    p->id = PHASOR;

    pphasor = p->params;

    pphasor->freq_low = 300.0;
    pphasor->freq_high = 2500.0;
    pphasor->f = pphasor->freq_low;
    pphasor->df = 42.0;
    pphasor->bandpass = 0;

    RC_setup(10, 1.5, &(pphasor->fd));
    RC_set_freq(pphasor->f, &(pphasor->fd));
}
