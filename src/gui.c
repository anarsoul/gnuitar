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
#include <gtk/gtk.h>
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
#include "tracker.h"
#include "utils.h"

#define VU_UPDATE_INTERVAL 20.0    /* ms */
 
void            bank_start_save(GtkWidget * widget, gpointer data);
void            bank_start_load(GtkWidget * widget, gpointer data);
void            sample_dlg(GtkWidget * widget, gpointer data);
void            update_sampling_params(GtkWidget * widget,
				       gpointer sparams);
void            quit(GtkWidget * widget, gpointer data);
void            about_dlg(void);
void            help_contents(void);
/*
 * These externs are from main.c 
 */
extern int      init_sound();
extern void     close_sound();
#ifndef _WIN32
extern pthread_mutex_t mutex;
extern pthread_cond_t suspend;
#else
extern HANDLE   audio_thread;
extern short    dsound;
extern unsigned short overrun_threshold;
#endif

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

extern unsigned short write_track;

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

#ifdef HAVE_GTK
    about = gtk_window_new(GTK_WINDOW_DIALOG);
#elif defined HAVE_GTK2
    about = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif

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

    gtk_text_freeze(MYGTK_TEXT(text));

    gtk_text_insert(MYGTK_TEXT(text), NULL, NULL, NULL, DISCLAIMER, -1);

    gtk_text_insert(MYGTK_TEXT(text), NULL, NULL, NULL,
		    "This program is distributed in the hope that it will be useful,\n"
		    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
		    "See the GNU General Public License for more details.\n\n",
		    -1);

    gtk_text_insert(MYGTK_TEXT(text), NULL, NULL, NULL,
		    "You should have received a copy of the GNU General Public License\n"
		    "along with this program; if not, write to the Free Software\n"
		    "Foundation, Inc., 59 Temple Place - Suite 330, Boston,\n"
		    "MA 02111-1307, USA.", -1);

    gtk_text_thaw(MYGTK_TEXT(text));

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

gint
delete_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    return (TRUE);
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

void
up_pressed(GtkWidget * widget, gpointer data)
{
    struct effect  *swap;

    if (curr_row > 0 && curr_row < n) {
	swap = effects[curr_row - 1];

	audio_lock = 1;

	effects[curr_row - 1] = effects[curr_row];
	effects[curr_row] = swap;

	audio_lock = 0;

	gtk_clist_freeze(GTK_CLIST(processor));
	gtk_clist_remove(GTK_CLIST(processor), curr_row - 1);
	gtk_clist_remove(GTK_CLIST(processor), curr_row - 1);
	gtk_clist_insert(GTK_CLIST(processor), curr_row - 1,
			 &effect_list[effects[curr_row]->id].str);
	gtk_clist_insert(GTK_CLIST(processor), curr_row - 1,
			 &effect_list[effects[curr_row - 1]->id].str);
	gtk_clist_select_row(GTK_CLIST(processor), curr_row - 1, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

void
down_pressed(GtkWidget * widget, gpointer data)
{
    struct effect  *swap;

    if (curr_row >= 0 && curr_row < n - 1) {
	swap = effects[curr_row + 1];

	audio_lock = 1;

	effects[curr_row + 1] = effects[curr_row];
	effects[curr_row] = swap;

	audio_lock = 0;

	gtk_clist_freeze(GTK_CLIST(processor));
	gtk_clist_remove(GTK_CLIST(processor), curr_row);
	gtk_clist_remove(GTK_CLIST(processor), curr_row);
	gtk_clist_insert(GTK_CLIST(processor), curr_row,
			 &effect_list[effects[curr_row + 1]->id].str);
	gtk_clist_insert(GTK_CLIST(processor), curr_row,
			 &effect_list[effects[curr_row]->id].str);
	gtk_clist_select_row(GTK_CLIST(processor), curr_row + 1, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

void
del_pressed(GtkWidget * widget, gpointer data)
{
    int             i;

    if (curr_row >= 0 && curr_row < n) {
	audio_lock = 1;

	effects[curr_row]->proc_done(effects[curr_row]);
	for (i = curr_row; i < n; i++)
	    effects[i] = effects[i + 1];
	effects[n--] = NULL;

	audio_lock = 0;

	gtk_clist_freeze(GTK_CLIST(processor));
	gtk_clist_remove(GTK_CLIST(processor), curr_row);
	if (curr_row == n - 1)
	    curr_row--;
	gtk_clist_select_row(GTK_CLIST(processor), curr_row, 0);
	gtk_clist_thaw(GTK_CLIST(processor));
    }
}

void
add_pressed(GtkWidget * widget, gpointer data)
{
    int             idx;
    int             i;
    struct effect  *tmp_effect;

    if (n < MAX_EFFECTS && effects_row >= 0
	&& effects_row <= EFFECT_AMOUNT) {
	tmp_effect = (struct effect *) malloc(sizeof(struct effect));
	effect_list[effects_row].create_f(tmp_effect);
	tmp_effect->proc_init(tmp_effect);

	audio_lock = 1;
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
	audio_lock = 0;

	gtk_clist_insert(GTK_CLIST(processor), idx,
			 &effect_list[effects[idx]->id].str);
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
    const char	    *name;
#ifdef _WIN32
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
    gtk_clist_append(GTK_CLIST(bank), &name);
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
    if (name != NULL)
	tracker_out(name);
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

    if (!write_track) {
	write_track = 1;
	filesel = gtk_file_selection_new("Enter track name");
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel),
#ifndef _WIN32
					"*.raw");
#else
					"*.wav");
#endif
	gtk_signal_connect(GTK_OBJECT
			   (GTK_FILE_SELECTION(filesel)->ok_button),
			   "clicked", GTK_SIGNAL_FUNC(start_tracker),
			   filesel);
	gtk_signal_connect(GTK_OBJECT
			   (GTK_FILE_SELECTION(filesel)->cancel_button),
			   "clicked", GTK_SIGNAL_FUNC(cancel_tracker),
			   GTK_WIDGET(filesel));
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
    if (vumeter_peak > 1.0) {

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
        /* every doubling in energy is ~ 3 dB more signal */
        power = log(vumeter_power) / log(10) * 10;
        /* 16 bits hold 91 dB resolution */
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
update_latency_label(GtkWidget * widget, gpointer sparams)
{
    char            tmp[256];
    int             bufsize,
                    n,
                    sr;
#ifndef _WIN32
    static int      old_bufsize = 0;
#endif

    sample_params  *sp = (sample_params *) sparams;
    bufsize =
	gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sp->latency));
#ifndef _WIN32
    /*
     * OSS cannot accept buffer size that is not a level of 2 
     */
    while (my_log2(bufsize) == 0) {
	if (bufsize > old_bufsize) {
	    bufsize += MIN_BUFFER_SIZE;
	} else {
	    bufsize -= MIN_BUFFER_SIZE;
	}
    }
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(sp->latency), bufsize);
    old_bufsize = bufsize;
#endif
    n = atoi(gtk_entry_get_text
	     (GTK_ENTRY(GTK_COMBO(sp->channels)->entry)));
    sr = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->rate)->entry)));
    sprintf(tmp, "latency = %.2fms", bufsize * 1000.0 / (sr * n));
    gtk_label_set_text(GTK_LABEL(sp->latency_label), tmp);
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
sample_dlg(GtkWidget * widget, gpointer data)
{
    static sample_params sparams;
    GtkWidget      *vpack0;
    GtkWidget      *hpack1;
    GtkWidget      *hpack2;
    GtkWidget      *hpack3;
#ifdef _WIN32
    GtkWidget      *hpack4;
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
    GtkWidget      *ok;
    GtkWidget      *cancel;
    GtkWidget      *group;
    GtkWidget      *vpack,
                   *buttons_pack;
    GList          *sample_rates = NULL;
    GList          *bits_list = NULL;
    GList          *channels_list = NULL;
    GtkObject      *latency_adj;
    char            tmp[256];
    GtkSpinButton  *dummy1;
    GtkEntry       *dummy2;

#ifdef HAVE_GTK
    sparams.dialog = gtk_window_new(GTK_WINDOW_DIALOG);
#elif defined HAVE_GTK2
    sparams.dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif

    gtk_widget_set_usize(sparams.dialog, 320, 200);
    gtk_container_set_border_width(GTK_CONTAINER(sparams.dialog), 5);
    vpack = gtk_vbox_new(FALSE, 20);

    group = gtk_frame_new("Sampling Parameters");
    gtk_container_add(GTK_CONTAINER(sparams.dialog), vpack);
    gtk_box_pack_start(GTK_BOX(vpack), group, TRUE, TRUE, 0);

    vpack0 = gtk_vbox_new(TRUE, 3);
    gtk_container_add(GTK_CONTAINER(group), vpack0);
    gtk_container_set_border_width(GTK_CONTAINER(vpack0), 3);
    gtk_window_set_title(GTK_WINDOW(sparams.dialog),
			 "Sampling Parameters");
    hpack1 = gtk_hbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vpack0), hpack1, TRUE, TRUE, 1);
    hpack2 = gtk_hbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vpack0), hpack2, TRUE, TRUE, 1);
#ifdef _WIN32
    hpack4 = gtk_hbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vpack0), hpack4, TRUE, TRUE, 1);
#endif
    hpack3 = gtk_hbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vpack0), hpack3, TRUE, TRUE, 1);

    rate_label = gtk_label_new("Sampling Rate:");
    gtk_box_pack_start(GTK_BOX(hpack1), rate_label, TRUE, FALSE, 1);

    sparams.rate = gtk_combo_new();
    sample_rates = g_list_append(sample_rates, "48000");
    sample_rates = g_list_append(sample_rates, "44100");
    sample_rates = g_list_append(sample_rates, "22050");
    sample_rates = g_list_append(sample_rates, "16000");
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.rate), sample_rates);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.rate)->entry),
		       my_itoa(sample_rate));
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.rate)->entry),
			   FALSE);
    gtk_widget_set_usize(sparams.rate, 70, 0);
    gtk_box_pack_start(GTK_BOX(hpack1), sparams.rate, TRUE, FALSE, 0);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.rate)->entry,"This is the current sampling rate.",NULL);

    channels_label = gtk_label_new("  Channels:");
    gtk_box_pack_start(GTK_BOX(hpack1), channels_label, TRUE, FALSE, 1);
    sparams.channels = gtk_combo_new();
    channels_list = g_list_append(channels_list, "2");
    channels_list = g_list_append(channels_list, "1");
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.channels),
				  channels_list);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.channels)->entry),
			   FALSE);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.channels)->entry),
		       my_itoa(nchannels));
    gtk_widget_set_usize(sparams.channels, 40, 0);
    gtk_box_pack_start(GTK_BOX(hpack1), sparams.channels, TRUE, FALSE, 1);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.channels)->entry,"Mono ? Stereo ?",NULL);

    bits_label = gtk_label_new("Bits:");
    gtk_box_pack_start(GTK_BOX(hpack2), bits_label, TRUE, FALSE, 1);
    sparams.bits = gtk_combo_new();
    bits_list = g_list_append(bits_list, "16");
    bits_list = g_list_append(bits_list, "8");
    gtk_combo_set_popdown_strings(GTK_COMBO(sparams.bits), bits_list);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sparams.bits)->entry),
		       my_itoa(bits));
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(sparams.bits)->entry),
			   FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(sparams.bits), FALSE);
    gtk_widget_set_usize(sparams.bits, 40, 0);
    gtk_box_pack_start(GTK_BOX(hpack2), sparams.bits, TRUE, FALSE, 1);
    gtk_tooltips_set_tip(tooltips,GTK_COMBO(sparams.bits)->entry,"I thought that no one will ever need 8 bits, and this option is disabled.",NULL);

    latency_label = gtk_label_new("Fragment size:");
    gtk_box_pack_start(GTK_BOX(hpack2), latency_label, TRUE, FALSE, 1);
    latency_adj =
	gtk_adjustment_new(buffer_size, MIN_BUFFER_SIZE, MAX_BUFFER_SIZE,
			   MIN_BUFFER_SIZE, MIN_BUFFER_SIZE, 0);
    sparams.latency =
	gtk_spin_button_new(GTK_ADJUSTMENT(latency_adj), 1, 0);
    dummy1 = GTK_SPIN_BUTTON(sparams.latency);
    dummy2 = &(dummy1->entry);
    gtk_entry_set_editable(dummy2, FALSE);
    gtk_widget_set_usize(sparams.latency, 70, 0);
    gtk_box_pack_start(GTK_BOX(hpack2), sparams.latency, FALSE, FALSE, 1);
    sprintf(tmp, "latency = %.2fms",
	    buffer_size * 1000.0 / (sample_rate * nchannels));
    sparams.latency_label = gtk_label_new(tmp);
    gtk_label_set_justify(GTK_LABEL(sparams.latency_label),
			  GTK_JUSTIFY_LEFT);
    gtk_box_pack_end(GTK_BOX(hpack3), sparams.latency_label, TRUE, TRUE,
		     1);
    gtk_tooltips_set_tip(tooltips,sparams.latency,"The fragment size is the number of samples " \
	"that the sound driver reads by one time. " \
	"The smaller is the fragment size, the lower is the latency, " \
	"and vice versa. However, small fragment size may cause buffer overruns. " \
	"If you encounter multiple buffer overruns, " \
	"try to increase this setting.",NULL);
#ifdef _WIN32
    /* DirectSound checkbox */
    directsound =
	gtk_check_button_new_with_label("Output via DirectSound");
    gtk_box_pack_start(GTK_BOX(hpack4), directsound, TRUE, TRUE, 1);
    if (dsound)
	GTK_TOGGLE_BUTTON(directsound)->active = 1;
    gtk_tooltips_set_tip(tooltips,directsound,"If this is turned on, " \
	"the playback will be output via DirectSound, " \
	"or via MME API otherwise. Resulting latency depends mostly on this." \
	"However, if you have WDM sound driver, try to turn this off.",NULL);

    /* threshold spin button */
    threshold_label = gtk_label_new("Overrun threshold:");
    gtk_box_pack_start(GTK_BOX(hpack4), threshold_label, TRUE, FALSE, 1);
    threshold_adj =
	gtk_adjustment_new(overrun_threshold, 0, 200,
			   1, 1, 0);
    threshold =
	gtk_spin_button_new(GTK_ADJUSTMENT(threshold_adj), 1, 0);
    dummy1 = GTK_SPIN_BUTTON(threshold);
    dummy2 = &(dummy1->entry);
    gtk_entry_set_editable(dummy2, FALSE);
    gtk_widget_set_usize(threshold, 30, 0);
    gtk_box_pack_start(GTK_BOX(hpack4), threshold, FALSE, FALSE, 1);
    gtk_tooltips_set_tip(tooltips,threshold,"Large value will force buffer overruns " \
	"to be ignored. If you encounter heavy overruns, " \
	"especially with autowah, decrease this to 1. " \
	"(for hackers: this is the number of fragments that are allowed to be lost).",NULL);
    threshold_fragments = gtk_label_new("fragments");
    gtk_box_pack_start(GTK_BOX(hpack4), threshold_fragments, TRUE, FALSE, 1);
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
		       GTK_SIGNAL_FUNC(update_sampling_params), &sparams);
    gtk_signal_connect(GTK_OBJECT(latency_adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_latency_label), &sparams);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sparams.channels)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_latency_label),
		       &sparams);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sparams.rate)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_latency_label),
		       &sparams);

    cancel = gtk_button_new_with_label("Cancel");
    gtk_box_pack_start(GTK_BOX(buttons_pack), cancel, TRUE, FALSE, 0);
    gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(sparams.dialog));
    gtk_box_pack_start(GTK_BOX(vpack), buttons_pack, TRUE, TRUE, 0);
    gtk_widget_grab_focus(ok);

    gtk_widget_show_all(sparams.dialog);
}

void
update_sampling_params(GtkWidget * dialog, gpointer sparams)
{
    sample_params  *sp = (sample_params *) sparams;

    buffer_size =
	gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sp->latency));
    bits = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->bits)->entry)));
    nchannels =
	atoi(gtk_entry_get_text
	     (GTK_ENTRY(GTK_COMBO(sp->channels)->entry)));
    sample_rate =
	atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(sp->rate)->entry)));
    gtk_widget_destroy(GTK_WIDGET(sp->dialog));
}

void
start_stop(GtkWidget * widget, gpointer data)
{
    int             error;
    if (GTK_TOGGLE_BUTTON(widget)->active) {
#ifndef _WIN32
	pthread_mutex_lock(&mutex);
#endif
	if ((error = init_sound()) != ERR_NOERROR) {
#ifndef _WIN32
	    pthread_cond_signal(&suspend);
	    pthread_mutex_unlock(&mutex);
#endif
	    exit(error);
	}
	audio_lock = 0;
#ifndef _WIN32
	pthread_cond_signal(&suspend);
	pthread_mutex_unlock(&mutex);
#else
	ResumeThread(audio_thread);
#endif
	gtk_widget_set_sensitive(GTK_WIDGET
				 (gtk_item_factory_get_widget
				  (item_factory, "/Options/Options")),
				 FALSE);
	gtk_label_set_text(GTK_LABEL(GTK_BIN(widget)->child), "STOP");
    } else {
	audio_lock = 1;
	close_sound();
#ifdef _WIN32
	SuspendThread(audio_thread);
#endif
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
    GdkFont        *new_font;

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
    gtk_window_set_title(GTK_WINDOW(mainWnd), MAINGUI_TITLE);

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
    start = gtk_toggle_button_new_with_label("STOP");
    gtk_tooltips_set_tip(tooltips,start,"This button starts/stops the sound processing.",NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start), 1);
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
		       GTK_SIGNAL_FUNC(add_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(tracker), "clicked",
		       GTK_SIGNAL_FUNC(tracker_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(processor), "select_row",
		       GTK_SIGNAL_FUNC(selectrow_processor), NULL);
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
