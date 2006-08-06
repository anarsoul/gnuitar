/* Common definitions for MIDI. (c) 2006 Antti S. Lankila
 * Licensed under GPL.
 * $Id$
 *
 * $Log$
 * Revision 1.1  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 */

#ifndef _AUDIO_MIDI_H_
#define _AUDIO_MIDI_H_ 1

typedef struct {
    float pitchbend;
    int key;
    short keyevent;
} midictrl_t;

extern volatile midictrl_t midictrl;

#endif
