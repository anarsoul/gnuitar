/*
 * GNUitar
 * Distortion effect
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
 * Revision 1.2  2003/04/12 20:00:56  fonin
 * Stupid bugfix (forgot to move forward buffer pointer
 * in the filter function); "level" control taken out.
 *
 * Revision 1.1  2003/04/11 18:32:24  fonin
 * New distortion effect.
 *
 */

#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#    include <fcntl.h>
#else
#    include <io.h>
#endif
#include "distort2.h"
#include "gui.h"

SAMPLE      tube[MAX_SAMPLE+1];	/* distortion lookup table */

void            distort2_filter(struct effect *p, struct data_block *db);

void load_distort2_lookup(int r1, int r2, int sr) {
    char filename[255]="";
    char tmp[255]="";
    int in;
    int i;

    strcpy(filename,"distort2/distort2lookup_");
//    sprintf(tmp,"%i",sr);
    strcat(filename,"44100");
    strncat(filename,tmp,255);
    strcat(filename,"_");
    sprintf(tmp,"%i",r1);
    strncat(filename,tmp,255);
    strcat(filename,"_");
    sprintf(tmp,"%i",r2);
    strncat(filename,tmp,255);
    in=open(filename,O_RDONLY);
    if(in==-1) {
	perror("open");
	return;
    }
    memset(tube,0,sizeof(tube));
    read(in,tube,sizeof(tube));
/*    printf("\n");
    for(i=0;i<=255;i++)
        printf("%i=%i;",i,tube[i]);*/
}

void
update_distort2_r1(GtkAdjustment * adj, struct distort2_params *params)
{
    params->r1 = (int) adj->value;
printf("\nr1=%i",params->r1);
    load_distort2_lookup(params->r1,params->r2,sample_rate);
}

void
update_distort2_drive(GtkAdjustment * adj, struct distort2_params *params)
{
    params->r2 = (int) adj->value * 4.8;
    params->r2-=params->r2 % 10;
    params->r2+=50;
printf("\nr2=%i",params->r2);
    load_distort2_lookup(params->r1,params->r2,sample_rate);
}

void
update_distort2_lowpass(GtkAdjustment * adj, struct distort2_params *params)
{
    params->lowpass = (int) adj->value;
}


void
toggle_distort2(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = distort2_filter;
	p->toggle = 1;
    }
}

void
distort2_init(struct effect *p)
{
    struct distort2_params *pdistort;

    GtkWidget      *drive;
    GtkWidget      *drive_label;
    GtkObject      *adj_drive;

    GtkWidget      *r1;
    GtkWidget      *r1_label;
    GtkObject      *adj_r1;

    GtkWidget      *lowpass;
    GtkWidget      *lowpass_label;
    GtkObject      *adj_lowpass;

    GtkWidget      *button;

    GtkWidget      *parmTable;

    pdistort = (struct distort2_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_drive = gtk_adjustment_new((pdistort->r2 + 50) / 4.8,
				   1.0, 101.0, 1, 1, 1);
    drive_label = gtk_label_new("Drive\n%");
    gtk_table_attach(GTK_TABLE(parmTable), drive_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_drive), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_drive), pdistort);

    drive = gtk_vscale_new(GTK_ADJUSTMENT(adj_drive));

    gtk_table_attach(GTK_TABLE(parmTable), drive, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_r1 =
	gtk_adjustment_new(pdistort->r1, 1.0, 101, 1.0, 1.0, 1.0);
    r1_label = gtk_label_new("R1\n%");
    gtk_table_attach(GTK_TABLE(parmTable), r1_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_r1), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_r1), pdistort);

    r1 = gtk_vscale_new(GTK_ADJUSTMENT(adj_r1));

    gtk_table_attach(GTK_TABLE(parmTable), r1, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_lowpass =
	gtk_adjustment_new(pdistort->lowpass, 1.0, 3000, 1.0, 1.0, 1.0);
    lowpass_label = gtk_label_new("Lowpass\nHz");
    gtk_table_attach(GTK_TABLE(parmTable), lowpass_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_lowpass), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_lowpass), pdistort);

    lowpass = gtk_vscale_new(GTK_ADJUSTMENT(adj_lowpass));

    gtk_table_attach(GTK_TABLE(parmTable), lowpass, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_distort2), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Distortion 2"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
distort2_filter(struct effect *p, struct data_block *db)
{
    int             count,
                    currchannel = 0,
                   *s;
    struct distort2_params *dp;

    dp = (struct distort2_params *) p->params;

    count = db->len;
    s = db->data;

    RC_highpass(db->data, db->len, &(dp->fd));

    /* process signal */
    while (count) {
	int tmp;
	tmp=*s;
	if(tmp>0 && tmp<MAX_SAMPLE) {
	    *s = tube[tmp];
	}
	else if(tmp<0 && tmp>-MAX_SAMPLE) {
	    *s = -tube[-tmp];
	}
	s++;
	count--;
    }

    LC_filter(db->data, db->len, LOWPASS, dp->lowpass, &(dp->noise));
}

void
distort2_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
distort2_save(struct effect *p, int fd)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;

    write(fd, &ap->r1, sizeof(ap->r1));
    write(fd, &ap->r2, sizeof(ap->r2));
    write(fd, &ap->lowpass, sizeof(ap->lowpass));
}

void
distort2_load(struct effect *p, int fd)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;

    read(fd, &ap->r1, sizeof(ap->r1));
    read(fd, &ap->r2, sizeof(ap->r2));
    read(fd, &ap->lowpass, sizeof(ap->lowpass));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = distort2_filter;
    }
}


void
distort2_create(struct effect *p)
{
    struct distort2_params *ap;

    p->params =
	(struct distort2_params *) malloc(sizeof(struct distort2_params));
    ap = (struct distort2_params *) p->params;

    p->proc_init = distort2_init;
    p->proc_filter = passthru;
    p->proc_save = distort2_save;
    p->proc_load = distort2_load;
    p->toggle = 0;
    p->id = DISTORT2;
    p->proc_done = distort2_done;

    ap->r2 = 510;
    ap->r1 = 1;
    ap->lowpass = 350;
    ap->noisegate = 3000;

    RC_setup(10, 1.5, &(ap->fd));
    RC_set_freq(ap->lowpass, &(ap->fd));
    RC_setup(10, 1, &(ap->noise));
    RC_set_freq(ap->noisegate, &(ap->noise));
    load_distort2_lookup(ap->r1,ap->r2,sample_rate);
}
