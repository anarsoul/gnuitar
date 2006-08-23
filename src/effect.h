/*
 * GNUitar
 * Common definitions for all effects.
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 *           (c) 2005,2006 Antti S. Lankila          <alankila@bel.fi>
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

#ifndef _EFFECT_H_
#define _EFFECT_H_ 1

#define MAX_EFFECTS 50

#ifdef __MINGW32__
#include <malloc.h>
#endif
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>
#include "glib12-compat.h"

#include "audio-driver.h"
#include "utils.h"

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

#ifdef __GNUC__
#define unlikely(x) __builtin_expect((x), 0)
#else
#warning "no unlikely"
#define unlikely(x) !!(x)
#endif

#define CLIP_SAMPLE(sm) \
    if (unlikely(sm > (typeof(sm)) MAX_SAMPLE)) \
        sm = (typeof(sm)) MAX_SAMPLE; \
    if (unlikely(sm < (typeof(sm)) -MAX_SAMPLE)) \
        sm = (typeof(sm)) -MAX_SAMPLE;

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

/* tabularised sin() */
#define SIN_LOOKUP_SIZE         65536
extern float sin_lookup_table[SIN_LOOKUP_SIZE+1];

static inline float
sin_lookup(float pos) {
    return sin_lookup_table[(int) (pos * (float) SIN_LOOKUP_SIZE)];
}

static inline DSP_SAMPLE
cos_lookup(float pos) {
    if (pos >= 0.75f)
        return sin_lookup(pos - 0.75f);
    else
        return sin_lookup(pos + 0.25f);
}

/* FreeBSD's malloc is aligned by 16 */
#if defined(__SSE__) && !defined(__FreeBSD__)

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

// global initialisers
void effect_start(void);
void effect_stop(void);

// effect list operations
void effect_list_print_all(void);
void effect_list_add_to_clist(GtkWidget *w);
int effect_list_find_by_name(const char *name);

// effect operations
effect_t *effect_create(const int idx);
effect_t *effect_create_without_init(const int idx);
void effect_iterate(void (*func)(effect_t *effect, int idx, void *data), void *data);
void effect_clear(void);
int effect_insert(effect_t *effect, const int idx);
int effect_move(const int start, const int end);
int effect_find(const effect_t *effect);
void effect_destroy(effect_t *effect);
effect_t *effect_delete(const int idx);

#endif
