/*
 * Audio driver interface to reduce file interdependencies and give home to
 * various state variables. (c) 2006 Antti S. Lankila. Licensed under GPL.
 *
 * $Id$
 *
 * $Log$
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
        ; /* nothing */
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
        ; /* nothing */
}
