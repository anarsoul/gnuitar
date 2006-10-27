/* Common definitions for MIDI. (c) 2006 Antti S. Lankila
 * Licensed under GPL.
 * $Id$
 *
 * $Log$
 * Revision 1.2  2006/10/27 21:54:46  alankila
 * - new source file: audio-midi.c. Do some data abstraction, prepare to
 *   support multiple midi continuous controls.
 *
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

void  midi_set_continuous_control(int type, float value);
float midi_get_continuous_control(int type);
void  midi_set_program(int program);
int   midi_get_program(void);

#endif
