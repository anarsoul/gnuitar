/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/01/11 13:21:41  fonin
 * Initial revision
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include "distort.h"
#include "gui.h"

void            distort_filter(struct effect *p, struct data_block *db);


void
update_distort_level(GtkAdjustment * adj, struct distort_params *params)
{
    params->level = (int) adj->value;
}

void
update_distort_sat(GtkAdjustment * adj, struct distort_params *params)
{
    params->sat = (int) adj->value;
}

void
update_distort_drive(GtkAdjustment * adj, struct distort_params *params)
{
    params->drive = (int) adj->value;
}

void
update_distort_lowpass(GtkAdjustment * adj, struct distort_params *params)
{
    params->lowpass = (int) adj->value;
}

void
toggle_clip(void *bullshit, struct distort_params *dp)
{
    if (dp->clip == 1) {
	dp->clip = 0;
    } else {
	dp->clip = 1;
    }
}


void
toggle_distort(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = distort_filter;
	p->toggle = 1;
    }
}

void
distort_init(struct effect *p)
{
    struct distort_params *pdistort;

    GtkWidget      *drive;
    GtkWidget      *drive_label;
    GtkObject      *adj_drive;

    GtkWidget      *sat;
    GtkWidget      *sat_label;
    GtkObject      *adj_sat;

    GtkWidget      *level;
    GtkWidget      *level_label;
    GtkObject      *adj_level;

    GtkWidget      *lowpass;
    GtkWidget      *lowpass_label;
    GtkObject      *adj_lowpass;


    GtkWidget      *button;
    GtkWidget      *clip;

    GtkWidget      *parmTable;


    pdistort = (struct distort_params *) p->params;


    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    rnd_window_pos(GTK_WINDOW(p->control));

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_drive = gtk_adjustment_new(pdistort->drive,
				   1.0, 1000.0, 1.0, 1.0, 1.0);
    drive_label = gtk_label_new("Drive");
    gtk_table_attach(GTK_TABLE(parmTable), drive_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_drive), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort_drive), pdistort);

    drive = gtk_vscale_new(GTK_ADJUSTMENT(adj_drive));

    gtk_table_attach(GTK_TABLE(parmTable), drive, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_level =
	gtk_adjustment_new(pdistort->level, 1.0, 255, 1.0, 1.0, 1.0);
    level_label = gtk_label_new("Level");
    gtk_table_attach(GTK_TABLE(parmTable), level_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_level), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort_level), pdistort);

    level = gtk_vscale_new(GTK_ADJUSTMENT(adj_level));

    gtk_table_attach(GTK_TABLE(parmTable), level, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_sat = gtk_adjustment_new(pdistort->sat, 1.0, 65000, 1.0, 1.0, 1.0);
    sat_label = gtk_label_new("Saturation");
    gtk_table_attach(GTK_TABLE(parmTable), sat_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_sat), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort_sat), pdistort);

    sat = gtk_vscale_new(GTK_ADJUSTMENT(adj_sat));

    gtk_table_attach(GTK_TABLE(parmTable), sat, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_lowpass =
	gtk_adjustment_new(pdistort->lowpass, 1.0, 3000, 1.0, 1.0, 1.0);
    lowpass_label = gtk_label_new("Lowpass");
    gtk_table_attach(GTK_TABLE(parmTable), lowpass_label, 7, 8, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_lowpass), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort_lowpass), pdistort);

    lowpass = gtk_vscale_new(GTK_ADJUSTMENT(adj_lowpass));

    gtk_table_attach(GTK_TABLE(parmTable), lowpass, 7, 8, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_distort), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    clip = gtk_check_button_new_with_label("Clip");
    gtk_signal_connect(GTK_OBJECT(clip), "toggled",
		       GTK_SIGNAL_FUNC(toggle_clip), pdistort);

    gtk_table_attach(GTK_TABLE(parmTable), clip, 5, 6, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (pdistort->clip == 1) {
	pdistort->clip = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(clip), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Distortion"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
distort_filter(struct effect *p, struct data_block *db)
{
    int             count,
                   *s;
    struct distort_params *dp;
    dp = (struct distort_params *) p->params;
    /*
     * sat clips derivative by limiting difference between samples. use Extra 
     * member to store last sample for seamlessness between chunks. 
     */
    count = db->len;
    s = db->data;
    RC_highpass(db->data, db->len, &(dp->fd));

    while (count) {
	/*
	 * apply drive  
	 */
	*s *= dp->drive;
	*s /= 16;

	/*
	 * apply sat 
	 */
	if ((*s - dp->lastval) > dp->sat)
	    *s = dp->lastval + dp->sat;
	else if ((dp->lastval - *s) > dp->sat)
	    *s = dp->lastval - dp->sat;

	dp->lastval = *s;

	/*
	 * apply Clip 
	 */
	if (dp->clip)
	    *s = (*s > 32767) ? 32767 : (*s < -32767) ? -32767 : *s;
	/*
	 * apply level 
	 */
	*s *= dp->level;
	*s /= 256;
	s++;
	count--;
    }
    LC_filter(db->data, db->len, LOWPASS, dp->lowpass, &(dp->noise));
}

void
distort_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
distort_save(struct effect *p, int fd)
{
    struct distort_params *ap;

    ap = (struct distort_params *) p->params;

    write(fd, &ap->sat, sizeof(int));
    write(fd, &ap->level, sizeof(int));
    write(fd, &ap->drive, sizeof(int));
    write(fd, &ap->clip, sizeof(short));
    write(fd, &ap->lowpass, sizeof(int));
}

void
distort_load(struct effect *p, int fd)
{
    struct distort_params *ap;

    ap = (struct distort_params *) p->params;

    read(fd, &ap->sat, sizeof(int));
    read(fd, &ap->level, sizeof(int));
    read(fd, &ap->drive, sizeof(int));
    read(fd, &ap->clip, sizeof(short));
    read(fd, &ap->lowpass, sizeof(int));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = distort_filter;
    }
}


void
distort_create(struct effect *p)
{
    struct distort_params *ap;

    p->params =
	(struct distort_params *) malloc(sizeof(struct distort_params));
    ap = (struct distort_params *) p->params;

    p->proc_init = distort_init;
    p->proc_filter = passthru;
    p->proc_save = distort_save;
    p->proc_load = distort_load;
    p->toggle = 0;
    p->id = DISTORT;
    p->proc_done = distort_done;

    ap->sat = 20000;
    ap->level = 100;
    ap->drive = 555;
    ap->clip = 0;
    ap->lowpass = 350;

    RC_setup(10, 1.5, &(ap->fd));
    RC_set_freq(350, &(ap->fd));

    RC_setup(10, 1, &(ap->noise));
    RC_set_freq(3000, &(ap->noise));
}
