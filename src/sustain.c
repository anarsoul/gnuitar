/*
 * GNUitar
 * Sustain effect
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
 * Revision 1.24  2006/08/06 20:14:55  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.23  2006/08/02 19:11:18  alankila
 * - add missing static declarations
 *
 * Revision 1.22  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.21  2005/09/04 19:30:23  alankila
 * - move the common clip code into a macro
 *
 * Revision 1.20  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.19  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.18  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.17  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.16  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.15  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.14  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.13  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.12  2004/08/10 15:21:16  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.10  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.9  2003/03/12 20:54:04  fonin
 * Better presets.
 *
 * Revision 1.8  2003/03/11 22:04:00  fonin
 * Measure control sliders in standard units (ms, %).
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
 * Revision 1.2  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:19  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "sustain.h"
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "gui.h"

static void
update_sustain_sust(GtkAdjustment * adj, struct sustain_params *params)
{
    params->sust = (int) adj->value * 2.56;
}

static void
update_sustain_noise(GtkAdjustment * adj, struct sustain_params *params)
{
    params->noise = (int) adj->value * 2.56;
}

static void
update_sustain_gate(GtkAdjustment * adj, struct sustain_params *params)
{
    params->threshold = (int) adj->value * 2.56;
}

static void
sustain_init(struct effect *p)
{
    struct sustain_params *psustain;

    GtkWidget      *gate;
    GtkWidget      *gate_label;
    GtkObject      *adj_gate;

    GtkWidget      *noise;
    GtkWidget      *noise_label;
    GtkObject      *adj_noise;

    GtkWidget      *sust;
    GtkWidget      *sust_label;
    GtkObject      *adj_sust;

    GtkWidget      *button;

    GtkWidget      *parmTable;


    psustain = (struct sustain_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    gtk_window_set_position(GTK_WINDOW(p->control), GTK_WIN_POS_CENTER);

    parmTable = gtk_table_new(4, 8, FALSE);

    adj_gate = gtk_adjustment_new(psustain->threshold / 2.56,
				  0.0, 101.0, 1.0, 1.0, 1.0);
    gate_label = gtk_label_new("Gate\n%");
    gtk_table_attach(GTK_TABLE(parmTable), gate_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_gate), "value_changed",
		       GTK_SIGNAL_FUNC(update_sustain_gate), psustain);

    gate = gtk_vscale_new(GTK_ADJUSTMENT(adj_gate));
    gtk_widget_set_size_request(GTK_WIDGET(gate),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), gate, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_sust =
	gtk_adjustment_new(psustain->sust / 2.56, 0.0, 101.0, 1.0, 1.0,
			   1.0);
    sust_label = gtk_label_new("Sustain\n%");
    gtk_table_attach(GTK_TABLE(parmTable), sust_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_sust), "value_changed",
		       GTK_SIGNAL_FUNC(update_sustain_sust), psustain);

    sust = gtk_vscale_new(GTK_ADJUSTMENT(adj_sust));

    gtk_table_attach(GTK_TABLE(parmTable), sust, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    adj_noise =
	gtk_adjustment_new(psustain->noise / 2.56, 0.0, 101.0, 1.0, 1.0,
			   1.0);
    noise_label = gtk_label_new("Noise\n%");
    gtk_table_attach(GTK_TABLE(parmTable), noise_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_noise), "value_changed",
		       GTK_SIGNAL_FUNC(update_sustain_noise), psustain);

    noise = gtk_vscale_new(GTK_ADJUSTMENT(adj_noise));

    gtk_table_attach(GTK_TABLE(parmTable), noise, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 3, 4, 3, 4,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Sustain"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

static void
sustain_filter(struct effect *p, data_block_t *db)
{

    int             count;
    DSP_SAMPLE     *s;
    struct sustain_params *ds;
    DSP_SAMPLE      volAccum,
                    tmp;
    float           CompW1;
    float           CompW2;
    float           gateFac;
    float           compFac;

    ds = (struct sustain_params *) p->params;

    count = db->len;
    s = db->data;

    volAccum = ds->volaccum;
    CompW1 = ds->sust / 100.0f;
    CompW2 = 1.0f - CompW1;

    while (count) {
	tmp = *s;
	/*
	 * update volAccum 
	 */
	tmp = (tmp < 0) ? -tmp : tmp;
	volAccum = (256 - ds->noise) * volAccum + ds->noise * tmp;
	volAccum /= 256;

	/*
	 * handle compression 
	 */
	compFac = 30000.0f / (float) volAccum;
	compFac = CompW1 * compFac + CompW2;
	/*
	 * handle gate 
	 */
	if (ds->threshold <= 1.0f)
	    gateFac = 1.0f;
	else
	    gateFac = (volAccum > (ds->threshold * 100)) ? 1.0f :
		((float) (volAccum) / (float) (ds->threshold * 100));
	/*
	 * process signal... 
	 */
	tmp = ((float) (*s) * compFac * gateFac);
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(tmp)
#endif
	*s = tmp;
	s++;
	count--;
    }
    ds->volaccum = volAccum;

}

static void
sustain_save(struct effect *p, SAVE_ARGS)
{
    struct sustain_params *params = p->params;

    SAVE_INT("sust", params->sust);
    SAVE_INT("noise", params->noise);
    SAVE_INT("threshold", params->threshold);
}

static void
sustain_load(struct effect *p, LOAD_ARGS)
{
    struct sustain_params *params = p->params;

    LOAD_INT("sust", params->sust);
    LOAD_INT("noise", params->noise);
    LOAD_INT("threshold", params->threshold);
}

static void
sustain_done(struct effect *p)
{
    struct sustain_params *dp;

    dp = (struct sustain_params *) p->params;

    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

effect_t *
sustain_create()
{
    effect_t       *p;
    struct sustain_params *psustain;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct sustain_params));
    p->proc_init = sustain_init;
    p->proc_filter = sustain_filter;
    p->proc_load = sustain_load;
    p->proc_save = sustain_save;
    p->toggle = 0;
    p->proc_done = sustain_done;

    psustain = p->params;
    psustain->noise = 40;
    psustain->sust = 256;
    psustain->threshold = 256;

    return p;
}
