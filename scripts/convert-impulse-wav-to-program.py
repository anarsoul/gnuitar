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

import wave, sys, struct
from scipy.fftpack import fft, ifft

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
    data = wf.readframes(len(wf))
    # transform it to values
    data = struct.unpack("%dh" % len(wf), data)
    return map(lambda _: float(_) / 32768, data)

# return new table which has power-of-two length, padded by zeroes.
def expand_to_power_of_two(table):
    tablelen = len(table)
    tablelen = math.log(tablelen) / math.log(2)
    tablelen = math.ceil(tablelen)
    tablelen = math.pow(2, tablelen)
    return table + [0] * (tablelen - len(table))

def main():
    try:
        wavfile = wave.open(sys.argv[1])
    except Exception, e:
        print "Error loading input wav file. Did you use: prog foo.wav?"
        print "Stack trace: %s" % file
        sys.exit(1)

    if wavfile.getframerate() != 44100:
        print "Error: we only support 44.1 kHz wav files at the moment."
        sys.exit(1)
    if wavfile.getnchannels() != 1:
        print "Error: this file has more than one channel. Impulses are mono."
        sys.exit(1)
    if wavfile.getsampwidth() != 2:
        print "Error: we are lame and can only read 16-bit PCM data... The current sample width is: %d! Sorry :-(" % wavfile.getsampwidth()
        sys.exit(1)

    table = load_wav_file(wavfile)
    table = expand_to_power_of_two(table)
    table = fir_minphase(table)
    # the minimum phase reconstruction put everything interesting at the start of table 
    # 256 may still be a bit low, but FIR convolutions are expensive.
    if len(table) > 256:
        table = table[0:256]
    table = map(lambda _: int(_ * 32767), _)
    print_fir(table, format='c')

if __name__ == '__main__':
    main()
