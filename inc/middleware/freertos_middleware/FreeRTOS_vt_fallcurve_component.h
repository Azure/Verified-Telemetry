/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef FREERTOS_VT_FALLCURVE_COMPONENT_H
#define FREERTOS_VT_FALLCURVE_COMPONENT_H


#include "vt_fc_api.h"
#include "azure_iot_hub_client.h"
#include "azure_iot_json_reader.h"
#include "azure_iot_json_writer.h"
typedef struct FreeRTOS_VT_FALLCURVE_COMPONENT_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[VT_COMPONENT_NAME_MAX_LENGTH];

    /* Component Name Length */
    UINT component_name_length;

    /* Telemetry associated with the fallcurve signature component*/
    UCHAR associated_telemetry[VT_ASSOCIATED_TELEMETRY_CSV_MAX_LENGTH];

    /* Status of the telemetry associated with the fallcurve signature component*/
    bool telemetry_status;

    /* Fallcurve Object */
    VT_FALLCURVE_OBJECT fc_object;

    /* Status of reported properties sent  */
    UINT property_sent;

    /* Stores on the scale of 0-100 how much Fingerprint Template can be trusted */
    UINT template_confidence_metric;

    /* Compute sensor status when a global command is issued */
    bool telemetry_status_auto_update;

} FreeRTOS_VT_FALLCURVE_COMPONENT;

AzureIoTResult_t FreeRTOS_vt_fallcurve_init(FreeRTOS_VT_FALLCURVE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update);

AzureIoTResult_t FreeRTOS_vt_fallcurve_process_command(FreeRTOS_VT_FALLCURVE_COMPONENT* handle,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    AzureIoTJSONReader_t* json_reader_ptr,
    AzureIoTJSONWriter_t* json_response_ptr,
    UINT* status_code);

AzureIoTResult_t FreeRTOS_vt_fallcurve_telemetry_status_property(
    FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTHubClient_t * xAzureIoTHubClient, bool* deviceStatus);

AzureIoTResult_t FreeRTOS_vt_fallcurve_fingerprint_type_property(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTHubClient_t * xAzureIoTHubClient);

AzureIoTResult_t FreeRTOS_vt_fallcurve_compute_sensor_status_global(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, bool toggle_verified_telemetry);

AzureIoTResult_t FreeRTOS_vt_fallcurve_process_reported_property_sync(FreeRTOS_VT_FALLCURVE_COMPONENT* handle,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr,
    UINT version);

    #endif /* FREERTOS_VT_FALLCURVE_COMPONENT_H */
