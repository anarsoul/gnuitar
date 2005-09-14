#!/usr/bin/python
#
# This program is used to show the frequency response of a FIR filter function.
#
# You should plot the spectrum with gnuplot using commands such as:
#
# plot "foo.txt" with lines smooth cspline

import math

def hamming(x):
    return math.sin(x * math.pi)

def hanning(x):
    return 0.04 + math.sin(x * math.pi) * 0.96

def sinc(x):
    if x == 0:
        return 1;
    return math.sin(x * math.pi) / x / math.pi
        
def reversebitorder(i, fftlen):
    j = 0;
    k = 1;
    while k < fftlen:
        j <<= 1
        k <<= 1
        j |= i & 1
        i >>= 1
    return j

# This is the tersest FFT code I could come up with.
def do_fft(fft, sign):
    fftlen = len(fft)
    for i in range(1, fftlen-1):
        j = reversebitorder(i, fftlen)
        if i < j:
            fft[i], fft[j] = fft[j], fft[i]

    le = 1
    while le < fftlen:
        for j in range(0, le):
            angle = math.pi * j / le
            u = math.cos(angle) + sign * math.sin(angle) * 1j;
            for i in range(j, fftlen, le << 1):
                t = fft[i+le] * u
                fft[i+le] = fft[i] - t
                fft[i]    = fft[i] + t
        le <<= 1

def main():
    spectrum = []
    # sinc with stopband starting at fs/2
    for _ in range(1024):
        spectrum += [sinc(_ / 2.0) + 0j]

    # window the sinc, taking 14 coefficients
    coeffs = 14
    for _ in range(0, 1024):
        if _ < coeffs:
            spectrum[_] *= hanning(0.5 + 1. * _ / coeffs)
        else:
            spectrum[_] *= 0j

    print "# FIR coefficients"
    print "#"
    for _ in range(0, 1024):
        if abs(spectrum[_]) > 1e-14:
            print "# %d %f" % (_, spectrum[_].real)
    print

    # -1 is forward, 1 is inverse
    do_fft(spectrum, -1)

    for _ in range(0, len(spectrum)/2 + 1):
        print "%f %f %f" % (_, spectrum[_].real, spectrum[_].imag)

if __name__ == '__main__':
    main()
