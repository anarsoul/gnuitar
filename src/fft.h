/* see FFT.c for license information */

#ifndef _FFT_H_
#define _FFT_H_ 1

typedef enum {
    FFT_FORWARD = -1,
    FFT_INVERSE =  1
} fft_direction_t;

void do_fft(float *, int, fft_direction_t);

#endif
