/*
 * GNUitar
 * Tremolo effect
 * Copyright (C) 2000,2001 Max Rudensky         <fonin@ziet.zhitomir.ua>
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
#include <math.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    define M_PI 3.14159265358979323846E0
#    include <io.h>
#endif
#include "gui.h"

void            tremolo_filter(struct effect *p, struct data_block *db);

void
update_tremolo_speed(GtkAdjustment * adj, struct tremolo_params *params)
{
    params->tremolo_speed = (int) adj->value;
}

void
update_tremolo_amplitude(GtkAdjustment * adj,
			 struct tremolo_params *params)
{
    params->tremolo_amplitude = (int) adj->value;
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
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_speed = gtk_adjustment_new(ptremolo->tremolo_speed,
				   3000.0, 35500.0, 1.0, 1.0, 1.0);
    speed_label = gtk_label_new("Speed");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_tremolo_speed), ptremolo);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));

    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_ampl = gtk_adjustment_new(ptremolo->tremolo_amplitude,
				  1.0, 1000.0, 1.0, 1.0, 1.0);
    ampl_label = gtk_label_new("Amplitude");
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

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Tremolo"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

void
tremolo_filter(struct effect *p, struct data_block *db)
{
    struct tremolo_params *tp;
    int             ef_index,
                   *s,
                    count;

    tp = p->params;

    s = db->data;
    count = db->len;

    while (count) {
	tp->history[tp->index++] = *s;	/* 
					 * add sample to history 
					 */
	if (tp->index == tp->tremolo_size)
	    tp->index = 0;	/* 
				 * wrap around 
				 */

	ef_index = tp->index;
	if (tp->index < tp->tremolo_index)
	    ef_index += tp->tremolo_size;
	tp->tremolo_phase++;
	if (tp->tremolo_phase >= tp->tremolo_speed)
	    tp->tremolo_phase = 0;

	tp->tremolo_index =
	    ef_index - tp->tremolo_amplitude - tp->tremolo_amplitude -
	    tp->phase_buffer[tp->tremolo_phase *
			     tp->tremolo_phase_buffer_size /
			     tp->tremolo_speed];
	if (tp->tremolo_index >= tp->tremolo_size)
	    tp->tremolo_index -= tp->tremolo_size;
	if (tp->tremolo_index < 0)
	    tp->tremolo_index = 0;
	*s = tp->history[tp->tremolo_index];

	s++;
	count--;
    }
}

void
tremolo_done(struct effect *p)
{
    struct tremolo_params *tp;

    tp = (struct tremolo_params *) p->params;

    free(tp->history);
    free(tp->phase_buffer);

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

    ptremolo->tremolo_phase_buffer_size = 60000;
    ptremolo->tremolo_size = 60000;
    ptremolo->tremolo_amplitude = 25;
    ptremolo->tremolo_speed = 8000;

    ptremolo->history =
	(int *) malloc(ptremolo->tremolo_size * sizeof(int));
    ptremolo->phase_buffer =
	(int *) malloc(ptremolo->tremolo_phase_buffer_size * sizeof(int));

    ptremolo->tremolo_index = 0;
    ptremolo->tremolo_phase = 0;

    for (i = 0; i < ptremolo->tremolo_phase_buffer_size; i++) {
	ptremolo->phase_buffer[i] = (int) ((double)
					   ptremolo->tremolo_amplitude *
					   sin(2 * M_PI * ((double)
							   i / (double)
							   ptremolo->
							   tremolo_phase_buffer_size)));
    }
    ptremolo->index = 0;
}
