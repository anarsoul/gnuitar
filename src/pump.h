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

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <gtk/gtk.h>
#include <sys/types.h>
#include <stdio.h>

#ifdef _WIN32
#  include <windows.h>      /* for DWORD */
#endif

#include "glib12-compat.h"

#ifdef FLOAT_DSP
typedef float	DSP_SAMPLE;
#else
typedef gint32	DSP_SAMPLE;
#endif

#ifdef __SSE__

/* for SSE we need aligned memory */
static inline void *
gnuitar_memalign(size_t num, size_t bytes) {
    void *mem = NULL;
    if (posix_memalign(&mem, 16, num * bytes)) {
        fprintf(stderr, "failed to allocate aligned memory.\n");
        exit(1);
    }
    assert(mem != NULL);

    memset(mem, 0, num * bytes);
    return mem;
}

#else
/* without SSE we just wrap calloc */
static inline void *
gnuitar_memalign(unsigned int num, size_t bytes)
{
    void *mem = calloc(num, bytes);
    if (mem == NULL) {
        fprintf(stderr, "failed to allocate aligned memory.\n");
        exit(1);
    }
    return mem;
}
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
#define MAX_SAMPLE_RATE 48000
#define MAX_EFFECTS 50

extern void gnuitar_printf(char *, ...);

struct audio_driver_channels {
    unsigned int in, out;
};

typedef struct {
    char    *str;
    int     enabled;
    struct audio_driver_channels *channels;
    unsigned int *bits;
    
    int     (*init)(void);
    void    (*finish)(void);
#ifndef _WIN32
    void *  (*thread)(void *);
#else
    DWORD (WINAPI *audio_proc)(void *);
#endif
} audio_driver_t;

struct data_block {
    DSP_SAMPLE     *data;
    DSP_SAMPLE     *data_swap;
    unsigned int    len;
    unsigned int    channels;
};
typedef struct data_block data_block_t;

struct effect {
    void           *params;
    void            (*proc_init) (struct effect *);
    void            (*proc_done) (struct effect *);
    void            (*proc_filter) (struct effect *, data_block_t *);
    void            (*proc_save) (struct effect *, GKeyFile *, gchar *);
    void            (*proc_load) (struct effect *, GKeyFile *, gchar *, GError **error);
    short           toggle;
    GtkWidget      *control;
};
typedef struct effect effect_t;

#define CLIP_SAMPLE(sm) \
    if (sm > MAX_SAMPLE) \
        sm = MAX_SAMPLE; \
    if (sm < -MAX_SAMPLE) \
        sm = -MAX_SAMPLE;

/* these macros are used to save my sanity */
#define SAVE_ARGS \
    GKeyFile *preset, gchar *group
#define LOAD_ARGS \
    GKeyFile *preset, gchar *group, GError **error
#define SAVE_DOUBLE(name, param) \
    g_key_file_set_double(preset, group, name, param);
#define LOAD_DOUBLE(name, param) \
    param = g_key_file_get_double(preset, group, name, error); \
    if (*error != NULL) { \
	gnuitar_printf("warning: couldn't read '%s', '%s'\n", group, name); \
	*error = NULL; \
    }
#define SAVE_INT(name, param) \
    g_key_file_set_integer(preset, group, name, param);
#define LOAD_INT(name, param) \
    param = g_key_file_get_integer(preset, group, name, error); \
    if (*error != NULL) { \
	gnuitar_printf("warning: couldn't read '%s', '%s'\n", group, name); \
	*error = NULL; \
    }

struct effect_creator {
    char           *str;
    effect_t *    (*create_f)();
};

#define SIN_LOOKUP_SIZE         65536
#define SIN_LOOKUP_AMPLITUDE    0x7ffffffL
extern int sin_lookup_table[SIN_LOOKUP_SIZE+1];

/* [0 .. 1[ -> sin(0 .. 2pi) */
#define sin_lookup(scale) ((float) sin_lookup_table[(int) ((scale) * SIN_LOOKUP_SIZE)] / SIN_LOOKUP_AMPLITUDE)

extern volatile unsigned short write_track;

extern char alsadevice_str[64];
extern unsigned short n_input_channels;
extern unsigned short n_output_channels;
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

extern int      pump_sample(data_block_t *db);
extern void     pump_start(int argc, char **argv);
extern void     pump_stop(void);
extern void     save_pump(const char *fname);
extern void     load_pump(const char *fname);
extern void     load_settings(void);
extern void     save_settings(void);
extern void     triangular_dither(data_block_t *db);

#endif

