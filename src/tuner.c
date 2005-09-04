/*
 * GNUitar - tuner plugin
 * Copyright (c) 2005 Antti Lankila  <alankila@bel.fi>
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
 ****************************************************************************
 *
 * This plugin makes no changes to the input signal.
 * 
 * The core of the program is basically a O(N*M) scanner that looks for
 * a repeating structure in the sample data, accepting some deviation.
 * It starts from the highest possible frequency (shortest possible loop)
 * and then goes down, finding the repeating structure. (This is why chords
 * measure as very low pitches as analysed by this program.) Despite the
 * algorithm is not impressive it is not overwhelmingly slow, because
 * N and M are reasonably small and the algorithm can take some shortcuts
 * in the typical cases. However, if you give it a complicated signal the
 * results are usually wrong.
 *
 * The signal is refined with the following steps:
 *
 * 1. noise reduction by averaging successive samples (pump.c)
 * 2. bias elimination by substracting an exponential average (pump.c)
 * 3. power normalizing to control the effects of decay
 * 4. when the repeating structure is found, the fractional loop
 *    length is estimated from weighing the good enough loop lengths.
 * 5. sometimes (due to noise or bad luck) the length measured is bogus.
 *    A sample of recent measurements is kept and its median is accepted
 *    as the actual measurement.
 * 6. the final result itself is an exponential average of the median.
 * 
 * Overall these steps should make the measurements very reliable and
 * stable, but your mileage may vary; it's tuned for my strato and may
 * still need some tweaking for electric basses and like. It also may
 * work better with particular pickups. At least here, neck pickups
 * seem to work very well.
 * 
 * TODO:
 *
 *  - support other scales, not just the equal tempered
 *    - need to input the desired base tone to tune against
 *    - finetuning? (set A to other values but 440 Hz)
 * 
 * $Id$
 * 
 * $Log$
 * Revision 1.15  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.14  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.13  2005/09/03 23:29:03  alankila
 * - I finally cracked the alignment problem. You align GTK labels with
 *   gtk_misc_set_alignment.
 *
 * Revision 1.12  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.11  2005/09/01 22:41:08  alankila
 * - simplifications and fixes
 *
 * Revision 1.10  2005/09/01 19:07:18  alankila
 * - make multichannel ready, although it just tries to mix channels together
 *
 * Revision 1.9  2005/09/01 13:36:23  alankila
 * Objectify backbuf, correct naming and make it a typedef.
 *
 * Revision 1.8  2005/08/28 21:45:30  fonin
 * Added type casting for >> operations on SAMPLE16/32 vars, to shut up MSVC
 *
 * Revision 1.7  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.6  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.5  2005/08/11 17:57:22  alankila
 * - add some missing headers & fix all compiler warnings on gcc 4.0.1+ -Wall
 *
 * Revision 1.4  2005/08/10 17:55:11  alankila
 * - migrate tuner to use the backbuff code
 *
 * Revision 1.3  2005/08/08 16:30:59  alankila
 * - noise-reducing 4-tap FIR. This should kill signal fairly completely
 *   around 11 kHz.
 *
 * Revision 1.2  2005/08/08 12:02:58  fonin
 * Fixed C++ coding style which did not work on pure C
 *
 * Revision 1.1  2005/08/07 12:53:42  alankila
 * - new tuner plugin / effect
 * - some gcc -Wall shutups
 * - added the entry required for gnuitar.vcproj as well but I can't test it
 * - changed pump.h to use enum instead of bunch-of-defines. Hopefully it's
 *   better that way.
 *
 */

#include "gui.h"
#include "pump.h"
#include "tuner.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

#define GUI_UPDATE_INTERVAL	125.0 /* ms */
#define MIN_HZ	    27.5	/* lowest tone that will work: freq. of A */
#define MAX_HZ	    1800.0	/* highest tone that will work */
#define HISTORY_SIZE 1024	/* history buffer size: allows ~50 Hz */
#define COMPARE_LEN 192		/* sample data examining length */
#define NOTES_N	    12		/* the note scale */
#define NOTES_TO_C  9		/* how many notes to C sound from MIN_HZ */
const char *notes[] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"
};

void tuner_filter(struct effect *p, struct data_block *db);
void tuner_done_really(struct effect *p);
gboolean timeout_update_label(gpointer gp);

void ignored_event(void *whatever) {
    return;
}

#define OPTS __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK)
void
tuner_init(struct effect *p)
{
    struct tuner_params *params;
    GtkWidget *label;
    GtkWidget *table;
    GtkWidget *slider;
    
    params = (struct tuner_params *) p->params;
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
                       GTK_SIGNAL_FUNC(delete_event), p);
    
    table = gtk_table_new(2, 3, FALSE);
 
    label = gtk_label_new("Current: ");
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
		     OPTS, OPTS, 2, 2);
    label = gtk_label_new("Ideal: ");
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
		     OPTS, OPTS, 2, 2);
    
    label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1,
		     OPTS, OPTS, 2, 2);
    params->label_current = label;
    label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 1, 2,
		     OPTS, OPTS, 2, 2);
    params->label_ideal = label;

    slider = gtk_hruler_new();
    gtk_ruler_set_range(GTK_RULER(slider), -0.5, 0.5, 0.0, 100);
    gtk_signal_connect(GTK_OBJECT(slider), "motion_notify_event",
                       GTK_SIGNAL_FUNC(ignored_event), NULL);
    gtk_table_attach(GTK_TABLE(table), slider, 0, 2, 2, 3,
		     OPTS, OPTS, 2, 2);
    params->ruler = slider;
    
    gtk_container_add(GTK_CONTAINER(p->control), table);
    g_timeout_add(GUI_UPDATE_INTERVAL, timeout_update_label, p);
    
    gtk_widget_show_all(p->control);
}

gboolean
timeout_update_label(gpointer gp)
{
    struct effect *p = gp;
    struct tuner_params *params;
    double halfnotes;
    double accuracy;
    double ideal;
    int note;
    
    params = p->params;
    if (params->quitting) {
	tuner_done_really(p);
	return FALSE;
    }
    
    if (params->freq == 0) {
	gtk_label_set_text(GTK_LABEL(params->label_current), "-");
	gtk_label_set_text(GTK_LABEL(params->label_ideal),   "-");
	gtk_ruler_set_range(GTK_RULER(params->ruler), -0.5, 0.5, 0.0, 100);
	return TRUE;
    }

    halfnotes = (log(params->freq) - log(MIN_HZ)) / log(2);
    note = (int) (halfnotes * NOTES_N + 0.5);
    ideal = MIN_HZ * pow(2, (double) note / NOTES_N);
    
    /* drop everything but dec fractions, output in [-0.5, 0.5] */
    accuracy = halfnotes * NOTES_N - note;
    gtk_ruler_set_range(GTK_RULER(params->ruler), -0.5, 0.5, accuracy, 100);
    
    /* clamp to array */
    note = (NOTES_TO_C + note) % NOTES_N;
    if (note < 0)
	note = 0; /* bullshit, but shouldn't trigger */
       	
    sprintf(params->freq_str_buf, "%.1f Hz", params->freq);
    gtk_label_set_text(GTK_LABEL(params->label_current), params->freq_str_buf);
    sprintf(params->freq_str_buf, "%.1f Hz (%s)", ideal, notes[note]);
    gtk_label_set_text(GTK_LABEL(params->label_ideal), params->freq_str_buf);
    
    return TRUE;
}

int cmp_double(const void *a, const void *b)
{
    const double *da = a;
    const double *db = b;
    if (*da < *db)
	return -1;
    if (*da > *db)
	return 1;
    return 0;
}

void
tuner_filter(struct effect *p, struct data_block *db)
{
    struct tuner_params *params;
    int			i, j, loop_len;
    DSP_SAMPLE	       *s;
    DSP_SAMPLE		newval;
    double		power = 0,
			good_loop_len = 0,
			good_loop_len_n = 0,
			max_diff = 0,
			max_tmp2 = 0,
			freq = 0;
    
    i = db->len;
    s = db->data;
    params = p->params;

    while (i > 0) {
	newval = 0;
	for (j = 0; j < db->channels; j += 1) {
	    newval += ((SAMPLE32)*s++ >> 8) / db->channels;
            i--;
        }

	/* smooth signal a bit for noise reduction */
	/* NR is FIR with y_n = 1/k * sum(x_i) */
	params->oldval[3] = params->oldval[2];
	params->oldval[2] = params->oldval[1];
	params->oldval[1] = params->oldval[0];
	params->oldval[0] = newval;
	
	newval = (params->oldval[3] + params->oldval[2] + params->oldval[1] + params->oldval[0]) / 4;
	
	power += newval * newval;
	params->history->add(params->history, newval);
    }
    power /= db->len;
    
    /* smoothed power of the signal */
    power = params->power = (power + params->power * 15) / 16;
 
    /* don't try to analyse too quiet a signal. */
    if (power < 900) {
        /* set "no measurement" state */
        params->freq = 0;
        return;
    }
    
    /* now look for similarities in the history buffer.
     * we start from the beginning of the history
     * and look for the first sequence that repeats
     * self-similarly enough (specified using power below). */

    /* 128 is semirandomly chosen. Higher values would work, too, but I want
     * the loop to have a good chance of finding a match. There shouldn't be
     * any more noise, and the weighing at end should keep precision good. */
    max_diff = power * COMPARE_LEN / 128.0;
    /* this value must be much higher or we give up too early on noisy signal */
    max_tmp2 = power / 3.0;
    loop_len = sample_rate / MAX_HZ - 1;
  NEXT_SEARCH:
    while (++loop_len < HISTORY_SIZE - COMPARE_LEN) {
        double diff = 0;
        double weight = 0;
        for (i = 0; i < COMPARE_LEN; i += 1) {
            DSP_SAMPLE tmp = params->history->get(params->history, i) - params->history->get(params->history, i + loop_len);
            double tmp2 = tmp * tmp;
            diff += tmp2;
            /* give up as soon as possible */
            if (diff > max_diff || tmp2 > max_tmp2) {
    	        /* exit when it looks like the values are getting worse */
	        if (good_loop_len_n != 0 && i < (int) (COMPARE_LEN * 0.98))
	            goto END_SEARCH;
	        else
	            goto NEXT_SEARCH;
            }
        }
        /* weigh results by goodness of match */
        weight = 1 - diff / max_diff + 0.001; /* guard against 0 */
        weight = weight * weight;
        good_loop_len   += loop_len * weight;
        good_loop_len_n += weight;
    }
  END_SEARCH:

    /* did we find any? */
    if (good_loop_len_n == 0) {
        params->freq = 0;
        return;
    }
    
    /* average loop len; hopefully better than any of the discrete values */
    good_loop_len /= good_loop_len_n;

    /* If we are reinitializing, prefill the history to improve the time
     * it takes to get a valid median */
    freq = sample_rate / good_loop_len;
    if (params->freq == 0) {
	for (i = 0; i < FREQ_SIZE; i += 1) {
            params->freq_history[i] = freq;
	}
	params->freq = freq;
    }
    
    /* prepare to eliminate noise from the measurements.
     * sometimes the sample data we get is bad (noisy/scratchy)
     * and we'll find bullshit values. */
    params->freq_history[params->freq_index++] = freq;
    if (params->freq_index == FREQ_SIZE)
	params->freq_index = 0;

    /* qsort and get median */
    memcpy(params->sorted_freq_history, params->freq_history, sizeof(params->sorted_freq_history));
    qsort(&params->sorted_freq_history, FREQ_SIZE, sizeof(params->sorted_freq_history[0]), cmp_double);
    /* average the two at middle to eliminate bias from median calculation */
    freq = (params->sorted_freq_history[FREQ_SIZE / 2] + params->sorted_freq_history[FREQ_SIZE / 2 - 1]) / 2;
    
    /* update the exponential average from the median */
    params->freq = (freq + 3 * params->freq) / 4;
    return;
}

void
tuner_done(struct effect *p)
{
    struct tuner_params *params;
    params = p->params;
    params->quitting = 1;
    return;
}

void tuner_done_really(struct effect *p) {
    struct tuner_params *params;
    
    params = p->params;
    del_Backbuf(params->history);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

effect_t *
tuner_create()
{
    effect_t       *p;
    struct tuner_params *params;
    
    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct tuner_params));
    p->proc_init = tuner_init;
    p->proc_filter = tuner_filter;
    p->id = TUNER;
    p->proc_done = tuner_done;
    p->proc_save = NULL;
    p->proc_load = NULL;
    
    params = p->params;
    params->history = new_Backbuf(HISTORY_SIZE);
    
    return p;
}
