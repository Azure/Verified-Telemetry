/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef FREERTOS_VERIFIED_TELEMETRY_H
#define FREERTOS_VERIFIED_TELEMETRY_H

#ifdef __cplusplus
extern "C" {
#endif


#include <string.h>
#include <stdio.h>
#include <time.h>
/* Kernel includes. */

/* Azure Provisioning/IoT Hub library includes */
#include "azure_iot_hub_client.h"
#include "FreeRTOS_vt_fallcurve_component.h"
#include "vt_defs.h"

#define NX_AZURE_IOT_PNP_PROPERTIES                                 0x00000004

#define VT_SIGNATURE_TYPE_FALLCURVE 0x01



union FreeRTOS_VT_SIGNATURE_COMPONENT_UNION_TAG {
    /* FallCurve Component */
    FreeRTOS_VT_FALLCURVE_COMPONENT fc;
};

typedef struct FreeRTOS_VT_OBJECT_TAG
{
    /* Signature Object Union */
    union FreeRTOS_VT_SIGNATURE_COMPONENT_UNION_TAG component;

    /* Signature Type */
    UINT signature_type;

    /* Pointer to next component */
    void* next_component;

} FreeRTOS_VT_OBJECT;

typedef struct FreeRTOS_VERIFIED_TELEMETRY_DB_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[VT_COMPONENT_NAME_MAX_LENGTH];

    UINT component_name_length;

    /* Pointer to first component */
    void* first_component;

    /* Pointer to last component */
    void* last_component;

    /* Number of component*/
    UINT components_num;

    /* Device specific implementations*/
    VT_DEVICE_DRIVER* device_driver;

    /* Enable Verified Telemetry*/
    bool enable_verified_telemetry;

    /* Device Status*/
    bool device_status;

    /* Device Status Property Sent*/
    bool device_status_property_sent;

} FreeRTOS_VERIFIED_TELEMETRY_DB;

AzureIoTResult_t FreeRTOS_vt_init(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    UCHAR* component_name_ptr,
    bool enable_verified_telemetry,
    VT_DEVICE_DRIVER* device_driver);


AzureIoTResult_t FreeRTOS_vt_signature_init(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    FreeRTOS_VT_OBJECT* handle,
    UCHAR* component_name_ptr,
    UINT signature_type,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update,
    VT_SENSOR_HANDLE* sensor_handle);



int32_t prvTelemetryPayloadCreate(char *ucScratchBuffer, 
                                        unsigned long ucScratchBufferSize,
                                        char *telemetryKey, double telemetryValue);


AzureIoTResult_t sendTelemetrySampleDevice(AzureIoTHubClient_t * xAzureIoTHubClient);

AzureIoTResult_t FreeRTOS_vt_verified_telemetry_message_create_send(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
                AzureIoTHubClient_t * xAzureIoTHubClient,
                const UCHAR* component_name_ptr,
                UINT component_name_length,
                UINT wait_option,
                const UCHAR* telemetry_data,
                UINT data_size);

AzureIoTResult_t FreeRTOS_vt_process_command(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    AzureIoTJSONReader_t* json_reader_ptr,
    AzureIoTJSONWriter_t* json_response_ptr,
    UINT* status_code);

AzureIoTResult_t FreeRTOS_vt_properties(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, AzureIoTHubClient_t * xAzureIoTHubClient);

AzureIoTResult_t FreeRTOS_vt_process_property_update(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* xReader,
    UINT version);

AzureIoTResult_t FreeRTOS_vt_compute_evaluate_fingerprint_all_sensors(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB);

AzureIoTResult_t FreeRTOS_vt_send_desired_property_after_boot(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, AzureIoTHubClient_t * xAzureIoTHubClient, UINT message_type);

AzureIoTResult_t FreeRTOS_vt_process_reported_property_sync(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr,
    UINT version);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_VERIFIED_TELEMETRY_H */
