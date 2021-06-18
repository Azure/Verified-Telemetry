/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_fft.h"
#include "math.h"

static const COMPLEX w[128] = {{1.00000f, -0.00000f},
    {0.99970f, -0.02454f},
    {0.99880f, -0.04907f},
    {0.99729f, -0.07356f},
    {0.99518f, -0.09802f},
    {0.99248f, -0.12241f},
    {0.98918f, -0.14673f},
    {0.98528f, -0.17096f},
    {0.98079f, -0.19509f},
    {0.97570f, -0.21910f},
    {0.97003f, -0.24298f},
    {0.96378f, -0.26671f},
    {0.95694f, -0.29028f},
    {0.94953f, -0.31368f},
    {0.94154f, -0.33689f},
    {0.93299f, -0.35990f},
    {0.92388f, -0.38268f},
    {0.91421f, -0.40524f},
    {0.90399f, -0.42756f},
    {0.89322f, -0.44961f},
    {0.88192f, -0.47140f},
    {0.87009f, -0.49290f},
    {0.85773f, -0.51410f},
    {0.84485f, -0.53500f},
    {0.83147f, -0.55557f},
    {0.81758f, -0.57581f},
    {0.80321f, -0.59570f},
    {0.78835f, -0.61523f},
    {0.77301f, -0.63439f},
    {0.75721f, -0.65317f},
    {0.74095f, -0.67156f},
    {0.72425f, -0.68954f},
    {0.70711f, -0.70711f},
    {0.68954f, -0.72425f},
    {0.67156f, -0.74095f},
    {0.65317f, -0.75721f},
    {0.63439f, -0.77301f},
    {0.61523f, -0.78835f},
    {0.59570f, -0.80321f},
    {0.57581f, -0.81758f},
    {0.55557f, -0.83147f},
    {0.53500f, -0.84485f},
    {0.51410f, -0.85773f},
    {0.49290f, -0.87009f},
    {0.47140f, -0.88192f},
    {0.44961f, -0.89322f},
    {0.42756f, -0.90399f},
    {0.40524f, -0.91421f},
    {0.38268f, -0.92388f},
    {0.35990f, -0.93299f},
    {0.33689f, -0.94154f},
    {0.31368f, -0.94953f},
    {0.29028f, -0.95694f},
    {0.26671f, -0.96378f},
    {0.24298f, -0.97003f},
    {0.21910f, -0.97570f},
    {0.19509f, -0.98079f},
    {0.17096f, -0.98528f},
    {0.14673f, -0.98918f},
    {0.12241f, -0.99248f},
    {0.09802f, -0.99518f},
    {0.07356f, -0.99729f},
    {0.04907f, -0.99880f},
    {0.02454f, -0.99970f},
    {-0.00000f, -1.00000f},
    {-0.02454f, -0.99970f},
    {-0.04907f, -0.99880f},
    {-0.07356f, -0.99729f},
    {-0.09802f, -0.99518f},
    {-0.12241f, -0.99248f},
    {-0.14673f, -0.98918f},
    {-0.17096f, -0.98528f},
    {-0.19509f, -0.98079f},
    {-0.21910f, -0.97570f},
    {-0.24298f, -0.97003f},
    {-0.26671f, -0.96378f},
    {-0.29028f, -0.95694f},
    {-0.31368f, -0.94953f},
    {-0.33689f, -0.94154f},
    {-0.35990f, -0.93299f},
    {-0.38268f, -0.92388f},
    {-0.40524f, -0.91421f},
    {-0.42756f, -0.90399f},
    {-0.44961f, -0.89322f},
    {-0.47140f, -0.88192f},
    {-0.49290f, -0.87009f},
    {-0.51410f, -0.85773f},
    {-0.53500f, -0.84485f},
    {-0.55557f, -0.83147f},
    {-0.57581f, -0.81758f},
    {-0.59570f, -0.80321f},
    {-0.61523f, -0.78835f},
    {-0.63439f, -0.77301f},
    {-0.65317f, -0.75721f},
    {-0.67156f, -0.74095f},
    {-0.68954f, -0.72425f},
    {-0.70711f, -0.70711f},
    {-0.72425f, -0.68954f},
    {-0.74095f, -0.67156f},
    {-0.75721f, -0.65317f},
    {-0.77301f, -0.63439f},
    {-0.78835f, -0.61523f},
    {-0.80321f, -0.59570f},
    {-0.81758f, -0.57581f},
    {-0.83147f, -0.55557f},
    {-0.84485f, -0.53500f},
    {-0.85773f, -0.51410f},
    {-0.87009f, -0.49290f},
    {-0.88192f, -0.47140f},
    {-0.89322f, -0.44961f},
    {-0.90399f, -0.42756f},
    {-0.91421f, -0.40524f},
    {-0.92388f, -0.38268f},
    {-0.93299f, -0.35990f},
    {-0.94154f, -0.33689f},
    {-0.94953f, -0.31368f},
    {-0.95694f, -0.29028f},
    {-0.96378f, -0.26671f},
    {-0.97003f, -0.24298f},
    {-0.97570f, -0.21910f},
    {-0.98079f, -0.19509f},
    {-0.98528f, -0.17096f},
    {-0.98918f, -0.14673f},
    {-0.99248f, -0.12241f},
    {-0.99518f, -0.09802f},
    {-0.99729f, -0.07356f},
    {-0.99880f, -0.04907f},
    {-0.99970f, -0.02454f}};

static VT_FLOAT sq(VT_FLOAT input)
{
    VT_FLOAT output = 0;
    output          = input * input;
    return output;
}

VT_VOID cs_fft_compute(COMPLEX* Y, VT_UINT N) /*input sample array, # of points      */
{
    COMPLEX temp1, temp2;        /*temporary storage variables          */
    VT_INT i, j, k;              /*loop counter variables               */
    VT_INT upper_leg, lower_leg; /*index of upper/lower butterfly leg   */
    VT_INT leg_diff;             /*difference between upper/lower leg   */
    VT_INT num_stages = 0;       /*number of FFT stages, or iterations  */
    VT_INT index, step;          /*index and step between twiddle factor*/

    /* log(base 2) of # of points = # of stages  */
    i = 1;
    do
    {
        num_stages += 1;
        i = i * 2;
    } while (i != N);

    /* starting difference between upper and lower butterfly legs*/
    leg_diff = N / 2;
    /* step between values in twiddle factor array twiddle.h     */
    step = 256 / N;
    /* For N-point FFT                                           */

    for (i = 0; i < num_stages; i++)
    {
        index = 0;
        for (j = 0; j < leg_diff; j++)
        {
            for (upper_leg = j; upper_leg < N; upper_leg += (2 * leg_diff))
            {
                lower_leg           = upper_leg + leg_diff;
                temp1.real          = (Y[upper_leg]).real + (Y[lower_leg]).real;
                temp1.imag          = (Y[upper_leg]).imag + (Y[lower_leg]).imag;
                temp2.real          = (Y[upper_leg]).real - (Y[lower_leg]).real;
                temp2.imag          = (Y[upper_leg]).imag - (Y[lower_leg]).imag;
                (Y[lower_leg]).real = temp2.real * (w[index]).real - temp2.imag * (w[index]).imag;
                (Y[lower_leg]).imag = temp2.real * (w[index]).imag + temp2.imag * (w[index]).real;
                (Y[upper_leg]).real = temp1.real;
                (Y[upper_leg]).imag = temp1.imag;
            }
            index += step;
        }
        leg_diff = leg_diff / 2;
        step *= 2;
    }
    /* bit reversal for resequencing data */
    j = 0;
    for (i = 1; i < (N - 1); i++)
    {
        k = N / 2;
        while (k <= j)
        {
            j = j - k;
            k = k / 2;
        }
        j = j + k;
        if (i < j)
        {
            temp1.real  = (Y[j]).real;
            temp1.imag  = (Y[j]).imag;
            (Y[j]).real = (Y[i]).real;
            (Y[j]).imag = (Y[i]).imag;
            (Y[i]).real = temp1.real;
            (Y[i]).imag = temp1.imag;
        }
    }
    return;
}

VT_VOID cs_fft_complex_to_magnitude(COMPLEX* Y, VT_UINT N)
{
    for (VT_UINT i = 0; i < N; i++)
    {
        Y[i].real = sqrt(sq(Y[i].real) + sq(Y[i].imag));
    }
}

VT_VOID cs_fft_normalize(COMPLEX* Y, VT_UINT N)
{
    VT_FLOAT maxY       = 1;
    VT_UINT IndexOfMaxY = 1;
    for (VT_UINT i = 1; i < ((N >> 1) + 1); i++)
    {
        if (Y[i].real > Y[IndexOfMaxY].real)
        {
            IndexOfMaxY = i;
        }
    }
    maxY = Y[IndexOfMaxY].real;
    if (maxY == 0)
    {
        return;
    }
    for (VT_UINT i = 1; i < ((N >> 1) + 1); i++)
    {
        Y[i].real /= maxY;
    }
}

VT_VOID cs_fft_dc_removal(COMPLEX* Y, VT_UINT N)
{
    // calculate the mean of vData
    VT_FLOAT mean = 0;
    for (VT_UINT i = 0; i < N; i++)
    {
        mean += Y[i].real;
    }
    mean /= (VT_FLOAT)N;
    // Subtract the mean from vData
    for (VT_UINT i = 0; i < N; i++)
    {
        Y[i].real -= mean;
    }
}

VT_VOID cs_fft_windowing(COMPLEX* Y, VT_UINT N, VT_UINT8 windowType, VT_UINT8 dir)
{
    // Weighing factors are computed once before multiple use of FFT
    // The weighing function is symetric; half the weighs are recorded
    VT_FLOAT samplesMinusOne = ((VT_FLOAT)N - 1.0f);
    for (VT_UINT i = 0; i < (N >> 1); i++)
    {
        VT_FLOAT indexMinusOne  = (VT_FLOAT)i;
        VT_FLOAT ratio          = (indexMinusOne / samplesMinusOne);
        VT_FLOAT weighingFactor = 1.0f;
        // Compute and record weighting factor
        switch (windowType)
        {
            case FFT_WIN_TYP_RECTANGLE: // rectangle (box car)
                weighingFactor = 1.0f;
                break;
            case FFT_WIN_TYP_HAMMING: // hamming
                weighingFactor = 0.54f - (0.46f * (VT_FLOAT)cos(twoPi * ratio));
                break;
            case FFT_WIN_TYP_HANN: // hann
                weighingFactor = 0.54f * (1.0f - (VT_FLOAT)cos(twoPi * ratio));
                break;
            case FFT_WIN_TYP_TRIANGLE: // triangle (Bartlett)
                weighingFactor = 1.0f - ((2.0f * (VT_FLOAT)fabs(indexMinusOne - (samplesMinusOne / 2.0f))) / samplesMinusOne);
                break;
            case FFT_WIN_TYP_NUTTALL: // nuttall
                weighingFactor = 0.355768f - (0.487396f * ((VT_FLOAT)cos(twoPi * ratio))) +
                                 (0.144232f * ((VT_FLOAT)cos(fourPi * ratio))) - (0.012604f * ((VT_FLOAT)cos(sixPi * ratio)));
                break;
            case FFT_WIN_TYP_BLACKMAN: // blackman
                weighingFactor =
                    0.42323f - (0.49755f * ((VT_FLOAT)cos(twoPi * ratio))) + (0.07922f * ((VT_FLOAT)cos(fourPi * ratio)));
                break;
            case FFT_WIN_TYP_BLACKMAN_NUTTALL: // blackman nuttall
                weighingFactor = 0.3635819f - (0.4891775f * ((VT_FLOAT)cos(twoPi * ratio))) +
                                 (0.1365995f * ((VT_FLOAT)cos(fourPi * ratio))) - (0.0106411f * ((VT_FLOAT)cos(sixPi * ratio)));
                break;
            case FFT_WIN_TYP_BLACKMAN_HARRIS: // blackman harris
                weighingFactor = 0.35875f - (0.48829f * ((VT_FLOAT)cos(twoPi * ratio))) +
                                 (0.14128f * ((VT_FLOAT)cos(fourPi * ratio))) - (0.01168f * ((VT_FLOAT)cos(sixPi * ratio)));
                break;
            case FFT_WIN_TYP_FLT_TOP: // flat top
                weighingFactor =
                    0.2810639f - (0.5208972f * (VT_FLOAT)cos(twoPi * ratio)) + (0.1980399f * (VT_FLOAT)cos(fourPi * ratio));
                break;
            case FFT_WIN_TYP_WELCH: // welch
                weighingFactor = 1.0f - sq((indexMinusOne - samplesMinusOne / 2.0f) / (samplesMinusOne / 2.0f));
                break;
        }
        if (dir == FFT_FORWARD)
        {
            Y[i].real *= weighingFactor;
            Y[N - (i + 1)].real *= weighingFactor;
        }
        else
        {
            Y[i].real /= weighingFactor;
            Y[N - (i + 1)].real /= weighingFactor;
        }
    }
}

VT_VOID cs_fft_major_peak(COMPLEX* Y, VT_UINT N, VT_FLOAT sampling_freq, VT_FLOAT* f, VT_FLOAT* v, VT_INT* index)
{
    VT_FLOAT maxY       = 0;
    VT_UINT IndexOfMaxY = 0;
    // If sampling_frequency = 2 * max_frequency in signal,
    // value would be stored at position samples/2
    for (VT_UINT i = 1; i < ((N >> 1) + 1); i++)
    {
        if ((Y[i - 1].real < Y[i].real) && (Y[i].real > Y[i + 1].real))
        {
            if (Y[i].real > maxY)
            {
                maxY        = Y[i].real;
                IndexOfMaxY = i;
            }
        }
    }
    VT_FLOAT delta         = 0.5f * ((Y[IndexOfMaxY - 1].real - Y[IndexOfMaxY + 1].real) /
                                (Y[IndexOfMaxY - 1].real - (2.0f * Y[IndexOfMaxY].real) + Y[IndexOfMaxY + 1].real));
    VT_FLOAT interpolatedX = ((IndexOfMaxY + delta) * sampling_freq) / (N - 1);
    if (IndexOfMaxY == (N >> 1)) // To improve calculation on edge values
        interpolatedX = ((IndexOfMaxY + delta) * sampling_freq) / (N);
    // returned value: interpolated frequency peak apex
    *f     = interpolatedX;
    *v     = (VT_FLOAT)(fabs(Y[IndexOfMaxY - 1].real - (2.0f * Y[IndexOfMaxY].real) + Y[IndexOfMaxY + 1].real));
    *index = IndexOfMaxY;
}
