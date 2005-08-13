/*
 * GNUitar
 * Vibrato effect
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
 * Revision 1.15  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.14  2005/08/12 17:56:16  alankila
 * use one global sin lookup table
 *
 * Revision 1.12  2005/08/08 18:34:45  alankila
 * - rename effects:
 *   * vibrato -> tremolo
 *   * tremolo -> vibrato
 *   * distort2 -> overdrive
 *
 * Revision 1.11  2004/08/10 15:21:16  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.9  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.8  2003/03/09 20:53:16  fonin
 * Meaningful params "speed" and "amplitude". Structures were redesigned
 * for new feature of on-the-fly change of sampling params.
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
 * Revision 1.4  2003/01/29 19:33:42  fonin
 * Fixed array referencing error that caused random crashes.
 * Win32 port.
 *
 * Revision 1.3  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.2  2001/03/25 12:10:50  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:26  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "tremolo.h"
#include "backbuf.h"
#include <math.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "utils.h"
#include "gui.h"

void            tremolo_filter(struct effect *p, struct data_block *db);

void
update_tremolo_speed(GtkAdjustment * adj, struct tremolo_params *params)
{
    params->tremolo_speed =
	(int) ((float) sample_rate * nchannels * adj->value / 1000.0);
}

void
update_tremolo_amplitude(GtkAdjustment * adj,
			 struct tremolo_params *params)
{
    params->tremolo_amplitude = (int) adj->value * 20;
}

void
toggle_tremolo(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = tremolo_filter;
	p->toggle = 1;
    }
}


void
tremolo_init(struct effect *p)
{
    struct tremolo_params *ptremolo;

    GtkWidget      *speed;
    GtkWidget      *speed_label;
    GtkObject      *adj_speed;

    GtkWidget      *ampl;
    GtkWidget      *ampl_label;
    GtkObject      *adj_ampl;

    GtkWidget      *button;

    GtkWidget      *parmTable;

    ptremolo = (struct tremolo_params *) p->params;


    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_speed = gtk_adjustment_new(ptremolo->tremolo_speed * 1000.0 /
				   (sample_rate * nchannels),
				   1.0, (MAX_TREMOLO_BUFSIZE * 1000.0 /
					 (sample_rate * nchannels)), 1.0,
				   1.0, 0.0);
    speed_label = gtk_label_new("Speed\n1/ms");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_tremolo_speed), ptremolo);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_ampl = gtk_adjustment_new(ptremolo->tremolo_amplitude / 20,
				  0.0, 100.0, 1.0, 1.0, 0.0);
    ampl_label = gtk_label_new("Amplitude\n%");
    gtk_table_attach(GTK_TABLE(parmTable), ampl_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_ampl), "value_changed",
		       GTK_SIGNAL_FUNC(update_tremolo_amplitude),
		       ptremolo);

    ampl = gtk_vscale_new(GTK_ADJUSTMENT(adj_ampl));

    gtk_table_attach(GTK_TABLE(parmTable), ampl, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_tremolo), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 2, 3,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Vibrato"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

void
tremolo_filter(struct effect *p, struct data_block *db)
{
    struct tremolo_params *tp;
    double          pos;
    int             count,
                    currchannel = 0;
    DSP_SAMPLE     *s;

    tp = p->params;
    s = db->data;
    count = db->len;

    while (count) {
        backbuff_add(tp->history[currchannel], *s);
	
	tp->tremolo_phase++;
	if (tp->tremolo_phase >= tp->tremolo_speed)
	    tp->tremolo_phase = 0;

	pos = tp->tremolo_amplitude * (1.0 + sin_lookup((double) tp->tremolo_phase / tp->tremolo_speed));
	*s = backbuff_get_interpolated(tp->history[currchannel], pos);

	if (nchannels > 1)
	    currchannel = !currchannel;

	s++;
	count--;
    }
}

void
tremolo_done(struct effect *p)
{
    struct tremolo_params *tp;
    int             i;

    tp = (struct tremolo_params *) p->params;

    for (i = 0; i < MAX_CHANNELS; i++) {
	backbuff_done(tp->history[i]);
        free(tp->history[i]);
    }

    free(tp);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;

}

void
tremolo_save(struct effect *p, int fd)
{
    struct tremolo_params *tp;

    tp = (struct tremolo_params *) p->params;

    write(fd, &tp->tremolo_amplitude, sizeof(tp->tremolo_amplitude));
    write(fd, &tp->tremolo_speed, sizeof(tp->tremolo_speed));
}

void
tremolo_load(struct effect *p, int fd)
{
    struct tremolo_params *tp;

    tp = (struct tremolo_params *) p->params;

    read(fd, &tp->tremolo_amplitude, sizeof(tp->tremolo_amplitude));
    read(fd, &tp->tremolo_speed, sizeof(tp->tremolo_speed));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = tremolo_filter;
    }
}

void
tremolo_create(struct effect *p)
{
    struct tremolo_params *ptremolo;
    int             i;

    p->params =
	(struct tremolo_params *) malloc(sizeof(struct tremolo_params));
    p->proc_init = tremolo_init;
    p->proc_load = tremolo_load;
    p->proc_save = tremolo_save;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->proc_done = tremolo_done;
    p->id = TREMOLO;

    ptremolo = (struct tremolo_params *) p->params;

    ptremolo->tremolo_size = MAX_TREMOLO_BUFSIZE;
    ptremolo->tremolo_amplitude = 25;
    ptremolo->tremolo_speed = MAX_TREMOLO_BUFSIZE * 0.2 / nchannels;

    for (i = 0; i < MAX_CHANNELS; i++) {
        ptremolo->history[i] = calloc(1, sizeof(ptremolo->history[0]));
	backbuff_init(ptremolo->history[i], ptremolo->tremolo_size);
    }
    ptremolo->tremolo_phase = 0;
}
