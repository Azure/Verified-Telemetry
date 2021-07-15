/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_FFT_H
#define _VT_CS_FFT_H

#include "vt_defs.h"

#define FFT_FORWARD 0x01
#define FFT_REVERSE 0x00

/* Windowing type */
#define FFT_WIN_TYP_RECTANGLE        0x00 /* rectangle (Box car) */
#define FFT_WIN_TYP_HAMMING          0x01 /* hamming */
#define FFT_WIN_TYP_HANN             0x02 /* hann */
#define FFT_WIN_TYP_TRIANGLE         0x03 /* triangle (Bartlett) */
#define FFT_WIN_TYP_NUTTALL          0x04 /* nuttall */
#define FFT_WIN_TYP_BLACKMAN         0x05 /* blackman */
#define FFT_WIN_TYP_BLACKMAN_NUTTALL 0x06 /* blackman nuttall */
#define FFT_WIN_TYP_BLACKMAN_HARRIS  0x07 /* blackman harris*/
#define FFT_WIN_TYP_FLT_TOP          0x08 /* flat top */
#define FFT_WIN_TYP_WELCH            0x09 /* welch */

/*Mathematial constants*/
#define twoPi  6.28318531f
#define fourPi 12.56637061f
#define sixPi  18.84955593f

typedef struct
{
    VT_FLOAT real;
    VT_FLOAT imag;
} COMPLEX;

VT_VOID cs_fft_compute(COMPLEX* Y, VT_UINT N);
VT_VOID cs_fft_complex_to_magnitude(COMPLEX* Y, VT_UINT N);
VT_VOID cs_fft_normalize(COMPLEX* Y, VT_UINT N);
VT_VOID cs_fft_dc_removal(COMPLEX* Y, VT_UINT N);
VT_VOID cs_fft_windowing(COMPLEX* Y, VT_UINT N, VT_UINT8 windowType, VT_UINT8 dir);
VT_VOID cs_fft_major_peak(COMPLEX* Y, VT_UINT N, VT_FLOAT sampling_freq, VT_FLOAT* f, VT_FLOAT* v, VT_INT* index);

#endif
