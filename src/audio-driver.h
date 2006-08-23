/* Common definitions for audio drivers. (c) 2006 Antti S. Lankila
 * Licensed under GPL.
 * $Id$
 *
 * $Log$
 * Revision 1.7  2006/08/23 08:11:23  alankila
 * - add unlikely to hopefully speed up if() test
 *
 * Revision 1.6  2006/08/22 06:40:05  alankila
 * - inform gcc on the rare case where it may be helpful that
 *   data and data_swap pointers do not alias.
 *
 * Revision 1.5  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.4  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.3  2006/08/07 12:55:30  alankila
 * - construct audio-driver.c to hold globals and provide some utility
 *   functions to its users. This slashes interdependencies somewhat.
 *
 * Revision 1.2  2006/08/06 20:45:52  alankila
 * - use stdint type rather than glib type
 *
 * Revision 1.1  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 */

#ifndef _AUDIO_DRIVER_H_
#define _AUDIO_DRIVER_H_ 1

#include <stdint.h>

/* get rid of __restrict__ type qualifier for MS Visual C */
#ifdef _MSC_VER
#define __restrict__
#endif

#define MAX_SAMPLE (32767 << 8)

#ifdef FLOAT_DSP
typedef float           DSP_SAMPLE;
#else
typedef int32_t         DSP_SAMPLE;
#endif

typedef struct {
    DSP_SAMPLE * __restrict__ data;
    DSP_SAMPLE * __restrict__ data_swap;
    int_fast32_t    len;
    int_fast8_t     channels;
} data_block_t;

struct audio_driver_channels {
    unsigned int in, out;
};

typedef struct {
    const char *str;
    int     enabled;
    const struct audio_driver_channels *channels;
    
    int     (*init)(void);
    void    (*finish)(void);
} audio_driver_t;

extern audio_driver_t *audio_driver;

typedef int16_t  SAMPLE16;
typedef int32_t  SAMPLE32;

#ifdef _WIN32
    #define MAX_BUFFERS	1024	/* number of input/output sound buffers */
#endif

#define MIN_BUFFER_SIZE 128
#define MAX_BUFFER_SIZE 65536
#define MAX_CHANNELS 4
#define MAX_SAMPLE_RATE 48000

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

#ifndef _WIN32
extern DSP_SAMPLE       procbuf[MAX_BUFFER_SIZE * MAX_CHANNELS];
extern DSP_SAMPLE       procbuf2[MAX_BUFFER_SIZE * MAX_CHANNELS];
#else
/* sadly, Windows and Linux have a different idea what the size of the buffer is.
 * Linux world talks about size in frames because that is most convenient for ALSA
 * and JACK (but less so for OSS). */
extern DSP_SAMPLE       procbuf[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
extern DSP_SAMPLE       procbuf2[MAX_BUFFER_SIZE / sizeof(SAMPLE16)];
#endif

void guess_audio_driver(void);
void set_audio_driver_from_str(const char const *str);
void triangular_dither(data_block_t *db, SAMPLE16 *target);

#endif
