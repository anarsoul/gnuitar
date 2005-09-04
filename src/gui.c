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
#ifdef _WIN32
#    include <io.h>
#    include <ctype.h>
#    include <windows.h>
#    include <process.h>
#    include "resource.h"
#else
#    include <libgen.h>
#    include <math.h>
#    include <unistd.h>
#    include <pthread.h>
#    include "gnuitar.xpm"
#endif

#include "gui.h"
#include "pump.h"
#include "main.h"
#include "tracker.h"
#include "utils.h"

#ifdef _WIN32
extern short dsound;
#endif

#define VU_UPDATE_INTERVAL 25.0    /* ms */

void            bank_start_save(GtkWidget *, gpointer);
void            bank_start_load(GtkWidget *, gpointer);
void            sample_dlg(GtkWidget *, gpointer);
void            update_sampling_params(GtkWidget *, gpointer);
void            update_sampling_params_and_close_dialog(GtkWidget *, gpointer);
void            quit(GtkWidget *, gpointer);
void            about_dlg(void);
void            help_contents(void);

static GtkItemFactoryEntry mainGui_menu[] = {
    {"/_File", "<alt>F", NULL, 0, "<Branch>"},

    {"/File/_Open Layout", "<control>O",
     (GtkSignalFunc) bank_start_load, 0, NULL},
    {"/File/_Save Layout", "<control>S", (GtkSignalFunc) bank_start_save,
     0, NULL},
    {"/File/sep1", NULL, NULL, 0, "<Separator>"},
    {"/File/E_xit", "<control>Q", (GtkSignalFunc) quit, 0, NULL},
    {"/_Options", "<alt>O", NULL, 0, "<Branch>"},
    {"/Options/O_ptions", "<control>P",
     (GtkSignalFunc) sample_dlg, 0, NULL},
    {"/_Help", NULL, NULL, 0, "<LastBranch>"},
    {"/_Help/Contents", NULL, (GtkSignalFunc) help_contents, 0, NULL},
    {"/_Help/About", NULL, (GtkSignalFunc) about_dlg, 0, NULL}
};
GtkWidget      *mainWnd;
GtkItemFactory *item_factory;
GtkWidget      *tbl;
GtkWidget      *menuBar;
GtkWidget      *processor;
GtkWidget      *processor_scroll;
GtkWidget      *known_effects;
GtkWidget      *effect_scroll;
GtkWidget      *bank;
GtkWidget      *bank_scroll;
GtkWidget      *bank_add;
GtkWidget      *bank_switch;
GtkWidget      *up;
GtkWidget      *down;
GtkWidget      *del;
GtkWidget      *add;
GtkWidget      *tracker;
GtkWidget      *start;
GtkTooltips    *tooltips;
double		master_volume;
gint            curr_row = -1;	/*
				 * current row in processor list
				 */
gint            effects_row = -1;	/*
					 * current row in known effects list
					 */
gint            bank_row = -1;	/*
				 * current row in bank list
				 */
extern my_mutex effectlist_lock;/* sorry for this - when I'm trying to export it in pump.h,
                                 * MSVC 6.0 complains: identifier effectlist_lock: */

/*
 * Cleaning and quit from application
 */
void
quit(GtkWidget * widget, gpointer data)
{
    gtk_main_quit();
}

void
about_dlg(void)
{
    GtkWidget      *about;
    GtkWidget      *about_label;
    GtkWidget      *vbox;
    GtkWidget      *scrolledwin;
    GtkWidget      *text;
    GtkWidget      *ok_button;

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
    vbox = gtk_vbox_new(FALSE, 8);
    gtk_container_add(GTK_CONTAINER(about), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), about_label, FALSE, FALSE, 0);

    scrolledwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwin),
				   GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledwin, TRUE, TRUE, 0);

    text = gtk_text_new(gtk_scrolled_window_get_hadjustment
			(GTK_SCROLLED_WINDOW(scrolledwin)),
			gtk_scrolled_window_get_vadjustment
			(GTK_SCROLLED_WINDOW(scrolledwin)));
    gtk_container_add(GTK_CONTAINER(scrolledwin), text);

    gtk_text_freeze(GTK_TEXT(text));

    gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, DISCLAIMER, -1);

    gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
		    "This program is distributed in the hope that it will be useful,\n"
		    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
		    "See the GNU General Public License for more details.\n\n",
		    -1);

    gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
		    "You should have received a copy of the GNU General Public License\n"
		    "along with this program; if not, write to the Free Software\n"
		    "Foundation, Inc., 59 Temple Place - Suite 330, Boston,\n"
		    "MA 02111-1307, USA.", -1);

    gtk_text_thaw(GTK_TEXT(text));

    ok_button = gtk_button_new_with_label("OK");
    gtk_box_pack_end(GTK_BOX(vbox), ok_button, FALSE, FALSE, 0);
    gtk_signal_connect_object(GTK_OBJECT(ok_button), "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(about));
    gtk_widget_grab_focus(ok_button);

    gtk_widget_show_all(about);
}

void
help_contents(void)
{
    char            path[2048] = "";
#ifndef _WIN32
    pid_t           pid;
    char            browser[2048] = "";
    char           *env_browser = NULL;
    char           *browsers[7] = {
	"/usr/bin/netscape",
	"/usr/bin/netscape-navigator",
	"/usr/bin/netscape-communicator",
	"/usr/bin/konqueror",
	"/usr/bin/mozilla",
	"/usr/bin/links",
	"/usr/bin/lynx"
    };
    char           *docs[7] = {
	"/usr/share/doc/gnuitar-" VERSION "/docs/index.html",
	"/usr/share/gnuitar/docs/index.html",
	"/usr/doc/gnuitar-" VERSION "/docs/index.html",
	"/usr/local/doc/gnuitar-" VERSION "/docs/index.html",
	"/usr/share/doc/gnuitar/docs/index.html",
	"/usr/doc/gnuitar/docs/index.html",
	"/usr/local/doc/gnuitar/docs/index.html"
    };
    int             i;

    /*
     * first get environment variable for a browser
     */
    env_browser = getenv("BROWSER");
    /*
     * if there is no preference, trying to guess
     */
    if (env_browser == NULL) {
	for (i = 0; i < 7; i++) {
	    if (access(browsers[i], X_OK) == 0) {
		strcpy(browser, browsers[i]);
		break;
	    }
	}
    } else {
	strcpy(browser, env_browser);
    }

    if (strcmp(browser, "") != 0) {
	pid = fork();
	if (pid == -1) {
	    perror("fork");
	    return;
	}
	/*
	 * child process
	 */
	if (pid == 0) {
	    for (i = 0; i < 7; i++) {
		if (access(docs[i], R_OK) == 0) {
		    strcpy(path, docs[i]);
		    break;
		}
	    }
	    if (strcmp(path, "") == 0) {
		getcwd(path, sizeof(path));
		strcat(path, "/../docs/index.html");
		if (access(path, R_OK) != 0) {
		    getcwd(path, sizeof(path));
		    strcat(path, "/index.html");
		    if (access(path, R_OK) != 0) {
			strcpy(path, "");
		    }
		}
	    }
	    if (strcmp(path, "") != 0)
		execl(browser, browser, path, NULL);
	}
    }
#else
    strcpy(path, "..\\docs\\index.html");
    if (access(path, R_OK) != 0) {
	strcpy(path, "index.html");
	if (access(path, R_OK) != 0) {
	    strcpy(path, "docs\\index.html");
	    if (access(path, R_OK) != 0) {
		strcpy(path, "");
	    }
	}
    }
    if (strcmp(path, "") != 0) {
	if (spawnlp(P_NOWAIT, "start", "start", path, NULL) == -1)
            if (spawnlp(P_NOWAIT, "cmd", "/c", "start", path, NULL) == -1)
	        perror("spawn");
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
    struct effect  *p = data;
    int             i;
    
    my_lock_mutex(effectlist_lock);
    for (i = 0; i < n; i++) {
        if (effects[i] == p)
            break;
    }
    if (i == n) {
        fprintf(stderr, "hmm, can't find effect to destroy in subwindow delete\n");
        return TRUE;
    }
    effects[i]->proc_done(effects[i]);
    gtk_clist_freeze(GTK_CLIST(processor));
    gtk_clist_remove(GTK_CLIST(processor), i);
    gtk_clist_thaw(GTK_CLIST(processor));
    for (; i < n-1; i += 1)
        effects[i] = effects[i+1];
    effects[n--] = NULL;
    my_unlock_mutex(effectlist_lock);

    return TRUE;
}

void
selectrow_processor(GtkWidget * widget, gint row, gint col,
		    GdkEventButton * event, gpointer data)
{
    curr_row = row;
}

void
selectrow_effects(GtkWidget * widget, gint row, gint col,
		  GdkEventButton * event, gpointer data)
{
    effects_row = row;
}

void rowmove_processor(GtkWidget * widget, gint start, gint end, gpointer data)
{
    effect_t   *swap;
    int         i;
    
    assert(start >= 0);
    assert(end <= n);
 
    my_lock_mutex(effectlist_lock);
    if (start < end) {
        swap = effects[start];
        for (i = start; i < end; i += 1)
            effects[i] = effects[i+1];
        effects[end] = swap;
    } else if (start > end) {
        swap = effects[start];
        for (i = start; i > end; i -= 1)
            effects[i] = effects[i-1];
        effects[end] = swap;
    }
    my_unlock_mutex(effectlist_lock);
}


void
up_pressed(GtkWidget *widget, gpointer data)
{
    effect_t       *swap;
    gchar          *name_above, *name_selected;
    
    if (curr_row > 0 && curr_row < n) {
	swap = effects[curr_row - 1];

        my_lock_mutex(effectlist_lock);
        effects[curr_row - 1] = effects[curr_row];
	effects[curr_row] = swap;
	my_unlock_mutex(effectlist_lock);

	gtk_clist_freeze(GTK_CLIST(processor));
        gtk_clist_get_text(GTK_CLIST(processor), curr_row-1, 0, &name_above);
        gtk_clist_get_text(GTK_CLIST(processor), curr_row, 0, &name_selected);
        name_above    = strdup(name_above);
        name_selected = strdup(name_selected);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row-1, 0, name_selected);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row, 0, name_above);

        gtk_clist_select_row(GTK_CLIST(processor), curr_row-1, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

void
down_pressed(GtkWidget * widget, gpointer data)
{
    effect_t       *swap;
    gchar          *name_below, *name_selected;

    if (curr_row >= 0 && curr_row < n - 1) {
	swap = effects[curr_row + 1];

	my_lock_mutex(effectlist_lock);
	effects[curr_row + 1] = effects[curr_row];
	effects[curr_row] = swap;
	my_unlock_mutex(effectlist_lock);

	gtk_clist_freeze(GTK_CLIST(processor));
        gtk_clist_get_text(GTK_CLIST(processor), curr_row, 0, &name_selected);
        gtk_clist_get_text(GTK_CLIST(processor), curr_row+1, 0, &name_below);
        name_selected = strdup(name_selected);
        name_below    = strdup(name_below);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row, 0, name_below);
        gtk_clist_set_text(GTK_CLIST(processor), curr_row+1, 0, name_selected);
	
        gtk_clist_select_row(GTK_CLIST(processor), curr_row+1, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

void
del_pressed(GtkWidget *widget, gpointer data)
{
    int             i;

    if (curr_row >= 0 && curr_row < n) {
        my_lock_mutex(effectlist_lock);
	effects[curr_row]->proc_done(effects[curr_row]);
	for (i = curr_row; i < n; i++)
	    effects[i] = effects[i + 1];
	effects[n--] = NULL;
        my_unlock_mutex(effectlist_lock);

	gtk_clist_freeze(GTK_CLIST(processor));
	gtk_clist_remove(GTK_CLIST(processor), curr_row);
	if (curr_row == n - 1)
	    curr_row--;
	gtk_clist_select_row(GTK_CLIST(processor), curr_row, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

void
add_pressed(GtkWidget *widget, gpointer data)
{
    int             idx, i;
    effect_t       *tmp_effect;
    gchar          *name;
    GtkWidget      *known_effects = data;
    
    if (n < MAX_EFFECTS && effects_row >= 0) {
	tmp_effect = effect_list[effects_row].create_f();
	tmp_effect->proc_init(tmp_effect);

        gtk_clist_get_text(GTK_CLIST(known_effects), effects_row, 0, &name);
        name = strdup(name);
        
	my_lock_mutex(effectlist_lock);
	if (curr_row >= 0 && curr_row < n) {
	    idx = curr_row + 1;
	    for (i = n; i > idx; i--) {
		effects[i] = effects[i - 1];
	    }
	    n++;
	} else {
	    idx = n++;
	}
	effects[idx] = tmp_effect;
        my_unlock_mutex(effectlist_lock);

	gtk_clist_insert(GTK_CLIST(processor), idx, &name);
	gtk_clist_select_row(GTK_CLIST(processor), idx, 0);

    }
}

/*
 * callback for gtk_set_pointer_data_full()
 */
void
free_clist_ptr(gpointer data)
{
    if (data != NULL)
	free(data);
}


void
bank_perform_add(GtkWidget * widget, GtkFileSelection * filesel)
{
    char            *fname;
#ifndef _WIN32
    const char	    *name;
#else
    char	    *name;
    int             str_len,
                    i;
    char            drive[_MAX_DRIVE],
                    dir[_MAX_DIR],
                    ext[_MAX_EXT];
#endif

    name = gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    fname = (char *) malloc(strlen(name) * sizeof(char) + 1);
    if (fname != NULL)
	strcpy(fname, name);

#ifdef _WIN32
    /*
     * GTK for Windows have a bug related to non-ascii characters
     * in the strings. We replace all non-ascii chars to ? character.
     */
    _splitpath(fname, drive, dir, name, ext);
    str_len = strlen(name);
    for (i = 0; i < str_len; i++)
	if (!isascii(name[i])) {
	    name[i] = '?';
	}
#else
    name = basename(fname);
#endif
    gtk_clist_append(GTK_CLIST(bank), (gchar **) &name);
    gtk_clist_moveto(GTK_CLIST(bank), GTK_CLIST(bank)->rows - 1, 0, 0.5,
		     1.0);
    gtk_clist_set_row_data_full(GTK_CLIST(bank), GTK_CLIST(bank)->rows - 1,
				fname, free_clist_ptr);
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

void
destroy_widget(GtkWidget * widget, GtkWidget * gallowman)
{
    gtk_widget_destroy(gallowman);
}

void
bank_add_pressed(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;

    filesel = gtk_file_selection_new("Select processor profile");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
		       "clicked", GTK_SIGNAL_FUNC(bank_perform_add),
		       filesel);
    gtk_signal_connect(GTK_OBJECT
		       (GTK_FILE_SELECTION(filesel)->cancel_button),
		       "clicked", GTK_SIGNAL_FUNC(destroy_widget),
		       GTK_WIDGET(filesel));

    gtk_widget_show(filesel);
}

void
bank_switch_pressed(GtkWidget * widget, gpointer data)
{
    char           *fname;

    if (bank_row == GTK_CLIST(bank)->rows - 1)
	bank_row = 0;
    else
	bank_row++;
    fname = gtk_clist_get_row_data(GTK_CLIST(bank), bank_row);
    gtk_clist_select_row(GTK_CLIST(bank), bank_row, 0);
    load_pump(fname);
}

void
bank_perform_save(GtkWidget * widget, GtkFileSelection * filesel)
{
    save_pump(gtk_file_selection_get_filename
	      (GTK_FILE_SELECTION(filesel)));
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

void
bank_start_save(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;

    filesel = gtk_file_selection_new("Select processor profile");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
		       "clicked", GTK_SIGNAL_FUNC(bank_perform_save),
		       filesel);
    gtk_signal_connect(GTK_OBJECT
		       (GTK_FILE_SELECTION(filesel)->cancel_button),
		       "clicked", GTK_SIGNAL_FUNC(destroy_widget),
		       GTK_WIDGET(filesel));

    gtk_widget_show(filesel);
}

void
bank_perform_load(GtkWidget * widget, GtkFileSelection * filesel)
{
    load_pump(gtk_file_selection_get_filename
	      (GTK_FILE_SELECTION(filesel)));
    gtk_widget_destroy(GTK_WIDGET(filesel));
}

void
bank_start_load(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;

    filesel = gtk_file_selection_new("Select processor profile");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
		       "clicked", GTK_SIGNAL_FUNC(bank_perform_load),
		       filesel);
    gtk_signal_connect(GTK_OBJECT
		       (GTK_FILE_SELECTION(filesel)->cancel_button),
		       "clicked", GTK_SIGNAL_FUNC(destroy_widget),
		       GTK_WIDGET(filesel));

    gtk_widget_show(filesel);
}

void
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

void
cancel_tracker(GtkWidget * widget, GtkFileSelection * filesel)
{
    gtk_widget_destroy(GTK_WIDGET(filesel));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tracker), 0);
}

void
tracker_pressed(GtkWidget * widget, gpointer data)
{
    GtkWidget      *filesel;
    time_t          t;
    char            defaultname[80];

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        time(&t);
        strftime(defaultname, 80, "%F-%T."
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

typedef struct SAMPLE_PARAMS {
    GtkWidget      *rate;
    GtkWidget      *channels;
    GtkWidget      *bits;
    GtkWidget      *latency;
    GtkWidget      *dialog;
    GtkWidget      *latency_label;
    GtkWidget      *driver;
} sample_params;

double vumeter_peak = 0;
double vumeter_power = 0;
void set_vumeter_value(double peak, double power) {
    /* accept peaks, decay exponentially otherwise */
    if (peak > vumeter_peak)
        vumeter_peak = peak;
    else
        vumeter_peak = (7 * vumeter_peak + peak) / 8;

    vumeter_power = 7 * (vumeter_power + power) / 8;
}

gboolean
timeout_update_vumeter(gpointer vumeter) {
    GtkRcStyle *rc_style = NULL;
    GdkColor color;
    double power = 0.0;

    rc_style = gtk_rc_style_new();
    if (vumeter_peak >= 1.0) {

        /* indicate distortion due to clipping */
	color.pixel = 0; /* unused */
        color.red   = 65535;
        color.green = 0;
        color.blue  = 0;

        rc_style->bg[GTK_STATE_NORMAL] = color;
        rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BG;
    }
    gtk_widget_modify_style(vumeter, rc_style);
    gtk_rc_style_unref(rc_style);

    if (vumeter_power != 0.0) {
        power = log(vumeter_power) / log(10) * 10;
        /* 16 bits hold ~91 dB resolution */
        if (power > 0)
            power = 0;
        if (power < -91)
            power = -91;
    }

    gtk_progress_set_value(GTK_PROGRESS(vumeter), power);
    return TRUE;
}

void update_master_volume(GtkAdjustment *adj, void *nothing) {
    master_volume = adj->value;
}

void
update_latency_label(GtkWidget *widget, gpointer data)
{
    gchar          *gtmp;
    sample_params  *sp = data;

    update_sampling_params(widget, data);
    gtmp = g_strdup_printf("%.2f ms", 1000.0 * buffer_size / (sample_rate * n_input_channels) / (bits / 8));
    gtk_label_set_text(GTK_LABEL(sp->latency_label), gtmp);
    free(gtmp);
}

void
update_driver(GtkWidget * widget, gpointer data)
{
    const char *tmp=NULL;
    sample_params *sp = (sample_params *) data;
    
    tmp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->driver)->entry));
    if(tmp == NULL)
	return;
#ifdef HAVE_ALSA
    if (strcmp(tmp,"ALSA")==0) {
        audio_driver = &alsa_driver;
        audio_driver_str = "ALSA";
    }
#endif
#ifdef HAVE_OSS
    if (strcmp(tmp,"OSS")==0) {
        audio_driver = &oss_driver;
        audio_driver_str = "OSS";
    }
#endif
#ifdef _WIN32
    if(strcmp(tmp,"MMSystem")==0) {
        audio_driver = &windows_driver;
        audio_driver_str = "MMSystem";
    }
#endif
    /* XXX we should now reopen the dialog because changing audio driver
     * updates bits & channels settings */
    state = STATE_ATHREAD_RESTART;
}

#ifdef _WIN32
void
toggle_directsound(GtkWidget * widget, gpointer threshold)
{
    dsound = !dsound;
    gtk_widget_set_sensitive(GTK_WIDGET(threshold), dsound);
}

void
update_threshold(GtkWidget * widget, gpointer threshold)
{
    overrun_threshold=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(threshold));
}
#endif

/*
 * Sampling parameters dialog
 */
void
sample_dlg(GtkWidget *widget, gpointer data)
{
    static sample_params sparams;
    int             i;
    gchar          *gtmp;
    GtkWidget      *sp_table;
#ifdef _WIN32
    GtkWidget      *directsound;
    GtkWidget      *threshold;
    GtkWidget      *threshold_label;
    GtkWidget      *threshold_fragments;
    GtkObject      *threshold_adj;
#endif
    GtkWidget      *rate_label;
    GtkWidget      *bits_label;
    GtkWidget      *channels_label;
    GtkWidget      *latency_label;
    GtkWidget	   *driver_label;
    GtkWidget      *ok;
    GtkWidget      *cancel;
    GtkWidget      *group;
    GtkWidget      *vpack,
                   *buttons_pack;
    GList          *sample_rates = NULL;
    GList          *bits_list = NULL;
    GList          *channels_list = NULL;
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

    sp_table = gtk_table_new(4, 4, FALSE);
    gtk_container_add(GTK_CONTAINER(group), sp_table);
    
#define TBLOPT  __GTKATTACHOPTIONS(GTK_FILL|GTK_EXPAND|GTK_SHRINK)
    
    rate_label = gtk_label_new("Sampling rate:");
    gtk_misc_set_alignment(GTK_MISC(rate_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), rate_label, 0, 1, 0, 1,
                     TBLOPT, TBLOPT, 3, 3);

    sparams.rate = gtk_combo_new();
    /* these may also be driver dependant but let's leave them as is for now */
    sample_rates = g_list_append(sample_rates, "48000");
    sample_rates = g_list_append(sample_rates, "44100");
    sample_rates = g_list_append(sample_rates, "22050");
    sample_rates = g_list_append(sample_rates, "16000");
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.rate), sample_rates);
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

    if (audio_driver) { 
        for (i = 0; audio_driver->channels[i].in != 0; i += 1) {
            gtmp = g_strdup_printf("%d in - %d out", audio_driver->channels[i].in, audio_driver->channels[i].out);
            channels_list = g_list_append(channels_list, gtmp);
        }
    }
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.channels),
				  channels_list);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.channels)->entry),
			   FALSE);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.channels)->entry),
		       g_strdup_printf("%d in - %d out", n_input_channels, n_output_channels));
    gtk_table_attach(GTK_TABLE(sp_table), sparams.channels, 3, 4, 0, 1,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.channels)->entry,"Mono/Stereo/Quadrophonic",NULL);
    bits_label = gtk_label_new("Bits:");
    gtk_misc_set_alignment(GTK_MISC(bits_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), bits_label, 0, 1, 1, 2,
                     TBLOPT, TBLOPT, 3, 3);
    sparams.bits = gtk_combo_new();

    if (audio_driver) { 
        for (i = 0; audio_driver->bits[i] != 0; i += 1) {
            gtmp = g_strdup_printf("%d", audio_driver->bits[i]);
            bits_list = g_list_append(bits_list, gtmp);
        }
    }
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.bits), bits_list);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.bits)->entry),
		       g_strdup_printf("%d", bits));
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.bits)->entry),
			   FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.bits, 1, 2, 1, 2,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.bits)->entry, "ALSA can do 32-bit input with some cards", NULL);

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
    gtk_table_attach(GTK_TABLE(sp_table), driver_label, 0, 1, 2, 3,
                     TBLOPT, TBLOPT, 3, 3);
    sparams.driver = gtk_combo_new();
#ifdef HAVE_OSS
    drivers_list = g_list_append(drivers_list, "OSS");
#endif
#ifdef HAVE_ALSA
    drivers_list = g_list_append(drivers_list, "ALSA");
#endif
#ifdef _WIN32
    drivers_list = g_list_append(drivers_list, "MMSystem");
    drivers_list = g_list_append(drivers_list, "DirectX");
#endif

    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.driver), drivers_list);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.driver)->entry), audio_driver_str);
    
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.driver)->entry),
			   FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), sparams.driver, 1, 2, 2, 3,
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
    /* DirectSound checkbox */
    directsound = gtk_check_button_new_with_label("Output via DirectSound");
    gtk_table_attach(GTK_TABLE(sp_table), directsound, 0, 1, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    if (dsound)
	GTK_TOGGLE_BUTTON(directsound)->active = 1;
    gtk_tooltips_set_tip(tooltips,directsound,"If this is turned on, " \
	"the playback will be output via DirectSound, " \
	"or via MME API otherwise. Resulting latency depends mostly on this." \
	"However, if you have WDM sound driver, try to turn this off.",NULL);

    /* threshold spin button */
    threshold_label = gtk_label_new("Overrun threshold:");
    gtk_misc_set_alignment(GTK_MISC(threshold_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), threshold_label, 1, 2, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    threshold_adj =
	gtk_adjustment_new(overrun_threshold, 0, 200,
			   1, 1, 0);
    threshold =
	gtk_spin_button_new(GTK_ADJUSTMENT(threshold_adj), 1, 0);
    dummy1 = GTK_SPIN_BUTTON(threshold);
    dummy2 = &(dummy1->entry);
    gtk_entry_set_editable(dummy2, FALSE);
    gtk_table_attach(GTK_TABLE(sp_table), threshold, 2, 3, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_tooltips_set_tip(tooltips,threshold,"Large value will force buffer overruns " \
	"to be ignored. If you encounter heavy overruns, " \
	"especially with autowah, decrease this to 1. " \
	"(for hackers: this is the number of fragments that are allowed to be lost).",NULL);
    threshold_fragments = gtk_label_new("fragments");
    gtk_misc_set_alignment(GTK_MISC(threshold_fragments), 0, 0.5);
    gtk_table_attach(GTK_TABLE(sp_table), threshold_fragments, 3, 4, 3, 4,
                     TBLOPT, TBLOPT, 3, 3);
    gtk_signal_connect(GTK_OBJECT(directsound), "toggled",
		       GTK_SIGNAL_FUNC(toggle_directsound), threshold);
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
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sparams.bits)->entry),
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

void
update_sampling_params(GtkWidget * dialog, gpointer data)
{
    int             tmp1, tmp2;
    sample_params  *sp = data;

    buffer_size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sp->latency));
    bits = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->bits)->entry)));
    sscanf(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->channels)->entry)),
           "%d in - %d out", &tmp1, &tmp2);
    n_input_channels = tmp1;
    n_output_channels = tmp2;
    sample_rate = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->rate)->entry)));
}

void update_sampling_params_and_close_dialog(GtkWidget *dialog, gpointer data)
{
    sample_params  *sp = data;
    
    update_sampling_params(dialog, data);
    gtk_widget_destroy(GTK_WIDGET(sp->dialog));
}

void
start_stop(GtkWidget *widget, gpointer data)
{
    int             error;
    if (! audio_driver) {
        /* don't allow user to press us into active state */
        if (GTK_TOGGLE_BUTTON(widget)->active)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), 0);
        return;
    }
    if (GTK_TOGGLE_BUTTON(widget)->active) {
#ifdef _WIN32
	ResumeThread(audio_thread);
	if(state != STATE_ATHREAD_RESTART)
	    state = STATE_PROCESS;
#endif
	if(state == STATE_ATHREAD_RESTART) {
            my_unlock_mutex(snd_open);
#ifndef _WIN32
            if (audio_thread)
                pthread_join(audio_thread, NULL);
	    state = STATE_PAUSE;
	    if (pthread_create(&audio_thread, NULL, audio_driver->thread, NULL)) {
		fprintf(stderr, "Audio thread restart failed!\n");
		state = STATE_EXIT;
	    }
#else
            WaitForSingleObject(audio_thread,INFINITE);
            state = STATE_PAUSE;
	    audio_thread =
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE) audio_driver->thread, 0,
		     0, &thread_id);

            /*
	     * set realtime priority to the thread
    	     */
	    if (!SetThreadPriority(audio_thread, THREAD_PRIORITY_TIME_CRITICAL))
		fprintf(stderr,
		    "\nFailed to set realtime priority to thread: %s. Continuing with default priority.",
			GetLastError());
#endif
	}

	if ((error = audio_driver->init()) != ERR_NOERROR) {
            fprintf(stderr, "warning: unable to begin audio processing (code %d)\n", error);
            gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child), "ERROR");
            return;
        }

	gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")),
				 FALSE);
	gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child), "STOP");
    } else {
#ifdef _WIN32
        state = STATE_PAUSE;
// FIXME: need to make sure that we left the pump_sample().
	SuspendThread(audio_thread);
#endif
	audio_driver->finish();
	gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")),
				 TRUE);
	gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child), "START");
    }
}

void
init_gui(void)
{
    GtkAccelGroup  *accel_group;
    GtkWidget      *vumeter;
    GtkObject      *adj_master;
    GtkWidget	   *master;
    int             i;
    gint            nmenu_items =
	sizeof(mainGui_menu) / sizeof(mainGui_menu[0]);
    char           *processor_titles[] = { "Current effects", NULL };
    char           *effects_titles[] = { "Known effects", NULL };
    char           *bank_titles[] = { "Processor bank", NULL };
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

    mainWnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(mainWnd, 700, 400);
    tbl = gtk_table_new(5, 6, FALSE);
    gtk_signal_connect(GTK_OBJECT(mainWnd), "destroy",
		       GTK_SIGNAL_FUNC(quit), NULL);


    /*
     * make menu
     */
    accel_group = gtk_accel_group_new();

    item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
					accel_group);
    gtk_item_factory_create_items(item_factory, nmenu_items, mainGui_menu,
				  NULL);
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

    for (i = 0; effect_list[i].str; i++)
	gtk_clist_append(GTK_CLIST(known_effects), &effect_list[i].str);

    bank = gtk_clist_new_with_titles(1, bank_titles);
    gtk_clist_set_selection_mode(GTK_CLIST(bank), GTK_SELECTION_SINGLE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(bank), 0, TRUE);
    bank_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(bank_scroll), bank);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(bank_scroll),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    gtk_tooltips_set_tip(tooltips,bank,"This area contains a list of presets." \
	"Use \"Add Preset\" button to add the preset to the list." \
	"Use SWITCH button to switch between presets.",NULL);

    gtk_table_attach(GTK_TABLE(tbl), processor_scroll, 3, 4, 1, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), effect_scroll, 5, 6, 1, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), bank_scroll, 1, 2, 1, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND), 0, 0);


    gtk_container_add(GTK_CONTAINER(mainWnd), tbl);
    gtk_window_set_title(GTK_WINDOW(mainWnd), "GNUitar");

    bank_add = gtk_button_new_with_label("Add Preset >>");
    gtk_tooltips_set_tip(tooltips,bank_add,"Use this button to add the presets to the presets list.",NULL);

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
    
    bank_switch = gtk_button_new_with_label("SWITCH");
    gtk_tooltips_set_tip(tooltips,bank_switch,"Use this button to switch between presets",NULL);
    up = gtk_button_new_with_label("Up");
    gtk_tooltips_set_tip(tooltips,up,"Use this button to move the current selected effect up.",NULL);
    down = gtk_button_new_with_label("Down");
    gtk_tooltips_set_tip(tooltips,down,"Use this button to move the current selected effect down.",NULL);
    del = gtk_button_new_with_label("Delete");
    gtk_tooltips_set_tip(tooltips,del,"Use this button to delete the current selected effect",NULL);
    add = gtk_button_new_with_label("<< Add");
    gtk_tooltips_set_tip(tooltips,add,"Use this button to add the current selected effect to the \"Current Effects\" list.",NULL);
    tracker = gtk_check_button_new_with_label("Write track");
    gtk_tooltips_set_tip(tooltips,tracker,"You can write the output to the file, did you know ?.",NULL);
    if (audio_driver) {
        start = gtk_toggle_button_new_with_label("STOP");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start), 1);
    } else {
        start = gtk_toggle_button_new_with_label("START\n(disabled:\nno audio\ndriver)");
    }
    gtk_tooltips_set_tip(tooltips,start,"This button starts/stops the sound processing.",NULL);
    vumeter = gtk_progress_bar_new();
    gtk_progress_set_format_string(GTK_PROGRESS(vumeter), "%v dB");
    gtk_progress_configure(GTK_PROGRESS(vumeter), -91, -91, 0);
    gtk_progress_set_show_text(GTK_PROGRESS(vumeter), TRUE);
    adj_master = gtk_adjustment_new(master_volume, -30.0, 30.0, 1.0, 5.0, 0.0);
    master = gtk_hscale_new(GTK_ADJUSTMENT(adj_master));
    gtk_scale_set_draw_value(GTK_SCALE(master), FALSE);

    gtk_table_attach(GTK_TABLE(tbl), bank_add, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), bank_switch, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL),
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_FILL), 5, 5);
    gtk_table_attach(GTK_TABLE(tbl), start, 0, 1, 2, 3,
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
    gtk_table_attach(GTK_TABLE(tbl), tracker, 0, 1, 5, 6,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), vumeter, 1, 3, 5, 6, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);
    gtk_table_attach(GTK_TABLE(tbl), master, 4, 6, 5, 6, __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK), __GTKATTACHOPTIONS(0), 2, 2);

    gtk_signal_connect(GTK_OBJECT(bank_add), "clicked",
		       GTK_SIGNAL_FUNC(bank_add_pressed), NULL);
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
    gtk_signal_connect(GTK_OBJECT(processor), "row_move",
		       GTK_SIGNAL_FUNC(rowmove_processor), NULL);
    gtk_signal_connect(GTK_OBJECT(known_effects), "select_row",
		       GTK_SIGNAL_FUNC(selectrow_effects), NULL);
    gtk_signal_connect(GTK_OBJECT(adj_master), "value_changed",
		       GTK_SIGNAL_FUNC(update_master_volume), NULL);
    gtk_widget_show_all(mainWnd);

    g_timeout_add(VU_UPDATE_INTERVAL, timeout_update_vumeter, vumeter);

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
					    &style->white, gnuitar_xpm);
    gdk_window_set_icon(mainWnd->window, mainWnd->window, app_icon, mask);
#endif
}

