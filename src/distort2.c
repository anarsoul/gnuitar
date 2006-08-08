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
 * Revision 1.66  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.65  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.64  2006/08/02 19:07:56  alankila
 * - add missing static declarations
 *
 * Revision 1.63  2006/07/27 19:24:41  alankila
 * - aligned memory needs aligned free operation.
 *
 * Revision 1.62  2006/07/14 14:21:30  alankila
 * - forgot to use the interpolated values in the actual loop.
 *
 * Revision 1.61  2006/07/08 18:11:33  alankila
 * - reduce overdrive effect cpu drain by implementing low-pass filtering
 *   in resampler and reusing the static 720 Hz lowpass filter as decimating
 *   filter. Should be 10-20 % faster.
 *
 * Revision 1.60  2006/05/29 23:46:02  alankila
 * - move _GNU_SOURCE into Makefile
 * - align memory for x86-32; x86-64 already aligned memory for us in glibc
 *   so we didn't crash. This is done through new gnuitar_memalign().
 * - cater to further restrictions in SSE instructions for x86 arhictecture:
 *   it appears that mulps memory must be aligned to 16 too. This crashed
 *   all biquad-using functions and tubeamp. :-(
 *
 * Revision 1.59  2006/05/19 11:38:30  alankila
 * - clip % was always almost identical to gain.
 * - replace clip control by level control, similar to real tubescreamer
 *
 * Revision 1.58  2005/11/01 12:32:44  alankila
 * - further reduce the fuzz level to more closely match real TS9
 * - increase bass cut
 *
 * Revision 1.57  2005/10/30 12:07:27  alankila
 * - the highpass is buggy; revert back to inverted lowpass
 * - do similar bass cut as real TS-9 does
 * - reduce clipping strength slightly
 *
 * Revision 1.56  2005/10/30 11:21:05  alankila
 * - more correct and precise output filtering!
 * - real device seems to have some kind of highpass filtering around 50 Hz
 *   maybe or so, because there's too much bass...
 *
 * Revision 1.55  2005/09/24 11:21:16  alankila
 * - remove the way incorrect simulation of the 51 pF capacitor. I need to
 *   puzzle out the discrete time equation for it.
 *
 * Revision 1.54  2005/09/20 16:31:10  alankila
 * - finally bought a TS9, now tuned the sound to fairly good approximation
 *
 * Revision 1.53  2005/09/13 18:23:35  alankila
 * - reinstate some old code, offer it as a toggle
 * - make standard "authentic" emulation harder
 * - improve recovery from newton convergence failures
 *
 * Revision 1.52  2005/09/13 08:18:28  alankila
 * - reduce the bandwidth of input to newton, hopefully giving fractionally
 *   yet more reliable behaviour overall
 * - reconsider the bandwidth of the downsampling, now setting it at maximum
 *
 * Revision 1.51  2005/09/10 10:53:38  alankila
 * - remove the need to reserve biquad's mem in caller's side
 *
 * Revision 1.50  2005/09/08 19:31:48  alankila
 * - fix one C++ declaration, tune clip % a bit
 *
 * Revision 1.49  2005/09/08 07:58:29  alankila
 * - tubescreamy defaults
 * - flipped clip % slider
 * - "authentic" output filtering: 720 Hz lowpass + variable 3200 Hz highpass
 *
 * Revision 1.48  2005/09/07 11:08:12  alankila
 * - oddly enough, the original equation was right all along. The problem that
 *   I was trying to repair by ear was probably in the filtering and too low
 *   upsampling of the original code. My month of work finally boils down to
 *   adding a new exponential function that makes the clipping more interesting
 *   and simulating the capacitor left out in the original.
 *
 * Revision 1.47  2005/09/04 23:17:07  alankila
 * - gtk+ ui fixes
 *
 * Revision 1.46  2005/09/04 23:05:17  alankila
 * - delete the repeated toggle_foo functions, use one global from gui.c
 *
 * Revision 1.45  2005/09/04 14:40:17  alankila
 * - get rid of effect->id and associated enumeration
 *
 * Revision 1.44  2005/09/04 12:12:35  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.43  2005/09/04 11:16:59  alankila
 * - destroy passthru function, move the toggle logic higher up
 *
 * Revision 1.42  2005/09/04 01:51:09  alankila
 * - GKeyFile-based preset load/save
 * - still need locale-immune %lf for printf and sscanf
 *
 * Revision 1.41  2005/09/03 23:29:03  alankila
 * - I finally cracked the alignment problem. You align GTK labels with
 *   gtk_misc_set_alignment.
 *
 * Revision 1.40  2005/09/03 12:14:02  alankila
 * - decrypt distort2 on-disk format
 *
 * Revision 1.39  2005/09/02 11:58:49  alankila
 * - remove #ifdef HAVE_GTK2 entirely from all effect code
 *
 * Revision 1.38  2005/09/01 23:52:15  alankila
 * - make window delete event do something useful
 *
 * Revision 1.37  2005/09/01 17:31:40  alankila
 * - various small fixes for multichannel / gui
 *
 * Revision 1.36  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.35  2005/08/24 10:49:47  fonin
 * Minor change to compile on windows (#include utils.h for isnan)
 *
 * Revision 1.34  2005/08/23 22:01:34  alankila
 * - add -Wall to ease developing
 * - fix warnings
 *
 * Revision 1.33  2005/08/22 22:11:59  alankila
 * - change RC filters to accept data_block
 * - LC filters have no concept of "LOWPASS" or "HIGHPASS" filtering, there's
 *   just filter_no.
 * - remove unused SAMPLE8 typedef
 *
 * Revision 1.32  2005/08/22 18:23:38  alankila
 * more self-documenting
 *
 * Revision 1.31  2005/08/22 18:11:49  alankila
 * - add a rough approximation of the gain-cutting 51 pF capacitor, update
 *   circuit diagram
 *
 * Revision 1.30  2005/08/22 15:51:38  alankila
 * - after long consideration, x2-y sounds better
 *
 * Revision 1.29  2005/08/20 22:17:55  alankila
 * This version sounds fairly closely to Boss's OD-3 pedal. In fact, now I
 * can duplicate its sound fairly well, and this is only the beginning...
 *
 * - downscale other diode pair current by factor of 20 to get wider
 *   harmonics
 * - make Is depend on mUt, update mUt range, again upsample more due to
 *   derivation difficulties with small mUt values and high pitches
 * - finetune filtering, now disable treble filter by default to not eat
 *   the bright upper harmonics
 *
 * Revision 1.28  2005/08/20 00:15:22  alankila
 * - update derivation expression to get rid of the damp factor
 *
 * Revision 1.27  2005/08/19 11:24:27  alankila
 * - make the tone knob a lot subtler by dropping the iterations to 1.
 * - add a default RC filter @ 3000 Hz, take off one chebyshev
 *   (according to some documentation, a real tubescreamer has filter tuned
 *    at 723 Hz but this is way too strong for my tastes)
 * - fix drive knob range (up 50 kohm)
 * - use the voltage at negative feedback loop's RC for drive
 *   (reduces distortion for bass)
 * - maybe should use exp((x2-y) / mUt) instead of exp((x-y) / mUt) but this
 *   makes only small difference and x-y seems to sound better, go figure
 * - drop upsampling again
 *
 * Revision 1.26  2005/08/18 23:54:32  alankila
 * - use GTK_WINDOW_DIALOG instead of TOPLEVEL, however #define them the same
 *   for GTK2.
 *
 * Revision 1.25  2005/08/18 19:06:20  alankila
 * argh, due to amp treble setting I tuned the noisegate up too much
 *
 * Revision 1.24  2005/08/18 18:57:39  alankila
 * - finetune ranges a bit, add missing scale factor to not alter Is
 *
 * Revision 1.23  2005/08/18 16:52:21  alankila
 * - make the effect more powerful:
 *   * use 2 exponentials per diode instead of one
 *   * need to damp newton now to control oscillations
 *   * reduce upsampling precision to compensate added cpu drain
 * - reduce discant filtering considerably
 * - improve gui a bit during window scaling
 * - change default values a bit again
 *
 * Revision 1.22  2005/08/14 23:33:04  alankila
 * - multichannel-clean version
 *
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

/* Simplified tubescreamer circuitry:
 *
 *                 500k 
 *              +-adj. R-+ "DRIVE"
 *              |        |
 *          51k R        |
 *              |   51p  |
 *              +---||---+
 *              |        |
 *              +---|<---+
 *              | "CLIP" |
 *              +--->|---+
 *    4.7k  47n |        |
 *  +--R----||--+ .      |
 *  |           | |\     |
 *  BIAS        +-|-\    |
 *                |  \   |
 *                |   >--+--Output ("TREBLE" filter is RC filter here)
 *                |  /
 *  Signal--R--+--|+/
 *             |  |/
 *          BIAS  '
 * 
 * The op-amp produces output voltage great enough to cancel the signal
 * input at + pin.
 *
 * There is a 51 pF capacitor in parallel with the diodes. It eats away
 * the circuit's gain somewhat. This component is not modelled for now.
 */
 
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#    include "utils.h"
#endif
#include "distort2.h"
#include "gui.h"
#include <math.h>

#define RC_FEEDBACK_R          4.7e3
#define RC_FEEDBACK_C          47e-9
#define RC_DRIVE_C             51e-12   /* farads */
#define DRIVE_STATIC           50e3     /* ohms */
#define DRIVE_LOG              500e3    /* ohms */
 
#define MAX_NEWTON_ITERATIONS   20      /* limits the time looking for convergence */
#define UPSAMPLING_RATE         4       /* you can't change this, see upsample[] */
 
/* the effect is defined in -1.0 .. 1.0 range */
#define DIST2_DOWNSCALE		(1.0 / MAX_SAMPLE)
#define DIST2_UPSCALE		(MAX_SAMPLE / 1.0)
/* when we have converged within one 16-bit sample, accept value */
#define EFFECT_PRECISION	(1.0 / 32768)

/* Check if the compiler is Visual C or GCC */
#if defined(_MSC_VER)
#   pragma intrinsic (exp)
#   define expf(x) exp(x)
#endif

static void
update_distort2_drive(GtkAdjustment *adj, struct distort2_params *params)
{
    params->drive = adj->value;

}

static void
update_distort2_mUt(GtkAdjustment *adj, struct distort2_params *params)
{
    params->clip = adj->value;
}

static void
update_distort2_treble(GtkAdjustment *adj, struct distort2_params *params)
{
    params->treble = adj->value;
}

static void
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

    GtkWidget      *parmTable;

    pdistort = (struct distort2_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(4, 2, FALSE);

    adj_drive = gtk_adjustment_new(pdistort->drive,
				   0.0, 100.0, 0, 0, 0);
    drive_label = gtk_label_new("Drive\n%");
    gtk_label_set_justify(GTK_LABEL(drive_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), drive_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 3, 0);

    gtk_signal_connect(GTK_OBJECT(adj_drive), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_drive), pdistort);
    drive = gtk_vscale_new(GTK_ADJUSTMENT(adj_drive));
    gtk_widget_set_size_request(GTK_WIDGET(drive),50,100);

    gtk_table_attach(GTK_TABLE(parmTable), drive, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);

    adj_mUt = gtk_adjustment_new(pdistort->clip,
				 0.0, 100.0, 1.0, 5.0, 0);
    mUt_label = gtk_label_new("Level\n%");
    gtk_label_set_justify(GTK_LABEL(mUt_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), mUt_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 3, 0);

    gtk_signal_connect(GTK_OBJECT(adj_mUt), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_mUt), pdistort);
    mUt = gtk_vscale_new(GTK_ADJUSTMENT(adj_mUt));
    gtk_widget_set_size_request(GTK_WIDGET(mUt), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), mUt, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);

    
    adj_treble = gtk_adjustment_new(pdistort->treble,
				   -6.0, 6.0, 1, 1, 0);
    treble_label = gtk_label_new("Treble\ndB");
    gtk_label_set_justify(GTK_LABEL(treble_label), GTK_JUSTIFY_CENTER);
    gtk_table_attach(GTK_TABLE(parmTable), treble_label, 2, 3, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 3, 0);

    gtk_signal_connect(GTK_OBJECT(adj_treble), "value_changed",
		       GTK_SIGNAL_FUNC(update_distort2_treble), pdistort);
    treble = gtk_vscale_new(GTK_ADJUSTMENT(adj_treble));
    gtk_widget_set_size_request(GTK_WIDGET(treble),50,100);

    gtk_table_attach(GTK_TABLE(parmTable), treble, 2, 3, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 3, 0);

    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Overdrive"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

static void
distort2_filter(struct effect *p, data_block_t *db)
{

    int_fast32_t        i,count,bailout;
    int_fast8_t	        curr_channel = 0;
    DSP_SAMPLE 	       *s;
    struct distort2_params *dp = p->params;
    float x,y,x1,x2,f,df,dx,e1,e2;
    DSP_SAMPLE upsample[UPSAMPLING_RATE];
    float DRIVE = (float) DRIVE_STATIC + dp->drive / 100.0f * (float) DRIVE_LOG;
    float mUt = (20.0f + 100.f - 70.f) * 1e-3f;
    /* correct Is with mUt to approximately keep drive the
     * same. Original parameters said Is is 10e-12 and mUt 30e-3.
     * If mUt grows, Is must shrink. 0.39 is experimental */
    float Is = 10e-12f * expf(0.39f/30e-3f - 0.39f/mUt);
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
        for (i = 0; i < MAX_CHANNELS; i += 1) {
            dp->last[i] = 0;
        }
        return;
    }

    set_rc_lowpass_biquad(sample_rate * UPSAMPLING_RATE, 720, &dp->rolloff);
    /*
     * process signal; x - input, in the range -1, 1
     */
    while (count) {
        /* "properly" interpolate previous input at positions 0 and 2 */
        fir_interpolate_2x(dp->interpolate_firmem[curr_channel],
                           *s, &upsample[2], &upsample[0]);
        /* estimate the rest, this should be good enough for our purposes. */
        upsample[1] = (upsample[0] + upsample[2]) / (DSP_SAMPLE) 2;
        /* looking into firmem is a gross violation of interface. This will
         * go away once I design fir_interpolate_4x. */
        upsample[3] = ((DSP_SAMPLE) 3 * upsample[2] + dp->interpolate_firmem[curr_channel][2]) / (DSP_SAMPLE) 4;

	/* Now the actual upsampled processing */
	for (i = 0; i < UPSAMPLING_RATE; i++)
	{
            /* scale down to -1 .. 1 range */
            x = upsample[i] * (float) DIST2_DOWNSCALE;
            
	    /* first compute the linear rc filter current output */
	    x2 = do_biquad(x, &dp->feedback_minus_loop, curr_channel);
            
            x1 = (x - x2) / (float) RC_FEEDBACK_R;
            
	    /* start searching from time previous point , improves speed */
	    y = dp->last[curr_channel];
            /* limit iterations if the algorithm fails to converge */
            bailout = MAX_NEWTON_ITERATIONS;
	    do {
		/* f(y) = 0 , y= ? */
                /* e^3 ~ 20 */
                e1 = expf((x - y) / mUt);
                e2 = 1.0f / e1;
		/* f=x1+(x-y)/DRIVE+Is*(exp((x-y)/mUt)-exp((y-x)/mUt));  optimized makes : */
		f = x1 + (x - y) / DRIVE + Is * (e1 - e2);
	
		/* df/dy */
		/*df=-1.0/DRIVE-Is/mUt*(exp((x-y)/mUt)+exp((y-x)/mUt)); optimized makes : */
		df = -1.0f / DRIVE - Is / mUt * (e1 + e2);
	
		/* This is the newton's algo, it searches a root of a function,
		 * f here, which must equal 0, using it's derivate. */
		dx = f/df;
		y -= dx;
	    }
	    while (fabs(dx) > (float) EFFECT_PRECISION && --bailout);
	    /* when dx gets very small, we found a solution. */
	    
            /* Ensure that the value gets reset if something goes wrong */
            if (isnan(y) || ! (y >= -2.0f && y <= 2.0f))
                y = 0.f;
            
	    dp->last[curr_channel] = y;
            /* static lowpass filtering -- this doubles as our decimation filter
             * the rolloff is at 720 Hz, but is only 6 dB/oct. */
            y = do_biquad(y, &dp->rolloff, curr_channel);
	}
        /* treble control + other final output filtering */
        y += (y - do_biquad(y, &dp->treble_highpass, curr_channel)) * dp->treble / 3.0f;
        y = do_biquad(y, &dp->output_bass_cut, curr_channel);
	
        /* scale up from -1..1 range */
	*s = y * (float) DIST2_UPSCALE * (dp->clip / 100.0f);

	/*if(*s > MAX_SAMPLE)
	    *s=MAX_SAMPLE;
	else if(*s < -MAX_SAMPLE)
	    *s=-MAX_SAMPLE;*/

	s++;
	count--;

        curr_channel = (curr_channel + 1) % db->channels;
    }
}

static void
distort2_done(struct effect *p)
{
    struct distort2_params *ap;

    ap = (struct distort2_params *) p->params;
    gnuitar_free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
distort2_save(struct effect *p, SAVE_ARGS)
{
    struct distort2_params *params = p->params;

    SAVE_DOUBLE("drive", params->drive);
    SAVE_DOUBLE("clip", params->clip);
    SAVE_DOUBLE("treble", params->treble);
}

static void
distort2_load(struct effect *p, LOAD_ARGS)
{
    struct distort2_params *params = p->params;

    LOAD_DOUBLE("drive", params->drive);
    LOAD_DOUBLE("clip", params->clip);
    LOAD_DOUBLE("treble", params->treble);
}

effect_t *
distort2_create()
{
    effect_t   *p;
    struct distort2_params *ap;

    p = calloc(1, sizeof(effect_t)); 
    p->params = gnuitar_memalign(1, sizeof(struct distort2_params));
    p->proc_init = distort2_init;
    p->proc_filter = distort2_filter;
    p->proc_save = distort2_save;
    p->proc_load = distort2_load;
    p->toggle = 0;
    p->proc_done = distort2_done;

    ap = p->params;
    ap->drive = 0.0;
    ap->clip = 100.0;
    ap->treble = 6.0;

    /* static shapers */
    set_rc_lowpass_biquad(sample_rate, 3200, &ap->treble_highpass);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLING_RATE, 
            1 / (2 * M_PI * RC_FEEDBACK_R * RC_FEEDBACK_C),
            &ap->feedback_minus_loop);
    set_rc_highpass_biquad(sample_rate, 160, &ap->output_bass_cut);
    
    return p;
}
