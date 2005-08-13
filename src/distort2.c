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
 * Revision 1.21  2005/08/13 12:06:08  alankila
 * - removed bunch of #ifdef HAVE_GTK/HAVE_GTK2 regarding window type
 *
 * Revision 1.20  2005/08/11 17:57:21  alankila
 * - add some missing headers & fix all compiler warnings on gcc 4.0.1+ -Wall
 *
 * Revision 1.19  2005/08/10 11:02:28  alankila
 * - sync to biquad interface
 * - change scale factors in order to not clip in the effect
 *
 * Revision 1.18  2005/08/09 00:50:22  alankila
 * - rename window to "Overdrive"
 *
 * Revision 1.17  2005/08/09 00:16:10  alankila
 * - make mUt tunable, call it Clip
 * - add some safeguards for Newton convergence and sanity checks for result
 *
 * Revision 1.16  2005/08/08 12:02:11  fonin
 * include float.h on windows
 *
 * Revision 1.15  2005/08/07 12:48:21  alankila
 * Update distort2 parameters:
 *
 * - scale simulation parameters back to -1.0 .. +1.0 range
 * - use symmetric up/downscaling to not change master volume
 * - set noisegate filter at loading time
 * - fix the use of gtk_adjustment to use true ranges
 * - fix broken #defines with ()
 * - add defines for the RC filter: 4700 ohms, 47 nF.
 * - remove some casts, use calloc instead of malloc + zero
 * - change distortion sound a bit to remove the "lo-fi" sound:
 *   * set treble noisegate on by default
 *   * set treble filter range from 6000 up to 8000
 *   * change treble filter default value from 3500 to 7000
 *   * require greater precision (keep on deriving while
 *     fabs(dx) >= one 16-bit sample, before it was about twice that)
 *   * more upsampling (from 4 -> 6, it sounds a bit better imo)
 *   * change mUt parameter to 21 * 10^-3 instead of 30 * 10^-3. Smaller
 *     values provide a bit smoother distortion
 *
 * Revision 1.14  2005/07/25 12:05:04  fonin
 * Workaround for NaN problem - thanks Antti S. Lankila <alankila@bel.fi>
 *
 * Revision 1.13  2005/04/24 19:11:22  fonin
 * Optimized for zero input (after the noise filter) to avoid the extra calcs
 *
 * Revision 1.12  2005/04/18 12:55:50  fonin
 * Fixed a typo in src/distort2.c
 *
 * Revision 1.11  2005/04/15 14:33:29  fonin
 * Code lickup
 *
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
#    include <float.h>
#endif
#include "distort2.h"
#include "gui.h"
#include "utils.h"
#include <math.h>

#define RC_R          4700.0          /* ohms */
#define RC_C          (47 * 1e-9)     /* farads */

#define UPSAMPLE	6

#define DIST2_DOWNSCALE	(1.0 / MAX_SAMPLE) 	/* Used to reduce the signal to */
						/* the limits needed by the     */
						/* simulation                   */
#define DIST2_UPSCALE	(MAX_SAMPLE / 2.0)	/* And back to the normal range */
/* taken as a funtion of MAX_SAMPLE because that is the reference for 
 * what the 'normal' signal should be */

/* Check if the compiler is Visual C or GCC */
#if defined(_MSC_VER)
#   pragma intrinsic (exp,log)
#endif

void            distort2_filter(struct effect *p, struct data_block *db);

void
update_distort2_drive(GtkAdjustment * adj, struct distort2_params *params)
{
    params->r2 = (int) adj->value * 5000;
    params->r2 += 50;
}

void
update_distort2_mUt(GtkAdjustment * adj, struct distort2_params *params)
{
    /* original parameter was 30e-3 */
    params->mUt = (10.0 + adj->value / 3) * 1e-3;
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

    GtkWidget      *mUt;
    GtkWidget      *mUt_label;
    GtkObject      *adj_mUt;
    
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
    p->control = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), NULL);

    parmTable = gtk_table_new(4, 2, FALSE);

    adj_drive = gtk_adjustment_new((pdistort->r2 + 50) / 100,
				   0.0, 100.0, 0, 0, 0);
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
    gtk_widget_set_size_request(GTK_WIDGET(drive),50,100);
#endif

    gtk_table_attach(GTK_TABLE(parmTable), drive, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_mUt = gtk_adjustment_new((pdistort->mUt / 1e-3 - 10.0) * 3,
				 0.0, 100.0, 1.0, 5.0, 0);
    mUt_label = gtk_label_new("Clip\n%");
    gtk_label_set_justify(GTK_LABEL(mUt_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), mUt_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_mUt), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_mUt), pdistort);
    mUt = gtk_vscale_new(GTK_ADJUSTMENT(adj_mUt));
#ifdef HAVE_GTK2
    gtk_widget_set_size_request(GTK_WIDGET(mUt), 50, 100);
#endif
    gtk_table_attach(GTK_TABLE(parmTable), mUt, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    
    adj_treble = gtk_adjustment_new(pdistort->noisegate,
				   1000.0, 8000.0, 1, 1, 0);
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
    gtk_widget_set_size_request(GTK_WIDGET(treble),50,100);
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
    if (pdistort->treble)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(treble_switch), TRUE);
    gtk_signal_connect(GTK_OBJECT(treble_switch), "toggled",
		       GTK_SIGNAL_FUNC(toggle_treble), pdistort);

    gtk_table_attach(GTK_TABLE(parmTable), treble_switch, 2, 3, 3, 4,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Overdrive"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

void
distort2_filter(struct effect *p, struct data_block *db)
{
#define Is				 (10*1e-12)

    int			i,count,bailout;
    int		        curr_channel = 0;
    DSP_SAMPLE 	       *s;
    struct distort2_params *dp;
    static double	x,y,x1,f,df,dx,e1,e2,mUt;
    static double upsample [UPSAMPLE];
#define DRIVE (dp->r2)
    dp = (struct distort2_params *) p->params;
    mUt = dp->mUt;
    count = db->len;
    s = db->data;

    /* no input, no output :-) to avoid extra calc. Optimized for noise gate,
     * when all input is zero.
     * This is the heuristics - since there is no the standard function
     * in the ANSI C library that reliably compares the memory region
     * with the given byte, we compare just a few excerpts from an array.
     * If everything is zero, we have a large chances that all array is zero. */
    if(s[0]==0 && s[1]==0 && s[16]==0 && s[17]==0 &&
          s[24]==0 && s[25]==0 && s[32]==0 && s[33]==0 &&
	  s[buffer_size-1]==0) {
	dp->last[0]=dp->last[1]=dp->lastupsample=0;
        return;
    }

    /*
     * process signal; x - input, in the range -1, 1
     */
    while (count) {

	/* scale down to -1..1 range */
	x = *s ;
	x *= DIST2_DOWNSCALE ;
	
	/* first we prepare the lineary interpoled upsamples */
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
	    x1 = (x-y) / RC_R;

	    /* start searching from time previous point , improves speed */
	    y = dp->last[curr_channel];
            /* limit iterations if the algorithm fails to converge */
            bailout = 10;
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
		dx =f/df;
		y-=dx;
	    }
	    while (fabs(dx) > DIST2_DOWNSCALE && --bailout);
	    /* when dx gets very small, we found a solution. */

	    /* we can get NaN after all, let's check for this */
	    if(isnan(y))
                y=0.0;
            /* the real limits are -1.0 to +1.0 but we allow for some headroom */
            if (y > 3.0)
                y = 3.0;
            if (y < -3.0)
                y = -3.0;
            
	    dp->last[curr_channel] = y;
	    y = do_biquad( y, &dp->cheb, curr_channel);
	    y = do_biquad( y, &dp->cheb1, curr_channel);
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
    free(ap->cheb.mem);
    free(ap->cheb1.mem);
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
    write(fd, &ap->mUt, sizeof(ap->mUt));
    write(fd, &ap->noise, sizeof(ap->noise));
    write(fd, &ap->treble, sizeof(ap->treble));
}

void
distort2_load(struct effect *p, int fd)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;

    read(fd, &ap->r2, sizeof(ap->r2));
    read(fd, &ap->mUt, sizeof(ap->mUt));
    read(fd, &ap->noise, sizeof(ap->noise));
    read(fd, &ap->treble, sizeof(ap->treble));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = distort2_filter;
    }
    RC_set_freq(ap->noisegate, &(ap->noise));
}

void
distort2_create(struct effect *p)
{
    struct distort2_params *ap;
    int         i;
    double	Ts, Ts1;
    double	RC = RC_C * RC_R;
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
    ap->mUt = (10.0 + 50.0 / 3) * 1e-3;
    ap->noisegate = 7000;
    ap->treble = 1;

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
    ap->cheb.mem  = calloc(nchannels, sizeof (double) * 4);
    ap->cheb1.mem = calloc(nchannels, sizeof (double) * 4);

    /* 2 lowpass Chebyshev filters for downsampling */
    set_chebyshev1_biquad(sample_rate * UPSAMPLE, 12000, 1, 1, &ap->cheb );
    set_chebyshev1_biquad(sample_rate * UPSAMPLE, 5500,  1, 1, &ap->cheb1);
}
