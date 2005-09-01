/*
 * GNUitar
 * Pump module - processing sound
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

#ifndef PUMP_H
#define PUMP_H 1

#include <gtk/gtk.h>
#include <sys/types.h>
#include <glib.h>

#ifdef FLOAT_DSP
typedef double	DSP_SAMPLE;
#else
typedef gint32	DSP_SAMPLE;
#endif

typedef gint16  SAMPLE16;
typedef gint32  SAMPLE32;

#ifdef _WIN32
#define MAX_BUFFERS	1024	/* number of input/output sound buffers */
#endif
#define MAX_SAMPLE (32767 << 8)

#define MIN_BUFFER_SIZE 128
#define MAX_BUFFER_SIZE 65536
#define MAX_CHANNELS 4
#define MAX_SAMPLE_RATE 48000	/* 48000 produces more noise */
#define MAX_EFFECTS 50

typedef enum {
    AUTOWAH = 0,
    DISTORT,
    DELAY,
    REVERB,
    VIBRATO,
    CHORUS,
    ECHO,
    PHASOR,
    TREMOLO,
    SUSTAIN,
    DISTORT2,
    NOISE,
    EQBANK,
    TUNER,
    EFFECT_AMOUNT
} GnuitarEffects;

struct data_block {
    DSP_SAMPLE     *data;
    unsigned int    len;
    unsigned int    channels;
};

struct effect {
    void           *params;
    void            (*proc_init) (struct effect *);
    void            (*proc_done) (struct effect *);
    void            (*proc_filter) (struct effect *, struct data_block *);
    void            (*proc_save) (struct effect *, int fd);
    void            (*proc_load) (struct effect *, int fd);
    short           toggle;
    unsigned short  id;
    GtkWidget      *control;
};

struct effect_creator {
    char           *str;
    void            (*create_f) (struct effect *);
};

#define SIN_LOOKUP_SIZE         36000
#define SIN_LOOKUP_AMPLITUDE    32768
extern int sin_lookup_table[SIN_LOOKUP_SIZE];

/* [0 .. 1[ -> sin(0 .. 2pi) */
#define sin_lookup(scale) ((double) sin_lookup_table[(int) ((scale) * SIN_LOOKUP_SIZE)] / SIN_LOOKUP_AMPLITUDE)

extern volatile unsigned short write_track;

extern unsigned short nchannels;
extern unsigned int sample_rate;
extern unsigned short bits;
extern unsigned int buffer_size;

#ifndef _WIN32
extern unsigned int fragments;
#else
extern unsigned int nbuffers;
#endif

extern int      n;
extern struct effect *effects[MAX_EFFECTS];
extern struct effect_creator effect_list[];

extern int      pump_sample(struct data_block *db);
extern void     pump_start(int argc, char **argv);
extern void     pump_stop(void);
extern void     save_pump(const char *fname);
extern void     load_pump(const char *fname);
extern void     passthru(struct effect *p, struct data_block *db);
extern void     load_settings(void);
extern void     save_settings(void);

#endif
