/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/01/11 13:21:26  fonin
 * Initial revision
 *
 */

#include "delay.h"
#include "gui.h"
#include <stdlib.h>
#include <unistd.h>

void            delay_filter(struct effect *p, struct data_block *db);


void
update_delay_decay(GtkAdjustment * adj, struct delay_params *params)
{
    params->delay_decay = (int) adj->value;
}

void
update_delay_time(GtkAdjustment * adj, struct delay_params *params)
{
    params->delay_start = (int) adj->value;
    params->delay_step = (int) adj->value;
}

void
update_delay_repeat(GtkAdjustment * adj, struct delay_params *params)
{
    params->delay_count = (int) adj->value;
}

void
toggle_delay(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = delay_filter;
	p->toggle = 1;
    }
}


void
delay_init(struct effect *p)
{
    struct delay_params *pdelay;

    GtkWidget      *decay;
    GtkWidget      *decay_label;
    GtkObject      *adj_decay;

    GtkWidget      *time;
    GtkWidget      *time_label;
    GtkObject      *adj_time;

    GtkWidget      *repeat;
    GtkWidget      *repeat_label;
    GtkObject      *adj_repeat;

    GtkWidget      *button;

    GtkWidget      *parmTable;


    pdelay = (struct delay_params *) p->params;


    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    rnd_window_pos(GTK_WINDOW(p->control));
    parmTable = gtk_table_new(2, 8, FALSE);

    adj_decay = gtk_adjustment_new(pdelay->delay_decay,
				   100.0, 1000.0, 1.0, 1.0, 1.0);
    decay_label = gtk_label_new("Decay");
    gtk_table_attach(GTK_TABLE(parmTable), decay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_decay), "value_changed",
		       GTK_SIGNAL_FUNC(update_delay_decay), pdelay);

    decay = gtk_vscale_new(GTK_ADJUSTMENT(adj_decay));

    gtk_table_attach(GTK_TABLE(parmTable), decay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_time = gtk_adjustment_new(pdelay->delay_step,
				  100.0, MAX_STEP, 1.0, 1.0, 1.0);
    time_label = gtk_label_new("Time");
    gtk_table_attach(GTK_TABLE(parmTable), time_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_time), "value_changed",
		       GTK_SIGNAL_FUNC(update_delay_time), pdelay);

    time = gtk_vscale_new(GTK_ADJUSTMENT(adj_time));

    gtk_table_attach(GTK_TABLE(parmTable), time, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_repeat = gtk_adjustment_new(pdelay->delay_count,
				    1.0, MAX_COUNT, 1.0, 1.0, 1.0);
    repeat_label = gtk_label_new("Repeat");
    gtk_table_attach(GTK_TABLE(parmTable), repeat_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_repeat), "value_changed",
		       GTK_SIGNAL_FUNC(update_delay_repeat), pdelay);

    repeat = gtk_vscale_new(GTK_ADJUSTMENT(adj_repeat));

    gtk_table_attach(GTK_TABLE(parmTable), repeat, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_delay), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Delay"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

void
delay_filter(struct effect *p, struct data_block *db)
{
    struct delay_params *dp;
    int             i,
                   *s,
                    count,
                    current_decay;

    dp = (struct delay_params *) p->params;

    s = db->data;
    count = db->len;

    while (count) {
	/*
	 * add sample to history 
	 */
	dp->history[dp->index++] = *s;
	/*
	 * wrap around 
	 */
	if (dp->index == dp->delay_size)
	    dp->index = 0;

	current_decay = dp->delay_decay;
	for (i = 0; i < dp->delay_count; i++) {
	    if (dp->index >= dp->idelay[i]) {
		if (dp->index - dp->idelay[i] ==
		    dp->delay_start + i * dp->delay_step) dp->idelay[i]++;
	    } else {
		if (dp->delay_size + dp->index - dp->idelay[i] ==
		    dp->delay_start + i * dp->delay_step)
		    dp->idelay[i]++;
	    }
	    if (dp->idelay[i] == dp->delay_size)
		dp->idelay[i] = 0;

	    *s += dp->history[dp->idelay[i]] * current_decay / 1000;
	    current_decay = current_decay * dp->delay_decay / 1000;
	}

	s++;
	count--;
    }
}

void
delay_done(struct effect *p)
{
    struct delay_params *dp;

    dp = (struct delay_params *) p->params;

    free(dp->history);
    free(dp->idelay);

    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
delay_save(struct effect *p, int fd)
{
    struct delay_params *dp;

    dp = (struct delay_params *) p->params;

    write(fd, &dp->delay_size, sizeof(int));
    write(fd, &dp->delay_decay, sizeof(int));
    write(fd, &dp->delay_start, sizeof(int));
    write(fd, &dp->delay_step, sizeof(short));
    write(fd, &dp->delay_count, sizeof(int));
}

void
delay_load(struct effect *p, int fd)
{
    struct delay_params *dp;

    dp = (struct delay_params *) p->params;

    read(fd, &dp->delay_size, sizeof(int));
    read(fd, &dp->delay_decay, sizeof(int));
    read(fd, &dp->delay_start, sizeof(int));
    read(fd, &dp->delay_step, sizeof(short));
    read(fd, &dp->delay_count, sizeof(int));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = delay_filter;
    }
}

void
delay_create(struct effect *p)
{
    struct delay_params *pdelay;
    int             i;

    p->params =
	(struct delay_params *) malloc(sizeof(struct delay_params));
    pdelay = p->params;
    p->proc_init = delay_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->id = DELAY;
    p->proc_done = delay_done;
    p->proc_save = delay_save;
    p->proc_load = delay_load;

    /*
     * 
     * Brian May echo stuff delay_start=delay_step=35000 delay_count=2
     * 
     * Diablo town music delay_start=delay_step=25000 delay_count=3
     * 
     */

    pdelay->delay_size = MAX_SIZE;
    pdelay->delay_decay = 700;
    pdelay->delay_start = 5000;
    pdelay->delay_step = 5000;
    pdelay->delay_count = 2;

    pdelay->history = (int *) malloc(MAX_SIZE * sizeof(int));
    pdelay->idelay = (int *) malloc(MAX_COUNT * sizeof(int));
    pdelay->index = 0;

    for (i = 0; i < MAX_COUNT; i++)
	pdelay->idelay[i] = 0;
}
