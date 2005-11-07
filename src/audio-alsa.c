/*
 * GNUitar
 * Alsa bits
 * Copyright (C) 2005 Antti Lankila  <alankila@bel.fi>
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
 * Revision 1.19  2005/11/07 20:33:42  alankila
 * - comment update
 *
 * Revision 1.18  2005/11/07 20:26:40  alankila
 * - I decided to start trusting snd_pcm_hw_params_set_buffer_size_near.
 *
 * Revision 1.17  2005/11/05 12:18:38  alankila
 * - pepper the code with static declarations for all private funcs and vars
 *
 * Revision 1.16  2005/10/01 07:59:04  fonin
 * Fixed driver_bits_cfg[] arrays (missing trailing zero)
 *
 * Revision 1.15  2005/09/06 23:25:38  alankila
 * - remove overrun/underrun indicators in order to not potentially retrigger
 *   an overrun/underrun with text scrolling in terminal
 *
 * Revision 1.14  2005/09/05 17:42:07  alankila
 * - fix some small memory leaks
 *
 * Revision 1.13  2005/09/04 19:30:46  fonin
 * Added casts for DSP_FLOAT
 *
 * Revision 1.12  2005/09/04 16:06:58  alankila
 * - first multichannel effect: delay
 * - need to use surround40 driver in alsa
 * - introduce new buffer data_swap so that effects need not reserve buffers
 * - correct off-by-one error in multichannel adapting
 *
 * Revision 1.11  2005/09/03 23:31:40  alankila
 * - add signs
 *
 * Revision 1.10  2005/09/03 22:13:56  alankila
 * - make multichannel processing selectable
 * - new GUI (it sucks as much as the old one and I'll need to grok GTK
 *   tables first before it gets better)
 * - make pump.c do the multichannel adapting bits
 * - effects can now change channel counts
 *
 * Revision 1.9  2005/09/03 20:20:42  alankila
 * - create audio_driver type and write all the driver stuff into it. This
 *   faciliates carrying configuration data about the capabilities of
 *   a specific audio driver and uses less global variables.
 *
 * Revision 1.8  2005/09/01 19:07:31  alankila
 * - ask audio in host byte order
 *
 * Revision 1.7  2005/08/28 21:41:28  fonin
 * Portability: introduced new functions for mutexes
 *
 * Revision 1.6  2005/08/28 12:28:44  alankila
 * switch to GMutex that is also available on win32
 *
 * Revision 1.5  2005/08/27 19:05:43  alankila
 * - introduce SAMPLE16 and SAMPLE32 types, and switch
 *
 * Revision 1.4  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.3  2005/08/26 15:59:56  fonin
 * Audio driver now can be chosen by user
 *
 * Revision 1.2  2005/08/24 22:33:02  alankila
 * - avoid reopening sound device at exit in order to cleanly shut it down the
 *   next moment
 *
 * Revision 1.1  2005/08/24 21:44:44  alankila
 * - split sound drivers off main.c
 * - add support for alsa
 * - rework thread locking
 * - in this version, sound drivers are chosen at compile time
 * - windows driver is probably broken
 *
 *
 */

#ifdef HAVE_ALSA

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <assert.h>

#include "pump.h"
#include "main.h"

// XXX: these should be made changeable in the UI
const char     *snd_device_in      = "plughw:0,0";
const char     *snd_2ch_device_out = "plughw:0,0";
const char     *snd_4ch_device_out = "surround40:0,0";

static short   restarting;
static snd_pcm_t *playback_handle;
static snd_pcm_t *capture_handle;

static void           *
alsa_audio_thread(void *V)
{
    int             i, frames, inframes, outframes;
    struct data_block db; 
    SAMPLE16    *rdbuf16 = (SAMPLE16 *) rdbuf;
    SAMPLE32    *rdbuf32 = (SAMPLE32 *) rdbuf;
    
    /* frame counts are always the same to both read and write */
    while (state != STATE_EXIT && state != STATE_ATHREAD_RESTART) {
        while (state == STATE_PAUSE) {
            usleep(10000);
        }
        my_lock_mutex(snd_open);
        /* catch transition PAUSE -> EXIT with mutex being waited already */
        if (state == STATE_EXIT || state == STATE_ATHREAD_RESTART) {
            my_unlock_mutex(snd_open);
            break;
        }
        
        frames = snd_pcm_bytes_to_frames(capture_handle, buffer_size);
        /* ensure output buffer has data in it,
         * this fixes the rattly scratching I used to be getting
         * after a restart. This is probably a bit too paranoid, but
         * damn it, it works. */
        if (restarting) {
            restarting = 0;
            
            /* drop any output we might got and stop */
            snd_pcm_drop(capture_handle);
            snd_pcm_drop(playback_handle);
            /* prepare for use */
            snd_pcm_prepare(capture_handle);
            snd_pcm_prepare(playback_handle);
            
            /* fill 2 playback buffer fragments. Normally this is the
             * maximum amount of fragments, and it ensures there's something
             * to play while we come up with more data to play. */
            for (i = 0; i < frames * n_output_channels; i += 1)
                rdbuf[i] = 0;
            for (i = 0; i < fragments; i += 1)
                snd_pcm_writei(playback_handle, rdbuf, frames);
        }

        while ((inframes = snd_pcm_readi(capture_handle, rdbuf, frames)) < 0) {
            if (inframes == -EAGAIN)
                continue;
            //fprintf(stderr, "Input buffer overrun\n");
            restarting = 1;
            snd_pcm_prepare(capture_handle);
        }
        if (inframes != frames)
            fprintf(stderr, "Short read from capture device: %d, expecting %d\n", inframes, frames);
        db.len = inframes * n_input_channels;
        db.data = procbuf;
        db.data_swap = procbuf2;
        db.channels = n_input_channels;
	if (bits == 32)
	    for (i = 0; i < db.len; i++)
		db.data[i] = rdbuf32[i] >> 8;
	else
	    for (i = 0; i < db.len; i++)
		db.data[i] = rdbuf16[i] << 8;
	pump_sample(&db);
	if (bits == 32)
	    for (i = 0; i < db.len; i++)
		rdbuf32[i] = (SAMPLE32)db.data[i] << 8;
	else
	    for (i = 0; i < db.len; i++)
		rdbuf16[i] = (SAMPLE32)db.data[i] >>  8;

        /* adapting must have worked, and effects must not have changed
         * frame counts somehow */
        assert(db.channels == n_output_channels);
        assert(db.len / n_output_channels == inframes);
        
        while ((outframes = snd_pcm_writei(playback_handle, rdbuf, inframes)) < 0) {
            if (outframes == -EAGAIN)
                continue;
            //fprintf(stderr, "Output buffer underrun\n");
            restarting = 1;
            snd_pcm_prepare(playback_handle);
        }
        if (outframes != frames)
            fprintf(stderr, "Short write to playback device: %d, expecting %d\n", outframes, frames);
        
        my_unlock_mutex(snd_open);
    }
    return NULL;
}

/*
 * sound shutdown 
 */
static void
alsa_finish_sound(void)
{
    state = STATE_PAUSE;
    my_lock_mutex(snd_open);
    snd_pcm_drop(playback_handle);
    snd_pcm_close(playback_handle);
    snd_pcm_drop(capture_handle);
    snd_pcm_close(capture_handle);
}

/* The adapting flag allows the first invocation to change sampling parameters.
 * On the second call, adapting is disabled. This is done to force identical
 * parameters on the two devices, which may not even be same physical
 * hardware. */
static int
alsa_configure_audio(snd_pcm_t *device, unsigned int fragments, unsigned int *frames, int channels, int adapting)
{
    snd_pcm_hw_params_t *hw_params;
    int                 err;
    unsigned int        tmp;
    snd_pcm_uframes_t   frame_info;
    
    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        fprintf (stderr, "can't allocate parameter structure: %s\n",
                 snd_strerror(err));
	return 1;
    }
    
    if ((err = snd_pcm_hw_params_any(device, hw_params)) < 0) {
        fprintf (stderr, "can't initialize parameter structure: %s\n",
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }

    if ((err = snd_pcm_hw_params_set_access(device, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "can't set access type: %s\n",
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }

    if (bits == 32)
	tmp = SND_PCM_FORMAT_S32;
    else {
	tmp = SND_PCM_FORMAT_S16;
	bits = 16;
    }
    if ((err = snd_pcm_hw_params_set_format(device, hw_params, tmp)) < 0) {
        fprintf (stderr, "can't set sample format: %s\n",
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }

    tmp = sample_rate;    
    if ((err = adapting
            ? snd_pcm_hw_params_set_rate_near(device, hw_params, &tmp, 0)
            : snd_pcm_hw_params_set_rate(device, hw_params, tmp, 0)) < 0) {
        fprintf (stderr, "can't set sample rate: %s\n",
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }
    if (tmp != sample_rate) {
        fprintf(stderr, "can't set requested sample rate, asked for %d got %d\n", sample_rate, tmp);
        sample_rate = tmp;
    }

    if ((err = snd_pcm_hw_params_set_channels(device, hw_params, channels)) < 0) {
        fprintf (stderr, "can't set channel count: %s\n",
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }

    /* set_periods_near fails on at least one hardware. Perhaps count of
     * periods can't really be specified before the period size is known?
     * The code calling this part tries various fragment counts in trying to
     * come up with identical parameters for playback and capture.
     *
     * Perhaps I should simply try setting buffer_time_near as user-specified
     * latency; it'd be at least as likely to work, and much simpler. I'd
     * still need to read the device to know what parameters ALSA eventually
     * chose, and enforce that periods and period size are the same for
     * playback and capture.
     *
     * To do that needs UI change -- most drivers should no longer allow
     * choosing  buffer size but rather let user pick a latency and try to
     * get a value as close as possible.
     */
    if ((err = snd_pcm_hw_params_set_periods(device, hw_params, fragments, 0)) < 0) {
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }
    
    //fprintf(stderr, "trying with %d fragments\n", fragments);
    if (adapting) {
        frame_info = *frames;
        if ((err = snd_pcm_hw_params_set_buffer_size_near(device, hw_params, &frame_info)) < 0) {
            fprintf(stderr, "can't set buffer_size to %d frames: %s\n",
                    (int) *frames, snd_strerror(err));
            snd_pcm_hw_params_free(hw_params);
            return 1;
	}
        if (*frames != frame_info) {
            fprintf(stderr, "alsa adjusted requested buffer size %d to %d frames\n", *frames, (int) frame_info);
        }
        *frames = frame_info;
    } else {
        if ((err = snd_pcm_hw_params_set_buffer_size(device, hw_params, *frames)) < 0) {
            fprintf(stderr, "can't set buffer_size to %d frames: %s\n",
                    (int) *frames, snd_strerror(err));
            snd_pcm_hw_params_free(hw_params);
            return 1;
        }
    }

    if ((err = snd_pcm_hw_params(device, hw_params)) < 0) {
        fprintf(stderr, "Error setting HW params: %s\n",
                snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }
    snd_pcm_hw_params_free(hw_params);
    //printf("pass!\n");
    
    return 0;
}

/*
 * sound initialization
 */
static int
alsa_init_sound(void)
{
    int             err;
    unsigned int    frames;
    const char     *snd_device_out;

    snd_device_out = n_output_channels == 4 ? snd_4ch_device_out : snd_2ch_device_out;

    if ((err = snd_pcm_open(&playback_handle, snd_device_out, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
	fprintf(stderr, "can't open output audio device %s: %s\n", snd_device_out, snd_strerror(err));
	state = STATE_EXIT;
	return ERR_WAVEOUTOPEN;
    }
    if ((err = snd_pcm_open(&capture_handle, snd_device_in, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
	fprintf(stderr, "can't open input audio device %s: %s\n", snd_device_in, snd_strerror(err));
        snd_pcm_close(playback_handle);
	state = STATE_EXIT;
	return ERR_WAVEINOPEN;
    }

#define MAX_FRAGMENTS 8
    fragments = 2;
    /* buffer size really defines the input buffer's size. We convert it to
     * frames and ask same count of frames in both directions */
    frames = buffer_size / n_input_channels / (bits / 8) * fragments;
    while (fragments < MAX_FRAGMENTS) {
        frames = buffer_size / n_input_channels / (bits / 8) * fragments;
        /* since the parameters can take a different form depending on which is
         * configured first, try configuring both ways before incrementing
         * fragments */ 
	if (
(alsa_configure_audio(playback_handle, fragments, &frames, n_output_channels, 1)
|| alsa_configure_audio(capture_handle, fragments, &frames, n_input_channels, 0))
        ) {
            frames = buffer_size / n_input_channels / (bits / 8) * fragments;
            if (
(alsa_configure_audio(capture_handle, fragments, &frames, n_input_channels, 1)
|| alsa_configure_audio(playback_handle, fragments, &frames, n_output_channels, 0))
            ) {
                fragments += 1;
            } else {
                break;
            }
	} else {
            break;
        }
    }
    /* if reached max we failed to find anything workable */
    if (fragments == MAX_FRAGMENTS) {
        snd_pcm_close(playback_handle);
        snd_pcm_close(capture_handle);
	return ERR_WAVEFRAGMENT;
    }
    buffer_size = frames * n_input_channels * (bits / 8) / fragments;
    restarting = 1;
    
    state = STATE_PROCESS;
    my_unlock_mutex(snd_open);
    return ERR_NOERROR;
}

int
alsa_available() {
    if (snd_pcm_open(&playback_handle, snd_2ch_device_out, SND_PCM_STREAM_PLAYBACK, 0) < 0)
	return 0;
    snd_pcm_close(playback_handle);
    return 1;
}

static struct audio_driver_channels alsa_channels_cfg[] = {
    { 1, 1 },
    { 1, 2 },
    { 1, 4 },
    { 2, 2 },
    { 2, 4 },
    { 0, 0 }
};

static unsigned int alsa_bits_cfg[] = { 16, 32, 0 };

audio_driver_t alsa_driver = {
    alsa_init_sound,
    alsa_finish_sound,
    alsa_audio_thread,
    alsa_channels_cfg,
    alsa_bits_cfg
};

#endif /* HAVE_ALSA */
