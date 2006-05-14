/*
 * GNUitar
 * Phasor effect
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
 * Revision 1.33  2006/05/14 08:47:34  alankila
 * - snafu: the previous code implemented sidewise panning, not phaser
 *
 * Revision 1.32  2006/05/13 17:10:06  alankila
 * - move hilbert transform into biquad.c
 * - implement stereo phaser using hilbert transform
 * - clean up remaining struct biquad references and replace them with typedef
 *
 * Revision 1.31  2006/05/13 09:33:16  alankila
 * - more power to phaser, less cpu use, good deal
 *
 * Revision 1.30  2005/10/07 12:50:12  alankila
 * - move delay shape computation to where it belongs and change it to bit
 *   smoother
 *
 * Revision 1.29  2005/09/30 12:43:55  alankila
 * - make effect deeper
 * - don't update biquad parameters so often to conserve some CPU
 * - make 0 % depth sound identical to dry
 * - change minimum period
 *
 * Revision 1.28  2005/09/12 09:42:25  fonin
 * - MSVC compatibility fixes
 *
 * Revision 1.27  2005/09/10 10:53:38  alankila
 * - remove the need to reserve biquad's mem in caller's side
 *
 * Revision 1.26  2005/09/09 20:52:20  alankila
 * - add dry/wet % for comb effect
 *
 * Revision 1.25  2005/09/09 20:22:17  alankila
 * - phasor reimplemented according to a popular algorithm that simulates
 *   high-impedance isolated varying capacitors
 *
 * Revision 1.24  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.23  2005/09/04 19:45:12  alankila
 * - replace "Speed 1/ms" with "Period ms" which is easier to understand
 *
 * Revision 1.22  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.21  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.20  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.19  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.18  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.17  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.16  2005/09/01 23:03:08  alankila
 * - use separate filter structs for different filters
 * - decrypt parameters, borrow code from autowah
 * - use same logarithmic sweep
 *
 * Revision 1.15  2005/08/22 22:11:59  alankila
 * - change RC filters to accept data_block
 * - LC filters have no concept of "LOWPASS" or "HIGHPASS" filtering, there's
 *   just filter_no.
 * - remove unused SAMPLE8 typedef
 *
 * Revision 1.14  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.13  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.12  2005/04/29 11:24:42  fonin
 * Better presets
 *
 * Revision 1.11  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.10  2003/03/13 20:24:30  fonin
 * New parameter "bandpass" - turn on bandpass function.
 *
 * Revision 1.9  2003/03/12 20:55:35  fonin
 * - meaningful measure units;
 * - code cleanup.
 *
 * Revision 1.8  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.7  2003/02/01 19:15:12  fonin
 * Use sizeof(variable) instead sizeof(type) in load/save procedures,
 * when reading/writing from file.
 *
 * Revision 1.6  2003/01/30 21:35:29  fonin
 * Got rid of rnd_window_pos().
 *
 * Revision 1.5  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.4  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.3  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.2  2001/01/13 10:02:12  fonin
 * Initial filter is passthru
 *
 * Revision 1.1.1.1  2001/01/11 13:21:58  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "phasor.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#    include "utils.h"		/* for M_PI */
#endif

#define PHASOR_UPDATE_INTERVAL 8
 
static void
update_phasor_speed(GtkAdjustment *adj, struct phasor_params *params)
{
    params->sweep_time = adj->value;
}

static void
update_phasor_depth(GtkAdjustment *adj, struct phasor_params *params)
{
    params->depth = adj->value;
}

static void
update_phasor_drywet(GtkAdjustment *adj, struct phasor_params *params)
{
    params->drywet = adj->value;
}

static void
toggle_stereo(GtkAdjustment *adj, struct phasor_params *params)
{
    params->stereo = !params->stereo;
}

static void
phasor_init(struct effect *p)
{
    struct phasor_params *params;

    GtkWidget      *speed;
    GtkWidget      *speed_label;
    GtkObject      *adj_speed;

    GtkWidget      *depth;
    GtkWidget      *depth_label;
    GtkObject      *adj_depth;

    GtkWidget      *drywet;
    GtkWidget      *drywet_label;
    GtkObject      *adj_drywet;

    GtkWidget      *button;
    GtkWidget      *parmTable;
    params = p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(3, 3, FALSE);


    adj_speed = gtk_adjustment_new(params->sweep_time, 150.0, 2000,
				   1.0, 10.0, 0.0);
    speed_label = gtk_label_new("Period\nms");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_phasor_speed), params);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_depth =
	gtk_adjustment_new(params->depth, 0.0, 100.0, 1.0, 5.0, 0.0);
    depth_label = gtk_label_new("Depth\n%");
    gtk_table_attach(GTK_TABLE(parmTable), depth_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_depth), "value_changed",
		       GTK_SIGNAL_FUNC(update_phasor_depth), params);

    depth = gtk_vscale_new(GTK_ADJUSTMENT(adj_depth));

    gtk_table_attach(GTK_TABLE(parmTable), depth, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_drywet =
	gtk_adjustment_new(params->drywet, 0.0, 100.0, 1.0, 5.0,
			   0.0);
    drywet_label = gtk_label_new("Dry/Wet\n%");
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_phasor_drywet), params);

    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));

    gtk_table_attach(GTK_TABLE(parmTable), drywet, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    button = gtk_check_button_new_with_label("On");
    if (p->toggle)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS
		     (GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    if (n_output_channels > 1) {
        button = gtk_check_button_new_with_label("Stereo");
        if (params->stereo)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        gtk_signal_connect(GTK_OBJECT(button), "toggled",
                           GTK_SIGNAL_FUNC(toggle_stereo), params);

        gtk_table_attach(GTK_TABLE(parmTable), button, 1, 2, 2, 3,
                         __GTKATTACHOPTIONS(GTK_EXPAND | GTK_SHRINK),
                         __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);
    }

    
    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Phasor"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

static void
phasor_filter_mono(struct effect *p, struct data_block *db)
{
    struct phasor_params *params = p->params;
    DSP_SAMPLE     *s, tmp;
    int             count, curr_channel = 0, i;
    float           delay, Dry, Wet, f;

    count = db->len;
    s = db->data;

    Dry = 1 - params->drywet / 100.0;
    Wet =     params->drywet / 100.0;
    f = params->f;
    
    while (count) {
        if (curr_channel == 0 && count % PHASOR_UPDATE_INTERVAL == 0) { 
            f += 1000.0 / params->sweep_time / sample_rate * PHASOR_UPDATE_INTERVAL;
            if (f >= 1.0)
                f -= 1.0;
            delay = (sin_lookup(f) + 1) / 2;
            delay *= params->depth / 100.0;
            delay = 1.0 - delay;

            for (i = 0; i < MAX_PHASOR_FILTERS; i += 1)
                set_2nd_allpass_biquad(delay, &params->allpass[i]);
        }
        
        tmp = *s;
        for (i = 0; i < MAX_PHASOR_FILTERS; i += 1)
            tmp = do_biquad(tmp, &params->allpass[i], curr_channel);
        *s = *s * Dry + tmp * Wet;

        curr_channel = (curr_channel + 1) % db->channels;
        *s++;
        count--;
    }
}

static void
phasor_filter_stereo(struct effect *p, struct data_block *db)
{
    struct phasor_params *params = p->params;
    float f, sinval=0, cosval=0;
    int i;
    
    db->channels = 2;
    db->len *= 2;
    f = params->f;
    for (i = 0; i < db->len / 2; i += 1) {
        DSP_SAMPLE x0, x1, y0, y1;
        if (i % PHASOR_UPDATE_INTERVAL == 0) { 
            float ftmp;
            
            f += 1000.0 / params->sweep_time / sample_rate * PHASOR_UPDATE_INTERVAL;
            if (f >= 1.0)
                f -= 1.0;
            sinval = sin_lookup(f);

            ftmp = f + 0.25;
            if (ftmp >= 1.0)
                ftmp -= 1.0;
            cosval = sin_lookup(ftmp);
        }

        hilbert_transform(db->data[i], &x0, &x1, &params->hilb);
        y0 = cosval * x0 + sinval * x1;
        y1 = cosval * x0 - sinval * x1;

        db->data_swap[i*2+0] = y0;//(cosval * y0 + sinval * y1);
        db->data_swap[i*2+1] = y1;//(cosval * y0 - sinval * y1);
    }
    /* swap to processed buffer for next effect */
    DSP_SAMPLE *tmp = db->data;
    db->data = db->data_swap;
    db->data_swap = tmp;
}

static void
phasor_filter(struct effect *p, struct data_block *db)
{
    struct phasor_params *params = p->params;

    if (n_output_channels > 1 && db->channels == 1 && params->stereo) {
        phasor_filter_stereo(p, db);
        params->f += 1000.0 / params->sweep_time / sample_rate * (db->len / db->channels) / 2;
    } else {
        phasor_filter_mono(p, db);
        params->f += 1000.0 / params->sweep_time / sample_rate * (db->len / db->channels);
    }
    
    if (params->f >= 1.0)
        params->f -= 1.0;
}

static void
phasor_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
phasor_save(struct effect *p, SAVE_ARGS)
{
    struct phasor_params *params = p->params;

    SAVE_DOUBLE("sweep_time", params->sweep_time);
    SAVE_DOUBLE("depth", params->depth);
    SAVE_DOUBLE("drywet", params->drywet);
    SAVE_INT("stereo", params->stereo);
}

static void
phasor_load(struct effect *p, LOAD_ARGS)
{
    struct phasor_params *params = p->params;

    LOAD_DOUBLE("sweep_time", params->sweep_time);
    LOAD_DOUBLE("depth", params->depth);
    LOAD_DOUBLE("drywet", params->drywet);
    LOAD_INT("stereo", params->stereo);
}

effect_t *
phasor_create()
{
    effect_t             *p;
    struct phasor_params *params;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct phasor_params));
    p->proc_init = phasor_init;
    p->proc_filter = phasor_filter;
    p->proc_done = phasor_done;
    p->proc_load = phasor_load;
    p->proc_save = phasor_save;

    params = p->params;

    params->sweep_time = 200.0;
    params->depth = 100.0;
    params->drywet = 50.0;
    params->f = 0;
    params->stereo = 0;

    hilbert_init(&params->hilb);

    return p;
}
