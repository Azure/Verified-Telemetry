/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "nx_vt_currentsense_component.h"

#include "vt_cs_api.h"
#include "vt_debug.h"

#define SAMPLE_COMMAND_SUCCESS_STATUS (200)
#define SAMPLE_COMMAND_ERROR_STATUS   (500)

/* Pnp command supported */
static const CHAR command_reset_fingerprint[]   = "setResetFingerprintTemplate";
static const CHAR command_retrain_fingerprint[] = "retrainFingerprintTemplate";

/* Names of properties for desired/reporting */

static const CHAR telemetry_name_telemetry_status[]     = "telemetryStatus";
static const CHAR fingerprint_type_property[]           = "fingerprintType";
static const CHAR template_confidence_metric_property[] = "fingerprintTemplateConfidenceMetric";
static const CHAR fingerprint_type_value[]              = "CurrentSense";
static const CHAR fingerprint_template_property[]       = "fingerprintTemplate";
static const CHAR num_signatures_json_property[]        = "numSignatures";
static const CHAR avg_curr_json_property[]              = "avgCurr";
static const CHAR lowest_sample_freq_json_property[]    = "lowestSampleFreq";
static const CHAR sampling_freq_json_property[]         = "samplingFreq";
static const CHAR signal_freq_json_property[]           = "signalFreq";
static const CHAR curr_diff_json_property[]             = "currDiff";
static const CHAR duty_cycle_json_property[]            = "dutyCycle";

UINT nx_vt_currentsense_init(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    UINT* shared_buffer,
    UINT shared_buffer_size)
{
    UINT status;
    CHAR vt_component_name[50];
    if (handle == NX_NULL)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    memset(vt_component_name, 0, sizeof(vt_component_name));
    snprintf(vt_component_name, sizeof(vt_component_name), "vT");
    strcat(vt_component_name, (CHAR*)component_name_ptr);
    strncpy((CHAR*)handle->component_name_ptr, vt_component_name, sizeof(handle->component_name_ptr));
    handle->component_name_length = strlen(vt_component_name);
    strncpy((CHAR*)handle->associated_telemetry, (CHAR*)associated_telemetry, sizeof(handle->associated_telemetry));
    handle->property_sent = 0;

    status =
        vt_currentsense_object_initialize(&(handle->cs_object), device_driver, sensor_handle, shared_buffer, shared_buffer_size);

    return (status);
}

/* reset fingerprint method implementation */
static UINT reset_reference_currentsense(NX_VT_CURRENTSENSE_COMPONENT* handle)
{
    vt_currentsense_object_sensor_calibrate(&(handle->cs_object));
    return (NX_AZURE_IOT_SUCCESS);
}

/* retrain fingerprint method implementation */
static UINT retrain_reference_currentsense(NX_VT_CURRENTSENSE_COMPONENT* handle)
{
    vt_currentsense_object_sensor_recalibrate(&(handle->cs_object));
    return (NX_AZURE_IOT_SUCCESS);
}

static UINT sync_fingerprint_template(NX_AZURE_IOT_JSON_READER* property_value_reader_ptr, NX_VT_CURRENTSENSE_COMPONENT* handle)
{
    VT_CURRENTSENSE_DATABASE_FLATTENED flattened_db;
    CHAR jsonKey[20];
    CHAR jsonValue[40];
    UINT bytes_copied = 0;

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(num_signatures_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)num_signatures_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.num_signatures, 0, sizeof(flattened_db.num_signatures));
    strncpy((VT_CHAR*)flattened_db.num_signatures, jsonValue, sizeof(flattened_db.num_signatures));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(avg_curr_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)avg_curr_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.avg_curr, 0, sizeof(flattened_db.avg_curr));
    strncpy((VT_CHAR*)flattened_db.avg_curr, jsonValue, sizeof(flattened_db.avg_curr));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(lowest_sample_freq_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)lowest_sample_freq_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.lowest_sample_freq, 0, sizeof(flattened_db.lowest_sample_freq));
    strncpy((VT_CHAR*)flattened_db.lowest_sample_freq, jsonValue, sizeof(flattened_db.lowest_sample_freq));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(sampling_freq_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)sampling_freq_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.sampling_freq, 0, sizeof(flattened_db.sampling_freq));
    strncpy((VT_CHAR*)flattened_db.sampling_freq, jsonValue, sizeof(flattened_db.sampling_freq));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(signal_freq_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)signal_freq_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.signal_freq, 0, sizeof(flattened_db.signal_freq));
    strncpy((VT_CHAR*)flattened_db.signal_freq, jsonValue, sizeof(flattened_db.signal_freq));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(curr_diff_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)curr_diff_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.curr_diff, 0, sizeof(flattened_db.curr_diff));
    strncpy((VT_CHAR*)flattened_db.curr_diff, jsonValue, sizeof(flattened_db.curr_diff));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(duty_cycle_json_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)duty_cycle_json_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.duty_cycle, 0, sizeof(flattened_db.duty_cycle));
    strncpy((VT_CHAR*)flattened_db.duty_cycle, jsonValue, sizeof(flattened_db.duty_cycle));

    vt_currentsense_object_database_sync(&(handle->cs_object), &flattened_db);

    return NX_SUCCESS;
}

static UINT nx_vt_currentsense_fingerprint_template_property(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db)
{
    UINT status          = true;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER)))
    {
        VTLogError("Failed create reported properties: error code = 0x%08x\r\n", status);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(
             iotpnp_client_ptr, &json_writer, handle->component_name_ptr, handle->component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_name(
             &json_writer, (UCHAR*)fingerprint_template_property, sizeof(fingerprint_template_property) - 1)) ||
        (status = nx_azure_iot_json_writer_append_begin_object(&json_writer)))
    {
        VTLogError("Failed to start building reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)num_signatures_json_property,
             strlen((const char*)num_signatures_json_property),
             flattened_db->num_signatures,
             strlen((CHAR*)flattened_db->num_signatures))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)avg_curr_json_property,
             strlen((const char*)avg_curr_json_property),
             flattened_db->avg_curr,
             strlen((CHAR*)flattened_db->avg_curr))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)lowest_sample_freq_json_property,
             strlen((const char*)lowest_sample_freq_json_property),
             flattened_db->lowest_sample_freq,
             strlen((CHAR*)flattened_db->lowest_sample_freq))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)sampling_freq_json_property,
             strlen((const char*)sampling_freq_json_property),
             flattened_db->sampling_freq,
             strlen((CHAR*)flattened_db->sampling_freq))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)signal_freq_json_property,
             strlen((const char*)signal_freq_json_property),
             flattened_db->signal_freq,
             strlen((CHAR*)flattened_db->signal_freq))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)curr_diff_json_property,
             strlen((const char*)curr_diff_json_property),
             flattened_db->curr_diff,
             strlen((CHAR*)flattened_db->curr_diff))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (UCHAR*)duty_cycle_json_property,
             strlen((const char*)duty_cycle_json_property),
             flattened_db->duty_cycle,
             strlen((CHAR*)flattened_db->duty_cycle))))
    {
        VTLogError("Failed to append currentsense DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_end_object(&json_writer)) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr, &json_writer)))
    {
        VTLogError("Failed to build reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_properties_send(
             iotpnp_client_ptr, &json_writer, NX_NULL, &response_status, NX_NULL, (5 * NX_IP_PERIODIC_RATE))))
    {
        VTLogError("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);

    if ((response_status < 200) || (response_status >= 300))
    {
        VTLogError("device twin report properties failed with code : %d\r\n", response_status);
        return (NX_NOT_SUCCESSFUL);
    }

    return (status);
}

static UINT nx_vt_currentsense_fingerprint_template_confidence_metric_property(
    NX_VT_CURRENTSENSE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr, UINT template_confidence_metric)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER)))
    {
        VTLogError("Failed create reported properties: error code = 0x%08x\r\n", status);
        return (status);
    }
    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(
             iotpnp_client_ptr, &json_writer, handle->component_name_ptr, handle->component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_int32_value(&json_writer,
             (const UCHAR*)template_confidence_metric_property,
             sizeof(template_confidence_metric_property) - 1,
             (int32_t)template_confidence_metric)) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr, &json_writer)))
    {
        VTLogError("Failed to build reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_properties_send(
             iotpnp_client_ptr, &json_writer, NX_NULL, &response_status, NX_NULL, (5 * NX_IP_PERIODIC_RATE))))
    {
        VTLogError("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);

    if ((response_status < 200) || (response_status >= 300))
    {
        VTLogError("device twin report properties failed with code : %d\r\n", response_status);
        return (NX_NOT_SUCCESSFUL);
    }

    return (status);
}

static UINT nx_vt_currentsense_fingerprint_template_associated_properties(
    NX_VT_CURRENTSENSE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
{
    UINT status          = true;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    VT_CURRENTSENSE_DATABASE_FLATTENED flattened_db;
    bool db_required_to_store       = true;
    UINT template_confidence_metric = 0;
    vt_currentsense_object_database_fetch(
        &(handle->cs_object), &flattened_db, &db_required_to_store, &template_confidence_metric);

    if (db_required_to_store)
    {
        if ((status = nx_vt_currentsense_fingerprint_template_property(handle, iotpnp_client_ptr, &flattened_db)))
        {
            VTLogError("Failed to update fingerprint template reported property: error code = 0x%08x\r\n", status);
            return (status);
        }
        if ((status = nx_vt_currentsense_fingerprint_template_confidence_metric_property(
                 handle, iotpnp_client_ptr, template_confidence_metric)))
        {
            VTLogError(
                "Failed to update fingerprint template confidence metric reported property: error code = 0x%08x\r\n", status);
            return (status);
        }
    }
    return (status);
}

static UINT nx_vt_currentsense_telemetry_status_property(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    bool* device_status,
    bool toggle_verified_telemetry)
{
    UINT status          = 0;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    UINT sensor_status    = 0;
    UINT sensor_drift     = 0;
    bool telemetry_status = false;

    if (toggle_verified_telemetry)
    {
        vt_currentsense_object_sensor_fetch_status(&(handle->cs_object), &sensor_status, &sensor_drift);
        telemetry_status = (sensor_status > 0) ? false : true;
    }

    *device_status = *device_status && telemetry_status;

    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER)))
    {
        VTLogError("Failed create reported properties: error code = 0x%08x\r\n", status);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(
             iotpnp_client_ptr, &json_writer, handle->component_name_ptr, handle->component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_bool_value(&json_writer,
             (const UCHAR*)telemetry_name_telemetry_status,
             sizeof(telemetry_name_telemetry_status) - 1,
             telemetry_status)) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr, &json_writer)))
    {
        VTLogError("Failed to build reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_properties_send(
             iotpnp_client_ptr, &json_writer, NX_NULL, &response_status, NX_NULL, (5 * NX_IP_PERIODIC_RATE))))
    {
        VTLogError("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);

    if ((response_status < 200) || (response_status >= 300))
    {
        VTLogError("device twin report properties failed with code : %d\r\n", response_status);
        return (NX_NOT_SUCCESSFUL);
    }

    return (status);
}

static UINT nx_vt_currentsense_fingerprint_type_property(
    NX_VT_CURRENTSENSE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER)))
    {
        VTLogError("Failed create reported properties: error code = 0x%08x\r\n", status);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(
             iotpnp_client_ptr, &json_writer, handle->component_name_ptr, handle->component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
             (const UCHAR*)fingerprint_type_property,
             sizeof(fingerprint_type_property) - 1,
             (const UCHAR*)fingerprint_type_value,
             strlen((const char*)fingerprint_type_value))) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr, &json_writer)))
    {
        VTLogError("Failed to build reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_properties_send(
             iotpnp_client_ptr, &json_writer, NX_NULL, &response_status, NX_NULL, (5 * NX_IP_PERIODIC_RATE))))
    {
        VTLogError("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);

    if ((response_status < 200) || (response_status >= 300))
    {
        VTLogError("device twin report properties failed with code : %d\r\n", response_status);
        return (NX_NOT_SUCCESSFUL);
    }

    return (status);
}

UINT nx_vt_currentsense_reported_properties(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    bool* device_status,
    bool toggle_verified_telemetry)
{
    UINT status = 0;

    if ((status = nx_vt_currentsense_telemetry_status_property(
             handle, iotpnp_client_ptr, device_status, toggle_verified_telemetry)))
    {
        VTLogError("Failed nx_vt_fallcurve_telemetry_status_property for component %.*s: error code = "
                   "0x%08x\r\n\n",
            (INT)handle->component_name_length,
            (CHAR*)handle->component_name_ptr,
            status);
    }

    else if ((status = nx_vt_currentsense_fingerprint_template_associated_properties(handle, iotpnp_client_ptr)))
    {
        VTLogError("Failed nx_vt_fallcurve_telemetry_status_property for component %.*s: error code = "
                   "0x%08x\r\n\n",
            (INT)handle->component_name_length,
            (CHAR*)handle->component_name_ptr,
            status);
    }

    else if (handle->property_sent == 0)
    {
        if ((status = nx_vt_currentsense_fingerprint_type_property(handle, iotpnp_client_ptr)))
        {
            VTLogError("Failed nx_vt_fallcurve_fingerprint_type_property for component %.*s: error code = 0x%08x\r\n",
                (INT)handle->component_name_length,
                (CHAR*)handle->component_name_ptr,
                status);
        }

        handle->property_sent = 1;
    }
    return status;
}

UINT nx_vt_currentsense_process_command(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code)
{
    UINT dm_status;

    if (handle == NX_NULL)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    if (handle->component_name_length != component_name_length ||
        strncmp((CHAR*)handle->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    // Command 1 : Reset Fingerprint
    if (((pnp_command_name_length == (sizeof(command_reset_fingerprint) - 1)) &&
            (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_reset_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (reset_reference_currentsense(handle) != NX_AZURE_IOT_SUCCESS) ? SAMPLE_COMMAND_ERROR_STATUS
                                                                                   : SAMPLE_COMMAND_SUCCESS_STATUS;
    }

    // Command 2 : Retrain Fingerprint
    else if (((pnp_command_name_length == (sizeof(command_retrain_fingerprint) - 1)) &&
                 (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_retrain_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (retrain_reference_currentsense(handle) != NX_AZURE_IOT_SUCCESS) ? SAMPLE_COMMAND_ERROR_STATUS
                                                                                     : SAMPLE_COMMAND_SUCCESS_STATUS;
    }

    else
    {
        VTLogError("PnP command=%.*s is not supported on vTInfo  component\r\n", pnp_command_name_length, pnp_command_name_ptr);
        dm_status = 404;
    }

    *status_code = dm_status;

    return (NX_AZURE_IOT_SUCCESS);
}

UINT nx_vt_currentsense_process_reported_property_sync(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version)
{
    CHAR property_name[50];
    UINT property_name_length = 0;

    if (handle == NX_NULL)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    if (handle->component_name_length != component_name_length ||
        strncmp((CHAR*)handle->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    // Get Property Name
    nx_azure_iot_json_reader_token_string_get(
        name_value_reader_ptr, (UCHAR*)property_name, sizeof(property_name), &property_name_length);

    // Property 1: FingerprintDB
    if (nx_azure_iot_json_reader_token_is_text_equal(
            name_value_reader_ptr, (UCHAR*)fingerprint_template_property, sizeof(fingerprint_template_property) - 1) == NX_TRUE)
    {
        VTLogInfo("Syncing Fingerprint Template...\r\n");
        if (nx_azure_iot_json_reader_next_token(name_value_reader_ptr) ||
            sync_fingerprint_template(name_value_reader_ptr, handle))
        {
            VTLogError("Could not sync fingerprint template for component %.*s, invoke command "
                       "setResetFingerprintTemplate to collect new fingerprint template \r\n\n",
                component_name_length,
                component_name_ptr);
            return (NX_AZURE_IOT_FAILURE);
        }
        else
        {
            VTLogInfo(
                "Successfully synced fingerprint template for component %.*s \r\n\n", component_name_length, component_name_ptr);
            return (NX_AZURE_IOT_SUCCESS);
        }
    }

    return (NX_AZURE_IOT_SUCCESS);
}

UINT nx_vt_currentsense_signature_read(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry)
{
    if (handle->associated_telemetry != associated_telemetry ||
        strncmp((CHAR*)handle->associated_telemetry, (CHAR*)associated_telemetry, associated_telemetry_length) != 0)
    {
        return (NX_NOT_SUCCESSFUL);
    }
    if (!toggle_verified_telemetry)
    {
        return (NX_NOT_SUCCESSFUL);
    }
    vt_currentsense_object_signature_read(&(handle->cs_object));
    return (NX_AZURE_IOT_SUCCESS);
}

UINT nx_vt_currentsense_signature_process(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry)
{
    if (handle->associated_telemetry != associated_telemetry ||
        strncmp((CHAR*)handle->associated_telemetry, (CHAR*)associated_telemetry, associated_telemetry_length) != 0)
    {
        return (NX_NOT_SUCCESSFUL);
    }
    if (!toggle_verified_telemetry)
    {
        return (NX_NOT_SUCCESSFUL);
    }
    vt_currentsense_object_signature_process(&(handle->cs_object));
    return (NX_AZURE_IOT_SUCCESS);
}

bool nx_vt_currentsense_fetch_telemetry_status(NX_VT_CURRENTSENSE_COMPONENT* handle, bool toggle_verified_telemetry)
{
    UINT sensor_status    = 0;
    UINT sensor_drift     = 0;
    bool telemetry_status = false;

    if (toggle_verified_telemetry)
    {
        vt_currentsense_object_sensor_fetch_status(&(handle->cs_object), &sensor_status, &sensor_drift);
        telemetry_status = (sensor_status > 0) ? false : true;
    }
    return telemetry_status;
}