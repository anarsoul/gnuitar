/*
 * GNUitar
 * Chorus effect
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
 * Revision 1.22  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.21  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.20  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.19  2005/09/01 23:38:01  alankila
 * - make chorus sample_rate independent
 * - use units in chorus depth
 * - greatly decrease effect depth to make small depth settings easier
 *
 * Revision 1.18  2005/09/01 14:18:38  alankila
 * - multichannel ready chorus
 *
 * Revision 1.17  2005/09/01 13:36:23  alankila
 * Objectify backbuf, correct naming and make it a typedef.
 *
 * Revision 1.16  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.15  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.14  2005/08/12 17:56:16  alankila
 * use one global sin lookup table
 *
 * Revision 1.13  2005/08/10 18:42:49  alankila
 * - use interpolating backbuffer
 * - vastly extend the precision of the sin lookup table
 * - use the smoothly varying Ang instead of the discrete cp->ang to avoid
 *   snapping sounds due to discontinuities at fragment edges
 * - make dry % and wet % mixers do what they say
 *
 * Revision 1.12  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.11  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.10  2003/03/11 22:04:00  fonin
 * Measure control sliders in standard units (ms, %).
 *
 * Revision 1.9  2003/03/09 20:49:45  fonin
 * Structures were redesigned to allow to change sampling params.
 *
 * Revision 1.8  2003/02/05 21:10:10  fonin
 * Cleanup before release.
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
 * Revision 1.3  2001/06/02 13:59:04  fonin
 * Fixed bug caused backbuff_get() assertion in chorus_filter().
 * Added GNU disclaimer.
 *
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
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>

void chorus_filter(struct effect *p, struct data_block *db);

void
update_chorus_speed(GtkAdjustment * adj, struct chorus_params *params)
{
    params->speed = adj->value;
}

void
update_chorus_depth(GtkAdjustment * adj, struct chorus_params *params)
{
    params->depth = adj->value;
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
    params->wet = (int) adj->value * 2.56;
}

void
update_chorus_dry(GtkAdjustment * adj, struct chorus_params *params)
{
    params->dry = (int) adj->value * 2.56;
}

void
update_chorus_regen(GtkAdjustment * adj, struct chorus_params *params)
{
    params->regen = (int) adj->value * 2.56;
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

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(5, 3, FALSE);

    adj_speed = gtk_adjustment_new(pchorus->speed, 20.0, 3500,
                                   0.1, 1.0, 0.0);
    speed_label = gtk_label_new("Speed\n1/ms");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_speed), pchorus);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), speed, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_depth =
	gtk_adjustment_new(pchorus->depth, 0.001, 10.0, 1.0, 1.0, 0.0);
    depth_label = gtk_label_new("Depth\nms");
    gtk_table_attach(GTK_TABLE(parmTable), depth_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_depth), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_depth), pchorus);

    depth = gtk_vscale_new(GTK_ADJUSTMENT(adj_depth));
    gtk_scale_set_digits(GTK_SCALE(depth), 3);

    gtk_table_attach(GTK_TABLE(parmTable), depth, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_wet =
	gtk_adjustment_new(pchorus->wet / 2.56, 0.0, 100.0, 1.0, 1.0, 0.0);
    wet_label = gtk_label_new("Wet\n%");
    gtk_table_attach(GTK_TABLE(parmTable), wet_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_wet), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_wet), pchorus);

    wet = gtk_vscale_new(GTK_ADJUSTMENT(adj_wet));

    gtk_table_attach(GTK_TABLE(parmTable), wet, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_dry =
	gtk_adjustment_new(pchorus->dry / 2.56, 0.0, 100.0, 1.0, 1.0, 0.0);
    dry_label = gtk_label_new("Dry\n%");
    gtk_table_attach(GTK_TABLE(parmTable), dry_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_dry), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_dry), pchorus);

    dry = gtk_vscale_new(GTK_ADJUSTMENT(adj_dry));

    gtk_table_attach(GTK_TABLE(parmTable), dry, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_regen =
	gtk_adjustment_new(pchorus->regen / 2.56, 0.0, 100.0, 1.0, 1.0,
			   0.0);
    regen_label = gtk_label_new("Regen\n%");
    gtk_table_attach(GTK_TABLE(parmTable), regen_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_regen), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_regen), pchorus);

    regen = gtk_vscale_new(GTK_ADJUSTMENT(adj_regen));

    gtk_table_attach(GTK_TABLE(parmTable), regen, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    flange = gtk_check_button_new_with_label("Flange");
    gtk_signal_connect(GTK_OBJECT(flange), "toggled",
		       GTK_SIGNAL_FUNC(update_chorus_mode), pchorus);

    gtk_table_attach(GTK_TABLE(parmTable), flange, 1, 2, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);
    if (pchorus->mode == 1) {
	pchorus->mode = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(flange), TRUE);
    }

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_chorus), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
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
    DSP_SAMPLE     *s;
    double          dly = 0;
    float           AngInc,
                    Ang;
    DSP_SAMPLE      tmp,
                    tot,
                    rgn;
    int		    currchannel = 0;

    cp = (struct chorus_params *) p->params;

    s = db->data;
    count = db->len;

#define MaxDly (cp->depth / 1000.0 * sample_rate)
    AngInc = 1000.0 / cp->speed / sample_rate;
    Ang = cp->ang;

    /*
     * process the samples 
     */
    while (count) {
	tmp = *s;
	tmp *= cp->dry;
	tmp /= 256;

        if (currchannel == 0) {
            switch (cp->mode) {
                case 0:		/* chorus */
                    dly =      MaxDly * (1 + sin_lookup(Ang)     );
                    break;
                case 1:		/* flange */
                    dly = 16 * MaxDly * (1 + sin_lookup(Ang) / 16);
                    break;
            };
            Ang += AngInc;
            if (Ang >= 1.0)
                Ang -= 1.0;
            if (dly < 0)
                dly = 0;
        }

	tot = cp->history[currchannel]->get_interpolated(cp->history[currchannel], dly);
	tot *= cp->wet;
	tot /= 256;
	tot += tmp;
#ifdef CLIP_EVERYWHERE
	tot =
	    (tot < -MAX_SAMPLE) ? -MAX_SAMPLE : (tot >
						 MAX_SAMPLE) ? MAX_SAMPLE :
	    tot;
#endif
	rgn =
	    (cp->history[currchannel]->get_interpolated(cp->history[currchannel], dly) *
	     cp->regen) / 256 + *s;
#ifdef CLIP_EVERYWHERE
	rgn =
	    (rgn < -MAX_SAMPLE) ? -MAX_SAMPLE : (rgn >
						 MAX_SAMPLE) ? MAX_SAMPLE :
	    rgn;
#endif
	cp->history[currchannel]->add(cp->history[currchannel], rgn);
	*s = tot;

	currchannel = (currchannel + 1) % db->channels;
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
    int             i;

    cp = (struct chorus_params *) p->params;

    for (i = 0; i < MAX_CHANNELS; i++)
	del_Backbuf(cp->history[i]);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
chorus_save(struct effect *p, SAVE_ARGS)
{
    struct chorus_params *params = p->params;

    SAVE_DOUBLE("wet", params->wet);
    SAVE_DOUBLE("dry", params->dry);
    SAVE_DOUBLE("depth", params->depth);
    SAVE_DOUBLE("speed", params->speed);
    SAVE_DOUBLE("regen", params->regen);
    SAVE_INT("mode", params->mode);
}

void
chorus_load(struct effect *p, LOAD_ARGS)
{
    struct chorus_params *params = p->params;

    LOAD_DOUBLE("wet", params->wet);
    LOAD_DOUBLE("dry", params->dry);
    LOAD_DOUBLE("depth", params->depth);
    LOAD_DOUBLE("speed", params->speed);
    LOAD_DOUBLE("regen", params->regen);
    LOAD_INT("mode", params->mode);
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
    int             i;

    p->params = calloc(1, sizeof(struct chorus_params));
    p->proc_init = chorus_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->id = CHORUS;
    p->proc_done = chorus_done;
    p->proc_save = chorus_save;
    p->proc_load = chorus_load;
    cp = p->params;

    /* 320 ms history -- XXX 16 is not right for the flange */
    for (i = 0; i < MAX_CHANNELS; i++)
	cp->history[i] = new_Backbuf(MAX_SAMPLE_RATE / 100 * 2 * 16);
    
    cp->ang = 0.0;
    cp->depth = 5;
    cp->speed = 300;
    cp->mode = 0;
    cp->wet = 250;
    cp->dry = 200;
    cp->regen = 0;
}
