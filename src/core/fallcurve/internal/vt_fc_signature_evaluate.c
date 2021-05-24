/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_debug.h"
#include "vt_fc_signature.h"

VT_FLOAT fc_calculate_falltime_deviation(VT_ULONG falltime_under_test, VT_ULONG falltime_saved)
{
    VT_FLOAT falltime_deviation =
        (abs_custom((VT_FLOAT)falltime_under_test - (VT_FLOAT)falltime_saved) / (VT_FLOAT)falltime_saved) * 100.0f;

#if VT_LOG_LEVEL > 2
    VT_INT32 decimal    = falltime_deviation;
    VT_FLOAT frac_float = falltime_deviation - (VT_FLOAT)decimal;
    VT_INT32 frac       = frac_float * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("FallTime Deviation: %lu.%lu \r\n", decimal, frac);

    return falltime_deviation;
}

VT_FLOAT fc_calculate_pearson_coeff_deviation(VT_FLOAT pearson_coeff_under_test, VT_FLOAT pearson_coeff_saved)
{
    VT_FLOAT pearson_coeff_deviation =
        (abs_custom(pearson_coeff_under_test - pearson_coeff_saved) / pearson_coeff_saved) * 100.0f;

#if VT_LOG_LEVEL > 2
    VT_INT32 decimal    = pearson_coeff_deviation;
    VT_FLOAT frac_float = pearson_coeff_deviation - (VT_FLOAT)decimal;
    VT_INT32 frac       = frac_float * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Pearson Coeff Deviation: %lu.%lu \r\n", decimal, frac);

    return pearson_coeff_deviation;
}

VT_UINT fc_signature_evaluate(
    VT_ULONG falltime_under_test, VT_ULONG falltime_saved, VT_FLOAT pearson_coeff_under_test, VT_FLOAT pearson_coeff_saved)
{
    if (fc_calculate_falltime_deviation(falltime_under_test, falltime_saved) < VT_FC_MAX_FALLTIME_DEVIATION)
    {
        if (fc_calculate_pearson_coeff_deviation(pearson_coeff_under_test, pearson_coeff_saved) < VT_FC_MAX_SHAPE_DEVIATION)
        {
            return VT_SUCCESS;
        }
    }
    return VT_ERROR;
}
