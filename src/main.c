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
#ifndef _WIN32
#    include <unistd.h>
#    include <sys/ioctl.h>
#    include <sys/time.h>
#    include <sched.h>
#    include <pthread.h>
#    include <sys/soundcard.h>
#else
#    include <time.h>
#    include <io.h>
#    include <stdlib.h>
#    include <windows.h>
#    include <process.h>
#    include <mmsystem.h>
#endif
#include <fcntl.h>
#include <sys/types.h>

#ifdef DEMO
#define DEMO_MSG "\n\nThis is the demo version of the GNUitar program." \
    "\nYou may download the full version as a source distribution" \
    "\nfrom http://freshmeat.net/projects/gnuitar" \
    "\nor purchase binary package from http://ziet.zhitomir.ua/~fonin/order.php\n"
#    ifdef _WIN32
#        define DEMO_TIMER 1
#    endif
#endif

#include "pump.h"
#include "tracker.h"
#include "gui.h"

#ifndef _WIN32
pthread_t       audio_thread;
#else
HANDLE          audio_thread;
DWORD           thread_id;
#endif

static int      stop = 0;	/* in Windows version, stop has special
				 * values: 0 - recording/playback is on 1
				 * - playback is paused 2 - exit thread 3
				 * - recording started, begin playback now
				 * For UNIX: 0 - recording/playback is on 1 
				 * - exit thread */
#ifndef _WIN32
int             fd;
#else
HWAVEIN         in;		/* input sound handle */
HWAVEOUT        out;		/* output sound handle */
MMRESULT        err;
			/*
			 * We use N WAVEHDR's for recording (ie,
			 * double-buffering)
			 */
WAVEHDR         wave_header[NBUFFERS];	/* input header */
WAVEHDR         write_header[NBUFFERS];	/* output headers */
char            cur_wr_hdr[NBUFFERS];	/* available write headers array */
char            wrbuf[BUFFER_SIZE * NBUFFERS];	/* write buffers */
char            rdbuf[BUFFER_SIZE * NBUFFERS];	/* receive buffer */
int             procbuf[BUFFER_SIZE];	/* procesing buffer */

void            serror(DWORD err, TCHAR * str);
#endif

char            version[32] = "GNUitar $Name$";

#ifndef _WIN32
void           *
#else
DWORD           WINAPI
#endif
audio_thread_start(void *V)
{
    int             count,
                    i;
#ifndef _WIN32
    SAMPLE          rdbuf[BUFFER_SIZE / 2];	/* receive buffer */
    int             procbuf[BUFFER_SIZE / 2];

    while (!stop) {
	count = read(fd, rdbuf, BUFFER_SIZE);
	if (count != BUFFER_SIZE) {
	    fprintf(stderr, "Cannot read samples!\n");
	    close(fd);
	    exit(0);
	}
	count /= 2;

	for (i = 0; i < count; i++)
	    procbuf[i] = rdbuf[i];

	pump_sample(procbuf, count);

	for (i = 0; i < count; i++) {
	    int             W = procbuf[i];
	    if (W < -32767)
		W = -32767;
	    if (W > 32767)
		W = 32767;
	    rdbuf[i] = W;
	}

	count = write(fd, rdbuf, BUFFER_SIZE);
	if (count != BUFFER_SIZE) {
	    fprintf(stderr, "Cannot write samples!\n");
	    close(fd);
	    exit(0);
	}
    }
#else
    MSG             msg;

    /*
     * Wait for a message sent to me by the audio driver 
     */
    while (stop != 2) {
	if (!GetMessage(&msg, 0, 0, 0)) {
	    return 0;
	}

	/*
	 * Figure out which message was sent 
	 */
	switch (msg.message) {
	case WM_QUIT:{
		return 0;
	    }
	    /*
	     * A buffer has been filled by the driver 
	     */
	case MM_WIM_DATA:{
		int             hdr_avail = -1;	/* available write header
						 * index */
		if (stop == 1) {
		    continue;
		}

		count = ((WAVEHDR *) msg.lParam)->dwBytesRecorded;
		if (count && !audio_lock) {
		    count /= 2;
		    for (i = 0; i < count; i++) {
			procbuf[i] =
			    ((SAMPLE *) (((WAVEHDR *) msg.lParam)->
					 lpData))[i];
		    }


		    /*
		     * find unused output buffer and queue it to output 
		     */
		    for (i = 0; i < NBUFFERS; i++)
			if (cur_wr_hdr[i] == 1) {
			    hdr_avail = i;
			    cur_wr_hdr[i] = 0;	/* ready to queue */
			    break;
			}

		    if (hdr_avail != -1) {
			pump_sample(procbuf, count);

			for (i = 0; i < count; i++) {
			    int             W = procbuf[i];
			    if (W < -32767)
				W = -32767;
			    if (W > 32767)
				W = 32767;
			    ((SAMPLE *) (write_header[hdr_avail].
					 lpData))[i] = W;
			}

			err =
			    waveOutWrite(out, &write_header[hdr_avail],
					 sizeof(WAVEHDR));

			if (err) {
			    serror(err, "\nwriting samples - ");
			}
		    } else
			printf("\nbuffer overrun.");
		} else {
		    // printf("\nbuffer underrun.");
		}
		/*
		 * Now we need to requeue this buffer so the driver can
		 * use it for another block of audio data. NOTE: We
		 * shouldn't need to waveInPrepareHeader() a WAVEHDR that
		 * has already been prepared once 
		 */
		waveInAddBuffer(in, (WAVEHDR *) msg.lParam,
				sizeof(WAVEHDR));
		continue;
	    }
	    /*
	     * Our main thread is opening the WAVE device 
	     */
	case MM_WIM_OPEN:{
		continue;
	    }
	    /*
	     * Our main thread is closing the WAVE device 
	     */
	case MM_WIM_CLOSE:{
		/*
		 * Terminate this thread (by return'ing) 
		 */
		break;
	    }
	    /*
	     * Audio driver is ready to playback next block 
	     */
	case MM_WOM_DONE:{
		/*
		 * Clear the WHDR_DONE bit (which the driver set last time 
		 * that this WAVEHDR was sent via waveOutWrite and was
		 * played). Some drivers need this to be cleared 
		 */
		((WAVEHDR *) msg.lParam)->dwFlags &= ~WHDR_DONE;
		for (i = 0; i < NBUFFERS; i++)
		    if (&write_header[i] == (WAVEHDR *) msg.lParam) {
			cur_wr_hdr[i] = 1;
			break;
		    }
		continue;
	    }
	default:
	    ;
	}
    }
    return 0;
#endif
}

#ifdef _WIN32
/*
 * Retrieves and displays an error message for the passed Wave In error
 * number. It does this using mciGetErrorString().
 */

void
serror(DWORD err, TCHAR * str)
{
    char            buffer[128];

    fprintf(stderr, "ERROR 0x%08X: %s", err, str);
    if (mciGetErrorString(err, &buffer[0], sizeof(buffer))) {
	fprintf(stderr, "%s\r\n", &buffer[0]);
    } else {
	fprintf(stderr, "0x%08X returned!\r\n", err);
    }
}

#endif

/*
 * graceful application shutdown 
 */
void
die(void)
{
    int             i;

    stop = 2;
    pump_stop();
    tracker_done();

#ifndef _WIN32
    close(fd);
#else
    /*
     * Stop Windows queuing of buffers 
     */
    waveOutReset(out);
    waveInReset(in);
    /*
     * Unprepare WAVE buffers 
     */
    for (i = 0; i < NBUFFERS; i++) {
	waveOutUnprepareHeader(out, &write_header[i], sizeof(WAVEHDR));
    }

    /*
     * We should unprepare the read headers here,
     * but it is possible not to do it at all,
     * since we didn't use malloc()'s to allocate them
     */

    /*
     * Close WAVE devices
     */
    waveOutClose(out);
    waveInClose(in);
#endif
}

#ifndef _WIN32
/*
 * Calculate base-2 logarithm of the value, up to 512k
 */
short
log2(int x)
{
    int             pow[] =
	{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192,
16384, 32768,
	65536, 131072, 262144, 524288
    };
    int             i;
    for (i = 0; i < sizeof(pow) / sizeof(int); i++) {
	if (pow[i] == x)
	    return i;
    }
    return 0;
}
#endif

#ifdef DEMO
#   ifdef _WIN32
VOID CALLBACK
expired(HWND hwnd, UINT msg, UINT timer_id, DWORD time)
{
    gtk_main_quit();
    die();
    printf("%s", DEMO_MSG);
    exit(10);
}

#   else
#   endif
#endif

int
main(int argc, char **argv)
{
#ifndef _WIN32
    int             max_priority,
                    i;
    struct sched_param p;


    if ((fd = open("/dev/dsp", O_RDWR)) == -1) {
	fprintf(stderr, "\nCannot open audio device!");
	return -1;
    }

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
    i = 0x7fff0000 + log2(BUFFER_SIZE);
    if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &i) < 0) {
	fprintf(stderr, "\nCannot setup fragments!");
	close(fd);
	return -1;
    }

    if (ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1) {
	fprintf(stderr, "\nCannot setup fullduplex audio!");
	close(fd);
	return -1;
    }

    if (ioctl(fd, SNDCTL_DSP_GETCAPS, &i) == -1) {
	fprintf(stderr, "\nCannot get soundcard capabilities!");
	close(fd);
	return -1;
    }

    if (!(i & DSP_CAP_DUPLEX)) {
	fprintf(stderr,
		"\nSorry but your soundcard isn't full duplex capable!");
	close(fd);
	return -1;
    }

    i = AFMT_S16_LE;
    if (ioctl(fd, SNDCTL_DSP_SETFMT, &i) == -1) {
	fprintf(stderr, "\nCannot setup 16 bit audio!");
	close(fd);
	return -1;
    }

    i = NCHANNELS - 1;
    if (ioctl(fd, SNDCTL_DSP_STEREO, &i) == -1) {
	fprintf(stderr, "\nCannot setup mono audio!");
	close(fd);
	return -1;
    }

    i = SAMPLE_RATE;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &i) == -1) {
	fprintf(stderr, "\nCannot setup sampling frequency %dHz!", i);
	close(fd);
	return -1;
    }
#else
    WAVEFORMATEX    format;	/* wave format */
    int             i;

#ifdef DEMO
    /*
     * For demo version, we exit after a random time, 10 up to 15 minutes
     */
    srand(time(NULL));
#   ifdef _WIN32
    SetTimer(GetActiveWindow(), DEMO_TIMER,
	     (unsigned int) (1000 * 60 * 10 +
			     1000 * 60 * 5 * rand() / (RAND_MAX + 1.0)),
	     expired);
#   else
#   endif
#endif

    ZeroMemory(&wave_header[0], sizeof(WAVEHDR) * NBUFFERS);
    ZeroMemory(&write_header[0], sizeof(WAVEHDR) * NBUFFERS);

    /*
     * we are NOT in record now - signal to the audio thread 
     */
    stop = 1;

    /*
     * set high priority to the process 
     */
    /*
     * if(!SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS)) {
     * fprintf(stderr,"\nFailed to set realtime priority to process:
     * %s.",GetLastError()); } 
     */
    /*
     * create audio thread 
     */
    audio_thread =
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) audio_thread_start, 0,
		     0, &thread_id);
    if (!audio_thread) {
	fprintf(stderr, "Can't create WAVE recording thread! -- %08X\n",
		GetLastError());
	return (-1);
    }
    /*
     * set realtime priority to the thread 
     */
    if (!SetThreadPriority(audio_thread, THREAD_PRIORITY_TIME_CRITICAL)) {
	fprintf(stderr,
		"\nFailed to set realtime priority to thread: %s. Continuing with default priority.",
		GetLastError());
    }
    CloseHandle(audio_thread);

    /*
     * set audio parameters - sampling rate, number of channels etc. 
     */
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = NCHANNELS;
    format.nSamplesPerSec = SAMPLE_RATE;
    format.wBitsPerSample = 16;
    format.nBlockAlign = format.nChannels * (format.wBitsPerSample / 8);
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize = 0;

    /*
     * Open Digital Audio In device 
     */
    err =
	waveInOpen(&in, WAVE_MAPPER, &format, (DWORD) thread_id, 0,
		   CALLBACK_THREAD);
    if (err) {
	serror(err,
	       "There was an error opening the Digital Audio In device\r\n");
	stop = 2;
	exit(-1);
    }

    /*
     * Open the Digital Audio Out device. 
     */
    if ((err =
	 waveOutOpen(&out, WAVE_MAPPER, &format, (DWORD) thread_id, 0,
		     CALLBACK_THREAD))) {
	serror(err,
	       "There was an error opening the Digital Audio Out device!\r\n");
	stop = 2;
	exit(-1);
    }
    for (i = 0; i < NBUFFERS; i++) {
	write_header[i].lpData = wrbuf + i * BUFFER_SIZE;
	/*
	 * Fill in WAVEHDR fields for buffer starting address and size 
	 */
	write_header[i].dwBufferLength = BUFFER_SIZE;
	/*
	 * Leave other WAVEHDR fields at 0 
	 */
	/*
	 * Prepare the N WAVEHDR's 
	 */
	if ((err =
	     waveOutPrepareHeader(out, &write_header[i],
				  sizeof(WAVEHDR)))) {
	    fprintf(stderr, "ERROR: preparing WAVEHDR %d! -- %08X\n", i,
		    err);
	    stop = 2;
	    exit(1);
	}
	cur_wr_hdr[i] = 1;
    }


    for (i = 0; i < NBUFFERS; i++) {
	wave_header[i].dwBufferLength = BUFFER_SIZE;
	wave_header[i].lpData = rdbuf + i * BUFFER_SIZE;
	/*
	 * Fill in WAVEHDR fields for buffer starting address. We've
	 * already filled in the size fields above 
	 */
	wave_header[i].dwFlags = 0;
	/*
	 * Leave other WAVEHDR fields at 0 
	 */

	/*
	 * Prepare the WAVEHDR's 
	 */
	if ((err =
	     waveInPrepareHeader(in, &wave_header[i], sizeof(WAVEHDR)))) {
	    serror(err, "Error preparing WAVEHDR!\n");
	    stop = 2;
	    exit(-1);
	}
	/*
	 * Queue WAVEHDR (recording hasn't started yet) 
	 */
	if ((err = waveInAddBuffer(in, &wave_header[i], sizeof(WAVEHDR)))) {
	    serror(err, "Error queueing WAVEHDR!\n");
	    stop = 2;
	    exit(-1);
	}
    }
    /*
     * Start recording. Our secondary thread will now be receiving
     * and processing audio data
     */
    if ((err = waveInStart(in))) {
	serror(err, "Error starting record!\n");
	stop = 2;
	exit(-1);
    }
    stop = 0;

#endif


#ifndef _WIN32
    max_priority = sched_get_priority_max(SCHED_FIFO);
    p.sched_priority = max_priority;

    if (sched_setscheduler(0, SCHED_FIFO, &p)) {
	printf
	    ("\nFailed to set scheduler priority. (Are you running as root?)");
	printf("\nContinuing with default priority");
    }
    if (pthread_create(&audio_thread, NULL, audio_thread_start, NULL)) {
	fprintf(stderr, "\nAudio thread creation failed!");
	exit(1);
    }
    /*
     * We were running to this point as setuid root program.
     * Switching to our native user id
     */
    setuid(getuid());
#endif

    printf("\n\n"COPYRIGHT \
	    "This program is a free software and distributed under GPL license;\n" \
	    "see Help->About for details.\n");

    gtk_init(&argc, &argv);
    init_gui();
    pump_start(argc, argv);
    gtk_main();

    die();
    return 0;
}
