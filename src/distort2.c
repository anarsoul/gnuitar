/*
 * GNUitar
 * Distortion effect
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
 * Revision 1.10  2005/04/06 19:34:20  fonin
 * Code lickup
 *
 * Revision 1.9  2004/10/21 11:05:40  dexterus
 * Fully working realtime version
 * Fixed bugs , improved sound, added oversampling
 * Note: this is an mathematically accurate simulation of
 * Ibanez Tube Screamer 9, with the excetion of diodes electrical paramaters
 * ( modified to make it sound more aggresive )
 *
 * Revision 1.8  2004/08/10 15:11:01  fonin
 * Reworked distortion - process in realtime rather then use lookup table
 *
 * Revision 1.7  2004/07/07 19:18:42  fonin
 * GTK2 port
 *
 * Revision 1.6  2003/04/21 09:35:51  fonin
 * Bugfix with missing parameter in strncat(), line 74.
 *
 * Revision 1.5  2003/04/17 12:22:00  fonin
 * More search paths for lookup dir.
 *
 * Revision 1.4  2003/04/16 18:40:00  fonin
 * - lookup dir search paths for Win32;
 * - R1 parameter should be inverted 100% == 1% and vice versa.
 *
 * Revision 1.3  2003/04/16 13:58:39  fonin
 * - trying to guess the lookup directory;
 * - filling the lookup table with constant 32767 by default.
 *
 * Revision 1.2  2003/04/12 20:00:56  fonin
 * Stupid bugfix (forgot to move forward buffer pointer
 * in the filter function); "level" control taken out.
 *
 * Revision 1.1  2003/04/11 18:32:24  fonin
 * New distortion effect.
 *
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include "distort2.h"
#include "gui.h"
#include "utils.h"
#include <math.h>


#define UPSAMPLE	4

#define DIST2_DOWNSCALE	16.0 / MAX_SAMPLE 	/* Used to reduce the signal to */
						/* the limits needed by the     */
						/* simulation                   */
#define DIST2_UPSCALE	MAX_SAMPLE * 0.2	/* And back to the normal range */
/* taken as a funtion of MAX_SAMPLE because that is the reference for 
 * what the 'normal' signal should be */

/* Check if the compiler is Visual C or GCC */
#if defined(_MSC_VER) || defined(__GNUC__)
#   pragma intrinsic (exp,log)
#endif

void            distort2_filter(struct effect *p, struct data_block *db);

void
update_distort2_drive(GtkAdjustment * adj, struct distort2_params *params)
{
    params->r2 = (int) adj->value * 5000;
    params->r2 -= params->r2 % 10;
    params->r2 += 50;
}

void
update_distort2_treble(GtkAdjustment * adj, struct distort2_params *params)
{
    params->noisegate = (int) adj->value;
    RC_set_freq(params->noisegate, &(params->noise));
}

void
toggle_distort2(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = distort2_filter;
	p->toggle = 1;
    }
}

void
toggle_treble(void *bullshit, struct distort2_params *params)
{
    params->treble = !params->treble;
}

void
distort2_init(struct effect *p)
{
    struct distort2_params *pdistort;

    GtkWidget      *drive;
    GtkWidget      *drive_label;
    GtkObject      *adj_drive;

    GtkWidget      *treble;
    GtkWidget      *treble_label;
    GtkObject      *adj_treble;

    GtkWidget      *button;
    GtkWidget      *treble_switch;

    GtkWidget      *parmTable;

    pdistort = (struct distort2_params *) p->params;

    /*
     * GUI Init
     */
#ifdef HAVE_GTK
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
#elif defined HAVE_GTK2
    p->control = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(4, 2, FALSE);

    adj_drive = gtk_adjustment_new((pdistort->r2 + 50) / 100,
				   1.0, 101.0, 1, 1, 1);
    drive_label = gtk_label_new("Drive\n%");
    gtk_table_attach(GTK_TABLE(parmTable), drive_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_drive), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_drive), pdistort);
    drive = gtk_vscale_new(GTK_ADJUSTMENT(adj_drive));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(drive),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), drive, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_treble = gtk_adjustment_new(pdistort->noisegate,
				   1000.0, 6001.0, 1, 1, 1);
    treble_label = gtk_label_new("Treble\nHz");
    gtk_table_attach(GTK_TABLE(parmTable), treble_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_treble), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_treble), pdistort);
    treble = gtk_vscale_new(GTK_ADJUSTMENT(adj_treble));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(treble),0,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), treble, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_distort2), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }

    treble_switch = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(treble_switch), "toggled",
		       GTK_SIGNAL_FUNC(toggle_treble), pdistort);

    gtk_table_attach(GTK_TABLE(parmTable), treble_switch, 2, 3, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Distortion 2"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
distort2_filter(struct effect *p, struct data_block *db)
{
/* the original value doesn't produce enough distortion,
 * maybe wrong electrical parameters ? */
// #define mUt				  30*1e-3
#define mUt				  30*1e+2
#define Is				 10*1e-12

    int			i,count;
    static int		curr_channel = 0;
    DSP_SAMPLE 	       *s;
    struct distort2_params *dp;
    static double	x,y,x1,f,df,dx,e1,e2;
    static double upsample [UPSAMPLE];
    
#define DRIVE (dp->r2)

    dp = (struct distort2_params *) p->params;
    count = db->len;
    s = db->data;

    /*
     * process signal; x - input, in the range -1, 1
     */
    while (count) {
	/* scale down to -1..1 range */
	x = *s ;
	x *= DIST2_DOWNSCALE ;
	
	/* fist we prepare the lineary interpoled upsamples */
	y = 0;
	upsample[0] = dp->lastupsample;
	y = 1.0 / UPSAMPLE;  /* temporary usage of y */
	for (i=1; i< UPSAMPLE; i++)
	{
	    upsample[i] = dp->lastupsample + ( x - dp->lastupsample) *y;
	    y += 1.0 / UPSAMPLE;
	}
	dp->lastupsample = x;
	/* Now the actual upsampled processing */
	for (i=0; i<UPSAMPLE; i++)
	{
	    x = upsample[i]; /*get one of the upsamples */

	    /* first compute the linear rc filter current output */
	    y = dp->c0*x + dp->d1 * dp->lyf;
	    dp->lyf = y;
	    x1 = (x-y) / 4700.0;

	    /* start seaching from time previous point , improves speed */
	    y = dp->last[curr_channel]; 
	    do {
		/* f(y) = 0 , y= ? */
		e1 = exp ( (x-y) / mUt );  e2 = 1.0 / e1;
    	
		/* f=x1+(x-y)/DRIVE+Is*(exp((x-y)/mUt)-exp((y-x)/mUt));  optimized makes : */
		f = x1 + (x-y)/ DRIVE + Is * (e1 - e2);
	
		/* df/dy */
		/*df=-1.0/DRIVE-Is/mUt*(exp((x-y)/mUt)+exp((y-x)/mUt)); optimized makes : */
		df = -1.0 / DRIVE - Is / mUt * (e1 + e2);
	
		/* This is the newton's algo, it searches a root of a function,
		 * f here, which must equal 0, using it's derivate. */
		dx=f/df;
		y-=dx;
	    }
	    while ((dx>0.01)||(dx<-0.01));
	    /* when dx gets very small, we found a solution. */

	    dp->last[curr_channel] = y;
	    y = doBiquad( y, &dp->cheb, 0);
	    y = doBiquad( y, &dp->cheb1, 0);
	}

	/* scale up from -1..1 range */
	*s = y * DIST2_UPSCALE;

	/*if(*s > MAX_SAMPLE)
	    *s=MAX_SAMPLE;
	else if(*s < -MAX_SAMPLE)
	    *s=-MAX_SAMPLE;*/

	s++;
	count--;

	if (nchannels > 1)
	    curr_channel = !curr_channel;
    }
    if(dp->treble)
	RC_lowpass(db->data, db->len, &(dp->noise));
#undef DRIVE
}

void
distort2_done(struct effect *p)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;
    free(ap->cheb.mem); free(ap->cheb1.mem);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
distort2_save(struct effect *p, int fd)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;

    write(fd, &ap->r2, sizeof(ap->r2));
    write(fd, &ap->noise, sizeof(ap->noise));
    write(fd, &ap->treble, sizeof(ap->treble));
}

void
distort2_load(struct effect *p, int fd)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;

    read(fd, &ap->r2, sizeof(ap->r2));
    read(fd, &ap->noise, sizeof(ap->noise));
    read(fd, &ap->treble, sizeof(ap->treble));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = distort2_filter;
    }
}


void
distort2_create(struct effect *p)
{
    struct distort2_params *ap;
    int         i;
    double	Ts, Ts1;
    double	RC=(0.047*1e-6)*(4.7*1e+3);
    p->params =
	(struct distort2_params *) malloc(sizeof(struct distort2_params));
    ap = (struct distort2_params *) p->params;

    p->proc_init = distort2_init;
    p->proc_filter = passthru;
    p->proc_save = distort2_save;
    p->proc_load = distort2_load;
    p->toggle = 0;
    p->id = DISTORT2;
    p->proc_done = distort2_done;

    ap->r2 = 520;
//    ap->lowpass = 150;
    ap->noisegate = 3500;
    ap->treble = 0;

    RC_setup(10, 1, &(ap->noise));
    RC_set_freq(ap->noisegate, &(ap->noise));
    /* RC Filter tied to ground setup */
    Ts = 1.0/sample_rate;
    Ts1 = Ts / UPSAMPLE;  /* Ts1 is Ts for upsampled processing  */

    /* Init stuff
     * This is the rc filter tied to ground. */
    ap->c0 = Ts1 / (Ts1 + RC);
    ap->d1 = RC / (Ts1 + RC);
    ap->lyf = 0;

    for (i=0; i < nchannels; i++)
	ap->last[i] = 0;
    ap->lastupsample = 0;
    ap->cheb.mem = (double*) malloc ( nchannels * sizeof (double) * 4 );
    memset ( (void*) ap->cheb.mem, 0 , nchannels * sizeof (double) * 4 );
    ap->cheb1.mem = (double*) malloc ( nchannels * sizeof (double) * 4 );
    memset ( (void*) ap->cheb1.mem, 0 , nchannels * sizeof (double) * 4 );

    /* 2 lowPass Chebyshev fiters used in downsampling */
    CalcChebyshev2(sample_rate * UPSAMPLE, 12000, 1, 1, &ap->cheb);
    CalcChebyshev2(sample_rate * UPSAMPLE, 5500, 1, 1, &ap->cheb1);
}
