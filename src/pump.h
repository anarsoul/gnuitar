/*
 * $Id$
 */

#ifndef PUMP_H
#define PUMP_H 1

#include <gtk/gtk.h>

#define BUFFER_SIZE 256
#define SAMPLE_RATE 48000
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
