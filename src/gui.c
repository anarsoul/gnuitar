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
#include <time.h>

#ifdef _WIN32
#    include <ctype.h>
#    include <string.h>
#    include <windows.h>
#    include <process.h>
#    include "resource.h"
#else
#    include <sys/stat.h>
#    include "gnuitar.xpm"
#endif

#include "gui.h"
#include "pump.h"
#include "tracker.h"

void            bank_start_save(GtkWidget * widget, gpointer data);
void            bank_start_load(GtkWidget * widget, gpointer data);
void            quit(GtkWidget * widget, gpointer data);
void            about_dlg(void);
void            help_contents(void);

static GtkItemFactoryEntry mainGui_menu[] = {
    {"/_File", "<alt>F", NULL, 0, "<Branch>"},

    {"/File/_Open Layout", "<control>O",
     (GtkSignalFunc) bank_start_load, 0, NULL},
    {"/File/_Save Layout", "<control>S", (GtkSignalFunc) bank_start_save,
     0, NULL},
    {"/File/sep1", NULL, NULL, 0, "<Separator>"},
    {"/File/E_xit", NULL, (GtkSignalFunc) quit, 0, NULL},
    {"/_Help", NULL, NULL, 0, "<LastBranch>"},
    {"/_Help/Contents", NULL, (GtkSignalFunc) help_contents, 0, NULL},
    {"/_Help/About", NULL, (GtkSignalFunc) about_dlg, 0, NULL}
};
GtkWidget      *mainWnd;
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

    about = gtk_window_new(GTK_WINDOW_DIALOG);
    about_label =
	gtk_label_new
	(COPYRIGHT);
    gtk_window_set_title(GTK_WINDOW(about), "About");
    gtk_container_set_border_width(GTK_CONTAINER(about), 8);
    gtk_widget_set_usize(about, 528, 358);
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

    gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,DISCLAIMER, -1);

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
    gtk_widget_grab_default(ok_button);
    gtk_signal_connect_object(GTK_OBJECT(ok_button), "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(about));

    gtk_widget_show_all(about);
}

void help_contents(void) {
#ifndef _WIN32
    char path[2048];
    pid_t pid;
    char browser[2048]="";
    char *env_browser=NULL;
    char *browsers[7]={
	"/usr/bin/netscape",
	"/usr/bin/netscape-navigator",
	"/usr/bin/netscape-communicator",
	"/usr/bin/konqueror",
	"/usr/bin/mozilla",
	"/usr/bin/links",
	"/usr/bin/lynx"
    };
    int i;
    struct stat bullshit;

    /* first get environment variable for a browser */
    env_browser=getenv("BROWSER");
    /* if there is no prefernce, trying to guess */
    if(env_browser==NULL) {
	for(i=0;i<7;i++) {
	    if(stat(browsers[i],&bullshit)==0) {
		strcpy(browser,browsers[i]);
		break;
	    }
	}
    }
    else {
	strcpy(browser,env_browser);
    }

    if(strcmp(browser,"")!=0) {
        pid=fork();
	if(pid==-1) {
	    perror("fork");
	    return;
	}
        /* child process */
	if(pid==0) {
	    getcwd(path,sizeof(path));
	    strcat(path,"/../docs/index.html");
	    execl(browser,browser,path,NULL);
	}
    }
#else
    if(spawnlp(P_NOWAIT,"start","start","..\\docs\\index.html")==-1) {
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
    char           *fname,
                   *name;
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
    tracker_out(gtk_file_selection_get_filename
		(GTK_FILE_SELECTION(filesel)));
    gtk_widget_destroy(GTK_WIDGET(filesel));
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
			   "clicked", GTK_SIGNAL_FUNC(destroy_widget),
			   GTK_WIDGET(filesel));
	gtk_widget_show(filesel);
    } else {
	write_track = 0;
	tracker_done();
    }
}

void
init_gui(void)
{
    GtkItemFactory *item_factory;
    GtkAccelGroup  *accel_group;
    int             i;
    gint            nmenu_items =
	sizeof(mainGui_menu) / sizeof(mainGui_menu[0]);
    char           *processor_titles[] = { "Current effects", NULL };
    char           *effects_titles[] = { "Known effects", NULL };
    char           *bank_titles[] = { "Processor bank", NULL };

#ifdef _WIN32
    HICON           app_icon,
                    small_icon;
    HWND            window;
    HMODULE         me;
#else
    GdkPixmap      *app_icon;
    GdkBitmap      *mask;
    GtkStyle       *style;
#endif

    mainWnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(mainWnd, 500, 370);
    tbl = gtk_table_new(5, 6, FALSE);
    gtk_signal_connect(GTK_OBJECT(mainWnd), "destroy",
		       GTK_SIGNAL_FUNC(quit), NULL);


    /*
     * make menu
     */
    accel_group = gtk_accel_group_new();

    /*
     * This function initializes the item factory. Param 1: The type of menu
     * - can be GTK_TYPE_MENU_BAR, GTK_TYPE_MENU, or GTK_TYPE_OPTION_MENU.
     * Param 2: The path of the menu. Param 3: A pointer to a
     * gtk_accel_group.  The item factory sets up the accelerator table while 
     * generating menus. 
     */

    item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
					accel_group);

    /*
     * This function generates the menu items. Pass the item factory, the
     * number of items in the array, the array itself, and any callback data
     * for the the menu items. 
     */
    gtk_item_factory_create_items(item_factory, nmenu_items, mainGui_menu,
				  NULL);

    /*
     * Attach the new accelerator group to the window. 
     */
    gtk_window_add_accel_group(GTK_WINDOW(mainWnd), accel_group);

    /*
     * Finally, return the actual menu bar created by the item factory. 
     */
    menuBar = gtk_item_factory_get_widget(item_factory, "<main>");

    gtk_table_attach(GTK_TABLE(tbl), menuBar, 0, 6, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(0), 0, 0);


    processor = gtk_clist_new_with_titles(1, processor_titles);
    gtk_clist_set_selection_mode(GTK_CLIST(processor),
				 GTK_SELECTION_SINGLE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(processor), 0, TRUE);
    processor_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(processor_scroll), processor);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(processor_scroll),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);

    known_effects = gtk_clist_new_with_titles(1, effects_titles);
    gtk_clist_set_selection_mode(GTK_CLIST(known_effects),
				 GTK_SELECTION_SINGLE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(known_effects), 0, TRUE);
    effect_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(effect_scroll), known_effects);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(effect_scroll),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);

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

    bank_add = gtk_button_new_with_label("Add >>");
    bank_switch = gtk_button_new_with_label("SWITCH");
    up = gtk_button_new_with_label("Up");
    down = gtk_button_new_with_label("Down");
    del = gtk_button_new_with_label("Delete");
    add = gtk_button_new_with_label("<< Add");
    tracker = gtk_check_button_new_with_label("Write track");

    gtk_table_attach(GTK_TABLE(tbl), bank_add, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 0, 0);
    gtk_table_attach(GTK_TABLE(tbl), bank_switch, 0, 1, 2, 4,
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_EXPAND |
					GTK_FILL),
		     __GTKATTACHOPTIONS(GTK_SHRINK | GTK_EXPAND |
					GTK_FILL), 0, 0);
    gtk_table_attach(GTK_TABLE(tbl), up, 2, 3, 1, 2, __GTKATTACHOPTIONS(0),
		     __GTKATTACHOPTIONS(0), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), down, 2, 3, 2, 3,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), del, 2, 3, 3, 4,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 0, 0);

    gtk_table_attach(GTK_TABLE(tbl), add, 4, 5, 1, 2,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 0, 0);
    gtk_table_attach(GTK_TABLE(tbl), tracker, 0, 1, 5, 6,
		     __GTKATTACHOPTIONS(0), __GTKATTACHOPTIONS(0), 0, 0);

    gtk_signal_connect(GTK_OBJECT(bank_add), "clicked",
		       GTK_SIGNAL_FUNC(bank_add_pressed), NULL);
    gtk_signal_connect(GTK_OBJECT(bank_switch), "clicked",
		       GTK_SIGNAL_FUNC(bank_switch_pressed), NULL);
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
    gtk_widget_show_all(mainWnd);

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
