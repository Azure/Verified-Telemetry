/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef FREERTOS_VT_CURRENTSENSE_COMPONENT_H
#define FREERTOS_VT_CURRENTSENSE_COMPONENT_H

#include "azure_iot_hub_client.h"
#include "azure_iot_json_reader.h"
#include "azure_iot_json_writer.h"

#include "vt_cs_api.h"

typedef struct FreeRTOS_VT_CURRENTSENSE_COMPONENT_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[30];

    /* Component Name Length */
    UINT component_name_length;

    /* Telemetry associated with the currentsense signature component*/
    UCHAR associated_telemetry[100];

    /* Currentsense Object */
    VT_CURRENTSENSE_OBJECT cs_object;

    /* Status of reported properties sent  */
    UINT property_sent;

} FreeRTOS_VT_CURRENTSENSE_COMPONENT;

AzureIoTResult_t FreeRTOS_vt_currentsense_init(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    CHAR* shared_buffer,
    UINT shared_buffer_size);

AzureIoTResult_t FreeRTOS_vt_currentsense_reported_properties(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    bool* device_status,
    bool toggle_verified_telemetry);

AzureIoTResult_t FreeRTOS_vt_currentsense_process_command(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    UINT* status_code);

AzureIoTResult_t FreeRTOS_vt_currentsense_process_reported_property_sync(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr);

AzureIoTResult_t FreeRTOS_vt_currentsense_signature_read(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry,UINT mode);

AzureIoTResult_t FreeRTOS_vt_currentsense_signature_process(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry);

bool FreeRTOS_vt_currentsense_fetch_telemetry_status(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle, bool toggle_verified_telemetry);

#ifdef __cplusplus
}
#endif
#endif /* FreeRTOS_VT_CURRENTSENSE_COMPONENT_H */
