/*
 * GNUitar
 * Amp effect
 * Copyright (C) 2006 Vasily Khoruzhick         <anarsoul@gmail.com>
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
 * Revision 1.4  2006/08/02 19:21:04  alankila
 * - add static declarations
 *
 * Revision 1.3  2006/05/29 18:35:39  anarsoul
 * Added macroses id and log to files
 *
 */

#include "amp.h"
#include "gui.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif

static void
update_amp_power(GtkAdjustment *adj, struct amp_params *params)
{
    params->amp_power = adj->value;
}


static void
amp_init(struct effect *p)
{
    struct amp_params *pamp;

    GtkWidget      *power;
    GtkWidget      *power_label;
    GtkObject      *adj_power;

    GtkWidget      *button;

    GtkWidget      *parmTable;


    pamp = (struct amp_params *) p->params;


    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(3, 3, FALSE);

    adj_power = gtk_adjustment_new(pamp->amp_power,
				   -30, 30, 1, 1, 0);
    power_label = gtk_label_new("Amplification\n(dB)");
    gtk_table_attach(GTK_TABLE(parmTable), power_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_power), "value_changed",
		       GTK_SIGNAL_FUNC(update_amp_power), pamp);

    power = gtk_vscale_new(GTK_ADJUSTMENT(adj_power));
    gtk_widget_set_size_request(GTK_WIDGET(power),0,100);

    gtk_table_attach(GTK_TABLE(parmTable), power, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);


    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control), "Amp");
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

static void
amp_filter(effect_t *p, data_block_t *db)
{
    struct amp_params *params = p->params;
    int             i;
    float           power;

    power = pow(10, params->amp_power / 20.0);
    for (i = 0; i < db->len; i++)
        db->data[i] *= power;
}

static void
amp_done(struct effect *p)
{
    struct amp_params *dp;

    dp = (struct amp_params *) p->params;

    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
amp_save(struct effect *p, SAVE_ARGS)
{
    struct amp_params *params = p->params;

    SAVE_DOUBLE("amp_power", params->amp_power);
}

static void
amp_load(struct effect *p, LOAD_ARGS)
{
    struct amp_params *params = p->params;

    LOAD_DOUBLE("amp_power", params->amp_power);
}

effect_t *
amp_create()
{
    effect_t           *p;
    struct amp_params *pamp;

    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct amp_params));
    pamp = p->params;
    p->proc_init = amp_init;
    p->proc_filter = amp_filter;
    p->toggle = 0;
    p->proc_done = amp_done;
    p->proc_save = amp_save;
    p->proc_load = amp_load;

    pamp->amp_power=0;
    return p;
}
