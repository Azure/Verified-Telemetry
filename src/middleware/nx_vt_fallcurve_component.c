/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "nx_vt_fallcurve_component.h"

#include "vt_fc_api.h"
#include "vt_debug.h"

#define SAMPLE_COMMAND_SUCCESS_STATUS (200)
#define SAMPLE_COMMAND_ERROR_STATUS   (500)

/* Pnp command supported */
static const CHAR command_reset_fingerprint[]   = "setResetFingerprintTemplate";
static const CHAR command_retrain_fingerprint[] = "retrainFingerprintTemplate";

/* Names of properties for desired/reporting */

static const CHAR telemetry_name_telemetry_status[] = "telemetryStatus";
static const CHAR fingerprint_type_property[]           = "fingerprintType";
static const CHAR template_confidence_metric_property[]  = "fingerprintTemplateConfidenceMetric";
static const CHAR fingerprint_type_value[]              = "FallCurve";
static const CHAR fingerprint_template_property[]       = "fingerprintTemplate";
static const CHAR num_signatures_json_property[]       = "numSignatures";
static const CHAR sampling_interval_us_property[]             = "samplingIntervalus";
static const CHAR falltime_property[]             = "falltime";
static const CHAR pearson_coeff_property[]             = "pearsonCoeff";

UINT nx_vt_fallcurve_init(NX_VT_FALLCURVE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update)
{
    CHAR vt_component_name[50];
    printf("[VT CS] Entered vt_fallcurve_init()\r\n");
    if (handle == NX_NULL)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    memset(vt_component_name, 0, sizeof(vt_component_name));
    snprintf(vt_component_name, sizeof(vt_component_name), "vT");
    strcat(vt_component_name, (CHAR*)component_name_ptr);
    strncpy((CHAR*)handle->component_name_ptr, vt_component_name, sizeof(handle->component_name_ptr));
    handle->component_name_length       = strlen(vt_component_name);
    strncpy((CHAR*)handle->associated_telemetry , (CHAR*)associated_telemetry, sizeof(handle->associated_telemetry));
    handle->telemetry_status            = false;
    handle->property_sent               = 0;
    handle->template_confidence_metric  = 0;
    printf("[VT CS] Entering vt_fallcurve_object_init()\r\n");

    vt_fallcurve_object_initialize(&(handle->fc_object),
        device_driver,
        sensor_handle);
    printf("[VT CS] Left vt_fallcurve_object_init()\r\n");

    return (NX_AZURE_IOT_SUCCESS);
}

/* reset fingerprint method implementation */
static UINT reset_refernce_fallcurve(NX_VT_FALLCURVE_COMPONENT* handle, NX_AZURE_IOT_JSON_READER* json_reader_ptr)
{
    uint8_t confidence_metric;
    UINT status  = vt_fallcurve_object_sensor_calibrate(&(handle->fc_object), &confidence_metric);
    handle->template_confidence_metric = confidence_metric;
    return (status);
}

/* retrain fingerprint method implementation */
static UINT retrain_refernce_fallcurve(NX_VT_FALLCURVE_COMPONENT* handle, NX_AZURE_IOT_JSON_READER* json_reader_ptr)
{
    uint8_t confidence_metric;
    UINT status  = vt_fallcurve_object_sensor_recalibrate(&(handle->fc_object), &confidence_metric);
    handle->template_confidence_metric = confidence_metric;
    return (status);
}

static UINT hub_store_all_db(NX_VT_FALLCURVE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    VT_FALLCURVE_DATABASE_FLATTENED flattened_db;
    vt_fallcurve_object_database_fetch(&(handle->fc_object), &flattened_db);

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

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer, (UCHAR*)num_signatures_json_property, 
        strlen((const char*)num_signatures_json_property), flattened_db.num_signatures, strlen((CHAR*)flattened_db.num_signatures))))
    {
        VTLogError("Failed to append fallcurve DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer, (UCHAR*)sampling_interval_us_property, 
        strlen((const char*)sampling_interval_us_property), flattened_db.sampling_interval_us, strlen((CHAR*)flattened_db.sampling_interval_us))))
    {
        VTLogError("Failed to append fallcurve DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer, (UCHAR*)falltime_property, 
        strlen((const char*)falltime_property), flattened_db.falltime, strlen((CHAR*)flattened_db.falltime))))
    {
        VTLogError("Failed to append fallcurve DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return (status);
    }
    
    if ((status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer, (UCHAR*)pearson_coeff_property, 
        strlen((const char*)pearson_coeff_property), flattened_db.pearson_coeff, strlen((CHAR*)flattened_db.pearson_coeff))))
    {
        VTLogError("Failed to append fallcurve DB data: error code = 0x%08x\r\n", status);
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

static UINT sync_fingerprint_template(
    NX_AZURE_IOT_JSON_READER* property_value_reader_ptr, NX_VT_FALLCURVE_COMPONENT* handle)
{
    VT_FALLCURVE_DATABASE_FLATTENED flattened_db;
    CHAR jsonKey[20];
    CHAR jsonValue[40];
    UINT bytes_copied  = 0;

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
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
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(sampling_interval_us_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)sampling_interval_us_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.sampling_interval_us, 0, sizeof(flattened_db.sampling_interval_us)); 
    strncpy((VT_CHAR*)flattened_db.sampling_interval_us, jsonValue, sizeof(flattened_db.sampling_interval_us));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(falltime_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)falltime_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.falltime, 0, sizeof(flattened_db.falltime)); 
    strncpy((VT_CHAR*)flattened_db.falltime, jsonValue, sizeof(flattened_db.falltime));

    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonKey, sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if (nx_azure_iot_json_reader_next_token(property_value_reader_ptr) ||
        nx_azure_iot_json_reader_token_string_get(
            property_value_reader_ptr, (UCHAR*)jsonValue, sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    VTLogInfo("\t%.*s: %.*s\r\n", strlen(jsonKey), jsonKey, bytes_copied, jsonValue);
    if (((strlen(jsonKey) == (sizeof(pearson_coeff_property) - 1)) &&
            (!(strncmp((CHAR*)jsonKey, (CHAR*)pearson_coeff_property, strlen(jsonKey))))) == 0)
    {
        return NX_NOT_SUCCESSFUL;
    }
    memset(flattened_db.pearson_coeff, 0, sizeof(flattened_db.pearson_coeff)); 
    strncpy((VT_CHAR*)flattened_db.pearson_coeff, jsonValue, sizeof(flattened_db.pearson_coeff));

    vt_fallcurve_object_database_sync(&(handle->fc_object), flattened_db);

    return NX_SUCCESS;
}

UINT nx_vt_fallcurve_telemetry_status_property(NX_VT_FALLCURVE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    bool toggleVerifiedTelemetry,
    bool* deviceStatus)
{
    UINT status          = 0;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    bool oldTelemetryStatus = handle->telemetry_status;

    /* Get fallcurve classification */
    VT_UINT sensor_status = 0;
    VT_UINT sensor_drift = 100;
    if(toggleVerifiedTelemetry)
    {
        handle->telemetry_status = false;
    }
    else
    {
        vt_fallcurve_object_sensor_status(&(handle->fc_object), &sensor_status, &sensor_drift);
        handle->telemetry_status =  (sensor_status > 0) ? false : true;
    }

    *deviceStatus &= handle->telemetry_status;

    if (oldTelemetryStatus == handle->telemetry_status && handle->property_sent != 0)
    {
        // VTLogInfo("Telemetry Status is the same, not updating digitalTwin!\r\n");
        return (status);
    }

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
             handle->telemetry_status)) ||
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

UINT nx_vt_fallcurve_fingerprint_type_property(NX_VT_FALLCURVE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
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

UINT nx_vt_fallcurve_fingerprint_template_confidence_metric_property(
    NX_VT_FALLCURVE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER)))
    {
        VTLogError("Failed create reported properties: error code = 0x%08x\r\n", status);
        return (status);
    }
    // CHAR *templateConfidenceMetricValue = ((double)handle->templateConfidenceMetric > 0.75) ? "HIGH"
    //                                     : ((double)handle->templateConfidenceMetric > 0.25) ? "MEDIUM"
    //                                     : "LOW";
    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(
             iotpnp_client_ptr, &json_writer, handle->component_name_ptr, handle->component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_int32_value(&json_writer,
             (const UCHAR*)template_confidence_metric_property,
             sizeof(template_confidence_metric_property) - 1,
             (int32_t)handle->template_confidence_metric)) ||
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

UINT nx_vt_fallcurve_process_command(NX_VT_FALLCURVE_COMPONENT* handle,
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
        printf("[VT CS] Command exit scenario 1)\r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    if (handle->component_name_length != component_name_length ||
        strncmp((CHAR*)handle->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        printf("[VT CS] Command exit scenario 2)\r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    // Command 1 : Reset Fingerprint
    if (((pnp_command_name_length == (sizeof(command_reset_fingerprint) - 1)) &&
            (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_reset_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (reset_refernce_fallcurve(handle, json_reader_ptr) != NX_AZURE_IOT_SUCCESS)
                        ? SAMPLE_COMMAND_ERROR_STATUS
                        : SAMPLE_COMMAND_SUCCESS_STATUS;

        if (hub_store_all_db(handle, iotpnp_client_ptr))
        {
            VTLogError("Failed to update db in cloud\r\n");
        }
        if (nx_vt_fallcurve_fingerprint_template_confidence_metric_property(handle, iotpnp_client_ptr))
        {
            VTLogError("Failed to update Fingerprint Template Confidence Metric\r\n");
        }
    }

    // Command 2 : Retrain Fingerprint
    else if (((pnp_command_name_length == (sizeof(command_retrain_fingerprint) - 1)) &&
                 (!(strncmp(
                     (CHAR*)pnp_command_name_ptr, (CHAR*)command_retrain_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (retrain_refernce_fallcurve(handle, json_reader_ptr) != NX_AZURE_IOT_SUCCESS)
                        ? SAMPLE_COMMAND_ERROR_STATUS
                        : SAMPLE_COMMAND_SUCCESS_STATUS;

        if (hub_store_all_db(handle, iotpnp_client_ptr))
        {
            VTLogError("Failed to update db in cloud\r\n");
        }
        if (nx_vt_fallcurve_fingerprint_template_confidence_metric_property(handle, iotpnp_client_ptr))
        {
            VTLogError("Failed to update Fingerprint Template Confidence Metric\r\n");
        }
    }

    else
    {
        VTLogError("PnP command=%.*s is not supported on vTInfo  component\r\n",
            pnp_command_name_length,
            pnp_command_name_ptr);
        dm_status = 404;
    }

    *status_code = dm_status;

    return (NX_AZURE_IOT_SUCCESS);
}

UINT nx_vt_fallcurve_process_reported_property_sync(NX_VT_FALLCURVE_COMPONENT* handle,
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
    if (nx_azure_iot_json_reader_token_is_text_equal(name_value_reader_ptr,
            (UCHAR*)fingerprint_template_property,
            sizeof(fingerprint_template_property) - 1) == NX_TRUE)
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
            VTLogInfo("Successfully synced fingerprint template for component %.*s \r\n\n",
                component_name_length,
                component_name_ptr);
            return (NX_AZURE_IOT_SUCCESS);
        }
    }

    return (NX_AZURE_IOT_SUCCESS);
}

