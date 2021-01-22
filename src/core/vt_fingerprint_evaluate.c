/* Copyright (c) Microsoft Corporation. */

#include "vt_fingerprint.h"

double _vt_fingerprint_evaluate_correlationCoefficient(uint32_t* fingerpint1, uint32_t* fingerpint2, int length)
{
    int n       = length;
    float sum_fingerpint1 = 0, sum_fingerpint2 = 0, sum_fingerpint1fingerpint2 = 0;
    float squareSum_fingerpint1 = 0, squareSum_fingerpint2 = 0;

    for (int i = 0; i < n; i++)
    {
        // sum of elements of array fingerpint1.
        sum_fingerpint1 = sum_fingerpint1 + fingerpint1[i];

        // sum of elements of array fingerpint2.
        sum_fingerpint2 = sum_fingerpint2 + fingerpint2[i];

        // sum of fingerpint1[i] * fingerpint2[i].
        sum_fingerpint1fingerpint2 = sum_fingerpint1fingerpint2 + fingerpint1[i] * fingerpint2[i];

        // sum of square of array elements.
        squareSum_fingerpint1 = squareSum_fingerpint1 + fingerpint1[i] * fingerpint1[i];
        squareSum_fingerpint2 = squareSum_fingerpint2 + fingerpint2[i] * fingerpint2[i];
    }

    // use formula for calculating
    // correlation coefficient.
    double corr = (double)(n * sum_fingerpint1fingerpint2 - sum_fingerpint1 * sum_fingerpint2) /
                  sqrt((n * squareSum_fingerpint1 - sum_fingerpint1 * sum_fingerpint1) * (n * squareSum_fingerpint2 - sum_fingerpint2 * sum_fingerpint2));

    return corr;
}

double _vt_fingerprint_evaluate_nrmse(uint32_t* fingerpint1, uint32_t* fingerpint2, int length)
{
    double nrmse;
    uint32_t diff;

    uint32_t aggregate = 0;
    uint32_t aggregatesq = 0;

    uint32_t mean;
    uint32_t meansq;
    double rmse;

    for (int iter1 = 0; iter1 < length; iter1++)
    {
        diff        = fingerpint1[iter1] - fingerpint2[iter1];
        aggregate   = aggregate + fingerpint1[iter1];
        aggregatesq = aggregatesq + diff * diff;
    }

    mean   = (aggregate) / length;
    meansq = (aggregatesq) / length;
    rmse   = sqrt((double)meansq);
    nrmse  = rmse / mean;

    return nrmse;
}