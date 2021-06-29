/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_SIGNATURE_FEATURES_H
#define _VT_CS_SIGNATURE_FEATURES_H

#include "vt_cs_api.h"
#include "vt_defs.h"

VT_UINT cs_repeating_signature_feature_vector_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* relative_current_draw);
VT_UINT cs_repeating_signature_offset_current_compute(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT lowest_sample_freq, VT_FLOAT* offset_current);
VT_UINT cs_non_repeating_signature_average_current_compute(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* avg_curr_on, VT_FLOAT* avg_curr_off);
VT_FLOAT cs_repeating_signature_feature_vector_evaluate(VT_FLOAT signature_frequency_under_test,
    VT_FLOAT signature_frequency_saved,
    VT_FLOAT duty_cycle_under_test,
    VT_FLOAT duty_cycle_saved,
    VT_FLOAT relative_current_draw_under_test,
    VT_FLOAT relative_current_draw_saved);
VT_FLOAT cs_repeating_signature_offset_current_evaluate(VT_FLOAT offset_current_under_test, VT_FLOAT offset_current_saved);
VT_FLOAT cs_non_repeating_signature_average_current_evaluate(
    VT_FLOAT avg_curr_on_under_test, VT_FLOAT avg_curr_on_saved, VT_FLOAT avg_curr_off_under_test, VT_FLOAT avg_curr_off_saved);

#endif