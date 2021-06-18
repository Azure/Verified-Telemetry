/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_signature_features.h"
#include "vt_debug.h"

VT_FLOAT cs_signature_features_evaluate(VT_FLOAT signature_frequency_under_test,
    VT_FLOAT signature_frequency_saved,
    VT_FLOAT duty_cycle_under_test,
    VT_FLOAT duty_cycle_saved,
    VT_FLOAT current_draw_under_test,
    VT_FLOAT current_draw_saved)
{
    VT_FLOAT signature_drift = 0;
    signature_drift += abs_custom((signature_frequency_under_test - signature_frequency_saved) / signature_frequency_saved) * 100;
    signature_drift += abs_custom((duty_cycle_under_test - duty_cycle_saved) / duty_cycle_saved) * 100;
    signature_drift += abs_custom((current_draw_under_test - current_draw_saved) / current_draw_saved) * 100;
    signature_drift /= 3.0f;
#if VT_LOG_LEVEL > 2
    int32_t decimal  = signature_drift;
    float frac_float = signature_drift - (float)decimal;
    int32_t frac     = frac_float * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Deviation in Signature: %lu.%lu \r\n", decimal, frac);
    return signature_drift;
}

VT_FLOAT cs_signature_features_avg_evaluate(VT_FLOAT avg_curr_under_test, VT_FLOAT avg_curr_saved)
{
    VT_FLOAT signature_drift = 0;
    signature_drift          = abs_custom((avg_curr_under_test - avg_curr_saved) / avg_curr_saved) * 100;
#if VT_LOG_LEVEL > 2
    int32_t decimal  = signature_drift;
    float frac_float = signature_drift - (float)decimal;
    int32_t frac     = frac_float * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Deviation in Signature: %lu.%lu \r\n", decimal, frac);
    return signature_drift;
}
