/*
 * GNUitar
 * Graphics user interface: definitions
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
 */

#ifndef _GUI_H_
#define _GUI_H_ 1

#include <gtk/gtk.h>

#define MAINGUI_TITLE "GNUitar"

#ifdef __cplusplus
#define __GTKATTACHOPTIONS GtkAttachOptions
#else
#define __GTKATTACHOPTIONS
#endif

#define VERSION "0.2.0"
#define DISCLAIMER  "This program is free software; you can redistribute it and/or modify\n" \
		    "it under the terms of the GNU General Public License as published by\n" \
		    "the Free Software Foundation; either version 2, or (at your option)\n" \
		    "any later version.\n\n" \
		    "This program makes usage of Glib and GTK+ libraries that are distributed\n" \
		    "under Library GNU Public License (LGPL).\n\n"
#define COPYRIGHT   "GNUitar "VERSION"\n" \
		    "Copyright (C) 2000,2001,2003 Max Rudensky <fonin@ziet.zhitomir.ua>\n" \
		    "http://ziet.zhitomir.ua/~fonin/\n"
void
                init_gui(void);
gint            delete_event(GtkWidget * widget, GdkEvent * event,
			     gpointer data);

extern GtkWidget *processor;

#endif
