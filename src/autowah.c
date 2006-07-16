/*
 * GNUitar
 * Autowah effect
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
 * Revision 1.38  2006/07/16 12:26:38  alankila
 * - add more wah algorithms, including gnuitar's original
 *
 * Revision 1.37  2006/05/31 14:05:10  fonin
 * Added #include for sanity
 *
 * Revision 1.36  2006/05/20 17:30:29  alankila
 * - use parameters that better demonstrate the "wahness" of the effect
 *
 * Revision 1.35  2006/05/07 10:25:17  alankila
 * - I misspelled Antti's name.
 *
 * Revision 1.34  2006/05/07 07:40:14  alankila
 * - still more parameter finetuning
 *
 * Revision 1.33  2006/05/07 07:11:46  alankila
 * - fix load/save function
 * - add static keywords where appropriate
 * - allow user to tune the resonance. Resonance values near 110 % trigger
 *   self-supporting oscillations of the moog filter, and are actually used
 *   by real moog synths for sound generation. I capped it at 120 % so that
 *   user can play with that a bit.
 *
 * Revision 1.32  2006/05/06 16:52:29  alankila
 * - better defaults
 * - more exciting Wah algorithm according to Antti Huovilainen's paper
 *   "Non-Linear Digital implementation Of The Moog Ladder Filter"
 *   (published on 7th int. conference on digital audio effects, DAFx'04)
 *
 * Revision 1.31  2005/10/22 14:21:05  alankila
 * - reduce wah sweep range to saner limits, also reduce number of magical-
 *   seeming constants. This version is most reliable pick detector yet.
 *
 * Revision 1.30  2005/10/22 13:55:14  alankila
 * - add history buffer to make wah behaviour sampling parameter independent.
 * - add rudimentary handling for multiple channels by ignoring all but first
 *   in the pick-sweep mode.
 *
 * Revision 1.29  2005/10/22 13:04:43  alankila
 * - add power and delta computation so that wah may key on either type of
 *   increase. Also convert units to dB.
 *
 * Revision 1.28  2005/10/21 19:19:48  alankila
 * - add a pick-sensitive mode where a downward sweep is started synchronously
 *   with picking
 * - reduce wah strength and gain to help with clipping and make effect more
 *   usable (more a wowwow-type sound and less resonance)
 *
 * Revision 1.27  2005/10/02 08:25:25  fonin
 * "Mix" checkbox converted to dry/wet slider
 *
 * Revision 1.26  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.25  2005/09/04 19:45:12  alankila
 * - replace "Speed 1/ms" with "Period ms" which is easier to understand
 *
 * Revision 1.24  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.23  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.22  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.21  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.20  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.19  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.18  2005/09/01 16:22:42  alankila
 * - oops, revert gain change
 *
 * Revision 1.17  2005/09/01 16:20:21  alankila
 * - fix comment, and reduce autowah gain
 *
 * Revision 1.16  2005/09/01 16:09:54  alankila
 * - make rcfilter and autowah multichannel ready. In addition, autowah
 *   now performs linear sweep in logarithmic domain rather than exponential.
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
 * Revision 1.12  2005/04/29 11:24:59  fonin
 * Return back the 1.5 amplify coeff
 *
 * Revision 1.11  2005/04/26 13:37:39  fonin
 * Declaring dry[] in the autowah_filter as static fixes the 100% CPU usage on windows; also amplify coefficients changed from 1.5 to 1.0
 *
 * Revision 1.10  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.9  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.8  2003/03/09 20:46:07  fonin
 * - parameter "speed" removed from effect internal structure, df (delta ef)
 *   instead;
 * - cleanup of dead code and variables in autowah_filter();
 * - parameter speed is measured in 1 wave per N msec.
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
 * Revision 1.3  2001/06/02 14:05:58  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.2  2001/03/25 12:10:49  fonin
 * Effect window control ignores delete event.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:05  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "autowah.h"
#include "glib12-compat.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#    include "utils.h"
#endif
#include <string.h>
#include <gtk/gtk.h>

/* these thresholds are used to trigger the sweep. The system accumulates
 * time-weighted average of square difference between samples ("delta") and
 * the energy per sample ("power"). If either suddenly increases, the
 * sweep triggers. Data is collected AUTOWAH_HISTORY_LENGTH ms apart. */

#define AUTOWAH_HISTORY_LENGTH  100  /* ms */
#define AUTOWAH_DISCANT_TRIGGER 0.65 /* dB */
#define AUTOWAH_BASS_TRIGGER    0.65 /* dB */

static const char *methods[] = {
    "Lowpass",
    "Bandpass",
    "Moog ladder",
    NULL
};

static void
update_wah_speed(GtkAdjustment *adj, struct autowah_params *params)
{
    params->sweep_time = adj->value;
}

static void
update_wah_freqlow(GtkAdjustment *adj, struct autowah_params *params)
{
    params->freq_low = adj->value;
}

static void
update_wah_freqhi(GtkAdjustment *adj, struct autowah_params *params)
{
    params->freq_high = adj->value;
}

static void
update_wah_drywet(GtkAdjustment *adj, struct autowah_params *params)
{
    params->drywet = adj->value;
}

static void
update_wah_res(GtkAdjustment *adj, struct autowah_params *params)
{
    params->res = adj->value;
}

static void
update_wah_continuous(GtkAdjustment *adj, struct autowah_params *params)
{
    params->continuous = !params->continuous;
}

static void
update_method(GtkWidget *w, struct autowah_params *params)
{
    int i;
    const char *tmp;
    
    tmp = gtk_entry_get_text(GTK_ENTRY(w));
    if (tmp == NULL)
        return;

    /* I guess we could also strdup the method from the entry. */ 
    for (i = 0; methods[i] != NULL; i += 1) {
	if (strcmp(tmp, methods[i]) == 0) {
	    params->method = i;
	    break;
	}
    }
}

static void
autowah_init(struct effect *p)
{
    int i;
    struct autowah_params *params;

    GtkWidget      *speed_label;
    GtkWidget      *speed;
    GtkObject      *adj_speed;

    GtkWidget      *freq_low;
    GtkWidget      *freqlow_label;
    GtkObject      *adj_freqlow;

    GtkWidget      *freq_high;
    GtkWidget      *freqhi_label;
    GtkObject      *adj_freqhi;
    
    GtkWidget      *res;
    GtkObject	   *adj_res;
    GtkWidget      *res_label;

    GtkWidget      *button, *continuous;
    GtkWidget      *drywet;
    GtkObject	   *adj_drywet;
    GtkWidget      *drywet_label;
    GtkWidget      *parmTable;

    GtkWidget      *method;
    GList          *glist_methods = NULL;

    params = (struct autowah_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(5, 3, FALSE);

    adj_speed = gtk_adjustment_new(params->sweep_time, 100.0,
                               10000.0, 1.0, 10.0, 0.0);
    speed_label = gtk_label_new("Period\nms");
    gtk_table_attach(GTK_TABLE(parmTable), speed_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_speed), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_speed), params);

    speed = gtk_vscale_new(GTK_ADJUSTMENT(adj_speed));
    gtk_widget_set_size_request(GTK_WIDGET(speed),0,100);
    gtk_table_attach(GTK_TABLE(parmTable), speed, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    adj_freqlow = gtk_adjustment_new(params->freq_low,
				     80.0, 330.0, 1.0, 1.0, 0.0);
    freqlow_label = gtk_label_new("Lower freq\n(Hz)");
    gtk_table_attach(GTK_TABLE(parmTable), freqlow_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_freqlow), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_freqlow), params);

    freq_low = gtk_vscale_new(GTK_ADJUSTMENT(adj_freqlow));
    gtk_table_attach(GTK_TABLE(parmTable), freq_low, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_freqhi = gtk_adjustment_new(params->freq_high,
				    500.0, 2000.0, 1.0, 1.0, 0.0);
    freqhi_label = gtk_label_new("Higher freq\n(Hz)");
    gtk_table_attach(GTK_TABLE(parmTable), freqhi_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_freqhi), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_freqhi), params);

    freq_high = gtk_vscale_new(GTK_ADJUSTMENT(adj_freqhi));
    gtk_table_attach(GTK_TABLE(parmTable), freq_high, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    continuous = gtk_check_button_new_with_label("Repeat sweep");
    if (params->continuous)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(continuous), TRUE);
    gtk_signal_connect(GTK_OBJECT(continuous), "toggled",
		       GTK_SIGNAL_FUNC(update_wah_continuous), params);
    gtk_table_attach(GTK_TABLE(parmTable), continuous, 1, 3, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND | GTK_FILL | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    if (p->toggle)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);
    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    drywet_label = gtk_label_new("Dry/Wet\n(%)");
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 4, 5, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);
    adj_drywet = gtk_adjustment_new(params->drywet,
				    0.0, 100.0, 1.0, 5.0, 0.0);
    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));
    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_drywet), params);
    gtk_table_attach(GTK_TABLE(parmTable), drywet, 4, 5, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    
    res_label = gtk_label_new("Resonance\n(%)");
    gtk_table_attach(GTK_TABLE(parmTable), res_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);
    adj_res = gtk_adjustment_new(params->res,
				    30.0, 100.0, 10, 30, 0.0);
    res = gtk_vscale_new(GTK_ADJUSTMENT(adj_res));
    gtk_signal_connect(GTK_OBJECT(adj_res), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_res), params);
    gtk_table_attach(GTK_TABLE(parmTable), res, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    for (i = 0; methods[i] != NULL; i += 1)
        glist_methods = g_list_append(glist_methods, (gchar *) methods[i]);
    method = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(method), glist_methods);
    g_list_free(glist_methods);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(method)->entry), FALSE);
    gtk_table_attach(GTK_TABLE(parmTable), method, 3, 5, 2, 3,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(method)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_method), params);
    
    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Wah-wah"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

static double
power2db(double power)
{
    return log(power) / log(10) * 10;
}

static void
autowah_filter(struct effect *p, struct data_block *db)
{
    struct autowah_params *ap;
    int             i, curr_channel = 0, delay_time;
    double          freq;

    ap = (struct autowah_params *) p->params;

    memcpy(db->data_swap, db->data, db->len * sizeof(DSP_SAMPLE));

    if (ap->continuous) {
        /* recover from noncontinuous sweep */
        if (ap->dir == 0)
            ap->dir = 1.0;
        
        if (ap->f > 1.0 && ap->dir > 0)
            ap->dir = -1;
        if (ap->f < 0.0 && ap->dir < 0)
            ap->dir = 1;
    } else {
        /* Firstly, quiesce wah if we have reached the end of sweep */
        if (ap->f < 0.0) {
            ap->f = 0.0;
            ap->dir = 0;
        }
        delay_time = sample_rate * AUTOWAH_HISTORY_LENGTH / 1000;
        
        /* Estimate signal higher frequency content's power. When user picks
         * the string strongly it's the high frequency content that increases
         * most. */

        /* XXX we should probably treat all channels separately.
         * We just skip all channels but the first presently. */
        for (i = 0; i < db->len; i++) { if (curr_channel == 0) {
            ap->delayed_accum_power += pow(ap->history->get(ap->history, delay_time), 2);
            ap->fresh_accum_power += pow(db->data[i], 2);
            
            ap->delayed_accum_delta +=
                pow(ap->history->get(ap->history, delay_time) -
                    ap->history->get(ap->history, delay_time - 1), 2);
            
            ap->fresh_accum_delta +=
                    pow(db->data[i] - ap->history->get(ap->history, 0), 2);

            ap->history->add(ap->history, db->data[i]);
            
            ap->accum_n += 1;
            if (ap->accum_n > 8192) {
                ap->fresh_accum_power   /= 2; 
                ap->fresh_accum_delta   /= 2; 
                ap->delayed_accum_power /= 2; 
                ap->delayed_accum_delta /= 2;
                ap->accum_n             /= 2;
            }
        } curr_channel = (curr_channel + 1) % db->channels; }

        /* I skip some scale factors here that would cancel out */
        if ((power2db(ap->fresh_accum_delta) > power2db(ap->delayed_accum_delta)
                                              + AUTOWAH_DISCANT_TRIGGER) ||
            (power2db(ap->fresh_accum_power) > power2db(ap->delayed_accum_power)
                                              + AUTOWAH_BASS_TRIGGER)) {
            ap->f = 1.0;
            ap->dir = -1.0;
        }
    }

    /* in order to have audibly linear sweep, we must map
     * [0..1] -> [freq_low, freq_high] linearly in log2, which requires
     * f(x) = a * 2 ^ (b * x)
     *
     * we know that f(0) = freq_low, and f(1) = freq_high. It follows that:
     * a = freq_low, and b = log2(freq_high / freq_low)
     */

    ap->smoothed_f = (ap->f + ap->smoothed_f) / 2;
    freq = ap->freq_low * pow(2, log(ap->freq_high / ap->freq_low)/log(2) * ap->smoothed_f + 0.2 * sin(ap->freq_vibrato));
    ap->f += ap->dir * 1000.0 / ap->sweep_time * db->len / (sample_rate * db->channels) * 2;

    ap->freq_vibrato += 1000.0 / ap->sweep_time * db->len / (sample_rate * db->channels) * M_PI;
    if (ap->freq_vibrato >= 2 * M_PI)
        ap->freq_vibrato -= 2 * M_PI;
    
    switch (ap->method) {
      case 0:
        /* lowpass resonant filter -- we must avoid setting value 0 to resonance */
        set_lpf_biquad(sample_rate, freq, 1.1 + -ap->res / 100.0, &ap->lpf);
        for (i = 0; i < db->len; i += 1) {
            db->data[i] = do_biquad(db->data[i], &ap->lpf, curr_channel);
            curr_channel = (curr_channel + 1) % db->channels;
        }
        break;
        
      case 1: 
        /* old gnuitar bandpass */
        RC_set_freq(freq, ap->fd);
        RC_bandpass(db, ap->fd);
        break;

      case 2:
        /* Moog ladder filter according to Antti Huovilainen. */

/* I, C, V = electrical parameters
 * f = center frequency
 * r = resonance amount 0 .. 1
 *
 * ya(n) = ya(n-1) + I / (C * f) * (tanh( (x(n) - 4 * r * yd(n-1)) / (2 * V) ) - Wa(n-1))
 * yb(n) = yb(n-1) + I / (C * f) * (Wa(n) - Wb(n-1))
 * yc(n) = yc(n-1) + I / (C * f) * (Wb(n) - Wc(n-1))
 * yd(n) = yd(n-1) + I / (C * f) * (Wc(n) - Wd(n-1))
 *
 * Wx = tanh(Yx(n) / (2 * Vt)) */

        for (i = 0; i < db->len; i += 1) {

#define PARAM_V (MAX_SAMPLE * 2.5) /* the sound gets dirtier if the factor gets small */
            float g = 1 - exp(-2 * M_PI * freq / sample_rate);
            ap->ya[curr_channel] += PARAM_V * g *
                (tanh( (db->data[i] - 4 * ap->res/100.0 * ap->yd[curr_channel]) / PARAM_V )
                 - tanh( ap->ya[curr_channel] / PARAM_V));
            ap->yb[curr_channel] += PARAM_V * g *
                (tanh( ap->ya[curr_channel] / PARAM_V )
                 - tanh( ap->yb[curr_channel] / PARAM_V ));
            ap->yc[curr_channel] += PARAM_V * g *
                (tanh( ap->yb[curr_channel] / PARAM_V )
                 - tanh( ap->yc[curr_channel] / PARAM_V ));
            ap->yd[curr_channel] += PARAM_V * g *
                (tanh( ap->yc[curr_channel] / PARAM_V )
                 - tanh( ap->yd[curr_channel] / PARAM_V ));

            /* the wah causes a gain loss of 12 dB which, but due to resonance we
             * may clip; regardless I'll adjust 12 dB back. */
            db->data[i] = ap->yd[curr_channel] * 4;
            curr_channel = (curr_channel + 1) % db->channels;
        }
        break;

      default:
        break;
    }
    
    /* mix with dry sound */
    for (i = 0; i < db->len; i++)
        db->data[i] = (db->data[i]*ap->drywet + db->data_swap[i]*(100-ap->drywet))/100.0;
}

static void
autowah_done(struct effect *p)
{
    struct autowah_params *ap;
    ap = (struct autowah_params *) p->params;

    free(ap->fd);
    del_Backbuf(ap->history);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
autowah_save(effect_t *p, SAVE_ARGS)
{
    struct autowah_params *params = p->params;

    SAVE_DOUBLE("sweep_time", params->sweep_time);
    SAVE_DOUBLE("freq_low", params->freq_low);
    SAVE_DOUBLE("freq_high", params->freq_high);
    SAVE_DOUBLE("res", params->res);
    SAVE_DOUBLE("drywet", params->drywet);
    SAVE_INT("continuous", params->continuous);
    SAVE_INT("method", params->method);
}

static void
autowah_load(effect_t *p, LOAD_ARGS)
{
    struct autowah_params *params = p->params;
    
    LOAD_DOUBLE("sweep_time", params->sweep_time);
    LOAD_DOUBLE("freq_low", params->freq_low);
    LOAD_DOUBLE("freq_high", params->freq_high);
    LOAD_DOUBLE("res", params->res);
    LOAD_DOUBLE("drywet", params->continuous);
    LOAD_INT("continuous", params->continuous);
    LOAD_INT("method", params->method);
}

effect_t *
autowah_create()
{
    effect_t *p;
    struct autowah_params *ap;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct autowah_params));
    ap = p->params;
    p->proc_init = autowah_init;
    p->proc_filter = autowah_filter;
    p->toggle = 0;
    p->proc_done = autowah_done;
    p->proc_save = autowah_save;
    p->proc_load = autowah_load;
    ap->fd = calloc(1, sizeof(struct filter_data));
    RC_setup(2, 1.48, ap->fd);
    ap->history = new_Backbuf(MAX_SAMPLE_RATE * AUTOWAH_HISTORY_LENGTH / 1000);
    
    ap->method = 0; /* low-pass resonant filter */
    ap->freq_low = 280;
    ap->freq_high = 900;
    ap->sweep_time = 500;
    ap->drywet = 100;
    ap->continuous = 0;
    ap->res = 85;

    return p;
}
