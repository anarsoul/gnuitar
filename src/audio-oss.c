/*
 * GNUitar
 * OSS parts
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
 * Revision 1.35  2006/10/27 22:02:39  alankila
 * - remove support for pitch bend for now
 *
 * Revision 1.34  2006/10/27 21:54:46  alankila
 * - new source file: audio-midi.c. Do some data abstraction, prepare to
 *   support multiple midi continuous controls.
 *
 * Revision 1.33  2006/10/27 16:23:53  alankila
 * - do not leak filehandles on midi errors
 * - make the midi code more likely to work the same way as ALSA/JACK code.
 *   I'm unable to cause OSS to make midi events, though. :-(
 *
 * Revision 1.32  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.31  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.30  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.29  2006/08/03 05:20:02  alankila
 * - don't crash on missing midi device
 * - alsa: keep on going if fragment number can't be set
 *
 * Revision 1.28  2006/07/28 20:18:04  alankila
 * - disable midi on first sniff of problems
 *
 * Revision 1.27  2006/07/28 20:11:51  alankila
 * - commit unfortunately untested MIDI code for OSS
 *
 * Revision 1.26  2006/07/27 19:24:41  alankila
 * - aligned memory needs aligned free operation.
 *
 * Revision 1.25  2006/07/26 23:09:09  alankila
 * - DirectSound may be buggy; MMSystem at least worked in mingw build.
 * - remove some sound-specific special cases in gui and main code.
 * - create thread in windows driver.
 * - remove all traces of "program states" variable.
 * - remove snd_open mutex: it is now unnecessary. Concurrency is handled
 *   through joining/waiting threads where necessary. (We assume JACK
 *   does its own locking, though.)
 *
 * Revision 1.24  2006/07/26 00:04:27  alankila
 * - move state off from main codebase into windows
 * - start audio thread directly in code
 *
 * Revision 1.23  2006/07/23 20:46:05  alankila
 * - it is illegal to write to GUI from audio thread.
 *
 * Revision 1.22  2006/07/17 21:39:38  alankila
 * - use dynamically allocated sample buffers instead of static ones.
 *   (Win32 still uses static buffers moved directly into audio-windows.c)
 *
 * Revision 1.21  2006/07/15 23:02:45  alankila
 * - remove the bits control -- just use the best available on every driver.
 *
 * Revision 1.20  2006/07/15 21:15:47  alankila
 * - implement triangular dithering on the sound drivers. Triangular dithering
 *   places more noise at the nyquist frequency so the noise floor is made
 *   smaller elsewhere.
 *
 * Revision 1.19  2006/07/14 14:19:50  alankila
 * - gui: OSS now supports 1-in 2-out mode.
 * - alsa: try to use recorded settings values before adapting attempts
 * - alsa: log adapt attempts and results
 *
 * Revision 1.18  2006/06/20 20:41:05  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.17  2006/05/20 09:56:58  alankila
 * - move audio_driver_str and audio_driver_enabled into driver structure
 * - Win32 drivers are ugly, with the need to differentiate between
 *   DirectX and MMSystem operation through dsound variable. The driver
 *   should probably be split with dsound-specific parts in its own driver.
 *
 * Revision 1.16  2006/05/20 07:59:39  alankila
 * - patch OSS to use the 16-bit version of the buffer.
 * - make OSS "buffer size" to be the size of one fragment, as with ALSA
 *
 * Revision 1.15  2005/11/05 12:18:38  alankila
 * - pepper the code with static declarations for all private funcs and vars
 *
 * Revision 1.14  2005/10/01 07:59:04  fonin
 * Fixed driver_bits_cfg[] arrays (missing trailing zero)
 *
 * Revision 1.13  2005/09/04 19:30:46  fonin
 * Added casts for DSP_FLOAT
 *
 * Revision 1.12  2005/09/04 16:06:59  alankila
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
 * Revision 1.6  2005/08/28 14:04:04  alankila
 * - OSS copypaste error fix
 * - remove my_log2 in favour of doing pow, trunc, log.
 * - OSS driver rounds buffer sizes to suitable values by itself now. There's
 *   a precedent in tuning user parameters automatically in ALSA code. The
 *   new behaviour rounds buffer size down, though.
 *
 * Revision 1.5  2005/08/28 12:28:44  alankila
 * switch to GMutex that is also available on win32
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

#ifdef HAVE_OSS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
 
#include "audio-oss.h"
#include "audio-midi.h"
#include "main.h"
#include "gui.h"
#include "utils.h"
#include "pump.h"

static SAMPLE16 *rwbuf = NULL;
static int fd = 0, midi_fd = 0;

static volatile int keepthreadrunning = 0;
static pthread_t audio_thread = 0;

/* XXX this code has never been properly tested.
 * I don't know how to cause midi events to occur on OSS.
 * If you know how and can test this on OS X or something such, please do. */
static void
oss_midi_event(void)
{
    int current = 0;
    int maxevents = 0;
    int pollstatus = 0;
    char midi_events[256];
    struct pollfd midi_poll = { .fd = midi_fd, .events = POLLIN, .revents = 0 };

    pollstatus = poll(&midi_poll, 1, 0);
    if (pollstatus == 0)
        return;
    if (pollstatus == -1) {
        fprintf(stderr, "error polling for midi events: %s -- disabling midi\n", strerror(errno));
        close(midi_fd);
        midi_fd = 0;
        return;
    }

    maxevents = read(midi_fd, midi_events, sizeof(midi_events));
    if (maxevents == -1) {
        fprintf(stderr, "error reading midi events: %s -- disabling midi\n", strerror(errno));
        close(midi_fd);
        midi_fd = 0;
        return;
    }

    /* the event tests are probably on. We want to support FC-200 and maybe
     * others. FC-200 sends CONTROL events (range 0 .. 127) when variable
     * pedal is used. It sends PROGRAM CHANGE events (range 0 .. x) when
     * other buttons are pressed. */
    while (current < maxevents) {
        /* if overflow, seek until beginning of new midi event */
        if (! midi_events[current] & 0x80) {
            current += 1;
            continue;
        }

        /* this debug is there to help until I've had chance
         * to ensure this actually works. */
        fprintf(stderr, "oss midi event: (%x, %x, %x) [%d/%d]\n",
            midi_events[current],
            midi_events[current+1],
            midi_events[current+2],
            current, maxevents);

        /* patch change: (instrument #, reserved) */
        if ((midi_events[current] & 0x70) == 0x50) {
            if (current + 2 > maxevents) {
                fprintf(stderr, "partial read of midi pc -- ignored.\n");
                break;
            }
            midi_set_program(midi_events[current + 1]);
            current += 3;
            continue;
        }
        /* continuous controller: (controller #, controller value) */
        if ((midi_events[current] & 0x70) == 0x40) {
            if (current + 2 > maxevents) {
                fprintf(stderr, "partial read of midi cc -- ignored.\n");
                break;
            }
            midi_set_continuous_control(midi_events[current + 1], midi_events[current + 2] / 127.f);
            current += 3;
            continue;
        }
        /* ignore message */
    }
}

static void *
oss_audio_thread(void *V)
{
    int             count, i;
    data_block_t db = {
        .data = procbuf,
        .data_swap = procbuf2,
        .len = buffer_size * 2 * n_output_channels
    };
    fd_set read_fds;
    struct timeval read_timeout;

    while (keepthreadrunning) {
	/* keep on reading and discard if select says we can read.
         * this will allow us to catch up if we fall behind */
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
	read_timeout.tv_sec  = 0;
	read_timeout.tv_usec = 0;
        do {
	    count = read(fd, rwbuf, buffer_size * n_output_channels * 2);
            if (count < 0) {
                perror("error reading from sound device: ");
                break;
            } else if (count != (int) buffer_size * n_output_channels * 2) {
                //gnuitar_printf( "warning: short read (%d/%d) from sound device\n", count, buffer_size);
                break;
	    }
	} while (select(fd+1, &read_fds, NULL, NULL, &read_timeout) != 0);
	
        db.len = buffer_size * n_input_channels;
        db.channels = n_input_channels;

        if (n_input_channels == n_output_channels) {
            for (i = 0; i < db.len; i++)
                db.data[i] = rwbuf[i] << 8;
        } else {
            /* 1 in, 2 out -- discard the right channel */
            for (i = 0; i < db.len; i ++)
                db.data[i] = rwbuf[i * 2] << 8;
        }

        if (midi_fd)
            oss_midi_event();
	pump_sample(&db);

        /* Ensure that pump adapted us to output */
        assert(db.channels == n_output_channels);
        assert(db.len == buffer_size * n_output_channels);
        triangular_dither(&db, rwbuf);

	count = write(fd, rwbuf, buffer_size * n_output_channels * 2);
        /*
	if (count != buffer_size * n_output_channels * 2)
	    gnuitar_printf( "warning: short write (%d/%d) to sound device\n", count, buffer_size);*/
    }

    return NULL;
}

static void
oss_finish_sound(void)
{
    keepthreadrunning = 0;
    pthread_join(audio_thread, NULL);
    gnuitar_free(rwbuf);
    oss_driver.enabled = 0;
    close(midi_fd);
    midi_fd = 0;
    close(fd);
}

static int
oss_init_sound(void)
{
    int             i;
    
    if ((fd = open("/dev/dsp", O_RDWR)) == -1) {
	gnuitar_printf("Cannot open /dev/dsp: %s\n", strerror(errno));
	return ERR_WAVEINOPEN;
    }

    /* force buffer size to 2^N */
    buffer_size = pow(2, (int) (log(buffer_size) / log(2)));

    /*
     * unlimited fragments, fragment size is equal to the buffer size,
     * so we operate with one full fragment at a time
     * The value is encoded like this:
     * 0xMMMMSSSS, where
     *   ^^^^      == number of fragments, 7fff = unlimited
     *       ^^^^  == 2-base logarithm of a fragment size,
     *                e.g. it is 8 for 256-byte fragment,
     *                           9 for 512
     *                           etc.
     */
    i = 0x7fff0000 + (int) (log(buffer_size * 2 * n_output_channels) / log(2));
    if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &i) < 0) {
	gnuitar_printf("Cannot setup fragments: %s\n", strerror(errno));
	close(fd);
	return ERR_WAVEFRAGMENT;
    }

    if (ioctl(fd, SNDCTL_DSP_GETCAPS, &i) == -1) {
	gnuitar_printf("Cannot get soundcard capabilities: %s\n", strerror(errno));
	close(fd);
	return ERR_WAVEGETCAPS;
    }

    if (!(i & DSP_CAP_DUPLEX)) {
	gnuitar_printf("It seems your soundcard is not full duplex capable. (Try ALSA driver before giving up hope.) Error from OSS: %s\n", strerror(errno));
	close(fd);
	exit(ERR_WAVENOTDUPLEX);
    }

    if (ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1) {
	gnuitar_printf("Cannot setup full-duplex audio: %s\n", strerror(errno));
	close(fd);
	return ERR_WAVEDUPLEX;
    }

    /* 16-bit recording is the best available with OSS */
    i = AFMT_S16_NE;
    if (ioctl(fd, SNDCTL_DSP_SETFMT, &i) == -1) {
	gnuitar_printf("Cannot setup 16-bit native-endian audio: %s\n", strerror(errno));
	close(fd);
	return ERR_WAVESETBIT;
    }

    i = n_output_channels;
    if (ioctl(fd, SNDCTL_DSP_CHANNELS, &i) == -1) {
	gnuitar_printf("Cannot setup %d-channel audio: %s\n", i, strerror(errno));
	close(fd);
	return ERR_WAVESETCHANNELS;
    }

    i = sample_rate;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &i) == -1) {
	gnuitar_printf("Cannot setup sampling frequency %d Hz: %s\n", i, strerror(errno));
	close(fd);
	return ERR_WAVESETRATE;
    }

    /* allocate sufficient memory for 16-bit sample buffer depending on
     *  which has more output channels. */
    if (n_input_channels > n_output_channels)
        rwbuf = gnuitar_memalign(n_input_channels * buffer_size, 16 >> 3);
    else
        rwbuf = gnuitar_memalign(n_output_channels * buffer_size, 16 >> 3);

    /* create the audio thread */
    keepthreadrunning = 1;
    if (pthread_create(&audio_thread, NULL, oss_audio_thread, NULL)) {
        gnuitar_printf("Audio thread creation failed: %s\n", strerror(errno));
        return ERR_THREAD;
    }

    if ((midi_fd = open("/dev/midi", O_RDONLY)) == -1) {
	gnuitar_printf("Cannot open /dev/midi: %s -- continuing without midi.",
                        strerror(errno));
        midi_fd = 0;
    }

    oss_driver.enabled = 1;
    return ERR_NOERROR;
}

/* try to open /dev/dsp. Is there a better test to know? */
int
oss_available() {
    if ((fd = open("/dev/dsp", O_RDWR | O_NONBLOCK)) == -1)
	return 0;
    close(fd);
    return 1;
}

static const struct audio_driver_channels oss_channels_cfg[] = {
    { 1, 1 },
    { 1, 2 },
    { 2, 2 },
    { 0, 0 }
};

audio_driver_t oss_driver = {
    .str = "OSS",
    .enabled = 0,
    .channels = oss_channels_cfg,

    .init = oss_init_sound,
    .finish = oss_finish_sound,
};

#endif /* HAVE_OSS */
