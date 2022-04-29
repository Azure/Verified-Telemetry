/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_database.h"

VT_UINT vt_currentsense_object_initialize(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    VT_CHAR* raw_signatures_buffer,
    VT_UINT raw_signatures_buffer_size)
{
    cs_object->sensor_handle = sensor_handle;

    cs_object->device_driver = device_driver;

    cs_reset_db(cs_object);

    cs_object->mode = VT_MODE_RUNTIME_EVALUATE;

    cs_object->db_updated = VT_DB_NOT_UPDATED;

    cs_object->sensor_status = VT_SIGNATURE_EVAL_DEFAULT_VALUE;

    cs_object->sensor_drift = VT_SIGNATURE_DRIFT_DEFAULT_VALUE;

    cs_object->template_confidence_metric = VT_SIGNATURE_TEMPLATE_CONFIDENCE_DEFAULT_VALUE;

    cs_object->raw_signatures_reader_initialized = false;

    for (int i=0;i<VT_HISTORY_ARRAY_LENGTH;i++){
        cs_object->hist_array[i]=1;
    }

    if (raw_signatures_buffer_size < sizeof(VT_CURRENTSENSE_RAW_SIGNATURES_READER))
    {
        return VT_ERROR;
    }

    cs_object->raw_signatures_reader = (VT_CURRENTSENSE_RAW_SIGNATURES_READER*)raw_signatures_buffer;
    cs_object->raw_signatures_reader->repeating_raw_signature_ongoing_collection  = false;
    cs_object->raw_signatures_reader->repeating_raw_signature_buffers_filled      = false;
    cs_object->raw_signatures_reader->non_repeating_raw_signature_stop_collection = false;
    cs_object->raw_signatures_reader_initialized                                  = true;

    return VT_SUCCESS;
}