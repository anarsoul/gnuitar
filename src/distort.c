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
 * Revision 1.30  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.29  2006/08/02 19:07:56  alankila
 * - add missing static declarations
 *
 * Revision 1.28  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.27  2005/09/04 19:30:23  alankila
 * - move the common clip code into a macro
 *
 * Revision 1.26  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.25  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.24  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.23  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.22  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.21  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.20  2005/09/01 17:31:40  alankila
 * - various small fixes for multichannel / gui
 *
 * Revision 1.19  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.18  2005/08/22 22:11:59  alankila
 * - change RC filters to accept data_block
 * - LC filters have no concept of "LOWPASS" or "HIGHPASS" filtering, there's
 *   just filter_no.
 * - remove unused SAMPLE8 typedef
 *
 * Revision 1.17  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.16  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.15  2005/08/12 11:24:05  alankila
 * - include math.h for isnan()
 *
 * Revision 1.14  2005/08/08 12:01:34  fonin
 * Include utils.h for isnan() on Windows
 *
 * Revision 1.13  2005/07/31 10:23:20  fonin
 * All processing in temp variable; added clipping code
 *
 * Revision 1.12  2005/04/29 11:24:43  fonin
 * Better presets
 *
 * Revision 1.11  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.10  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.9  2003/03/11 22:04:00  fonin
 * Measure control sliders in standard units (ms, %).
 *
 * Revision 1.8  2003/03/09 20:49:45  fonin
 * Structures were redesigned to allow to change sampling params.
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
 * Revision 1.3  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.2  2001/03/25 12:10:14  fonin
 * Removed clip functionality. Effect control window ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:41  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include <stdlib.h>
#include <math.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "distort.h"
#include "gui.h"
#include "utils.h"

static void
update_distort_level(GtkAdjustment * adj, struct distort_params *params)
{
    params->level = (int) adj->value * 2.56;
}

static void
update_distort_sat(GtkAdjustment * adj, struct distort_params *params)
{
    params->sat = (int) adj->value * 300;
}

static void
update_distort_drive(GtkAdjustment * adj, struct distort_params *params)
{
    params->drive = (int) adj->value * 10;
}

static void
update_distort_lowpass(GtkAdjustment * adj, struct distort_params *params)
{
    params->lowpass = (int) adj->value;
}

static void
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

    GtkWidget      *parmTable;

    pdistort = (struct distort_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(2, 8, FALSE);

    adj_drive = gtk_adjustment_new(pdistort->drive / 10,
				   1.0, 100.0, 1.0, 1.0, 1.0);
    drive_label = gtk_label_new("Drive\n%");
    gtk_table_attach(GTK_TABLE(parmTable), drive_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_drive), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort_drive), pdistort);

    drive = gtk_vscale_new(GTK_ADJUSTMENT(adj_drive));
    gtk_widget_set_size_request(GTK_WIDGET(drive),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), drive, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_level =
	gtk_adjustment_new(pdistort->level / 2.56, 1.0, 101, 1.0, 1.0,
			   1.0);
    level_label = gtk_label_new("Level\n%");
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

    adj_sat =
	gtk_adjustment_new(pdistort->sat / 300, 1.0, 101, 1.0, 1.0, 1.0);
    sat_label = gtk_label_new("Saturation\n%");
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
    lowpass_label = gtk_label_new("Lowpass\nHz");
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
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Distortion"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

static void
distort_filter(struct effect *p, data_block_t *db)
{
    int             count,
                    currchannel = 0;
    DSP_SAMPLE     *s;
    struct distort_params *dp;
    double	    t;

    dp = (struct distort_params *) p->params;
    /*
     * sat clips derivative by limiting difference between samples. Use lastval 
     * member to store last sample for seamlessness between chunks. 
     */
    count = db->len;
    s = db->data;

    RC_highpass(db, &(dp->fd));

    while (count) {
	/*
	 * apply drive  
	 */
	t=*s;
	t *= dp->drive;
	t /= 16;

	/*
	 * apply sat 
	 */
	if ((t - dp->lastval[currchannel]) > (dp->sat << 8))
	    t = dp->lastval[currchannel] + (dp->sat << 8);
	else if ((dp->lastval[currchannel] - t) > (dp->sat << 8))
	    t = dp->lastval[currchannel] - (dp->sat << 8);

	dp->lastval[currchannel] = t;
        currchannel = (currchannel + 1) % db->channels;
        
	t *= dp->level;
	t /= 256;
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(t)
#endif
	if(isnan(t))
	    t=0;
	*s=t;

	s++;
	count--;
    }

    LC_filter(db, 0, dp->lowpass, &(dp->noise));
}

static void
distort_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
distort_save(struct effect *p, SAVE_ARGS)
{
    struct distort_params *params = p->params;

    SAVE_INT("sat", params->sat);
    SAVE_INT("level", params->level);
    SAVE_INT("drive", params->drive);
    SAVE_INT("lowpass", params->lowpass);
}

static void
distort_load(struct effect *p, LOAD_ARGS)
{
    struct distort_params *params = p->params;

    LOAD_INT("sat", params->sat);
    LOAD_INT("level", params->level);
    LOAD_INT("drive", params->drive);
    LOAD_INT("lowpass", params->lowpass);
}

effect_t *
distort_create()
{
    effect_t *p;
    struct distort_params *ap;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct distort_params));
    ap = p->params;

    p->proc_init = distort_init;
    p->proc_filter = distort_filter;
    p->proc_save = distort_save;
    p->proc_load = distort_load;
    p->toggle = 0;
    p->proc_done = distort_done;

    ap->sat = 10000;
    ap->level = 20;
    ap->drive = 555;
    ap->lowpass = 350;
    ap->noisegate = 3000;

    RC_setup(10, 1.5, &(ap->fd));
    RC_set_freq(ap->lowpass, &(ap->fd));

    RC_setup(10, 1, &(ap->noise));
    RC_set_freq(ap->noisegate, &(ap->noise));

    return p;
}
