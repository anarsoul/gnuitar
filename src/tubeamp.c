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
 * Revision 1.30  2006/05/25 17:14:30  alankila
 * - rebalance against new waveshaper equation
 *
 * Revision 1.29  2006/05/25 16:54:12  alankila
 * - use 12 dB/oct lowpass filter between stages to help with our treble
 *   problem. Sounds less aliased now.
 *
 * Revision 1.28  2006/05/25 11:32:51  alankila
 * - the simulated tube "hit bottom". Apply waveshaping on both sides of
 *   waveform, but differently.
 *
 * Revision 1.27  2006/05/25 09:03:05  alankila
 * - replace the SSE code with even faster version. Tubeamp effect now runs
 *   20 % faster on my computer. Add some alignment directives to make future
 *   use of SSE potentially easier.
 *
 * Revision 1.26  2006/05/24 19:49:45  alankila
 * - save some CPU by using only float as the DSP accuracy. We can do this
 *   because even floats give us > 100 dB SNR -- that simply has to be enough.
 * - implement tubeamp.c convolution with SSE where available. The floats are
 *   not one bit faster than the integer version of the code, though, but
 *   SSE2 should do the convolution bit faster. I'll see what I can do to
 *   optimize it further.
 *
 * Revision 1.25  2006/05/23 15:28:45  alankila
 * - allow negative parameters for more sound. Positive parameters largerly
 *   distort only treble, while negatives also distort bass.
 *
 * Revision 1.24  2006/05/20 22:19:04  alankila
 * - rebalance 4th stage
 *
 * Revision 1.23  2006/05/20 12:18:48  alankila
 * - tabularize the nonlinearity
 * - new asymmetric nonlinearity based on solving x - y = exp(y) - exp(-y)
 *   type equation
 *
 * Revision 1.22  2006/05/20 10:22:31  alankila
 * - simplify the convolution loop to extreme
 *   (it doesn't appear to vectorize no matter what I do, though)
 *
 * Revision 1.21  2006/05/19 16:16:18  alankila
 * - make the gui scale properly
 *
 * Revision 1.20  2006/05/19 07:30:58  alankila
 * - rebalance to Princeton II that I basically like
 *
 * Revision 1.19  2006/05/18 22:14:29  alankila
 * - more models (commented out)
 *
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

#define UPSAMPLE_RATIO 7

/*
 * Marshall Pro Jr
DSP_SAMPLE impulse[256] = {
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
*/

 /*
  * Marshall G15R
DSP_SAMPLE impulse[512] = {
  4405,  17364,  30475,  33517,  28810,  20846,   9309,  -4045, -13421, -13737,  -6939,
  -644,   2381,   4726,   4890,   -577,  -8708, -13224, -11835,  -6840,   -805,   5847,
 11158,  10895,   3963,  -5524, -11923, -13616, -12717,  -9577,  -4247,   -180,    568,
  -647,  -3434,  -7154,  -9508,  -9045,  -5524,   -436,   3582,   4809,   2857,   -600,
 -3441,  -5228,  -5474,  -3608,   -936,    568,    785,    961,   1268,    763,   -657,
 -1941,  -2769,  -3712,  -4174,  -3405,  -2362,  -2122,  -2521,  -2793,  -2499,  -1808,
 -1174,  -1051,  -1339,  -1579,  -1982,  -2796,  -3538,  -3834,  -3492,  -2315,   -813,
   461,   1832,   3235,   3535,   1981,   -688,  -2823,  -3401,  -2683,  -1290,    290,
  1276,    924,   -613,  -2299,  -3054,  -2582,  -1428,   -275,    411,    317,   -452,
 -1440,  -2337,  -2950,  -3028,  -2519,  -1646,   -584,    580,   1663,   2431,   2623,
  2135,   1272,    563,    325,    470,    861,   1568,   2341,   2490,   1838,    954,
   227,   -215,   -167,    182,    305,    140,    -70,   -334,   -565,   -404,    123,
   592,    852,   1030,   1158,   1222,   1264,   1225,    932,    389,   -103,   -322,
  -347,   -283,    -81,    280,    683,   1030,   1335,   1549,   1539,   1323,   1070,
   928,   1095,   1687,   2403,   2755,   2615,   2172,   1513,    736,    167,    -33,
   -77,    -72,     70,    261,    311,    206,     41,   -127,   -177,     79,    635,
  1166,   1339,   1139,    812,    641,    707,    913,   1170,   1414,   1597,   1697,
  1667,   1492,   1250,    993,    687,    328,      7,   -170,   -125,    118,    346,
   425,    483,    579,    577,    465,    405,    358,    138,   -141,   -242,   -189,
  -124,    -41,    136,    373,    533,    603,    652,    682,    710,    728,    673,
   560,    468,    378,    222,     39,    -20,    117,    391,    740,   1171,   1582,
  1737,   1543,   1113,    601,    108,   -273,   -479,   -516,   -482,   -554,   -802,
 -1038,  -1068,   -905,   -618,   -236,    137,    338,    388,    463,    582,    567,
   325,     52,    -29,     22,    141,    347,    481,    382,    206,    121,     38,
  -141,   -394,   -684,   -914,   -923,   -729,   -536,   -457,   -459,   -499,   -510,
  -412,   -254,   -135,   -115,   -243,   -509,   -723,   -695,   -588,   -668,   -874,
 -1000,   -994,   -927,   -852,   -743,   -587,   -456,   -436,   -472,   -439,   -281,
   -41,    140,    152,     17,   -204,   -454,   -649,   -740,   -707,   -573,   -403,
  -268,   -229,   -307,   -460,   -685,  -1015,  -1384,  -1657,  -1755,  -1664,  -1434,
 -1150,   -859,   -555,   -255,    -60,    -17,    -32,    -22,     -2,    -20,    -83,
  -169,   -288,   -388,   -351,   -173,     47,    273,    483,    573,    444,     86,
  -348,   -621,   -701,   -706,   -631,   -489,   -438,   -519,   -577,   -562,   -513,
  -373,   -123,    113,    269,    380,    438,    421,    343,    202,     -1,   -229,
  -401,   -450,   -374,   -220,    -65,     23,     47,     17,    -60,   -135,   -151,
   -83,     72,    264,    398,    415,    295,     73,   -134,   -237,   -268,   -307,
  -370,   -414,   -411,   -308,    -64,    233,    424,    422,    267,     72,    -23,
    63,    282,    545,    824,   1069,   1165,   1098,   1012,    931,    752,    577,
   612,    765,    831,    816,    783,    684,    537,    484,    564,    671,    759,
   843,    860,    742,    569,    459,    432,    459,    541,    680,    798,    812,
   741,    611,    453,    352,    382,    527,    728,    923,   1016,    928,    707,
   491,    393,    467,    670,    900,   1073,   1115,   1000,    778,    577,    520,
   562,    574,    536,    494,    407,    222,     15,    -98,   -111,    -97,    -68,
     6,     73,    109,    224,    442,    592,    535,    306,     37,   -127,   -114,
    23,    183,    268,    193,     30,    -61,    -74,    -46,     45,    156,    201,
   217,    275,    317,    268,    146,    -16,   -186,   -257,   -148,     74,    275,
   372,    338,    207,     53,    -63,   -131,   -187,   -265,   -372,   -482,   -551,
  -587,   -642,   -689,   -662,   -577,   -485
};
*/

 /* Princeton II */
DSP_SAMPLE_ALIGN impulse[512] = {
  2799,  11631,  23881,  32811,  34786,  30693,  22401,  12097,   3608,    333,   1986,
  5050,   5906,   3149,  -2263,  -7957, -11151,  -9808,  -4421,   1179,   2345,  -1974,
 -8064, -11426, -10826,  -7845,  -4476,  -2085,  -1307,  -1743,  -2306,  -2291,  -1539,
  -317,    118,  -1496,  -4272,  -5880,  -5257,  -2844,    410,   2743,   1949,  -2161,
 -6821,  -9053,  -8010,  -4596,   -871,    603,  -1322,  -5235,  -8392,  -8852,  -6539,
 -2851,    514,   2273,   1827,   -491,  -3243,  -4689,  -4452,  -3242,  -1617,   -302,
  -339,  -1949,  -3846,  -4448,  -3687,  -3003,  -3510,  -4938,  -6335,  -7114,  -7239,
 -6769,  -5656,  -4182,  -2937,  -2249,  -1904,  -1544,  -1072,   -366,    566,   1098,
   532,  -1043,  -2570,  -2751,  -1216,   1249,   3294,   3731,   2278,   -206,  -2371,
 -3195,  -2402,   -639,    959,   1533,    852,   -597,  -1887,  -2102,   -820,   1549,
  3846,   4837,   4143,   2398,    624,   -298,    -81,    793,   1552,   1605,    889,
   -26,   -445,   -138,    631,   1455,   1937,   1740,    745,   -735,  -2002,  -2435,
 -1966,  -1092,   -495,   -539,  -1109,  -1708,  -1762,  -1079,     21,    935,   1103,
   489,   -400,   -960,   -718,    427,   2035,   3310,   3636,   3115,   2310,   1690,
  1475,   1623,   1770,   1452,    548,   -577,  -1348,  -1424,   -922,   -242,    234,
   357,    244,     66,      9,    208,    450,    376,    -69,   -688,  -1261,  -1605,
 -1569,  -1160,   -563,    -59,    110,    -19,   -277,   -570,   -774,   -798,   -733,
  -693,   -715,   -705,   -525,   -217,     69,    263,    338,    298,    199,     98,
    28,     83,    344,    774,   1293,   1772,   2010,   1960,   1771,   1555,   1321,
  1015,    609,    246,     59,    -23,    -17,    164,    447,    666,    749,    681,
   472,    145,   -206,   -444,   -476,   -389,   -386,   -535,   -683,   -692,   -618,
  -550,   -459,   -335,   -222,   -121,    -20,     42,     19,   -109,   -284,   -432,
  -588,   -803,   -925,   -743,   -296,    158,    419,    486,    393,    140,   -150,
  -305,   -249,    -36,    209,    377,    353,    143,    -27,     72,    414,    789,
  1050,   1150,   1066,    861,    697,    716,    850,    914,    803,    592,    463,
   502,    652,    831,    933,    826,    398,   -277,   -895,  -1136,   -937,   -460,
    54,    374,    374,    147,    -70,   -119,    -51,     13,     48,     69,     99,
   171,    305,    515,    759,    919,    904,    753,    606,    589,    731,    945,
  1125,   1182,   1069,    824,    600,    546,    682,    890,   1042,   1070,    917,
   588,    228,     58,    181,    439,    561,    488,    369,    277,    186,    150,
   218,    306,    309,    228,    161,    210,    355,    474,    526,    538,    479,
   352,    292,    356,    429,    432,    369,    220,     33,     -9,    149,    329,
   374,    347,    353,    356,    312,    369,    588,    735,    650,    497,    459,
   465,    419,    401,    447,    408,    218,     75,    122,    298,    550,    802,
   914,    793,    468,    134,     48,    265,    538,    603,    401,    -11,   -499,
  -805,   -752,   -451,   -167,   -106,   -314,   -666,   -922,   -896,   -621,   -313,
  -169,   -249,   -522,   -850,   -981,   -774,   -374,    -45,     49,    -87,   -320,
  -443,   -281,    207,    831,   1269,   1309,    999,    586,    307,    276,    451,
   608,    527,    265,     86,    145,    403,    710,    869,    762,    456,    139,
   -46,    -62,     53,    189,    214,     73,   -149,   -301,   -285,   -111,    104,
   202,    101,    -96,   -195,   -102,    106,    263,    252,     62,   -223,   -442,
  -446,   -214,    166,    549,    811,    893,    791,    564,    360,    344,    506,
   676,    744,    719,    635,    511,    382,    289,    245,    207,    124,     35,
     6,     -1,    -50,    -83,    -24,    115,    247,    331,    411,    479,    436,
   235,    -15,   -148,   -130,    -84,    -98,   -135,   -147,   -160,   -220,   -304,
  -396,   -515,   -664,   -772,   -733,   -528,   -267,    -96,   -105,   -307,   -610,
  -870,   -939,   -708,   -263,    109,    199
};

static float nonlinearity[1000];

#define IMPULSE_SIZE (sizeof(impulse) / sizeof(impulse[0])) /* <= MAX_IMPULSE_SIZE */

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
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
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
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->gain, 35.0, 50.0, 0.1, 1, 0);
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
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->asymmetry, -5000.0, 5000.0, 0.1, 1, 0);
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
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
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
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK),
                     3, 0);
    o = gtk_adjustment_new(params->biasfactor, -25.0, 25.0, 0.1, 1, 0);
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
		     __GTKATTACHOPTIONS(GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Tube amplifier"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

/* waveshaper based on generic lookup table */
static float
F_tube(float in, float r_i)
{
    float pos;
    int idx;

    r_i /= 3000;
    pos = (in / r_i) * 50 + 500;
    //printf("%f ", pos);
    
    /* This safety catch should be made unnecessary. */
    if (pos <= 0)
        pos = 0;
    if (pos >= 998)
        pos = 998;
    idx = pos;
    pos -= idx;

    return (nonlinearity[idx] * (1.0-pos) + nonlinearity[idx+1] * pos) * r_i;
}

static void
tubeamp_filter(struct effect *p, struct data_block *db)
{
    int i, j, k, curr_channel = 0;
    DSP_SAMPLE *ptr1;
    struct tubeamp_params *params = p->params;
    float gain;

    /*
    for (j = 0; j < params->stages; j += 1)
        set_peq_biquad(sample_rate * UPSAMPLE_RATIO, 720, 500.0, params->middlefreq, &params->middlecut[j]);
    */
    gain = powf(10, params->gain / 20);
    
    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < db->len; i += 1) {
        float result;
        for (k = 0; k < UPSAMPLE_RATIO; k += 1) {
            /* IIR interpolation */
            params->in[curr_channel] = (db->data[i] + params->in[curr_channel] * (UPSAMPLE_RATIO-1)) / UPSAMPLE_RATIO;
            result = params->in[curr_channel] / MAX_SAMPLE;
            for (j = 0; j < params->stages; j += 1) {
                /* gain of the block */
                result *= gain;
                /* low-pass filter that mimicks input capacitance */
                result = do_biquad(result, &params->lowpass[j], curr_channel);
                /* add feedback bias current for "punch" simulation for waveshaper */
                result = F_tube(params->bias[j] - result, params->r_i[j]);
                /* feedback bias */
                params->bias[j] = do_biquad((params->asymmetry - params->biasfactor * result) * params->r_k[j] / params->r_p[j], &params->biaslowpass[j], curr_channel);
                /* high pass filter to remove bias from the current stage */
                result = do_biquad(result, &params->highpass[j], curr_channel);
                /* middlecut for user tone control, for the "metal crunch" sound */
                //result = do_biquad(result, &params->middlecut[j], curr_channel);
            }
            result = do_biquad(result, &params->decimation_filter1, curr_channel);
            result = do_biquad(result, &params->decimation_filter2, curr_channel);
        }
        ptr1 = params->buf[curr_channel] + params->bufidx[curr_channel];
        
        /* convolve the output. We put two buffers side-by-side to avoid & in loop. */
        ptr1[IMPULSE_SIZE] = ptr1[0] = result / 500 * (MAX_SAMPLE >> 13);
        db->data[i] = convolve(ptr1, impulse, IMPULSE_SIZE) / 32;
        
        params->bufidx[curr_channel] -= 1;
        if (params->bufidx[curr_channel] < 0)
            params->bufidx[curr_channel] += IMPULSE_SIZE;
        
        curr_channel = (curr_channel + 1) % db->channels;
    }
//    for (i = 0; i < params->stages; i += 1)
//        printf("%d. bias=%.1f\n", i, params->bias[i]);
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
    SAVE_INT("biasfactor", params->biasfactor);
    SAVE_INT("asymmetry", params->asymmetry);
    SAVE_DOUBLE("gain", params->gain);
    SAVE_DOUBLE("middlefreq", params->middlefreq);
}

static void
tubeamp_load(struct effect *p, LOAD_ARGS)
{
    struct tubeamp_params *params = p->params;
    LOAD_INT("stages", params->stages);
    LOAD_INT("biasfactor", params->biasfactor);
    LOAD_INT("asymmetry", params->asymmetry);
    LOAD_DOUBLE("gain", params->gain);
    LOAD_DOUBLE("middlefreq", params->middlefreq);
}

effect_t *
tubeamp_create()
{
    int i;
    float tmp;
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
    params->gain = 37.0;
    params->biasfactor = -5;
    params->asymmetry = -4000;

    /* configure the various stages */
    params->r_i[0] = 68e3;
    params->r_p[0] = 100000;
    params->r_k[0] = 2700;
    set_lpf_biquad(sample_rate * UPSAMPLE_RATIO, 22570, 2.0, &params->lowpass[0]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 86, &params->biaslowpass[0]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[0]);
    
    params->r_i[1] = 250e3;
    params->r_p[1] = 100000;
    params->r_k[1] = 1500;
    set_lpf_biquad(sample_rate * UPSAMPLE_RATIO, 6531, 2.0, &params->lowpass[1]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 132, &params->biaslowpass[1]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[1]);
    
    params->r_i[2] = 250e3;
    params->r_p[2] = 100000;
    params->r_k[2] = 820;
    set_lpf_biquad(sample_rate * UPSAMPLE_RATIO, 6531, 2.0, &params->lowpass[2]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 194, &params->biaslowpass[2]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[2]);
    
    params->r_i[3] = 250e3;
    params->r_p[3] = 100000;
    params->r_k[3] = 450;
    set_lpf_biquad(sample_rate * UPSAMPLE_RATIO, 6531, 2.0, &params->lowpass[3]);
    set_rc_lowpass_biquad(sample_rate * UPSAMPLE_RATIO, 250, &params->biaslowpass[3]);
    set_rc_highpass_biquad(sample_rate * UPSAMPLE_RATIO, 37, &params->highpass[3]);

    /* 10 kHz decimation IIR -- we should be 24 dB down by 20 kHz */
    set_chebyshev1_biquad(sample_rate * UPSAMPLE_RATIO, 10000, 5.0, TRUE, &params->decimation_filter1);
    set_chebyshev1_biquad(sample_rate * UPSAMPLE_RATIO, 11000, 5.0, TRUE, &params->decimation_filter2);

#define STEEPNESS   1e-3
#define SCALE       2e3
#define STEEPNESS2  1e-2
#define SCALE2      5e-1
    for (i = 0; i < 1000; i += 1) {
        int iter = 1000;
        /* Solve implicit equation
         * x - y = e^(-y / 10) / 10
         * for x values from -250 to 250. */
        float y = 0.0;
        float x = i - 500;
        while (--iter) {
            float value = x - y - SCALE * exp(STEEPNESS * y) + SCALE2 * exp(STEEPNESS2 * -y);
            float dvalue_y = -1 - (SCALE * STEEPNESS) * exp(STEEPNESS * y) - (SCALE2 * STEEPNESS2) * exp(STEEPNESS2 * -y);
            float dy = value / dvalue_y;
            y = (y + (y - dy)) / 2; /* average damp */

            if (fabs(value) < 1e-3)
                break;
        }
        if (iter == 0) {
            fprintf(stderr, "Failed to solve the nonlinearity equation for %f!\n", x);
        }
        nonlinearity[i] = y / 50;
        // printf("%d %f\n", i, nonlinearity[i]);
    }
    /* balance median to 0 */
    tmp = nonlinearity[500];
    for (i = 0; i < 1000; i += 1)
        nonlinearity[i] -= tmp;

    return p;
}
