/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_calibrate.h"
#include "vt_cs_database.h"
#include "vt_cs_raw_signature_read.h"
#include "vt_cs_sensor_status.h"

VT_VOID vt_currentsense_object_signature_read(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT sampling_frequencies[VT_CS_MAX_SIGNATURES];
    VT_UINT num_sampling_frqeuencies = 0;
    if (cs_object->mode == VT_MODE_RUNTIME_EVALUATE)
    {
        cs_fetch_evaluation_sampling_frequencies(
            cs_object, sampling_frequencies, VT_CS_MAX_SIGNATURES, &num_sampling_frqeuencies);
    }
    else
    {
        cs_calibrate_compute_sampling_frequencies(
            cs_object, sampling_frequencies, VT_CS_MAX_SIGNATURES, &num_sampling_frqeuencies);
    }
    cs_raw_signature_read(cs_object, sampling_frequencies, num_sampling_frqeuencies, VT_CS_SAMPLE_LENGTH);
}

VT_VOID vt_currentsense_object_signature_process(VT_CURRENTSENSE_OBJECT* cs_object)
{
    while (cs_object->raw_signatures_reader->raw_signature_ongoing_collection)
    {
        // do nothing, wait till raw signatures are collected
    }
    switch (cs_object->mode)
    {
        case VT_MODE_RUNTIME_EVALUATE:
            cs_sensor_status(cs_object);
            break;

        case VT_MODE_CALIBRATE:
            cs_calibrate_sensor(cs_object);
            cs_object->mode = VT_MODE_RUNTIME_EVALUATE;
            break;

        case VT_MODE_RECALIBRATE:
            cs_recalibrate_sensor(cs_object);
            cs_object->mode = VT_MODE_RUNTIME_EVALUATE;
            break;
    }
}