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
 * Revision 1.33  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.32  2006/08/10 13:57:48  alankila
 * - use fftw3f instead of fftw3 to avoid slower doubles
 *
 * Revision 1.31  2006/08/06 20:14:55  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.30  2006/08/02 19:07:57  alankila
 * - add missing static declarations
 *
 * Revision 1.29  2006/07/18 21:35:59  alankila
 * - add optional FFT-based implementation -- it is several times faster
 *   than the time-domain version and nearly as good.
 *
 * Revision 1.28  2005/10/01 07:55:27  fonin
 * Decreased accuracy required to light the led
 *
 * Revision 1.27  2005/09/06 09:51:50  fonin
 * - added layout for 6-string octave down (mine)
 *
 * Revision 1.26  2005/09/06 01:14:28  alankila
 * - fix dumb copypaste problem
 *
 * Revision 1.25  2005/09/06 01:11:15  alankila
 * - add some new layouts
 *
 * Revision 1.24  2005/09/06 01:01:56  alankila
 * - simplify layout specification a bit
 *
 * Revision 1.23  2005/09/05 19:30:07  alankila
 * - remove some code duplication
 * - to make bass tuning possible, the lowest measurable frequency need to be
 *   adjusted. Bass plays at 42 Hz, so I made it 40 Hz at 48000 Hz sampling
 *
 * Revision 1.22  2005/09/05 19:08:11  alankila
 * - abolish global variables. It's either that, or we forbid opening more than
 *   one tuner at once
 *
 * Revision 1.21  2005/09/05 11:13:19  alankila
 * - tune the table paddings a bit, fix the updated led table to vertical too
 *
 * Revision 1.20  2005/09/05 08:36:12  fonin
 * Indicators turned vertically.
 *
 * Revision 1.19  2005/09/04 23:17:07  alankila
 * - gtk+ ui fixes
 *
 * Revision 1.18  2005/09/04 19:30:24  fonin
 * Added lid indicators for tuner. Added tuning layouts (so far for bass and guitar)
 *
 * Revision 1.17  2005/09/04 14:50:28  alankila
 * - tuner defaults to on
 *
 * Revision 1.16  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
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
#include "tuner.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define logf(x) log(x)
#endif

#define GUI_UPDATE_INTERVAL	125.0 /* ms */
#define MIN_HZ	    27.5	/* lowest tone that will work: freq. of A */
#define MAX_HZ	    400.0	/* highest tone that will work */
#define NOTES_N	    12		/* the note scale */
#define NOTES_TO_C  9		/* how many notes to C sound from MIN_HZ */
#define MAX_STRINGS 6		/* max.number of strings */

static const char *notes[] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"
};

/* images */
static const char *empty_xpm[] = { /* empty black light */
"7 7 10 1",
" 	c None",
".	c #FFFFFF",
"+	c #FFFF00",
"@	c #FF00FF",
"#	c #FF0000",
"$	c #00FFFF",
"%	c #00FF00",
"&	c #0000FF",
"*	c #000000",
"=	c #CFD9CB",
" =***= ",
"=*****=",
"*******",
"*******",
"*******",
"=*****=",
" =***= "};

static const char *green_xpm[] = {	/* green light */
"7 7 4 1",
" 	c None",
".	c #64A356",
"+	c #CFD9CB",
"@	c #77936B",
" +@.@+ ",
"+.....+",
"@.....@",
".......",
"@.....@",
"+.....+",
" +@.@+ "};

GtkPixmap green,black;

/* Tuner layouts
 * The format is the following: the values are distances between tones
 * measured in half-notes. First distance is from the C in the octave
 * that defines MIN_HZ. The subsequent distances are from 1st tone to 2nd
 * and so on. For now, we assume all instruments have 6 strings.
 */
static const unsigned short layouts[][MAX_STRINGS+1]={
    { 24+4, 5, 5, 5, 4, 5, 0 },
    { 24+3, 5, 5, 5, 4, 5, 0 },
    { 24+2, 5, 5, 5, 4, 5, 0 },
    { 12+4, 5, 5, 5, 4, 5, 0 },
    { 12+4, 5, 5, 5, 0, 0, 0 }
};
static const char *layout_names[] = {
    "6-str. guitar E A D G H E",
    "6-str. guitar halfnote down",
    "6-str. guitar fullnote down",
    "6-str. guitar octave down",
    "4-str. bass   E A D G"
};

static void tuner_done_really(effect_t *);
static gboolean timeout_update_label(gpointer gp);
static void calc_layout_gui(struct tuner_params *);
static void update_layout(GtkWidget *widget, gpointer data);

static void
ignored_event(void *whatever, void *whatever2) {
    return;
}

#define OPTS_EXP __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND)
#define OPTS __GTKATTACHOPTIONS(GTK_FILL)
static void
tuner_init(effect_t *p)
{
    struct tuner_params *params = p->params;
    GtkWidget  *label, *table;
    GtkStyle   *style;
    GList      *tuning_layouts = NULL;
    int		i;
    
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_widget_realize(p->control);
    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
                       GTK_SIGNAL_FUNC(delete_event), p);
    
    table = gtk_table_new(3, 4, FALSE);
 
    label = gtk_label_new("Current:");
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
		     OPTS_EXP, OPTS, 3, 3);
    label = gtk_label_new("Ideal:");
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
		     OPTS_EXP, OPTS, 3, 3);
    
    label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1,
		     OPTS_EXP, OPTS, 3, 3);
    params->label_current = label;
    label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 1, 2,
		     OPTS_EXP, OPTS, 3, 3);
    params->label_ideal = label;

    params->ruler = gtk_hruler_new();
    gtk_ruler_set_range(GTK_RULER(params->ruler), -0.5, 0.5, 0.0, 100);
    gtk_signal_connect(GTK_OBJECT(params->ruler), "motion_notify_event",
                       GTK_SIGNAL_FUNC(ignored_event), NULL);
    gtk_table_attach(GTK_TABLE(table), params->ruler, 0, 2, 2, 3,
		     OPTS_EXP, OPTS_EXP, 3, 3);
    
    gtk_container_add(GTK_CONTAINER(p->control), table);
    g_timeout_add(GUI_UPDATE_INTERVAL, timeout_update_label, p);
    
    /* create pixmaps */
    style = gtk_widget_get_style( p->control );

    green.pixmap = gdk_pixmap_create_from_xpm_d( p->control->window,
					    &(green.mask),
                                            &style->bg[GTK_STATE_NORMAL],
                                            (gchar **) green_xpm);

    black.pixmap = gdk_pixmap_create_from_xpm_d( p->control->window,
					    &(black.mask),
                                            &style->bg[GTK_STATE_NORMAL],
                                            (gchar **) empty_xpm);
    gtk_widget_show_all(p->control);

    params->led_table = gtk_table_new(2, MAX_STRINGS, FALSE);
    calc_layout_gui(params);

    gtk_table_attach(GTK_TABLE(table), params->led_table, 2, 3, 0, 4,
		     OPTS, OPTS_EXP, 6, 0);

    for (i = 0; layout_names[i] != NULL; i += 1)
	tuning_layouts = g_list_append(tuning_layouts, (gchar *) layout_names[i]); 
    params->layout_combo = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(params->layout_combo), tuning_layouts);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(params->layout_combo)->entry), FALSE);
    gtk_table_attach(GTK_TABLE(table), params->layout_combo, 0, 2, 3, 4,
		     OPTS_EXP, OPTS, 3, 3);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(params->layout_combo)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_layout), params);

    gtk_widget_show_all(p->control);
}

static void
update_layout(GtkWidget *widget, gpointer data) {
    struct tuner_params *params = data;
    int i;
    const char *tmp=NULL;
    
    tmp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(params->layout_combo)->entry));
    if(tmp == NULL)
	return;

    for (i = 0; layout_names[i] != NULL; i += 1) {
	if (strcmp(tmp, layout_names[i]) == 0) {
	    params->curr_layout = i;
	    break;
	}
    }

    for(i=0;i<MAX_STRINGS;i++) {
        if (params->leds[i] == NULL)
            break;
        gtk_widget_destroy(params->leds[i]);
        gtk_widget_destroy(params->note_letters[i]);
    }
    calc_layout_gui(params);
    for(i=0;i<MAX_STRINGS;i++) {
        if (params->leds[i] == NULL)
            break;
        gtk_widget_show(params->leds[i]);
        gtk_widget_show(params->note_letters[i]);
    }
}

static gboolean
timeout_update_label(gpointer gp)
{
    struct effect *p = gp;
    struct tuner_params *params = p->params;
    gchar  *gtmp;
    double halfnotes;
    double accuracy;
    double ideal;
    int note, note_idx;
    
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
    note_idx = (NOTES_TO_C + note) % NOTES_N;
    if (note_idx < 0)
	note_idx = 0; /* bullshit, but shouldn't trigger */
       	
    gtmp = g_strdup_printf("%.1f Hz", params->freq);
    gtk_label_set_text(GTK_LABEL(params->label_current), gtmp);
    free(gtmp);
    gtmp = g_strdup_printf("%.1f Hz (%s)", ideal, notes[note_idx]);
    gtk_label_set_text(GTK_LABEL(params->label_ideal), gtmp);
    free(gtmp);

    /* light the led, if the accuracy is good */
    if(fabs(accuracy)<=0.05) {
	int i;
	int string=-1;
	/* search in array for the string's index */
	for(i=0;i<MAX_STRINGS;i++) {
	    if (NOTES_TO_C + note == params->layout[i]) {
		string=i;
		break;
	    }
	}
	if(string>=0) {
	    if(params->leds[string]) {
		gtk_widget_destroy(params->leds[string]);
	    }
	    params->leds[string]=gtk_pixmap_new(green.pixmap,green.mask);
    	    gtk_table_attach(GTK_TABLE(params->led_table), params->leds[string], 1, 2,
                             string, string+1, OPTS, OPTS, 2, 2);
	}
    }
    /* darken all leds */
    else {
	int i;
	for(i=0;i<MAX_STRINGS;i++) {
	    if (params->leds[i]) {
		gtk_widget_destroy(params->leds[i]);
	        params->leds[i]=gtk_pixmap_new(black.pixmap,black.mask);
    		gtk_table_attach(GTK_TABLE(params->led_table), params->leds[i], 1, 2,
			         i, i + 1, OPTS, OPTS, 2, 2);
	    }
	}
    }
    gtk_widget_show_all(params->led_table);
    
    return TRUE;
}

static int
cmp_float(const void *a, const void *b)
{
    const float *da = a;
    const float *db = b;
    if (*da < *db)
	return -1;
    if (*da > *db)
	return 1;
    return 0;
}

static void
tuner_filter(struct effect *p, data_block_t *db)
{
    struct tuner_params *params;
    int			i, j;
    DSP_SAMPLE	       *s;
    float		power = 0,
			freq = 0;
#ifdef HAVE_FFTW3
    float               weighted_bin = 0,
                        weighted_bin_n = 0,
                        maxlen = 0;
    int                 maxbin = 0;
#else
    float               good_loop_len = 0,
			good_loop_len_n = 0,
			max_diff = 0,
			max_tmp2 = 0;
    int                 loop_len;
#endif
    s = db->data;
    params = p->params;

    for (i = 0; i < db->len; i += db->channels) {
	DSP_SAMPLE newval = 0;
	for (j = 0; j < db->channels; j += 1) {
	    newval += *s / db->channels;
            s++;
        }

	/* smooth signal a bit for noise reduction */
	/* NR is FIR with y_n = 1/k * sum(x_i) */
	params->oldval[2] = params->oldval[1];
	params->oldval[1] = params->oldval[0];
	params->oldval[0] = newval;
	
	newval = (params->oldval[2] + params->oldval[1] + params->oldval[1] + params->oldval[0]) / (DSP_SAMPLE) 4;
        newval /= (DSP_SAMPLE) 256;
	
        power += newval * newval;
	params->history->add(params->history, newval);
    }
    power /= db->len;
    
    /* smoothed power of the signal */
    power = params->power = (power + params->power * 3.f) / 4.f;
 
    /* don't try to analyse too quiet a signal. */
    if (power < 1800.f) {
        /* set "no measurement" state */
        params->freq = 0;
        return;
    }

#ifdef HAVE_FFTW3
#define FFT_SIZE 4096
#define HISTORY_SIZE FFT_SIZE

    /* don't do FFT too often, with small fragment size the cost is prohibtive... */
    params->count += db->len / db->channels;
    if (params->count < FFT_SIZE/2)
        return;
    params->count -= FFT_SIZE/2;

    /* fill FFT from last input */
    for (i = 0; i < FFT_SIZE; i += 1) {
        /* raised cosine window */
        float w = 0.5f - 0.5f * cos_lookup(i / (float) FFT_SIZE);
        params->fftin[i][0] = params->history->get(params->history, FFT_SIZE - i) * w;
        params->fftin[i][1] = 0;
    }

    /* obtain spectrum of input */
    fftwf_execute(params->fftfw);

    /* obtain logaritmic magnitude of spectrum */
    for (i = 0; i < FFT_SIZE; i += 1) {
        /* abs()**2 */
        float len = params->fftin[i][0] * params->fftin[i][0] + params->fftin[i][1] * params->fftin[i][1];
        /* log(0) -> NaN, let's avoid NaN */
        if (len > 0.f)
            len = logf(len) / 2.f; /* compensate for lack of sqrt with 2 */
        else
            len = -999.f;  /* or any small value */

        /* store magnitudes */
        params->fftin[i][0] = len;
        params->fftin[i][1] = 0;
    }

    /* obtain cepstrum of real magnitude spectrum */
    fftwf_execute(params->fftbw);
    /* note: the complex part should be neglible */

    /* search for maximum quefrency -- this analysis could be better though... */
    for (i = sample_rate / MAX_HZ; i < sample_rate / MIN_HZ; i += 1) {
        float len = fabs(params->fftin[i][0]);
        if (len > maxlen) {
            maxbin = i;
            maxlen = len;
        }
    }

    /* now weigh 10 % on both sides to patch our accuracy up a bit */
    for (i = maxbin * 0.9; i <= maxbin * 1.1; i += 1) {
        float len;
        if (i > FFT_SIZE / 2 - 1)
            continue;
        len = params->fftin[i][0] * params->fftin[i][0];
        weighted_bin += len * i;
        weighted_bin_n += len;
    }
    /* this should be a good approximation of the true quefrency */
    weighted_bin /= weighted_bin_n;

    freq = (float) sample_rate / weighted_bin;
#else
#define HISTORY_SIZE 1200	/* history buffer size: allows ~40 Hz */
#define COMPARE_LEN 192		/* sample data examining length */

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
        float diff = 0;
        float weight = 0;
        for (i = 0; i < COMPARE_LEN; i += 1) {
            DSP_SAMPLE tmp = params->history->get(params->history, i) - params->history->get(params->history, i + loop_len);
            float tmp2 = tmp * tmp;
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
        weight = 1.f - diff / max_diff + 0.001f; /* guard against 0 */
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
#endif

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
    qsort(&params->sorted_freq_history, FREQ_SIZE, sizeof(params->sorted_freq_history[0]), cmp_float);
    /* average the two at middle to eliminate bias from median calculation */
    freq = (params->sorted_freq_history[FREQ_SIZE / 2] + params->sorted_freq_history[FREQ_SIZE / 2 - 1]) / 2;
    
    /* update the exponential average from the median */
    params->freq = (freq + 2.f * params->freq) / 3.f;
    return;
}

/* this method is asynchronous because of timer that needs to cancel itself */
static void
tuner_done(effect_t *p)
{
    struct tuner_params *params = p->params;

    params->quitting = 1;
    return;
}

static void
tuner_done_really(effect_t *p) {
    struct tuner_params *params = p->params;
    
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

    /* standard effect init */
    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct tuner_params));
    p->proc_init = tuner_init;
    p->proc_filter = tuner_filter;
    p->proc_done = tuner_done;
    p->proc_save = NULL;
    p->proc_load = NULL;
    p->toggle = 1;
    
    params = p->params;
    params->history = new_Backbuf(HISTORY_SIZE);

#ifdef HAVE_FFTW3
    /* prepare for FFT transforms */
    params->fftin = fftwf_malloc(sizeof(fftwf_complex) * FFT_SIZE);
    params->fftfw = fftwf_plan_dft_1d(FFT_SIZE, params->fftin, params->fftin, FFTW_FORWARD, FFTW_ESTIMATE);
    params->fftbw = fftwf_plan_dft_1d(FFT_SIZE, params->fftin, params->fftin, FFTW_BACKWARD, FFTW_ESTIMATE);
#endif

    return p;
}

/* Function with side effect - modifies global array "layout".
 * Takes the layout index on input, and calculates freqs of the
 * particular tones. */
static void
calc_layout_gui(struct tuner_params *params) {
    unsigned short curr_note = 0; /* the note # from bottom A */
    int i;

    memset(params->layout, 0, sizeof(params->layout));
    memset(params->leds, 0, sizeof(params->leds));
    memset(params->note_letters, 0, sizeof(params->note_letters));
    curr_note = layouts[params->curr_layout][0];
    for (i = 0; i < MAX_STRINGS; i += 1) {
        params->layout[i] = curr_note;
	if (layouts[params->curr_layout][i+1] == 0)
            break;
        curr_note += layouts[params->curr_layout][i+1];
    }
    
    for (i = 0; i < MAX_STRINGS; i += 1) {
	if (params->layout[i] == 0)
            break;
	
        params->leds[i]=gtk_pixmap_new(black.pixmap,black.mask);
	gtk_table_attach(GTK_TABLE(params->led_table), params->leds[i], 1, 2, i, i+1,
		     OPTS, OPTS_EXP, 2, 2);
	params->note_letters[i]=gtk_label_new(notes[params->layout[i]%NOTES_N]);
	gtk_table_attach(GTK_TABLE(params->led_table), params->note_letters[i], 0, 1, i, i+1,
		     OPTS, OPTS_EXP, 2, 2);
    }
}
