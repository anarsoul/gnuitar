/*
 * GNUitar
 * Sustain effect
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
 * Revision 1.8  2003/03/11 22:04:00  fonin
 * Measure control sliders in standard units (ms, %).
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
 * Revision 1.1.1.1  2001/01/11 13:22:19  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "sustain.h"
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "gui.h"


void            sustain_filter(struct effect *p, struct data_block *db);


void
update_sustain_sust(GtkAdjustment * adj, struct sustain_params *params)
{
    params->sust = (int) adj->value * 2.56;
}

void
update_sustain_noise(GtkAdjustment * adj, struct sustain_params *params)
{
    params->noise = (int) adj->value * 2.56;
}

void
update_sustain_gate(GtkAdjustment * adj, struct sustain_params *params)
{
    params->threshold = (int) adj->value * 2.56;
}


void
toggle_sustain(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = sustain_filter;
	p->toggle = 1;
    }
}

void
sustain_init(struct effect *p)
{
    struct sustain_params *psustain;

    GtkWidget      *gate;
    GtkWidget      *gate_label;
    GtkObject      *adj_gate;

    GtkWidget      *noise;
    GtkWidget      *noise_label;
    GtkObject      *adj_noise;

    GtkWidget      *sust;
    GtkWidget      *sust_label;
    GtkObject      *adj_sust;

    GtkWidget      *button;

    GtkWidget      *parmTable;


    psustain = (struct sustain_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    gtk_window_set_position(GTK_WINDOW(p->control), GTK_WIN_POS_CENTER);

    parmTable = gtk_table_new(4, 8, FALSE);

    adj_gate = gtk_adjustment_new(psustain->threshold / 2.56,
				  0.0, 101.0, 1.0, 1.0, 1.0);
    gate_label = gtk_label_new("Gate\n%");
    gtk_table_attach(GTK_TABLE(parmTable), gate_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_gate), "value_changed",
		       GTK_SIGNAL_FUNC(update_sustain_gate), psustain);

    gate = gtk_vscale_new(GTK_ADJUSTMENT(adj_gate));

    gtk_table_attach(GTK_TABLE(parmTable), gate, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_sust =
	gtk_adjustment_new(psustain->sust / 2.56, 0.0, 101.0, 1.0, 1.0,
			   1.0);
    sust_label = gtk_label_new("Sustain\n%");
    gtk_table_attach(GTK_TABLE(parmTable), sust_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_sust), "value_changed",
		       GTK_SIGNAL_FUNC(update_sustain_sust), psustain);

    sust = gtk_vscale_new(GTK_ADJUSTMENT(adj_sust));

    gtk_table_attach(GTK_TABLE(parmTable), sust, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_noise =
	gtk_adjustment_new(psustain->noise / 2.56, 0.0, 101.0, 1.0, 1.0,
			   1.0);
    noise_label = gtk_label_new("Noise\n%");
    gtk_table_attach(GTK_TABLE(parmTable), noise_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_noise), "value_changed",
		       GTK_SIGNAL_FUNC(update_sustain_noise), psustain);

    noise = gtk_vscale_new(GTK_ADJUSTMENT(adj_noise));

    gtk_table_attach(GTK_TABLE(parmTable), noise, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_sustain), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Sustain"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
sustain_filter(struct effect *p, struct data_block *db)
{

    int             count,
                   *s;
    struct sustain_params *ds;
    int             volAccum;
    float           CompW1;
    float           CompW2;
    float           gateFac;
    float           compFac;
    int             tmp;

    ds = (struct sustain_params *) p->params;

    count = db->len;
    s = db->data;

    volAccum = ds->volaccum;
    CompW1 = ds->sust / 100.0f;
    CompW2 = 1.0f - CompW1;

    while (count) {
	tmp = *s;
	/*
	 * update volAccum 
	 */
	tmp = (tmp < 0) ? -tmp : tmp;
	volAccum = (256 - ds->noise) * volAccum + ds->noise * tmp;
	volAccum /= 256;

	/*
	 * handle compression 
	 */
	compFac = 30000.0f / (float) volAccum;
	compFac = CompW1 * compFac + CompW2;
	/*
	 * handle gate 
	 */
	if (ds->threshold <= 1.0f)
	    gateFac = 1.0f;
	else
	    gateFac = (volAccum > (ds->threshold * 100)) ? 1.0f :
		((float) (volAccum) / (float) (ds->threshold * 100));
	/*
	 * process signal... 
	 */
	tmp = (int) ((float) (*s) * compFac * gateFac);
	tmp = (tmp < -32767) ? -32767 : (tmp > 32767) ? 32767 : tmp;
	*s = tmp;
	s++;
	count--;
    }
    ds->volaccum = volAccum;

}

void
sustain_save(struct effect *p, int fd)
{
    struct sustain_params *sp;

    sp = (struct sustain_params *) p->params;

    write(fd, &sp->sust, sizeof(sp->sust));
    write(fd, &sp->noise, sizeof(sp->noise));
    write(fd, &sp->threshold, sizeof(sp->threshold));
}

void
sustain_load(struct effect *p, int fd)
{
    struct sustain_params *sp;

    sp = (struct sustain_params *) p->params;

    read(fd, &sp->sust, sizeof(sp->sust));
    read(fd, &sp->noise, sizeof(sp->noise));
    read(fd, &sp->threshold, sizeof(sp->threshold));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = sustain_filter;
    }
}

void
sustain_done(struct effect *p)
{
    struct sustain_params *dp;

    dp = (struct sustain_params *) p->params;

    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
sustain_create(struct effect *p)
{
    struct sustain_params *psustain;

    p->params =
	(struct sustain_params *) malloc(sizeof(struct sustain_params));
    p->proc_init = sustain_init;
    p->proc_filter = passthru;
    p->proc_load = sustain_load;
    p->proc_save = sustain_save;
    p->toggle = 0;
    p->proc_done = sustain_done;
    p->id = SUSTAIN;

    psustain = (struct sustain_params *) p->params;

    psustain->noise = 40;
    psustain->sust = 100;
    psustain->threshold = 100;
}
