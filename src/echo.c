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
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

void
                echo_filter(struct effect *p, struct data_block *db);

void
update_echo_decay(GtkAdjustment * adj, struct echo_params *params)
{
    params->echo_decay = (int) adj->value * 10;
}

void
update_echo_count(GtkAdjustment * adj, struct echo_params *params)
{
    params->buffer_count = (int) adj->value;
}

void
update_echo_size(GtkAdjustment * adj, struct echo_params *params)
{
    params->echo_size = (int) adj->value * sample_rate * nchannels / 1000;
}

void
toggle_echo(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = echo_filter;
	p->toggle = 1;
    }
}


int
prime(int n)
{
    int             i;

    for (i = 2; i < n; i++)
	if (n % i == 0)
	    return 0;

    return 1;
}

void
echo_init(struct effect *p)
{
    struct echo_params *pecho;

    GtkWidget      *decay;
    GtkWidget      *decay_label;
    GtkObject      *adj_decay;

    GtkWidget      *count;
    GtkWidget      *count_label;
    GtkObject      *adj_count;

    GtkWidget      *size;
    GtkWidget      *size_label;
    GtkObject      *adj_size;

    GtkWidget      *button;
    GtkWidget      *parmTable;

    pecho = (struct echo_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_decay = gtk_adjustment_new(pecho->echo_decay / 10,
				   1.0, 100.0, 1.0, 10.0, 1.0);
    decay_label = gtk_label_new("Decay\n%");
    gtk_table_attach(GTK_TABLE(parmTable), decay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_decay), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_decay), pecho);

    decay = gtk_vscale_new(GTK_ADJUSTMENT(adj_decay));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(decay),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), decay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_count = gtk_adjustment_new(pecho->buffer_count,
				   1.0, MAX_ECHO_COUNT, 1.0, 1.0, 1.0);
    count_label = gtk_label_new("Repeat\ntimes");
    gtk_table_attach(GTK_TABLE(parmTable), count_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_count), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_count), pecho);

    count = gtk_vscale_new(GTK_ADJUSTMENT(adj_count));

    gtk_table_attach(GTK_TABLE(parmTable), count, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_size =
	gtk_adjustment_new(pecho->echo_size * 1000 /
			   (sample_rate * nchannels), 1.0,
			   MAX_ECHO_SIZE * 1000 / (sample_rate *
						   nchannels), 1.0, 1.0,
			   1.0);
    size_label = gtk_label_new("Delay\nms");
    gtk_table_attach(GTK_TABLE(parmTable), size_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_size), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_size), pecho);

    size = gtk_vscale_new(GTK_ADJUSTMENT(adj_size));

    gtk_table_attach(GTK_TABLE(parmTable), size, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_echo), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Echo"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

void
echo_filter(struct effect *p, struct data_block *db)
{
    DSP_SAMPLE     *s,sample;
    int             j,
                    count;
    struct echo_params *ep;

    s = db->data;
    count = db->len;

    ep = p->params;

    while (count) {
	sample = *s * ep->buffer_count;
	/*
	 * add sample, decay history 
	 */
	for (j = 0; j < ep->buffer_count; j++) {
	    sample +=
		(ep->history[j][ep->index[j]] =
		 ep->history[j][ep->index[j]] * (ep->echo_decay +
						 j * 0) / 1000 + *s);
	    ep->index[j]++;
	    if (ep->index[j] >= ep->size[j])
		ep->index[j] = 0;
	}
	sample = sample / 2 / ep->buffer_count;
	*s = sample;

	s++;
	count--;
    }
}

void
echo_done(struct effect *p)
{
    struct echo_params *ep;
    int             i;

    ep = p->params;

    for (i = 0; i < ep->buffer_count; i++) {
	free(ep->history[i]);
    }

    free(ep->history);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
echo_save(struct effect *p, int fd)
{
    struct echo_params *ep;

    ep = (struct echo_params *) p->params;

    write(fd, &ep->echo_size, sizeof(ep->echo_size));
    write(fd, &ep->echo_decay, sizeof(ep->echo_decay));
    write(fd, &ep->buffer_count, sizeof(ep->buffer_count));
}

void
echo_load(struct effect *p, int fd)
{
    struct echo_params *ep;

    ep = (struct echo_params *) p->params;

    read(fd, &ep->echo_size, sizeof(ep->echo_size));
    read(fd, &ep->echo_decay, sizeof(ep->echo_decay));
    read(fd, &ep->buffer_count, sizeof(ep->buffer_count));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = echo_filter;
    }
}

void
echo_create(struct effect *p)
{
    struct echo_params *pecho;
    int             i = 10,
                    k = 0;

    p->params = (struct echo_params *) malloc(sizeof(struct echo_params));
    p->proc_init = echo_init;
    p->proc_filter = passthru;
    p->proc_save = echo_save;
    p->proc_load = echo_load;
    p->toggle = 0;
    p->id = ECHO;
    p->proc_done = echo_done;

    pecho = (struct echo_params *) p->params;

    pecho->echo_size = 128;
    pecho->echo_decay = 700;
    pecho->buffer_count = 20;

    pecho->history = (DSP_SAMPLE **) malloc(MAX_ECHO_COUNT * sizeof(DSP_SAMPLE *));
    pecho->index = (int *) calloc(MAX_ECHO_COUNT, sizeof(int));
    pecho->size = (int *) malloc(MAX_ECHO_COUNT * sizeof(int));
    pecho->factor = (int *) malloc(MAX_ECHO_COUNT * sizeof(int));

    while (k < MAX_ECHO_COUNT) {
	while (!prime(i))
	    i++;
	i++;
	while (!prime(i))
	    i++;
	pecho->factor[k] = i;
	k++;
	i++;
    }

    for (i = 0; i < MAX_ECHO_COUNT; i++) {
	pecho->size[i] = pecho->factor[i] * MAX_ECHO_SIZE;
	pecho->history[i] = (DSP_SAMPLE *) calloc(pecho->size[i], sizeof(DSP_SAMPLE));
    }
}
