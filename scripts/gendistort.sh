#!/bin/sh

for R1 in 1 2 4 6 8 10 12 14 16 18 20; do
    for((R2=400;R2<=520;R2+=10)); do
	./gen_distort2_lookup $R1 $R2 $sample_rate
    done
done
