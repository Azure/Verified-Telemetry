/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>

#include "vt_fingerprint.h"

float _vt_fingerprint_evaluate_correlation_coefficient(
    uint32_t* fingerpint1, uint32_t* fingerpint2, uint8_t fingerprint_length)
{
    float sum_fingerprint1             = 0;
    float sum_fingerprint2             = 0;
    float sum_fingerprint1fingerprint2 = 0;
    float squareSum_fingerprint1       = 0;
    float squareSum_fingerprint2       = 0;

    for (uint8_t i = 0; i < fingerprint_length; i++)
    {
        // sum of elements of array fingerpint1.
        sum_fingerprint1 = sum_fingerprint1 + fingerpint1[i];

        // sum of elements of array fingerpint2.
        sum_fingerprint2 = sum_fingerprint2 + fingerpint2[i];

        // sum of fingerpint1[i] * fingerpint2[i].
        sum_fingerprint1fingerprint2 = sum_fingerprint1fingerprint2 + fingerpint1[i] * fingerpint2[i];

        // sum of square of array elements.
        squareSum_fingerprint1 = squareSum_fingerprint1 + fingerpint1[i] * fingerpint1[i];
        squareSum_fingerprint2 = squareSum_fingerprint2 + fingerpint2[i] * fingerpint2[i];
    }

    // use formula for calculating
    // correlation coefficient.
    float corr = (float)(fingerprint_length * sum_fingerprint1fingerprint2 - sum_fingerprint1 * sum_fingerprint2) /
                 sqrtf((fingerprint_length * squareSum_fingerprint1 - sum_fingerprint1 * sum_fingerprint1) *
                       (fingerprint_length * squareSum_fingerprint2 - sum_fingerprint2 * sum_fingerprint2));

    return corr;
}

float _vt_fingerprint_evaluate_nrmse(uint32_t* fingerpint1, uint32_t* fingerpint2, uint8_t length)
{
    float nrmse;
    uint32_t diff;

    uint32_t aggregate   = 0;
    uint32_t aggregatesq = 0;

    uint32_t mean;
    float meansq;
    float rmse;

    for (uint8_t iter1 = 0; iter1 < length; iter1++)
    {
        diff        = fingerpint1[iter1] - fingerpint2[iter1];
        aggregate   = aggregate + fingerpint1[iter1];
        aggregatesq = aggregatesq + diff * diff;
    }

    mean   = aggregate / length;
    meansq = aggregatesq / length;
    rmse   = sqrtf(meansq);
    nrmse  = rmse / mean;

    return nrmse;
}
