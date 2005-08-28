/*
 * GNUitar
 * Windows sound driver
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
 * Revision 1.6  2005/08/28 12:39:01  alankila
 * - make audio_lock a real mutex
 * - fix mutex cleanup at exit
 *
 * Revision 1.5  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.4  2005/08/26 16:00:36  fonin
 * Fixed error with wrong uppercase audio_thread identifier
 *
 * Revision 1.3  2005/08/25 19:51:45  fonin
 * Fixed windows audio driver
 *
 * Revision 1.2  2005/08/24 21:55:05  alankila
 * slight bit likelier to compile
 *
 * Revision 1.1  2005/08/24 21:44:44  alankila
 * - split sound drivers off main.c
 * - add support for alsa
 * - rework thread locking
 * - in this version, sound drivers are chosen at compile time
 * - windows driver is probably broken
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#include <dsound.h>
#include <fcntl.h>
#include <sys/types.h>

#include "pump.h"
#include "main.h"
#include "tracker.h"
#include "gui.h"
#include "utils.h"

HANDLE          input_bufs_done,
                output_bufs_done;
DWORD           thread_id;

LPDIRECTSOUND   snd = NULL;	/* DirectSound object */
LPDIRECTSOUNDBUFFER dbuffer = NULL;	/* DS buffer */
short           dsound = 0;	/* flag - do we use DirectSound for output ? */
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
DSP_SAMPLE      procbuf[MAX_BUFFER_SIZE];	/* procesing buffer */
int             active_in_buffers = 0,
                active_out_buffers = 0;

void            serror(DWORD err, TCHAR * str);

DWORD           WINAPI
windows_audio_thread(void *V)
{
    int             count, i, j, k;
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
    struct data_block db;

    /*
     * Wait for a message sent to me by the audio driver
     */
    while (state != STATE_EXIT && state != STATE_ATHREAD_RESTART) {
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
		if (count) {
		    count /= bits >> 3;
			//WAVEHDR *twh;
			//twh = (WAVEHDR*) msg.lParam;
		    for (i = 0; i < count; i++) {
			procbuf[i] =
			   ((SAMPLE *) (((WAVEHDR *) msg.lParam)->
					 lpData))[i] << 8;
				//procbuf[i] = ((SAMPLE*) twh->lpData)[i];
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
                        db.data = procbuf;
                        db.len = count;
                        db.channels = nchannels;
			pump_sample(&db);

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
				DSP_SAMPLE      W = procbuf[i] >> 8;
				SAMPLE         *curpos;

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
				DSP_SAMPLE W = procbuf[i] >> 8;
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
}

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

/*
 * sound shutdown
 */
void
windows_finish_sound(void)
{
    int             i;

    state = STATE_PAUSE;

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
}

/*
 * sound initialization
 */
int
windows_init_sound(void)
{
    int             i;
    WAVEFORMATEX    format;	/* wave format */

    /*
     * set audio parameters - sampling rate, number of channels etc.
     */
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = nchannels;
    format.nSamplesPerSec = sample_rate;
    format.wBitsPerSample = bits;
    format.nBlockAlign = format.nChannels * (format.wBitsPerSample >> 3);
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
		windows_finish_sound();
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
	    windows_finish_sound();
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
		windows_finish_sound();
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
	    windows_finish_sound();
	    TerminateThread(audio_thread, ERR_WAVEINHDR);
	    return ERR_WAVEINHDR;
	}
	/*
	 * Queue WAVEHDR (recording hasn't started yet)
	 */
	if ((err = waveInAddBuffer(in, &wave_header[i], sizeof(WAVEHDR)))) {
	    serror(err, "Error queueing WAVEHDR!\n");
	    state = STATE_EXIT;
	    windows_finish_sound();
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
	windows_finish_sound();
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
	    windows_finish_sound();
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

    return ERR_NOERROR;
}

