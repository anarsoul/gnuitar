/*
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

void            rnd_window_pos(GtkWindow * wnd);
void
                init_gui(void);
gint delete_event(GtkWidget* widget, GdkEvent* event, gpointer data);

extern GtkWidget *processor;

#endif
