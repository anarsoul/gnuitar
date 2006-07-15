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
#include <sys/soundcard.h>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>
#include <assert.h>
 
#include "pump.h"
#include "main.h"
#include "utils.h"

static int fd;

static void *
oss_audio_thread(void *V)
{
    int             count, i;
    struct data_block db = {
        .data = procbuf,
        .data_swap = procbuf2,
        .len = buffer_size * 2 * n_output_channels
    };
    fd_set read_fds;
    struct timeval read_timeout;
    SAMPLE16 *rdbuf16 = (SAMPLE16 *) rdbuf;
    SAMPLE16 *wrbuf16 = (SAMPLE16 *) wrbuf;

    while (state != STATE_EXIT && state != STATE_ATHREAD_RESTART) {
	if (state == STATE_PAUSE) {
	    usleep(10000);
	}
        my_lock_mutex(snd_open);
        /* catch transition PAUSE -> EXIT with mutex being waited already */
        if (state == STATE_EXIT || state == STATE_ATHREAD_RESTART) {
            my_unlock_mutex(snd_open);
            break;
        }

	/* keep on reading and discard if select says we can read.
         * this will allow us to catch up if we fall behind */
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
	read_timeout.tv_sec  = 0;
	read_timeout.tv_usec = 0;
        do {
	    count = read(fd, rdbuf16, buffer_size * n_output_channels * 2);
            if (count < 0) {
                perror("error reading from sound device: ");
                break;
            } else if (count != buffer_size * n_output_channels * 2) {
                gnuitar_printf( "warning: short read (%d/%d) from sound device\n", count, buffer_size);
                break;
	    }
	} while (select(fd+1, &read_fds, NULL, NULL, &read_timeout) != 0);
	
        db.len = buffer_size * n_input_channels;
        db.channels = n_input_channels;

        if (n_input_channels == n_output_channels) {
            /* 16 bits is the only possible for OSS */
            for (i = 0; i < db.len; i++)
                db.data[i] = rdbuf16[i] << 8;
        } else {
            /* 1 in, 2 out -- discard the right channel */
            for (i = 0; i < db.len; i ++)
                db.data[i] = rdbuf16[i * 2] << 8;
        }
	pump_sample(&db);

        /* Ensure that pump adapted us to output */
        assert(db.channels == n_output_channels);
        assert(db.len == buffer_size * n_output_channels);
        triangular_dither(&db);
	for (i = 0; i < db.len; i++)
	    wrbuf16[i] = (SAMPLE32) db.data[i] >> 8;

	count = write(fd, wrbuf16, buffer_size * n_output_channels * 2);
	if (count != buffer_size * n_output_channels * 2)
	    gnuitar_printf( "warning: short write (%d/%d) to sound device\n", count, buffer_size);
        my_unlock_mutex(snd_open);
    }

    return NULL;
}

static void
oss_finish_sound(void)
{
    state = STATE_PAUSE;
    my_lock_mutex(snd_open);
    oss_driver.enabled = 0;
    close(fd);
}

static int
oss_init_sound(void)
{
    int             i;
    
    if ((fd = open("/dev/dsp", O_RDWR)) == -1) {
	perror("cannot open audio device: ");
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
	gnuitar_printf( "Cannot setup fragments!\n");
	close(fd);
	return ERR_WAVEFRAGMENT;
    }

    if (ioctl(fd, SNDCTL_DSP_GETCAPS, &i) == -1) {
	gnuitar_printf( "Cannot get soundcard capabilities!\n");
	close(fd);
	return ERR_WAVEGETCAPS;
    }

    if (!(i & DSP_CAP_DUPLEX)) {
	gnuitar_printf(
		"Sorry but your soundcard isn't full duplex capable!\n");
	close(fd);
	exit(ERR_WAVENOTDUPLEX);
    }

    if (ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1) {
	gnuitar_printf( "Cannot setup fullduplex audio!\n");
	close(fd);
	return ERR_WAVEDUPLEX;
    }

    /* 16-bit recording is the best available with OSS */
    i = AFMT_S16_NE;
    bits = 16;
    if (ioctl(fd, SNDCTL_DSP_SETFMT, &i) == -1) {
	gnuitar_printf( "Cannot setup %d bit audio!\n", bits);
	close(fd);
	return ERR_WAVESETBIT;
    }

    i = n_output_channels;
    if (ioctl(fd, SNDCTL_DSP_CHANNELS, &i) == -1) {
	gnuitar_printf( "Cannot setup %d-channel audio!\n", i);
	close(fd);
	return ERR_WAVESETCHANNELS;
    }

    i = sample_rate;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &i) == -1) {
	gnuitar_printf( "Cannot setup sampling frequency %d Hz!\n", i);
	close(fd);
	return ERR_WAVESETRATE;
    }
    
    state = STATE_PROCESS;
    oss_driver.enabled = 1;
    my_unlock_mutex(snd_open);
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

static struct audio_driver_channels oss_channels_cfg[] = {
    { 1, 1 },
    { 1, 2 },
    { 2, 2 },
    { 0, 0 }
};

static unsigned int oss_bits_cfg[] = { 16, 0 };

audio_driver_t oss_driver = {
    .str = "OSS",
    .enabled = 0,
    .channels = oss_channels_cfg,
    .bits = oss_bits_cfg,

    .init = oss_init_sound,
    .finish = oss_finish_sound,
    .thread = oss_audio_thread
};

#endif /* HAVE_OSS */
