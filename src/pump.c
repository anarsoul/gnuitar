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
 * Revision 1.77  2006/07/29 12:04:36  alankila
 * - effect lickup:
 *   * stereo phaser has less severe phase cancellation effects
 *   * rotary speaker can go even faster
 * - improve performance of sin_lookup, add cos_lookup
 *
 * Revision 1.76  2006/07/28 19:42:15  alankila
 * - fix compat library to not crash on invalid free
 *
 * Revision 1.75  2006/07/28 19:23:41  alankila
 * - reduce lock contention when switching between presets
 *
 * Revision 1.74  2006/07/28 19:08:40  alankila
 * - add midi event listeners into JACK and ALSA
 * - make gui listen to midi events and switch bank
 * - fix a bug involving "add pressed"
 * - change documentation of "Switch preset" to "Cycle presets" which is what
 *   it does now.
 *
 * Revision 1.73  2006/07/27 19:49:39  alankila
 * - blah, forgot one }
 *
 * Revision 1.72  2006/07/27 19:48:46  alankila
 * - remove debug print
 *
 * Revision 1.71  2006/07/27 19:15:35  alankila
 * - split windows driver architecture now compiles and runs.
 *
 * Revision 1.70  2006/07/27 18:31:15  alankila
 * - split dsound and winmm into separate drivers.
 *
 * Revision 1.69  2006/07/16 20:43:32  alankila
 * - use non-white triangular noise for slightly better dithering.
 *
 * Revision 1.68  2006/07/15 23:21:23  alankila
 * - show input/output vu meters separately
 *
 * Revision 1.67  2006/07/15 23:02:45  alankila
 * - remove the bits control -- just use the best available on every driver.
 *
 * Revision 1.66  2006/07/15 21:15:47  alankila
 * - implement triangular dithering on the sound drivers. Triangular dithering
 *   places more noise at the nyquist frequency so the noise floor is made
 *   smaller elsewhere.
 *
 * Revision 1.65  2006/07/15 20:42:56  alankila
 * - remove global dithering code for now. JACK does its own dithering if
 *   configured, so the decision to dither in pump is no longer appropriate.
 *
 * Revision 1.64  2006/06/20 20:41:07  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.63  2006/06/16 12:32:19  alankila
 * - the reasonably trivial vibrato effect is now reinstated as tremolo
 *   in a file with proper name. Old vibrato can be achieved through 1-voice
 *   100% wet chorus.
 *
 * Revision 1.62  2006/05/24 20:06:15  alankila
 * - save some CPU by avoiding the very expensive pow() call on every input.
 *   Use floats because their accuracy suffices for the work.
 *
 * Revision 1.61  2006/05/20 17:32:02  alankila
 * - rename Tremolo effect as Tremolo Bar.
 * - I think about changing Vibrato into what Tremolo used to be. This way the
 *   source file names again make sense.
 *
 * Revision 1.60  2006/05/20 09:56:58  alankila
 * - move audio_driver_str and audio_driver_enabled into driver structure
 * - Win32 drivers are ugly, with the need to differentiate between
 *   DirectX and MMSystem operation through dsound variable. The driver
 *   should probably be split with dsound-specific parts in its own driver.
 *
 * Revision 1.59  2006/05/19 15:12:54  alankila
 * I keep on getting rattles with ALSA playback, seems like ALSA doesn't
 * know when to swap buffers or allows write to go on too easily. I
 * performed a major overhaul/cleanup in trying to kill this bug for good.
 *
 * - fix confusion about what "buffer_size" really means and how many bytes
 *   it takes.
 *   - buffer size is ALWAYS the fragment size in all audio drivers
 *     (ALSA, OSS, Win32 driver)
 *   - it follows that memory needed is buffer_size times maximum
 *     frame size. (32-bit samples, 4 channels, max buffer size.)
 *   - latency calculation updated, but it may be incorrect yet
 * - add write buffer for faster ALSA read-write cycle. (Hopefully this
 *   reduces buffer underruns and rattles and all sort of ugliness we
 *   have with ALSA)
 * - redesign the ALSA configuration code. Now we let ALSA choose the
 *   parameters during the adjustment phase, then we try same for playback.
 * - some bugs squashed in relation to this, variables renamed, etc.
 * - if opening audio driver fails, do not kill the user's audio_driver
 *   choice. (It makes configuring bits, channels, etc. difficult.)
 *   We try to track whether processing is on/off through new variable,
 *   audio_driver_enabled.
 *
 * Note: all the GUI code related to audio is in need of a major overhaul.
 * Several variables should be renamed, variable visibility better controlled.
 *
 * Revision 1.58  2006/05/19 10:17:04  alankila
 * - avoid floating point in input path, especially denormal numbers
 *
 * Revision 1.57  2006/05/13 10:57:57  alankila
 * - We can get better dynamic range if we don't clip everywhere, but only at
 *   the master gain adjustment, just before output is produced.
 *
 * Revision 1.56  2006/05/13 10:16:53  alankila
 * - produce 2 output channels by default rather than 1 to allow new effects
 *   such as rotary speaker to work
 *
 * Revision 1.55  2006/05/13 10:09:52  alankila
 * - allow sin_lookup(1.0) to work
 *
 * Revision 1.54  2006/05/13 07:44:50  alankila
 * - remove pump's noise reduction -- we'll have the real effect soon enough
 * - add rotary speaker simulator based on hilbert transform
 *
 * Revision 1.53  2006/05/07 13:22:12  alankila
 * - new bare bones distortion effect: tubeamp
 *
 * Revision 1.52  2006/05/01 10:23:54  anarsoul
 * Alsa device is selectable and input volume is adjustable now. Added new filter - amp.
 *
 * Revision 1.51  2005/11/05 12:18:38  alankila
 * - pepper the code with static declarations for all private funcs and vars
 *
 * Revision 1.50  2005/10/02 08:23:50  fonin
 * Master volume is stored in the preset file
 *
 * Revision 1.49  2005/09/28 19:52:47  fonin
 * Load and save windows settings correctly
 *
 * Revision 1.48  2005/09/12 09:42:25  fonin
 * - MSVC compatibility fixes
 *
 * Revision 1.47  2005/09/11 12:54:14  alankila
 * - add a tentative pitch shift effect
 *
 * Revision 1.46  2005/09/09 20:22:17  alankila
 * - phasor reimplemented according to a popular algorithm that simulates
 *   high-impedance isolated varying capacitors
 *
 * Revision 1.45  2005/09/05 17:42:07  alankila
 * - fix some small memory leaks
 *
 * Revision 1.44  2005/09/04 20:45:01  alankila
 * - store audio driver into config
 * - make it possible to start gnuitar with invalid audio driver and enter
 *   options and correct the driver. Some rough edges still remain with
 *   the start/stop button, mutexes, etc.
 *
 * Revision 1.43  2005/09/04 16:06:59  alankila
 * - first multichannel effect: delay
 * - need to use surround40 driver in alsa
 * - introduce new buffer data_swap so that effects need not reserve buffers
 * - correct off-by-one error in multichannel adapting
 *
 * Revision 1.42  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.41  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.40  2005/09/04 11:21:48  alankila
 * - one warning is really an error
 *
 * Revision 1.39  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.38  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.37  2005/09/03 23:46:04  alankila
 * - add some release polish
 *
 * Revision 1.36  2005/09/03 22:13:56  alankila
 * - make multichannel processing selectable
 * - new GUI (it sucks as much as the old one and I'll need to grok GTK
 *   tables first before it gets better)
 * - make pump.c do the multichannel adapting bits
 * - effects can now change channel counts
 *
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
#include <sys/stat.h>
#include "pump.h"
#include "main.h"
#include "gui.h"
#include "glib12-compat.h"

#include "amp.h"
#include "autowah.h"
#include "phasor.h"
#include "chorus.h"
#include "delay.h"
#include "echo.h"
#include "tremolo.h"
#include "vibrato.h"
#include "distort.h"
#include "distort2.h"
#include "tubeamp.h"
#include "sustain.h"
#include "reverb.h"
#include "rotary.h"
#include "tracker.h"
#include "noise.h"
#include "eqbank.h"
#include "pitch.h"
#include "tuner.h"
#include "utils.h"

effect_t       *effects[MAX_EFFECTS];
int             n = 0;

/* flag for whether we are creating .wav */
volatile unsigned short  write_track = 0;
volatile midictrl_t midictrl = { 0, 0, 0 };

/* default settings */
char            alsadevice_str[64];
unsigned short  n_input_channels = 1;
unsigned short  n_output_channels = 2;
unsigned int    sample_rate = 44100;
unsigned int    buffer_size = MIN_BUFFER_SIZE * 2;
my_mutex        effectlist_lock = NULL;
#ifndef _WIN32
unsigned int    fragments = 2;
#else
unsigned int    overrun_threshold = 4;
unsigned int    nbuffers = MAX_BUFFERS;
#endif

float sin_lookup_table[SIN_LOOKUP_SIZE + 1];

/* from JACK -- blindingly fast */
static inline unsigned int prng() {
    static unsigned int seed = 22222;
    seed = (seed * 96314165) + 907633515;
    return seed;
}

/* This is triangular correlated noise with frequency spectrum that increases
 * 6 dB per octave, thus most noise occurs at high frequencies. The probability
 * distribution still looks like triangle. Idea and implementation borrowed from
 * JACK. */
void
triangular_dither(data_block_t *db)
{
    static int correlated_noise[MAX_CHANNELS] = { 0, 0, 0, 0 };
    int i, current_channel = 0;
    
    for (i = 0; i < db->len; i += 1) {
        DSP_SAMPLE tmp = db->data[i];
        int noise = (prng() & 0x1ff) - 256; /* -256 to 255 */
        
        tmp += noise - correlated_noise[current_channel];
        correlated_noise[current_channel] = noise;
        
        CLIP_SAMPLE(tmp);
        db->data[i] = tmp;
        current_channel = (current_channel + 1) % db->channels;
    }
}

/* If the long-term average of input data does not exactly equal to 0,
 * compensate. Some soundcards would also need highpass filtering ~20 Hz
 * or so. */
static void
bias_elimination(data_block_t *db) {
    static double bias_s[MAX_CHANNELS] = { 0, 0, 0, 0 };
    static int    bias_n[MAX_CHANNELS] = { 10, 10, 10, 10 };

    int             i, biasadj;
    int             curr_channel = 0;

    biasadj = bias_s[curr_channel] / bias_n[curr_channel];
    
    for (i = 0; i < db->len; i += 1) {
        bias_s[curr_channel] += db->data[i];
	bias_n[curr_channel] += 1;
        db->data[i] -= biasadj;
        curr_channel = (curr_channel + 1) % db->channels;
    }
    /* keep bias within computable value */
    for (i = 0; i < MAX_CHANNELS; i += 1) {
	if (fabs(bias_s[i]) > 1E12) {
	    bias_s[i] /= 2;
	    bias_n[i] /= 2;
	}
    }
}

/* accumulate power estimate and monitor clipping */
static double
vu_meter(data_block_t *db) {
    int             i;
    DSP_SAMPLE      sample, max_sample = 0;
    float           power = 0;

    for (i = 0; i < db->len; i += 1) {
        sample = db->data[i];
        power += (float) sample * (float) sample;
        if (sample < 0)
            sample = -sample;
        if (sample > max_sample)
            max_sample = sample;
    }
    /* energy per sample scaled down to 0.0 - 1.0 */
    return power / db->len / MAX_SAMPLE / MAX_SAMPLE;
}

/* adjust master volume according to the main window slider and clip */
static void
adjust_master_volume(data_block_t *db) {
    int		    i;
    double	    volume = pow(10, master_volume / 20.0);

    for (i = 0; i < db->len; i += 1) {
	double val = db->data[i] * volume;
        CLIP_SAMPLE(val);
        db->data[i] = val;
    }
}

static void
adjust_input_volume(data_block_t *db) {
    int		    i;
    double	    volume = pow(10, input_volume / 20.0);

    for (i = 0; i < db->len; i += 1) {
	double val = db->data[i] * volume;
        db->data[i] = val;
    }
}


static void
adapt_to_output(data_block_t *db)
{
    int             i;
    int             size = db->len;
    DSP_SAMPLE     *s = db->data;

    assert(db->channels <= n_output_channels);

    /* nothing to do */
    if (db->channels == n_output_channels)
        return;
    /* clone 1 to 2 */
    if (db->channels == 1 && n_output_channels == 2) {
        for (i = size - 1; i >= 0; i -= 1) {
            s[i*2+1] = s[i];
            s[i*2  ] = s[i];
        }
        db->channels = 2;
        db->len *= 2;
        return;
    }
    /* clone 1 to 4, mute channels 2 & 3 (the rear channels) */
    if (db->channels == 1 && n_output_channels == 4) {
        for (i = size - 1; i >= 0; i -= 1) {
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
        for (i = size/2-1; i >= 0; i -= 1) {
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

    gnuitar_printf( "unknown channel combination: %d in and %d out",
            db->channels, n_output_channels);
}

void
pump_sample(data_block_t *db)
{
    int             i;

    /* NR is enabled only experimentally until
     * the noise filter will have been implemented */
     
    bias_elimination(db);
    adjust_input_volume(db);
    set_vumeter_in_value(vu_meter(db));

    my_lock_mutex(effectlist_lock);
    for (i = 0; i < n; i++) {
        if (effects[i]->toggle)
            effects[i]->proc_filter(effects[i], db);
    }
    my_unlock_mutex(effectlist_lock);

    adjust_master_volume(db);
    set_vumeter_out_value(vu_meter(db));
    
    adapt_to_output(db);
    if (write_track) {
	track_write(db->data, db->len);
    }
}

/* note that vibrato & tremolo effects are swapped */
struct effect_creator effect_list[] = {
    {"Amp", amp_create},
    {"Autowah", autowah_create},
    {"Distort", distort_create},
    {"Delay", delay_create},
    {"Reverb", reverb_create},
    {"Tremolo bar", vibrato_create},
    {"Chorus / Flanger", chorus_create},
    {"Echo", echo_create},
    {"Phaser", phasor_create},
    {"Tremolo", tremolo_create},
    {"Sustain", sustain_create},
    {"Overdrive", distort2_create},
    {"Tube amplifier", tubeamp_create},
    {"Rotary speaker", rotary_create},
    {"Noise gate", noise_create},
    {"Eq bank", eqbank_create},
    {"Pitch shift", pitch_create},
    {"Tuner", tuner_create},
    {NULL, NULL}
};

static void
init_sin_lookup_table() {
    int i = 0;
    for (i = 0; i < SIN_LOOKUP_SIZE + 1; i += 1)
        sin_lookup_table[i] = sin(2 * M_PI * i / SIN_LOOKUP_SIZE);
}

static const gchar *
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
    gchar          *gstr;
    gint            tmp;

    settingspath = discover_settings_path();
    file = g_key_file_new();

    /* Thanks, glib! */
    g_key_file_load_from_file(file, settingspath, G_KEY_FILE_NONE, NULL);

    error = NULL;
    gstr = g_key_file_get_string(file, "global", "driver", &error);
    if (error == NULL) {
#ifdef HAVE_JACK
        if (strcmp(gstr, "JACK") == 0)
            audio_driver = &jack_driver;
#endif
#ifdef HAVE_ALSA
        if (strcmp(gstr, "ALSA") == 0)
            audio_driver = &alsa_driver;
#endif
#ifdef HAVE_OSS
        if (strcmp(gstr, "OSS") == 0)
            audio_driver = &oss_driver;
#endif
#ifdef HAVE_WINMM
        if (strcmp(gstr, "MMSystem") == 0)
            audio_driver = &winmm_driver;
#endif
#ifdef HAVE_DSOUND
        if (strcmp(gstr, "DirectX") == 0)
            audio_driver = &dsound_driver;
#endif
	free(gstr);
    }
    
    error = NULL;
    gstr = g_key_file_get_string(file, "global", "alsadevice", &error);
    strcpy(alsadevice_str, "default");
    if (error == NULL) {
        strncpy(alsadevice_str, gstr, sizeof(alsadevice_str)-1);
	free(gstr);	
    }

    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "n_output_channels", &error);
    if (error == NULL)
        n_output_channels = tmp;

    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "n_input_channels", &error);
    if (error == NULL)
        n_input_channels = tmp;

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
    
    error = NULL;
    tmp = g_key_file_get_integer(file, "global", "overrun_threshold", &error);
    if (error == NULL)
        overrun_threshold = tmp;
#endif
    g_key_file_free(file);
    g_free((void *) settingspath);
    return;
}

void
save_settings() {
    const gchar    *settingspath;
    GKeyFile       *file;
    gchar          *key_file_as_str;
    gsize           length;
    int             w_length;
    int             fd;

    settingspath = discover_settings_path();
    file = g_key_file_new();

    if (audio_driver)
        g_key_file_set_string(file, "global", "driver", audio_driver->str);
    
    g_key_file_set_string(file, "global", "alsadevice", alsadevice_str);
    g_key_file_set_integer(file, "global", "n_output_channels", n_output_channels);
    g_key_file_set_integer(file, "global", "n_input_channels", n_input_channels);
    g_key_file_set_integer(file, "global", "sample_rate", sample_rate);
    g_key_file_set_integer(file, "global", "buffer_size", buffer_size);
#ifdef _WIN32
    g_key_file_set_integer(file, "global", "n_inout_buffers", nbuffers);
    g_key_file_set_integer(file, "global", "overrun_threshold", overrun_threshold);
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
    g_free((void *) settingspath);
    g_free((void *) key_file_as_str);
    return;
}

void
pump_start(int argc, char **argv)
{
    int             i,
                    j;

    effect_t *      (*create_f[10])();

    my_create_mutex(&effectlist_lock);
    init_sin_lookup_table();

    master_volume = 0.0;
    input_volume = 0.0;
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
	effects[n] = create_f[n]();
	effects[n]->proc_init(effects[n]);
	n++;
    }
}

void
pump_stop(void)
{
    int             i;

    if (write_track) {
        write_track = 0;
        tracker_done();
    }
    for (i = 0; i < n; i++)
	effects[i]->proc_done(effects[i]);
    n = 0;

    my_close_mutex(effectlist_lock);
}

void
save_pump(const char *fname)
{
    int             i, fd, w_length;
    gsize           length;
    gchar	   *gtmp, *key_file_as_str, *effect_name;
    GKeyFile	   *preset;

    if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE)) < 0) {
        perror("Save failed: ");
        return;
    }

    preset = g_key_file_new();

    /* record software version */
    g_key_file_set_string(preset, "global", "version", version);

    my_lock_mutex(effectlist_lock);
    g_key_file_set_integer(preset, "global", "effects", n);
    for (i = 0; i < n; i++) {
	gtmp = g_strdup_printf("effect%d", i+1);
        gtk_clist_get_text(GTK_CLIST(processor), i, 0, &effect_name);
	g_key_file_set_string(preset, "global", gtmp, effect_name);
	/* save enabled flag */
	g_key_file_set_integer(preset, gtmp, "enabled", effects[i]->toggle);
	/* save effect-specific settings */
	if (effects[i]->proc_save != NULL)
	    effects[i]->proc_save(effects[i], preset, gtmp);
	free(gtmp);
    }
    my_unlock_mutex(effectlist_lock);

    g_key_file_set_double(preset, "global", "volume", master_volume);
    g_key_file_set_double(preset, "global", "input_volume", input_volume);

    key_file_as_str = g_key_file_to_data(preset, &length, NULL);
    w_length = write(fd, key_file_as_str, length);
    if (length != w_length)
	perror("Failed to write settings file completely: ");

    g_key_file_free(preset);

    close(fd);
}

void
load_pump(const char *fname)
{
    int		    i, j, n_effects, tmp_n;
    GKeyFile	   *preset;
    GError	   *error = NULL;
    gchar	   *gtmp, *effect_name;

    preset = g_key_file_new();
    g_key_file_load_from_file(preset, fname, G_KEY_FILE_NONE, NULL);
    gtmp = g_key_file_get_string(preset, "global", "version", &error);
    if (error != NULL) {
	gnuitar_printf( "error: failed to read file version.\n");
	g_key_file_free(preset);
	return;
    }

    /* we should adapt the keyfile between version changes */
    if (strncmp(gtmp, version, 13) != 0) {
	gnuitar_printf( "warning: version number mismatch: %s vs. %s\n", version, gtmp);
    }
    free(gtmp);

    n_effects = g_key_file_get_integer(preset, "global", "effects", &error);
    if (error != NULL) {
	gnuitar_printf( "error: failed to read effect count.\n");
	g_key_file_free(preset);
	return;
    }

    /* we do not take effectlist lock. I have carefully built this code to not need it. */

    gtk_clist_clear(GTK_CLIST(processor));

    /* this lock is only taken for a split second in order to ensure that n=0
     * is seen by audio thread. Otherwise we might destroy memory used by the thread. */
    my_lock_mutex(effectlist_lock);
    tmp_n = n;
    n = 0;
    my_unlock_mutex(effectlist_lock);
    for (i = 0; i < tmp_n; i++)
	effects[i]->proc_done(effects[i]);

    for (i = 0; i < n_effects; i += 1) {
	gtmp = g_strdup_printf("effect%d", i+1);
	effect_name = g_key_file_get_string(preset, "global", gtmp, &error);
	if (error != NULL) {
	    gnuitar_printf( "warning: effect tag '%s' not found\n", gtmp);
	    error = NULL;
	    free(gtmp);
	    continue;
	}
	for (j = 0; effect_list[j].str != NULL; j += 1) {
	    if (strcmp(effect_list[j].str, effect_name) == 0)
		break;
	}
	if (effect_list[j].str == NULL) {
	    gnuitar_printf( "warning: no effect called '%s'\n", effect_name);
	    free(effect_name);
	    free(gtmp);
	    continue;
	}

	effects[n] = effect_list[j].create_f();

	/* read enabled flag */
	effects[n]->toggle = g_key_file_get_integer(preset, gtmp, "enabled", &error);
	if (error != NULL) {
	    gnuitar_printf( "warning: no state flag in '%s'\n", effect_name);
	    error = NULL;
	    free(effect_name);
	    free(gtmp);
	    continue;
	}

	/* load effect specific settings */
	if (effects[n]->proc_load != NULL)
	    effects[n]->proc_load(effects[n], preset, gtmp, &error);

	effects[n]->proc_init(effects[n]);
	gtk_clist_append(GTK_CLIST(processor), &effect_list[j].str);
	n += 1;

	free(effect_name);
	free(gtmp);
    }

    master_volume = g_key_file_get_double(preset, "global", "volume", &error);
    if (error != NULL) {
	master_volume = 0.0;
	error = NULL;
    }
    
    input_volume = g_key_file_get_double(preset, "global", "input_volume", &error);
    if (error != NULL) {
        input_volume = 0.0;
        error = NULL;
    }
        
    g_key_file_free(preset);
    
    /* update master volume */
    gtk_adjustment_set_value(GTK_ADJUSTMENT(adj_master), master_volume);
    /* update input volume */
    gtk_adjustment_set_value(GTK_ADJUSTMENT(adj_input), input_volume);
}
