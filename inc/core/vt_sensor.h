/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_SENSOR_H
#define _VT_SENSOR_H

#include "vt_api.h"
#include "vt_fingerprint.h"

typedef enum VT_FALL_STATE
{
    //VT_FALL_STATE_INIT       = 0,
    VT_FALL_STATE_OVERSHOOT  = 1,
    VT_FALL_STATE_UNDERSHOOT = 2,
    VT_FALL_STATE_TARGET     = 3
} VT_FALL_STATE;

typedef enum VT_NOISE_STATE
{
    //VT_NOISE_STATE_INIT = 0,
    VT_NOISE_STATE_MAX  = 1,
    VT_NOISE_STATE_MID  = 2,
    VT_NOISE_STATE_MIN  = 3,
    VT_NOISE_STATE_EXIT = 4
} VT_NOISE_STATE;

typedef struct VT_STATE_BLOCK
{
    int previous_sampling_frequency;
    VT_CURVE_SHAPE previous_shape;
    uint32_t previous_fingerprint[VT_FINGERPRINT_LENGTH];

    int current_sampling_frequency;
    VT_CURVE_SHAPE current_shape;
    uint32_t current_fingerprint[VT_FINGERPRINT_LENGTH];

    VT_NOISE_STATE noise_state;
} VT_STATE_BLOCK;


uint32_t  _vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, int sampling_frequency);

#endif