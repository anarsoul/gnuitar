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
 * Revision 1.22  2006/08/07 13:20:43  alankila
 * - group all effects through effect.h rather than enumerating them in
 *   pump.c.
 *
 * Revision 1.21  2006/08/06 20:14:55  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.20  2006/07/26 17:13:05  alankila
 * - win32 build fixes
 *
 * Revision 1.19  2006/06/20 20:41:08  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.18  2006/05/19 15:12:54  alankila
 * I keep on getting rattles with ALSA playback, seems like ALSA doesn't
 * know when to swap buffers or allows write to go on too easily. I
 * performed a major overhaul/cleanup in trying to kill this bug for good.
 *
 * - fix confusion about what "buffer_size" really means and how many bytes
 *   it takes.
 *   - buffer size is ALWAYS the fragment size in all audio drivers
 *     (ALSA, OSS, Win32 driver)
 *   - it follows that memory needed is buffer_size times maximum
 *     frame size. (32-bit samples, 4 channels, max buffer size.)
 *   - latency calculation updated, but it may be incorrect yet
 * - add write buffer for faster ALSA read-write cycle. (Hopefully this
 *   reduces buffer underruns and rattles and all sort of ugliness we
 *   have with ALSA)
 * - redesign the ALSA configuration code. Now we let ALSA choose the
 *   parameters during the adjustment phase, then we try same for playback.
 * - some bugs squashed in relation to this, variables renamed, etc.
 * - if opening audio driver fails, do not kill the user's audio_driver
 *   choice. (It makes configuring bits, channels, etc. difficult.)
 *   We try to track whether processing is on/off through new variable,
 *   audio_driver_enabled.
 *
 * Note: all the GUI code related to audio is in need of a major overhaul.
 * Several variables should be renamed, variable visibility better controlled.
 *
 * Revision 1.17  2005/09/03 22:13:56  alankila
 * - make multichannel processing selectable
 * - new GUI (it sucks as much as the old one and I'll need to grok GTK
 *   tables first before it gets better)
 * - make pump.c do the multichannel adapting bits
 * - effects can now change channel counts
 *
 * Revision 1.16  2005/08/28 21:45:30  fonin
 * Added type casting for >> operations on SAMPLE16/32 vars, to shut up MSVC
 *
 * Revision 1.15  2005/08/27 19:05:43  alankila
 * - introduce SAMPLE16 and SAMPLE32 types, and switch
 *
 * Revision 1.14  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
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
#include "gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#   include <unistd.h>
#   include <sys/ioctl.h>
#   ifdef HAVE_SNDFILE
#       include <sndfile.h>
#   else
#       include <fcntl.h>
#   endif
#else
#     include <io.h>
#     include <string.h>
#     include <windows.h>
#     include <mmsystem.h>
#endif
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
    sfinfo.channels   = n_output_channels;
    sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    
    fout = sf_open(outfile, SFM_WRITE, &sfinfo);
    if (! fout)
        gnuitar_printf( "Error: unable to open output file: %s",
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
	    gnuitar_printf( "\nCreating RIFF chunk failed.");
	    return;
	}
	fmt.ckid = mmioStringToFOURCC("fmt", 0);
	if (mmioCreateChunk(fout, &fmt, 0) != MMSYSERR_NOERROR) {
	    gnuitar_printf( "\nCreating FMT chunk failed.");
	    return;
	}
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = n_output_channels;
	format.nSamplesPerSec = sample_rate;
	format.wBitsPerSample = 16;
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
	    gnuitar_printf( "\nCreating data chunk failed.");
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
    SAMPLE16        tmp[MAX_BUFFER_SIZE];
    int             i;

    /*
     * Convert to 16bit raw data
     */
    for (i = 0; i < count; i++)
       tmp[i] = (SAMPLE32)s[i] >> 8;

#ifndef _WIN32
#ifdef HAVE_SNDFILE
    if (sf_write_short(fout, tmp, count) != count)
        gnuitar_printf( "Error writing samples: %s\n", sf_strerror(fout));
#else
    write(fout, tmp, sizeof(SAMPLE16) * count);
#endif
#else
    if (fout != NULL)
	mmioWrite(fout, tmp, sizeof(SAMPLE16) * count);
#endif
}
