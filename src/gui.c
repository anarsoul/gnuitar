/*
 * GNUitar
 * Graphics user interface
 * Copyright (C) 2000,2001 Max Rudensky		<fonin@ziet.zhitomir.ua>
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
#include "gui.h"
#include "pump.h"
#include "tracker.h"

void            bank_start_save(GtkWidget * widget, gpointer data);
void            bank_start_load(GtkWidget * widget, gpointer data);
void            quit(GtkWidget * widget, gpointer data);

static GtkItemFactoryEntry mainGui_menu[] = {
    {"/_File", "<alt>F", NULL, 0, "<Branch>"},

    {"/File/_Open Layout", "<control>O",
     (GtkSignalFunc) bank_start_load, 0, NULL},
    {"/File/_Save Layout", "<control>S", (GtkSignalFunc) bank_start_save,
     0, NULL},
    {"/File/sep1", NULL, NULL, 0, "<Separator>"},
    {"/File/E_xit", NULL, (GtkSignalFunc) quit, 0, NULL},
    {"/_Help", NULL, NULL, 0, "<LastBranch>"},
    {"/_Help/About", NULL, NULL, 0, NULL}
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
    pump_stop();

    tracker_done();
}


void
rnd_window_pos(GtkWindow * wnd)
{
    int             x,
                    y;
    srand(time(NULL));
    x = 1 + (int) (800.0 * rand() / (RAND_MAX + 1.0));
    y = 1 + (int) (600.0 * rand() / (RAND_MAX + 1.0));
    gtk_widget_set_uposition(GTK_WIDGET(wnd), x, y);
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

    if (n < MAX_EFFECTS && effects_row >= 0
	&& effects_row <= EFFECT_AMOUNT) {
	audio_lock = 1;
	if (curr_row >= 0 && curr_row < n) {
	    idx = curr_row+1;
	    for (i = n; i > idx; i--) {
		effects[i] = effects[i - 1];
	    }
	    n++;
	} else {
	    idx = n++;
	}
	effects[idx] = (struct effect *) malloc(sizeof(struct effect));
	effect_list[effects_row].create_f(effects[idx]);
	effects[idx]->proc_init(effects[idx]);
	audio_lock = 0;

	gtk_clist_insert(GTK_CLIST(processor), idx,
			 &effect_list[effects[idx]->id].str);
	gtk_clist_select_row(GTK_CLIST(processor), idx, 0);

    }
}

void
bank_perform_add(GtkWidget * widget, GtkFileSelection * filesel)
{
    char           *fname;

    fname = gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
    gtk_clist_append(GTK_CLIST(bank), &fname);
    gtk_widget_destroy(GTK_WIDGET(filesel));
    gtk_clist_moveto(GTK_CLIST(bank), GTK_CLIST(bank)->rows - 1, 0, 0.5,
		     1.0);
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
    gtk_clist_get_text(GTK_CLIST(bank), bank_row, 0, &fname);
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
					"*.raw");
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
}
