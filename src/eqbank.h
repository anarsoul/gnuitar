/*
 * GNUitar
 * Eq Bank effect
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 * Ciulei Bogdan /Dexterus		<dexterus@hackernetwork.com>

 */

#ifndef _EQBANK_H_
#define _EQBANK_H_ 1


#include "pump.h"
#include "biquad.h"

extern void     eqbank_create(struct effect *);

struct eqbank_params {
	int *boosts;
	int volume;
	float ocoeff; /* contant to multiply with for volume*/
	struct Biquad *filters;
};

#endif