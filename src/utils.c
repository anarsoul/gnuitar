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
 * Revision 1.2  2003/04/11 18:33:56  fonin
 * my_itoa() moved to utils.h.
 *
 * Revision 1.1  2003/03/09 21:00:32  fonin
 * Utility constants and functions.
 *
 */
#include "utils.h"

#ifndef _WIN32
/*
 * Calculate base-2 logarithm of the value, up to 512k
 */
short
log2(int x)
{
    int             pow[] =
	{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192,
	16384, 32768,
	65536, 131072, 262144, 524288
    };
    int             i;
    for (i = 0; i < sizeof(pow) / sizeof(int); i++) {
	if (pow[i] == x)
	    return i;
    }
    return 0;
}

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
    case 1:
	return "1";
    case 2:
	return "2";
    }
    return "";
}

#endif
