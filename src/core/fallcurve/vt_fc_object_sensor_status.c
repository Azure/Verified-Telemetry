/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_debug.h"
#include "vt_fc_api.h"
#include "vt_fc_database.h"
#include "vt_fc_signature.h"

VT_VOID vt_fallcurve_object_sensor_status(VT_FALLCURVE_OBJECT* fc_object, VT_UINT* sensor_status, VT_UINT* sensor_drift)
{
    VT_ULONG falltime                   = 0;
    VT_FLOAT pearson_coeff              = 0;
    VT_ULONG sampling_interval_us_saved = 0;
    VT_ULONG falltime_saved             = 0;
    VT_FLOAT pearson_coeff_saved        = 0;

    VT_BOOL signature_compute_fail  = false;
    VT_BOOL signature_evaluate_fail = false;

    for (VT_UINT iter = 0; iter < VT_FC_MAX_SIGNATURES; iter++)
    {
        if (fc_fetch_signature(fc_object, iter, &sampling_interval_us_saved, &falltime_saved, &pearson_coeff_saved))
        {
            break;
        }
        if (fc_signature_compute(fc_object, sampling_interval_us_saved, &falltime, &pearson_coeff))
        {
            signature_compute_fail = true;
            continue;
        }
        if (fc_signature_evaluate(falltime, falltime_saved, pearson_coeff, pearson_coeff_saved))
        {
            signature_evaluate_fail = true;
            continue;
        }

        *sensor_status = VT_SIGNATURE_MATCHING;
        *sensor_drift  = 0;
        return;
    }

    if (signature_evaluate_fail)
    {
        *sensor_status = VT_SIGNATURE_NOT_MATCHING;
        *sensor_drift  = 100;
    }
    else if (signature_compute_fail)
    {
        *sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
        *sensor_drift  = 100;
    }
    else
    {
        *sensor_status = VT_SIGNATURE_DB_EMPTY;
        *sensor_drift  = 100;
    }
}
