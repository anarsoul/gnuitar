/*
 * GNUitar
 * Pump module - processing sound
 * Copyright (C) 2000,2001 Max Rudensky         <fonin@ziet.zhitomir.ua>
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

#ifndef _WIN32
typedef gint16 SAMPLE;
#else
typedef short SAMPLE;
#define NBUFFERS 512		/* number of input/output sound buffers */
#endif

#define NCHANNELS 1
#define BUFFER_SIZE 128*2
#define SAMPLE_RATE 44100
#define MAX_EFFECTS 50
#define EFFECT_AMOUNT 9

/*
 * Indices in effect_creator[] array
 */
#define AUTOWAH 0
#define DISTORT 1
#define DELAY	2
#define REVERB	3
#define VIBRATO	4
#define CHORUS	5
#define ECHO	6
#define PHASOR	7
#define TREMOLO	8
#define SUSTAIN	9

struct data_block {
    int            *data;
    int             len;
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

extern unsigned short audio_lock;
extern int      n;
extern struct effect *effects[MAX_EFFECTS];
extern struct effect_creator effect_list[];

extern int      pump_sample(int *s, int size);
extern void     pump_start(int argc, char **argv);
extern void     pump_stop(void);
extern void     save_pump(char *fname);
extern void     load_pump(char *fname);
extern void     passthru(struct effect *p, struct data_block *db);

#endif
