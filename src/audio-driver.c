/*
 * Audio driver interface to reduce file interdependencies and give home to
 * various state variables. (c) 2006 Antti S. Lankila. Licensed under GPL.
 *
 * $Id$
 *
 * $Log$
 * Revision 1.3  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.2  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.1  2006/08/07 12:55:30  alankila
 * - construct audio-driver.c to hold globals and provide some utility
 *   functions to its users. This slashes interdependencies somewhat.
 *
 */


#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "audio-driver.h"
#include "audio-alsa.h"
#include "audio-oss.h"
#include "audio-jack.h"
#include "audio-winmm.h"
#include "audio-dsound.h"

#ifndef _WIN32
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE * MAX_CHANNELS];
DSP_SAMPLE      procbuf2[MAX_BUFFER_SIZE * MAX_CHANNELS];
#else
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
DSP_SAMPLE      procbuf2[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
#endif

audio_driver_t  *audio_driver = NULL;

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

/* from JACK -- blindingly fast */
static inline unsigned int
prng(void)
{
    static unsigned int seed = 22222;
    seed = (seed * 96314165) + 907633515;
    return seed;
}

/* This is triangular correlated noise with frequency spectrum that increases
 * 6 dB per octave, thus most noise occurs at high frequencies. The probability
 * distribution still looks like triangle. Idea and implementation borrowed from
 * JACK. */
void
triangular_dither(data_block_t *db, SAMPLE16 *target)
{
    static SAMPLE32 correlated_noise[MAX_CHANNELS] = { 0, 0, 0, 0 };
    int_fast16_t i, current_channel = 0;
    
    for (i = 0; i < db->len; i += 1) {
        SAMPLE32 tmp = db->data[i];
        SAMPLE32 noise = (prng() & 0x1ff) - 256; /* -256 to 255 */
        
        tmp += noise - correlated_noise[current_channel];
        correlated_noise[current_channel] = noise;
        tmp >>= 8;
        if (tmp > 32767)
            tmp = 32767;
        if (tmp < -32768)
            tmp = -32768;
        target[i] = tmp;
        current_channel = (current_channel + 1) % db->channels;
    }
}

void
guess_audio_driver(void)
{
    audio_driver = NULL;
#ifdef HAVE_JACK
    if (jack_available()) {
        audio_driver = &jack_driver;
    } else
#endif
#ifdef HAVE_ALSA
    if (alsa_available()) {
        audio_driver = &alsa_driver;
    } else
#endif
#ifdef HAVE_OSS
    if (oss_available()) {
        audio_driver = &oss_driver;
    } else
#endif
#ifdef HAVE_DSOUND
    if (audio_driver == NULL) {
	audio_driver = &dsound_driver;
    } else
#endif
#ifdef HAVE_WINMM
    if (audio_driver == NULL) {
	audio_driver = &winmm_driver;
    } else
#endif
        return;
}

void
set_audio_driver_from_str(const char const *tmp)
{
#ifdef HAVE_JACK
    if (strcmp(tmp, "JACK") == 0) {
        audio_driver = &jack_driver;
    } else
#endif
#ifdef HAVE_ALSA
    if (strcmp(tmp, "ALSA") == 0) {
        audio_driver = &alsa_driver;
    } else
#endif
#ifdef HAVE_OSS
    if (strcmp(tmp, "OSS") == 0) {
        audio_driver = &oss_driver;
    } else
#endif
#ifdef HAVE_DSOUND
    if(strcmp(tmp, "DirectX") == 0) {
        audio_driver = &dsound_driver;
    } else
#endif
#ifdef HAVE_WINMM
    if(strcmp(tmp, "MMSystem") == 0) {
        audio_driver = &winmm_driver;
        buffer_size = pow(2, (int) (log(buffer_size) / log(2)));
    } else
#endif
        return;
}
