/*
 * $Id$
 *
 * $Log$
 * Revision 1.2  2001/03/25 12:10:50  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:31  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "vibrato.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "gui.h"

void
                vibrato_filter(struct effect *p, struct data_block *db);

void
update_vibrato_speed(GtkAdjustment * adj, struct vibrato_params *params)
{
    params->vibrato_speed = (int) adj->value;
}

void
update_vibrato_ampl(GtkAdjustment * adj, struct vibrato_params *params)
{
    int             i;
    params->vibrato_amplitude = (int) adj->value;
    for (i = 0; i < params->vibrato_phase_buffer_size; i++) {
	params->phase_buffer[i] = (int) (
					 (double) params->vibrato_amplitude
					 * sin(2 * M_PI * ((double)
							   i / (double)
							   params->
							   vibrato_phase_buffer_size)));
    }

}

void
toggle_vibrato(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = vibrato_filter;
	p->toggle = 1;
    }
}

void
vibrato_init(struct effect *p)
{
    struct vibrato_params *pvibrato;

    GtkWidget      *speed;
    GtkWidget      *speed_label;
    GtkObject      *adj_speed;

    GtkWidget      *ampl;
    GtkWidget      *ampl_label;
    GtkObject      *adj_ampl;

    GtkWidget      *button;
    GtkWidget      *parmTable;

    pvibrato = (struct vibrato_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    rnd_window_pos(GTK_WINDOW(p->control));

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_speed = gtk_adjustment_new(pvibrato->vibrato_speed,
				   5000.0,
				   MAX_VIBRATO_BUFSIZE, 0.1, 1.0, 1.0);
    speed_label = gtk_label_new("Speed");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_vibrato_speed), pvibrato);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));

    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_ampl = gtk_adjustment_new(pvibrato->vibrato_amplitude,
				  100.0, 5000.0, 1.0, 1.0, 1.0);
    ampl_label = gtk_label_new("Amplitude");
    gtk_table_attach(GTK_TABLE(parmTable), ampl_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_ampl), "value_changed",
		       GTK_SIGNAL_FUNC(update_vibrato_ampl), pvibrato);

    ampl = gtk_vscale_new(GTK_ADJUSTMENT(adj_ampl));

    gtk_table_attach(GTK_TABLE(parmTable), ampl, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_vibrato), p);

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
vibrato_filter(struct effect *p, struct data_block *db)
{
    int             ratio,
                   *s,
                    count;
    struct vibrato_params *vp;

    s = db->data;
    count = db->len;

    vp = p->params;

    while (count) {
	ratio = 1000 + vp->phase_buffer[vp->vibrato_phase];
	*s = *s * ratio / 1000;

	vp->vibrato_phase++;
	if (vp->vibrato_phase >= vp->vibrato_speed)
	    vp->vibrato_phase = 0;

	s++;
	count--;
    }
}

void
vibrato_done(struct effect *p)
{
    struct vibrato_params *vp;

    vp = p->params;

    free(vp->phase_buffer);
    free(vp);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;

}

void
vibrato_save(struct effect *p, int fd)
{
    struct vibrato_params *vp;

    vp = (struct vibrato_params *) p->params;

    write(fd, &vp->vibrato_speed, sizeof(int));
    write(fd, &vp->vibrato_amplitude, sizeof(int));
}

void
vibrato_load(struct effect *p, int fd)
{
    int             i;
    struct vibrato_params *vp;

    vp = (struct vibrato_params *) p->params;

    read(fd, &vp->vibrato_speed, sizeof(int));
    read(fd, &vp->vibrato_amplitude, sizeof(int));
    for (i = 0; i < vp->vibrato_phase_buffer_size; i++) {
	vp->phase_buffer[i] = (int) (
				     (double) vp->vibrato_amplitude *
				     sin(2 * M_PI * ((double) i / (double)
						     vp->
						     vibrato_phase_buffer_size)));
    }

    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = vibrato_filter;
    }
}

void
vibrato_create(struct effect *p)
{
    int             i;
    struct vibrato_params *pvibrato;

    p->params =
	(struct vibrato_params *) malloc(sizeof(struct vibrato_params));
    p->proc_init = vibrato_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->proc_done = vibrato_done;
    p->id = VIBRATO;
    p->proc_load = vibrato_load;
    p->proc_save = vibrato_save;

    pvibrato = (struct vibrato_params *) p->params;

    pvibrato->vibrato_amplitude = 800;
    pvibrato->vibrato_speed = 8000;
    pvibrato->vibrato_phase_buffer_size = MAX_VIBRATO_BUFSIZE;

    pvibrato->phase_buffer =
	(int *) malloc(MAX_VIBRATO_BUFSIZE * sizeof(int));
    pvibrato->vibrato_phase = 0;

    for (i = 0; i < pvibrato->vibrato_phase_buffer_size; i++) {
	pvibrato->phase_buffer[i] = (int) ((double)
					   pvibrato->vibrato_amplitude *
					   sin(2 * M_PI * ((double)
							   i / (double)
							   pvibrato->
							   vibrato_phase_buffer_size)));
    }
}
