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
#    include <dsound.h>
#endif
#include <fcntl.h>
#include <sys/types.h>

#ifdef DEMO
#define DEMO_MSG "\n\nThis is the demo version of the GNUitar program." \
    "\nYou may download the full version as a source distribution" \
    "\nfrom http://freshmeat.net/projects/gnuitar" \
    "\nor purchase binary package from http://ns2.ziet.zhitomir.ua/~fonin/order.php\n"
#    ifdef _WIN32
#        define DEMO_TIMER 1
#    endif
#endif

#include "pump.h"
#include "tracker.h"
#include "gui.h"
#include "utils.h"

static int      state = STATE_PROCESS;	/* 
					 * State of the program 
					 * (see utils.h for possible values)
					 */
char            version[32] = "GNUitar $Name$";

#ifndef _WIN32
pthread_t       audio_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  suspend = PTHREAD_COND_INITIALIZER;
int             fd;

#else
HANDLE          audio_thread;
HANDLE          input_bufs_done,
                output_bufs_done;
DWORD           thread_id;

LPDIRECTSOUND   snd = NULL;	/* DirectSound object */
LPDIRECTSOUNDBUFFER dbuffer = NULL;	/* DS buffer */
short           dsound = 1;	/* flag - do we use DirectSound for output ? */
unsigned short	overrun_threshold=4;	/* after this number of fragments 
					 * overran buffer will be recovered  */
HWAVEIN         in;		/* input sound handle */
HWAVEOUT        out;		/* output sound handle */
MMRESULT        err;
			/*
			 * We use N WAVEHDR's for recording (ie,
			 * double-buffering)
			 */
WAVEHDR         wave_header[MAX_BUFFERS];	/* input header */
WAVEHDR         write_header[MAX_BUFFERS];	/* output headers */
char            cur_wr_hdr[MAX_BUFFERS];	/* available write headers 
						 * array */
char            wrbuf[MIN_BUFFER_SIZE * MAX_BUFFERS];	/* write buffers */
char            rdbuf[MIN_BUFFER_SIZE * MAX_BUFFERS];	/* receive buffer */
int             procbuf[MAX_BUFFER_SIZE];	/* procesing buffer */
int             active_in_buffers = 0,
                active_out_buffers = 0;

void            serror(DWORD err, TCHAR * str);
#endif


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
    SAMPLE          rdbuf[MAX_BUFFER_SIZE / sizeof(SAMPLE)];	/* receive 
								 * buffer */
    int             procbuf[MAX_BUFFER_SIZE / sizeof(SAMPLE)];

    while (state != STATE_EXIT) {
	if (state == STATE_PAUSE) {
	    pthread_cond_wait(&suspend, &mutex);
	}

	count = read(fd, rdbuf, buffer_size);
	if (count != buffer_size) {
	    fprintf(stderr, "Cannot read samples!\n");
	    close(fd);
	    exit(ERR_WAVEINRECORD);
	}
	count /= bits / 8;

	for (i = 0; i < count; i++)
	    procbuf[i] = rdbuf[i];
	pump_sample(procbuf, count);

	for (i = 0; i < count; i++) {
	    int             W = procbuf[i];
	    if (W < -MAX_SAMPLE)
		W = -MAX_SAMPLE;
	    if (W > MAX_SAMPLE)
		W = MAX_SAMPLE;
	    rdbuf[i] = W;
	}

	count = write(fd, rdbuf, buffer_size);
	if (count != buffer_size) {
	    fprintf(stderr, "Cannot write samples!\n");
	}
    }
#else
    MSG             msg;
    HRESULT         res;
    DWORD           write_pos = 0,
                    read_pos = 0,
                    len1 = 0,
                    len2 = 0;
    /*
     * read/write cursors and lengths for DS calls 
     */
    SAMPLE         *pos1 = NULL,
                   *pos2 = NULL;	/* pointers for DirectSound lock
					 * call */
    static unsigned int bufpos = 0;	/* current write position in the
					 * buffer (DirectSound) */
    int             j,
                    k;

    /*
     * Wait for a message sent to me by the audio driver 
     */
    while (state != STATE_EXIT) {
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
		active_in_buffers--;
		if (state == STATE_PAUSE || state == STATE_START_PAUSE) {
		    continue;
		}

		count = ((WAVEHDR *) msg.lParam)->dwBytesRecorded;
		if (count && !audio_lock) {
		    count /= bits / 8;
		    for (i = 0; i < count; i++) {
			procbuf[i] =
			    ((SAMPLE *) (((WAVEHDR *) msg.lParam)->
					 lpData))[i];
		    }


		    /*
		     * find unused output buffer and queue it to output 
		     */
		    for (i = 0; !dsound && i < nbuffers; i++)
			if (cur_wr_hdr[i] == 1) {
			    hdr_avail = i;
			    cur_wr_hdr[i] = 0;	/* ready to queue */
			    break;
			}

		    if (dsound || hdr_avail != -1) {
			pump_sample(procbuf, count);

			/*
			 * DirectSound output:
			 * 1) get current read/write buffer cursors
			 * 2) lock buffer
			 * 3) fill buffer
			 * 4) unlock buffer
			 * 5) if this is a first fragment read after
			 *    sound initialize, we should
			 *    start playback
			 */
			if (dsound) {
			    res =
				IDirectSoundBuffer_GetCurrentPosition
				(dbuffer, &read_pos, &write_pos);
			    /*
			     * If this is a start state, buffer position
			     * is equal to the write position
			     * returned by the call above
			     */
			    if (state == STATE_START) {
				bufpos = write_pos+buffer_size;
			    }
			    /*
			     * otherwise we just increment buffer position
			     * by the fragment size
			     */
			    else {
				/* workaround buffer overrun */
				if(bufpos<write_pos &&
				    /* this condition handles buffer wrap around */
				    write_pos-bufpos>buffer_size*overrun_threshold &&
				    write_pos-bufpos<buffer_size*200) {
				    bufpos=write_pos+buffer_size;
				    fprintf(stderr,"\nbuffer overrun !");
				}
				else bufpos += buffer_size;
			    }
			    /*
			     * handle wrap around 
			     */
			    if (bufpos >= MIN_BUFFER_SIZE * MAX_BUFFERS)
				bufpos = 0;
//fprintf(stdout,"\n%d,%d,%d,%d",read_pos,write_pos,bufpos-write_pos,bufpos);

			    if (res == DS_OK) {
				res = IDirectSoundBuffer_Lock(dbuffer,
							      bufpos,
							      buffer_size,
							      &pos1, &len1,
							      &pos2, &len2,
							      0);

				if (res != DS_OK) {
				    fprintf(stderr,
					    "\nCannot lock buffer: ");
				    switch (res) {
				    case DSERR_INVALIDCALL:{
					    fprintf(stderr,
						    "DSERR_INVALIDCALL");
					    break;
					}
				    case DSERR_INVALIDPARAM:{
					    fprintf(stderr,
						    "DSERR_INVALIDPARAM");
					    break;
					}
				    case DSERR_BUFFERLOST:{
					    fprintf(stderr,
						    "DSERR_BUFFERLOST");
					    break;
					}
				    case DSERR_PRIOLEVELNEEDED:{
					    fprintf(stderr,
						    "DSERR_PRIOLEVELNEEDED ");
					    break;
					}
				    }
				    state = STATE_EXIT;
				}

			    }
			    for (i = 0, j = 0, k = 0; i < count; i++) {
				int             W = procbuf[i];
				SAMPLE         *curpos;
				if (W < -MAX_SAMPLE)
				    W = -MAX_SAMPLE;
				if (W > MAX_SAMPLE)
				    W = MAX_SAMPLE;

				if (j * sizeof(SAMPLE) >= len1
				    && pos2 != NULL
				    && k * sizeof(SAMPLE) < len2) {
				    curpos = pos2 + k;
				    k++;
				} else if (pos1 != NULL) {
				    curpos = pos1 + j;
				    j++;
				}
				*(SAMPLE *) curpos = W;
			    }

			    res =
				IDirectSoundBuffer_Unlock(dbuffer, pos1,
							  j *
							  sizeof(SAMPLE),
							  pos2,
							  k *
							  sizeof(SAMPLE));
			    if (res != DS_OK) {
				fprintf(stderr, "\nunlock:");
				switch (res) {
				case DS_OK:{
					fprintf(stderr, "DS_OK");
					break;
				    }
				case DSERR_INVALIDCALL:{
					fprintf(stderr,
						"DSERR_INVALIDCALL");
					break;
				    }
				case DSERR_INVALIDPARAM:{
					fprintf(stderr,
						"DSERR_INVALIDPARAM");
					break;
				    }
				case DSERR_PRIOLEVELNEEDED:{
					fprintf(stderr,
						"DSERR_PRIOLEVELNEEDED ");
				    }
				    break;
				}
			    }
			    /*
			     * Start DirectSound playback, if this is a first
			     * recorded buffer 
			     */
			    if (state == STATE_START) {
				res =
				    IDirectSoundBuffer_Play(dbuffer, 0, 0,
							DSBPLAY_LOOPING);
				if (res != DS_OK) {
				    fprintf(stderr,
					"\nCannot start playback via DirectSound: ");
				    switch (res) {
				    case DSERR_INVALIDCALL:{
					fprintf(stderr,
						"DSERR_INVALIDCALL");
					break;
				    }
				    case DSERR_INVALIDPARAM:{
					fprintf(stderr,
						"DSERR_INVALIDPARAM");
					break;
				    }
				    case DSERR_BUFFERLOST:{
					fprintf(stderr,
						"DSERR_BUFFERLOST");
					break;
				    }
				    case DSERR_PRIOLEVELNEEDED:{
					fprintf(stderr,
						"DSERR_PRIOLEVELNEEDED ");
					break;
				    }
				    }
				    state = STATE_EXIT;
				}
				state = STATE_PROCESS;
			    }
			}

			/*
			 * start playback - MME output 
			 */
			else {
			    for (i = 0; i < count; i++) {
				int             W = procbuf[i];
				if (W < -MAX_SAMPLE)
				    W = -MAX_SAMPLE;
				if (W > MAX_SAMPLE)
				    W = MAX_SAMPLE;
				((SAMPLE *) (write_header[hdr_avail].
					     lpData))[i] = W;
			    }

			    err =
				waveOutWrite(out, &write_header[hdr_avail],
					     sizeof(WAVEHDR));
			    if (err) {
				serror(err, "\nwriting samples - ");
			    } else
				active_out_buffers++;
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
		active_in_buffers++;
		if (active_in_buffers == 0 && state == STATE_PAUSE)
		    SetEvent(input_bufs_done);
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
		if (dsound)
		    break;
		((WAVEHDR *) msg.lParam)->dwFlags &= ~WHDR_DONE;
		for (i = 0; i < nbuffers; i++)
		    if (&write_header[i] == (WAVEHDR *) msg.lParam) {
			cur_wr_hdr[i] = 1;
			break;
		    }
		active_out_buffers++;
		if (active_out_buffers == 0 && state == STATE_PAUSE)
		    SetEvent(output_bufs_done);
		continue;
	    }
	default:
	    ;
	}
    }
    CloseHandle(audio_thread);
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
 * sound shutdown 
 */
void
close_sound(void)
{
    int             i;

    /*
     * pause sound 
     */
    state = STATE_PAUSE;

#ifndef _WIN32
    close(fd);
#else
    /*
     * Stop Windows queuing of buffers 
     */
    if (!dsound)
	waveOutReset(out);
    else if (dbuffer != NULL)
	IDirectSoundBuffer_Stop(dbuffer);
    waveInReset(in);

    /*
     * Wait until all output buffers return
     */
    if (!dsound)
	WaitForSingleObject(output_bufs_done, INFINITE);
    WaitForSingleObject(input_bufs_done, INFINITE);

    /*
     * Unprepare WAVE buffers 
     */
    for (i = 0; i < nbuffers; i++) {
	if (!dsound)
	    waveOutUnprepareHeader(out, &write_header[i], sizeof(WAVEHDR));
	waveInUnprepareHeader(in, &wave_header[i], sizeof(WAVEHDR));
    }

    /*
     * We should unprepare the read headers here,
     * but it is possible not to do it at all,
     * since we didn't use malloc()'s to allocate them
     */

    /*
     * Close WAVE devices
     */
    if (!dsound)
	waveOutClose(out);
    else {
	if(dbuffer)
	    IDirectSoundBuffer_Release(dbuffer);
	if(snd)
	    IDirectSound_Release(snd);
	dbuffer=NULL;
	snd=NULL;
    }
    waveInClose(in);
#endif
}

/*
 * sound initialization
 */
int
init_sound(void)
{
    int             i;

#ifndef _WIN32
    if ((fd = open("/dev/dsp", O_RDWR)) == -1) {
	fprintf(stderr, "\nCannot open audio device!");
	state = STATE_EXIT;
	return ERR_WAVEINOPEN;
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
    i = 0x7fff0000 + log2(buffer_size);
    if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &i) < 0) {
	fprintf(stderr, "\nCannot setup fragments!");
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVEFRAGMENT;
    }

    if (ioctl(fd, SNDCTL_DSP_GETCAPS, &i) == -1) {
	fprintf(stderr, "\nCannot get soundcard capabilities!");
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVEGETCAPS;
    }

    if (!(i & DSP_CAP_DUPLEX)) {
	fprintf(stderr,
		"\nSorry but your soundcard isn't full duplex capable!");
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVENOTDUPLEX;
    }

    if (ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1) {
	fprintf(stderr, "\nCannot setup fullduplex audio!");
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVEDUPLEX;
    }

    switch (bits) {
    case 16:{
	    i = AFMT_S16_LE;
	    break;
	}
    case 8:{
	    i = AFMT_S8;
	    break;
	}
    }
    if (ioctl(fd, SNDCTL_DSP_SETFMT, &i) == -1) {
	fprintf(stderr, "\nCannot setup %d bit audio!", bits);
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVESETBIT;
    }

    i = nchannels - 1;
    if (ioctl(fd, SNDCTL_DSP_STEREO, &i) == -1) {
	fprintf(stderr, "\nCannot setup mono audio!");
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVESETCHANNELS;
    }

    i = sample_rate;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &i) == -1) {
	fprintf(stderr, "\nCannot setup sampling frequency %dHz!", i);
	close(fd);
	state = STATE_EXIT;
	return ERR_WAVESETRATE;
    }
    state = STATE_PROCESS;
#else
    WAVEFORMATEX    format;	/* wave format */

    /*
     * set audio parameters - sampling rate, number of channels etc. 
     */
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = nchannels;
    format.nSamplesPerSec = sample_rate;
    format.wBitsPerSample = bits;
    format.nBlockAlign = format.nChannels * (format.wBitsPerSample / 8);
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize = 0;

    ZeroMemory(&wave_header[0], sizeof(WAVEHDR) * MAX_BUFFERS);
    ZeroMemory(&write_header[0], sizeof(WAVEHDR) * MAX_BUFFERS);

    nbuffers = MIN_BUFFER_SIZE * MAX_BUFFERS / buffer_size;
    if (nbuffers > MAX_BUFFERS)
	nbuffers = MAX_BUFFERS;
    /*
     * Open Digital Audio In device 
     */
    err =
	waveInOpen(&in, WAVE_MAPPER, &format, (DWORD) thread_id, 0,
		   CALLBACK_THREAD);
    if (err) {
	serror(err,
	       "There was an error opening the Digital Audio In device\r\n");
	state = STATE_EXIT;
	TerminateThread(audio_thread, ERR_WAVEINOPEN);
	return ERR_WAVEINOPEN;
    }

    /*
     * Open the Digital Audio Out device - MMSystem init
     */
    if (!dsound) {
	if ((err =
	     waveOutOpen(&out, WAVE_MAPPER, &format, (DWORD) thread_id, 0,
			 CALLBACK_THREAD))) {
	    serror(err,
		   "There was an error opening the Digital Audio Out device!\r\n");
	    state = STATE_EXIT;
	    waveInClose(in);
	    TerminateThread(audio_thread, ERR_WAVEOUTOPEN);
	    return ERR_WAVEOUTOPEN;
	}
	for (i = 0; i < nbuffers; i++) {
	    write_header[i].lpData = wrbuf + i * buffer_size;
	    /*
	     * Fill in WAVEHDR fields for buffer starting address and size.
	     * Leave other WAVEHDR fields at 0.
	     */
	    write_header[i].dwBufferLength = buffer_size;

	    /*
	     * Prepare the N WAVEHDR's 
	     */
	    if ((err =
		 waveOutPrepareHeader(out, &write_header[i],
				      sizeof(WAVEHDR)))) {
		fprintf(stderr, "ERROR: preparing WAVEHDR %d! -- %08X\n",
			i, err);
		state = STATE_EXIT;
		close_sound();
		TerminateThread(audio_thread, ERR_WAVEOUTHDR);
		return ERR_WAVEOUTHDR;
	    }
	    cur_wr_hdr[i] = 1;
	    active_out_buffers++;
	}
    }
    /*
     * DirectSound init 
     */
    else {
	DWORD           bufsize = MIN_BUFFER_SIZE * MAX_BUFFERS;
	DSBUFFERDESC    buffer_desc;
	HRESULT         res;
	HWND            window;

	ZeroMemory(&buffer_desc, sizeof(DSBUFFERDESC));
	buffer_desc.dwSize = sizeof(DSBUFFERDESC);
	buffer_desc.dwFlags = 0;
	buffer_desc.dwBufferBytes = bufsize;
	buffer_desc.lpwfxFormat = &format;

	/*
	 * open the DirectSound interface 
	 */
	if (DirectSoundCreate(NULL, &snd, NULL) != DS_OK) {
	    state = STATE_EXIT;
	    fprintf(stderr, "\nError creating DirectSound object !");
	    waveInClose(in);
	    TerminateThread(audio_thread, ERR_WAVEOUTOPEN);
	    return ERR_DSOUNDOPEN;
	}

	if (IDirectSound_CreateSoundBuffer(snd, &buffer_desc, &dbuffer, NULL) !=
	    DS_OK) {
	    state = STATE_EXIT;
	    close_sound();
	    fprintf(stderr, "\nError creating DirectSound buffer !");
	    TerminateThread(audio_thread, ERR_WAVEOUTHDR);
	    return ERR_DSOUNDBUFFER;
	}
	/*
	 * Try to set primary mixing privileges 
	 */
	window = GetActiveWindow();
	if (window != NULL) {
	    res = IDirectSound_SetCooperativeLevel(snd, window,
						   DSSCL_PRIORITY);
	    if (res != DS_OK) {
		state = STATE_EXIT;
		close_sound();
		fprintf(stderr,
			"\nError setting up the cooperative level: ");
		switch (res) {
		case DSERR_ALLOCATED:{
			fprintf(stderr, "DSERR_ALLOCATED");
			break;
		    }
		case DSERR_INVALIDPARAM:{
			fprintf(stderr, "DSERR_INVALIDPARAM");
			break;
		    }
		case DSERR_UNINITIALIZED:{
			fprintf(stderr, "DSERR_UNINITIALIZED");
		    }
		    break;
		case DSERR_UNSUPPORTED:{
			fprintf(stderr, "DSERR_UNSUPPORTED");
		    }
		    break;
		}
		TerminateThread(audio_thread, ERR_DSCOOPLEVEL);
		return ERR_DSCOOPLEVEL;
	    }
	}
    }

    for (i = 0; i < nbuffers; i++) {
	wave_header[i].dwBufferLength = buffer_size;
	wave_header[i].lpData = rdbuf + i * buffer_size;
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
	    state = STATE_EXIT;
	    close_sound();
	    TerminateThread(audio_thread, ERR_WAVEINHDR);
	    return ERR_WAVEINHDR;
	}
	/*
	 * Queue WAVEHDR (recording hasn't started yet) 
	 */
	if ((err = waveInAddBuffer(in, &wave_header[i], sizeof(WAVEHDR)))) {
	    serror(err, "Error queueing WAVEHDR!\n");
	    state = STATE_EXIT;
	    close_sound();
	    TerminateThread(audio_thread, ERR_WAVEINQUEUE);
	    return ERR_WAVEINQUEUE;
	}
	active_in_buffers++;
    }
    /*
     * Start recording. Our secondary thread will now be receiving
     * and processing audio data
     */
    if ((err = waveInStart(in))) {
	serror(err, "Error starting record!\n");
	state = STATE_EXIT;
	close_sound();
	TerminateThread(audio_thread, ERR_WAVEINRECORD);
	return ERR_WAVEINRECORD;
    }
    /*
     * start DirectSound playback 
     */
    if (dsound && state != STATE_START_PAUSE) {
/*
	HRESULT res;
	res=IDirectSoundBuffer_Play(dbuffer,0,0,DSBPLAY_LOOPING);
	if(res!=DS_OK) {
	    state = STATE_EXIT;
	    close_sound();
	    fprintf(stderr,"\nCannot start playback via DirectSound !");
	    TerminateThread(audio_thread, ERR_WAVEINRECORD);
	    return ERR_DSOUNDPLAYBACK;
	}
*/
    }
    if (dsound) {
	if (state != STATE_START_PAUSE)
	    state = STATE_START;
    } else
	state = STATE_PROCESS;
#endif

    return ERR_NOERROR;
}


#ifdef DEMO
#   ifdef _WIN32
VOID            CALLBACK
expired(HWND hwnd, UINT msg, UINT timer_id, DWORD time)
{
    gtk_main_quit();
    close_sound();
    printf("%s", DEMO_MSG);
    exit(ERR_DEMOEXPIRED);
}

#   else
#   endif
#endif

int
main(int argc, char **argv)
{
    int             error = 0;
#ifndef _WIN32
    int             max_priority;
    struct sched_param p;


    if ((error = init_sound()) != ERR_NOERROR)
	return error;

    max_priority = sched_get_priority_max(SCHED_FIFO);
    p.sched_priority = max_priority;

    if (sched_setscheduler(0, SCHED_FIFO, &p)) {
	printf
	    ("\nFailed to set scheduler priority. (Are you running as root?)");
	printf("\nContinuing with default priority");
    }
    if (pthread_create(&audio_thread, NULL, audio_thread_start, NULL)) {
	fprintf(stderr, "\nAudio thread creation failed!");
	return ERR_THREAD;
    }
    /*
     * We were running to this point as setuid root program.
     * Switching to our native user id
     */
    setuid(getuid());
#else

#ifdef DEMO
    /*
     * For demo version, we exit after a random time, 10 up to 15 minutes
     */
    srand(time(NULL));
    SetTimer(GetActiveWindow(), DEMO_TIMER,
	     (unsigned int) (1000 * 60 * 10 +
			     1000 * 60 * 5 * rand() / (RAND_MAX + 1.0)),
	     expired);
#endif

    /*
     * we are NOT in record now - signal to the audio thread 
     */
    state = STATE_START_PAUSE;

    /*
     * set high priority to the process 
     */
/*
    if(!SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS)) {
	fprintf(stderr,"\nFailed to set realtime priority to process:%s.",GetLastError());
    }
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
	return (ERR_THREAD);
    }

    if ((error = init_sound()) != ERR_NOERROR)
	return error;

    /*
     * set realtime priority to the thread 
     */
    if (!SetThreadPriority(audio_thread, THREAD_PRIORITY_TIME_CRITICAL)) {
	fprintf(stderr,
		"\nFailed to set realtime priority to thread: %s. Continuing with default priority.",
		GetLastError());
    }
#endif

    printf("\n\n" COPYRIGHT
	   "This program is a free software and distributed under GPL license;\n"
	   "see Help->About for details.\n");

    gtk_init(&argc, &argv);
    init_gui();
#ifdef _WIN32
    if (dsound) {
	HRESULT         res;

	res = IDirectSound_SetCooperativeLevel(snd, GetActiveWindow(),
					       DSSCL_PRIORITY);
	if (res != DS_OK) {
	    state = STATE_EXIT;
	    close_sound();
	    fprintf(stderr, "\nError setting up the cooperative level: ");
	    switch (res) {
	    case DSERR_ALLOCATED:{
		    fprintf(stderr, "DSERR_ALLOCATED");
		    break;
		}
	    case DSERR_INVALIDPARAM:{
		    fprintf(stderr, "DSERR_INVALIDPARAM");
		    break;
		}
	    case DSERR_UNINITIALIZED:{
		    fprintf(stderr, "DSERR_UNINITIALIZED");
		}
		break;
	    case DSERR_UNSUPPORTED:{
		    fprintf(stderr, "DSERR_UNSUPPORTED");
		}
		break;
	    }
	    TerminateThread(audio_thread, ERR_DSCOOPLEVEL);
	}

	state = STATE_START;
    }
#endif
    pump_start(argc, argv);
    gtk_main();

    state = STATE_EXIT;
    pump_stop();
    tracker_done();
    close_sound();

    return ERR_NOERROR;
}
