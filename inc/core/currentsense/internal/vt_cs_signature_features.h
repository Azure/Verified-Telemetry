/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_SIGNATURE_FEATURES_H
#define _VT_CS_SIGNATURE_FEATURES_H

#include "vt_cs_api.h"
#include "vt_defs.h"

VT_UINT cs_repeating_signature_feature_vector_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* relative_current_draw,
    VT_FLOAT* current_cluster_1_standby,
    VT_FLOAT* current_cluster_2_active,
    VT_FLOAT* current_average);
VT_UINT cs_repeating_signature_offset_current_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT* offset_current);
VT_UINT cs_non_repeating_signature_average_current_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT* avg_curr_on,
    VT_FLOAT* avg_curr_off);
VT_FLOAT cs_repeating_signature_feature_vector_evaluate(VT_FLOAT* signature_frequency_under_test,
    VT_FLOAT signature_frequency_saved,
    VT_FLOAT duty_cycle_under_test,
    VT_FLOAT duty_cycle_saved,
    VT_FLOAT relative_current_draw_under_test,
    VT_FLOAT relative_current_draw_saved,
    VT_FLOAT sec_sig_freq);
VT_FLOAT cs_repeating_signature_offset_current_evaluate(VT_FLOAT offset_current_under_test, VT_FLOAT offset_current_saved);
VT_FLOAT cs_repeating_signature_relative_current_evaluate(VT_FLOAT offset_current_under_test, VT_FLOAT offset_current_saved,VT_FLOAT average_current_under_test, VT_FLOAT average_current_saved);
VT_FLOAT cs_non_repeating_signature_average_current_evaluate(
    VT_FLOAT avg_curr_on_under_test, VT_FLOAT avg_curr_on_saved, VT_FLOAT avg_curr_off_under_test, VT_FLOAT avg_curr_off_saved, VT_FLOAT* avg_curr_off_drift,VT_FLOAT* avg_curr_on_drift);

#endif