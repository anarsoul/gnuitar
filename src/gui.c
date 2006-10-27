/*
 * GNUitar
 * Graphics user interface
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
 * Revision 1.122  2006/10/27 21:54:46  alankila
 * - new source file: audio-midi.c. Do some data abstraction, prepare to
 *   support multiple midi continuous controls.
 *
 * Revision 1.121  2006/10/27 18:44:27  alankila
 * - reduce the visibility of a shitload of public widgets
 *
 * Revision 1.120  2006/09/12 16:06:40  alankila
 * - make the master controls way finer: 0.1 microsteps to adjust to 0.0 and
 *   1.0 dB steps for jumps
 *
 * Revision 1.119  2006/09/09 19:14:59  alankila
 * make VU meter react faster
 *
 * Revision 1.118  2006/08/29 09:57:29  alankila
 * - disallow resampling in ALSA
 * - add some gnuitar_printf metadata into the errors/warnings/infos
 * - prefer hw:0,0 as alsa device over default that may be dmix or something
 *
 * Revision 1.117  2006/08/11 16:08:53  alankila
 * - don't crash if no audio driver can be opened.
 *
 * Revision 1.116  2006/08/10 18:52:07  alankila
 * - declare prototypes properly
 * - hide some accidentally global methods
 *
 * Revision 1.115  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.114  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.113  2006/08/07 20:01:50  alankila
 * - move all modifications of effect list structures into effect.c.
 *
 * Revision 1.112  2006/08/07 13:20:42  alankila
 * - group all effects through effect.h rather than enumerating them in
 *   pump.c.
 *
 * Revision 1.111  2006/08/07 12:55:30  alankila
 * - construct audio-driver.c to hold globals and provide some utility
 *   functions to its users. This slashes interdependencies somewhat.
 *
 * Revision 1.110  2006/08/07 12:18:41  alankila
 * - document GTK+ bug at lack of initial scrolling of debug window
 *
 * Revision 1.109  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.108  2006/08/03 08:22:15  alankila
 * - doh! forgot HAVE_GTK2 around text buffer adjustment when I shuffled
 *   the code around.
 *
 * Revision 1.107  2006/08/03 05:20:33  alankila
 * - strange, somehow missed one n => effects_n
 *
 * Revision 1.106  2006/07/31 22:48:59  alankila
 * - selecting pre- and post-gains are an exact science, so show the values.
 *
 * Revision 1.105  2006/07/30 11:14:06  alankila
 * - rename n variable to effects_n -- n is too short for a global
 *
 * Revision 1.104  2006/07/29 15:25:46  alankila
 * - support doubleclick on preset list
 *
 * Revision 1.103  2006/07/29 15:16:28  alankila
 * - remember presets between gnuitar invocations
 * - remember effect settings between gnuitar invocations
 *
 * Revision 1.102  2006/07/29 13:54:34  alankila
 * - add audio driver configuration guide, discussion about using JACK and
 *   MIDI
 *
 * Revision 1.101  2006/07/28 19:08:40  alankila
 * - add midi event listeners into JACK and ALSA
 * - make gui listen to midi events and switch bank
 * - fix a bug involving "add pressed"
 * - change documentation of "Switch preset" to "Cycle presets" which is what
 *   it does now.
 *
 * Revision 1.100  2006/07/27 20:15:54  alankila
 * - one instance of DSound / MMSystem tied to _WIN32 still found. Fixed.
 *
 * Revision 1.99  2006/07/27 19:15:35  alankila
 * - split windows driver architecture now compiles and runs.
 *
 * Revision 1.98  2006/07/27 18:31:15  alankila
 * - split dsound and winmm into separate drivers.
 *
 * Revision 1.97  2006/07/27 10:31:01  alankila
 * - make surround40 -> 4 channel link work
 * - remove process headers from gui.c
 *
 * Revision 1.96  2006/07/27 00:45:57  alankila
 * - get rid of GTK_ENABLE_BROKEN -- no more gtk_text_new() for GTK2.
 *
 * Revision 1.95  2006/07/27 00:13:35  alankila
 * - switch to 100% dynamic gnuitar_printf routine
 *
 * Revision 1.94  2006/07/26 23:09:09  alankila
 * - DirectSound may be buggy; MMSystem at least worked in mingw build.
 * - remove some sound-specific special cases in gui and main code.
 * - create thread in windows driver.
 * - remove all traces of "program states" variable.
 * - remove snd_open mutex: it is now unnecessary. Concurrency is handled
 *   through joining/waiting threads where necessary. (We assume JACK
 *   does its own locking, though.)
 *
 * Revision 1.93  2006/07/26 18:08:39  alankila
 * - implement various compile fixes for mingw
 *
 * Revision 1.92  2006/07/25 23:47:51  alankila
 * - depend on audio-windows.h to obtain description of the state variable
 *
 * Revision 1.91  2006/07/25 23:41:14  alankila
 * - this patch may break win32. I can't test it.
 *   - move audio_thread handling code into sound driver init/finish
 *   - remove state variable from sight of the Linux code -- it should be
 *     killed also on Win32 side using similar strategies
 *   - snd_open mutex starts to look spurious. It can probably be removed.
 *
 * Revision 1.90  2006/07/25 22:51:38  alankila
 * - simplify help browser forking, get rid of all string operations
 *
 * Revision 1.89  2006/07/19 22:30:57  alankila
 * - remove all freeze/thaw cycles around singular operations too
 *
 * Revision 1.88  2006/07/19 22:16:39  anarsoul
 * - Fixed copy-paste error (No operations between gtk_text_freeze and gtk_text_thaw)
 *
 * Revision 1.87  2006/07/19 10:43:42  alankila
 * - remove unnecessary paths in help_contents
 *
 * Revision 1.86  2006/07/15 23:21:23  alankila
 * - show input/output vu meters separately
 *
 * Revision 1.85  2006/07/15 23:02:45  alankila
 * - remove the bits control -- just use the best available on every driver.
 *
 * Revision 1.84  2006/07/14 14:19:50  alankila
 * - gui: OSS now supports 1-in 2-out mode.
 * - alsa: try to use recorded settings values before adapting attempts
 * - alsa: log adapt attempts and results
 *
 * Revision 1.83  2006/06/20 20:41:06  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.82  2006/06/05 13:41:46  anarsoul
 * Added tabbed about dialog
 *
 * Revision 1.81  2006/06/01 16:16:11  fonin
 * How many times can I fix the same bug ? strftime() must not use locale params, because it gives slashes in the date which is not ok by the filesystem
 *
 * Revision 1.80  2006/05/31 13:48:00  fonin
 * Alsa device dropdown code is now ifdef'ed. Someone broke the windows code in update_latency_label(), fixed too.
 *
 * Revision 1.79  2006/05/29 18:36:54  anarsoul
 * Initial JACK support
 *
 * Revision 1.78  2006/05/20 09:56:58  alankila
 * - move audio_driver_str and audio_driver_enabled into driver structure
 * - Win32 drivers are ugly, with the need to differentiate between
 *   DirectX and MMSystem operation through dsound variable. The driver
 *   should probably be split with dsound-specific parts in its own driver.
 *
 * Revision 1.77  2006/05/20 08:49:35  alankila
 * - squash various memory leaks in GTK+ interfacing code
 * - make sampling parameter dialog more dynamic.
 *   When user updates audio driver, the bits and channels dialogs are
 *   rebuilt to reflect settings available for that driver.
 * - I still couldn't protect user against choosing surround40 and
 *   using less than 4 channels for output. My attempts were rewarded by
 *   a segfault from GTK+. :-(
 *
 * Revision 1.76  2006/05/19 15:12:54  alankila
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
 * Revision 1.75  2006/05/15 19:39:46  alankila
 * - add surround40 to the list of chooseable ALSA devices.
 * - this has the drawback that user choosing surround40 must also choose
 *   an output mode with 4 channels, or the audio driver can not be opened.
 *   Shitty.
 *
 * Revision 1.74  2006/05/13 11:37:31  alankila
 * - shut up glib
 *
 * Revision 1.73  2006/05/13 11:27:58  alankila
 * - remember last path in effect/layout functions
 * - more string changes
 *
 * Revision 1.72  2006/05/13 11:02:53  alankila
 * - small GUI overhaul
 *
 * Revision 1.71  2006/05/13 10:45:44  alankila
 * - increase window height to make space for more effects :-)
 * - rename "Processor list" to "Preset list"
 *
 * Revision 1.70  2006/05/01 10:23:54  anarsoul
 * Alsa device is selectable and input volume is adjustable now. Added new filter - amp.
 *
 * Revision 1.69  2006/02/07 13:30:57  fonin
 * Fixes to ALSA driver by Vasily Khoruzhick
 *
 * Revision 1.68  2005/11/05 12:18:38  alankila
 * - pepper the code with static declarations for all private funcs and vars
 *
 * Revision 1.67  2005/10/02 08:21:39  fonin
 * - Added a button to remove preset from a list;
 * - Double-click on the effect name puts a focus on its window
 *
 * Revision 1.66  2005/10/01 07:54:28  fonin
 * - Tracker suggests filename: fixed date format in strftime(),
 *   because MSVC does not recognize C99 extensions;
 * - Align fragment size to MIN_BUFFER_SIZE for OSS and MMSystem drivers.
 *
 * Revision 1.65  2005/09/28 19:53:21  fonin
 * Taken off DirectSound checkbox from the
 *   sample params dialog
 *
 * Revision 1.64  2005/09/12 09:42:25  fonin
 * - MSVC compatibility fixes
 *
 * Revision 1.63  2005/09/05 17:42:07  alankila
 * - fix some small memory leaks
 *
 * Revision 1.62  2005/09/05 17:13:16  alankila
 * - make it possible to add effects with doubleclick
 * - when deleting effects, do not cause -1 row to become selected
 *
 * Revision 1.61  2005/09/04 23:27:38  alankila
 * - in case audio driver init fails, set audio_driver to null, user needs
 *   to go to options to fix it.
 *
 * Revision 1.60  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.59  2005/09/04 21:04:21  alankila
 * - handle no audio driver more gracefully
 *
 * Revision 1.58  2005/09/04 20:45:00  alankila
 * - store audio driver into config
 * - make it possible to start gnuitar with invalid audio driver and enter
 *   options and correct the driver. Some rough edges still remain with
 *   the start/stop button, mutexes, etc.
 *
 * Revision 1.57  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.56  2005/09/04 14:20:30  alankila
 * - correct a brainfart about how row moving happens
 *
 * Revision 1.55  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.54  2005/09/03 23:29:03  alankila
 * - I finally cracked the alignment problem. You align GTK labels with
 *   gtk_misc_set_alignment.
 *
 * Revision 1.53  2005/09/03 22:59:24  alankila
 * - be more diligent about updating latency
 *
 * Revision 1.52  2005/09/03 22:13:56  alankila
 * - make multichannel processing selectable
 * - new GUI (it sucks as much as the old one and I'll need to grok GTK
 *   tables first before it gets better)
 * - make pump.c do the multichannel adapting bits
 * - effects can now change channel counts
 *
 * Revision 1.51  2005/09/03 20:20:42  alankila
 * - create audio_driver type and write all the driver stuff into it. This
 *   faciliates carrying configuration data about the capabilities of
 *   a specific audio driver and uses less global variables.
 *
 * Revision 1.50  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.49  2005/09/02 00:06:34  alankila
 * - support row moving event
 *
 * Revision 1.48  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.47  2005/08/31 14:43:35  alankila
 * fix one more qualifier err
 *
 * Revision 1.46  2005/08/28 21:41:28  fonin
 * Portability: introduced new functions for mutexes
 *
 * Revision 1.45  2005/08/28 14:04:04  alankila
 * - OSS copypaste error fix
 * - remove my_log2 in favour of doing pow, trunc, log.
 * - OSS driver rounds buffer sizes to suitable values by itself now. There's
 *   a precedent in tuning user parameters automatically in ALSA code. The
 *   new behaviour rounds buffer size down, though.
 *
 * Revision 1.44  2005/08/28 12:42:27  alankila
 * move write_track flag properly into pump.h, make it volatile because it's
 * shared by threads
 *
 * Revision 1.43  2005/08/28 12:39:01  alankila
 * - make audio_lock a real mutex
 * - fix mutex cleanup at exit
 *
 * Revision 1.42  2005/08/28 12:28:44  alankila
 * switch to GMutex that is also available on win32
 *
 * Revision 1.41  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.40  2005/08/26 15:59:56  fonin
 * Audio driver now can be chosen by user
 *
 * Revision 1.39  2005/08/24 21:55:05  alankila
 * slight bit likelier to compile
 *
 * Revision 1.38  2005/08/24 21:44:44  alankila
 * - split sound drivers off main.c
 * - add support for alsa
 * - rework thread locking
 * - in this version, sound drivers are chosen at compile time
 * - windows driver is probably broken
 *
 * Revision 1.37  2005/08/24 13:13:38  alankila
 * - battle about whether *name is const or not continues
 *
 * Revision 1.36  2005/08/24 10:51:55  fonin
 * Wrapped sndfile code into #ifdef HAVE_SNDFILE
 *
 * Revision 1.35  2005/08/21 23:57:04  alankila
 * get rid of the effects of the "clicked" ghost event.
 *
 * Revision 1.34  2005/08/21 23:44:13  alankila
 * - use libsndfile on Linux to write audio as .wav
 * - move clipping tests into pump.c to save writing it in tracker and 3 times
 *   in main.c
 * - give default name to .wav from current date and time (in ISO format)
 * - there's a weird bug if you cancel the file dialog, it pops up again!
 *   I have to look into what's going on.
 *
 * Revision 1.33  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.32  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.31  2005/08/13 11:38:15  alankila
 * - some final warning fixups and removal of MYGTK_TEXT hack
 *
 * Revision 1.30  2005/08/12 22:39:05  alankila
 * correct -96 to -91 because doing x / log(2) * 3 is not quite same as
 * x / log(10) * 10
 *
 * Revision 1.29  2005/08/12 11:30:30  alankila
 * - add some const qualifiers
 *
 * Revision 1.28  2005/08/12 11:21:38  alankila
 * - add master volume widget
 * - reimplement bias computation to use true average
 *
 * Revision 1.27  2005/08/11 17:57:21  alankila
 * - add some missing headers & fix all compiler warnings on gcc 4.0.1+ -Wall
 *
 * Revision 1.26  2005/08/10 12:09:36  alankila
 * - oops; forgot couple of lines from last patch, so peak indication didn't
 *   work!
 *
 * Revision 1.25  2005/08/10 10:54:39  alankila
 * - add output VU meter. The scale is logarithmic, resolves down to -96 dB
 *   although it's somewhat wasteful at the low end.
 * - add bias elimination code -- what I want is just a long-term average
 *   for estimating the bias. Right now it's a bad estimation of average but
 *   I'll improve it later on.
 * - the vu-meter background flashes red if clipping. (I couldn't make the
 *   bar itself flash red, the colour is maybe not FG or something.)
 * - add *experimental* noise reduction code. This code will be moved into
 *   a separate NR effect later on. Right now it's useful for testing, and
 *   should not perceptibly degrade the signal anyway.
 *
 * Revision 1.24  2005/04/26 13:38:05  fonin
 * Fixed help contents bug on Win2k
 *
 * Revision 1.23  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.22  2003/05/30 12:49:23  fonin
 * log2() renamed to my_log2() since log2 is a reserved word on MacOS X.
 *
 * Revision 1.21  2003/05/01 19:09:25  fonin
 * Bugfix with spawnl() syntax - the last parameter always should be NULL.
 *
 * Revision 1.20  2003/04/16 18:42:58  fonin
 * R_OK macro definition moved to utils.h.
 *
 * Revision 1.19  2003/04/11 18:33:56  fonin
 * my_itoa() moved to utils.h.
 *
 * Revision 1.18  2003/03/25 19:56:57  fonin
 * Added tooltips to most important controls.
 *
 * Revision 1.17  2003/03/25 14:03:31  fonin
 * New control in options dialog for the buffer overrun threshold.
 *
 * Revision 1.16  2003/03/23 20:05:18  fonin
 * sample_dlg(): checkbox to switch playback method between DirectSound and MME.
 *
 * Revision 1.15  2003/03/13 20:23:45  fonin
 * Selecting the current bank when the switch is pressed.
 *
 * Revision 1.14  2003/03/09 21:10:12  fonin
 * - new toggle button to start/stop recording;
 * - new menu item for sampling params;
 * - new dialog for sampling params.
 *
 * Revision 1.13  2003/02/05 21:07:39  fonin
 * Fix: when a the write track checkbox is clicked, and then action is cancelled,
 * checkbox remained toggled.
 *
 * Revision 1.12  2003/02/04 20:42:18  fonin
 * Heuristic to search for docs through the few directories.
 *
 * Revision 1.11  2003/02/03 17:24:24  fonin
 * Launch HTML browser to view docs.
 *
 * Revision 1.10  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.9  2003/02/03 11:36:38  fonin
 * Add icon to the main window on startup.
 *
 * Revision 1.8  2003/01/30 21:32:25  fonin
 * Removed gui_done()
 *
 * Revision 1.7  2003/01/30 21:31:34  fonin
 * - worked around the Win32-GTK bug with non-ASCII characters in Win32 build;
 * - show only filename in the bank list, instead of full path;
 * - get rid of rnd_window_pos()
 *
 * Revision 1.6  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.5  2001/06/02 14:27:14  fonin
 * Added about dialog.
 *
 * Revision 1.4  2001/06/02 14:05:42  fonin
 * Effects pushed to the END of stack.
 * Added GNU disclaimer.
 *
 * Revision 1.3  2001/03/25 12:09:51  fonin
 * Added function delete_event(). Effect control windows use it to ignore windows destroy event.
 *
 * Revision 1.2  2001/03/11 20:16:44  fonin
 * Fixed destroying of main window.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:51  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#ifdef _WIN32
#    include <io.h>
#    include <ctype.h>
#    include <windows.h>
#    include <process.h>
#    include "resource.h"
#else
#    include <libgen.h>
#    include <unistd.h>
#    include "gnuitar.xpm"
#endif
#include <stdarg.h>

#include "gui.h"
#include "audio-midi.h"
#include "authors_text.h"
#include "about_text.h"
#include "license_text.h"
#include "pump.h"
#include "main.h"
#include "audio-midi.h"
#include "audio-driver.h"
#include "tracker.h"
#include "utils.h"

#define VU_UPDATE_INTERVAL   100.0    /* ms */
#define BANK_UPDATE_INTERVAL 20.0     /* ms */

static void     add_pressed(GtkWidget *, gpointer);
static void     bank_start_save(GtkWidget *, gpointer);
static void     bank_start_load(GtkWidget *, gpointer);
static void     sample_dlg(GtkWidget *, gpointer);
static void     update_sampling_params(GtkWidget *, gpointer);
static void     update_sampling_params_and_close_dialog(GtkWidget *, gpointer);
static void     quit(GtkWidget *, gpointer);
static void     about_dlg(void);
static void     help_contents(void);

static GtkWidget      *tracker;
static GtkWidget      *bank;
static GtkTooltips    *tooltips;
static GtkItemFactory *item_factory;
static GtkWidget      *status_text = NULL;
static GtkWidget      *status_window = NULL;
static gchar *effects_dir = NULL;

static GtkItemFactoryEntry mainGui_menu[] = {
    {"/_File", "<alt>F", NULL, 0, "<Branch>", NULL},

    {"/File/_Open Layout", "<control>O",
     (GtkSignalFunc) bank_start_load, 0, NULL, NULL},
    {"/File/_Save Layout", "<control>S", (GtkSignalFunc) bank_start_save,
     0, NULL, NULL},
    {"/File/sep1", NULL, NULL, 0, "<Separator>", NULL},
    {"/File/E_xit", "<control>Q", (GtkSignalFunc) quit, 0, NULL, NULL},
    {"/_Options", "<alt>O", NULL, 0, "<Branch>", NULL},
    {"/Options/O_ptions", "<control>P",
     (GtkSignalFunc) sample_dlg, 0, NULL, NULL},
    {"/_Help", NULL, NULL, 0, "<LastBranch>", NULL},
    {"/_Help/Contents", NULL, (GtkSignalFunc) help_contents, 0, NULL, NULL},
    {"/_Help/About", NULL, (GtkSignalFunc) about_dlg, 0, NULL, NULL}
};
/* some public GUI widgets */
GtkWidget      *processor;
/* master volume and its current value */
GtkObject      *adj_master;
float		master_volume;
GtkObject      *adj_input;
float		input_volume;

/* vumeter state */
static float vumeter_in_power  = -96;
static float vumeter_out_power = -96;
/* current row in processor list */
static gint            curr_row = -1;
/* current row in known effects list */
static gint            effects_row = -1;
/* current row in bank list */
static gint            bank_row = -1;

//function for printing debuging messages
//if GUI isn't created, text will be buffered
void 
gnuitar_printf(const char *frm, ...)
{
    va_list args;
    static GList *bufferedmsgs = NULL;
    GList *listtmp;
    gchar *tmp;
#ifdef HAVE_GTK2
    GtkAdjustment *adj;
#endif

    va_start(args, frm);
    tmp = g_strdup_vprintf(frm, args);
    va_end(args);
   
    if (status_text == NULL) {
        fprintf(stderr, "%s", tmp);
        bufferedmsgs = g_list_append(bufferedmsgs, tmp);
	return;
    }
    
    /* append the buffered data, get rid of bufferedmsgs */ 
    for (listtmp = g_list_first(bufferedmsgs); listtmp != NULL; listtmp = g_list_next(listtmp)) {
        gnuitar_gtk_text_view_append(status_text, listtmp->data);
        g_free(listtmp->data);
    }
    if (bufferedmsgs) {
        g_list_free(bufferedmsgs);
        bufferedmsgs = NULL;
    }

    /* append the given input buffer */
    gnuitar_gtk_text_view_append(status_text, tmp);
    g_free(tmp);

#ifdef HAVE_GTK2
    /* scroll to end */
    adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(status_window));
    gtk_adjustment_set_value(adj, adj->upper);
#endif
}

/*
 * Cleaning and quit from application
 */
static void
quit(GtkWidget * widget, gpointer data)
{
    gchar *presetpath;
    gchar *filename;

    /* on quit, save settings... */
    presetpath = discover_preset_path();
    filename = g_strdup_printf("%s" FILESEP "%s", presetpath, "__default__.gnuitar");
    save_pump(filename);
    g_free(filename);
    g_free(presetpath);

    gtk_main_quit();
}

static void
about_dlg(void)
{
    GtkWidget      *about;
    GtkWidget      *about_label;
    GtkWidget      *vbox;
    GtkWidget      *about_scrolledwin, *license_scrolledwin, *authors_scrolledwin;
    GtkWidget      *about_text, *license_text, *authors_text;
    GtkWidget      *about_plabel, *license_plabel, *authors_plabel;
    GtkWidget      *ok_button;
    GtkWidget      *notebook;

    about = gtk_window_new(GTK_WINDOW_DIALOG);

    about_label = gtk_label_new(COPYRIGHT);
    gtk_window_set_title(GTK_WINDOW(about), "About");
    gtk_container_set_border_width(GTK_CONTAINER(about), 8);
#ifdef HAVE_GTK
    gtk_widget_set_usize(about, 528, 358);
#elif defined(HAVE_GTK2)
    gtk_window_set_default_size(GTK_WINDOW(about), 528, 358);
#endif
    gtk_window_set_position(GTK_WINDOW(about), GTK_WIN_POS_CENTER);
    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
    
    vbox = gtk_vbox_new(FALSE, 8);
    gtk_container_add(GTK_CONTAINER(about), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), about_label, FALSE, FALSE, 0);

    about_scrolledwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(about_scrolledwin),
				   GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
				 
    license_scrolledwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(license_scrolledwin),
				   GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    authors_scrolledwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(authors_scrolledwin),
				   GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
				 
    //gtk_box_pack_start(GTK_BOX(vbox), scrolledwin, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    //gtk_container_add (GTK_CONTAINER (frame), scrolledwin);
    
    about_plabel = gtk_label_new ("About");
    authors_plabel = gtk_label_new ("Authors");
    license_plabel = gtk_label_new ("License");

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), about_scrolledwin, about_plabel);
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), authors_scrolledwin, authors_plabel);
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), license_scrolledwin, license_plabel);
    
    about_text = gnuitar_gtk_text_view_new(GTK_CONTAINER(about_scrolledwin));
    gnuitar_gtk_text_view_append(about_text, "\n GNUitar " VERSION "\n\n");
    gnuitar_gtk_text_view_append(about_text, about_txt);
    
    authors_text = gnuitar_gtk_text_view_new(GTK_CONTAINER(authors_scrolledwin));
    gnuitar_gtk_text_view_append(authors_text, authors_txt);

    license_text = gnuitar_gtk_text_view_new(GTK_CONTAINER(license_scrolledwin));
    gnuitar_gtk_text_view_append(license_text, license_txt);

    ok_button = gtk_button_new_with_label("OK");
    gtk_box_pack_end(GTK_BOX(vbox), ok_button, FALSE, FALSE, 0);
    gtk_signal_connect_object(GTK_OBJECT(ok_button), "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(about));
    gtk_widget_grab_focus(ok_button);

    gtk_widget_show_all(about);
}

static void
help_contents(void)
{
    char const     *path = NULL;
    int             i;
#ifndef _WIN32
    pid_t           pid;
    char const     *browser;
    const char     *browsers[] = {
        "/usr/bin/x-www-browser",
        "/usr/bin/firefox",
        "/usr/bin/mozilla",
        "/usr/bin/opera",
        "/usr/bin/konqueror",
        "/usr/bin/dillo",
        "/usr/bin/galeon",
        NULL,
    };
#endif
    const char     *docs[] = {
/* we should propagate install path from build directory, get rid of this */
#ifdef _WIN32
        "..\\docs\\index.html",
        "docs\\index.html",
        "index.html",
#else
	"/usr/local/share/doc/gnuitar/docs/index.html",
        "/usr/share/doc/gnuitar/docs/index.html",
        "../docs/index.html",
        "docs/index.html",
        "index.html",
#endif
        NULL
    };

    /* try to find the document directory */
    for (i = 0; docs[i] != NULL; i++) {
        if (access(docs[i], R_OK) == 0) {
            path = docs[i];
            break;
        }
    }

    if (path == NULL) {
        gnuitar_printf("Failed to find the documents in any known dir.\n");
        return;
    }

#ifndef _WIN32
    /* unix code must find browser to run */
    browser = getenv("BROWSER");
    if (browser == NULL) {
        /* nope -- must guess. */
	for (i = 0; browsers[i] != NULL; i++) {
	    if (access(browsers[i], X_OK) == 0) {
		browser = browsers[i];
		break;
	    }
	}
    }

    if (browser == NULL) {
        gnuitar_printf("Failed to find a working browser to launch.\n");
        return;
    }

    pid = fork();
    if (pid == -1) {
        gnuitar_printf("Failed to fork a process: %s\n", strerror(errno));
        return;
    }
    if (pid != 0) {
        /* parent has no work to do and can return. */
        return;
    }
    
    /* child process will either become the browser or be dead & gone. */
    execl(browser, browser, path, NULL);
    /* since we are child, best not disturb GTK+ */
    fprintf(stderr, "Failed to exec '%s' with arg '%s': %s\n", browser, path, strerror(errno));
    exit(1);
#else
    if (spawnlp(P_NOWAIT, "start", "start", path, NULL) == -1 &&
        spawnlp(P_NOWAIT, "cmd", "/c", "start", path, NULL) == -1) {
            gnuitar_printf("Failed to run browser for help display: %s\n", strerror(errno));
    }
#endif
}

void
toggle_effect(GtkWidget *widget, effect_t *p)
{
    p->toggle = !p->toggle;
}

gint
delete_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    effect_t       *effect = data;
    int             i;

    i = effect_find(effect);
    if (i == -1) {
        gnuitar_printf("hmm, can't find effect to destroy in subwindow delete\n");
        return TRUE;
    }
    effect_delete(i);
    effect_destroy(effect);
    gtk_clist_remove(GTK_CLIST(processor), i);
    return TRUE;
}

static void
selectrow_processor(GtkWidget *widget, gint row, gint col,
		    GdkEventButton *event, gpointer data)
{
    curr_row = row;
}

static void
selectrow_bank(GtkWidget *widget, gint row, gint col,
		    GdkEventButton *event, gpointer data)
{
    gchar *fname;

    bank_row = row;
    if (event && event->type == GDK_2BUTTON_PRESS) {
        fname = gtk_clist_get_row_data(GTK_CLIST(bank), bank_row);
        if (fname == NULL)
            return;
        gtk_clist_select_row(GTK_CLIST(bank), bank_row, 0);
        load_pump(fname);
    }
}

static void
selectrow_effects(GtkWidget *widget, gint row, gint col,
		  GdkEventButton *event, gpointer data)
{
    effects_row = row;
    /* doubleclick */
    if (event && event->type == GDK_2BUTTON_PRESS) {
        add_pressed(NULL, data);
    }
}

static void
rowmove_processor(GtkWidget *widget, gint start, gint end, gpointer data)
{
    effect_move(start, end);
}

static void
up_pressed(GtkWidget *widget, gpointer data)
{
    gchar          *name_above, *name_selected;

    if (effect_move(curr_row, curr_row - 1)) {
	gtk_clist_freeze(GTK_CLIST(processor));
        gtk_clist_get_text(GTK_CLIST(processor), curr_row-1, 0, &name_above);
        gtk_clist_get_text(GTK_CLIST(processor), curr_row, 0, &name_selected);
        name_above    = strdup(name_above);
        name_selected = strdup(name_selected);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row-1, 0, name_selected);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row, 0, name_above);
        free(name_above);
        free(name_selected);

        gtk_clist_select_row(GTK_CLIST(processor), curr_row-1, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

static void
down_pressed(GtkWidget *widget, gpointer data)
{
    gchar          *name_below, *name_selected;

    if (effect_move(curr_row, curr_row + 1)) {
	gtk_clist_freeze(GTK_CLIST(processor));
        gtk_clist_get_text(GTK_CLIST(processor), curr_row, 0, &name_selected);
        gtk_clist_get_text(GTK_CLIST(processor), curr_row+1, 0, &name_below);
        name_selected = strdup(name_selected);
        name_below    = strdup(name_below);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row, 0, name_below);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row+1, 0, name_selected);
        free(name_selected);
        free(name_below);

        gtk_clist_select_row(GTK_CLIST(processor), curr_row+1, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

static void
del_pressed(GtkWidget *widget, gpointer data)
{
    effect_t *effect = effect_delete(curr_row);
    if (effect)
        effect_destroy(effect);
    gtk_clist_remove(GTK_CLIST(processor), curr_row);
    gtk_clist_select_row(GTK_CLIST(processor), curr_row, 0);
}

static void
add_pressed(GtkWidget *widget, gpointer data)
{
    int             idx;
    gchar          *name;
    GtkWidget      *known_effects = data;

    effect_t *effect = effect_create(effects_row);
    idx = effect_insert(effect, curr_row);

    gtk_clist_get_text(GTK_CLIST(known_effects), effects_row, 0, &name);
    name = strdup(name);

    gtk_clist_insert(GTK_CLIST(processor), idx, &name);
    free(name);
    gtk_clist_select_row(GTK_CLIST(processor), idx, 0);
}

/*
 * callback for gtk_set_pointer_data_full()
 */
static void
free_clist_ptr(gpointer data)
{
    if (data != NULL)
	free(data);
}

/* called from pump, too */
void bank_append_entry(char *fname) {
    char *name;
#ifdef _WIN32
    int             str_len,
                    i;
    char            drive[_MAX_DRIVE],
                    dir[_MAX_DIR],
                    ext[_MAX_EXT];
    
    /*
     * GTK for Windows have a bug related to non-ascii characters
     * in the strings. We replace all non-ascii chars to ? character.
     */
    fname = g_strdup(fname);
    _splitpath(fname, drive, dir, name, ext);
    str_len = strlen(name);
    for (i = 0; i < str_len; i++)
	if (!isascii(name[i])) {
	    name[i] = '?';
	}
#else
    fname = g_strdup(fname);
    name = basename(fname);
#endif
    gtk_clist_append(GTK_CLIST(bank), &name);
    gtk_clist_moveto(GTK_CLIST(bank), GTK_CLIST(bank)->rows - 1, 0, 0.5, 1.0);
    gtk_clist_set_row_data_full(GTK_CLIST(bank), GTK_CLIST(bank)->rows - 1,
				fname, free_clist_ptr);
}

static void
bank_perform_add(GtkWidget *widget, GtkFileSelection *filesel)
{
    char	    *name;
    /* this cast is to shut up const qualifier ignore due to
     * differences between gcc, mingw and msvc++. */ 
    name = (char *) gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    bank_append_entry(name);
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

static void
destroy_widget(GtkWidget *widget, GtkWidget *gallowman)
{
    gtk_widget_destroy(gallowman);
}

static void
bank_add_pressed(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;

    filesel = gtk_file_selection_new("Select preset to add");
    if (effects_dir)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel), effects_dir);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
		       "clicked", GTK_SIGNAL_FUNC(bank_perform_add),
		       filesel);
    gtk_signal_connect(GTK_OBJECT
		       (GTK_FILE_SELECTION(filesel)->cancel_button),
		       "clicked", GTK_SIGNAL_FUNC(destroy_widget),
		       GTK_WIDGET(filesel));

    gtk_widget_show(filesel);
}

static void
bank_del_pressed(GtkWidget * widget, gpointer data)
{
    int bank_len;
    bank_len=GTK_CLIST(bank)->rows;

    if (bank_row >= 0 && bank_row < bank_len) {
	gtk_clist_remove(GTK_CLIST(bank), bank_row);
	if (bank_row == bank_len - 1)
	    bank_row--;
	gtk_clist_select_row(GTK_CLIST(bank), bank_row, 0);
    }
}

static void
bank_switch_pressed(GtkWidget *widget, gpointer data)
{
    char           *fname;
    int             rows;

    rows = GTK_CLIST(bank)->rows;
    if (! rows)
        return;

    bank_row = (bank_row + 1) % rows;
    fname = gtk_clist_get_row_data(GTK_CLIST(bank), bank_row);
    if (fname == NULL)
        return;
    gtk_clist_select_row(GTK_CLIST(bank), bank_row, 0);
    load_pump(fname);
}

static void
bank_perform_save(GtkWidget * widget, GtkFileSelection * filesel)
{
    const gchar *path = gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    save_pump(path);
    if (effects_dir)
        g_free(effects_dir);
    effects_dir = g_strdup(path);
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

static void
bank_start_save(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;

    filesel = gtk_file_selection_new("Save current effect settings");
    if (effects_dir)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel), effects_dir);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
		       "clicked", GTK_SIGNAL_FUNC(bank_perform_save),
		       filesel);
    gtk_signal_connect(GTK_OBJECT
		       (GTK_FILE_SELECTION(filesel)->cancel_button),
		       "clicked", GTK_SIGNAL_FUNC(destroy_widget),
		       GTK_WIDGET(filesel));

    gtk_widget_show(filesel);
}

static void
bank_perform_load(GtkWidget * widget, GtkFileSelection * filesel)
{
    const gchar *path = gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    load_pump(path);
    if (effects_dir)
        g_free(effects_dir);
    effects_dir = g_strdup(path);
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

static void
bank_start_load(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;

    filesel = gtk_file_selection_new("Load effect settings");
    if (effects_dir)
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel), effects_dir);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
		       "clicked", GTK_SIGNAL_FUNC(bank_perform_load),
		       filesel);
    gtk_signal_connect(GTK_OBJECT
		       (GTK_FILE_SELECTION(filesel)->cancel_button),
		       "clicked", GTK_SIGNAL_FUNC(destroy_widget),
		       GTK_WIDGET(filesel));

    gtk_widget_show(filesel);
}

static void
start_tracker(GtkWidget * widget, GtkFileSelection * filesel)
{
    const char		*name;
    name = gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    if (name != NULL) {
	tracker_out(name);
	write_track = 1;
    }
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

static void
cancel_tracker(GtkWidget * widget, GtkFileSelection * filesel)
{
    gtk_widget_destroy(GTK_WIDGET(filesel));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tracker), 0);
}

static void
tracker_pressed(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;
    time_t          t;
    char            defaultname[80];

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        time(&t);
        strftime(defaultname, 80, "%d-%m-%Y_%H.%M.%S."
#if defined(HAVE_SNDFILE) || defined(_WIN32)
	    "wav"
#else
	    "raw"
#endif
	    , localtime(&t));

	filesel = gtk_file_selection_new("Enter track name");
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel),
					defaultname);
	gtk_signal_connect(GTK_OBJECT
			   (GTK_FILE_SELECTION(filesel)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(start_tracker),
			   filesel);
	gtk_signal_connect(GTK_OBJECT
			   (GTK_FILE_SELECTION(filesel)->cancel_button),
			   "clicked", GTK_SIGNAL_FUNC(cancel_tracker),
			   filesel);
	gtk_widget_show(filesel);
    } else {
	write_track = 0;
	tracker_done();
    }
}

typedef struct sample_params {
    GtkWidget      *rate;
    GtkWidget      *channels;
    GtkWidget      *latency;
#ifdef HAVE_ALSA
    GtkWidget      *alsadevice;
    GtkWidget      *alsadevice_label;
#endif
    GtkWidget      *dialog;
    GtkWidget      *latency_label;
    GtkWidget      *driver;
} sample_params_t;

void
set_vumeter_in_value(float power) {
    vumeter_in_power = (vumeter_in_power + power) / 2.f;
}

void
set_vumeter_out_value(float power) {
    vumeter_out_power = (vumeter_out_power + power) / 2.f;
}

static gboolean
timeout_update_vumeter_in(gpointer vumeter) {
    float power  = -96.f;

    if (vumeter_in_power != 0.f) {
        power = log(vumeter_in_power) / log(10) * 10.f;
        /* 16 bits hold ~96 dB resolution */
        if (power > 0.f)
            power = 0.f;
        if (power < -96.f)
            power = -96.f;
    }
    
    gtk_progress_set_value(GTK_PROGRESS(vumeter), power);
    return TRUE;
}

static gboolean
timeout_update_vumeter_out(gpointer vumeter) {
    float power  = -96.f;

    if (vumeter_out_power != 0.f) {
        power = log(vumeter_out_power) / log(10) * 10.f;
        /* 16 bits hold ~96 dB resolution */
        if (power > 0.f)
            power = 0.f;
        if (power < -96.f)
            power = -96.f;
    }
    
    gtk_progress_set_value(GTK_PROGRESS(vumeter), power);
    return TRUE;
}

static gboolean
timeout_update_bank(gpointer whatever) {
    int program;
    char *fname;

    program = midi_get_program();
    if (program < 0)
        return TRUE;
    bank_row = program;

    fname = gtk_clist_get_row_data(GTK_CLIST(bank), bank_row);
    if (fname == NULL)
        return TRUE;
    gtk_clist_select_row(GTK_CLIST(bank), bank_row, 0);
    load_pump(fname);
    return TRUE;
}

static void
update_master_volume(GtkAdjustment *adj, void *nothing) {
    master_volume = adj->value;
}

static void
update_input_volume(GtkAdjustment *adj, void *nothing) {
    input_volume = adj->value;
}

static void
populate_sparams_channels(GtkWidget *w)
{
    GList *channels_list = NULL, *iter = NULL;
    gchar *defchoice = NULL;

    defchoice = g_strdup_printf("%d in - %d out", n_input_channels, n_output_channels);

    if (audio_driver) {
        int i;
        for (i = 0; audio_driver->channels[i].in != 0; i += 1) {
            gchar *gtmp = g_strdup_printf("%d in - %d out", audio_driver->channels[i].in, audio_driver->channels[i].out);
            channels_list = g_list_append(channels_list, gtmp);
        }
    }
    gtk_combo_set_popdown_strings(GTK_COMBO(w), channels_list);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(w)->entry), defchoice);

    for (iter = g_list_first(channels_list); iter != NULL; iter = g_list_next(iter))
        g_free(iter->data);
    g_list_free(channels_list);
    g_free(defchoice);
}

static void
update_latency_label(GtkWidget *widget, gpointer data)
{
    gchar          *gtmp;
    sample_params_t *sparams = data;

    update_sampling_params(widget, data);
#ifndef _WIN32
    gtmp = g_strdup_printf("%.2f ms", 1000.0 * (buffer_size * (fragments-1)) / sample_rate);
#else
    gtmp = g_strdup_printf("%.2f ms", 1000.0 * buffer_size / sample_rate);
#endif
    gtk_label_set_text(GTK_LABEL(sparams->latency_label), gtmp);
    free(gtmp);
}

static void
update_driver(GtkWidget *widget, gpointer data)
{
    const char *tmp;
    sample_params_t *sparams = (sample_params_t *) data;

    tmp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sparams->driver)->entry));
    if (tmp == NULL)
	return;
    set_audio_driver_from_str(tmp);
    if (strcmp(tmp, "MMSystem") == 0) {
        buffer_size = pow(2, (int) (log(buffer_size) / log(2)));
    }
    populate_sparams_channels(sparams->channels);
}

#ifdef _WIN32
static void
update_threshold(GtkWidget * widget, gpointer threshold)
{
    overrun_threshold=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(threshold));
}
#endif

/*
 * Sampling parameters dialog
 */
static void
sample_dlg(GtkWidget *widget, gpointer data)
{
    static sample_params_t sparams;
    GtkWidget      *sp_table;
#ifdef _WIN32
    GtkWidget      *threshold;
    GtkWidget      *threshold_label;
    GtkWidget      *threshold_fragments;
    GtkObject      *threshold_adj;
#endif
    GtkWidget      *rate_label;
    GtkWidget      *channels_label;
        
    GtkWidget      *latency_label;
    GtkWidget	   *driver_label;
    GtkWidget      *ok;
    GtkWidget      *cancel;
    GtkWidget      *group;
    GtkWidget      *vpack,
                   *buttons_pack;
    GList          *sample_rates = NULL;
#ifdef HAVE_ALSA
    GList          *alsadevice_list = NULL;
#endif
    GList          *drivers_list = NULL;
    GtkObject      *latency_adj;
    GtkSpinButton  *dummy1;
    GtkEntry       *dummy2;

    sparams.dialog = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_window_set_title(GTK_WINDOW(sparams.dialog),
			 "Sampling Parameters");

    gtk_container_set_border_width(GTK_CONTAINER(sparams.dialog), 5);

    group = gtk_frame_new("Sampling Parameters");
    vpack = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vpack), group, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(sparams.dialog), vpack);

    sp_table = gtk_table_new(4, 5, FALSE);
    gtk_container_add(GTK_CONTAINER(group), sp_table);

#define TBLOPT  __GTKATTACHOPTIONS(GTK_FILL|GTK_EXPAND|GTK_SHRINK)

    rate_label = gtk_label_new("Sampling rate:");
    gtk_misc_set_alignment(GTK_MISC(rate_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), rate_label, 0, 1, 0, 1,
                     TBLOPT, TBLOPT, 3, 3);
#ifdef HAVE_ALSA
    sparams.alsadevice_label = gtk_label_new("Alsa Device:");
    gtk_misc_set_alignment(GTK_MISC(sparams.alsadevice_label), 0, 0.5);
    
    gtk_table_attach(GTK_TABLE(sp_table), sparams.alsadevice_label, 0, 1, 2, 3,
                     TBLOPT, TBLOPT, 3, 3);
		     
    sparams.alsadevice = gtk_combo_new();
    
    alsadevice_list = g_list_append(alsadevice_list, "hw:0,0");
    alsadevice_list = g_list_append(alsadevice_list, "default");
    alsadevice_list = g_list_append(alsadevice_list, "guitar");
    alsadevice_list = g_list_append(alsadevice_list, "surround40");
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.alsadevice), alsadevice_list);
    g_list_free(alsadevice_list);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.alsadevice)->entry), alsadevice_str);
		       //g_strdup_printf("%d", sample_rate));
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.alsadevice)->entry), TRUE);
    
    //gtk_misc_set_alignment(GTK_MISC(alsadevice), 0, 0.5);
    
    gtk_table_attach(GTK_TABLE(sp_table),  sparams.alsadevice, 1, 2, 2, 3,
                     TBLOPT, TBLOPT, 3, 3);
		     
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.alsadevice)->entry,
        "Name of ALSA output device. (Used only with ALSA driver)", NULL);
#endif
    
    sparams.rate = gtk_combo_new();
    /* these may also be driver dependant but let's leave them as is for now */
    sample_rates = g_list_append(sample_rates, "96000");
    sample_rates = g_list_append(sample_rates, "88200");
    sample_rates = g_list_append(sample_rates, "48000");
    sample_rates = g_list_append(sample_rates, "44100");
    sample_rates = g_list_append(sample_rates, "22050");
    sample_rates = g_list_append(sample_rates, "16000");
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.rate), sample_rates);
    g_list_free(sample_rates);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.rate)->entry),
		       g_strdup_printf("%d", sample_rate));
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.rate)->entry), FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.rate, 1, 2, 0, 1,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.rate)->entry,"This is the current sampling rate.",NULL);

    channels_label = gtk_label_new("Channels:");
    gtk_misc_set_alignment(GTK_MISC(channels_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), channels_label, 2, 3, 0, 1,
                     TBLOPT, TBLOPT, 3, 3);
    sparams.channels = gtk_combo_new();

    populate_sparams_channels(sparams.channels);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.channels)->entry),
			   FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.channels, 3, 4, 0, 1,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.channels)->entry,"Mono/Stereo/Quadrophonic",NULL);

    latency_label = gtk_label_new("Fragment size:");
    gtk_misc_set_alignment(GTK_MISC(latency_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), latency_label, 2, 3, 1, 2,
                     TBLOPT, TBLOPT, 3, 3);
    latency_adj =
	gtk_adjustment_new(buffer_size, MIN_BUFFER_SIZE, MAX_BUFFER_SIZE,
			   MIN_BUFFER_SIZE, MIN_BUFFER_SIZE, 0);
    sparams.latency =
	gtk_spin_button_new(GTK_ADJUSTMENT(latency_adj), 1, 0);
    dummy1 = GTK_SPIN_BUTTON(sparams.latency);
    dummy2 = &(dummy1->entry);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.latency, 3, 4, 1, 2,
                     TBLOPT, TBLOPT, 3, 3);

    driver_label = gtk_label_new("Audio Driver:");
    gtk_misc_set_alignment(GTK_MISC(driver_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), driver_label, 0, 1, 1, 2,
                     TBLOPT, TBLOPT, 3, 3);
    sparams.driver = gtk_combo_new();
#ifdef HAVE_JACK
    drivers_list = g_list_append(drivers_list, "JACK");
#endif
#ifdef HAVE_ALSA
    drivers_list = g_list_append(drivers_list, "ALSA");
#endif
#ifdef HAVE_OSS
    drivers_list = g_list_append(drivers_list, "OSS");
#endif
#ifdef HAVE_WINMM
    drivers_list = g_list_append(drivers_list, "MMSystem");
#endif
#ifdef HAVE_DSOUND
    drivers_list = g_list_append(drivers_list, "DirectX");
#endif

    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.driver), drivers_list);
    g_list_free(drivers_list);
    
    if (audio_driver)
        gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.driver)->entry), audio_driver->str);

    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.driver)->entry),
			   FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.driver, 1, 2, 1, 2,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.driver)->entry,
	"Sound driver is an API that you use to capture/playback sound.",NULL);

    latency_label = gtk_label_new("Latency:");
    gtk_misc_set_alignment(GTK_MISC(latency_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), latency_label, 2, 3, 2, 3,
                     TBLOPT, TBLOPT, 3, 3);
    sparams.latency_label = gtk_label_new("- ms");
    gtk_misc_set_alignment(GTK_MISC(sparams.latency_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.latency_label, 3, 4, 2, 3,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,sparams.latency,"The fragment size is the number of samples " \
	"that the sound driver reads by one time. " \
	"The smaller is the fragment size, the lower is the latency, " \
	"and vice versa. However, small fragment size may cause buffer overruns. " \
	"If you encounter multiple buffer overruns, " \
	"try to increase this setting.",NULL);
#ifdef _WIN32
    /* threshold spin button */
    threshold_label = gtk_label_new("Overrun threshold:");
    gtk_misc_set_alignment(GTK_MISC(threshold_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), threshold_label, 2, 3, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    threshold_adj =
	gtk_adjustment_new(overrun_threshold, 0, 200,
			   1, 1, 0);
    threshold =
	gtk_spin_button_new(GTK_ADJUSTMENT(threshold_adj), 1, 0);
    dummy1 = GTK_SPIN_BUTTON(threshold);
    dummy2 = &(dummy1->entry);
    gtk_entry_set_editable(dummy2, FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), threshold, 3, 4, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,threshold,"Large value will force buffer overruns " \
	"to be ignored. If you encounter heavy overruns, " \
	"especially with autowah, decrease this to 1. " \
	"(for hackers: this is the number of fragments that are allowed to be lost).",NULL);
    threshold_fragments = gtk_label_new("fragments");
    gtk_misc_set_alignment(GTK_MISC(threshold_fragments), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), threshold_fragments, 3, 4, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_signal_connect(GTK_OBJECT(threshold_adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_threshold), threshold);
#endif

    buttons_pack = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(buttons_pack), 15);
    ok = gtk_button_new_with_label("OK");
    gtk_box_pack_start(GTK_BOX(buttons_pack), ok, TRUE, FALSE, 0);

    gtk_signal_connect(GTK_OBJECT(ok), "clicked",
		       GTK_SIGNAL_FUNC(update_sampling_params_and_close_dialog), &sparams);
    gtk_signal_connect(GTK_OBJECT(latency_adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_latency_label), &sparams);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sparams.channels)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_latency_label),
		       &sparams);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sparams.rate)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_latency_label),
		       &sparams);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sparams.driver)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_driver),
		       &sparams);

    cancel = gtk_button_new_with_label("Cancel");
    gtk_box_pack_start(GTK_BOX(buttons_pack), cancel, TRUE, FALSE, 0);
    gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(sparams.dialog));
    gtk_box_pack_start(GTK_BOX(vpack), buttons_pack, TRUE, TRUE, 0);
    gtk_widget_grab_focus(ok);

    update_latency_label(NULL, &sparams);
    gtk_widget_show_all(sparams.dialog);
}

static void
update_sampling_params(GtkWidget * dialog, gpointer data)
{
    int             tmp1, tmp2;
    sample_params_t *sparams = data;
#ifdef HAVE_ALSA
    const char *tmp=NULL;

    tmp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sparams->alsadevice)->entry));
    if (tmp == NULL || strlen(tmp) == 0) {
	strcpy(alsadevice_str, "default");
    } else {
	strncpy(alsadevice_str, tmp, sizeof(alsadevice_str)-1);
    }
#endif    
    buffer_size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sparams->latency));
    /* for certain audio drivers, make the fragment size to be a multiple
     * of the MIN_BUFFER_SIZE */
    if (audio_driver) {
        if (strcmp(audio_driver->str, "OSS") == 0
            || strcmp(audio_driver->str, "MMSystem") == 0) {
            buffer_size -= buffer_size % MIN_BUFFER_SIZE;
            gtk_adjustment_set_value(gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(sparams->latency)),
                                                                    buffer_size);
        }
    }
    sscanf(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sparams->channels)->entry)),
           "%d in - %d out", &tmp1, &tmp2);
    n_input_channels = tmp1;
    n_output_channels = tmp2;
    
    /* if alsa and surround40 selected, force 4-channel output. */
    if (audio_driver && strcmp(audio_driver->str, "ALSA") == 0
        && strcmp(alsadevice_str, "surround40") == 0)
        n_output_channels = 4;

    sample_rate = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sparams->rate)->entry)));
}


static void
update_sampling_params_and_close_dialog(GtkWidget *dialog, gpointer data)
{
    sample_params_t *sparams = data;

    update_sampling_params(dialog, data);
    gtk_widget_destroy(GTK_WIDGET(sparams->dialog));
}

static void
start_stop(GtkWidget *widget, gpointer data)
{
    int error;

    /* without audio driver, we can't allow user to proceed */
    if (! audio_driver) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), 0);
	gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")), TRUE);
	gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child),
                           "ERROR\nNo audio driver");
        return;
    }
 
    if (! audio_driver->enabled) {
        /* don't allow user to press us into active state if we can't play */
        if (! GTK_TOGGLE_BUTTON(widget)->active) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), 0);
            /* GTK+ will call us again in response of set_active event */
	    gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")),
				 !audio_driver->enabled);
            gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child),
                    audio_driver->enabled ? "Stop" : "ERROR\nSee debug log");
            return;
        }
    }

    if (GTK_TOGGLE_BUTTON(widget)->active) {
	if ((error = audio_driver->init()) != ERR_NOERROR)
            gnuitar_printf("warning: unable to begin audio processing (code %d)\n", error);

	gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")),
				 !audio_driver->enabled);
	gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child),
                    audio_driver->enabled ? "Stop" : "ERROR\nSee debug log");
        if (!audio_driver->enabled)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), 0);
    } else {
	audio_driver->finish();
	gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")),
				 TRUE);
	gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child), "Start");
    }
}

void
init_gui(void)
{
    gchar          *tmp;
    GtkAccelGroup  *accel_group;
    GtkWidget      *vumeter_in;
    GtkWidget      *vumeter_out;
    GtkWidget      *processor_scroll;
    GtkWidget	   *master;
    GtkWidget      *input;
    GtkWidget      *mainWnd;
    GtkWidget      *tbl;
    GtkWidget      *menuBar;
    GtkWidget      *known_effects;
    GtkWidget      *effect_scroll;
    GtkWidget      *bank_scroll;
    GtkWidget      *bank_add;
    GtkWidget      *bank_switch;
    GtkWidget      *bank_del;
    GtkWidget      *up;
    GtkWidget      *down;
    GtkWidget      *del;
    GtkWidget      *add;
    GtkWidget      *start;
    GtkWidget      *volume_label;
    GtkWidget      *input_label;

    gint            nmenu_items =
	sizeof(mainGui_menu) / sizeof(mainGui_menu[0]);
    char     *processor_titles[] = { "Current effects", NULL };
    char     *effects_titles[] = { "Known effects", NULL };
    char     *bank_titles[] = { "Preset list", NULL };
#ifdef HAVE_GTK
    GdkFont        *new_font;
#endif

#ifdef _WIN32
    HICON           app_icon,
                    small_icon;
    HWND            window;
    HMODULE         me;
#else
    GdkPixmap      *app_icon;
    GdkBitmap      *mask;
#endif
    GtkStyle       *style;
    
    tmp = discover_preset_path();
    effects_dir = g_strdup_printf("%s" FILESEP "presetname.gnuitar", tmp);
    g_free(tmp);

    mainWnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(mainWnd, 700, 450);
    tbl = gtk_table_new(7, 6, FALSE);
    gtk_signal_connect(GTK_OBJECT(mainWnd), "destroy",
		       GTK_SIGNAL_FUNC(quit), NULL);


    /*
     * make menu
     */
    accel_group = gtk_accel_group_new();

    item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
					accel_group);
    gtk_item_factory_create_items(item_factory, nmenu_items, mainGui_menu, NULL);
    gtk_window_add_accel_group(GTK_WINDOW(mainWnd), accel_group);
    menuBar = gtk_item_factory_get_widget(item_factory, "<main>");

    gtk_table_attach(GTK_TABLE(tbl), menuBar, 0, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(0), 0, 0);
    /*
     * disable options menu
     */
    if (audio_driver)
        gtk_widget_set_sensitive(GTK_WIDGET
                                 (gtk_item_factory_get_widget
                                  (item_factory, "/Options/Options")), FALSE);



    tooltips=gtk_tooltips_new();

    processor = gtk_clist_new_with_titles(1, processor_titles);
    gtk_clist_set_selection_mode(GTK_CLIST(processor),
				 GTK_SELECTION_SINGLE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(processor), 0, TRUE);
    processor_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(processor_scroll), processor);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(processor_scroll),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    gtk_tooltips_set_tip(tooltips,processor,"This area contains a list of current applied effects." \
	"You can use Add/Up/Down/Delete buttons to control this list.",NULL);

    known_effects = gtk_clist_new_with_titles(1, effects_titles);
    gtk_clist_set_selection_mode(GTK_CLIST(known_effects),
				 GTK_SELECTION_SINGLE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(known_effects), 0, TRUE);
    effect_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(effect_scroll), known_effects);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(effect_scroll),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    gtk_tooltips_set_tip(tooltips,known_effects,"This area contains a list of available effects." \
	"To apply effects to the sound, you need to add the effects" \
	"to the \"Current Effects\" list." \
	"You can use Add/Up/Down/Delete buttons to do this.",NULL);
    effect_list_add_to_clist(known_effects);

    bank = gtk_clist_new_with_titles(1, bank_titles);
    gtk_clist_set_selection_mode(GTK_CLIST(bank), GTK_SELECTION_SINGLE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(bank), 0, TRUE);
    bank_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(bank_scroll), bank);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(bank_scroll),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    gtk_tooltips_set_tip(tooltips,bank,"This area contains the available presets." \
	"Use \"Add preset\" button to add more presets to the list." \
	"Use \"Cycle presets\" to go through the list of available presets.", NULL);

    gtk_table_attach(GTK_TABLE(tbl), processor_scroll, 3, 4, 1, 5,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), effect_scroll, 5, 6, 1, 5,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), bank_scroll, 1, 2, 1, 5,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND), 0, 0);


    gtk_container_add(GTK_CONTAINER(mainWnd), tbl);
    gtk_window_set_title(GTK_WINDOW(mainWnd), "GNUitar");

    bank_add = gtk_button_new_with_label("Add preset...");
    gtk_tooltips_set_tip(tooltips,bank_add,"Load a file into the presets list.", NULL);

    style = gtk_widget_get_style(bank_add);
#ifdef HAVE_GTK
    new_font =
	gdk_fontset_load
	("-adobe-helvetica-medium-r-normal--*-100-*-*-*-*-*-*");
    if (new_font != NULL) {
	gdk_font_unref(style->font);
	style->font = new_font;
	gtk_widget_set_style(bank_add, style);
    }
#elif defined(HAVE_GTK2)
    pango_font_description_set_size(style->font_desc,10);
    pango_font_description_set_family(style->font_desc,"helvetica");
    pango_font_description_set_style(style->font_desc,PANGO_STYLE_NORMAL);
    pango_font_description_set_weight(style->font_desc,PANGO_WEIGHT_NORMAL);
#endif
    gtk_clist_set_reorderable(GTK_CLIST(processor), TRUE);

    bank_del = gtk_button_new_with_label("Remove preset");
    gtk_tooltips_set_tip(tooltips,bank_del,"Remove preset from the presets list.",NULL);
    bank_switch = gtk_button_new_with_label("Cycle\npresets");
    gtk_tooltips_set_tip(tooltips,bank_switch,"Cycle through effect presets",NULL);
    up = gtk_button_new_with_label("Up");
    gtk_tooltips_set_tip(tooltips,up,"Move the currently selected effect up.", NULL);
    down = gtk_button_new_with_label("Down");
    gtk_tooltips_set_tip(tooltips,down,"Move the currently selected effect down.", NULL);
    del = gtk_button_new_with_label("Delete");
    gtk_tooltips_set_tip(tooltips,del,"Delete the currently selected effect", NULL);
    add = gtk_button_new_with_label("<< Add");
    gtk_tooltips_set_tip(tooltips,add,"Add the selected effect to Current Effects.", NULL);
    tracker = gtk_check_button_new_with_label("Record audio...");
    gtk_tooltips_set_tip(tooltips,tracker,"Toggle to begin recording audio.", NULL);
    if (audio_driver && audio_driver->enabled) {
        start = gtk_toggle_button_new_with_label("Stop");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start), 1);
    } else {
        start = gtk_toggle_button_new_with_label("Start\n");
    }
    gtk_tooltips_set_tip(tooltips,start,"Pause and resume audio processing.", NULL);
    vumeter_in = gtk_progress_bar_new();
    gtk_progress_set_format_string(GTK_PROGRESS(vumeter_in), "%v dB");
    gtk_progress_configure(GTK_PROGRESS(vumeter_in), -96, -96, 0);
    gtk_progress_set_show_text(GTK_PROGRESS(vumeter_in), TRUE);
    vumeter_out = gtk_progress_bar_new();
    gtk_progress_set_format_string(GTK_PROGRESS(vumeter_out), "%v dB");
    gtk_progress_configure(GTK_PROGRESS(vumeter_out), -96, -96, 0);
    gtk_progress_set_show_text(GTK_PROGRESS(vumeter_out), TRUE);
    adj_master = gtk_adjustment_new(master_volume, -30.0, 30.0, 0.1, 1.0, 0.0);
    adj_input = gtk_adjustment_new(input_volume, -30.0, 30.0, 0.1, 1.0, 0.0);
    

    master = gtk_hscale_new(GTK_ADJUSTMENT(adj_master));
    input = gtk_hscale_new(GTK_ADJUSTMENT(adj_input));
    
    gtk_tooltips_set_tip(tooltips, master, "Change output gain (post-amp)", NULL);
    gtk_tooltips_set_tip(tooltips, input, "Change input gain (pre-amp)", NULL);
    
    gtk_scale_set_value_pos(GTK_SCALE(master), GTK_POS_RIGHT);
    gtk_scale_set_value_pos(GTK_SCALE(input), GTK_POS_RIGHT);
    
    volume_label = gtk_label_new("Master volume:");
    input_label = gtk_label_new("Input volume:");

    /* debug window creation */    
    status_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(status_window),
				   GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_container_set_border_width(GTK_CONTAINER(status_window), 10);
    
#ifdef HAVE_GTK2
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(status_window), GTK_SHADOW_IN);
#endif
    status_text = gnuitar_gtk_text_view_new(GTK_CONTAINER(status_window));
    /* Side effect: update container data with text buffered before the window existed.
     * Scrolling to window end doesn't work though. Blame this on GTK+. My guess is that
     * the widgets are not properly realized yet and the scrolling adjustment isn't
     * updated until everything has been drawn properly. */
    gnuitar_printf("");
    
    gtk_widget_set_sensitive(GTK_WIDGET(
                                gtk_item_factory_get_widget(
                                    item_factory, "/Options/Options")),
			     !audio_driver || !audio_driver->enabled);
    gtk_label_set_text(GTK_LABEL(GTK_BIN(start)->child),
                    audio_driver && audio_driver->enabled ? "Stop" : "ERROR\nPush to retry");
    
    gtk_table_attach(GTK_TABLE(tbl), status_window, 0 ,7, 7, 9,
    		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL),
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL), 5, 5);

    gtk_table_attach(GTK_TABLE(tbl), bank_add, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), bank_del, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), bank_switch, 0, 1, 4, 5,
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL),
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), start, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL),
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL), 5, 5);

    gtk_table_attach(GTK_TABLE(tbl), up, 2, 3, 1, 2, __GTKATTACHOPTIONS(0),
		     __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), down, 2, 3, 2, 3,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), del, 2, 3, 3, 4,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), add, 4, 5, 1, 2,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), tracker, 0, 1, 5, 7,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), vumeter_in, 1, 3, 5, 6, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), vumeter_out, 1, 3, 6, 7, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), input, 4, 6, 5, 6, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), master, 4, 6, 6, 7, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);

    gtk_table_attach(GTK_TABLE(tbl), volume_label, 3, 4, 6, 7, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), input_label, 3, 4, 5, 6, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);

    gtk_signal_connect(GTK_OBJECT(bank_add), "clicked",
		       GTK_SIGNAL_FUNC(bank_add_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(bank_del), "clicked",
		       GTK_SIGNAL_FUNC(bank_del_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(bank_switch), "clicked",
		       GTK_SIGNAL_FUNC(bank_switch_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(start), "clicked",
		       GTK_SIGNAL_FUNC(start_stop), NULL);
    gtk_signal_connect(GTK_OBJECT(up), "clicked",
		       GTK_SIGNAL_FUNC(up_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(down), "clicked",
		       GTK_SIGNAL_FUNC(down_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(del), "clicked",
		       GTK_SIGNAL_FUNC(del_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(add), "clicked",
		       GTK_SIGNAL_FUNC(add_pressed), known_effects);
    gtk_signal_connect(GTK_OBJECT(tracker), "clicked",
		       GTK_SIGNAL_FUNC(tracker_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(processor), "select_row",
		       GTK_SIGNAL_FUNC(selectrow_processor), NULL);
    gtk_signal_connect(GTK_OBJECT(bank), "select_row",
		       GTK_SIGNAL_FUNC(selectrow_bank), NULL);
    gtk_signal_connect(GTK_OBJECT(processor), "row_move",
		       GTK_SIGNAL_FUNC(rowmove_processor), NULL);
    gtk_signal_connect(GTK_OBJECT(known_effects), "select_row",
		       GTK_SIGNAL_FUNC(selectrow_effects), known_effects);
    gtk_signal_connect(GTK_OBJECT(adj_master), "value_changed",
		       GTK_SIGNAL_FUNC(update_master_volume), NULL);		       
    gtk_signal_connect(GTK_OBJECT(adj_input), "value_changed",
		       GTK_SIGNAL_FUNC(update_input_volume), NULL);
    gtk_widget_show_all(mainWnd);

    g_timeout_add(VU_UPDATE_INTERVAL,   timeout_update_vumeter_in,  vumeter_in );
    g_timeout_add(VU_UPDATE_INTERVAL,   timeout_update_vumeter_out, vumeter_out);
    g_timeout_add(BANK_UPDATE_INTERVAL, timeout_update_bank,        NULL);

    /*
     * Attach icon to the window
     */
#ifdef _WIN32
    window = GetActiveWindow();
    me = GetModuleHandle(NULL);
    app_icon = LoadIcon(me, MAKEINTRESOURCE(APP_ICON));
    small_icon = LoadIcon(me, MAKEINTRESOURCE(SMALL_ICON));
    if (app_icon)
	SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM) app_icon);
    if (small_icon)
	SendMessage(window, WM_SETICON, ICON_SMALL, (LPARAM) small_icon);

#else
    style = gtk_widget_get_style(mainWnd);
    app_icon = gdk_pixmap_create_from_xpm_d(mainWnd->window, &mask,
					    &style->white, (gchar **) gnuitar_xpm);
    gdk_window_set_icon(mainWnd->window, mainWnd->window, app_icon, mask);
#endif
}
