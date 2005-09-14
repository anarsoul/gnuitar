#!/usr/bin/python
#
# This program is used to show the frequency response of a FIR filter function.
#
# You should plot the spectra with gnuplot using commands such as:
#
# set xscale [20:20000]
# set logscale x
# plot "foo.txt" with lines smooth cspline

import math

def hamming(x):
    return math.sin(x * math.pi)

def hanning(x):
    return 0.04 + math.sin(x * math.pi) * 0.46

def sinc(x):
    if (x == 0):
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
    spectra = []
    for _ in range(1024):
        spectra += [sinc(_ / 2.0) + 0j]
    for _ in range(0, 1024):
        if _ < 14:
            spectra[_] *= hanning(0.5 + (_ + 0.5) / 14.0)
        else:
            spectra[_] = 0j

    print "# FIR coefficients"
    print "#"
    for _ in range(0, 1024):
        if abs(spectra[_]) > 1e-10:
            print "# %d %f" % (_, spectra[_].real)
    print

    #for _ in range(0, len(spectra), 2):
    #    spectra[_] *= hamming(1.0 * _ / len(spectra))
    #
    do_fft(spectra, -1)

    #for _ in range(1024):
    #    power = 0.0
    #    if _ < 32:
    #        power = 1.0
    #    spectra += [power + 0j]
    #do_fft(spectra, 1)

    for _ in range(0, len(spectra)/2 + 1):
        print "%f %f %f" % (_, spectra[_].real, spectra[_].imag)

if __name__ == '__main__':
    main()
