/*
 * GNUitar
 * Eq Bank effect
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 * Ciulei Bogdan /Dexterus              <dexterus@hackernetwork.com>
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
 * $Log$
 * Revision 1.5  2005/09/04 12:12:36  alankila
 * - make create() and done() symmetric in memory allocation/free
 *
 * Revision 1.4  2005/08/10 11:06:26  alankila
 * - sync to biquad interface
 * - change storage types of boosts and volume to double to keep fractions
 * - use sizeof(params->foo) instead of hardcoded sizes
 *
 * Revision 1.3  2004/10/21 11:16:26  dexterus
 * Made to work with new biquad.c version (1.3)
 * Overall functional
 *
 * Revision 1.2  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 */

#ifndef _EQBANK_H_
#define _EQBANK_H_ 1


#include "pump.h"

#ifndef _BIQUAD_H_
	#include "biquad.h"
#endif

extern effect_t *   eqbank_create();

struct eqbank_params {
    double          *boosts;
    double          volume;
    struct Biquad  *filters;
};



#endif
