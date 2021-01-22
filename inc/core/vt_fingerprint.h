/* Copyright (c) Microsoft Corporation. */

#ifndef _VT_FINGERPRINT_H
#define _VT_FINGERPRINT_H

#include "vt_api.h"

typedef enum VT_CURVE_SHAPE
{
    VT_SHAPE_FALL  = 0,
    VT_SHAPE_RISE  = 1,
    VT_SHAPE_STEP  = 2,
    VT_SHAPE_NOISE = 3
} VT_CURVE_SHAPE;


//Calculate
uint _vt_fingerprint_calculate_falltime_pearsoncoefficient(uint32_t *fingerprint, uint32_t fingerprint_length, uint32_t sampling_frequency, int* fall_time, double* pearson_coefficient);
VT_CURVE_SHAPE _vt_fingerprint_calculate_shape(uint32_t* fingerprint, int fingerprint_length);
uint32_t _vt_fingerprint_calculate_maximum_index(uint32_t* fingerprint, uint32_t fingerprint_length);
uint32_t _vt_fingerprint_calculate_minimum_index(uint32_t* fingerprint, uint32_t fingerprint_length);
uint32_t _vt_fingerprint_calculate_37index(uint32_t* fingerprint, uint32_t fingerprint_length);

//Evaluate
double _vt_fingerprint_evaluate_correlationCoefficient(uint32_t* fingerprint1, uint32_t* fingerprint2, int fingeprprint_length);
double _vt_fingerprint_evaluate_nrmse(uint32_t* fingerpint1, uint32_t* fingerprint2, int fingerpint_length);

//Rankify(Utils)
void _vt_fingerprint_rankify(uint32_t* fingerprint, int fingerprint_length, uint32_t* fingerprint_ranked);

#endif