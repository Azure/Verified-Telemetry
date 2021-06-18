/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_SIGNATURE_FEATURES_H
#define _VT_CS_SIGNATURE_FEATURES_H

#include "vt_cs_api.h"
#include "vt_defs.h"

VT_UINT cs_signature_features_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* current_draw);
VT_VOID cs_signature_features_avg_compute(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT lowest_sample_freq, VT_FLOAT* avg_curr);
VT_FLOAT cs_signature_features_evaluate(VT_FLOAT signature_frequency_under_test,
    VT_FLOAT signature_frequency_saved,
    VT_FLOAT duty_cycle_under_test,
    VT_FLOAT duty_cycle_saved,
    VT_FLOAT current_draw_under_test,
    VT_FLOAT current_draw_saved);
VT_FLOAT cs_signature_features_avg_evaluate(VT_FLOAT avg_curr_under_test, VT_FLOAT avg_curr_saved);

#endif