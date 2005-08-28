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

#include "pump.h"
#include "main.h"
#include "utils.h"

int             fd;

void           *
oss_audio_thread(void *V)
{
    int             count, i;
    struct data_block db;
    fd_set read_fds;
    struct timeval read_timeout;

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
	    count = read(fd, rdbuf, buffer_size);
            if (count < 0) {
                perror("error reading from sound device: ");
                break;
            } else if (count != buffer_size) {
                fprintf(stderr, "warning: short read (%d/%d) from sound device\n", count, buffer_size);
                break;
	    }
	} while (select(fd+1, &read_fds, NULL, NULL, &read_timeout) != 0);
	
	count /= bits / 8;
        db.data = procbuf;
        db.len = count;
        db.channels = nchannels;

	/* 16 bits is the only possible for OSS */
	for (i = 0; i < count; i++)
	    db.data[i] = rdbuf[i] << 8;
	pump_sample(&db);
	for (i = 0; i < count; i++)
	    rdbuf[i] = db.data[i] >> 8;

	count = write(fd, rdbuf, buffer_size);
	if (count != buffer_size)
	    fprintf(stderr, "warning: short write (%d/%d) to sound device\n", count, buffer_size);
        my_unlock_mutex(snd_open);
    }

    return NULL;
}

void
oss_finish_sound(void)
{
    state = STATE_PAUSE;
    my_lock_mutex(snd_open);
    close(fd);
}

int
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
    i = 0x7fff0000 + (int) (log(buffer_size) / log(2));
    if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &i) < 0) {
	fprintf(stderr, "Cannot setup fragments!\n");
	close(fd);
	return ERR_WAVEFRAGMENT;
    }

    if (ioctl(fd, SNDCTL_DSP_GETCAPS, &i) == -1) {
	fprintf(stderr, "Cannot get soundcard capabilities!\n");
	close(fd);
	return ERR_WAVEGETCAPS;
    }

    if (!(i & DSP_CAP_DUPLEX)) {
	fprintf(stderr,
		"Sorry but your soundcard isn't full duplex capable!\n");
	close(fd);
	exit(ERR_WAVENOTDUPLEX);
    }

    if (ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1) {
	fprintf(stderr, "Cannot setup fullduplex audio!\n");
	close(fd);
	return ERR_WAVEDUPLEX;
    }

    /* 16-bit recording is the best available with OSS */
    i = AFMT_S16_LE;
    bits = 16;
    if (ioctl(fd, SNDCTL_DSP_SETFMT, &i) == -1) {
	fprintf(stderr, "Cannot setup %d bit audio!\n", bits);
	close(fd);
	return ERR_WAVESETBIT;
    }

    i = nchannels;
    if (ioctl(fd, SNDCTL_DSP_CHANNELS, &i) == -1) {
	fprintf(stderr, "Cannot setup mono audio!\n");
	close(fd);
	return ERR_WAVESETCHANNELS;
    }

    i = sample_rate;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &i) == -1) {
	fprintf(stderr, "Cannot setup sampling frequency %d Hz!\n", i);
	close(fd);
	return ERR_WAVESETRATE;
    }
    
    state = STATE_PROCESS;
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

#endif /* HAVE_OSS */
