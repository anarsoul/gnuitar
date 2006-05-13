#!/usr/bin/python
# encoding: iso-8859-15
#
# Plots biquad filters. See main() and modify for your needs.
# To view output graphically, you can use gnuplot. This program produces
# a text output of format "frequency magnitude angle". Assuming:
#
#   ./filter-response.py > output.txt
#
# you can view the plot in gnuplot with the following commands in gnuplot:
#
#   set logscale x
#   set xrange [20:20000]
#   plot "output.txt" using ($1):($2) smooth cspline
#   plot "output.txt" using ($1):($3) smooth cspline
#
# The 1st plot is frequency (Hz) vs. magnitude (dB)
# The 2nd plot is frequency (Hz) vs. phase (°)
#
# The bandwidth of many filters is thought to be where the output has dropped
# 3 dB, but for PEQ you should consider bandwidth as the frequency difference
# where the output has diminished to half the peak frequency.

import sys, math

class BiquadFilter(object):
    __slots__ = ['b0', 'b1', 'b2', 'b3', 'b4', 'b5', 'a1', 'a2', 'a3', 'a4', 'a5', 'x1', 'x2', 'y1', 'y2']

    def __init__(self, b0, b1, b2, a1, a2):
        self.b0 = b0
        self.b1 = b1
        self.b2 = b2
        self.b3 = self.b4 = self.b5 = 0
        self.a1 = a1
        self.a2 = a2
        self.a3 = self.a4 = self.a5 = 0

        self.x1 = 0.0
        self.x2 = 0.0
        self.y1 = 0.0
        self.y2 = 0.0
    
    def filter(self, x0):
        y0 = self.b0*x0 + self.b1*self.x1 + self.b2*self.x2 - self.a1*self.y1 - self.a2*self.y2

        self.x2 = self.x1
        self.x1 = x0

        self.y2 = self.y1
        self.y1 = y0

        return y0

    # mag and phi code comes from
    # http://www.yohng.com/dsp/cfsmp.c
    # it's nearly verbatim with only minor prologue to adapt C to Python
    # observe that this code uses swaps b and a around!
    def mag(self, rate, f):
        a0 = self.b0
        a1 = self.b1
        a2 = self.b2
        a3 = self.b3
        a4 = self.b4
        a5 = self.b5
        b0 = 1.0
        b1 = self.a1
        b2 = self.a2
        b3 = self.a3
        b4 = self.a4
        b5 = self.a5
        
        sin = math.sin
        cos = math.cos
        C_PI = math.pi
        sqrt = math.sqrt

        return sqrt((a0*a0 + a1*a1 + a2*a2 + a3*a3 + a4*a4 + a5*a5 +
              2*(a0*a1 + a1*a2 + a2*a3 + a3*a4 + a4*a5)*cos((2*f*C_PI)/rate) +
              2*(a0*a2 + a1*a3 + a2*a4 + a3*a5)*cos((4*f*C_PI)/rate) +
              2*a0*a3*cos((6*f*C_PI)/rate) + 2*a1*a4*cos((6*f*C_PI)/rate) +
              2*a2*a5*cos((6*f*C_PI)/rate) + 2*a0*a4*cos((8*f*C_PI)/rate) +
              2*a1*a5*cos((8*f*C_PI)/rate) + 2*a0*a5*cos((10*f*C_PI)/rate))/
              (b0*b0 + b1*b1 + b2*b2 + b3*b3 + b4*b4 + b5*b5 +
              2*(b0*b1 + b1*b2 + b2*b3 + b3*b4 + b4*b5)*cos((2*f*C_PI)/rate) +
              2*(b0*b2 + b1*b3 + b2*b4 + b3*b5)*cos((4*f*C_PI)/rate) +
              2*b0*b3*cos((6*f*C_PI)/rate) + 2*b1*b4*cos((6*f*C_PI)/rate) +
              2*b2*b5*cos((6*f*C_PI)/rate) + 2*b0*b4*cos((8*f*C_PI)/rate) +
              2*b1*b5*cos((8*f*C_PI)/rate) + 2*b0*b5*cos((10*f*C_PI)/rate)))

    def phi(self, rate, f):
        a0 = self.b0
        a1 = self.b1
        a2 = self.b2
        a3 = self.b3
        a4 = self.b4
        a5 = self.b5
        b0 = 1.0
        b1 = self.a1
        b2 = self.a2
        b3 = self.a3
        b4 = self.a4
        b5 = self.a5
        
        sin = math.sin
        cos = math.cos
        C_PI = math.pi
        atan2 = math.atan2
        
        return atan2((a0*b0 + a1*b1 + a2*b2 + a3*b3 + a4*b4 + a5*b5 +
              (a0*b1 + a1*(b0 + b2) + a2*(b1 + b3) + a5*b4 + a3*(b2 + b4) +
              a4*(b3 + b5))*cos((2*f*C_PI)/rate) +
              ((a0 + a4)*b2 + (a1 + a5)*b3 + a2*(b0 + b4) +
              a3*(b1 + b5))*cos((4*f*C_PI)/rate) + a3*b0*cos((6*f*C_PI)/rate) +
              a4*b1*cos((6*f*C_PI)/rate) + a5*b2*cos((6*f*C_PI)/rate) +
              a0*b3*cos((6*f*C_PI)/rate) + a1*b4*cos((6*f*C_PI)/rate) +
              a2*b5*cos((6*f*C_PI)/rate) + a4*b0*cos((8*f*C_PI)/rate) +
              a5*b1*cos((8*f*C_PI)/rate) + a0*b4*cos((8*f*C_PI)/rate) +
              a1*b5*cos((8*f*C_PI)/rate) +
              (a5*b0 + a0*b5)*cos((10*f*C_PI)/rate))/
              (b0*b0 + b1*b1 + b2*b2 + b3*b3 + b4*b4 + b5*b5 +
              2*((b0*b1 + b1*b2 + b3*(b2 + b4) + b4*b5)*cos((2*f*C_PI)/rate) +
              (b2*(b0 + b4) + b3*(b1 + b5))*cos((4*f*C_PI)/rate) +
              (b0*b3 + b1*b4 + b2*b5)*cos((6*f*C_PI)/rate) +
              (b0*b4 + b1*b5)*cos((8*f*C_PI)/rate) +
              b0*b5*cos((10*f*C_PI)/rate))),
             
            ((a1*b0 + a3*b0 + a5*b0 - a0*b1 + a2*b1 + a4*b1 - a1*b2 +
              a3*b2 + a5*b2 - a0*b3 - a2*b3 + a4*b3 -
              a1*b4 - a3*b4 + a5*b4 - a0*b5 - a2*b5 - a4*b5 +
              2*(a3*b1 + a5*b1 - a0*b2 + a4*(b0 + b2) - a1*b3 + a5*b3 +
              a2*(b0 - b4) - a0*b4 - a1*b5 - a3*b5)*cos((2*f*C_PI)/rate) +
              2*(a3*b0 + a4*b1 + a5*(b0 + b2) - a0*b3 - a1*b4 - a0*b5 - a2*b5)*
              cos((4*f*C_PI)/rate) + 2*a4*b0*cos((6*f*C_PI)/rate) +
              2*a5*b1*cos((6*f*C_PI)/rate) - 2*a0*b4*cos((6*f*C_PI)/rate) -
              2*a1*b5*cos((6*f*C_PI)/rate) + 2*a5*b0*cos((8*f*C_PI)/rate) -
              2*a0*b5*cos((8*f*C_PI)/rate))*sin((2*f*C_PI)/rate))/
              (b0*b0 + b1*b1 + b2*b2 + b3*b3 + b4*b4 + b5*b5 +
              2*(b0*b1 + b1*b2 + b2*b3 + b3*b4 + b4*b5)*cos((2*f*C_PI)/rate) +
              2*(b0*b2 + b1*b3 + b2*b4 + b3*b5)*cos((4*f*C_PI)/rate) +
              2*b0*b3*cos((6*f*C_PI)/rate) + 2*b1*b4*cos((6*f*C_PI)/rate) +
              2*b2*b5*cos((6*f*C_PI)/rate) + 2*b0*b4*cos((8*f*C_PI)/rate) +
              2*b1*b5*cos((8*f*C_PI)/rate) + 2*b0*b5*cos((10*f*C_PI)/rate)))

    @classmethod
    def lin2db(self, lin):
        return 20 * (math.log(lin) / math.log(10))

def make_rc_lopass(sample_rate, freq):
    if freq > sample_rate / 2.0:
        freq = sample_rate / 2.0

    # correction for frequency warping effect
    omega = 2 * math.pi * freq / sample_rate;
    omega = math.tan(omega / 2) * 2

    term = 1 + 1/omega;
    return BiquadFilter(1/term, 0.0, 0.0, -1.0 + 1/term, 0.0);

# XXX hipass and hiboost are a bit dubious, hipass is probably wrong
def make_rc_hipass(sample_rate, res, cap):
    rc = res * cap
    ts = 1.0 / sample_rate
    return BiquadFilter(1, -1, 0.0, -rc/(ts+rc), 0.0);

def make_rc_hiboost(sample_rate, res, cap):
    rc = res * cap
    ts = 1.0 / sample_rate
    return BiquadFilter(1 + rc/(ts+rc), 0.0, 0.0, rc/(ts+rc), 0.0);

def make_2nd_allpass(a):
    a = a * a
    return BiquadFilter(a, 0, -1, 0, -a);

# from gnuitar/src/biquad.c
def make_chebyshev_1(Fs, Fc, ripple, lowpass):
    c = -math.cos(math.pi / 4);
    v = math.sin(math.pi / 4);

    if (ripple > 0):
	t = 100.0 / (100.0 - ripple);
	x = math.sqrt(t * t - 1);
	t = 1.0 / x;
	r = t + math.sqrt(t / x);
	y = 0.5 * math.log(r + 1);
	z = 0.5 * math.log(r - 1);
	t = math.exp(z);
	z = (t + 1.0 / t) / 2.0;
	t = math.exp(y);
	c *= (t - 1.0 / t) / 2.0 / z;
	v *= (t + 1.0 / t) / 2.0 / z;
    
    tt = 2 * math.tan(0.5);
    tt2 = tt * tt;
    om = 2 * math.pi * Fc / Fs;
    m = c * c + v * v;
    d = 4.0 - 4.0 * c * tt + m * tt2;
    x0 = tt2 / d;
    y1p = (8 - 2 * m * tt2) / d;
    y2 = (-4 - 4 * c * tt - m * tt2) / d;

    if lowpass:
	k = math.sin(0.5 - om / 2.0) / math.sin(0.5 + om / 2.0);
    else:
	k = -math.cos(om / 2 + 0.5) / math.cos(om / 2 - 0.5);
    
    a0 = 1.0 + k * (y1p - y2 * k);
    b0 = (x0 - k * (2 - k) * x0)               / a0;
    b1 = 2.0 * b0;
    b2 =       b0;
    a1 = -(k * (2.0 + y1p * k - 2 * y2) + y1p) / a0;
    a2 = -(-k * (k + y1p) + y2)                / a0;

    if not lowpass:
	b1 = -b1;
	a1 = -a1;

    return BiquadFilter(b0, b1, b2, a1, a2)

# from biquad.c attributed to Tom St Denis
def make_filter(type, samplerate, center_frequency, bandwidth, db_gain):
    A = math.pow(10, db_gain / 40);
    omega = 2 * math.pi * center_frequency / samplerate;
    sn = math.sin(omega);
    cs = math.cos(omega);
    alpha = sn * math.sinh(math.log(2) / 2 * bandwidth * omega / sn);
    beta = math.sqrt(A + A);

    if type == 'LPF':
        b0 = (1 - cs) / 2;
        b1 = 1 - cs;
        b2 = (1 - cs) / 2;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
    elif type == 'HPF':
        b0 = (1 + cs) / 2;
        b1 = -(1 + cs);
        b2 = (1 + cs) / 2;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
    elif type == 'BPF':
        b0 = alpha;
        b1 = 0;
        b2 = -alpha;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
    elif type == 'NOTCH':
        b0 = 1;
        b1 = -2 * cs;
        b2 = 1;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
    elif type == 'PEQ':
        b0 = 1 + (alpha * A);
        b1 = -2 * cs;
        b2 = 1 - (alpha * A);
        a0 = 1 + (alpha / A);
        a1 = -2 * cs;
        a2 = 1 - (alpha / A);
    elif type == 'LSH':
        b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
        b1 = 2 * A * ((A - 1) - (A + 1) * cs);
        b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
        a0 = (A + 1) + (A - 1) * cs + beta * sn;
        a1 = -2 * ((A - 1) + (A + 1) * cs);
        a2 = (A + 1) + (A - 1) * cs - beta * sn;
    elif type == 'HSH':
        b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
        b1 = -2 * A * ((A - 1) + (A + 1) * cs);
        b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
        a0 = (A + 1) - (A - 1) * cs + beta * sn;
        a1 = 2 * ((A - 1) - (A + 1) * cs);
        a2 = (A + 1) - (A - 1) * cs - beta * sn;
    else:
        raise RuntimeError, "Unknown filter type. Pick one from LPF, HPF, BPF, PEQ, NOTCH, LSH, HSH"

    return BiquadFilter(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0)

def make_allpass(delay):
    if delay > 1.0 or delay < -1.0:
	raise RuntimeError, "invalid arguments"

    return BiquadFilter(delay, 1.0, 0.0, delay, 0.0)

def make_nr(length):
    if length == 2:
        return BiquadFilter(0.5, 0.5, 0.0, 0.0, 0.0)
    if length == 3:
        return BiquadFilter(0.25, 0.5, 0.25, 0.0, 0.0)
    raise RuntimeError, "only lenghts 2, 3 supported"

def make_butterworth(fs, fc):
    # a0 = a2 = omega^2 / c
    # a1 = 2 * a0
    # b1 = 2 * (omega^2 - 1)
    # b2 = (1 - 2 * cos(pi / 4) * omega + omega^2) / c
    # c = 1 + 2 * cos(pic / 4) * omega + omega^2
    omega = 2 * math.pi * fc / fs;

    c = 1 + 2 * math.cos(math.pi / 4) * omega + omega ** 2
    a0 = a2 = omega ** 2 / c
    a1 = 2 * a0
    b1 = 2 * (omega ** 2 - 1) / c
    b2 = (1 - 2 * math.cos(math.pi / 4) * omega + omega ** 2) / c
    
    return BiquadFilter(a0, a1, a2, b1, b2)

def main():
    filter1 = BiquadFilter(1, 0, 0, 0, 0)
    filter2 = BiquadFilter(1, 0, 0, 0, 0)
    filter3 = BiquadFilter(1, 0, 0, 0, 0)
    filter4 = BiquadFilter(1, 0, 0, 0, 0)
    # frequency is actually fairly irrelevant, but you can compare the
    # performance of some of the filters near 20 kHz using 44.1 kHz sampling
    # frequency if you like.
    sampling_rate_hz = int(sys.argv[1])

    # A500 / A1200 LED filter -- third order roll-off at 2.5 kHz
    #filter1 = make_rc_lopass(sampling_rate_hz, 6070)
    #filter2 = make_rc_lopass(sampling_rate_hz, 10070)
    #filter3 = make_rc_lopass(sampling_rate_hz, 10070)
    
    # A500 static filter -- first order roll-off at 5 kHz
    #filter1 = make_rc_lopass(sampling_rate_hz, 7600)
    #filter2 = make_rc_lopass(sampling_rate_hz, 80500)

    # null filter
    filter1 = make_2nd_allpass(0.9)
 
    print "#f1: b0=%f, b1=%f, b2=%f, a1=%f, a2=%f" % (filter1.b0, filter1.b1, filter1.b2, filter1.a1, filter1.a2)
    print "#f2: b0=%f, b1=%f, b2=%f, a1=%f, a2=%f" % (filter2.b0, filter2.b1, filter2.b2, filter2.a1, filter2.a2)
    print "#f3: b0=%f, b1=%f, b2=%f, a1=%f, a2=%f" % (filter3.b0, filter3.b1, filter3.b2, filter3.a1, filter3.a2)
    print "#f4: b0=%f, b1=%f, b2=%f, a1=%f, a2=%f" % (filter4.b0, filter4.b1, filter4.b2, filter4.a1, filter4.a2)

    for dp in range(90):
        freq_hz = 20 * math.pow(2, math.log(sampling_rate_hz/20./2)/math.log(2) * dp / 90.)
        power_db = BiquadFilter.lin2db(
              filter1.mag(sampling_rate_hz, freq_hz)
            * filter2.mag(sampling_rate_hz, freq_hz)
            * filter3.mag(sampling_rate_hz, freq_hz)
            * filter4.mag(sampling_rate_hz, freq_hz)
        )
        phase_ang = (
                  filter1.phi(sampling_rate_hz, freq_hz)
                 + filter2.phi(sampling_rate_hz, freq_hz)
                 + filter3.phi(sampling_rate_hz, freq_hz)
                 + filter4.phi(sampling_rate_hz, freq_hz)
        ) / math.pi * 180
        print "%8.2f %7.3f %5.1f" % (freq_hz, power_db, phase_ang)
    
if __name__ == '__main__':
    main()

