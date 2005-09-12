/*
 * GNUitar
 * Main module
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
 * Revision 1.49  2005/09/12 22:10:50  alankila
 * - add missing checks into configure.in to silence autoscan's warnings
 *
 * Revision 1.48  2005/09/12 09:42:25  fonin
 * - MSVC compatibility fixes
 *
 * Revision 1.47  2005/09/04 23:28:32  alankila
 * - in case audio driver opening fails, set driver to invalid
 *
 * Revision 1.46  2005/09/04 21:04:21  alankila
 * - handle no audio driver more gracefully
 *
 * Revision 1.45  2005/09/04 20:45:01  alankila
 * - store audio driver into config
 * - make it possible to start gnuitar with invalid audio driver and enter
 *   options and correct the driver. Some rough edges still remain with
 *   the start/stop button, mutexes, etc.
 *
 * Revision 1.44  2005/09/04 16:06:59  alankila
 * - first multichannel effect: delay
 * - need to use surround40 driver in alsa
 * - introduce new buffer data_swap so that effects need not reserve buffers
 * - correct off-by-one error in multichannel adapting
 *
 * Revision 1.43  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.42  2005/09/03 20:20:42  alankila
 * - create audio_driver type and write all the driver stuff into it. This
 *   faciliates carrying configuration data about the capabilities of
 *   a specific audio driver and uses less global variables.
 *
 * Revision 1.41  2005/08/28 21:41:28  fonin
 * Portability: introduced new functions for mutexes
 *
 * Revision 1.40  2005/08/28 14:04:04  alankila
 * - OSS copypaste error fix
 * - remove my_log2 in favour of doing pow, trunc, log.
 * - OSS driver rounds buffer sizes to suitable values by itself now. There's
 *   a precedent in tuning user parameters automatically in ALSA code. The
 *   new behaviour rounds buffer size down, though.
 *
 * Revision 1.39  2005/08/28 12:39:01  alankila
 * - make audio_lock a real mutex
 * - fix mutex cleanup at exit
 *
 * Revision 1.38  2005/08/28 12:28:44  alankila
 * switch to GMutex that is also available on win32
 *
 * Revision 1.37  2005/08/27 19:05:43  alankila
 * - introduce SAMPLE16 and SAMPLE32 types, and switch
 *
 * Revision 1.36  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.35  2005/08/26 15:59:56  fonin
 * Audio driver now can be chosen by user
 *
 * Revision 1.34  2005/08/25 19:52:11  fonin
 * Removed code for demo version
 *
 * Revision 1.33  2005/08/24 22:33:02  alankila
 * - avoid reopening sound device at exit in order to cleanly shut it down the
 *   next moment
 *
 * Revision 1.32  2005/08/24 21:59:00  alankila
 * some minor cleanup yet
 *
 * Revision 1.31  2005/08/24 21:44:44  alankila
 * - split sound drivers off main.c
 * - add support for alsa
 * - rework thread locking
 * - in this version, sound drivers are chosen at compile time
 * - windows driver is probably broken
 *
 * Revision 1.30  2005/08/24 18:41:36  fonin
 * Bugfix: when the # of samples read != buffer size, program exited. This condition sometimes happen when press on STOP button.
 *
 * Revision 1.29  2005/08/22 22:11:59  alankila
 * - change RC filters to accept data_block
 * - LC filters have no concept of "LOWPASS" or "HIGHPASS" filtering, there's
 *   just filter_no.
 * - remove unused SAMPLE8 typedef
 *
 * Revision 1.28  2005/08/22 11:07:27  alankila
 * - move last bits of tracker support off main.c to pump.c
 * - add settings loader/saver for GTK2, now needs GTK+ 2.6 in minimum
 *
 * Revision 1.27  2005/08/21 23:44:13  alankila
 * - use libsndfile on Linux to write audio as .wav
 * - move clipping tests into pump.c to save writing it in tracker and 3 times
 *   in main.c
 * - give default name to .wav from current date and time (in ISO format)
 * - there's a weird bug if you cancel the file dialog, it pops up again!
 *   I have to look into what's going on.
 *
 * Revision 1.26  2005/08/14 23:36:13  alankila
 * - set # of channels directly. What I really need is to control number of
 *   input and output channels separately, but it seems impossible with OSS.
 *   It's probably time to switch to ALSA.
 *
 * Revision 1.25  2005/08/11 17:57:22  alankila
 * - add some missing headers & fix all compiler warnings on gcc 4.0.1+ -Wall
 *
 * Revision 1.24  2005/08/08 12:03:26  fonin
 * Fixed include sys/select.h which did not work on windows.
 *
 * Revision 1.23  2005/08/07 13:13:14  alankila
 * oops: reinstate the MAX_BUFFER_SIZE / sizeof(SAMPLE) in the rdbuf & procbuf.
 * Removing that was a mistake. Keep the rest, though.
 *
 * Revision 1.22  2005/08/07 13:03:57  alankila
 * - add select() around the read() part to read and discard buffers if
 *   we begin to fall behind in audio processing
 *
 * Revision 1.21  2005/04/15 14:37:41  fonin
 * Fixed version variable
 *
 * Revision 1.20  2005/04/06 19:34:58  fonin
 * Fixed the accidental typo with "count=bits >> 8" that caused the floating exception
 *
 * Revision 1.19  2004/10/21 11:19:18  dexterus
 * Bug in the win 32 section related to sample type (SAMPLE insted of
 * DSP_SAMPLE ) fixed -- win32 working
 *
 * Revision 1.18  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.17  2003/05/30 12:49:23  fonin
 * log2() renamed to my_log2() since log2 is a reserved word on MacOS X.
 *
 * Revision 1.16  2003/03/25 14:03:01  fonin
 * Work around buffer overruns with DirectSound playback.
 *
 * Revision 1.15  2003/03/23 20:05:42  fonin
 * New playback method via DirectSound.
 *
 * Revision 1.14  2003/03/15 20:07:01  fonin
 * Moved function expired(), fixed compilation error.
 *
 * Revision 1.13  2003/03/09 21:05:57  fonin
 * Internal redesign for new "change sampling params" feature.
 * New functions init_sound() and close_sound().
 *
 * Revision 1.12  2003/02/11 21:45:03  fonin
 * URL fixes.
 *
 * Revision 1.11  2003/02/05 21:10:10  fonin
 * Cleanup before release.
 *
 * Revision 1.10  2003/02/03 17:24:04  fonin
 * Disclaimer and legal notice text moved to a string constants to gui.h
 *
 * Revision 1.9  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.8  2003/01/31 15:18:04  fonin
 * Few cleanups, more comments; start recording AFTER all input buffers are
 * queued.
 *
 * Revision 1.7  2003/01/30 21:33:31  fonin
 * - Added demo version code for Win32;
 * - NCHANNELS now is used in UNIX build.
 *
 * Revision 1.6  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.5  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.4  2001/04/27 14:29:14  fonin
 * <sys/soundcard.h> for better compatibility with FreeBSD.
 *
 * Revision 1.3  2001/03/25 17:42:55  fonin
 * Switching back to real user identifier immediately after setting realtime priority.
 *
 * Revision 1.2  2001/03/25 12:10:06  fonin
 * Text messages begin from newline rather than end with it.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:53  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#include <pthread.h>
#endif

#include "pump.h"
#include "main.h"
#include "tracker.h"
#include "gui.h"

volatile audio_driver_t  *audio_driver = NULL;
volatile int    state;
char            version[13] = "GNUitar "VERSION;

my_mutex        snd_open=NULL;
#ifndef _WIN32
pthread_t       audio_thread = 0;

SAMPLE16        rdbuf[MAX_BUFFER_SIZE   / sizeof(SAMPLE16)];
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
DSP_SAMPLE      procbuf2[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
#else
HANDLE          audio_thread = 0;

char            wrbuf[MIN_BUFFER_SIZE * MAX_BUFFERS];
char            rdbuf[MIN_BUFFER_SIZE * MAX_BUFFERS];
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE];
DSP_SAMPLE      procbuf2[MAX_BUFFER_SIZE];
#endif

int
main(int argc, char **argv)
{
    int             error = 0;
#ifndef _WIN32
    int             max_priority;
    struct sched_param p;

    max_priority = sched_get_priority_max(SCHED_FIFO);
    p.sched_priority = max_priority;

    if (sched_setscheduler(0, SCHED_FIFO, &p)) {
	fprintf(stderr, "warning: unable to set realtime priority (needs root privileges)\n");
    }

    /*
     * We were running to this point as setuid root program.
     * Switching to our native user id
     */
    setuid(getuid());
#endif

    printf(COPYRIGHT
       "This program is a free software under the GPL;\n"
       "see Help->About for details.\n");

    gtk_init(&argc, &argv);
    load_settings();

#ifndef _WIN32
    state = STATE_PAUSE;
    /* choose audio driver if not given in config */
    if (audio_driver_str == NULL) {
        fprintf(stderr, "Discovering audio driver.\n");
#    ifdef HAVE_JACK
        if (jack_available()) {
            audio_driver_str = "JACK";
        } else
#    endif
#    ifdef HAVE_ALSA
        if (alsa_available()) {
            audio_driver_str = "ALSA";
        } else
#    endif
#    ifdef HAVE_OSS
        if (oss_available()) {
            audio_driver_str = "OSS";
        } else
#    endif
            printf("warning: no usable audio driver found. Tried jack, alsa and oss (if compiled in.)\n");
    }
#    ifdef HAVE_JACK
    if (strcmp(audio_driver_str, "JACK") == 0) {
        audio_driver = &jack_driver;
    } else
#    endif
#    ifdef HAVE_ALSA
    if (strcmp(audio_driver_str, "ALSA") == 0) {
        audio_driver = &alsa_driver;
    } else
#    endif
#    ifdef HAVE_OSS
    if (strcmp(audio_driver_str, "OSS") == 0) {
        audio_driver = &oss_driver;
    }
#    endif
    g_thread_init(NULL);
    my_create_mutex(&snd_open);
    my_lock_mutex(snd_open);

    if (audio_driver) {
        if (pthread_create(&audio_thread, NULL, audio_driver->thread, NULL)) {
            fprintf(stderr, "Audio thread creation failed!\n");
            return ERR_THREAD;
        }
    }
#else
    state = STATE_START_PAUSE;

    audio_driver = &windows_driver;
    my_create_mutex(&snd_open);
    /*
     * create audio thread
     */
    audio_thread =
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) audio_driver->audio_proc, 0,
		     0, &thread_id);
    if (!audio_thread) {
	fprintf(stderr, "Can't create WAVE recording thread! -- %08X\n",
		GetLastError());
	return ERR_THREAD;
    }

    /*
     * set realtime priority to the thread
     */
    if (!SetThreadPriority(audio_thread, THREAD_PRIORITY_TIME_CRITICAL)) {
	fprintf(stderr,
		"\nFailed to set realtime priority to thread: %s. Continuing with default priority.",
		GetLastError());
    }
#endif
    init_gui();

    pump_start(argc, argv);
    if (audio_driver) {
        if ((error = audio_driver->init()) != ERR_NOERROR) {
            fprintf(stderr, "warning: unable to begin audio processing (code %d)\n", error);
	    audio_driver = NULL;
	    audio_driver_str = "invalid";
        }
    }

    gtk_main();

#ifndef _WIN32
    /* wait for audio thread to finish */
    if (state == STATE_PAUSE || state == STATE_ATHREAD_RESTART) {
        state = STATE_EXIT;
        my_unlock_mutex(snd_open);
        pthread_join(audio_thread, NULL);
    } else {
        state = STATE_EXIT;
        pthread_join(audio_thread, NULL);
        audio_driver->finish();
        my_unlock_mutex(snd_open);
    }
#else
    state = STATE_EXIT;
    my_unlock_mutex(snd_open);
    audio_driver->finish();
#endif
    pump_stop();
    save_settings();
    my_close_mutex(snd_open);

    return ERR_NOERROR;
}


