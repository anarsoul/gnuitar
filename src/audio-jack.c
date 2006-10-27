/*
 * GNUitar
 * Jack driver
 * Copyright (C) 2006 Vasily Khoruzhick  <anarsoul@gmail.com>
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
 * Revision 1.24  2006/10/27 22:02:39  alankila
 * - remove support for pitch bend for now
 *
 * Revision 1.23  2006/10/27 21:54:46  alankila
 * - new source file: audio-midi.c. Do some data abstraction, prepare to
 *   support multiple midi continuous controls.
 *
 * Revision 1.22  2006/10/27 15:54:43  alankila
 * - FC-200 support now tested & works
 *
 * Revision 1.21  2006/08/11 16:18:39  alankila
 * - ensure that the physical in/out ports are of correct type
 *
 * Revision 1.20  2006/08/10 16:18:36  alankila
 * - improve const correctness and make gnuitar compile cleanly under
 *   increasingly pedantic warning models.
 *
 * Revision 1.19  2006/08/08 21:05:31  alankila
 * - optimize gnuitar: this breaks dsound, I'll fix it later
 *
 * Revision 1.18  2006/08/07 12:55:30  alankila
 * - construct audio-driver.c to hold globals and provide some utility
 *   functions to its users. This slashes interdependencies somewhat.
 *
 * Revision 1.17  2006/08/06 20:14:54  alankila
 * - split pump.h into several domain-specific headers to reduce file
 *   interdependencies (everyone included pump.h). New files are:
 *   - effect.h for effect definitions
 *   - audio-driver.h for work relating to audio drivers
 *   - audio-midi.h for MIDI interaction.
 *
 * Revision 1.16  2006/08/03 05:20:02  alankila
 * - don't crash on missing midi device
 * - alsa: keep on going if fragment number can't be set
 *
 * Revision 1.15  2006/07/31 22:45:27  alankila
 * - don't quit on failure to connect to all capture/playback ports.
 *   We will assume user using JACK knows what he is doing.
 *
 * Revision 1.14  2006/07/29 15:34:50  alankila
 * - compatibility with JACK versions that don't have jack_options_t.
 *
 * Revision 1.13  2006/07/28 19:24:35  alankila
 * - avoid destroying sequencer if for some reason init is called twice. The
 *   circumstances where this might occur smell a bit dubious though.
 *
 * Revision 1.12  2006/07/28 19:08:40  alankila
 * - add midi event listeners into JACK and ALSA
 * - make gui listen to midi events and switch bank
 * - fix a bug involving "add pressed"
 * - change documentation of "Switch preset" to "Cycle presets" which is what
 *   it does now.
 *
 * Revision 1.11  2006/07/26 23:09:09  alankila
 * - DirectSound may be buggy; MMSystem at least worked in mingw build.
 * - remove some sound-specific special cases in gui and main code.
 * - create thread in windows driver.
 * - remove all traces of "program states" variable.
 * - remove snd_open mutex: it is now unnecessary. Concurrency is handled
 *   through joining/waiting threads where necessary. (We assume JACK
 *   does its own locking, though.)
 *
 * Revision 1.10  2006/07/25 23:41:14  alankila
 * - this patch may break win32. I can't test it.
 *   - move audio_thread handling code into sound driver init/finish
 *   - remove state variable from sight of the Linux code -- it should be
 *     killed also on Win32 side using similar strategies
 *   - snd_open mutex starts to look spurious. It can probably be removed.
 *
 * Revision 1.9  2006/07/23 21:01:08  alankila
 * - move the small loop out of the hot spot, put the larger loop in its stead
 *
 * Revision 1.8  2006/07/20 22:37:21  alankila
 * - can't write to debug buffer from audio thread: instant gui crash results
 *
 * Revision 1.7  2006/07/15 23:02:45  alankila
 * - remove the bits control -- just use the best available on every driver.
 *
 * Revision 1.6  2006/07/15 13:29:55  alankila
 * - fix copypaste error reporting
 *
 * Revision 1.5  2006/07/14 23:11:41  alankila
 * - set jack buffer size from gnuitar (this might not be the most friendly
 *   thing to do, but we can do it and I think we should).
 * - add 1,4 and 2,4 channel pairs to the config. To make these work you need
 *   an asym surroud40 type .asoundrc configuration, though.
 * - add sample rate adjustment callback. I'd prefer to be able to force
 *   sample_rate myself, but this will have to do.
 *
 * Revision 1.4  2006/07/14 21:24:18  alankila
 * - fix English, normalise spacing
 * - add tests to ensure that input and output ports do exist
 * - force the driver to give up if all specified input/output ports can not
 *   be registered.
 * - don't leak memory on failed JACK initialisations.
 *
 * Revision 1.3  2006/06/20 20:41:05  anarsoul
 * Added some kind of status window. Now we can use gnuitar_printf(char *fmt, ...) that redirects debug information in this window.
 *
 * Revision 1.2  2006/05/30 08:48:41  anarsoul
 * Fixed crash if GNUitar exited without stopping sound processing
 *
 * Revision 1.1  2006/05/29 18:36:54  anarsoul
 * Initial JACK support
 *
 */

#ifdef HAVE_JACK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <assert.h>
#include <jack/jack.h>
#ifdef HAVE_ALSA /* for midi -- it seems JACK doesn't have its own midi system. */
#include <alsa/asoundlib.h>
#include "audio-midi.h"
#endif

#include "audio-jack.h"
#include "audio-driver.h"
#include "main.h"
#include "gui.h"
#include "pump.h"

static char *jack_server_name = NULL; /* in the future, maybe something else? */
//static jack_options_t options = JackNullOption;
static int options = 0; /* for older JACK... */

static jack_status_t status;
static jack_client_t *client;
static jack_port_t *input_ports[MAX_CHANNELS];  // capture ports
static jack_port_t *output_ports[MAX_CHANNELS]; // playback ports

#ifdef HAVE_ALSA
static snd_seq_t *sequencer_handle = NULL;

static void
alsa_midi_event(void)
{
    snd_seq_event_t *ev;

    /* if no events in software/hardware queue, do nothing */
    if (! snd_seq_event_input_pending(sequencer_handle, 1))
        return;

    /* while more midi events in software queue */
    while (snd_seq_event_input_pending(sequencer_handle, 0)) {
        /* obtain an event */
        snd_seq_event_input(sequencer_handle, &ev);
        switch (ev->type) {
            /* program change events */
            case SND_SEQ_EVENT_PGMCHANGE:
                midi_set_program(ev->data.control.value);
                break;
            /* controller (this is the thing that does the magic on FC-200) */
            case SND_SEQ_EVENT_CONTROLLER:
                midi_set_continuous_control(ev->data.control.param, ev->data.control.value / 127.f);
                break;
            default:
                break;
        }
        /* free on event is no longer required, it isn't malloced */
    }
}
#endif

static int 
process (jack_nframes_t nframes, void *arg)
{
    int i, k;
    jack_nframes_t j;
    data_block_t db = {
        .data = procbuf,
        .data_swap = procbuf2,
    };
    
    if (! audio_driver->enabled)
        return 0;
    
    if (nframes != buffer_size)
	buffer_size = nframes;
    
    //capturing
    db.len = buffer_size * n_input_channels;
    db.channels = n_input_channels;
    
    /* convert JACK's buffers to our interleaved format */
    for (i = 0; i < n_input_channels; i += 1) {
        jack_default_audio_sample_t *buf = jack_port_get_buffer(input_ports[i], nframes);
        assert(buf != NULL);
        
        k = i;
        for (j = 0; j < nframes; j += 1) {
            db.data[k] = buf[j] * (jack_default_audio_sample_t) (1 << 23);
            k += n_input_channels;
        }
    }

#ifdef HAVE_ALSA
    if (sequencer_handle != NULL)
        alsa_midi_event();
#endif
    pump_sample(&db);
    
    assert(db.channels == n_output_channels);
    assert(db.len / n_output_channels == buffer_size);

    /* convert our interleaved format to buffers expected by JACK */ 
    for (i = 0; i < n_output_channels; i += 1) {
        jack_default_audio_sample_t *buf = jack_port_get_buffer(output_ports[i], nframes);
        assert(buf != NULL);
	
        k = i;
        for (j = 0; j < nframes; j += 1) {
            buf[j] = (jack_default_audio_sample_t) db.data[k] / (jack_default_audio_sample_t) (1 << 23);
            k += n_output_channels;
        }
    }
    return 0;
}

/* sound shutdown */
#ifdef HAVE_ALSA
static void
alsa_midi_finish(void)
{
    if (sequencer_handle) {
        snd_seq_close(sequencer_handle);
        sequencer_handle = NULL;
    }
}
#endif

static void
jack_finish_sound(void)
{
    jack_driver.enabled = 0;
    jack_client_close(client);
#ifdef HAVE_ALSA
    alsa_midi_finish();
#endif
}

static void
jack_shutdown (void *arg)
{
    jack_finish_sound();
}

static int
update_sample_rate(jack_nframes_t nframes, void *args)
{
    sample_rate = nframes;
    return 0;
}

/* sound initialization */
#ifdef HAVE_ALSA
static int
alsa_midi_init(void)
{
    int client_id, port_id;

    if (snd_seq_open(&sequencer_handle, "default", SND_SEQ_OPEN_INPUT, 0) < 0) {
        sequencer_handle = NULL; /* just to be sure */
        gnuitar_printf("MIDI sequencer could not be opened -- skipping midi features.\n");
        return 1;
    }
    snd_seq_set_client_name(sequencer_handle, "GNUitar");
    client_id = snd_seq_client_id(sequencer_handle);
    
    if ((port_id = snd_seq_create_simple_port(sequencer_handle, "input",
                    SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
                    SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
        gnuitar_printf("Could not create MIDI port -- skipping midi features.\n");
        snd_seq_close(sequencer_handle);
        sequencer_handle = NULL;
        return 1;
    }
    
    return 0;
}
#endif

static int
jack_init_sound(void)
{
    unsigned int temp_rate;
    int i;
    const char **ports;
    char portname[20];
    
    //creating jack client
    client = jack_client_open ("GNUitar", options, &status, jack_server_name);
    if (client == NULL) {
	gnuitar_printf ("jack_client_open() failed (status=%d)\n", status);
	return ERR_WAVEOUTOPEN;
    }
    
    // This function is called as soon as data becomes available.
    jack_set_process_callback(client, process, NULL);
    
    // Tell the JACK server to call jack_shutdown() if if it shuts down
    jack_on_shutdown(client, jack_shutdown, NULL);
        
    // adapting to JACK's sample rate and buffer size
    temp_rate = jack_get_sample_rate(client);
    if (temp_rate != sample_rate) {
	gnuitar_printf("Adapting to JACK's sample rate: %d\n", temp_rate);
	sample_rate = temp_rate;
    }
    jack_set_sample_rate_callback(client, update_sample_rate, NULL);
    
    // register I/O ports
    if (n_input_channels > MAX_CHANNELS) n_input_channels = MAX_CHANNELS;
    if (n_output_channels > MAX_CHANNELS) n_output_channels = MAX_CHANNELS;
    
    for (i = 0; i < n_input_channels; i++) {
	sprintf(portname, "input_%d", i);
	input_ports[i] = jack_port_register(client, portname,
					    JACK_DEFAULT_AUDIO_TYPE,
					    JackPortIsInput, 0);
	if (input_ports[i] == NULL) {
	    gnuitar_printf("Registering input ports: tried to register %s but failed.\n",
                           portname);
	    jack_client_close(client);
	    return ERR_WAVEINOPEN;
	}
    }
    
    for (i = 0; i < n_output_channels; i++) {
	sprintf(portname, "output_%d", i);
	output_ports[i] = jack_port_register(client, portname,
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsOutput, 0);
	if (output_ports[i] == NULL) {
	    gnuitar_printf("Registering output ports: tried to register %s but failed.\n",
                           portname);
	    jack_client_close(client);
	    return ERR_WAVEOUTOPEN;
	}
    
    }
    
    /* Tell the JACK server that we are ready to roll. Our processing
     * callback will start running now. Until enabled is set to true,
     * however, it will do nothing. */
    if (jack_activate(client)) {
	gnuitar_printf("Failed to active client -- unknown reason, see STDERR output\n");
	jack_client_close(client);
	return ERR_WAVEOUTOPEN;
    }
   
    /* Try to set buffer size for JACK. This is potentially unfriendly action to do. *
     * Result will be handled through adaptation by the processing callback. */
    jack_set_buffer_size(client, buffer_size);
    
    /* Connecting capture ports to our ports. I think gnuitar might be
     * used as an effect processor even when there is no actual sound
     * hardware, so some errors are not fatal. */
    ports = jack_get_ports(client, NULL, JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsPhysical|JackPortIsOutput);
		
    if (ports == NULL) {
	gnuitar_printf("warning: No physical capture ports. Not making connections.\n");
    } else {
        for (i = 0; i < n_input_channels; i++) {
            gnuitar_printf("Reading channel %d input from port: %s\n", i+1, ports[i]);
	    if (ports[i] == NULL) {
	        gnuitar_printf("warning: No physical capture port for channel %d!\n", i+1);
                continue;
	    }

	    /* On the other hand, if port is there, the connection should
             * succeed. Something is seriously wrong if not. */
            if (jack_connect(client, ports[i], jack_port_name(input_ports[i]))) {
	        gnuitar_printf("Cannot connect capture port %s to %s\n", ports[i], jack_port_name(input_ports[i]));
	        jack_client_close(client);
                free(ports);
	        return ERR_WAVEINOPEN;
            }
	}
        free(ports);
    }
    
    // Connecting our ports to playback ports
    ports = jack_get_ports(client, NULL, JACK_DEFAULT_AUDIO_TYPE,
		           JackPortIsPhysical|JackPortIsInput);
		
    if (ports == NULL) {
	gnuitar_printf("warning: No physical playback ports. Not making connections.\n");
    } else {
        for (i = 0; i < n_output_channels; i++) {
            gnuitar_printf("Sending channel %d output to port: %s\n", i+1, ports[i]);
	    if (ports[i] == NULL) {
	        gnuitar_printf("warning: No physical playback port for channel %d.\n", i+1);
	        continue;
	    }
	
	    if (jack_connect(client, jack_port_name(output_ports[i]), ports[i])) {
	        gnuitar_printf("Cannot connect playback port %s to %s\n", jack_port_name(output_ports[i]), ports[i]);
	        jack_client_close(client);
                free(ports);
	        return ERR_WAVEOUTOPEN;
            }
	}
        free(ports);
    }

#ifdef HAVE_ALSA
    if (sequencer_handle == NULL)
        alsa_midi_init();
#endif
    jack_driver.enabled = 1;
    return ERR_NOERROR;
}

int
jack_available()
{
    client = jack_client_open("GNUitar", options, &status, jack_server_name);

    // if client creation failed	    
    if (client == NULL) {
	gnuitar_printf("JACK sound server couldn't be opened -- skipping JACK driver\n");
	return 1;
    }
    
    // test ok: JACK available, closing client
    jack_client_close(client);
    client = NULL;
    
    return 0;
    
}

static const struct audio_driver_channels jack_channels_cfg[] = {
    { 1, 1 },
    { 1, 2 },
    { 1, 4 },
    { 2, 2 },
    { 2, 4 },
    { 0, 0 }
};

audio_driver_t jack_driver = {
    .str = "JACK",
    .enabled = 0,
    .channels = jack_channels_cfg,
    
    .init = jack_init_sound,
    .finish = jack_finish_sound,
};

#endif /* HAVE_JACK */
