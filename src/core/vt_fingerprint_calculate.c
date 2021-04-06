/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>

#include "vt_api.h"
#include "vt_fingerprint.h"

static uint8_t _vt_fingerprint_calculate_minimum_index(uint32_t* fingerprint, uint32_t fingerprint_length);
static void _vt_fingerprint_rankify(uint32_t* fingerprint, uint8_t fingerprint_length, uint32_t* fingerprint_ranked);

static uint8_t _vt_fingerprint_calculate_minimum_index(uint32_t* fingerprint, uint32_t fingerprint_length)
{
    uint8_t index_min = 0;

    for (uint8_t i = 0; i < fingerprint_length; i++)
    {
        if (fingerprint[i] < fingerprint[index_min])
        {
            index_min = i;
        }
    }

    return index_min;
}

static void _vt_fingerprint_rankify(uint32_t* fingerprint, uint8_t fingerprint_length, uint32_t* fingerprint_ranked)
{

    for (uint8_t i = 0; i < fingerprint_length; i++)
    {
        uint8_t r = 1;
        uint8_t s = 1;

        // Count no of smaller elements
        // in 0 to i-1
        for (uint8_t j = 0; j < i; j++)
        {
            if (fingerprint[j] < fingerprint[i])
            {
                r++;
            }

            if (fingerprint[j] == fingerprint[i])
            {
                s++;
            }
        }

        // Count no of smaller elements
        // in i+1 to N-1
        for (uint8_t j = i + 1; j < fingerprint_length; j++)
        {
            if (fingerprint[j] < fingerprint[i])
            {
                r++;
            }
            if (fingerprint[j] == fingerprint[i])
            {
                s++;
            }
        }

        // Use Fractional Rank formula
        // fractional_rank = r + (n-1)/2
        fingerprint_ranked[i] = r + (s - 1) * 0.5;
    }
}

uint32_t _vt_fingerprint_calculate_falltime_pearsoncoefficient(uint32_t* fingerprint,
    uint32_t fingerprint_length,
    uint32_t sampling_frequency,
    uint32_t* fall_time,
    float* pearson_coefficient)
{

    // Find index of  Maxima
    uint8_t index_max = _vt_fingerprint_calculate_maximum_index(fingerprint, fingerprint_length);

    // Delete data BEFORE the maxima
    fingerprint        = fingerprint + index_max;
    fingerprint_length = fingerprint_length - index_max;

    // Find index of  Minima
    uint8_t index_min = _vt_fingerprint_calculate_minimum_index(fingerprint, fingerprint_length);
    uint32_t minima   = fingerprint[index_min];

    // Subtract minima from fingerprint
    uint8_t i = 0;
    for (i = 0; i < fingerprint_length; i++)
        fingerprint[i] = fingerprint[i] - minima;

    // Find data point whose value falls below 37% of maxima
    uint8_t index_37 = _vt_fingerprint_calculate_37index(fingerprint, fingerprint_length);

    // fingerprint_length of this new fingerprint
    if (index_37 != 255)
    {
        fingerprint_length = index_37 + 1;
    }

    // N > 2 ?
    if (fingerprint_length > 2)
    {
        // Calculate FallTime
        *fall_time = fingerprint_length * sampling_frequency;

        // Reconstruct exponential fall for the N points
        uint32_t fingerprint_reconstructed[100];
        for (i = 0; i < fingerprint_length; i++)
            fingerprint_reconstructed[i] = fingerprint[0] * (float)exp((-1 * (i / (float)(fingerprint_length - 1))));

        // Calculate pearson coefficient
        *pearson_coefficient =
            _vt_fingerprint_evaluate_correlation_coefficient(fingerprint_reconstructed, fingerprint, fingerprint_length);

        return VT_SUCCESS;
    }

    return VT_ERROR;
}

VT_CURVE_SHAPE _vt_fingerprint_calculate_shape(uint32_t* fingerprint, uint8_t fingerprint_length)
{
    uint32_t ranked[VT_FINGERPRINT_LENGTH];
    uint32_t arraylinear[VT_FINGERPRINT_LENGTH];
    uint32_t arrayexp[VT_FINGERPRINT_LENGTH];
    uint8_t i;

    _vt_fingerprint_rankify(fingerprint, fingerprint_length, ranked);

    for (i = 0; i < fingerprint_length; i++)
    {
        arraylinear[i] = fingerprint_length - i;
        arrayexp[i]    = (uint32_t)(1000 * (exp((-1 * (i / (float)(fingerprint_length - 1))))));
    }

    if (_vt_fingerprint_evaluate_correlation_coefficient(ranked, arraylinear, fingerprint_length) > VT_SHAPE_THRESHOLD ||
        _vt_fingerprint_evaluate_correlation_coefficient(fingerprint, arrayexp, fingerprint_length) > VT_SHAPE_THRESHOLD)
    {
        return VT_SHAPE_FALL;
    }

    for (i = 0; i < fingerprint_length; i++)
    {
        arraylinear[i] = i;
        arrayexp[i]    = (uint32_t)(1000 * (1 - exp((-1 * (i / (float)(fingerprint_length - 1))))));
    }
    if (_vt_fingerprint_evaluate_correlation_coefficient(ranked, arraylinear, fingerprint_length) > VT_SHAPE_THRESHOLD ||
        _vt_fingerprint_evaluate_correlation_coefficient(fingerprint, arrayexp, fingerprint_length) > VT_SHAPE_THRESHOLD)
    {
        return VT_SHAPE_RISE;
    }

    arraylinear[0] = fingerprint_length;
    for (i = 1; i < fingerprint_length; i++)
        arraylinear[i] = 1;

    if (_vt_fingerprint_evaluate_correlation_coefficient(fingerprint, arraylinear, fingerprint_length) >
        VT_SHAPE_THRESHOLD)
    {
        return VT_SHAPE_STEP;
    }

    return VT_SHAPE_NOISE;
}

uint8_t _vt_fingerprint_calculate_maximum_index(uint32_t* fingerprint, uint32_t fingerprint_length)
{
    uint32_t index_max = 0;

    for (uint32_t i = 0; i < fingerprint_length; i++)
    {
        if (fingerprint[i] > fingerprint[index_max])
        {
            index_max = i;
        }
    }

    return index_max;
}

uint8_t _vt_fingerprint_calculate_37index(uint32_t* fingerprint, uint32_t fingerprint_length)
{
    uint8_t index_max = _vt_fingerprint_calculate_maximum_index(fingerprint, fingerprint_length);

    for (uint8_t i = index_max; i < fingerprint_length; i++)
    {
        if (fingerprint[i] <= (0.37 * fingerprint[index_max]))
        {
            return i;
        }
    }

    return 255;
}
