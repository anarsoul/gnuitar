#!/usr/bin/python

import math
from scipy.fftpack import fft

basefreq = 2000
sampling_rate = 48000

# build 3 sec of signal
table = []
for _ in range(sampling_rate * 3):
    x = math.sin(2 * math.pi * _ / sampling_rate * basefreq)
    # make spectrum more interesting
    table.append(x)

# define shifting parameters
halfnotes = -5
pitch_modulation_frequency = 2.0

# compute
depth = pow(2, halfnotes / 12.0) - 1;
granulelen = depth * sampling_rate / pitch_modulation_frequency

output = []
# run it through pitch shifting algorithm, build 1 sec of pitch shifted output
phase_inc = pitch_modulation_frequency / sampling_rate
phase = 0
for _ in range(48000):
    tmp = 0
    phase_tmp = phase
    for i in range(3):
        gain = math.pow(math.sin(phase_tmp * math.pi), 2)
        tmp += gain * table[int(48000 + _ + granulelen * phase_tmp)]
        phase_tmp += 1/3.0
        if phase_tmp >= 1.0:
            phase_tmp -= 1.0
    output.append(tmp)
    phase += phase_inc
    if phase >= 1.0:
        phase -= 1.0

output = fft(output)
table = fft(table[48000:96000])
for _ in range(len(output)):
    print "%s %s %s" % (_, abs(output[_]) ** 2, abs(table[_]) ** 2)

#for _ in range(len(output)):
#    print "%s %s %s" % (_, output[_], table[4800 + _])


