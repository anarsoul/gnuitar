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
 * Revision 1.71  2006/08/10 12:54:58  alankila
 * - denormal number avoidance routine contained a double constant instead
 *   of float constant. Ouch. This removes a truckload of unnecessary
 *   datatype conversions.
 * - in addition, SSE gnuitar now emits manual code to turn denormal number
 *   avoidance flags on. This may be unnecessary due to -ffast-math, though.
 *
 * Revision 1.70  2006/08/07 12:55:30  alankila
 * - construct audio-driver.c to hold globals and provide some utility
 *   functions to its users. This slashes interdependencies somewhat.
 *
 * Revision 1.69  2006/08/07 12:19:19  alankila
 * - linux cpufreq can cause latency issues for JACK and other operation.
 *   Write a test that emits a warning to user if cpufreq is not set to
 *   performance mode.
 *
 * Revision 1.68  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.67  2006/07/29 15:16:28  alankila
 * - remember presets between gnuitar invocations
 * - remember effect settings between gnuitar invocations
 *
 * Revision 1.66  2006/07/27 19:15:35  alankila
 * - split windows driver architecture now compiles and runs.
 *
 * Revision 1.65  2006/07/27 18:31:15  alankila
 * - split dsound and winmm into separate drivers.
 *
 * Revision 1.64  2006/07/27 00:13:35  alankila
 * - switch to 100% dynamic gnuitar_printf routine
 *
 * Revision 1.63  2006/07/26 23:09:09  alankila
 * - DirectSound may be buggy; MMSystem at least worked in mingw build.
 * - remove some sound-specific special cases in gui and main code.
 * - create thread in windows driver.
 * - remove all traces of "program states" variable.
 * - remove snd_open mutex: it is now unnecessary. Concurrency is handled
 *   through joining/waiting threads where necessary. (We assume JACK
 *   does its own locking, though.)
 *
 * Revision 1.62  2006/07/25 23:41:14  alankila
 * - this patch may break win32. I can't test it.
 *   - move audio_thread handling code into sound driver init/finish
 *   - remove state variable from sight of the Linux code -- it should be
 *     killed also on Win32 side using similar strategies
 *   - snd_open mutex starts to look spurious. It can probably be removed.
 *
 * Revision 1.61  2006/07/17 21:39:38  alankila
 * - use dynamically allocated sample buffers instead of static ones.
 *   (Win32 still uses static buffers moved directly into audio-windows.c)
 *
 * Revision 1.60  2006/07/17 21:09:47  alankila
 * - silence gcc warnings.
 *
 * Revision 1.59  2006/07/15 16:54:48  alankila
 * - ignore PIPE to avoid exit on jackd termination
 *
 * Revision 1.58  2006/07/14 14:20:52  alankila
 * - move g_thread_init() early
 *
 * Revision 1.57  2006/07/03 12:08:15  alankila
 * - remove alignment requirement from DSP_SAMPLE; it's not likely we can ever
 *   really make significant use of the procbuf's alignment due to channel
 *   interleaving issues.
 * - move aligned(16) from the datatype to variable declaration; this seems to
 *   work or at least it didn't crash on me when I tried it on x86-32.
 *
 * Revision 1.56  2006/06/20 20:41:07  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.55  2006/05/25 09:03:05  alankila
 * - replace the SSE code with even faster version. Tubeamp effect now runs
 *   20 % faster on my computer. Add some alignment directives to make future
 *   use of SSE potentially easier.
 *
 * Revision 1.54  2006/05/20 09:56:58  alankila
 * - move audio_driver_str and audio_driver_enabled into driver structure
 * - Win32 drivers are ugly, with the need to differentiate between
 *   DirectX and MMSystem operation through dsound variable. The driver
 *   should probably be split with dsound-specific parts in its own driver.
 *
 * Revision 1.53  2006/05/19 15:12:54  alankila
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
 * Revision 1.52  2006/05/01 10:23:54  anarsoul
 * Alsa device is selectable and input volume is adjustable now. Added new filter - amp.
 *
 * Revision 1.50  2005/09/28 19:55:04  fonin
 * DirectSound hook on startup
 *
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
#include <signal.h>
#include <pthread.h>
#endif

#include "main.h"
#include "pump.h"
#include "gui.h"
#include "audio-driver.h"

#ifdef __SSE__
#include <xmmintrin.h>
#endif
#ifdef __SSE3__
#include <pmmintrin.h>
#endif

char            version[13] = "GNUitar "VERSION;

#ifdef linux
/* Low-latency operation is improved if the CPUs do not change speed.
 * Especially with JACK it is problematic because JACK tends to quit
 * if the system doesn't seem fast enough. JACK should be smarter
 * about cpufreq. */
static void
test_linux_cpufreq()
{
    int i;
    FILE           *cpufd;
    char            entry[100];

    /* examine sysfs and try to determine the used scaling governor. This might
     * fail for any number of reasons, so I'm pretty silent about errors. */
    for (i = 0; i < 8; i += 1) {
        char cpuentry[100];
        sprintf(cpuentry, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", i);
        if ((cpufd = fopen(cpuentry, "r")) != NULL) {
            size_t bytes = fread(entry, sizeof(entry[0]), sizeof(entry), cpufd);
            if (bytes > 0 && bytes < sizeof(entry)) {
                if (entry[bytes-1] == '\n')
                    entry[bytes-1] = '\0';
                else
                    entry[bytes] = '\0';
                if (strncmp(entry, "performance", bytes) != 0)
                    gnuitar_printf("warning: CPU%d not set to performance mode by cpufreq driver (mode is: %s).\n", i, entry);
            }
            fclose(cpufd);
        }
    }
    /* we could change the scaling governor temporarily, but that would be impolite. */
}
#endif

int
main(int argc, char **argv)
{
    int             error = 0;
#ifndef _WIN32
    int             max_priority;
    struct sched_param p;
    sigset_t ignore_set;
    
    max_priority = sched_get_priority_max(SCHED_FIFO);
    p.sched_priority = max_priority/2;

    if (sched_setscheduler(0, SCHED_FIFO, &p)) {
	gnuitar_printf("warning: unable to set realtime priority (needs root privileges)\n");
    }

    /* We might have been running to this point as setuid root program.
     * Switching to real user id. */
    setuid(getuid());
 
    /* JACK can give us PIPE if the server terminates abruptly,
     * ignoring it allows us to avoid exit(). */
    sigemptyset(&ignore_set);
    sigaddset(&ignore_set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &ignore_set, NULL);
#endif
    printf(COPYRIGHT
       "This program is a free software under the GPL;\n"
       "see Help->About for details.\n");
    gnuitar_printf("GNUitar " VERSION " debug window.\n");
#ifdef linux
    test_linux_cpufreq();
#endif

    /* Some FPU flags for faster performance. It is likely that
     * -ffast-math already generates code to turn these on, though. */
#ifdef __SSE__
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
#ifdef __SSE3__
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif

    /* GTK+ manual suggests this regarding threads:
     * http://developer.gimp.org/api/2.0/gdk/gdk-Threads.html
     *
     * We shouldn't need to initialize gtk/gdk mutex because our audio thread 
     * does not participate in the GUI. */
    g_thread_init(NULL);
    gtk_init(&argc, &argv);
    load_settings();

    /* choose audio driver if not given in config */
    if (audio_driver == NULL) {
        gnuitar_printf("Discovering audio driver.\n");
        guess_audio_driver();
    }
    if (audio_driver == NULL)
	gnuitar_printf("warning: no usable audio driver found.\n");

    pump_start();
    if (audio_driver && (error = audio_driver->init()) != ERR_NOERROR) {
        gnuitar_printf("warning: unable to begin audio processing (code %d)\n", error);
    }

    init_gui();
    load_initial_state(argv, argc);
    gtk_main();

    if (audio_driver && audio_driver->enabled)
        audio_driver->finish();

    pump_stop();
    save_settings();

    return ERR_NOERROR;
}
