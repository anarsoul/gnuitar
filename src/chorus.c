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
 * Revision 1.36  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.35  2006/08/02 19:21:04  alankila
 * - add static declarations
 *
 * Revision 1.34  2005/11/05 14:51:16  alankila
 * - use basedelay scaling also in multichannel
 * - make maximum period longer
 *
 * Revision 1.33  2005/10/30 22:26:45  alankila
 * - make chorus fatter by spreading voices apart in fractions of chosen
 *   static delay
 * - limit regen to 50 % as the effect tends to go out of control with
 *   values approaching 80 %, and even 50 % sounds rather like reverb.
 *
 * Revision 1.32  2005/09/09 20:57:20  alankila
 * - merge dry/wet sliders together
 *
 * Revision 1.31  2005/09/06 14:54:31  alankila
 * - set button states at loadup
 * - make echo multichannel aware. Echo currently can do almost everything
 *   reverb can do, so we could remove reverb.
 *
 * Revision 1.30  2005/09/05 20:07:49  alankila
 * - multichannel chorus
 * - add some code to synchronize output volumes regardless of voices #
 *   based on the random walk theorem
 *
 * Revision 1.29  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.28  2005/09/04 19:56:41  alankila
 * - final strokes
 *
 * Revision 1.27  2005/09/04 19:45:12  alankila
 * - replace "Speed 1/ms" with "Period ms" which is easier to understand
 *
 * Revision 1.26  2005/09/04 19:29:40  alankila
 * - generalize flange as base delay setting
 * - support additional chorus voices
 * - new default settings that use the new features
 *
 * Revision 1.25  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.24  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.23  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
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
#include <assert.h>
#include <math.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEPTH       20
#define MAX_BASEDELAY   50
 
static void
update_chorus_basedelay(GtkAdjustment *adj, struct chorus_params *params)
{
    params->basedelay = adj->value;
}

static void
update_chorus_depth(GtkAdjustment *adj, struct chorus_params *params)
{
    params->depth = adj->value;
}

static void
update_chorus_speed(GtkAdjustment *adj, struct chorus_params *params)
{
    params->speed = adj->value;
}

static void
update_chorus_voices(GtkAdjustment *adj, struct chorus_params *params)
{
    params->voices = adj->value;
}

static void
update_chorus_drywet(GtkAdjustment *adj, struct chorus_params *params)
{
    params->drywet = adj->value;
}

static void
update_chorus_regen(GtkAdjustment *adj, struct chorus_params *params)
{
    params->regen = adj->value;
}

static void
toggle_chorus_multichannel(void *bullshit, struct chorus_params *params)
{
    params->multichannel = !params->multichannel;
}

static void
chorus_init(struct effect *p)
{
    struct chorus_params *pchorus;

    GtkWidget      *basedelay;
    GtkWidget      *basedelay_label;
    GtkObject      *adj_basedelay;

    GtkWidget      *depth;
    GtkWidget      *depth_label;
    GtkObject      *adj_depth;

    GtkWidget      *speed;
    GtkWidget      *speed_label;
    GtkObject      *adj_speed;

    GtkWidget      *voices;
    GtkWidget      *voices_label;
    GtkObject      *adj_voices;

    GtkWidget      *drywet;
    GtkWidget      *drywet_label;
    GtkObject      *adj_drywet;

    GtkWidget      *regen;
    GtkWidget      *regen_label;
    GtkObject      *adj_regen;

    GtkWidget      *button, *mcbutton;

    GtkWidget      *parmTable;

    pchorus = (struct chorus_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(9, 3, FALSE);

    adj_basedelay = gtk_adjustment_new(pchorus->basedelay, 0.0, MAX_BASEDELAY, 1.0, 1.0, 0.0);
    basedelay_label = gtk_label_new("Delay\nms");
    gtk_label_set_justify(GTK_LABEL(basedelay_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), basedelay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 3, 0);
    
    gtk_signal_connect(GTK_OBJECT(adj_basedelay), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_basedelay), pchorus);

    basedelay = gtk_vscale_new(GTK_ADJUSTMENT(adj_basedelay));
    gtk_scale_set_digits(GTK_SCALE(basedelay), 3);

    gtk_table_attach(GTK_TABLE(parmTable), basedelay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);
    
    adj_depth = gtk_adjustment_new(pchorus->depth, 0.0, MAX_DEPTH, 1.0, 1.0, 0.0);
    depth_label = gtk_label_new("Depth\nms");
    gtk_label_set_justify(GTK_LABEL(depth_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), depth_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 3, 0);
    
    gtk_signal_connect(GTK_OBJECT(adj_depth), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_depth), pchorus);

    depth = gtk_vscale_new(GTK_ADJUSTMENT(adj_depth));
    gtk_scale_set_digits(GTK_SCALE(depth), 3);

    gtk_table_attach(GTK_TABLE(parmTable), depth, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);

    adj_speed = gtk_adjustment_new(pchorus->speed, 200.0, 5000, 0.1, 1.0, 0.0);
    speed_label = gtk_label_new("Period\nms");
    gtk_label_set_justify(GTK_LABEL(speed_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 3, 0);

    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_speed), pchorus);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), speed, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);
    
    adj_voices = gtk_adjustment_new(pchorus->voices, 1, 4, 1, 1, 0);
    voices_label = gtk_label_new("Voices\n#");
    gtk_label_set_justify(GTK_LABEL(voices_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), voices_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 3, 0);

    gtk_signal_connect(GTK_OBJECT(adj_voices), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_voices), pchorus);

    voices = gtk_vscale_new(GTK_ADJUSTMENT(adj_voices));
    gtk_scale_set_digits(GTK_SCALE(voices), 0);
    gtk_widget_set_size_request(GTK_WIDGET(voices),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), voices, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);

    adj_drywet = gtk_adjustment_new(pchorus->drywet, 0.0, 100.0, 1.0, 1.0, 0.0);
    drywet_label = gtk_label_new("Dry/Wet\n%");
    gtk_label_set_justify(GTK_LABEL(drywet_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 3, 0);


    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_drywet), pchorus);

    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));

    gtk_table_attach(GTK_TABLE(parmTable), drywet, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);

    adj_regen = gtk_adjustment_new(pchorus->regen, 0.0, 50.0, 1.0, 1.0, 0.0);
    regen_label = gtk_label_new("Regen\n%");
    gtk_label_set_justify(GTK_LABEL(regen_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), regen_label, 7, 8, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 3, 0);

    gtk_signal_connect(GTK_OBJECT(adj_regen), "value_changed",
		       GTK_SIGNAL_FUNC(update_chorus_regen), pchorus);

    regen = gtk_vscale_new(GTK_ADJUSTMENT(adj_regen));

    gtk_table_attach(GTK_TABLE(parmTable), regen, 7, 8, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);
    
    if (n_input_channels == 1 && n_output_channels > 1) {
        mcbutton = gtk_check_button_new_with_label("Multichannel flange");
        if (pchorus->multichannel)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mcbutton), TRUE);
        gtk_signal_connect(GTK_OBJECT(mcbutton), "toggled",
                           GTK_SIGNAL_FUNC(toggle_chorus_multichannel), pchorus);
        gtk_table_attach(GTK_TABLE(parmTable), mcbutton, 1, 8, 2, 3,
                         __GTKATTACHOPTIONS(GTK_EXPAND |
                                            GTK_SHRINK),
                         __GTKATTACHOPTIONS(GTK_FILL |
                                            GTK_SHRINK), 0, 0);
    }

    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control), "Chorus");
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

static void
chorus_filter_mono(struct effect *p, data_block_t *db)
{
    struct chorus_params *cp;
    int             count, i, curr_channel = 0;
    double          dly, Speed, tmp_ang, Depth, BaseDelay, Dry, Wet, Rgn;
    DSP_SAMPLE     *s;
    DSP_SAMPLE      tmp, rgn;

    cp = (struct chorus_params *) p->params;

    s = db->data;
    count = db->len;

    Dry = 1 - cp->drywet / 100.0;
    Wet =     cp->drywet / 100.0;
    Rgn = cp->regen / 100.0;
    Speed = 1000.0 / cp->speed / sample_rate;
    Depth = cp->depth / 1000.0 * sample_rate;
    BaseDelay = cp->basedelay / 1000.0 * sample_rate;

    while (count) {
        tmp = 0.0;
        tmp_ang = cp->ang;
        for (i = 0; i < cp->voices; i += 1) {
            dly = BaseDelay * i / cp->voices + Depth * (1 + sin_lookup(tmp_ang)) / 2.0;
            tmp += cp->history[curr_channel]->get_interpolated(cp->history[curr_channel], dly) / sqrt(cp->voices);
            tmp_ang += 1.0 / cp->voices;
            if (tmp_ang >= 1.0)
                tmp_ang -= 1.0;
        }
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(tmp)
#endif
        /* XXX regen sounds generally bad, maybe we should take it away? */
        dly = BaseDelay + Depth * (1 + sin_lookup(cp->ang)) / 2.0;
        rgn = cp->history[curr_channel]->get_interpolated(cp->history[curr_channel], dly) * Rgn + *s;
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(rgn)
#endif
        cp->history[curr_channel]->add(cp->history[curr_channel], rgn);
	*s = *s * Dry / sqrt(cp->voices) + tmp * Wet;

	curr_channel = (curr_channel + 1) % db->channels;
        if (curr_channel == 0) {
            cp->ang += Speed;
            if (cp->ang >= 1.0)
                cp->ang -= 1.0;
        }

	s++;
	count--;
    }
}

/* mono to N */
static void
chorus_filter_mc(struct effect *p, data_block_t *db)
{
    struct chorus_params *cp;
    int             i, count, curr_channel = 0;
    double          dly, Speed, tmp_ang, Depth, BaseDelay, Dry, Wet, Rgn;
    DSP_SAMPLE     *outs, *ins;
    DSP_SAMPLE      tmp, rgn;

    cp = (struct chorus_params *) p->params;

    assert(db->channels == 1);
    
    ins = db->data;
    outs = db->data_swap;
    db->data = outs;
    db->data_swap = ins;

    count = db->len;
    
    Dry = 1 - cp->drywet / 100.0;
    Wet =     cp->drywet / 100.0;
    Rgn = cp->regen / 100.0;
    Speed = 1000.0 / cp->speed / sample_rate;
    Depth = cp->depth / 1000.0 * sample_rate;
    BaseDelay = cp->basedelay / 1000.0 * sample_rate;

    db->channels = n_output_channels;
    db->len *= n_output_channels;

    while (count) {
        tmp_ang = cp->ang;

        i = 0;
        /* mix # voices repeatedly into output channels if need be */
        for (curr_channel = 0; curr_channel < db->channels; curr_channel += 1) {
            dly = BaseDelay * i / db->channels + Depth * (1 + sin_lookup(tmp_ang)) / 2.0;
            tmp = cp->history[0]->get_interpolated(cp->history[0], dly);
            tmp_ang += 1.0 / cp->voices;
            if (tmp_ang >= 1.0)
                tmp_ang -= 1.0;
            *outs++ = (*ins * Dry + tmp * Wet) / sqrt(2);
            i += 1;
        }
        
        dly = BaseDelay + Depth * (1 + sin_lookup(cp->ang)) / 2.0;
        rgn = cp->history[0]->get_interpolated(cp->history[0], dly) * Rgn + *ins;
        cp->history[0]->add(cp->history[0], rgn);

        cp->ang += Speed;
        if (cp->ang >= 1.0)
            cp->ang -= 1.0;

	ins++;
	count--;
    }
}

static void
chorus_filter(struct effect *p, data_block_t *db)
{
    struct chorus_params *params = p->params;
    
    if (params->multichannel && db->channels == 1 && n_output_channels > 1) {
        chorus_filter_mc(p, db);
    } else {
        chorus_filter_mono(p, db);
    }
}

static void
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
}

static void
chorus_save(struct effect *p, SAVE_ARGS)
{
    struct chorus_params *params = p->params;

    SAVE_DOUBLE("drywet", params->drywet);
    SAVE_DOUBLE("basedelay", params->basedelay);
    SAVE_DOUBLE("depth", params->depth);
    SAVE_DOUBLE("speed", params->speed);
    SAVE_DOUBLE("regen", params->regen);
    SAVE_INT("voices", params->voices);
    SAVE_INT("multichannel", params->voices);
}

static void
chorus_load(struct effect *p, LOAD_ARGS)
{
    struct chorus_params *params = p->params;

    LOAD_DOUBLE("drywet", params->drywet);
    LOAD_DOUBLE("basedelay", params->basedelay);
    LOAD_DOUBLE("depth", params->depth);
    LOAD_DOUBLE("speed", params->speed);
    LOAD_DOUBLE("regen", params->regen);
    LOAD_INT("voices", params->voices);
    LOAD_INT("multichannel", params->voices);
}

effect_t *
chorus_create()
{
    effect_t       *p;
    struct chorus_params *cp;
    int             i;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct chorus_params));
    p->proc_init = chorus_init;
    p->proc_filter = chorus_filter;
    p->toggle = 0;
    p->proc_done = chorus_done;
    p->proc_save = chorus_save;
    p->proc_load = chorus_load;
    cp = p->params;

    for (i = 0; i < MAX_CHANNELS; i++)
	cp->history[i] = new_Backbuf((MAX_DEPTH + MAX_BASEDELAY) / 1000.0 * MAX_SAMPLE_RATE);
    
    cp->ang = 0.0;
    cp->depth = 2.5;
    cp->basedelay = 3.5;
    cp->voices = 3;
    cp->speed = 1000;
    cp->drywet = 50;
    cp->regen = 0;

    return p;
}
