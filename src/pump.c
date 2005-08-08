/*
 * GNUitar
 * Pump module - processeing sound
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
 *
 * $Log$
 * Revision 1.20  2005/08/08 18:34:45  alankila
 * - rename effects:
 *   * vibrato -> tremolo
 *   * tremolo -> vibrato
 *   * distort2 -> overdrive
 *
 * Revision 1.19  2005/08/07 12:53:42  alankila
 * - new tuner plugin / effect
 * - some gcc -Wall shutups
 * - added the entry required for gnuitar.vcproj as well but I can't test it
 * - changed pump.h to use enum instead of bunch-of-defines. Hopefully it's
 *   better that way.
 *
 * Revision 1.18  2005/04/24 19:11:22  fonin
 * Optimized for zero input (after the noise filter) to avoid the extra calcs
 *
 * Revision 1.17  2005/04/15 14:32:39  fonin
 * Few improvements with the effects save/load; fixed nasty bug with CR/LF translation when saving preset files on Win32
 *
 * Revision 1.16  2004/08/10 15:07:31  fonin
 * Support processing in float/int - type DSP_SAMPLE
 *
 * Revision 1.15  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 * Revision 1.14  2003/12/21 08:40:36  dexterus
 * biquad files amd eqbank working
 *
 * Revision 1.13  2003/04/12 20:02:10  fonin
 * New noise gate effect.
 *
 * Revision 1.12  2003/04/11 18:34:36  fonin
 * Added distort2 effect.
 *
 * Revision 1.11  2003/03/28 19:56:08  fonin
 * Sampling rate is 44100 by default.
 *
 * Revision 1.10  2003/03/09 21:12:41  fonin
 * New variables for new "change sampling params" feature.
 *
 * Revision 1.9  2003/02/05 21:10:10  fonin
 * Cleanup before release.
 *
 * Revision 1.8  2003/02/03 17:23:26  fonin
 * One more newline after the effects were loaded by pump_start().
 *
 * Revision 1.7  2003/02/03 11:39:25  fonin
 * Copyright year changed.
 *
 * Revision 1.6  2003/01/29 19:34:00  fonin
 * Win32 port.
 *
 * Revision 1.5  2001/06/02 14:05:59  fonin
 * Added GNU disclaimer.
 *
 * Revision 1.4  2001/03/25 17:42:32  fonin
 * open() can overwrite existing files from now, because program switches back to real user priorities after start.
 *
 * Revision 1.3  2001/03/25 12:10:06  fonin
 * Text messages begin from newline rather than end with it.
 *
 * Revision 1.2  2001/01/13 10:02:35  fonin
 * Fix: setuid root program shouldnt overwrite existing files.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:01  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <io.h>
#endif
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pump.h"
#include "gui.h"

#include "autowah.h"
#include "phasor.h"
#include "chorus.h"
#include "delay.h"
#include "echo.h"
#include "tremolo.h"
#include "vibrato.h"
#include "distort.h"
#include "distort2.h"
#include "sustain.h"
#include "reverb.h"
#include "tracker.h"
#include "noise.h"
#include "eqbank.h"
#include "tuner.h"

struct effect  *effects[MAX_EFFECTS];
int             n = 0;
unsigned short  audio_lock = 0;	/* 
				 * when nonzero pause pumping 
				 */
extern char     version[];
unsigned short  write_track = 0;	/* 
					 * when nonzero we should write
					 * sample to disk 
					 */
extern void     initSinLookUp(void);	/* 
					 * from chorus.c 
					 */

unsigned short  nchannels = 1;
unsigned int    sample_rate = 44100;
unsigned short  bits = 16;
unsigned int    buffer_size = MIN_BUFFER_SIZE * 2;
#ifdef _WIN32
unsigned int    nbuffers = MAX_BUFFERS;
#endif

int
pump_sample(DSP_SAMPLE *s, int size)
{
    struct data_block db;
    int             i;

    if (audio_lock)
	return 0;

    db.data = s;
    db.len = size;

    /* no input, no output :-) to avoid extra calc. Optimized for noise gate,
     * when all input is zero.
     * This is the heuristics - since there is no the standard function
     * in the ANSI C library that reliably compares the memory region
     * with the given byte, we compare just a few excerpts from an array.
     * If everything is zero, we have a large chances that all array is zero. */
    if(s[0]==0 && s[1]==0 && s[16]==0 && s[17]==0 &&
          s[24]==0 && s[25]==0 && s[32]==0 && s[33]==0 &&
	  s[buffer_size-1]==0) {
	/* nothing */
    }
    /*
     * Pumping
     */
    else for (i = 0; i < n; i++) {
	effects[i]->proc_filter(effects[i], &db);
    }

    /*
     * Writing track
     */
    if (write_track) {
	track_write(s, size);
    }

    return 0;
}

/* note that vibrato & tremolo effects are swapped */
struct effect_creator effect_list[] = {
    {"autowah", autowah_create},
    {"distort", distort_create},
    {"delay", delay_create},
    {"reverb", reverb_create},
    {"tremolo", vibrato_create},
    {"chorus", chorus_create},
    {"echo", echo_create},
    {"phasor", phasor_create},
    {"vibrato", tremolo_create},
    {"sustain", sustain_create},
    {"overdrive", distort2_create},
    {"noise gate", noise_create},
    {"eq bank", eqbank_create},
    {"tuner", tuner_create},
    {NULL, NULL}
};

void
pump_start(int argc, char **argv)
{
    int             i,
                    j;

    void            (*create_f[10]) (struct effect *);

    initSinLookUp();

    audio_lock = 1;
    j = 0;

    if (argc == 1) {
	int             k = 0;
	printf("\nPossible effects:");
	while (effect_list[k].str) {
	    printf("\n  %s", effect_list[k].str);
	    k++;
	}
    }
    for (i = 1; i < argc; i++) {
	int             k = 0;
	while (effect_list[k].str && strcmp(argv[i], effect_list[k].str)) {
	    k++;
	}
	if (effect_list[k].str) {
	    create_f[j++] = effect_list[k].create_f;
	    printf("\nadding %s", effect_list[k].str);
	    gtk_clist_append(GTK_CLIST(processor), &effect_list[k].str);
	} else {
	    printf("\n%s is not a known effect", argv[i]);
	}
    }
    create_f[j++] = NULL;

    /*
     * Cleaning effects[]
     */
    for (j = 0; j < MAX_EFFECTS; j++) {
	effects[j] = NULL;
    }

    while (n < MAX_EFFECTS && create_f[n]) {
	effects[n] = (struct effect *) calloc(1, sizeof(struct effect));
	create_f[n] (effects[n]);
	effects[n]->proc_init(effects[n]);
	n++;
    }
    audio_lock = 0;
}

void
pump_stop(void)
{
    int             i;

    audio_lock = 1;
    for (i = 0; i < n; i++) {
	effects[i]->proc_done(effects[i]);
    }
    n = 0;
}

void
passthru(struct effect *p, struct data_block *db)
{
}

/* for UNIX; O_BINARY exists only on Win32. We must open() files with this 
 * flag because otherwise it gets corrupted by the CR/LF translation */
#ifndef O_BINARY
#  define O_BINARY 0
#endif

void
save_pump(const char *fname)
{
    int             i;
    int             fd = 0;

    fprintf(stderr, "\nWriting preset (%s)...", fname);
    if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                                            S_IREAD | S_IWRITE)) < 0) {
	perror("Save failed");
	return;
    }

    /*
     * writing signature 
     */
    write(fd, version, 13);
    for (i = 0; i < n; i++) {
	if (effects[i]->proc_save != NULL) {
	    write(fd, &effects[i]->id, sizeof(effects[i]->id));
	    write(fd, &effects[i]->toggle, sizeof(effects[i]->toggle));
	    effects[i]->proc_save(effects[i], fd);
	}
    }
    close(fd);
    fprintf(stderr, "ok\n");
}

void
load_pump(const char *fname)
{
    int             fd = 0;
    unsigned short  effect_tag = MAX_EFFECTS+1;
    char            rc_version[32]="";

    if (!(fd = open(fname, O_RDONLY | O_BINARY, S_IREAD | S_IWRITE))) {
	perror("Load failed");
	return;
    }

    /*
     * reading signature and compare with our version 
     */
    read(fd, rc_version, 13);
    if (strncmp(version, rc_version, 13) != 0) {
	fprintf(stderr, "\nThis is not my rc file.");
	close(fd);
	return;
    }

    gtk_clist_clear(GTK_CLIST(processor));
    audio_lock = 1;
    pump_stop();

    n = 0;
    while (read(fd, &effect_tag, sizeof(unsigned short)) > 0) {
	if(effect_tag < 0 || effect_tag > EFFECT_AMOUNT) {
            fprintf(stderr,"\nInvalid effect %i, load finished",effect_tag);
            break;
        }

	fprintf(stderr, "\nloading %s", effect_list[effect_tag].str);

	effects[n] = (struct effect *) calloc(1, sizeof(struct effect));
	effect_list[effect_tag].create_f(effects[n]);
	read(fd, &effects[n]->toggle, sizeof(unsigned short));
	effects[n]->proc_load(effects[n], fd);
	effects[n]->proc_init(effects[n]);
	gtk_clist_append(GTK_CLIST(processor),
			 &effect_list[effect_tag].str);
	n++;
    }
    close(fd);
    fprintf(stderr, "\n");
    audio_lock = 0;
}
