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

#ifndef _UTILS_H_
#define _UTILS_H_ 1

#define ERR_NOERROR		0
#define ERR_THREAD		1	/* cannot create audio thread */
#define ERR_DEMOEXPIRED		2	/* demo version is expired */
#define ERR_WAVEINOPEN		3	/* cannot open wave in device */
#define ERR_WAVEOUTOPEN		4	/* cannot open wave out device */
#define ERR_WAVEFRAGMENT	5	/* cannot set fragments */
#define ERR_WAVEDUPLEX		6	/* cannot open device in duplex
					 * mode */
#define ERR_WAVENOTDUPLEX	7	/* device is not full-duplex
					 * capable */
#define ERR_WAVEGETCAPS		8	/* cannot get device capabilities */
#define ERR_WAVESETBIT		9	/* cannot set 8/16bits */
#define ERR_WAVESETCHANNELS	10	/* cannot set mono/stereo mode */
#define ERR_WAVESETRATE		11	/* cannot set sampling rate */
#define ERR_WAVEOUTHDR		12	/* error preparing write header */
#define ERR_WAVEINHDR		13	/* error preparing record header */
#define ERR_WAVEINQUEUE		14	/* error queuing record header */
#define ERR_WAVEINRECORD	15	/* recording error */
#define ERR_DSOUNDOPEN		16
#define ERR_DSOUNDBUFFER	17
#define ERR_DSOUNDPLAYBACK	18
#define ERR_DSCOOPLEVEL		19


/*
 * Program states:
 */
#define STATE_PROCESS		0	/* 0 - recording/playback is on */
#define STATE_PAUSE		1	/* 1 - playback is paused */
#define STATE_EXIT		2	/* 2 - exit thread */
#define STATE_START		3	/* 3 - the thread is just started
					 * (first loop), playback is on */
#define STATE_START_PAUSE	4	/* 4 - application is starting,
					 * playback is paused */

#include "pump.h"
#ifdef _WIN32
#    define M_PI 3.1415926535897932384626433832795
#    ifndef  R_OK
#        define  R_OK 04
#    endif
#    ifndef  X_OK
#        define  X_OK 01
#    endif
#    ifndef  F_OK
#        define  F_OK 00
#    endif
#    define FILESEP "\\"
#else
#    define FILESEP "/"
#endif

extern short    my_log2(int);
extern char    *my_itoa(int i);

#endif
