/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/01/11 13:21:41  fonin
 * Initial revision
 *
 */

#include "echo.h"
#include "gui.h"
#include <stdlib.h>
#include <unistd.h>

void
                echo_filter(struct effect *p, struct data_block *db);

void
update_echo_decay(GtkAdjustment * adj, struct echo_params *params)
{
    params->echo_decay = (int) adj->value;
}

void
update_echo_count(GtkAdjustment * adj, struct echo_params *params)
{
    params->buffer_count = (int) adj->value;
}

void
update_echo_size(GtkAdjustment * adj, struct echo_params *params)
{
    params->echo_size = (int) adj->value;
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
    rnd_window_pos(GTK_WINDOW(p->control));
    parmTable = gtk_table_new(2, 8, FALSE);

    adj_decay = gtk_adjustment_new(pecho->echo_decay,
				   1.0, 1000.0, 0.1, 1.0, 1.0);
    decay_label = gtk_label_new("Decay");
    gtk_table_attach(GTK_TABLE(parmTable), decay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_decay), "value_changed",
		       GTK_SIGNAL_FUNC(update_echo_decay), pecho);

    decay = gtk_vscale_new(GTK_ADJUSTMENT(adj_decay));

    gtk_table_attach(GTK_TABLE(parmTable), decay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_count = gtk_adjustment_new(pecho->buffer_count,
				   1.0, MAX_ECHO_COUNT, 1.0, 1.0, 1.0);
    count_label = gtk_label_new("Count");
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

    adj_size = gtk_adjustment_new(pecho->echo_size,
				  1.0, MAX_ECHO_SIZE, 1.0, 1.0, 1.0);
    size_label = gtk_label_new("Size");
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
    int            *s,
                    j,
                    sample,
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

    write(fd, &ep->echo_size, sizeof(int));
    write(fd, &ep->echo_decay, sizeof(int));
    write(fd, &ep->buffer_count, sizeof(int));
}

void
echo_load(struct effect *p, int fd)
{
    struct echo_params *ep;

    ep = (struct echo_params *) p->params;

    read(fd, &ep->echo_size, sizeof(int));
    read(fd, &ep->echo_decay, sizeof(int));
    read(fd, &ep->buffer_count, sizeof(int));
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
                    j,
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

    pecho->history = (int **) malloc(MAX_ECHO_COUNT * sizeof(int *));
    pecho->index = (int *) malloc(MAX_ECHO_COUNT * sizeof(int));
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
	pecho->history[i] = (int *) malloc(pecho->size[i] * sizeof(int));
	for (j = 0; j < pecho->size[i]; j++)
	    pecho->history[i][j] = 0;
	pecho->index[i] = 0;
    }
}
