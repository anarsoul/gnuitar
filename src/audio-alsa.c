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
 * Revision 1.28  2006/05/20 09:56:58  alankila
 * - move audio_driver_str and audio_driver_enabled into driver structure
 * - Win32 drivers are ugly, with the need to differentiate between
 *   DirectX and MMSystem operation through dsound variable. The driver
 *   should probably be split with dsound-specific parts in its own driver.
 *
 * Revision 1.27  2006/05/19 15:12:54  alankila
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
 * Revision 1.26  2006/05/19 12:19:29  alankila
 * - write output buffer as soon as input buffer is read -- this hopefully
 *   avoids the rattle effect I've been getting lately. :-/
 *
 * Revision 1.25  2006/05/15 19:32:23  alankila
 * - improve error handling that hopefully will help users to decipher errors
 *   from gnuitar
 * - specifying surround40 ought to enforce number of output channels to 4.
 *   several other such rules should be written in gui.c to protect the user.
 *
 * Revision 1.24  2006/05/01 10:23:54  anarsoul
 * Alsa device is selectable and input volume is adjustable now. Added new filter - amp.
 *
 * Revision 1.23  2006/02/16 19:23:43  alankila
 * - fix a useless use of comparison to what was probably meant
 *
 * Revision 1.22  2006/02/07 13:30:57  fonin
 * Fixes to ALSA driver by Vasily Khoruzhick
 *
 * Revision 1.21  2005/12/03 20:01:05  alankila
 * - fix snafu, the device I meant was default, not default:0,0 :-(
 *
 * Revision 1.20  2005/12/02 11:37:33  alankila
 * - let's not hardcode plughw:0,0, it's better to use default:0,0
 *
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

/* these parameters affect our persistency in attempts to configure playback */
#define MAX_TRIES 8

// XXX: these should be made changeable in the UI
static const char     *snd_device_in       = "default";
static const char     *snd_test_device_out = "default";

static short   restarting;
static snd_pcm_t *playback_handle;
static snd_pcm_t *capture_handle;

static void           *
alsa_audio_thread(void *V)
{
    int             i, inframes, outframes;
    struct data_block db = {
        .data = procbuf,
        .data_swap = procbuf2,
        .len = buffer_size * n_output_channels
    };
    SAMPLE16    *rdbuf16 = (SAMPLE16 *) rdbuf;
    SAMPLE32    *rdbuf32 = (SAMPLE32 *) rdbuf;
    SAMPLE16    *wrbuf16 = (SAMPLE16 *) wrbuf;
    SAMPLE32    *wrbuf32 = (SAMPLE32 *) wrbuf;
    
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

            //fprintf(stderr, "Resyncing input/output\n");
            /* Prefill 2 playback buffer fragments. Normally this is the
             * maximum amount of fragments, and it ensures there's something
             * to play while we come up with more data to play. */
            for (i = 0; i < buffer_size * n_output_channels; i += 1)
                wrbuf[i] = 0;
            for (i = 0; i < fragments; i += 1)
                snd_pcm_writei(playback_handle, wrbuf, buffer_size);
        }

        while ((inframes = snd_pcm_readi(capture_handle, rdbuf, buffer_size)) < 0) {
            //fprintf(stderr, "Input buffer overrun\n");
            restarting = 1;
            snd_pcm_prepare(capture_handle);
        }
        if (inframes != buffer_size)
            fprintf(stderr, "Short read from capture device: %d, expecting %d\n", inframes, buffer_size);
        
        /* prepare output */
	if (bits == 32)
	    for (i = 0; i < db.len; i++)
		wrbuf32[i] = (SAMPLE32) db.data[i] << 8;
	else
	    for (i = 0; i < db.len; i++)
		wrbuf16[i] = (SAMPLE32) db.data[i] >> 8;

        /* write output */
        while ((outframes = snd_pcm_writei(playback_handle, wrbuf, buffer_size)) < 0) {
            //fprintf(stderr, "Output buffer underrun\n");
            restarting = 1;
            snd_pcm_prepare(playback_handle);
        }
        if (outframes != buffer_size)
            fprintf(stderr, "Short write to playback device: %d, expecting %d\n", outframes, buffer_size);

        /* now that output is out of the way, we have most time for running effects */ 
        db.len = buffer_size * n_input_channels;
        db.channels = n_input_channels;
	if (bits == 32)
	    for (i = 0; i < db.len; i++)
		db.data[i] = rdbuf32[i] >> 8;
	else
	    for (i = 0; i < db.len; i++)
		db.data[i] = rdbuf16[i] << 8;
	pump_sample(&db);
        
        /* adapting must have worked, and effects must not have changed
         * frame counts somehow */
        assert(db.channels == n_output_channels);
        assert(db.len / n_output_channels == buffer_size);

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
    alsa_driver.enabled = 0;
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
alsa_configure_audio(snd_pcm_t *device, unsigned int *fragments, unsigned int *frames, int channels, int adapting)
{
    /*fprintf(stderr,"So we want to have:\n");
    fprintf(stderr,"Fragments: %d\n", *fragments);
    fprintf(stderr,"Frames: %d\n", *frames);
    fprintf(stderr,"Channels: %d\n", channels);*/
    snd_pcm_hw_params_t *hw_params;
    int                 err;
    unsigned int        tmp;
    
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
        fprintf (stderr, "can't set access type RW_INTERLEAVE (try using plughw): %s\n",
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
        fprintf (stderr, "can't set sample format %d bits: %s\n", bits,
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }

    tmp = sample_rate;    
    if ((err = adapting
            ? snd_pcm_hw_params_set_rate_near(device, hw_params, &tmp, 0)
            : snd_pcm_hw_params_set_rate(device, hw_params, tmp, 0)) < 0) {
        fprintf (stderr, "can't set sample rate %d: %s\n", tmp,
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }
    if (tmp != sample_rate) {
        fprintf(stderr, "can't set requested sample rate, asked for %d got %d\n", sample_rate, tmp);
        sample_rate = tmp;
    }

    if ((err = snd_pcm_hw_params_set_channels(device, hw_params, channels)) < 0) {
        fprintf (stderr, "can't set channel count %d: %s\n", channels,
                 snd_strerror(err));
        snd_pcm_hw_params_free(hw_params);
	return 1;
    }

    if (adapting) {
        /* let the sound driver pick something that we agree to be happy with */
        snd_pcm_uframes_t alsa_frames;
        tmp = (float) (*frames * *fragments) / sample_rate * 1E6;

        if ((err = snd_pcm_hw_params_set_buffer_time_near(device, hw_params, &tmp, NULL)) < 0) {
            fprintf(stderr, "can't set buffer time near %d: %s\n", tmp,
                    snd_strerror(err));
            snd_pcm_hw_params_free(hw_params);
            return 1;
        }
        /* obtain the frames and fragments chosen by ALSA */
        if ((err = snd_pcm_hw_params_get_period_size(hw_params, &alsa_frames, NULL)) < 0) {
	    fprintf(stderr, "can't get period size value from ALSA (read: %d): %s\n",
                    (int) alsa_frames, snd_strerror(err));
            snd_pcm_hw_params_free(hw_params);
            return 1;
        }
        *frames = alsa_frames;
        if ((err = snd_pcm_hw_params_get_periods(hw_params, fragments, NULL)) < 0) {
	    fprintf(stderr, "can't get fragments value from ALSA (read: %d): %s\n",
                    *fragments, snd_strerror(err));
            snd_pcm_hw_params_free(hw_params);
            return 1;
        }
    } else {
        if ((err = snd_pcm_hw_params_set_period_size(device, hw_params, *frames, 0)) < 0) {
            fprintf(stderr, "can't set period size to %d frames: %s\n",
                    (int) *frames, snd_strerror(err));
            snd_pcm_hw_params_free(hw_params);
            return 1;
        }
        
        if ((err = snd_pcm_hw_params_set_periods(device, hw_params, *fragments, 0)) < 0) {
            fprintf(stderr, "can't set periods to %d: %s\n", *fragments,
                    snd_strerror(err));

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
    //printf("pass!\n");
    snd_pcm_hw_params_free(hw_params);
    return 0;
}

/*
 * sound initialization
 */
static int
alsa_init_sound(void)
{
    int             err;
    unsigned int    frames, fragments_try, tries;

    if ((err = snd_pcm_open(&playback_handle, alsadevice_str, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
	fprintf(stderr, "can't open output audio device %s: %s\n", alsadevice_str, snd_strerror(err));
	state = STATE_EXIT;
	return ERR_WAVEOUTOPEN;
    }
    if ((err = snd_pcm_open(&capture_handle, snd_device_in, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
	fprintf(stderr, "can't open input audio device %s: %s\n", snd_device_in, snd_strerror(err));
        snd_pcm_close(playback_handle);
	state = STATE_EXIT;
	return ERR_WAVEINOPEN;
    }

    tries = 0;
    fragments = 2;
    while (tries < MAX_TRIES) {
	/* since the parameters could take a different form depending on which is
         * configured first, try configuring both ways before incrementing
         * fragments */ 
	frames = buffer_size;
	fragments_try = fragments;
        
	if (
(alsa_configure_audio(capture_handle, &fragments, &frames, n_input_channels, 1)
|| alsa_configure_audio(playback_handle, &fragments, &frames, n_output_channels, 0))
        ) {
	    /* no go; try the other way */
            fragments_try = fragments;
            frames = buffer_size;
            
            if (
(alsa_configure_audio(playback_handle, &fragments_try, &frames, n_output_channels, 1)
|| alsa_configure_audio(capture_handle, &fragments_try, &frames, n_input_channels, 0))
            ) {
                /* no go -- try with more fragments */
		fragments += 1;
                /* XXX should we try perturbing buffer size too? */
            } else {
                break;
            }
	} else {
            break;
        }
        tries++;
    }
    /* if reached max we failed to find anything workable */
    if (tries == MAX_TRIES) {
        snd_pcm_close(playback_handle);
        snd_pcm_close(capture_handle);
	return ERR_WAVEFRAGMENT;
    }
    
    buffer_size = frames;
    restarting = 1;
    
    state = STATE_PROCESS;
    alsa_driver.enabled = 1;
    my_unlock_mutex(snd_open);
    return ERR_NOERROR;
}

int
alsa_available() {
    if (snd_pcm_open(&playback_handle, snd_test_device_out, SND_PCM_STREAM_PLAYBACK, 0) < 0)
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
    .str = "ALSA",
    .enabled = 0,
    .channels = alsa_channels_cfg,
    .bits = alsa_bits_cfg,
    
    .init = alsa_init_sound,
    .finish = alsa_finish_sound,
    .thread = alsa_audio_thread
};

#endif /* HAVE_ALSA */
