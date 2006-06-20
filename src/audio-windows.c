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
 *
 * $Id$
 *
 * $Log$
 * Revision 1.15  2006/06/20 20:41:06  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.14  2006/05/31 13:56:36  fonin
 * GCC-style typedef'd structure init does not work in MSVC6.0; also few #includes for sanity
 *
 * Revision 1.13  2006/05/20 09:56:58  alankila
 * - move audio_driver_str and audio_driver_enabled into driver structure
 * - Win32 drivers are ugly, with the need to differentiate between
 *   DirectX and MMSystem operation through dsound variable. The driver
 *   should probably be split with dsound-specific parts in its own driver.
 *
 * Revision 1.12  2006/05/20 08:01:22  alankila
 * - patch Windows also to use the 16-bit version of sample buffer
 *
 * Revision 1.11  2005/09/28 19:51:27  fonin
 * - Rewritten Windows audio driver, in particular -
 *   DirectSound part.
 *
 * Revision 1.10  2005/09/04 16:06:59  alankila
 * - first multichannel effect: delay
 * - need to use surround40 driver in alsa
 * - introduce new buffer data_swap so that effects need not reserve buffers
 * - correct off-by-one error in multichannel adapting
 *
 * Revision 1.9  2005/09/03 22:13:56  alankila
 * - make multichannel processing selectable
 * - new GUI (it sucks as much as the old one and I'll need to grok GTK
 *   tables first before it gets better)
 * - make pump.c do the multichannel adapting bits
 * - effects can now change channel counts
 *
 * Revision 1.8  2005/09/03 20:20:42  alankila
 * - create audio_driver type and write all the driver stuff into it. This
 *   faciliates carrying configuration data about the capabilities of
 *   a specific audio driver and uses less global variables.
 *
 * Revision 1.7  2005/08/28 21:41:51  fonin
 * Fixed mutex locking
 *
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
#include <fcntl.h>
#include <sys/types.h>
#include <dsound.h>

#include "pump.h"
#include "main.h"
#include "tracker.h"
#include "gui.h"
#include "utils.h"
#include "audio-windows.h"


HANDLE          input_bufs_done,
                output_bufs_done;
DWORD           thread_id;

LPDIRECTSOUND   snd = NULL;	        /* DirectSound rendering object */
LPDIRECTSOUNDCAPTURE capture = NULL;    /* DirectSound capture object */
LPDIRECTSOUNDBUFFER pbuffer = NULL;	/* DS rendering buffer */
LPDIRECTSOUNDCAPTUREBUFFER cbuffer = NULL;  /* DS capture buffer */
HANDLE          notify_event;           /* for DS notify events */
DSBPOSITIONNOTIFY   notify_handlers[MAX_BUFFERS+1];
LPDIRECTSOUNDNOTIFY notify;
DWORD           bufsize = MIN_BUFFER_SIZE * MAX_BUFFERS;

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
int             active_in_buffers = 0,
                active_out_buffers = 0;

void            serror(DWORD err, TCHAR * str);
void            dserror(HRESULT res, char *s);

DWORD           WINAPI
windows_audio_thread(void *V)
{
    int             count, i, j, k, old_count = 0;
    MSG             msg;
    HRESULT         res;
    DWORD           write_pos = 0,
                    read_pos = 0,
                    len1 = 0,
                    len2 = 0;
    DWORD           event = 0;  /* for WaitForMultipleObjects() */

    /*
     * read/write cursors and lengths for DS calls
     */
    SAMPLE16       *pos1 = NULL,
                   *pos2 = NULL;	/* pointers for DirectSound lock
					 * call */
    static unsigned int wbufpos = 0,	/* current write position in the */
			rbufpos = 0;	/* buffer (DirectSound) */
    struct data_block db;

    /*
     * Wait for a message sent to me by the audio driver
     */
    while (state != STATE_EXIT && state != STATE_ATHREAD_RESTART) {
        while (state == STATE_PAUSE || state == STATE_START_PAUSE) {
	    Sleep(10);
	}
        if (!dsound)
            if(!GetMessage(&msg, 0, 0, 0)) {
                return 0;
	    }
        my_lock_mutex(snd_open);
        /* catch transition PAUSE -> EXIT with mutex being waited already */
        if (state == STATE_EXIT || state == STATE_ATHREAD_RESTART) {
            my_unlock_mutex(snd_open);
            break;
        }


        if(dsound) {
            /*
	     * DirectSound capture:
             * 1) if this is START state, init the capture
	     * 2) get current read/write buffer cursors
	     * 3) lock buffer
	     * 4) copy buffer to the processing structure buffer
	     * 5) unlock buffer
	     */

       	    /*
	     * Start DirectSound capture, if this is a first
	     * recorded buffer
	     */
	    if (state == STATE_START) {
		res = IDirectSoundCaptureBuffer_Start(cbuffer, DSCBSTART_LOOPING);
		if (res != DS_OK) {
		    dserror(res,"\nCannot start capture via DirectSound: ");
		    state = STATE_EXIT;
		}

	        /*
	         * Start DirectSound playback, if this is a first
	         * recorded buffer
	         */
                res = IDirectSoundBuffer_Play(pbuffer, 0, 0, DSBPLAY_LOOPING);
		if (res != DS_OK) {
		    dserror(res,"\nCannot start playback via DirectSound: ");
		    state = STATE_EXIT;
		}

                /* must wait here until the buffer gets filled */
                for(i=0;i<overrun_threshold-1;i++) {
                    WaitForSingleObject(notify_event,INFINITE);
                    ResetEvent(notify_event);
                    if(i==0)
                        IDirectSoundCaptureBuffer_GetCurrentPosition(cbuffer, NULL, &rbufpos);
                }
            }

            res = IDirectSoundCaptureBuffer_GetCurrentPosition(cbuffer, &write_pos, &read_pos);
            if(write_pos>rbufpos+buffer_size && write_pos-rbufpos<buffer_size*2) {
//                fprintf(stderr,"\ncapture buffer underrun: rbufpos=%i, read_pos=%i,capture_pos=%i,delta=%i",rbufpos,read_pos,write_pos,write_pos-read_pos);
                goto END_LOOP;
            }
            /*
	     * If this is a start state, buffer position
	     * is equal to the read position
	     * returned by the call above
	     */
            /* no bytes read since the last time; skip the cycle */
            else if(rbufpos == read_pos) {
                goto END_LOOP;
            }
	    /*
	     * otherwise we just increment buffer position
	     * by the fragment size
	     */
	    else {
		/* workaround buffer overrun */
		if(rbufpos<read_pos &&
		        /* this condition handles buffer wrap around */
		        read_pos-rbufpos > buffer_size*overrun_threshold &&
		        read_pos-rbufpos < buffer_size*nbuffers/2) {
		    gnuitar_printf("\ncapture buffer overrun: real position=%u, calculated=%u",read_pos,rbufpos);
		    rbufpos=read_pos;
		}
		else rbufpos += buffer_size;
	    }

            /*
	     * handle wrap around
	     */
            if (rbufpos >= bufsize) {
                rbufpos-=bufsize;
            }
	    if (res == DS_OK) {
		res = IDirectSoundCaptureBuffer_Lock(cbuffer, rbufpos, buffer_size,
							      &pos1, &len1,
							      &pos2, &len2,
							      0);

		if (res != DS_OK) {
	    	    dserror(res, "\nCannot lock buffer: ");
	    	    state = STATE_EXIT;
        	}
	    }

            /* bytes read */
            old_count=count;
            count = len1 + len2;

            /* copying the data block */
            for (i = 0, j = 0, k = 0; i < count / sizeof(SAMPLE16); i++) {
		SAMPLE16       *curpos;

		if (j * sizeof(SAMPLE16) >= len1 && pos2 != NULL && k * sizeof(SAMPLE16) < len2) {
                    curpos = pos2 + k;
		    k++;
		} else if (pos1 != NULL) {
		    curpos = pos1 + j;
		    j++;
		}
                procbuf[i]= (*(SAMPLE16 *) curpos) << 8;
            }
	    res = IDirectSoundCaptureBuffer_Unlock(cbuffer, pos1, /*j * sizeof(SAMPLE16)*/len1, pos2,
							   /*k * sizeof(SAMPLE16)*/len2);
	    if (res != DS_OK)
		dserror(res, "\nunlock:");


            /* process the sound */
            db.data = procbuf;
            db.data_swap = procbuf2;
            db.len = count/sizeof(SAMPLE16);
            db.channels = n_input_channels;
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
	    res = IDirectSoundBuffer_GetCurrentPosition(pbuffer, &read_pos, &write_pos);
            /*
	     * If this is a start state, buffer position
	     * is equal to the write position
	     * returned by the call above
	     */
	    if (state == STATE_START) {
		wbufpos = write_pos;//+buffer_size;
	    }
	    /*
	     * otherwise we just increment buffer position
	     * by the fragment size
	     */
	    else {
		/* workaround buffer overrun */
		if(wbufpos<write_pos &&
		        /* this condition handles buffer wrap around */
		        abs(write_pos-wbufpos)>buffer_size*overrun_threshold &&
		        abs(write_pos-wbufpos)<buffer_size*100) {
		    gnuitar_printf("\nplayback buffer overrun: real position=%u, calculated=%u",
                            write_pos,wbufpos);
		    wbufpos=write_pos+buffer_size;
		}
                else wbufpos += (old_count?old_count:buffer_size);
	    }
	    /*
	     * handle wrap around
	     */
            if (wbufpos >= MIN_BUFFER_SIZE * MAX_BUFFERS) {//bufsize) {
                if(abs(write_pos-wbufpos)>buffer_size*100)
                    wbufpos=write_pos;
                else wbufpos-=MIN_BUFFER_SIZE * MAX_BUFFERS;
            }

            if (res == DS_OK) {
		res = IDirectSoundBuffer_Lock(pbuffer, wbufpos, count,
							      &pos1, &len1,
							      &pos2, &len2,
							      0);
                if (res != DS_OK) {
	    	    dserror(res, "\nCannot lock buffer: ");
	    	    state = STATE_EXIT;
        	}
	    }
	    for (i = 0, j = 0, k = 0; i < count / sizeof(SAMPLE16); i++) {
		DSP_SAMPLE      W = (SAMPLE32)db.data[i] >> 8;
		SAMPLE16       *curpos;

		if (j * sizeof(SAMPLE16) >= len1 && pos2 != NULL && k * sizeof(SAMPLE16) < len2) {
		    curpos = pos2 + k;
		    k++;
		} else if (pos1 != NULL) {
		    curpos = pos1 + j;
		    j++;
		}
		*(SAMPLE16 *) curpos = W;
	    }

	    res = IDirectSoundBuffer_Unlock(pbuffer, pos1, len1, pos2, len2);
	    if (res != DS_OK)
		dserror(res, "\nunlock:");

            if (state == STATE_START)
                state=STATE_PROCESS;
        }


	/*
	 * Figure out which message was sent
	 */
	else switch (msg.message) {
	case WM_QUIT:{
		return 0;
	    }
	    /*
	     * A buffer has been filled by the driver
	     */
	case MM_WIM_DATA:{
            if(!dsound) {
                int             hdr_avail = -1;	/* available write header
						 * index */
		active_in_buffers--;
		if (state == STATE_PAUSE || state == STATE_START_PAUSE) {
                    my_unlock_mutex(snd_open);
		    continue;
		}

		count = ((WAVEHDR *) msg.lParam)->dwBytesRecorded;
		if (count) {
		    count /= bits >> 3;
			//WAVEHDR *twh;
			//twh = (WAVEHDR*) msg.lParam;
		    for (i = 0; i < count; i++) {
			procbuf[i] =
			   ((SAMPLE16 *) (((WAVEHDR *) msg.lParam)->lpData))[i] << 8;
		    }


		    /*
		     * find unused output buffer and queue it to output
		     */
		    for (i = 0; i < nbuffers; i++)
			if (cur_wr_hdr[i] == 1) {
			    hdr_avail = i;
			    cur_wr_hdr[i] = 0;	/* ready to queue */
			    break;
			}

		    if (hdr_avail != -1) {
                        db.data = procbuf;
                        db.data_swap = procbuf2;
                        db.len = count;
                        db.channels = n_input_channels;
			pump_sample(&db);


			/*
			 * start playback - MME output
			 */
			for (i = 0; i < count; i++) {
			    DSP_SAMPLE W = (SAMPLE32)db.data[i] >> 8;
			    ((SAMPLE16 *) (write_header[hdr_avail].lpData))[i] = W;
			}

			err = waveOutWrite(out, &write_header[hdr_avail],sizeof(WAVEHDR));
			if (err) {
			    serror(err, "\nwriting samples - ");
			} else
			    active_out_buffers++;
		    } else
		        gnuitar_printf("\nbuffer overrun.");
	        } else
	            gnuitar_printf("\nbuffer underrun.");

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
                my_unlock_mutex(snd_open);
		continue;
            }
	}
	/*
	 * Our main thread is opening the WAVE device
	 */
	case MM_WIM_OPEN:{
                my_unlock_mutex(snd_open);
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
		for (i = 0; i < nbuffers; i++)
		    if (&write_header[i] == (WAVEHDR *) msg.lParam) {
			cur_wr_hdr[i] = 1;
			break;
		    }
		active_out_buffers++;
		if (active_out_buffers == 0 && state == STATE_PAUSE)
		    SetEvent(output_bufs_done);
                my_unlock_mutex(snd_open);
		continue;
	    }
	default:
	    ;
        }

END_LOOP:
        my_unlock_mutex(snd_open);
        /* now let's freeze the thread until the next data block is read */
        if(dsound) {
            event=WaitForSingleObject(notify_event,INFINITE);
            ResetEvent(notify_event);
        }
    }
    CloseHandle(audio_thread);
    return 0;
}


/*
 * sound shutdown
 */
void
windows_finish_sound(void)
{
    int             i;

    state = STATE_PAUSE;
    my_lock_mutex(snd_open);
    windows_driver.enabled = 0;
    SuspendThread(audio_thread);

    if(!dsound) {
        /*
         * Stop Windows queuing of buffers
         */
    	waveOutReset(out);
        waveInReset(in);
        /*
         * Wait until all output buffers return
         */
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
	waveOutClose(out);
        waveInClose(in);
    }

    if(dsound) {
        if (cbuffer)
	    IDirectSoundCaptureBuffer_Stop(cbuffer);
        if (pbuffer)
	    IDirectSoundBuffer_Stop(pbuffer);
        if(cbuffer)
	    IDirectSoundCaptureBuffer_Release(cbuffer);
	if(capture)
	    IDirectSoundCapture_Release(capture);
        if(pbuffer)
	    IDirectSoundBuffer_Release(pbuffer);
	if(snd)
	    IDirectSound_Release(snd);
	cbuffer=NULL;
	capture=NULL;
        notify=NULL;
	pbuffer=NULL;
	snd=NULL;
    }
}

/*
 * sound initialization
 */
int
windows_init_sound(void)
{
    int             i;
    WAVEFORMATEX    format;	/* wave format */
    SAMPLE16       *rdbuf16 = rdbuf;
    SAMPLE16       *wrbuf16 = wrbuf;

    /*
     * set audio parameters - sampling rate, number of channels etc.
     */
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = n_input_channels;
    n_output_channels = n_input_channels;
    format.nSamplesPerSec = sample_rate;
    format.wBitsPerSample = bits;
    format.nBlockAlign = format.nChannels * (format.wBitsPerSample >> 3);
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize = 0;

    /*
     * Open the Digital Audio Out device - MMSystem init
     */
    if (!dsound) {

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
	    write_header[i].lpData = wrbuf16 + i * buffer_size;
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
		gnuitar_printf( "ERROR: preparing WAVEHDR %d! -- %08X\n",
			i, err);
		state = STATE_EXIT;
		windows_finish_sound();
		TerminateThread(audio_thread, ERR_WAVEOUTHDR);
		return ERR_WAVEOUTHDR;
	    }
	    cur_wr_hdr[i] = 1;
	    active_out_buffers++;
	}

        for (i = 0; i < nbuffers; i++) {
	    wave_header[i].dwBufferLength = buffer_size;
	    wave_header[i].lpData = rdbuf16 + i * buffer_size;

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
	state = STATE_PROCESS;
    }
    /*
     * DirectSound init
     */
    else {
	DSBUFFERDESC    buffer_desc;
        DSCBUFFERDESC   capture_desc;
	HRESULT         res;
	HWND            window;
        DWORD           old_read_pos=0,     /* old capture position, for bufsize probe */
                        read_pos=0,         /* current capture position, for bufsize probe */
                        nreads=0,           /* number of reads that succeed, for bufsize probe */
                        nattempts=0,        /* number of times we actually attempted to probe */
                        probed_bufsize=0;   /* probed buffer size */
        const DWORD     MAX_PROBE_ATTEMPTS=1;/* max number of probe attempts, for bailout */


        /* Rendering device; initialize playback after the buffer size probe,
         * to make sure the capture and playback buffers are the same size */
        ZeroMemory(&buffer_desc, sizeof(DSBUFFERDESC));
	buffer_desc.dwSize = sizeof(DSBUFFERDESC);
	buffer_desc.dwFlags = 0;
	buffer_desc.dwBufferBytes = MIN_BUFFER_SIZE * MAX_BUFFERS;
	buffer_desc.lpwfxFormat = &format;

	/*
	 * open the rendering DirectSound interface
	 */
	if (DirectSoundCreate(NULL, &snd, NULL) != DS_OK) {
	    state = STATE_EXIT;
	    gnuitar_printf( "\nError opening DirectSound rendering device !");
	    TerminateThread(audio_thread, ERR_WAVEOUTOPEN);
	    return ERR_DSOUNDOPEN;
	}

	if (IDirectSound_CreateSoundBuffer(snd, &buffer_desc, &pbuffer, NULL) != DS_OK) {
	    state = STATE_EXIT;
	    windows_finish_sound();
	    gnuitar_printf( "\nError creating DirectSound rendering buffer !");
	    TerminateThread(audio_thread, ERR_WAVEOUTHDR);
	    return ERR_DSOUNDBUFFER;
	}

        /* set notifier for the capture buffer */
        /* This event means - we reached the next position in the capture buffer */
        notify_event=CreateEvent(NULL,FALSE,FALSE,NULL);

        nbuffers=bufsize/buffer_size;
        while(nattempts++ <= MAX_PROBE_ATTEMPTS) {
            int total_read=0;   /* for buffer size probe; total read bytes */
            read_pos=old_read_pos=nreads=0;

            /*
	     * open the capture DirectSound interface
	     */
	    ZeroMemory(&capture_desc, sizeof(DSCBUFFERDESC));
	    capture_desc.dwSize = sizeof(DSCBUFFERDESC);
	    capture_desc.dwFlags = 0;
	    capture_desc.dwBufferBytes = bufsize;
	    capture_desc.lpwfxFormat = &format;
	    if ((res=DirectSoundCaptureCreate(NULL, &capture, NULL)) != DS_OK) {
	        state = STATE_EXIT;
	        dserror(res, "\nError opening DirectSound capture object !");
	        TerminateThread(audio_thread, ERR_WAVEOUTOPEN);
	        return ERR_DSOUNDOPEN;
	    }

	    if ((res=IDirectSoundCapture_CreateCaptureBuffer(capture, &capture_desc, &cbuffer, NULL)) !=
	                                                                                DS_OK) {
	        state = STATE_EXIT;
	        windows_finish_sound();
	        dserror(res, "\nError creating DirectSound capture buffer !");
	        TerminateThread(audio_thread, ERR_WAVEOUTHDR);
	        return ERR_DSOUNDBUFFER;
	    }
            if(res=IDirectSoundCaptureBuffer_QueryInterface(cbuffer,&IID_IDirectSoundNotify,(LPVOID*)&notify)!=S_OK) {
                state = STATE_EXIT;
	        windows_finish_sound();
	        gnuitar_printf( "\nError creating DirectSound notifier !");
	        TerminateThread(audio_thread, ERR_WAVEOUTHDR);
	        return ERR_DSOUNDBUFFER;
            }

            ZeroMemory(notify_handlers,sizeof(notify_handlers));
            for(i=0;i<nbuffers;i++) {
                notify_handlers[i].dwOffset=buffer_size*(i+1)-1;
                notify_handlers[i].hEventNotify=notify_event;
            }
            IDirectSoundNotify_SetNotificationPositions(notify,nbuffers,notify_handlers);

            /* probe for buffer size. Somehow, my DirectSound ignores my notification
             * positions and always notify me after 882 bytes read. */
            res = IDirectSoundCaptureBuffer_Start(cbuffer, DSCBSTART_LOOPING);
            if (res != DS_OK)
		dserror(res,"\nProbe: cannot start capture via DirectSound.");
            for(i=0;i<5;i++) {
                /* must wait here until the buffer gets filled */
                WaitForSingleObject(notify_event,INFINITE);
                old_read_pos=read_pos;
                res = IDirectSoundCaptureBuffer_GetCurrentPosition(cbuffer, NULL, &read_pos);
                if(res!=DS_OK)
                    dserror(res,"\nError getting capture position via DirectSound.");
                else if(old_read_pos && read_pos>old_read_pos) {
                    total_read+=read_pos-old_read_pos;
                    nreads++;
                }
            }
            if(nreads) {
                probed_bufsize=buffer_size=total_read/nreads;
                nbuffers=MIN_BUFFER_SIZE * MAX_BUFFERS / buffer_size;
                bufsize=buffer_size*nbuffers;
            }
            /* destroy capture buffer to adjust the bufsize for the new attempt */
            if(nattempts<MAX_PROBE_ATTEMPTS) {
                if((res=IDirectSoundCaptureBuffer_Stop(cbuffer))!=DS_OK)
                    dserror(res,"\nProbe: error stopping DirectSound capture.");
                if((res=IDirectSoundCaptureBuffer_Release(cbuffer))!=DS_OK)
                    dserror(res,"\nProbe: error destroying DirectSound capture buffer.");
            }
            if((res=IDirectSoundNotify_Release(notify))!=DS_OK)
                dserror(res,"\nProbe: error destroying DirectSound notifier.");
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
		dserror(res,
			"\nError setting up the cooperative level: ");
		TerminateThread(audio_thread, ERR_DSCOOPLEVEL);
		return ERR_DSCOOPLEVEL;
	    }
	}
	if (state != STATE_START_PAUSE)
	    state = STATE_START;
    }

    windows_driver.enabled = 1;
    my_unlock_mutex(snd_open);
    return ERR_NOERROR;
}

/*
 * Retrieves and displays an error message for the passed Wave In error
 * number. It does this using mciGetErrorString().
 */
void
serror(DWORD err, TCHAR * str)
{
    char            buffer[128];

    gnuitar_printf( "ERROR 0x%08X: %s", err, str);
    if (mciGetErrorString(err, &buffer[0], sizeof(buffer))) {
	gnuitar_printf( "%s\r\n", &buffer[0]);
    } else {
	gnuitar_printf( "0x%08X returned!\r\n", err);
    }
}

/* DirectSound Error */
void dserror(HRESULT res, char *s) {
    gnuitar_printf(s);
    switch (res) {
	case DSERR_ALLOCATED:{
	    gnuitar_printf( "DSERR_ALLOCATED");
	    break;
	}
	case DSERR_INVALIDPARAM:{
	    gnuitar_printf( "DSERR_INVALIDPARAM");
	    break;
	}
	case DSERR_UNINITIALIZED:{
	    gnuitar_printf( "DSERR_UNINITIALIZED");
	    break;
        }
        case DSERR_UNSUPPORTED:{
	    gnuitar_printf( "DSERR_UNSUPPORTED");
	    break;
        }
        case DSERR_INVALIDCALL:{
            gnuitar_printf("DSERR_INVALIDCALL");
            break;
        }
        case DSERR_BUFFERLOST:{
            gnuitar_printf("DSERR_BUFFERLOST");
            break;
	}
        case DSERR_PRIOLEVELNEEDED:{
            gnuitar_printf("DSERR_PRIOLEVELNEEDED ");
            break;
	}
    }
}

struct audio_driver_channels windows_channels_cfg[]={
    { 1, 1 },
    { 2, 2 },
    { 0, 0 }
};
int windows_bits_cfg[1] = { 16 };

audio_driver_t windows_driver = {
    "Windows",              /* str */
    0,                      /* enabled */
    windows_channels_cfg,   /* channels */
    windows_bits_cfg,       /* bits */
    windows_init_sound,     /* init */
    windows_finish_sound,   /* finish */
    windows_audio_thread    /* audio_proc */
};
