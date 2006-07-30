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

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <gtk/gtk.h>
#include <sys/types.h>
#include <stdio.h>

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
#ifndef __MINGW32__
    if (posix_memalign(&mem, 16, num * bytes)) {
        fprintf(stderr, "failed to allocate aligned memory.\n");
        exit(1);
    }
#else
    mem = __mingw_aligned_malloc(num * bytes, 16);
#endif
    assert(mem != NULL);

    memset(mem, 0, num * bytes);
    return mem;
}

static inline void
gnuitar_free(void *memory) {
#ifndef __MINGW32__
    free(memory);
#else
    __mingw_aligned_free(memory);
#endif
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

static inline void
gnuitar_free(void *memory) {
    free(memory);
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

void gnuitar_printf(char *, ...);

struct audio_driver_channels {
    unsigned int in, out;
};

typedef struct {
    char    *str;
    int     enabled;
    struct audio_driver_channels *channels;
    
    int     (*init)(void);
    void    (*finish)(void);
} audio_driver_t;

typedef struct {
    float pitchbend;
    int key;
    short keyevent;
} midictrl_t;

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

/* tabularised sin() */
#define SIN_LOOKUP_SIZE         65536
extern float sin_lookup_table[SIN_LOOKUP_SIZE+1];

static inline float
sin_lookup(float pos) {
    return sin_lookup_table[(int) (pos * SIN_LOOKUP_SIZE)];
}

static inline DSP_SAMPLE
cos_lookup(float pos) {
    if (pos >= 0.75)
        return sin_lookup(pos - 0.75);
    else
        return sin_lookup(pos + 0.25);
}

extern volatile unsigned short write_track;
extern volatile midictrl_t midictrl;

extern char alsadevice_str[64];
extern unsigned short n_input_channels;
extern unsigned short n_output_channels;
extern unsigned int sample_rate;
extern unsigned int buffer_size;

#ifndef _WIN32
extern unsigned int fragments;
#else
extern unsigned int nbuffers;
extern unsigned int overrun_threshold;
#endif

extern int      effects_n;
extern struct effect *effects[MAX_EFFECTS];
extern struct effect_creator effect_list[];

void     pump_sample(data_block_t *db);
void     pump_start(int argc, char **argv);
void     pump_stop(void);
void     save_pump(const char *fname);
void     load_pump(const char *fname);
void     load_settings(void);
void     save_settings(void);
void     triangular_dither(data_block_t *db);
void     load_initial_state(void);
char    *discover_preset_path(void);

#endif

