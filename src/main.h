/*
 * GNUitar
 * Utility functions
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
 */

#ifndef _MAIN_H_
#define _MAIN_H_ 1

typedef enum {
    ERR_NOERROR = 0,
    ERR_THREAD, 		/* cannot create audio thread */
    ERR_WAVEINOPEN,		/* cannot open wave in device */
    ERR_WAVEOUTOPEN,		/* cannot open wave out device */
    ERR_WAVEFRAGMENT,   	/* cannot set fragments */
    ERR_WAVEDUPLEX,		/* cannot open device in duplex mode */
    ERR_WAVENOTDUPLEX,  	/* device is not full-duplex capable */
    ERR_WAVEGETCAPS,		/* cannot get device capabilities */
    ERR_WAVESETBIT,		/* cannot set 8/16bits */
    ERR_WAVESETCHANNELS,	/* cannot set mono/stereo mode */
    ERR_WAVESETRATE,		/* cannot set sampling rate */
    ERR_WAVEOUTHDR,		/* error preparing write header */
    ERR_WAVEINHDR,		/* error preparing record header */
    ERR_WAVEINQUEUE,		/* error queuing record header */
    ERR_WAVEINRECORD,           /* recording error */
    ERR_DSOUNDOPEN,		
    ERR_DSOUNDBUFFER,	
    ERR_DSOUNDPLAYBACK,	
    ERR_DSCOOPLEVEL,
    ERR_NOAUDIOAVAILABLE
} GnuitarErr;

extern char version[];

#endif
