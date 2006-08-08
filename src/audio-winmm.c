/*
 * GNUitar
 * Windows sound driver
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 *           (C) 2005-2006 Antti S. Lankila          <alankila@bel.fi>
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
 * $Log$
 * Revision 1.5  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
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
 *
 */

#ifdef HAVE_WINMM

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#include <fcntl.h>
#include <sys/types.h>

#include "audio-winmm.h"
#include "main.h"
#include "pump.h"
#include "gui.h"
#include "utils.h"

static volatile int keepthreadrunning = 0;
static HANDLE       input_bufs_done = 0, output_bufs_done = 0, audio_thread = 0;
static DWORD        thread_id = 0;

static HWAVEIN  in;		/* input sound handle */
static HWAVEOUT	out;		/* output sound handle */
static MMRESULT err;

/* We use N WAVEHDR's for recording (ie, double-buffering) */
static WAVEHDR  wave_header[MAX_BUFFERS];	/* input header */
static WAVEHDR  write_header[MAX_BUFFERS];	/* output headers */
static char     cur_wr_hdr[MAX_BUFFERS];	/* available write headers
						 * array */
static int      active_in_buffers = 0,
                active_out_buffers = 0;

static void     serror(DWORD err, TCHAR * str);

static unsigned int bits = 16;
static SAMPLE16 wrbuf16[MIN_BUFFER_SIZE * MAX_BUFFERS / sizeof(SAMPLE16)];
static SAMPLE16 rdbuf16[MIN_BUFFER_SIZE * MAX_BUFFERS / sizeof(SAMPLE16)];

static DWORD WINAPI
winmm_audio_thread(void *V)
{
    int             count = 0, i;
    MSG             msg;

    /* read/write cursors and lengths for DS calls */
    data_block_t db;

    /*
     * Wait for a message sent to me by the audio driver
     */
    while (keepthreadrunning) {
	int hdr_avail = -1;	/* available write header index */
	if (! audio_driver->enabled) {
	    Sleep(10);
	    continue;
	}

        if (! GetMessage(&msg, 0, 0, 0)) {
            fprintf(stderr, "Failed to get MMS message -- thread exit\n");
            break;
        }
        switch (msg.message) {
            case MM_WIM_CLOSE:
            case WM_QUIT:
                break;
            /*
             * A buffer has been filled by the driver
         */
            case MM_WIM_DATA:
                active_in_buffers--;

                count = ((WAVEHDR *) msg.lParam)->dwBytesRecorded;
                if (count) {
                    count /= bits >> 3;
                    //WAVEHDR *twh;
                    //twh = (WAVEHDR*) msg.lParam;
                    for (i = 0; i < count; i++)
                        procbuf[i] = ((SAMPLE16 *) (((WAVEHDR *) msg.lParam)->lpData))[i] << 8;

                    /*
                     * find unused output buffer and queue it to output
                     */
                    for (i = 0; i < nbuffers; i++) {
                        if (cur_wr_hdr[i] == 1) {
                            hdr_avail = i;
                            cur_wr_hdr[i] = 0;	/* ready to queue */
                            break;
                        }
                    }

                    if (hdr_avail != -1) {
                        db.data = procbuf;
                        db.data_swap = procbuf2;
                        db.len = count;
                        db.channels = n_input_channels;
                        pump_sample(&db);
                        triangular_dither(&db, (SAMPLE16 *) (write_header[hdr_avail].lpData));

                        err = waveOutWrite(out, &write_header[hdr_avail],sizeof(WAVEHDR));
                        if (err) {
                            serror(err, "\nwriting samples - ");
                        } else
                            active_out_buffers++;
                    } else
                        fprintf(stderr, "\nbuffer overrun.");
                } else
                    fprintf(stderr, "\nbuffer underrun.");

            /*
             * Now we need to requeue this buffer so the driver can
             * use it for another block of audio data. NOTE: We
             * shouldn't need to waveInPrepareHeader() a WAVEHDR that
             * has already been prepared once
             */
            waveInAddBuffer(in, (WAVEHDR *) msg.lParam,
                            sizeof(WAVEHDR));
                active_in_buffers++;
                continue;
            /*
             * Our main thread is opening the WAVE device
             */
            case MM_WIM_OPEN:
                continue;
            /*
             * Audio driver is ready to playback next block
             */
            case MM_WOM_DONE:
                /*
                 * Clear the WHDR_DONE bit (which the driver set last time
                 * that this WAVEHDR was sent via waveOutWrite and was
                 * played). Some drivers need this to be cleared
                 */
                ((WAVEHDR *) msg.lParam)->dwFlags &= ~WHDR_DONE;
                for (i = 0; i < nbuffers; i++) {
                    if (&write_header[i] == (WAVEHDR *) msg.lParam) {
                        cur_wr_hdr[i] = 1;
                        break;
                    }
                }
                active_out_buffers++;
                continue;
        }
    }
    return 0;
}

static void
winmm_driver_cleanup(void)
{
    int             i;

    keepthreadrunning = 0;
    if (audio_thread) {
	WaitForSingleObject(audio_thread, INFINITE);
	CloseHandle(audio_thread);
	audio_thread = NULL;
    }
    if (out)
        waveOutReset(out);
    if (in)
        waveInReset(in);
    /*
     * Wait until all output buffers return
     */
    if (output_bufs_done)
        WaitForSingleObject(output_bufs_done, INFINITE);
    if (input_bufs_done)
        WaitForSingleObject(input_bufs_done, INFINITE);
    output_bufs_done = NULL;
    input_bufs_done = NULL;

    /*
     * Unprepare WAVE buffers
     */
    for (i = 0; i < nbuffers; i++) {
        if (out)
            waveOutUnprepareHeader(out, &write_header[i], sizeof(WAVEHDR));
        if (in)
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
    if (out)
        waveOutClose(out);
    if (in)
        waveInClose(in);
    out = NULL;
    in = NULL;
}

/*
 * sound shutdown
 */
static void
winmm_finish_sound(void)
{
    winmm_driver_cleanup();
    winmm_driver.enabled = 0;
}

/*
 * sound initialization
 */
static int
winmm_init_sound(void)
{
    int             i;
    WAVEFORMATEX    format;	/* wave format */

    /* start thread -- it is paused until enabled is set to 1 */
    keepthreadrunning = 1;
    audio_thread = CreateThread(NULL, 0, winmm_audio_thread, NULL, 0, &thread_id);
    if (! audio_thread) {
	gnuitar_printf("Failed to create audio recording thread: %08X\n", GetLastError());
	winmm_driver_cleanup();
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

    /*
     * Open the Digital Audio Out device - MMSystem init
     */
    ZeroMemory(&wave_header[0], sizeof(WAVEHDR) * MAX_BUFFERS);
    ZeroMemory(&write_header[0], sizeof(WAVEHDR) * MAX_BUFFERS);

    nbuffers = MIN_BUFFER_SIZE * MAX_BUFFERS / buffer_size;
    if (nbuffers > MAX_BUFFERS)
        nbuffers = MAX_BUFFERS;
    /*
     * Open Digital Audio In device
     */
    err = waveInOpen(&in, WAVE_MAPPER, &format, thread_id, 0,
                   CALLBACK_THREAD);
    if (err) {
        serror(err,
            "There was an error opening the Digital Audio In device\r\n");
        return ERR_WAVEINOPEN;
    }

    if ((err = waveOutOpen(&out, WAVE_MAPPER, &format, thread_id, 0,
                     CALLBACK_THREAD))) {
        serror(err,
               "There was an error opening the Digital Audio Out device!\r\n");
        winmm_driver_cleanup();
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
            winmm_driver_cleanup();
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
        if ((err = waveInPrepareHeader(in, &wave_header[i], sizeof(WAVEHDR)))) {
            serror(err, "Error preparing WAVEHDR!\n");
            winmm_driver_cleanup();
            return ERR_WAVEINHDR;
        }
        /*
         * Queue WAVEHDR (recording hasn't started yet)
         */
        if ((err = waveInAddBuffer(in, &wave_header[i], sizeof(WAVEHDR)))) {
            serror(err, "Error queueing WAVEHDR!\n");
            winmm_driver_cleanup();
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
        winmm_driver_cleanup();
        return ERR_WAVEINRECORD;
    }
    winmm_driver.enabled = 1;
    return ERR_NOERROR;
}

/*
 * Retrieves and displays an error message for the passed Wave In error
 * number. It does this using mciGetErrorString().
 */
static void
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

static struct audio_driver_channels winmm_channels_cfg[]={
    { 1, 1 },
    { 2, 2 },
    { 0, 0 }
};

audio_driver_t winmm_driver = {
    .str = "MMSystem",
    .enabled = 0,
    .channels = winmm_channels_cfg,
    .init = winmm_init_sound,
    .finish = winmm_finish_sound,
};

#endif
