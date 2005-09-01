/*
 * GNUitar
 * Pump module - processeing sound
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
 * Revision 1.35  2005/09/01 00:35:35  alankila
 * - add support for GKeyFile into glib 1.2
 *
 * Revision 1.34  2005/08/28 21:45:12  fonin
 * Portability: introduced new functions for mutexes
 *
 * Revision 1.33  2005/08/28 12:42:27  alankila
 * move write_track flag properly into pump.h, make it volatile because it's
 * shared by threads
 *
 * Revision 1.32  2005/08/28 12:39:01  alankila
 * - make audio_lock a real mutex
 * - fix mutex cleanup at exit
 *
 * Revision 1.31  2005/08/27 19:05:43  alankila
 * - introduce SAMPLE16 and SAMPLE32 types, and switch
 *
 * Revision 1.30  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.29  2005/08/24 21:44:44  alankila
 * - split sound drivers off main.c
 * - add support for alsa
 * - rework thread locking
 * - in this version, sound drivers are chosen at compile time
 * - windows driver is probably broken
 *
 * Revision 1.28  2005/08/23 22:01:34  alankila
 * - add -Wall to ease developing
 * - fix warnings
 *
 * Revision 1.27  2005/08/22 11:09:13  alankila
 * - close fd at end
 *
 * Revision 1.26  2005/08/22 11:07:27  alankila
 * - move last bits of tracker support off main.c to pump.c
 * - add settings loader/saver for GTK2, now needs GTK+ 2.6 in minimum
 *
 * Revision 1.25  2005/08/21 23:44:13  alankila
 * - use libsndfile on Linux to write audio as .wav
 * - move clipping tests into pump.c to save writing it in tracker and 3 times
 *   in main.c
 * - give default name to .wav from current date and time (in ISO format)
 * - there's a weird bug if you cancel the file dialog, it pops up again!
 *   I have to look into what's going on.
 *
 * Revision 1.24  2005/08/12 17:56:16  alankila
 * use one global sin lookup table
 *
 * Revision 1.23  2005/08/12 11:21:38  alankila
 * - add master volume widget
 * - reimplement bias computation to use true average
 *
 * Revision 1.22  2005/08/10 17:52:40  alankila
 * - don't test foo < 0 for unsigned
 *
 * Revision 1.21  2005/08/10 10:54:39  alankila
 * - add output VU meter. The scale is logarithmic, resolves down to -96 dB
 *   although it's somewhat wasteful at the low end.
 * - add bias elimination code -- what I want is just a long-term average
 *   for estimating the bias. Right now it's a bad estimation of average but
 *   I'll improve it later on.
 * - the vu-meter background flashes red if clipping. (I couldn't make the
 *   bar itself flash red, the colour is maybe not FG or something.)
 * - add *experimental* noise reduction code. This code will be moved into
 *   a separate NR effect later on. Right now it's useful for testing, and
 *   should not perceptibly degrade the signal anyway.
 *
 * Revision 1.20  2005/08/08 18:34:45  alankila
 * - rename effects:
 *   * vibrato -> tremolo
 *   * tremolo -> vibrato
 *   * distort2 -> overdrive
 *
 * Revision 1.19  2005/08/07 12:53:42  alankila
 * - new tuner plugin / effect
 * - some gcc -Wall shutups
 * - added the entry required for gnuitar.vcproj as well but I can't test it
 * - changed pump.h to use enum instead of bunch-of-defines. Hopefully it's
 *   better that way.
 *
 * Revision 1.18  2005/04/24 19:11:22  fonin
 * Optimized for zero input (after the noise filter) to avoid the extra calcs
 *
 * Revision 1.17  2005/04/15 14:32:39  fonin
 * Few improvements with the effects save/load; fixed nasty bug with CR/LF translation when saving preset files on Win32
 *
 * Revision 1.16  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.15  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 * Revision 1.14  2003/12/21 08:40:36  dexterus
 * biquad files amd eqbank working
 *
 * Revision 1.13  2003/04/12 20:02:10  fonin
 * New noise gate effect.
 *
 * Revision 1.12  2003/04/11 18:34:36  fonin
 * Added distort2 effect.
 *
 * Revision 1.11  2003/03/28 19:56:08  fonin
 * Sampling rate is 44100 by default.
 *
 * Revision 1.10  2003/03/09 21:12:41  fonin
 * New variables for new "change sampling params" feature.
 *
 * Revision 1.9  2003/02/05 21:10:10  fonin
 * Cleanup before release.
 *
 * Revision 1.8  2003/02/03 17:23:26  fonin
 * One more newline after the effects were loaded by pump_start().
 *
 * Revision 1.7  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.6  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.5  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.4  2001/03/25 17:42:32  fonin
 * open() can overwrite existing files from now, because program switches back to real user priorities after start.
 *
 * Revision 1.3  2001/03/25 12:10:06  fonin
 * Text messages begin from newline rather than end with it.
 *
 * Revision 1.2  2001/01/13 10:02:35  fonin
 * Fix: setuid root program shouldnt overwrite existing files.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:01  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#    include <windows.h>
#endif
#include <assert.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include "pump.h"
#include "gui.h"
#include <glib12-compat.h>

#include "autowah.h"
#include "phasor.h"
#include "chorus.h"
#include "delay.h"
#include "echo.h"
#include "tremolo.h"
#include "vibrato.h"
#include "distort.h"
#include "distort2.h"
#include "sustain.h"
#include "reverb.h"
#include "tracker.h"
#include "noise.h"
#include "eqbank.h"
#include "tuner.h"
#include "utils.h"

struct effect  *effects[MAX_EFFECTS];

int             n = 0;

extern char     version[];
/* flag for whether we are creating .wav */
volatile unsigned short  write_track = 0;

/* default settings */
unsigned short  nchannels = 1;
unsigned int    sample_rate = 44100;
unsigned short  bits = 16;
unsigned int    buffer_size = MIN_BUFFER_SIZE * 2;
my_mutex        effectlist_lock=NULL;
#ifndef _WIN32
unsigned int    fragments = 2;
#else
unsigned int    nbuffers = MAX_BUFFERS;
#endif

int sin_lookup_table[SIN_LOOKUP_SIZE];

double bias_s[MAX_CHANNELS];
int    bias_n[MAX_CHANNELS];

void
bias_elimination(struct data_block *db) {
    int             i;
    int             curr_channel = 0;

    for (i = 0; i < db->len; i += 1) {
        bias_s[curr_channel] += db->data[i];
	bias_n[curr_channel] += 1;
        db->data[i] -= bias_s[curr_channel] / bias_n[curr_channel];
        curr_channel = (curr_channel + 1) % db->channels;
    }
    /* keep bias within computable value */
    for (i = 0; i < MAX_CHANNELS; i += 1) {
	if (fabs(bias_s[i]) >= 1E12) {
	    bias_s[i] /= 2;
	    bias_n[i] /= 2;
	}
    }
}

/* a practical maximum is 3. Above that you lose too much discant and
 * it starts to sound like a big-speakered guitar amp... */
#define NR_SIZE 2
DSP_SAMPLE nr_last[MAX_CHANNELS][NR_SIZE];

void
noise_reduction(struct data_block *db) {
    int             i, j;
    int             curr_channel = 0;
    DSP_SAMPLE      tmp;

    for (i = 0; i < db->len; i += 1) {
        for (j = NR_SIZE; j > 1; j -= 1)
            nr_last[curr_channel][j-1] = nr_last[curr_channel][j-2];
        nr_last[curr_channel][0] = db->data[i];

        tmp = 0;
        for (j = 0; j < NR_SIZE; j += 1)
            tmp += nr_last[curr_channel][j];
        db->data[i] = tmp / NR_SIZE;

        curr_channel = (curr_channel + 1) % db->channels;
    }
}

void
vu_meter(struct data_block *db) {
    int             i;
    DSP_SAMPLE      sample, max_sample = 0;
    double          peak, power = 0;

    for (i = 0; i < db->len; i += 1) {
        sample = db->data[i];
        power += pow(sample, 2);
        if (sample < 0)
            sample = -sample;
        if (sample > max_sample)
            max_sample = sample;
    }
    /* energy per sample scaled down to 0.0 - 1.0 */
    power = power / db->len / MAX_SAMPLE / MAX_SAMPLE;
    peak = (double) max_sample / MAX_SAMPLE;
    set_vumeter_value(peak, power);
}

void
adjust_master_volume(struct data_block *db) {
    int		    i;
    double	    volume = pow(10, master_volume / 20.0);

    for (i = 0; i < db->len; i += 1) {
	double val = db->data[i] * volume;
        if (val < -MAX_SAMPLE)
            val = -MAX_SAMPLE;
        if (val > MAX_SAMPLE)
            val = MAX_SAMPLE;
        db->data[i] = val;
    }
}

void
dither_output(struct data_block *db) {
    int		    i;
    static unsigned int randseed = 1;

    for (i = 0; i < db->len; i += 1) {
	db->data[i] += rand_r(&randseed) / (RAND_MAX / 256);
    }
}

void
adapt_to_output(struct data_block *db)
{
    int             i;
    int             size = db->len;
    DSP_SAMPLE     *s = db->data;

    /* temporarily here to make this compile */
    int n_output_channels;
    return;

    assert(db->channels <= n_output_channels);

    /* nothing to do */
    if (db->channels == n_output_channels)
        return;
    /* clone 1 to 2 */
    if (db->channels == 1 && n_output_channels == 2) {
        for (i = size - 1; i > 0; i -= 1) {
            s[i*2+1] = s[i];
            s[i*2  ] = s[i];
        }
        db->channels = 2;
        db->len *= 2;
        return;
    }
    /* clone 1 to 4, mute channels 2 & 3 (the rear channels) */
    if (db->channels == 1 && n_output_channels == 4) {
        for (i = size - 1; i > 0; i -= 1) {
            s[i*4+3] = 0;
            s[i*4+2] = 0;
            s[i*4+1] = s[i];
            s[i*4  ] = s[i];
        }
        db->channels = 4;
        db->len *= 4;
        return;
    }
    /* clone 2 to 4, mute channels 2 & 3 */
    if (db->channels == 2 && n_output_channels == 4) {
        for (i = size/2-1; i > 0; i -= 1) {
            s[i*4+3] = 0;
            s[i*4+2] = 0;
            s[i*4+1] = s[i*2+1];
            s[i*4+0] = s[i*2+0];
        }
        db->channels = 4;
        db->len *= 2;
        return;
    }
    /* we shouldn't have more than 2 channels coming in, and we don't support
     * generating to 5 channels, so error out */

    fprintf(stderr, "unknown channel combination: %d in and %d out",
            db->channels, n_output_channels);
}

int
pump_sample(struct data_block *db)
{
    int             i;

    /* NR is enabled only experimentally until
     * the noise filter will have been implemented */
    noise_reduction(db);
    bias_elimination(db);

    my_lock_mutex(effectlist_lock);
    for (i = 0; i < n; i++)
	effects[i]->proc_filter(effects[i], db);
    my_unlock_mutex(effectlist_lock);

    adapt_to_output(db);
    adjust_master_volume(db);
    if (bits == 16)
	dither_output(db);
    vu_meter(db);

    if (write_track) {
	track_write(db->data, db->len);
    }

    return 0;
}

/* note that vibrato & tremolo effects are swapped */
struct effect_creator effect_list[] = {
    {"autowah", autowah_create},
    {"distort", distort_create},
    {"delay", delay_create},
    {"reverb", reverb_create},
    {"tremolo", vibrato_create},
    {"chorus", chorus_create},
    {"echo", echo_create},
    {"phasor", phasor_create},
    {"vibrato", tremolo_create},
    {"sustain", sustain_create},
    {"overdrive", distort2_create},
    {"noise gate", noise_create},
    {"eq bank", eqbank_create},
    {"tuner", tuner_create},
    {NULL, NULL}
};

void init_sin_lookup_table() {
    int i = 0;
    for (i = 0; i < SIN_LOOKUP_SIZE; i += 1)
        sin_lookup_table[i] = sin(2 * M_PI * i / SIN_LOOKUP_SIZE) * SIN_LOOKUP_AMPLITUDE;
}

const gchar *
discover_settings_path() {
    const gchar *path;
#ifdef _WIN32
    path = g_strdup_printf("%s\\%s", g_get_home_dir(), ".gnuitarrc");
#else
    path = g_strdup_printf("%s/%s", g_get_home_dir(), ".gnuitarrc");
#endif
    return path;
}

void
load_settings() {
    const gchar    *settingspath;
    GKeyFile       *file;
    GError         *error;
    gint            tmp;

    settingspath = discover_settings_path();
    file = g_key_file_new();

    /* Thanks, glib! */
    g_key_file_load_from_file(file, settingspath, G_KEY_FILE_NONE, NULL);

    /* this seems a bit clumsy, maybe I should do
     * { "bits", &bits, INTEGER } structure */
    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "bits", &error);
    if (error == NULL)
        bits = tmp;

    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "n_output_channels", &error);
    if (error == NULL)
        nchannels = tmp;

    /*
    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "n_input_channels", &error);
    if (error == NULL)
        n_input_channels = tmp;
    */

    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "sample_rate", &error);
    if (error == NULL)
        sample_rate = tmp;

    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "buffer_size", &error);
    if (error == NULL)
        buffer_size = tmp;

#ifdef _WIN32
    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "n_inout_buffers", &error);
    if (error == NULL)
        nbuffers = tmp;
#endif
    g_key_file_free(file);
    free((void *) settingspath);
    return;
}

void save_settings() {
    const gchar    *settingspath;
    GKeyFile       *file;
    gchar          *key_file_as_str;
    gsize           length;
    int             w_length;
    int             fd;

    settingspath = discover_settings_path();
    file = g_key_file_new();

    g_key_file_set_integer(file, "global", "bits", bits);
    g_key_file_set_integer(file, "global", "n_output_channels", nchannels);
    //g_key_file_set_integer(file, "global", "n_input_channels", n_input_channels);
    g_key_file_set_integer(file, "global", "sample_rate", sample_rate);
    g_key_file_set_integer(file, "global", "buffer_size", buffer_size);
#ifdef _WIN32
    g_key_file_set_integer(file, "global", "n_inout_buffers", nbuffers);
#endif
    key_file_as_str = g_key_file_to_data(file, &length, NULL);

    /* there's g_set_file_contents() in glib 2.8 */
    fd = g_open(settingspath, O_WRONLY | O_TRUNC | O_CREAT, 0777);
    if (fd == -1)
        goto SAVE_SETTINGS_CLEANUP1;
    w_length = write(fd, key_file_as_str, length);
    if (w_length != length)
        perror("Failed to write settings file completely: ");
    close(fd);

  SAVE_SETTINGS_CLEANUP1:
    g_key_file_free(file);
    free((void *) settingspath);
    free((void *) key_file_as_str);
    return;
}

void
pump_start(int argc, char **argv)
{
    int             i,
                    j;

    void            (*create_f[10]) (struct effect *);

    my_create_mutex(&effectlist_lock);
    init_sin_lookup_table();

    master_volume = 0.0;
    j = 0;

    if (argc == 1) {
	int             k = 0;
	printf("\nPossible effects:");
	while (effect_list[k].str) {
	    printf("\n  %s", effect_list[k].str);
	    k++;
	}
    }
    for (i = 1; i < argc; i++) {
	int             k = 0;
	while (effect_list[k].str && strcmp(argv[i], effect_list[k].str)) {
	    k++;
	}
	if (effect_list[k].str) {
	    create_f[j++] = effect_list[k].create_f;
	    printf("\nadding %s", effect_list[k].str);
	    gtk_clist_append(GTK_CLIST(processor), &effect_list[k].str);
	} else {
	    printf("\n%s is not a known effect", argv[i]);
	}
    }
    create_f[j++] = NULL;

    /*
     * Cleaning effects[]
     */
    for (j = 0; j < MAX_EFFECTS; j++) {
	effects[j] = NULL;
    }

    while (n < MAX_EFFECTS && create_f[n]) {
	effects[n] = (struct effect *) calloc(1, sizeof(struct effect));
	create_f[n] (effects[n]);
	effects[n]->proc_init(effects[n]);
	n++;
    }

    memset(bias_s, 0, sizeof(bias_s));
    memset(bias_n, 0, sizeof(bias_n));
    memset(nr_last, 0, sizeof(nr_last));
}

void
pump_stop(void)
{
    int             i;

    if (write_track) {
        write_track = 0;
        tracker_done();
    }
    for (i = 0; i < n; i++) {
	effects[i]->proc_done(effects[i]);
    }
    n = 0;
    my_close_mutex(effectlist_lock);
}

void
passthru(struct effect *p, struct data_block *db)
{
}

/* for UNIX; O_BINARY exists only on Win32. We must open() files with this
 * flag because otherwise it gets corrupted by the CR/LF translation */
#ifndef O_BINARY
#  define O_BINARY 0
#endif

void
save_pump(const char *fname)
{
    int             i;
    int             fd = 0;

    fprintf(stderr, "\nWriting preset (%s)...", fname);
    if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                                            S_IREAD | S_IWRITE)) < 0) {
	perror("Save failed");
	return;
    }

    /*
     * writing signature
     */
    write(fd, version, 13);
    for (i = 0; i < n; i++) {
	if (effects[i]->proc_save != NULL) {
	    write(fd, &effects[i]->id, sizeof(effects[i]->id));
	    write(fd, &effects[i]->toggle, sizeof(effects[i]->toggle));
	    effects[i]->proc_save(effects[i], fd);
	}
    }
    close(fd);
    fprintf(stderr, "ok\n");
}

void
load_pump(const char *fname)
{
    int             fd = 0;
    unsigned short  effect_tag = MAX_EFFECTS+1;
    char            rc_version[32]="";

    if (!(fd = open(fname, O_RDONLY | O_BINARY, S_IREAD | S_IWRITE))) {
	perror("Load failed");
	return;
    }

    /*
     * reading signature and compare with our version
     */
    read(fd, rc_version, 13);
    if (strncmp(version, rc_version, 13) != 0) {
	fprintf(stderr, "\nThis is not my rc file.");
	close(fd);
	return;
    }

    gtk_clist_clear(GTK_CLIST(processor));
    my_lock_mutex(effectlist_lock);
    pump_stop();

    n = 0;
    while (read(fd, &effect_tag, sizeof(unsigned short)) > 0) {
	if (effect_tag > EFFECT_AMOUNT) {
            fprintf(stderr,"\nInvalid effect %i, load finished",effect_tag);
            break;
        }

	fprintf(stderr, "\nloading %s", effect_list[effect_tag].str);

	effects[n] = (struct effect *) calloc(1, sizeof(struct effect));
	effect_list[effect_tag].create_f(effects[n]);
	read(fd, &effects[n]->toggle, sizeof(unsigned short));
	effects[n]->proc_load(effects[n], fd);
	effects[n]->proc_init(effects[n]);
	gtk_clist_append(GTK_CLIST(processor),
			 &effect_list[effect_tag].str);
	n++;
    }
    close(fd);
    fprintf(stderr, "\n");
    my_unlock_mutex(effectlist_lock);
}

