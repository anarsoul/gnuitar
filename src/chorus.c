/*
 * $Id$
 *
 * $Log$
 * Revision 1.2  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:15  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "chorus.h"
#include "backbuf.h"
#include "gui.h"
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int             sinLookUp[360];
short           isSinLookUp = 0;

void
                chorus_filter(struct effect *p, struct data_block *db);

void
update_chorus_speed(GtkAdjustment * adj, struct chorus_params *params)
{
    params->speed = (int) adj->value;
}

void
update_chorus_depth(GtkAdjustment * adj, struct chorus_params *params)
{
    params->depth = (int) adj->value;
}

void
update_chorus_mode(GtkAdjustment * adj, struct chorus_params *params)
{
    if (params->mode == 0) {
	params->mode = 1;
    } else {
	params->mode = 0;
    }
}

void
update_chorus_wet(GtkAdjustment * adj, struct chorus_params *params)
{
    params->wet = (int) adj->value;
}

void
update_chorus_dry(GtkAdjustment * adj, struct chorus_params *params)
{
    params->dry = (int) adj->value;
}

void
update_chorus_regen(GtkAdjustment * adj, struct chorus_params *params)
{
    params->regen = (int) adj->value;
}

void
toggle_chorus(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = chorus_filter;
	p->toggle = 1;
    }
}


void
chorus_init(struct effect *p)
{
    struct chorus_params *pchorus;

    GtkWidget      *speed;
    GtkWidget      *speed_label;
    GtkObject      *adj_speed;

    GtkWidget      *depth;
    GtkWidget      *depth_label;
    GtkObject      *adj_depth;

    GtkWidget      *wet;
    GtkWidget      *wet_label;
    GtkObject      *adj_wet;

    GtkWidget      *dry;
    GtkWidget      *dry_label;
    GtkObject      *adj_dry;

    GtkWidget      *regen;
    GtkWidget      *regen_label;
    GtkObject      *adj_regen;

    GtkWidget      *button;
    GtkWidget      *flange;

    GtkWidget      *parmTable;

    pchorus = (struct chorus_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    rnd_window_pos(GTK_WINDOW(p->control));

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_speed =
	gtk_adjustment_new(pchorus->speed, 0.0, 100.0, 0.1, 1.0, 1.0);
    speed_label = gtk_label_new("Speed");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_speed), pchorus);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));

    gtk_table_attach(GTK_TABLE(parmTable), speed, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_depth =
	gtk_adjustment_new(pchorus->depth, 0.0, 50.0, 1.0, 1.0, 1.0);
    depth_label = gtk_label_new("Depth");
    gtk_table_attach(GTK_TABLE(parmTable), depth_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_depth), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_depth), pchorus);

    depth = gtk_vscale_new(GTK_ADJUSTMENT(adj_depth));

    gtk_table_attach(GTK_TABLE(parmTable), depth, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_wet = gtk_adjustment_new(pchorus->wet, 0.0, 256.0, 1.0, 1.0, 1.0);
    wet_label = gtk_label_new("Wet");
    gtk_table_attach(GTK_TABLE(parmTable), wet_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_wet), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_wet), pchorus);

    wet = gtk_vscale_new(GTK_ADJUSTMENT(adj_wet));

    gtk_table_attach(GTK_TABLE(parmTable), wet, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_dry = gtk_adjustment_new(pchorus->dry, 0.0, 256.0, 1.0, 1.0, 1.0);
    dry_label = gtk_label_new("Dry");
    gtk_table_attach(GTK_TABLE(parmTable), dry_label, 7, 8, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_wet), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_dry), pchorus);

    dry = gtk_vscale_new(GTK_ADJUSTMENT(adj_dry));

    gtk_table_attach(GTK_TABLE(parmTable), dry, 7, 8, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_regen =
	gtk_adjustment_new(pchorus->regen, 0.0, 50.0, 1.0, 1.0, 1.0);
    regen_label = gtk_label_new("Regen");
    gtk_table_attach(GTK_TABLE(parmTable), regen_label, 9, 10, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_regen), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_regen), pchorus);

    regen = gtk_vscale_new(GTK_ADJUSTMENT(adj_regen));

    gtk_table_attach(GTK_TABLE(parmTable), regen, 9, 10, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);



    flange = gtk_check_button_new_with_label("Flange");
    gtk_signal_connect(GTK_OBJECT(flange), "toggled",
		       GTK_SIGNAL_FUNC(update_chorus_mode), pchorus);

    gtk_table_attach(GTK_TABLE(parmTable), flange, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (pchorus->mode == 1) {
	pchorus->mode = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(flange), TRUE);
    }


    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_chorus), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 5, 6,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Chorus"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
chorus_filter(struct effect *p, struct data_block *db)
{
    struct chorus_params *cp;
    int             count;
    int            *s;
    int             dly = 0;
    float           AngInc,
                    Ang;
    int             tmp,
                    tot,
                    rgn;

    cp = (struct chorus_params *) p->params;

    s = db->data;
    count = db->len;


#define MaxDly ((int)cp->depth * 8)
    AngInc = cp->speed / 1000.0f;
    Ang = cp->ang;

    /*
     * process the samples 
     */
    while (count) {
	tmp = *s;
	tmp *= cp->dry;
	tmp /= 256;
	switch (cp->mode) {
	case 0:		/*
				 * * chorus 
				 */
	    dly = MaxDly * (1024 + sinLookUp[(int) cp->ang]);
	    dly /= 2048;
	    Ang += AngInc;
	    if (Ang >= 359.0f)
		Ang = 0.0f;
	    if (dly < 0)
		dly = 0;
	    break;
	case 1:		/*
				 * * flange 
				 */
	    dly = 16 * MaxDly * (1024 + sinLookUp[(int) Ang] / 16);
	    dly /= 2048;
	    Ang += AngInc;
	    if (Ang >= 359.0f)
		Ang = 0.0f;
	    if (dly < 0)
		dly = 0;
	    break;
	};

	tot = backbuff_get(cp->memory, (unsigned int) dly);
	tot *= cp->wet;
	tot /= 256;
	tot += tmp;
	tot = (tot < -32767) ? -32767 : (tot > 32767) ? 32767 : tot;
	rgn =
	    backbuff_get(cp->memory,
			 (unsigned int) dly * cp->regen) / 256 + *s;
	rgn = (rgn < -32767) ? -32767 : (rgn > 32767) ? 32767 : rgn;
	backbuff_add(cp->memory, rgn);
	*s = tot;

	s++;
	count--;

    }

    cp->ang = Ang;

#undef MaxDly
}

void
chorus_done(struct effect *p)
{
    struct chorus_params *cp;

    cp = (struct chorus_params *) p->params;

    backbuff_done(cp->memory);
    free(cp->memory);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
initSinLookUp(void)
{
    int             i;
    float           arg,
                    val;

    if (isSinLookUp)
	return;
    printf("\nInitializing sin lookup table");
    for (i = 0; i < 360; i++) {
	arg = ((float) i * M_PI) / 180.0f;
	val = sin(arg);
	sinLookUp[i] = (int) (val * 1024);
    }
    isSinLookUp = 1;
}

void
chorus_save(struct effect *p, int fd)
{
    struct chorus_params *cp;

    cp = (struct chorus_params *) p->params;

    write(fd, &cp->wet, sizeof(int));
    write(fd, &cp->dry, sizeof(int));
    write(fd, &cp->depth, sizeof(int));
    write(fd, &cp->mode, sizeof(short));
    write(fd, &cp->speed, sizeof(int));
    write(fd, &cp->regen, sizeof(int));
}

void
chorus_load(struct effect *p, int fd)
{
    struct chorus_params *cp;

    cp = (struct chorus_params *) p->params;

    read(fd, &cp->wet, sizeof(int));
    read(fd, &cp->dry, sizeof(int));
    read(fd, &cp->depth, sizeof(int));
    read(fd, &cp->mode, sizeof(short));
    read(fd, &cp->speed, sizeof(int));
    read(fd, &cp->regen, sizeof(int));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = chorus_filter;
    }
}

void
chorus_create(struct effect *p)
{
    struct chorus_params *cp;
    p->params =
	(struct chorus_params *) malloc(sizeof(struct chorus_params));
    p->proc_init = chorus_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->id = CHORUS;
    p->proc_done = chorus_done;
    p->proc_save = chorus_save;
    p->proc_load = chorus_load;

    cp = (struct chorus_params *) p->params;
    cp->memory = (struct backBuf *) malloc(sizeof(struct backBuf));
    backbuff_init(cp->memory, SAMPLE_RATE);	/*
						 * 1 second memory 
						 */

    cp->ang = 0.0f;
    cp->depth = 50;
    cp->speed = 5;
    cp->mode = 0;
    cp->wet = 100;
    cp->dry = 200;
    cp->regen = 0;
}
