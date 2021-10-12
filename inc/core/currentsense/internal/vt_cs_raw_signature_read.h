/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_RAW_SIGNATURE_READ_H
#define _VT_CS_RAW_SIGNATURE_READ_H

#include "vt_cs_api.h"
#include "vt_defs.h"


VT_UINT cs_raw_signature_read(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* repeating_signature_sampling_frequencies,
    VT_UINT num_repeating_signature_sampling_frequencies,
    VT_UINT sample_length);

VT_UINT cs_repeating_raw_signature_fetch_stored_current_measurement(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* repeating_raw_signature, VT_FLOAT sampling_frequency, VT_UINT sample_length);

VT_UINT cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* extrapolated_repeating_raw_signature,
    VT_FLOAT desired_sampling_frequency,
    VT_UINT desired_sample_length);

VT_UINT cs_non_repeating_raw_signature_fetch_stored_current_measurement(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* non_repeating_raw_signature,
    VT_FLOAT* sampling_frequency,
    VT_UINT* num_datapoints);

#endif
