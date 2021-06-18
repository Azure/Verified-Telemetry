/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_DATABASE_H
#define _VT_CS_DATABASE_H

#include "vt_cs_api.h"
#include "vt_defs.h"

VT_VOID cs_reset_db(VT_CURRENTSENSE_OBJECT* cs_object);
VT_UINT cs_store_signature(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT signature_frequency,
    VT_FLOAT duty_cycle,
    VT_FLOAT current_draw);
VT_UINT cs_update_average_current_draw(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT lowest_sample_freq, VT_FLOAT avg_curr);
VT_UINT cs_fetch_signature(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_UINT signature_iter,
    VT_FLOAT* sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* current_draw);
VT_UINT cs_fetch_average_current(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* lowest_sample_freq, VT_FLOAT* avg_curr);
VT_VOID cs_fetch_evaluation_sampling_frequencies(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* sampling_freqeuencies,
    VT_UINT sampling_frequencies_buffer_length,
    VT_UINT* num_sampling_frequencies);

#endif