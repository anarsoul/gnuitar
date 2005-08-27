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
#include <unistd.h>
#include <sys/types.h>
#ifndef _WIN32
#include <pthread.h>
#endif
 
#include "pump.h"
#include "main.h"
#include "tracker.h"
#include "gui.h"

volatile int    state;
char            version[13] = "GNUitar "VERSION;

#ifndef _WIN32
pthread_t       audio_thread;
pthread_mutex_t snd_open = PTHREAD_MUTEX_INITIALIZER;

SAMPLE16        rdbuf[MAX_BUFFER_SIZE   / sizeof(SAMPLE16)];
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
void*           (*audio_proc) (void *V); /* pointer to audio thread routine */
#else
HANDLE          audio_thread;

char            wrbuf[MIN_BUFFER_SIZE * MAX_BUFFERS];
char            rdbuf[MIN_BUFFER_SIZE * MAX_BUFFERS];
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE];
DWORD WINAPI    (*audio_proc) (void *V); /* pointer to audio thread routine */
#endif
int		(*audio_init) (void);	/* init sound routine   */
void		(*audio_finish) (void);	/* finish sound routine */

int
main(int argc, char **argv)
{
    int             error = 0;
#ifndef _WIN32
    int             max_priority;
    struct sched_param p;

#ifdef HAVE_JACK
    if (jack_available()) {
	audio_init = jack_init_sound;
	audio_finish = jack_finish_sound;
	audio_proc = jack_audio_thread;
    } else
#endif
#ifdef HAVE_ALSA
    if (alsa_available()) {
	audio_init = alsa_init_sound;
	audio_finish = alsa_finish_sound;
	audio_proc = alsa_audio_thread;
    } else
#endif
#ifdef HAVE_OSS
    if (oss_available()) {
	audio_init=alsa_init_sound;
	audio_finish=alsa_finish_sound;
	audio_proc=alsa_audio_thread;
    } else
#endif
    {
	printf("No usable audio driver found. Tried jack, alsa and oss (if compiled in.)\n");
	return ERR_NOAUDIOAVAILABLE;
    }
    
    /* prepare state & mutex for audio thread init_sound() */
    state = STATE_PAUSE;
    pthread_mutex_lock(&snd_open);
    
    max_priority = sched_get_priority_max(SCHED_FIFO);
    p.sched_priority = max_priority;

    if (sched_setscheduler(0, SCHED_FIFO, &p)) {
	fprintf(stderr, "warning: unable to set realtime priority (needs root privileges)\n");
    }

    if (pthread_create(&audio_thread, NULL, audio_proc, NULL)) {
	fprintf(stderr, "Audio thread creation failed!\n");
	return ERR_THREAD;
    }
    /*
     * We were running to this point as setuid root program.
     * Switching to our native user id
     */
    setuid(getuid());
#else
    state = STATE_START_PAUSE;
    audio_proc=windows_audio_thread;
    audio_init=windows_init_sound;
    audio_finish=windows_finish_sound;

    /*
     * create audio thread 
     */
    audio_thread =
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) audio_proc, 0,
		     0, &thread_id);
    if (!audio_thread) {
	fprintf(stderr, "Can't create WAVE recording thread! -- %08X\n",
		GetLastError());
	return (ERR_THREAD);
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

    printf(COPYRIGHT
       "This program is a free software under the GPL;\n"
       "see Help->About for details.\n");

    gtk_init(&argc, &argv);
    load_settings();
    init_gui();
    
    pump_start(argc, argv);
    if ((error = (*audio_init)()) != ERR_NOERROR) {
	fprintf(stderr, "warning: unable to begin audio processing (code %d)\n", error);
    }

    gtk_main();

#ifndef _WIN32
    /* wait for audio thread to finish */
    if (state == STATE_PAUSE || state == STATE_ATHREAD_RESTART) {
        state = STATE_EXIT;
        pthread_mutex_unlock(&snd_open);
        pthread_join(audio_thread, NULL);
    } else {
        state = STATE_EXIT;
        pthread_join(audio_thread, NULL);
        (*audio_finish)();
    }
#else
    state = STATE_EXIT;
    (*audio_finish)();
#endif
    pump_stop();
    save_settings();
    
    return ERR_NOERROR;
}
