/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef NX_VERIFIED_TELEMETRY_H
#define NX_VERIFIED_TELEMETRY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_pnp_client.h"

/* Verified Telemetry Components */
#include "nx_vt_fallcurve_component.h"

#define VT_SIGNATURE_TYPE_FALLCURVE 0x01
#define VT_SIGNATURE_TYPE_CURRENTSENSE 0x02


union NX_VT_SIGNATURE_COMPONENT_UNION_TAG
{
    /* FallCurve Component */
    NX_VT_FALLCURVE_COMPONENT fc;

    /* CurrentSense Component */
    // NX_VT_CURRENTSENSE_COMPONENT cs;

};

typedef struct NX_VT_OBJECT_TAG
{
    /* Signature Object Union */
    union NX_VT_SIGNATURE_COMPONENT_UNION_TAG component;
    
    /* Signature Type */
    UINT signature_type;

    /* Pointer to next component */
    void *next_component;

} NX_VT_OBJECT;


typedef struct NX_VERIFIED_TELEMETRY_DB_TAG
{
    /* Name of this component */
    UCHAR* component_name_ptr;

    UINT component_name_length;

    /* Pointer to first component */
    void *first_component;

    /* Pointer to last component */
    void *last_component;

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

} NX_VERIFIED_TELEMETRY_DB;

// VT Initialization
UINT nx_vt_init(void* verified_telemetry_DB,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    bool enable_verified_telemetry,
    VT_DEVICE_DRIVER* device_driver);

UINT nx_vt_signature_init(void* verified_telemetry_DB,
    NX_VT_OBJECT* handle,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UINT signature_type,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update,
    VT_SENSOR_HANDLE* sensor_handle);

// VT Configuration Command and Properties

UINT nx_vt_process_command(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code);

UINT nx_vt_process_property_update(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);

UINT nx_vt_properties(void* verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr);

UINT nx_vt_send_desired_property_after_boot(
    void* verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr, UINT message_type);

// VT Fingerprint Template Sync

UINT nx_vt_process_reported_property_sync(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);

// VT Create and Send Telemetry JSON with message properties

UINT nx_vt_verified_telemetry_message_create_send(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* pnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    UINT wait_option,
    const UCHAR* telemetry_data,
    UINT data_size);

// VT Compute and Evaluate Sensor Fingerprints

// UINT nx_vt_compute_evaluate_fingerprint_all_sensors(void* verified_telemetry_DB);

// UINT nx_vt_compute_evaluate_fingerprint_one_sensor(void* verified_telemetry_DB, const UCHAR* component_name_ptr, 
//     UINT component_name_length);


#ifdef __cplusplus
}
#endif
#endif /* NX_VERIFIED_TELEMETRY_H */
