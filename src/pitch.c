/*
 * GNUitar
 * Pitch shifter effect
 * Copyright (c) 2005 Antti S. Lankila  <alankila@bel.fi>
 *
 * See COPYING about details for license.
 *
 * New, better algorithm replacing the earlier overlapping cosine-windowed
 * rather simplistic approach.
 *
 * This should be the stock WSOLA algorithm. Waveform similarity is estimated
 * through cross-correlation with x[n] * y[n] function. SSE acceleration is
 * provided.
 *
 * Overlap-add is implemented with tabularised Hann window. The overlap
 * factor is 2.
 * 
 * $Id$
 */

/* nag about correct compilation options. */
#if !defined(FLOAT_DSP) && !defined(__SSE__)
    #warning "warning: pitch.c: floating point (-DFLOAT_DSP) and SSE (-march=something) recommended for performance."
#endif

#include <assert.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#include "pitch.h"
#include "backbuf.h"
#include "biquad.h"
#include "gui.h"

/* CPU time is spent in correlation scan function.
 * It has cost that follows loop_length * (memory_length - loop_length * 1.5)
 * This function has maximum is memory length is 3x loop length.
 *
 * The guitar can produce combined tones with very low frequency oscillating pattern.
 * For instance, just striking E and A strings simultaneously causes 27 Hz modulation.
 * I designed this effect to handle that case, which is fairly common in powerful
 * chords. It seems that LOOP_LENGTH needs to be at least half of repeating
 * pattern length, and MEMORY_LENGTH must naturally contain at least one copy of the
 * data looked for. Some part of the memory is sacrificed (precisely LOOP_LENGTH/3*2)
 * which needs to be accounted for.
 * */
#define LOOP_LENGTH 1000
#define MEMORY_LENGTH (1800 + LOOP_LENGTH * 3 / 2) /* 1800 ~ 26.7 Hz at 48 kHz */
#define MAX_RESAMPLING_FACTOR 2.0
#define MAX_OUTPUT_BUFFER (MAX_RESAMPLING_FACTOR * (MEMORY_LENGTH + LOOP_LENGTH))

static float *window_memory = NULL;

static void
update_pitch_halfnote(GtkAdjustment *adj, struct pitch_params *params)
{
    params->output_pos = 0;
    params->halfnote = adj->value;
}

static void
update_pitch_finetune(GtkAdjustment *adj, struct pitch_params *params)
{
    params->output_pos = 0;
    params->finetune = adj->value;
}

static void
update_pitch_drywet(GtkAdjustment *adj, struct pitch_params *params)
{
    params->drywet = adj->value;
}

static void
pitch_init(struct effect *p)
{
    struct pitch_params *params = p->params;

    GtkWidget      *halfnote_label;
    GtkWidget      *halfnote;
    GtkObject      *adj_halfnote;

    GtkWidget      *finetune_label;
    GtkWidget      *finetune;
    GtkObject      *adj_finetune;

    GtkWidget      *drywet_label;
    GtkWidget      *drywet;
    GtkObject      *adj_drywet;

    GtkWidget      *parmTable, *button;
    
    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);

    parmTable = gtk_table_new(3, 3, FALSE);

    adj_halfnote = gtk_adjustment_new(params->halfnote, -24.0,
                               12.0, 1.0, 1.0, 0.0);
    halfnote_label = gtk_label_new("Pitch\n(halfnotes)");
    gtk_table_attach(GTK_TABLE(parmTable), halfnote_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_signal_connect(GTK_OBJECT(adj_halfnote), "value_changed",
		       GTK_SIGNAL_FUNC(update_pitch_halfnote), params);

    halfnote = gtk_vscale_new(GTK_ADJUSTMENT(adj_halfnote));
    gtk_scale_set_digits(GTK_SCALE(halfnote), 0);
    gtk_widget_set_size_request(GTK_WIDGET(halfnote),0,100);
    gtk_table_attach(GTK_TABLE(parmTable), halfnote, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_finetune = gtk_adjustment_new(params->finetune,
				     -0.5, 0.5, 0.1, 0.1, 0.0);
    finetune_label = gtk_label_new("Finetune\n(halfnotes)");
    gtk_table_attach(GTK_TABLE(parmTable), finetune_label, 1, 2, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_finetune), "value_changed",
		       GTK_SIGNAL_FUNC(update_pitch_finetune), params);

    finetune = gtk_vscale_new(GTK_ADJUSTMENT(adj_finetune));
    gtk_scale_set_digits(GTK_SCALE(finetune), 2);
    gtk_table_attach(GTK_TABLE(parmTable), finetune, 1, 2, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    adj_drywet = gtk_adjustment_new(params->drywet,
				     0, 100.0, 1.0, 1.0, 0.0);
    drywet_label = gtk_label_new("Dry / Wet\n(%)");
    gtk_table_attach(GTK_TABLE(parmTable), drywet_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_drywet), "value_changed",
		       GTK_SIGNAL_FUNC(update_pitch_drywet), params);

    drywet = gtk_vscale_new(GTK_ADJUSTMENT(adj_drywet));
    gtk_table_attach(GTK_TABLE(parmTable), drywet, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL | GTK_EXPAND |
					GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    if (p->toggle == 1)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 0, 1, 2, 3,
		     __GTKATTACHOPTIONS(GTK_EXPAND |
					GTK_SHRINK),
		     __GTKATTACHOPTIONS(GTK_FILL |
					GTK_SHRINK), 0, 0);

    gtk_window_set_title(GTK_WINDOW(p->control), (gchar *) ("Pitch shift"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);
}

/* might be defined by compiler or some included thingy, might be not... */
#ifndef min
static inline int
min(a, b) {
    return a < b ? a : b;
}
#endif

#ifndef max
static inline int
max(a, b) {
    return a > b ? a : b;
}
#endif

/* this function scans only about 1/4th of the full range of possible
 * offsets, allowing us to work realtime on somewhat worse hardware.
 * GNUitar is however becoming very SSE dependant. */
static int
estimate_best_correlation(DSP_SAMPLE *data, const int frames, const int alignoff, DSP_SAMPLE *ref, const int looplen)
{
    int_fast16_t i = alignoff, best = 0, apprx;
    float goodness = 0;
    /* i is chosen so that data + i is aligned by 16, which allows the
     * use of optimum assembly instructions in the faster scan. */

    /* estimate the position of a local maximum */
    while (i < frames - looplen) {
        /* compute correlation term. The aim is to maximise this value. */
        float goodness_try = convolve_aligned(ref, data + i, looplen);
        /* HACK: skip forward faster if currently anticorrelated. This is purely
         * for performance: we want as long convolution buffers as possible without
         * paying so much for them. Since the max guitar frequency is roughly
         * 1.2 kHz we can assume that the output oscillates at roughly 40 sample long
         * patterns. If we are negative, it is almost certain that about 10 samples
         * one quarter of that can be skipped forward without bypassing any peak. */
        if (goodness_try >= goodness) {
            goodness = goodness_try;
            best = i;
        }
        /* increment by 4 keeps data+i aligned by 16 */
        i += 4;
    }
    apprx = best;

    /* now look around the estimated maximum for the best match */
    for (i = max(best - 3, 0); i < min(best + 4, frames - looplen); i += 1) {
        /* don't recompute the convolution we already know */
        if (i == apprx)
            continue;
        float goodness_try = convolve(ref, data + i, looplen);
        if (goodness_try >= goodness) {
            goodness = goodness_try;
            best = i;
        }
    }

    return best;
}

#ifdef __SSE__
static void
copy_to_output_buffer(DSP_SAMPLE *in, DSP_SAMPLE *out, float *wp, const int length)
{
    const __m128 ones = { 1.f, 1.f, 1.f, 1.f };
    __m128 * __restrict__ out4 = (__m128 * __restrict__) out;
    const __m128 * __restrict__ wp4 = (const __m128 * __restrict__) wp;
    int_fast16_t i;

    /* sum the first half with the tail of previous buffer, but overwrite
     * with the second half because the data on that side is old. */
    for (i = 0; i < length / 2 / 4; i += 1) {
        __m128 w = wp4[i];
        /* unfortunately we must take the performance hit of unaligned load */
        out4[i] = w * _mm_loadu_ps(in + i * 4) + (ones - w) * out4[i + length / 2 / 4];
    }
    memcpy(out + length / 2, in + length / 2, sizeof(float) * length / 2);
    /* output buffer can now be read from 0 to length / 2 */
}
#else
static void
copy_to_output_buffer(DSP_SAMPLE *in, DSP_SAMPLE *out, float *wp, const int length)
{
    int_fast16_t i;

    /* sum the first half with the tail of previous buffer, but overwrite
     * with the second half because the data on that side is old. */
    for (i = 0; i < length / 2; i += 1) {
        float w = wp[i];
        out[i] = w * in[i] + (1.f - w) * out[i + length/2];
    }
    memcpy(out + length / 2, in + length / 2, sizeof(float) * length / 2);
    /* output buffer can now be read from 0 to length / 2 */
}
#endif

static void
resample_to_output(Backbuf_t *history, const int deststart, const int destend, DSP_SAMPLE *input, const int sourcelength)
{
    int_fast16_t i;
    const int_fast16_t destlength = destend - deststart;
    float factor = (float) sourcelength / destlength, pos = 0;

    /* very primitive resampler but it should be good enough for now */
    for (i = 0; i < destlength; i += 1) {
        int idx;
        pos += factor;
        idx = pos;
        float mid = pos - idx;
        history->add(history, (1.f - mid) * input[idx] + mid * input[idx + 1]);
    }
}

static void
pitch_filter(effect_t *p, data_block_t *db)
{
    struct pitch_params *params = p->params;
    DSP_SAMPLE *s = db->data;
    int count = db->len / db->channels;
    int i;
    float depth, Wet, Dry, output_inc;

    depth = powf(2.f, (params->halfnote + params->finetune) / 12.f);
    if (depth < 0.25f)
       depth = 0.25f;
    if (depth > (float) MAX_RESAMPLING_FACTOR)
       depth = (float) MAX_RESAMPLING_FACTOR;

    Wet = params->drywet / 100.0f;
    Dry = 1.f - Wet;

    output_inc = LOOP_LENGTH / 2 / depth; 
    while (count --) {
        /* uninterleave channel data */
        for (i = 0; i < db->channels; i += 1) {
            params->channel_memory[i][params->memory_index] =
            params->channel_memory[i][params->memory_index + MEMORY_LENGTH] =
                *s++;
        }
        params->memory_index += 1;
        if (params->memory_index == MEMORY_LENGTH)
            params->memory_index = 0;
       
        /* test whether we have advanced enough to do chunk of output */ 
        params->output_buffer_trigger -= 1;
        if (params->output_buffer_trigger >= 0.f)
            continue;

        /* the factor 2.0 comes from doing 2x overlap with Hann window. */
        params->output_buffer_trigger += LOOP_LENGTH / depth / 2.0f;

        /* at this point, all data from memory-index forward in each buffer
         * is the oldest data recalled. There is exactly MEMORY_LENGTH
         * frames of valid data forward from that point as continuous chunk. */

        /* the aim now is to produce LOOP_LENGTH of data in the output buffer
         * and sum the first half using some window with the old data,
         * and overwrite the other half. After this, data from
         * position 0 to LOOP_LEN/2 can be resampled to output. */

        /* We start by searching for data in the input that looks like the
         * last chunk we put into output. Output is split into two parts:
         * the data mixed with the "tail" of the previous buffer and
         * pristine input data. At each iteration, we use only half of the
         * output buffer for actual output, and copy the latter half as the
         * first half for next iteration. */
        for (i = 0; i < db->channels; i += 1) {
            int bestpos = estimate_best_correlation(
                params->channel_memory[i] + params->memory_index,
                MEMORY_LENGTH - LOOP_LENGTH / 2, /* look at next stmt */
                (4 - (params->memory_index % 4)) % 4, /* alignment 0..3 */
                params->output_memory[i],
                LOOP_LENGTH);
            
            /* copy the data after the best match into the output buffer. The
             * 0..length/2 part is windowed with the previous part, and the
             * length/2 .. length part is new. So by advanging forwards by
             * LOOP_LENGTH / 2 we start mixing over the part that the previous
             * memcpy() left us (see end of this loop). */
            bestpos += LOOP_LENGTH / 2;

            copy_to_output_buffer(params->channel_memory[i] + params->memory_index + bestpos, params->output_memory[i], window_memory, LOOP_LENGTH);

            /* write to output from memory -- this algorithm is rubbish.
             * I should probably produce full length resampled stream and
             * then do the resampling in one step. */
            resample_to_output(params->history[i], params->output_pos, params->output_pos + output_inc, params->output_memory[i], LOOP_LENGTH / 2);
        }
        
        params->output_pos += output_inc;
    }
    /* now output_memory holds the resampled output. The trouble is, there
     * might not be enough bytes to fill the final output buffer. At least
     * one LOOP_LENGTH / RESAMPLE_RATIO bytes should be kept extra... */

    s = db->data; 
    count = db->len / db->channels;
    while (count --) {
        /* don't consume until we have enough for full fill. This should
         * not trigger after the first few startup frames. It might be
         * best to do this initially as a small hack during initializing. */
        if (params->output_pos < count) {
            for (i = 0; i < db->channels; i += 1)
                *s++ = 0;
            continue;
        }
        /* consume history now. */
        for (i = 0; i < db->channels; i += 1) {
            *s = Dry * *s + Wet * params->history[i]->get(params->history[i], (int) params->output_pos);
            s++;
        }
        params->output_pos -= 1.f;
    }
}

static void
pitch_done(struct effect *p)
{
    struct pitch_params *params = p->params;
    int i;
    
    for (i = 0; i < MAX_CHANNELS; i += 1) {
        gnuitar_free(params->channel_memory[i]);
        gnuitar_free(params->output_memory[i]);
        del_Backbuf(params->history[i]);
    }
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
}

static void
pitch_save(effect_t *p, SAVE_ARGS)
{
    struct pitch_params *params = p->params;

    SAVE_INT("halfnote", params->halfnote);
    SAVE_DOUBLE("finetune", params->finetune);
    SAVE_DOUBLE("drywet", params->drywet);
}

static void
pitch_load(effect_t *p, LOAD_ARGS)
{
    struct pitch_params *params = p->params;
    
    LOAD_INT("halfnote", params->halfnote);
    LOAD_DOUBLE("finetune", params->finetune);
    LOAD_DOUBLE("drywet", params->drywet);
}

effect_t *
pitch_create()
{
    effect_t           *p;
    struct pitch_params *params;
    int                 i;
    
    p = calloc(1, sizeof(effect_t));
    p->params = calloc(1, sizeof(struct pitch_params));
    p->proc_init = pitch_init;
    p->proc_filter = pitch_filter;
    p->toggle = 0;
    p->proc_done = pitch_done;
    p->proc_save = pitch_save;
    p->proc_load = pitch_load;

    params = p->params;
    
    for (i = 0; i < MAX_CHANNELS; i += 1) {
        params->channel_memory[i] = gnuitar_memalign(MEMORY_LENGTH * 2, sizeof(float));
        params->output_memory[i] = gnuitar_memalign(LOOP_LENGTH, sizeof(float));
        params->history[i] = new_Backbuf(MAX_OUTPUT_BUFFER);
    }
    if (window_memory == NULL) {
        /* I will never free this memory -- some effects should have
         * a global init and destroy funcs and I don't have them. */
        window_memory = gnuitar_memalign(LOOP_LENGTH, sizeof(float));
        for (i = 0; i < LOOP_LENGTH; i += 1)
            window_memory[i] = 0.5f-0.5f * cos_lookup((float) i / LOOP_LENGTH);
    }
    
    params->drywet = 100;
    return p;
}
