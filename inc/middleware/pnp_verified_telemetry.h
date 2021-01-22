/* Copyright (c) Microsoft Corporation. */

#ifndef PNP_VERIFIED_TELEMETRY_H
#define PNP_VERIFIED_TELEMETRY_H

#ifdef __cplusplus
extern   "C" {
#endif

#include "nx_azure_iot_pnp_client.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"	

/* Verified Telemetry Components */
#include "pnp_fallcurve_component.h"

typedef struct VERIFIED_TELEMETRY_DB_TAG
{
    /* FallCurve components Array*/
    PNP_FALLCURVE_COMPONENT **fallcurve_components;

    /* FallCurve components Array Length*/
    UINT fallcurve_components_num;

    /* Enable Verified Telemetry*/
    bool enableVerifiedTelemetry;

    /* Device Status*/
    bool deviceStatus;

    /* Device Status Property Sent*/
    bool deviceStatusProperty_sent;

} VERIFIED_TELEMETRY_DB;

UINT pnp_vt_process_command(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                        UCHAR *component_name_ptr, UINT component_name_length, 
                                        UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length, 
                                        NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                        NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code);

UINT pnp_vt_process_property_update(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                                   const UCHAR *component_name_ptr, UINT component_name_length,
                                                   NX_AZURE_IOT_JSON_READER *name_value_reader_ptr, UINT version);

UINT pnp_vt_process_reported_property_sync(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                                   const UCHAR *component_name_ptr, UINT component_name_length,
                                                   NX_AZURE_IOT_JSON_READER *name_value_reader_ptr, UINT version);


UINT pnp_vt_properties(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr);


UINT pnp_vt_init(void *verified_telemetry_DB, PNP_FALLCURVE_COMPONENT **fallcurve_components,
                                UINT numberVerifiedTelemetries, bool enableVerifiedTelemetry, UINT flash_address);
                                

#ifdef __cplusplus
}
#endif
#endif /* PNP_VERIFIED_TELEMETRY_H */