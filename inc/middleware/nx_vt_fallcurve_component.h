/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef NX_VT_FALLCURVE_COMPONENT_H
#define NX_VT_FALLCURVE_COMPONENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_pnp_client.h"

#include "vt_fc_api.h"

typedef struct NX_VT_FALLCURVE_COMPONENT_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[30];

    /* Component Name Length */
    UINT component_name_length;

    /* Telemetry associated with the fallcurve signature component*/
    UCHAR associated_telemetry[100];

    /* Status of the telemetry associated with the fallcurve signature component*/
    bool telemetry_status;

    /* Currentsense Object */
    VT_FALLCURVE_OBJECT fc_object;

    /* Status of reported properties sent  */
    UINT property_sent;

    /* Stores on the scale of 0-100 how much Fingerprint Template can be trusted */
    UINT template_confidence_metric;

    /* Compute sensor status when a global command is issued */
    bool telemetry_status_auto_update;

} NX_VT_FALLCURVE_COMPONENT;

UINT nx_vt_fallcurve_init(NX_VT_FALLCURVE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update);

UINT nx_vt_fallcurve_telemetry_status_property(NX_VT_FALLCURVE_COMPONENT* handle, 
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    bool* device_status);

UINT nx_vt_fallcurve_fingerprint_type_property(
    NX_VT_FALLCURVE_COMPONENT* handle, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr);

UINT nx_vt_fallcurve_process_command(NX_VT_FALLCURVE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code);

UINT nx_vt_fallcurve_process_reported_property_sync(NX_VT_FALLCURVE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);


UINT nx_vt_fallcurve_compute_sensor_status_global(NX_VT_FALLCURVE_COMPONENT* handle, bool toggle_verified_telemetry);

// UINT nx_vt_fallcurve_compute_sensor_status(NX_VT_FALLCURVE_COMPONENT* handle);


#ifdef __cplusplus
}
#endif
#endif /* NX_VT_FALLCURVE_COMPONENT_H */