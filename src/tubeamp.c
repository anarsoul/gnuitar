/*
 * GNUitar
 * Distortion effect 3 -- Tube amplifier
 * Copyright (C) 2006 Antti S. Lankila  <alankila@bel.fi>
 *
 * GPL license.
 *
 * $Id$
 *
 * $Log$
 * Revision 1.18  2006/05/18 20:58:05  alankila
 * - mimick Marshall Pro Jr
 *
 * Revision 1.17  2006/05/17 10:22:38  alankila
 * - further parameter tuning
 * - flip bias term to drive the distortion to occur the other way
 *
 * Revision 1.16  2006/05/15 19:08:26  alankila
 * - remove several multiplier terms, collect them under F_tube
 * - some remaining constants such as 250 indicate original tube mains voltage
 * - increase feedback strength by some 10x -- warmer now
 *
 * Revision 1.15  2006/05/15 16:00:59  alankila
 * - move middle control into the inner loop
 *
 * Revision 1.14  2006/05/15 11:32:18  alankila
 * - rebalance bias for "punch"
 *
 * Revision 1.13  2006/05/15 10:55:46  alankila
 * - make it sound sweeter
 * - make initial lowpass IIR stronger
 *
 * Revision 1.12  2006/05/15 09:45:01  alankila
 * - new model according to Antti Huoviala et al. "Virtual Air Guitar",
 *   presented at 117th Audio Engineering Society conference.
 * - still missing: proper F_tube.
 *
 * Revision 1.11  2006/05/13 16:20:23  alankila
 * - further small tuning of the formula and the defaults
 *
 * Revision 1.10  2006/05/13 13:33:38  alankila
 * - new biasfreq, more tubish sound
 *
 * Revision 1.9  2006/05/13 08:53:05  alankila
 * - load/save functions
 *
 * Revision 1.8  2006/05/13 08:04:23  alankila
 * - parameter lickup
 *
 * Revision 1.7  2006/05/08 07:29:14  alankila
 * - improve nonlinearity crunch a bit -- add gain -- get by with less stages
 *
 * Revision 1.6  2006/05/07 22:39:18  alankila
 * - correct some errors in the model
 *
 * Revision 1.5  2006/05/07 21:53:26  alankila
 * - remove some multiplications in the waveshaper path
 *
 * Revision 1.4  2006/05/07 18:58:53  alankila
 * - knob to tone the midrange distortion fatness
 *
 * Revision 1.3  2006/05/07 18:22:23  alankila
 * - produce controls and more aggressive defaults
 *
 * Revision 1.2  2006/05/07 14:38:15  alankila
 * - reimplement shelve as partial highpass
 * - use less stages
 *
 * Revision 1.1  2006/05/07 13:22:12  alankila
 * - new bare bones distortion effect: tubeamp
 *
 *
 */

#include <math.h>
#include <gui.h>
#include <stdlib.h>

#include "pump.h"
#include "biquad.h"
#include "tubeamp.h"

#define UPSAMPLE_RATIO 4

/* Narshall Pro Jr */
int impulse[256] = {
  4138,  14611,  26189,  34250,  37102,  34071,  26597,  16581,   7236,   2504,   1816,
  1633,    800,   -196,  -1047,   -969,    137,   1698,   3159,   3497,   2204,    399,
 -1583,  -4201,  -7083, -10054, -13786, -17384, -18258, -15413, -10370,  -5328,  -1841,
  -382,  -1525,  -4882,  -7757,  -8338,  -7171,  -5643,  -4865,  -4798,  -4937,  -5238,
 -5322,  -5120,  -4950,  -4658,  -4049,  -3294,  -2562,  -2205,  -2353,  -2971,  -3915,
 -4662,  -4769,  -4166,  -3396,  -2813,  -2258,  -1482,   -675,   -198,   -131,   -414,
  -983,  -1688,  -2454,  -3162,  -3583,  -3602,  -3422,  -3060,  -2433,  -1858,  -1866,
 -2707,  -3905,  -4531,  -4241,  -3507,  -2595,  -1626,   -925,   -556,   -103,    976,
  2625,   3995,   4524,   4214,   3286,   2029,    577,  -1039,  -2628,  -4108,  -5456,
 -6359,  -6527,  -5884,  -4494,  -2661,   -927,    361,   1241,   1894,   2221,   1833,
   574,  -1097,  -2559,  -3387,  -3414,  -2812,  -1936,  -1234,   -999,   -990,   -650,
   272,   1553,   2671,   3237,   3259,   2924,   2415,   1840,   1189,    453,   -282,
  -829,  -1002,   -718,   -226,    146,    347,    455,    501,    530,    470,    268,
  -159,   -857,  -1543,  -1878,  -1698,  -1072,   -253,    563,   1311,   2039,   2823,
  3560,   3939,   3677,   2807,   1718,    847,    343,     57,   -184,   -355,   -337,
    -7,    656,   1508,   2346,   3003,   3333,   3253,   2798,   2143,   1504,    979,
   417,   -299,  -1098,  -1740,  -1960,  -1694,  -1113,   -459,     -9,      4,   -417,
 -1113,  -1837,  -2300,  -2307,  -1811,   -964,    -75,    633,   1048,   1109,    854,
   402,    -90,   -446,   -545,   -349,     25,    332,    380,    179,   -170,   -601,
 -1008,  -1214,  -1079,   -637,    -60,    428,    620,    385,   -219,   -994,  -1763,
 -2413,  -2872,  -3076,  -2973,  -2598,  -2076,  -1529,  -1010,   -508,    -10,    504,
  1041,   1599,   2127,   2540,   2741,   2655,   2261,   1620,    873,    173,   -410,
  -812,   -949,   -808,   -467,    -43,    329,    539,    593,    593,    622,    716,
   859,    989,   1040
};

static void
update_stages(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->stages = adj->value;
}

static void
update_gain(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->gain = adj->value;
}

static void
update_asymmetry(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->asymmetry = adj->value;
}

static void
update_biasfactor(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->biasfactor = adj->value;
}

/*
static void
update_middlefreq(GtkAdjustment *adj, struct tubeamp_params *params)
{
    params->middlefreq = adj->value;
}*/

static void
tubeamp_init(struct effect *p)
{
    GtkWidget      *w;
    GtkObject      *o;
    GtkWidget      *parmTable;
    struct tubeamp_params *params = p->params;
    
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);

    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(5, 2, FALSE);

    w = gtk_label_new("Stages\n(n)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->stages, 2, MAX_STAGES, 1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_stages), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_scale_set_digits(GTK_SCALE(w), 0);
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    
    w = gtk_label_new("Gain\n(dB)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 1, 2, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->gain, 30.0, 45.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_gain), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 1, 2, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);

    w = gtk_label_new("Absolute bias");
    gtk_table_attach(GTK_TABLE(parmTable), w, 2, 3, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->asymmetry, 2000.0, 3500.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_asymmetry), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 2, 3, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    /*
    w = gtk_label_new("Middle cut\n(dB)");
    gtk_table_attach(GTK_TABLE(parmTable), w, 3, 4, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->middlefreq, -5.0, 0.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_middlefreq), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_range_set_inverted(GTK_RANGE(w), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 3, 4, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    */
    w = gtk_label_new("Dynamic bias");
    gtk_table_attach(GTK_TABLE(parmTable), w, 4, 5, 0, 1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->biasfactor, 1.0, 40.0, 0.1, 1, 0);
    gtk_signal_connect(GTK_OBJECT(o), "value_changed",
                       GTK_SIGNAL_FUNC(update_biasfactor), params);
    w = gtk_vscale_new(GTK_ADJUSTMENT(o));
    gtk_widget_set_size_request(GTK_WIDGET(w), 50, 100);
    gtk_table_attach(GTK_TABLE(parmTable), w, 4, 5, 1, 2,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND | GTK_SHRINK),
                     3, 0);
    
    w = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
    gtk_signal_connect(GTK_OBJECT(w), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), w, 0, 1, 3, 4,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Tube amplifier"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

/* no decent waveshaping at the moment */
static float
F_tube(float in, float r_i)
{
    r_i /= 3000;
    return tanh(in / r_i) * r_i;
}

static void
tubeamp_filter(struct effect *p, struct data_block *db)
{
    int i, j, k, curr_channel = 0;
    struct tubeamp_params *params = p->params;
    float gain;

    /*
    for (j = 0; j < params->stages; j += 1)
        set_peq_biquad(sample_rate * UPSAMPLE_RATIO, 720, 500.0, params->middlefreq, &params->middlecut[j]);
    */
    gain = pow(10, params->gain / 20);
    
    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < db->len; i += 1) {
        float result;
        for (k = 0; k < UPSAMPLE_RATIO; k += 1) {
            /* IIR interpolation */
            params->in[curr_channel] = (db->data[i] + params->in[curr_channel] * 5) / 6.0;
            result = params->in[curr_channel] / MAX_SAMPLE;
            for (j = 0; j < params->stages; j += 1) {
                /* gain of the block */
                result *= gain;
                /* low-pass filter that mimicks input capacitance */
                result = do_biquad(result, &params->lowpass[j], curr_channel);
                /* add feedback bias current for "punch" simulation */
                result += params->bias[j];
                /* run waveshaper */
                result = F_tube(result, params->r_i[j]);
                /* feedback bias */
                params->bias[j] = do_biquad((params->asymmetry - params->biasfactor * result) * params->r_k[j] / params->r_p[j], &params->biaslowpass[j], curr_channel);
                /* high pass filter to remove bias from the current stage */
                result = do_biquad(result, &params->highpass[j], curr_channel);
                /* middlecut for user tone control, for the "metal crunch" sound */
                //result = do_biquad(result, &params->middlecut[j], curr_channel);
                result = -result;
            }
            /* final tone control, a poor man's cabinet simulation */
        }
        DSP_SAMPLE *ptr = params->buf[curr_channel];
        int bufidx = params->bufidx[curr_channel];
        /* convolve the output */
        ptr[bufidx] = result / 200 * (MAX_SAMPLE >> 12);
        DSP_SAMPLE val = 0;
        for (j = 0; j < 255; j += 1) {
            val += ptr[(bufidx - j) & 0xff] * impulse[j];
        }
        db->data[i] = val >> 6;
        params->bufidx[curr_channel] += 1;
        if (params->bufidx[curr_channel] == 256)
            params->bufidx[curr_channel] = 0;
        curr_channel = (curr_channel + 1) % db->channels;
    }
    //for (i = 0; i < params->stages; i += 1)
    //    printf("%d. bias=%.1f\n", i, params->bias[i]);
}

static void
tubeamp_done(struct effect *p)
{
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
tubeamp_save(struct effect *p, SAVE_ARGS)
{
    struct tubeamp_params *params = p->params;
    SAVE_INT("stages", params->stages);
    SAVE_DOUBLE("gain", params->gain);
    SAVE_DOUBLE("middlefreq", params->middlefreq);
}

static void
tubeamp_load(struct effect *p, LOAD_ARGS)
{
    struct tubeamp_params *params = p->params;
    LOAD_INT("stages", params->stages);
    LOAD_DOUBLE("gain", params->gain);
    LOAD_DOUBLE("middlefreq", params->middlefreq);
}

effect_t *
tubeamp_create()
{
    effect_t   *p;
    struct tubeamp_params *params;

    p = calloc(1, sizeof(effect_t)); 
    params = p->params = calloc(1, sizeof(struct tubeamp_params));
    p->proc_init = tubeamp_init;
    p->proc_filter = tubeamp_filter;
    p->proc_save = tubeamp_save;
    p->proc_load = tubeamp_load;
    p->toggle = 0;
    p->proc_done = tubeamp_done;

    params->stages = 3;
    params->gain = 40.0;
    params->biasfactor = 20;
    params->asymmetry = 2500;

    /* configure the various stages */
    params->r_i[0] = 68e3;
    params->r_p[0] = 100000;
    params->r_k[0] = 2700;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 22570, &params->lowpass[0]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 86, &params->biaslowpass[0]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[0]);
    
    params->r_i[1] = 250e3;
    params->r_p[1] = 100000;
    params->r_k[1] = 1500;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 6531, &params->lowpass[1]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 132, &params->biaslowpass[1]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[1]);
    
    params->r_i[2] = 250e3;
    params->r_p[2] = 100000;
    params->r_k[2] = 820;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 6531, &params->lowpass[2]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 194, &params->biaslowpass[2]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[2]);
    
    params->r_i[3] = 250e3;
    params->r_p[3] = 100000;
    params->r_k[3] = 810;
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 6531, &params->lowpass[3]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 194, &params->biaslowpass[3]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[3]);
    
    return p;
}
