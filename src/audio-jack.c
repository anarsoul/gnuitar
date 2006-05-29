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

static jack_options_t options = JackNullOption;

static jack_status_t status;
static jack_client_t *client;
static jack_port_t *input_ports[4]; //capture ports
static jack_port_t *output_ports[4]; //playback ports
volatile static unsigned short buf_ready;

int 
process (jack_nframes_t nframes, void *arg)
{
    if (state == STATE_EXIT) return 0;
    if (!jack_driver.enabled) return 0;

    jack_default_audio_sample_t *in[4], *out[4], *buf;
    int i,j,k;
//    size_t size =  sizeof (jack_default_audio_sample_t) * nframes;
//    SAMPLE16    *rdbuf16 = (SAMPLE16 *) rdbuf;
    SAMPLE32    *rdbuf32 = (SAMPLE32 *) rdbuf;
//    SAMPLE16    *wrbuf16 = (SAMPLE16 *) wrbuf;
    SAMPLE32    *wrbuf32 = (SAMPLE32 *) wrbuf;

    
    static struct data_block db;
    
    db.data = procbuf;
    db.data_swap = procbuf2;
    db.len = buffer_size * n_output_channels;
    
    if (nframes != buffer_size)
    {
	fprintf(stderr,"Buffer size and nframes are different\n");
	buffer_size = nframes;
    }
    
    my_lock_mutex(snd_open);
    
    //capturing
    buf = (jack_default_audio_sample_t *)rdbuf;
    db.len = buffer_size * n_input_channels;
    db.channels = n_input_channels;
    
    for (k = 0; k < n_input_channels; k++)
    {
	in[k] = jack_port_get_buffer (input_ports[k], nframes);
    }
    
	
    k = 0;
    for (i = 0; i < nframes; i++)
    {
	for (j = 0; j < n_input_channels; j++)
	{
	    if (in[j][i]>0) rdbuf32[k] = (SAMPLE32) (in[j][i]*8388607.0);
	    else rdbuf32[k] = (SAMPLE32) (in[j][i]*8388608.0);
	    k++;
	}
    }
	
    
    for (i = 0; i < db.len; i++)
	db.data[i] = rdbuf32[i];
    
    pump_sample(&db);
    
    assert(db.channels == n_output_channels);
    assert(db.len / n_output_channels == buffer_size);
    
    //playback
    for (i = 0; i < db.len; i++)
	wrbuf32[i] = (SAMPLE32) db.data[i];
    
    for (k = 0; k < n_output_channels; k++)
    {
	out[k] = jack_port_get_buffer (output_ports[k], nframes);
    }
    k = 0;	
    for (i = 0; i < nframes; i++)
    {
	for (j = 0; j < n_output_channels; j++)
	{
	    if (wrbuf[k] > 0) out[j][i] = (jack_default_audio_sample_t) wrbuf[k]/8388607.0;
	    else
    	    out[j][i] = (jack_default_audio_sample_t) wrbuf[k]/8388608.0;
	    k++;
	}	
    
    }
    
    my_unlock_mutex(snd_open);

    return 0;

}

static void *
jack_audio_thread(void *V)
{
    while (state != STATE_EXIT && state != STATE_ATHREAD_RESTART) {
	
	usleep(1000);
    }
    
    jack_client_close(client);
    
    return NULL;
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
    jack_client_close (client);
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
    const char *server_name = NULL;
    unsigned int temp_rate, temp_buffersize;
    int i;
    const char **ports;
    char portname[256];
    
    buf_ready = 0;
    options = JackNullOption;

    //creating jack client
    client = jack_client_open ("GNUitar", options, &status, server_name);
    
    if (client == 0)
    {
	fprintf (stderr, "jack_client_new() failed\n");
//	    "status = 0x%2.0x\n", status);
//	if (status & JackServerFailed) 
//	{
//	    fprintf(stderr, "Unable to connect to JACK server \n");
//	}
	
	return ERR_WAVEOUTOPEN;
    }
    
    //thread_info.client = client;
    //thread_info.can_process = 0;
    //setting up callback function
    jack_set_process_callback (client, process, 0);
    
    //tell the JACK server to call 'jack_shutdown()' if
    //if ever shuts down
    jack_on_shutdown (client, jack_shutdown, 0);
    
    
    //adapting to JACK's sample rate and buffer size
    //'cos we have no own resampler :)
    temp_rate = jack_get_sample_rate (client);
    
    if (temp_rate != sample_rate)
    {
	fprintf(stderr, "Adapting to JACK's sample rate: %d\n", temp_rate);
	sample_rate = temp_rate;
    }
    
    temp_buffersize = jack_get_buffer_size (client);
    
    if (temp_buffersize != buffer_size)
    {
	fprintf(stderr, "Adapting to JACK's buffer size: %d\n", temp_buffersize);
	buffer_size = temp_buffersize;
    }
    
    if (sizeof(jack_default_audio_sample_t) == 4 && bits != 32)
    {
	bits = sizeof(jack_default_audio_sample_t) << 3;
	fprintf(stderr, "Adapting to JACK's bits: %d\n", bits);

    }
    
    if (sizeof(jack_default_audio_sample_t) == 2 && bits != 16)
    {
	bits = sizeof(jack_default_audio_sample_t) << 3;
	fprintf(stderr, "Adapting to JACK's bits: %d\n", bits);

    }
    
    fprintf(stderr, "Bits: %d\n", bits);
    //registering input\output ports
    if (n_input_channels > 4) n_input_channels = 4;
    if (n_output_channels > 4) n_output_channels = 4;
    
    for (i = 0; i < n_input_channels; i++)
    {
	sprintf(portname, "input_%d", i);
	input_ports[i] = jack_port_register (client, portname,
					    JACK_DEFAULT_AUDIO_TYPE,
					    JackPortIsInput, 0);
	if (input_ports[i] == NULL) 
	{
	    fprintf(stderr, "no more JACK ports available\n");
	    jack_client_close(client);
	    return ERR_WAVEINOPEN;
	}
    
    }
    
    for (i = 0; i< n_output_channels; i++)
    {
	sprintf(portname, "output_%d", i);
	output_ports[i] = jack_port_register (client, portname,					    
					    JACK_DEFAULT_AUDIO_TYPE,
					    JackPortIsOutput, 0);
	if (output_ports[i] == NULL) 
	{
	    fprintf(stderr, "no more JACK ports available\n");
	    jack_client_close(client);
	    return ERR_WAVEOUTOPEN;
	}
    
    }
    
    //Tell the JACK server that we are ready to roll. Out process()
    //callback will start running now
    
    if (jack_activate (client))
    {
	fprintf (stderr, "Cannot activate client");
	jack_client_close(client);
	return ERR_WAVEOUTOPEN;
    }
    
    //Connect the ports. We can't do this before the client is
    //activated
    

    
    //connecting capture ports;
    ports = jack_get_ports (client, NULL, NULL,
		JackPortIsPhysical|JackPortIsOutput);
		
    if (ports == NULL)
    {
	fprintf(stderr, "no physical capture ports\n");
	jack_client_close(client);
	return ERR_WAVEINOPEN;
    }
    
    for (i = 0; i < n_input_channels; i++)
    {
	fprintf(stderr, "physical capture port: %s\n", ports[i]);
	if (ports[i] == 0)
	{
	    fprintf(stderr, "No such ports\n");
	    //jack_client_close(client);
	    //return ERR_WAVEINOPEN;
	}

	if (jack_connect(client, ports[i], jack_port_name (input_ports[i]))) 
	{
	    fprintf(stderr, "Cannot connect capture ports\n");
	    jack_client_close(client);
	    return ERR_WAVEINOPEN;
	}
    
    }
    
    free (ports);
    
    //connecting playback ports;
    ports = jack_get_ports (client, NULL, NULL,
		JackPortIsPhysical|JackPortIsInput);
		
    if (ports == NULL)
    {
	fprintf(stderr, "no physical playback ports\n");
	jack_client_close(client);
	return ERR_WAVEINOPEN;
    }
    
    for (i = 0; i < n_output_channels; i++)
    {
	fprintf(stderr, "physical playback port: %s\n", ports[i]);    
	
	if (ports[i] == 0)
	{
	    fprintf(stderr, "No such ports\n");
	    //jack_client_close(client);
	    //return ERR_WAVEOUTOPEN;
	}
	
	if (jack_connect(client , jack_port_name (output_ports[i]), ports[i])) 
	{
	    fprintf(stderr, "Cannot connect playback ports\n");
	    //jack_client_close(client);
	    //return ERR_WAVEOUTOPEN;
	}
    
    }
    
    free (ports);

    
    //done :)
    
    state = STATE_PROCESS;
    jack_driver.enabled = 1;
    my_unlock_mutex(snd_open);
    return ERR_NOERROR;
}

int
jack_available() {

    //try to create client and connect it to server
    const char *server_name = NULL;
    options = JackNullOption;
    
    
    client = jack_client_open ("GNUitar", options, &status, server_name);

    //if client creation failed	    
    if (client == NULL) {

	fprintf (stderr, "jack_client_new() failed\n");
	//if (status & JackServerFailed) {
	//    fprintf(stderr, "Unable to connect to JACK server \n");
	//    }
	return 1;
    }
    
    //all is OK, JACK available, closing client

    jack_client_close (client);
    fprintf (stderr, "connected to JACK server!\n");
    
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
