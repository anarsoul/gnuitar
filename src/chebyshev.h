/*
 * GNUitar
 * Chebyshev filter
 * Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
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
 * $Id$
 *
 * $Log$
 * Revision 1.2  2005/04/06 19:34:20  fonin
 * Code lickup
 *
 */

#ifndef _BIQUAD_H_
#include "biquad.h"
#endif

#ifndef _CHEBYSHEV_H_
#define _CHEBYSHEV_H_ 1

void            CalcChebyshev2(double Fs, double Fc, double ripple,
			       int lowpass, struct Biquad *filter);



#endif
