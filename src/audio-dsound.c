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
 * Revision 1.7  2006/08/08 21:21:53  alankila
 * - skip dithering for now on this driver
 *
 * Revision 1.6  2006/08/07 23:52:51  alankila
 * - fix priority level setting
 *
 * Revision 1.5  2006/08/07 23:39:55  alankila
 * - the repeated reconfigurings of directsound were a problem at least for
 *   me. It now produces some kind of buggy, snapping playback.
 *
 * Revision 1.4  2006/08/07 22:06:27  alankila
 * - make win32 compile again.
 * - utils.h now loads math.h also for win side; makes sense with mingw
 *   at least.
 *
 * Revision 1.3  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.2  2006/07/27 19:15:35  alankila
 * - split windows driver architecture now compiles and runs.
 *
 * Revision 1.1  2006/07/27 18:31:15  alankila
 * - split dsound and winmm into separate drivers.
 *
 * Revision 1.24  2006/07/26 23:09:09  alankila
 * - DirectSound may be buggy; MMSystem at least worked in mingw build.
 * - remove some sound-specific special cases in gui and main code.
 * - create thread in windows driver.
 * - remove all traces of "program states" variable.
 * - remove snd_open mutex: it is now unnecessary. Concurrency is handled
 *   through joining/waiting threads where necessary. (We assume JACK
 *   does its own locking, though.)
 *
 * Revision 1.23  2006/07/26 21:08:43  alankila
 * - it is illegal to use gnuitar_printf from audio thread; this corrects
 *   crashes in stopping and restarting MMSystem and DirectSound.
 *
 * Revision 1.22  2006/07/26 21:03:21  alankila
 * - correct various type errors in the code, although some errors remain.
 *
 * Revision 1.21  2006/07/25 23:41:14  alankila
 * - this patch may break win32. I can't test it.
 *   - move audio_thread handling code into sound driver init/finish
 *   - remove state variable from sight of the Linux code -- it should be
 *     killed also on Win32 side using similar strategies
 *   - snd_open mutex starts to look spurious. It can probably be removed.
 *
 * Revision 1.20  2006/07/25 22:49:04  alankila
 * - balance commented { in order to make vim brace matching work
 *
 * Revision 1.19  2006/07/17 21:39:38  alankila
 * - use dynamically allocated sample buffers instead of static ones.
 *   (Win32 still uses static buffers moved directly into audio-windows.c)
 *
 * Revision 1.18  2006/07/17 11:21:40  alankila
 * - add a _WIN32 define protecting compile for simplifying cmake build
 *
 * Revision 1.17  2006/07/15 23:02:45  alankila
 * - remove the bits control -- just use the best available on every driver.
 *
 * Revision 1.16  2006/07/15 21:15:47  alankila
 * - implement triangular dithering on the sound drivers. Triangular dithering
 *   places more noise at the nyquist frequency so the noise floor is made
 *   smaller elsewhere.
 *
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

#ifdef HAVE_DSOUND

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <windows.h>
#include <process.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dsound.h>

#include "audio-dsound.h"
#include "main.h"
#include "pump.h"
#include "gui.h"
#include "utils.h"

static volatile int keepthreadrunning = 0, firstbuffer = 0;
static HANDLE       audio_thread = 0;
static DWORD        thread_id = 0;

static LPDIRECTSOUND	    snd = NULL;		    /* DS rendering object */
static LPDIRECTSOUNDCAPTURE capture = NULL;	    /* DS capture object */
static LPDIRECTSOUNDBUFFER  pbuffer = NULL;	    /* DS rendering buffer */
static LPDIRECTSOUNDCAPTUREBUFFER cbuffer = NULL;   /* DS capture buffer */
static HANDLE		    notify_event;	    /* for DS notify events */
static DSBPOSITIONNOTIFY    notify_handlers[MAX_BUFFERS+1];
static LPDIRECTSOUNDNOTIFY  notify;
static DWORD		    bufsize = MIN_BUFFER_SIZE * MAX_BUFFERS;

static void     dserror(HRESULT res, char *s);

static unsigned int bits = 16;

static DWORD WINAPI
dsound_audio_thread(void *V)
{
    int             count = 0, i, j, k, old_count = 0;
    HRESULT         res;
    DWORD           write_pos = 0,
                    read_pos = 0,
                    len1 = 0,
                    len2 = 0;
    DWORD           event = 0;  /* for WaitForMultipleObjects() */

    /* read/write cursors and lengths for DS calls */
    LPVOID      pos1 = NULL,
                pos2 = NULL;	/* pointers for DirectSound lock call */
    DWORD       wbufpos = 0,	/* current write position in the */
		rbufpos = 0;	/* buffer (DirectSound) */
    data_block_t db;

    /*
     * Wait for a message sent to me by the audio driver
     */
    while (keepthreadrunning) {
	if (! audio_driver->enabled) {
	    Sleep(10);
	    continue;
	}

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
        if (firstbuffer) {
            res = IDirectSoundCaptureBuffer_Start(cbuffer, DSCBSTART_LOOPING);
            if (res != DS_OK) {
                dserror(res,"\nCannot start capture via DirectSound: ");
                break;
            }

            /*
             * Start DirectSound playback, if this is a first
             * recorded buffer
             */
            res = IDirectSoundBuffer_Play(pbuffer, 0, 0, DSBPLAY_LOOPING);
            if (res != DS_OK) {
                dserror(res,"\nCannot start playback via DirectSound: ");
                break;
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
            break;
        }
        /*
         * If this is a start state, buffer position
         * is equal to the read position
         * returned by the call above
         */
        /* no bytes read since the last time; skip the cycle */
        else if (rbufpos == read_pos) {
            break;
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
                fprintf(stderr, "\ncapture buffer overrun: real position=%u, calculated=%u", (int) read_pos, (int) rbufpos);
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
                break;
            }
        }

        /* bytes read */
        old_count=count;
        count = len1 + len2;

        /* copying the data block */
        for (i = 0, j = 0, k = 0; i < count / sizeof(SAMPLE16); i++) {
            SAMPLE16       *curpos = NULL;

            if (j >= len1 && pos2 != NULL && k < len2) {
                curpos = pos2 + k;
                k += sizeof(SAMPLE16);
            } else if (pos1 != NULL) {
                curpos = pos1 + j;
                j += sizeof(SAMPLE16);
            } else {
                /* curpos is rubbish if neither condition is true */
                continue;
            }
            procbuf[i] = *curpos << 8;
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
        //triangular_dither(&db);

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
        if (firstbuffer)
            wbufpos = write_pos;
        /* workaround buffer overrun */
        else if (wbufpos<write_pos &&
                /* this condition handles buffer wrap around */
                abs(write_pos-wbufpos)>buffer_size*overrun_threshold &&
                abs(write_pos-wbufpos)<buffer_size*100) {
            fprintf(stderr, "\nplayback buffer overrun: real position=%u, calculated=%u",
                    (int) write_pos, (int) wbufpos);
            wbufpos=write_pos+buffer_size;
        }
        else wbufpos += (old_count?old_count:buffer_size);
        /*
         * handle wrap around
         */
        if (wbufpos >= MIN_BUFFER_SIZE * MAX_BUFFERS) {//bufsize) {   // } for vim
            if (abs(write_pos-wbufpos)>buffer_size*100)
                wbufpos = write_pos;
            else
                wbufpos -= MIN_BUFFER_SIZE * MAX_BUFFERS;
        }

        if (res == DS_OK) {
            res = IDirectSoundBuffer_Lock(pbuffer, wbufpos, count, &pos1, &len1, &pos2, &len2, 0);
            if (res != DS_OK) {
                dserror(res, "\nCannot lock buffer: ");
                break;
            }
        }
        for (i = 0, j = 0, k = 0; i < count / sizeof(SAMPLE16); i++) {
            DSP_SAMPLE      W = (SAMPLE32)db.data[i] >> 8;
            SAMPLE16       *curpos = NULL;

            if (j >= len1 && pos2 != NULL && k < len2) {
                curpos = pos2 + k;
                k += sizeof(SAMPLE16);
            } else if (pos1 != NULL) {
                curpos = pos1 + j;
                j += sizeof(SAMPLE16);
            } else {
                /* curpos is rubbish if neither condition is true */
                continue;
            }
            *curpos = W;
        }

        res = IDirectSoundBuffer_Unlock(pbuffer, pos1, len1, pos2, len2);
        if (res != DS_OK)
            dserror(res, "\nunlock:");

        firstbuffer = 0;

        event=WaitForSingleObject(notify_event,INFINITE);
        ResetEvent(notify_event);
    }
    return 0;
}

static void
dsound_driver_cleanup(void)
{
    keepthreadrunning = 0;
    if (audio_thread) {
	WaitForSingleObject(audio_thread, INFINITE);
	CloseHandle(audio_thread);
	audio_thread = NULL;
    }
    if (cbuffer)
        IDirectSoundCaptureBuffer_Stop(cbuffer);
    if (pbuffer)
        IDirectSoundBuffer_Stop(pbuffer);
    if (cbuffer)
        IDirectSoundCaptureBuffer_Release(cbuffer);
    if (capture)
        IDirectSoundCapture_Release(capture);
    if (pbuffer)
        IDirectSoundBuffer_Release(pbuffer);
    if (snd)
        IDirectSound_Release(snd);
    cbuffer = NULL;
    capture = NULL;
    notify = NULL;
    pbuffer = NULL;
    snd = NULL;
}

/*
 * sound shutdown
 */
static void
dsound_finish_sound(void)
{
    dsound_driver_cleanup();
    dsound_driver.enabled = 0;
}

/*
 * sound initialization
 */
static int
dsound_init_sound(void)
{
    int             i;
    WAVEFORMATEX    format;	/* wave format */
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

    /* start thread -- it is paused until enabled is set to 1 */
    keepthreadrunning = 1;
    firstbuffer = 1;
    audio_thread = CreateThread(NULL, 0, dsound_audio_thread, NULL, 0, &thread_id);
    if (! audio_thread) {
	gnuitar_printf("Failed to create audio recording thread: %08X\n", GetLastError());
	dsound_driver_cleanup();
	return ERR_THREAD;	
    }
    if (!SetThreadPriority(audio_thread, THREAD_PRIORITY_TIME_CRITICAL)) {
	gnuitar_printf("Failed to set realtime priority to thread: %s. Continuing with default priority.", GetLastError());
    }

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
        gnuitar_printf( "\nError opening DirectSound rendering device !");
        dsound_driver_cleanup();
        return ERR_DSOUNDOPEN;
    }

    if (IDirectSound_CreateSoundBuffer(snd, &buffer_desc, &pbuffer, NULL) != DS_OK) {
        gnuitar_printf( "\nError creating DirectSound rendering buffer !");
        dsound_driver_cleanup();
        return ERR_DSOUNDBUFFER;
    }

    /* set notifier for the capture buffer */
    /* This event means - we reached the next position in the capture buffer */
    notify_event = CreateEvent(NULL,FALSE,FALSE,NULL);

    nbuffers = bufsize/buffer_size;
    if ((res=DirectSoundCaptureCreate(NULL, &capture, NULL)) != DS_OK) {
	dserror(res, "\nError opening DirectSound capture object: ");
	dsound_driver_cleanup();
	return ERR_DSOUNDOPEN;
    }

    while (nattempts++ <= MAX_PROBE_ATTEMPTS) {
        int total_read=0;   /* for buffer size probe; total read bytes */
        read_pos=old_read_pos=nreads=0;
    
	/* Create the capture buffer */
	ZeroMemory(&capture_desc, sizeof(DSCBUFFERDESC));
	capture_desc.dwSize = sizeof(DSCBUFFERDESC);
	capture_desc.dwFlags = 0;
	capture_desc.dwBufferBytes = bufsize;
	capture_desc.lpwfxFormat = &format;

	if ((res=IDirectSoundCapture_CreateCaptureBuffer(capture, &capture_desc, &cbuffer, NULL)) != DS_OK) {
	    dserror(res, "Error creating DirectSound capture buffer: ");
	    dsound_driver_cleanup();
	    return ERR_DSOUNDBUFFER;
	}

        if ((res = IDirectSoundCaptureBuffer_QueryInterface(cbuffer,&IID_IDirectSoundNotify, (LPVOID) &notify)) != S_OK) {
            gnuitar_printf( "\nError creating DirectSound notifier: ");
            dsound_driver_cleanup();
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
            dserror(res,"\nProbe: cannot start capture via DirectSound: ");
        for (i = 0; i < 5; i++) {
            /* must wait here until the buffer gets filled */
            WaitForSingleObject(notify_event,INFINITE);
            old_read_pos=read_pos;
            res = IDirectSoundCaptureBuffer_GetCurrentPosition(cbuffer, NULL, &read_pos);
            if (res!=DS_OK)
                dserror(res,"\nError getting capture position via DirectSound: ");
            else if(old_read_pos && read_pos>old_read_pos) {
                total_read+=read_pos-old_read_pos;
                nreads++;
            }
        }
        if (nreads) {
            probed_bufsize=buffer_size=total_read/nreads;
            nbuffers=MIN_BUFFER_SIZE * MAX_BUFFERS / buffer_size;
            bufsize=buffer_size*nbuffers;
	    break;
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

    /* Try to set primary mixing privileges. We don't have our own window
     * yet, so Desktop will have to do. I hate Windows programming. */
    window = GetDesktopWindow();
    res = IDirectSound_SetCooperativeLevel(snd, window, DSSCL_PRIORITY);
    if (res != DS_OK) {
	dserror(res, "\nError setting up the cooperative level: ");
	dsound_driver_cleanup();
	return ERR_DSCOOPLEVEL;
    }

    dsound_driver.enabled = 1;
    return ERR_NOERROR;
}

/* DirectSound Error */
static void 
dserror(HRESULT res, char *s) {
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

static struct audio_driver_channels dsound_channels_cfg[]={
    { 1, 1 },
    { 2, 2 },
    { 0, 0 }
};

audio_driver_t dsound_driver = {
    .str = "DirectX",
    .enabled = 0,
    .channels = dsound_channels_cfg,
    .init = dsound_init_sound,
    .finish = dsound_finish_sound,
};

#endif
