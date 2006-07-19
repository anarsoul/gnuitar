#!/usr/bin/python

import math

STEEPNESS  = 4e-3
SCALE      = 1e2
STEEPNESS2 = 1e-2
SCALE2     = 5e-1
def solve_equation(x):
    y = 0.0
    while True:
        value = x - y - SCALE * math.exp(STEEPNESS * y) + SCALE2 * math.exp(STEEPNESS2 * -y);
        
        # newton
        dvalue_y = -1 - (SCALE * STEEPNESS) * math.exp(STEEPNESS * y) - (SCALE2 * STEEPNESS2) * math.exp(STEEPNESS2 * -y);

        dy = value / dvalue_y
        y = (y + (y - dy)) / 2;

        # determine when we are near root
        if abs(value) < 1e-4:
            return y

for _ in range(-8192, 8192, 10):
    print "%s %s" % (_, solve_equation(_))
