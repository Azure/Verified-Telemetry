/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>

#include "vt_debug.h"
#include "vt_fc_read.h"
#include "vt_fc_signature.h"

static VT_UINT fc_signature_calculate_maximum_index(VT_UINT* raw_signature, VT_UINT sample_length)
{
    VT_UINT index_max = 0;

    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        if (raw_signature[iter] > raw_signature[index_max])
        {
            index_max = iter;
        }
    }

    return index_max;
}

static VT_INT fc_signature_calculate_37index(VT_UINT* raw_signature, VT_UINT sample_length)
{

    for (VT_UINT iter = 1; iter < sample_length; iter++)
    {
        if ((VT_FLOAT)raw_signature[iter] <= (VT_FLOAT)(0.37f * (VT_FLOAT)raw_signature[0]))
        {
            return iter;
        }
    }

    return -1;
}

static VT_FLOAT fc_signature_calculate_correlation_coefficient(VT_UINT* signature1, VT_UINT* signature2, VT_UINT sample_length)
{
    VT_FLOAT sum_signature1            = 0;
    VT_FLOAT sum_signature2            = 0;
    VT_FLOAT sum_signature1_signature2 = 0;
    VT_FLOAT square_sum_signature1     = 0;
    VT_FLOAT square_sum_signature2     = 0;

    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        // sum of elements of array signature1.
        sum_signature1 += signature1[iter];

        // sum of elements of array signature2.
        sum_signature2 += signature2[iter];

        // sum of signature1[i] * signature2[i].
        sum_signature1_signature2 += (signature1[iter] * signature2[iter]);

        // sum of square of array elements.
        square_sum_signature1 += (signature1[iter] * signature1[iter]);
        square_sum_signature2 += (signature2[iter] * signature2[iter]);
    }

    // use formula for calculating correlation coefficient.
    VT_FLOAT corr = ((VT_FLOAT)sample_length * sum_signature1_signature2 - sum_signature1 * sum_signature2) /
                    sqrtf(((VT_FLOAT)sample_length * square_sum_signature1 - sum_signature1 * sum_signature1) *
                          ((VT_FLOAT)sample_length * square_sum_signature2 - sum_signature2 * sum_signature2));

    return corr;
}

VT_UINT fc_signature_compute(
    VT_FALLCURVE_OBJECT* fc_object, VT_ULONG sampling_interval_us, VT_ULONG* falltime, VT_FLOAT* pearson_coeff)
{
    VT_UINT raw_signature[VT_FC_SAMPLE_LENGTH] = {0};
    VT_UINT sample_length                      = VT_FC_SAMPLE_LENGTH;
    VT_ULONG falltime_computed                 = 0;
    VT_FLOAT pearson_coeff_computed            = 0;
    fc_adc_read(fc_object, raw_signature, sampling_interval_us, VT_FC_SAMPLE_LENGTH);

    VTLogDebug("FallCurve Raw: \r\n");
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        VTLogDebugNoTag("%d, ", raw_signature[iter]);
    }
    VTLogDebugNoTag("\r\n");

    // Find index of Maxima
    VT_UINT index_max = fc_signature_calculate_maximum_index(raw_signature, VT_FC_SAMPLE_LENGTH);
    // Delete data BEFORE the maxima
    sample_length = sample_length - index_max;
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        raw_signature[iter] = raw_signature[iter + index_max];
    }
    // Find datapoint which reaches 37% of the starting value
    VT_INT index_37 = fc_signature_calculate_37index(raw_signature, sample_length);
    // fingerprint_length of this new fingerprint
    if (index_37 > 0)
    {
        sample_length = index_37 + 1;
    }
    // Calculate FallTime
    falltime_computed = (VT_ULONG)sample_length * sampling_interval_us;
    VTLogDebug("FallTime computed: %lu\r\n", falltime_computed);

    // Reconstruct exponential fall for the N points
    VT_UINT perfect_exponential_raw_siganture[VT_FC_SAMPLE_LENGTH];
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        perfect_exponential_raw_siganture[iter] =
            round((VT_FLOAT)raw_signature[0] * (VT_FLOAT)exp(-1.0f * ((VT_FLOAT)iter / (VT_FLOAT)(sample_length - 1))));
    }
    // Calculate pearson coefficient
    pearson_coeff_computed =
        fc_signature_calculate_correlation_coefficient(perfect_exponential_raw_siganture, raw_signature, sample_length);
#if VT_LOG_LEVEL > 2
    VT_INT32 decimal    = pearson_coeff_computed;
    VT_FLOAT frac_float = pearson_coeff_computed - (VT_FLOAT)decimal;
    VT_INT32 frac       = frac_float * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Pearson Coeff: %lu.%lu \r\n", decimal, frac);

    if (sample_length > VT_FC_MIN_FALLTIME_DATAPOINTS && pearson_coeff_computed > VT_FC_MIN_SHAPE_MATCH)
    {
        *falltime      = falltime_computed;
        *pearson_coeff = pearson_coeff_computed;
        return VT_SUCCESS;
    }

    return VT_ERROR;
}