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
#include "pump.h"
#include "main.h"

static char *jack_server_name = NULL; /* in the future, maybe something else? */
static jack_options_t options = JackNullOption;

static jack_status_t status;
static jack_client_t *client;
static jack_port_t *input_ports[MAX_CHANNELS];  // capture ports
static jack_port_t *output_ports[MAX_CHANNELS]; // playback ports

static int 
process (jack_nframes_t nframes, void *arg)
{
    int i,j,k;
    jack_default_audio_sample_t *in[MAX_CHANNELS], *out[MAX_CHANNELS];
    static struct data_block db = {
        .data = procbuf,
        .data_swap = procbuf2,
    };
    
    if (nframes != buffer_size)
    {
	gnuitar_printf("JACK gave %d frames instead of expected %d\n",
                       nframes, buffer_size);
	buffer_size = nframes;
    }
    
    my_lock_mutex(snd_open);
    
    //capturing
    db.len = buffer_size * n_input_channels;
    db.channels = n_input_channels;
    
    for (k = 0; k < n_input_channels; k++) {
        jack_default_audio_sample_t *buf = jack_port_get_buffer(input_ports[k], nframes);
        assert(buf != NULL);
        in[k] = buf;
    }
    
    k = 0;
    for (i = 0; i < nframes; i++)
    {
	for (j = 0; j < n_input_channels; j++)
            db.data[k++] = in[j][i] * (1 << 23);
    }
    
    pump_sample(&db);
    
    assert(db.channels == n_output_channels);
    assert(db.len / n_output_channels == buffer_size);
    
    for (k = 0; k < n_output_channels; k++) {
        jack_default_audio_sample_t *buf = jack_port_get_buffer(output_ports[k], nframes);
        assert(buf != NULL);
        out[k] = buf;
    }
    
    k = 0;	
    for (i = 0; i < nframes; i++)
    {
	for (j = 0; j < n_output_channels; j++)
            out[j][i] = (jack_default_audio_sample_t) db.data[k++] / (1 << 23);
    }
    
    my_unlock_mutex(snd_open);

    return 0;
}



/*
 * sound shutdown 
 */
static void
jack_finish_sound(void)
{
    state = STATE_PAUSE;
    my_lock_mutex(snd_open);
    jack_driver.enabled = 0;
    jack_client_close(client);
    client = NULL;
}

/* The audio thread is unnecessary for JACK.
 * It's still started, but it exits as soon as it starts. */
static void *
jack_audio_thread(void *V)
{
    return NULL;
}


static void
jack_shutdown (void *arg)
{
    jack_finish_sound();
}


/*
 * sound initialization
 */
static int
jack_init_sound(void)
{
    unsigned int temp_rate, temp_buffersize;
    int i;
    const char **ports;
    char portname[20];
    
    //creating jack client
    client = jack_client_open ("GNUitar", options, &status, jack_server_name);
    if (client == NULL)
    {
	gnuitar_printf ("jack_client_new() failed (status=%d)\n", status);
	return ERR_WAVEOUTOPEN;
    }
    
    // This function is called as soon as data becomes available.
    jack_set_process_callback (client, process, 0);
    
    // Tell the JACK server to call jack_shutdown() if if it shuts down
    jack_on_shutdown(client, jack_shutdown, 0);
    
    // adapting to JACK's sample rate and buffer size
    temp_rate = jack_get_sample_rate(client);
    if (temp_rate != sample_rate)
    {
	gnuitar_printf("Adapting to JACK's sample rate: %d\n", temp_rate);
	sample_rate = temp_rate;
    }
    
    temp_buffersize = jack_get_buffer_size (client);
    if (temp_buffersize != buffer_size)
    {
	gnuitar_printf("Adapting to JACK's buffer size: %d\n", temp_buffersize);
	buffer_size = temp_buffersize;
    }
   
    // register I/O ports
    if (n_input_channels > MAX_CHANNELS) n_input_channels = MAX_CHANNELS;
    if (n_output_channels > MAX_CHANNELS) n_output_channels = MAX_CHANNELS;
    
    for (i = 0; i < n_input_channels; i++)
    {
	sprintf(portname, "input_%d", i);
	input_ports[i] = jack_port_register(client, portname,
					    JACK_DEFAULT_AUDIO_TYPE,
					    JackPortIsInput, 0);
	if (input_ports[i] == NULL) 
	{
	    gnuitar_printf("Registering input ports: tried to register %s but failed.\n",
                           portname);
	    jack_client_close(client);
	    return ERR_WAVEINOPEN;
	}
    
    }
    
    for (i = 0; i < n_output_channels; i++)
    {
	sprintf(portname, "output_%d", i);
	output_ports[i] = jack_port_register(client, portname,
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsOutput, 0);
	if (output_ports[i] == NULL) 
	{
	    gnuitar_printf("Registering output ports: tried to register %s but failed.\n",
                           portname);
	    jack_client_close(client);
	    return ERR_WAVEOUTOPEN;
	}
    
    }
    
    // Tell the JACK server that we are ready to roll. Our process()
    // callback will start running now.
    if (jack_activate(client))
    {
	gnuitar_printf("failed to active client -- unknown reason, see STDERR output\n");
	jack_client_close(client);
	return ERR_WAVEOUTOPEN;
    }
   
    
    // Connecting capture ports to our ports
    ports = jack_get_ports(client, NULL, NULL,
                           JackPortIsPhysical|JackPortIsOutput);
		
    if (ports == NULL)
    {
	gnuitar_printf("No physical capture ports!\n");
	jack_client_close(client);
	return ERR_WAVEINOPEN;
    }
    
    for (i = 0; i < n_input_channels; i++)
    {
	if (ports[i] == NULL)
	{
	    gnuitar_printf("No physical capture port for channel %d!\n", i+1);
	    jack_client_close(client);
            free(ports);
	    return ERR_WAVEINOPEN;
	}
        gnuitar_printf("Reading channel %d input from port: %s\n", i+1, ports[i]);

	if (jack_connect(client, ports[i], jack_port_name(input_ports[i]))) 
	{
	    gnuitar_printf("Cannot connect capture port %s to %s\n", ports[i], jack_port_name(input_ports[i]));
	    jack_client_close(client);
            free(ports);
	    return ERR_WAVEINOPEN;
	}
    }
    free(ports);
    
    // Connecting our ports to playback ports
    ports = jack_get_ports(client, NULL, NULL,
		           JackPortIsPhysical|JackPortIsInput);
		
    if (ports == NULL)
    {
	gnuitar_printf("No physical playback ports.\n");
	jack_client_close(client);
	return ERR_WAVEINOPEN;
    }
    
    for (i = 0; i < n_output_channels; i++)
    {
	if (ports[i] == NULL)
	{
	    gnuitar_printf("No physical playback port for channel %d.\n", i+1);
	    jack_client_close(client);
	    free(ports);
            return ERR_WAVEOUTOPEN;
	}
        gnuitar_printf("Sending channel %d output to port: %s\n", i+1, ports[i]);
	
	if (jack_connect(client, jack_port_name(output_ports[i]), ports[i])) 
	{
	    gnuitar_printf("Cannot connect playback port %s to %s\n", jack_port_name(output_ports[i]), ports[i]);
	    jack_client_close(client);
            free(ports);
	    return ERR_WAVEOUTOPEN;
	}
    }
    free (ports);
    
    state = STATE_PROCESS;
    jack_driver.enabled = 1;
    my_unlock_mutex(snd_open);
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

static struct audio_driver_channels jack_channels_cfg[] = {
    { 1, 1 },
    { 1, 2 },
    { 2, 2 },
    { 0, 0 }
};

static unsigned int jack_bits_cfg[] = { 16, 32, 0 };

audio_driver_t jack_driver = {
    .str = "JACK",
    .enabled = 0,
    .channels = jack_channels_cfg,
    .bits = jack_bits_cfg,
    
    .init = jack_init_sound,
    .finish = jack_finish_sound,
    .thread = jack_audio_thread
};

#endif /* HAVE_JACK */
