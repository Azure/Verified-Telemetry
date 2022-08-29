/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_DATABASE_H
#define _VT_CS_DATABASE_H

#include "vt_cs_api.h"
#include "vt_defs.h"

VT_VOID cs_reset_db(VT_CURRENTSENSE_OBJECT* cs_object);
VT_VOID cs_reset_Repeatability_Data_Array(VT_CURRENTSENSE_OBJECT* cs_object);
VT_UINT cs_store_repeating_signature_feature_vector(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT signature_frequency,
    VT_FLOAT duty_cycle,
    VT_FLOAT relative_current_draw);
VT_UINT cs_update_repeating_signature_offset_current_draw(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT lowest_sample_freq, VT_FLOAT offset_curr);
VT_UINT cs_update_non_repeating_signature_average_current_draw(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT avg_curr_on, VT_FLOAT avg_curr_off);
VT_UINT cs_fetch_template_repeating_signature_feature_vector(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_UINT signature_iter,
    VT_FLOAT* sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* relative_current_draw,
    VT_FLOAT* average_current_draw,
    VT_FLOAT* sec_sig_freq);
VT_UINT cs_fetch_template_repeating_signature_offset_current(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* lowest_sample_freq, VT_FLOAT* offset_current);
VT_UINT cs_fetch_template_non_repeating_signature_average_current(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* avg_curr_on, VT_FLOAT* avg_curr_off);
VT_VOID cs_fetch_template_repeating_signature_sampling_frequencies(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* repeating_signature_sampling_freqeuencies,
    VT_UINT repeating_signature_sampling_frequencies_buffer_capacity,
    VT_UINT* num_repeating_signature_sampling_frequencies);

#endif