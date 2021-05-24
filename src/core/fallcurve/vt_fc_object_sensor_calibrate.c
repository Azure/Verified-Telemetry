/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_fc_api.h"
#include "vt_fc_database.h"
#include "vt_fc_signature.h"

VT_UINT vt_fallcurve_object_sensor_calibrate(VT_FALLCURVE_OBJECT* fc_object, VT_UINT8* confidence_metric)
{
    VT_ULONG sampling_interval_us = 0;
    VT_ULONG falltime             = 0;
    VT_FLOAT pearson_coeff        = 0;

    if (fc_signature_compute_collection_settings(fc_object, &sampling_interval_us, confidence_metric))
    {
        return VT_ERROR;
    }

    if (fc_signature_compute(fc_object, sampling_interval_us, &falltime, &pearson_coeff))
    {
        return VT_ERROR;
    }

    fc_reset_db(fc_object);
    if (fc_store_signature(fc_object, sampling_interval_us, falltime, pearson_coeff))
    {
        return VT_ERROR;
    }

    return VT_SUCCESS;
}

VT_UINT vt_fallcurve_object_sensor_recalibrate(VT_FALLCURVE_OBJECT* fc_object, VT_UINT8* confidence_metric)
{
    VT_ULONG sampling_interval_us = 0;
    VT_ULONG falltime             = 0;
    VT_FLOAT pearson_coeff        = 0;

    if (fc_signature_compute_collection_settings(fc_object, &sampling_interval_us, confidence_metric))
    {
        return VT_ERROR;
    }

    if (fc_signature_compute(fc_object, sampling_interval_us, &falltime, &pearson_coeff))
    {
        return VT_ERROR;
    }

    if (fc_store_signature(fc_object, sampling_interval_us, falltime, pearson_coeff))
    {
        return VT_ERROR;
    }

    return VT_SUCCESS;
}