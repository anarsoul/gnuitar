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
 *
 * $Log$
 * Revision 1.7  2005/08/28 21:45:12  fonin
 * Portability: introduced new functions for mutexes
 *
 * Revision 1.6  2005/08/28 14:04:04  alankila
 * - OSS copypaste error fix
 * - remove my_log2 in favour of doing pow, trunc, log.
 * - OSS driver rounds buffer sizes to suitable values by itself now. There's
 *   a precedent in tuning user parameters automatically in ALSA code. The
 *   new behaviour rounds buffer size down, though.
 *
 * Revision 1.5  2005/08/27 18:11:35  alankila
 * - support 32-bit sampling
 * - use 24-bit precision in integer arithmetics
 * - fix effects that contain assumptions about absolute sample values
 *
 * Revision 1.4  2003/05/30 12:49:23  fonin
 * log2() renamed to my_log2() since log2 is a reserved word on MacOS X.
 *
 * Revision 1.3  2003/04/16 18:40:56  fonin
 * my_itoa() is defined for Windows also.
 *
 * Revision 1.2  2003/04/11 18:33:56  fonin
 * my_itoa() moved to utils.h.
 *
 * Revision 1.1  2003/03/09 21:00:32  fonin
 * Utility constants and functions.
 *
 */
#include "utils.h"

char           *
my_itoa(int i)
{
    switch (i) {
    case 48000:
	return "48000";
    case 44100:
	return "44100";
    case 22050:
	return "22050";
    case 16000:
	return "16000";
    case 8:
	return "8";
    case 16:
	return "16";
    case 32:
	return "32";
    case 1:
	return "1";
    case 2:
	return "2";
    case 4:
	return "4";
    }
    return "";
}

#ifndef _WIN32
inline void my_create_mutex(my_mutex* m) {
    *m=g_mutex_new();
}

inline void my_lock_mutex(my_mutex m) {
    g_mutex_lock(m);
}

inline void my_unlock_mutex(my_mutex m) {
    g_mutex_unlock(m);
}

inline void my_close_mutex(my_mutex m) {
    g_mutex_free(m);
}

#else

inline void my_create_mutex(my_mutex m*) {
    *m=CreateMutex(NULL,FALSE,NULL);
}

inline void my_lock_mutex(my_mutex m) {
    if(m)
        WaitForSingleObject(m,INFINITE);
}

inline void my_unlock_mutex(my_mutex m) {
    if(m)
        ReleaseMutex(m);
}

inline void my_close_mutex(my_mutex m) {
    if(m)
        CloseHandle(m);
}
#endif
