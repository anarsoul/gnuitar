/*
 * GNUitar
 * Tracker module - write samples to file
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
 * Revision 1.13  2005/08/24 10:51:55  fonin
 * Wrapped sndfile code into #ifdef HAVE_SNDFILE
 *
 * Revision 1.12  2005/08/23 22:01:34  alankila
 * - add -Wall to ease developing
 * - fix warnings
 *
 * Revision 1.11  2005/08/21 23:44:13  alankila
 * - use libsndfile on Linux to write audio as .wav
 * - move clipping tests into pump.c to save writing it in tracker and 3 times
 *   in main.c
 * - give default name to .wav from current date and time (in ISO format)
 * - there's a weird bug if you cancel the file dialog, it pops up again!
 *   I have to look into what's going on.
 *
 * Revision 1.10  2005/07/25 12:05:29  fonin
 * Workaround for crappy sound in .raw files - thanks Antti S. Lankila <alankila@bel.fi>
 *
 * Revision 1.9  2004/08/10 15:21:16  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.7  2003/03/09 20:58:47  fonin
 * Redesign for the new "change sampling params" feature.
 *
 * Revision 1.6  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.5  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.4  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.3  2001/03/25 17:42:32  fonin
 * open() can overwrite existing files from now, because program switches back to real user priorities after start.
 *
 * Revision 1.2  2001/01/14 21:28:42  fonin
 * Fix: track write could overwrite existing files if executing in suid root mode.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:22  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "tracker.h"
#include "pump.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#ifndef _WIN32
#     include <unistd.h>
#     include <sys/ioctl.h>
#ifdef HAVE_SNDFILE
#     include <sndfile.h>
#endif
#else
#     include <io.h>
#     include <string.h>
#     include <windows.h>
#     include <mmsystem.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#ifndef _WIN32
#ifdef HAVE_SNDFILE
SNDFILE  *fout = NULL;
#else
static int      fout = -1;
#endif // SNDFILE
#else
static HMMIO    fout = NULL;
static MMCKINFO data,
                riff;
#endif // _WIN32

void
tracker_out(const char *outfile)
{
#ifndef _WIN32
#ifdef HAVE_SNDFILE
    SF_INFO             sfinfo;

    memset(&sfinfo, 0, sizeof(sfinfo));
    sfinfo.samplerate = sample_rate;
    sfinfo.frames     = 0;
    sfinfo.channels   = nchannels;
    sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    
    fout = sf_open(outfile, SFM_WRITE, &sfinfo);
    if (! fout)
        fprintf(stderr, "Error: unable to open output file: %s",
                        sf_strerror(fout));
#else
    fout = open(outfile, O_NONBLOCK | O_WRONLY | O_CREAT, 0644);
    if (ioctl(fout, O_NONBLOCK, 0) == -1)
	perror("ioctl");
#endif // SNDFILE

#else
    MMCKINFO        fmt;
    WAVEFORMATEX    format;

    ZeroMemory(&riff, sizeof(MMCKINFO));
    ZeroMemory(&fmt, sizeof(MMCKINFO));
    ZeroMemory(&format, sizeof(WAVEFORMATEX));

    fout = mmioOpen(outfile, NULL, MMIO_CREATE | MMIO_WRITE);
    if (fout != NULL) {
	riff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioCreateChunk(fout, &riff, MMIO_CREATERIFF) !=
	    MMSYSERR_NOERROR) {
	    fprintf(stderr, "\nCreating RIFF chunk failed.");
	    return;
	}
	fmt.ckid = mmioStringToFOURCC("fmt", 0);
	if (mmioCreateChunk(fout, &fmt, 0) != MMSYSERR_NOERROR) {
	    fprintf(stderr, "\nCreating FMT chunk failed.");
	    return;
	}
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = nchannels;
	format.nSamplesPerSec = sample_rate;
	format.wBitsPerSample = bits;
	format.nBlockAlign =
	    format.nChannels * (format.wBitsPerSample / 8);
	format.nAvgBytesPerSec =
	    format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = 0;
	mmioWrite(fout, &format, sizeof(WAVEFORMATEX) - 2);
	mmioAscend(fout, &fmt, 0);
	ZeroMemory(&data, sizeof(MMCKINFO));
	data.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioCreateChunk(fout, &data, 0) != MMSYSERR_NOERROR) {
	    fprintf(stderr, "\nCreating data chunk failed.");
	    return;
	}
    }
#endif
}

void
tracker_done()
{
#ifndef _WIN32
#ifdef HAVE_SNDFILE
    sf_close(fout);
#else
    if (fout > 0)
	close(fout);
#endif
#else
    if (fout != NULL) {
	mmioAscend(fout, &data, 0);
	mmioAscend(fout, &riff, 0);
	mmioClose(fout, 0);
    }
#endif
}


void
track_write(DSP_SAMPLE *s, int count)
{
    SAMPLE          tmp[MAX_BUFFER_SIZE / sizeof(SAMPLE)];
    int             i;

    /*
     * Convert to 16bit raw data
     */
    for (i = 0; i < count; i++)
       tmp[i] = s[i];

#ifndef _WIN32
#ifdef HAVE_SNDFILE
    if (sf_write_short(fout, tmp, count) != count)
        fprintf(stderr, "Error writing samples: %s\n", sf_strerror(fout));
#else
    write(fout, tmp, sizeof(SAMPLE) * count);
#endif
#else
    if (fout != NULL)
	mmioWrite(fout, tmp, sizeof(SAMPLE) * count);
#endif
}
