#!/usr/bin/python

# this program loads a WAV file and spits out an impulse response suitable for
# use with gnuitar.

# The processing chain is as follows:
#
# impulse response
# -> pad to power of 2
# -> cepstrum
# -> minimum group delay modification (rejection of anticausal components in cepstrum)
# -> impulse
# -> truncation

import math, wave, sys, struct, optparse
from scipy.fftpack import fft, ifft
from scipy.signal import get_window

# this returns minimum phase reconstruction of the original input
def fir_minphase(table, pad_size=4):
    # table should be a real-valued table of FIR coefficients
    convolution_size = len(table)
    table += [0] * (convolution_size * (pad_size - 1))

    # compute the real cepstrum
    # fft -> abs + ln -> ifft -> real
    cepstrum = ifft(map(lambda x: math.log(x), abs(fft(table))))
    # because the positive and negative freqs were equal, imaginary content is neglible
    # cepstrum = map(lambda x: x.real, cepstrum)

    # window the cepstrum in such a way that anticausal components become rejected
    cepstrum[1                :len(cepstrum)/2] *= 2;
    cepstrum[len(cepstrum)/2+1:len(cepstrum)  ] *= 0;

    # now cancel the previous steps:
    # fft -> exp -> ifft -> real
    cepstrum = ifft(map(cexp, fft(cepstrum)))
    return map(lambda x: x.real, cepstrum[0:convolution_size])

# firstly, exp(xi) = (cos x + sin x * 1j)
# exp(a + bi) = exp(a) * exp(bi)
def cexp(x):
    x = complex(x)
    real = x.real
    imag = x.imag
    return math.exp(real) * (math.cos(imag) + math.sin(imag) * 1j)

def print_spectrum(table, sample_rate, full=False):
    fact = 2
    if full:
        fact = 1

    for _ in range(len(table) / fact):
        mag = math.log(abs(table[_])) / math.log(10) * 20
        pha = math.atan2(table[_].real, table[_].imag) / math.pi * 180
        print "%s %s %s" % (float(_) / len(table) * sample_rate, mag, pha)

def print_fir(table, format='gnuplot'):
    if format == 'gnuplot':
        for _ in range(len(table)):
            print "%s %s" % (_, table[_])
    elif format == 'c':
        for _ in range(len(table)):
            print ("%6s," % table[_]),
            if _ % 11 == 10:
                print

# in: opened wav file with 1 channel and 16 bit PCM
# out: table of floating point values in range [-1, 1[
def load_wav_file(wf):
    # read all data
    data = wf.readframes(wf.getnframes())
    # transform it to values

    if wf.getsampwidth() == 2: # 16-bit int
        data = struct.unpack("%dh" % wf.getnframes(), data)
        data = map(lambda _: float(_) / 65536, data)
    elif wf.getsampwidth() == 3: # 24-bit int
        # close eyes now
        tmp = []
        for _ in range(0, len(data), 3):
            dataslice = data[_ : _+3]
            dataslice = chr(0) + dataslice
            val, = struct.unpack("i", dataslice)
            tmp.append(float(val) / 2 ** 32)
        data = tmp
    elif wf.getsampwidth() == 4: # 32-bit float
        data = struct.unpack("%df" % wf.getnframes(), data)

    return data

# return new table which has power-of-two length, padded by zeroes.
def expand_to_power_of_two(table):
    tablelen = len(table)
    tablelen = math.log(tablelen) / math.log(2)
    tablelen = math.ceil(tablelen)
    tablelen = math.pow(2, tablelen)
    return table + [0] * int(tablelen - len(table))

def main():
    usage = "%prog [options] <wav file>"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('-s', '--spectrum', action='store_true', help='Dump spectrum');
    parser.add_option('-l', '--length', help='Spectrum size', default=256, type=int);
    options, args = parser.parse_args(sys.argv[1:])

    try:
        wavfile = wave.open(args[0])
    except Exception, e:
        print "Error loading input wav file: %s" % e
        sys.exit(1)

    if wavfile.getframerate() != 44100:
        print "Error: we only support 44.1 kHz wav files at the moment."
        sys.exit(1)
    if wavfile.getnchannels() != 1:
        print "Error: this file has more than one channel. Impulses are mono."
        sys.exit(1)

    table = load_wav_file(wavfile)
    table = expand_to_power_of_two(table)
    table = fir_minphase(table)
    if len(table) > options.length:
        table = table[0:options.length]
    if len(table) < options.length:
        table += [0] * (options.length - len(table))

    if options.spectrum:
        table = fft(table * get_window('hanning', len(table)))
        print_spectrum(table, sample_rate=wavfile.getframerate())
    else:
        table = map(lambda _: int(_ * 65536), table)
        print "int impulse[%d] = {" % len(table)
        print_fir(table, format='c')
        print "\n};"

if __name__ == '__main__':
    main()
