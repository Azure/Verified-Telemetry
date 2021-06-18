/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef NX_VT_CURRENTSENSE_COMPONENT_H
#define NX_VT_CURRENTSENSE_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_pnp_client.h"

#include "vt_cs_api.h"

typedef struct NX_VT_CURRENTSENSE_COMPONENT_TAG
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

} NX_VT_CURRENTSENSE_COMPONENT;

UINT nx_vt_currentsense_init(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    UINT* shared_buffer,
    UINT shared_buffer_size);

UINT nx_vt_currentsense_reported_properties(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    bool* device_status,
    bool toggle_verified_telemetry);

UINT nx_vt_currentsense_process_command(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code);

UINT nx_vt_currentsense_process_reported_property_sync(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);

UINT nx_vt_currentsense_signature_read(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry);

UINT nx_vt_currentsense_signature_process(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry);

bool nx_vt_currentsense_fetch_telemetry_status(NX_VT_CURRENTSENSE_COMPONENT* handle, bool toggle_verified_telemetry)

#ifdef __cplusplus
}
#endif
#endif /* NX_VT_CURRENTSENSE_COMPONENT_H */
