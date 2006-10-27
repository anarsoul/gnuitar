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
#include "effect.h"
#include "glib12-compat.h"

#ifdef __cplusplus
#define __GTKATTACHOPTIONS GtkAttachOptions
#else
#define __GTKATTACHOPTIONS
#endif

#define VERSION "0.3.2"
#define DISCLAIMER  "This program is free software; you can redistribute it and/or modify\n" \
		    "it under the terms of the GNU General Public License as published by\n" \
		    "the Free Software Foundation; either version 2, or (at your option)\n" \
		    "any later version.\n\n" \
		    "This program makes usage of Glib and GTK+ libraries that are distributed\n" \
		    "under Library GNU Public License (LGPL).\n\n"
#define COPYRIGHT   "GNUitar "VERSION"\n" \
		    "Copyright (C) 2000,2001,2003,2004 Max Rudensky <fonin@omnistaronline.com>\n" \
		    "http://www.omnistaronline.com/~fonin/\n"

#define LICENSE     "GPL license"

void		init_gui(void);
gint            delete_event(GtkWidget *, GdkEvent *, gpointer);
void            toggle_effect(GtkWidget *, effect_t *);
void            bank_append_entry(gchar *entry);
void            set_vumeter_in_value(float power);
void            set_vumeter_out_value(float power);
void            gnuitar_printf(const char *fmt, ...);

extern GtkWidget *processor;
extern float master_volume;
extern float input_volume;
extern GtkObject* adj_input;
extern GtkObject* adj_master;

#endif
