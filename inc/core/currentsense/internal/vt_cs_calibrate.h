/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_CALIBRATE_H
#define _VT_CS_CALIBRATE_H

#include "vt_cs_api.h"
#include "vt_defs.h"

VT_VOID cs_calibrate_repeating_signatures_compute_sampling_frequencies(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* sampling_frequencies,
    VT_UINT sampling_frequencies_buffer_length,
    VT_UINT* num_sampling_frequencies);

VT_VOID cs_calibrate_repeating_signatures_compute_collection_settings(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* top_N_sample_frequencies, VT_FLOAT* lowest_sample_freq);

VT_VOID cs_calibrate_sensor(VT_CURRENTSENSE_OBJECT* cs_object);

VT_VOID cs_recalibrate_sensor(VT_CURRENTSENSE_OBJECT* cs_object);

#endif