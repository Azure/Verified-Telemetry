/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <stdio.h>
#include "pnp_verified_telemetry.h"

#define DB_Size 110

static const CHAR enableVerifiedTelemetry_property[] = "enableVerifiedTelemetry";
static const CHAR deviceStatus_property[] = "deviceStatus";

static const CHAR temp_response_description_success[] = "success";
static const CHAR temp_response_description_failed[] = "failed";

static VOID send_reported_property( NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr, bool status,
                                    INT status_code, UINT version, const CHAR *description, UCHAR *property_name, UINT property_name_length)
{
    UINT response_status;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    if (nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr,
                                                           &json_writer, NX_WAIT_FOREVER))
    {
        printf("Failed to build reported response\r\n");
        return;
    }

    if (nx_azure_iot_pnp_client_reported_property_status_begin(iotpnp_client_ptr,
                                                               &json_writer, (UCHAR *)property_name,
                                                               property_name_length,
                                                               status_code, version,
                                                               (const UCHAR *)description, strlen(description)) ||
        nx_azure_iot_json_writer_append_bool(&json_writer,status) ||
        nx_azure_iot_pnp_client_reported_property_status_end(iotpnp_client_ptr, &json_writer))
    {
        nx_azure_iot_json_writer_deinit(&json_writer);
        printf("Failed to build reported response\r\n");
    }
    else
    {
        if (nx_azure_iot_pnp_client_reported_properties_send(iotpnp_client_ptr,
                                                             &json_writer, NX_NULL,
                                                             &response_status, NX_NULL,
                                                             (5 * NX_IP_PERIODIC_RATE)))
        {
            printf("Failed to send reported response\r\n");
        }

        nx_azure_iot_json_writer_deinit(&json_writer);
    }
}

UINT pnp_vt_deviceStatus_property(bool deviceStatus, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr,
                                                                     &json_writer, NX_WAIT_FOREVER)))
    {
        printf("Failed create reported properties: error code = 0x%08x\r\n", status);
        return(status);
    }

    if ((status = nx_azure_iot_json_writer_append_property_with_bool_value(&json_writer,
                                                                             (const UCHAR *)deviceStatus_property,
                                                                             sizeof(deviceStatus_property) - 1,
                                                                             deviceStatus)))
    {
        printf("Failed to build reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_properties_send(iotpnp_client_ptr,
                                                                   &json_writer,
                                                                   NX_NULL, &response_status,
                                                                   NX_NULL,
                                                                   (5 * NX_IP_PERIODIC_RATE))))
    {
        printf("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);

    if ((response_status < 200) || (response_status >= 300))
    {
        printf("device twin report properties failed with code : %d\r\n", response_status);
        return(NX_NOT_SUCCESSFUL);
    }

    return(status);
}

UINT pnp_vt_process_command(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                        UCHAR *component_name_ptr, UINT component_name_length, UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length, NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                        NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code)
{

    UINT status = 0;
    UINT iter = 0;
    PNP_FALLCURVE_COMPONENT **fallcurve_components = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> fallcurve_components;
    UINT fallcurve_components_num = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> fallcurve_components_num;

    for (iter = 0; iter < fallcurve_components_num; iter++ )
    {
        if ((status = pnp_fallcurve_process_command(    fallcurve_components[iter], 
                                                        iotpnp_client_ptr, component_name_ptr, 
                                                        component_name_length,
                                                        pnp_command_name_ptr, pnp_command_name_length,
                                                        json_reader_ptr, json_response_ptr, status_code)) == NX_AZURE_IOT_SUCCESS)
        {
            printf("Successfully executed command %.*s on %.*s component\r\n", pnp_command_name_length, pnp_command_name_ptr, component_name_length, component_name_ptr);
            return NX_AZURE_IOT_SUCCESS;
        }
    }
    
    return NX_NOT_SUCCESSFUL;

}

UINT pnp_vt_process_property_update(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                                   const UCHAR *component_name_ptr, UINT component_name_length,
                                                   NX_AZURE_IOT_JSON_READER *name_value_reader_ptr, UINT version)
{                                                   
    UINT parsed_value = 0;
    INT status_code;
    const CHAR *description;
    CHAR property_name[50];
    UINT property_name_length = 0;

    if (!component_name_ptr)
    {
        //Get Property Name
        nx_azure_iot_json_reader_token_string_get(name_value_reader_ptr, (UCHAR*)property_name, sizeof(property_name), &property_name_length);

        //Property 1: Enable Verified Telemetry
        if (nx_azure_iot_json_reader_token_is_text_equal(name_value_reader_ptr,
                                                        (UCHAR *)enableVerifiedTelemetry_property,
                                                        sizeof(enableVerifiedTelemetry_property) - 1) == NX_TRUE)
        {
            if (nx_azure_iot_json_reader_next_token(name_value_reader_ptr) ||
                nx_azure_iot_json_reader_token_bool_get(name_value_reader_ptr, &parsed_value))
            {
                status_code = 401;
                description = temp_response_description_failed;
            }
            else
            {
                status_code = 200;
                description = temp_response_description_success;

                ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> enableVerifiedTelemetry = (bool)parsed_value;
                printf("Received Enable Verified Telemetry Twin update with value %s\r\n", (bool)parsed_value ? "true" : "false");
            }
            send_reported_property( iotpnp_client_ptr, (bool)parsed_value,
                                    status_code, version, description, (UCHAR *) property_name, property_name_length);
            return NX_AZURE_IOT_SUCCESS;
        }
    }

    return NX_NOT_SUCCESSFUL;

}

UINT pnp_vt_process_reported_property_sync(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                                   const UCHAR *component_name_ptr, UINT component_name_length,
                                                   NX_AZURE_IOT_JSON_READER *name_value_reader_ptr, UINT version)
{                                                   
    UINT iter = 0;
    PNP_FALLCURVE_COMPONENT **fallcurve_components = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> fallcurve_components;
    UINT fallcurve_components_num = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> fallcurve_components_num;
    for (iter = 0; iter < fallcurve_components_num; iter++ )
    {
        if (pnp_fallcurve_process_reported_property_sync(fallcurve_components[iter],
                                                    iotpnp_client_ptr,
                                                    component_name_ptr, component_name_length,
                                                    name_value_reader_ptr, version) == NX_AZURE_IOT_SUCCESS)
        {
            printf("property synced for %.*s component\r\n", component_name_length, component_name_ptr);
            return NX_AZURE_IOT_SUCCESS;
        }
    }
    return NX_NOT_SUCCESSFUL;
}

UINT pnp_vt_properties(void *verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr)
{
    UINT status = 0;

    PNP_FALLCURVE_COMPONENT **fallcurve_components = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> fallcurve_components;
    UINT fallcurve_components_num = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> fallcurve_components_num;
    bool enableVerifiedTelemetry = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> enableVerifiedTelemetry;
    bool deviceStatus = true;
    for (UINT i = 0; i < fallcurve_components_num; i++)
    {
        
        if ((status = pnp_fallcurve_telemetryStatus_property(fallcurve_components[i], iotpnp_client_ptr, enableVerifiedTelemetry, &deviceStatus)))
        {
            printf("Failed pnp_fallcurve_telemetryStatus_property: error code = 0x%08x\r\n", status);
        }

        if ((fallcurve_components[i] -> vTInfo_property_sent) == 0)
        {
            if ((status = pnp_fallcurve_fingerprintType_property(fallcurve_components[i], iotpnp_client_ptr)))
            {
                printf("Failed pnp_fingerprintType_property: error code = 0x%08x\r\n", status);
            }
            if ((status = pnp_fallcurve_telemetryStatus_property(fallcurve_components[i], iotpnp_client_ptr, enableVerifiedTelemetry, &deviceStatus)))
            {
                printf("Failed pnp_fallcurve_telemetryStatus_property: error code = 0x%08x\r\n", status);
            }

            fallcurve_components[i] -> vTInfo_property_sent = 1;
        }
        
    }
    
    if(deviceStatus != ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> deviceStatus || ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> deviceStatusProperty_sent == 0)
    {
        
        ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> deviceStatus = deviceStatus;
        if ((status = pnp_vt_deviceStatus_property(deviceStatus, iotpnp_client_ptr)))
        {
            printf("Failed pnp_vt_deviceStatus_property: error code = 0x%08x\r\n", status);
        }   
        else if(((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> deviceStatusProperty_sent == 0)
        {
            ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB) -> deviceStatusProperty_sent = 1; 
        }
    }

    return status;
}

UINT pnp_vt_init(void *verified_telemetry_DB, PNP_FALLCURVE_COMPONENT **fallcurve_components,
                                UINT numberVerifiedTelemetries, bool enableVerifiedTelemetry, UINT flash_address)
{
    VERIFIED_TELEMETRY_DB *VT_DB = ((VERIFIED_TELEMETRY_DB *) verified_telemetry_DB);
    VT_DB -> fallcurve_components = fallcurve_components;
    VT_DB -> fallcurve_components_num = numberVerifiedTelemetries;
    VT_DB -> enableVerifiedTelemetry = enableVerifiedTelemetry;
    VT_DB -> deviceStatusProperty_sent = false;

    for (UINT i = 0; i < VT_DB -> fallcurve_components_num; i++)
    {
        vt_database_initialize(&(fallcurve_components[i]->fingerprintdb), flash_address, (i+1));
        fallcurve_components[i]->flash_address = flash_address;
    }

    return NX_AZURE_IOT_SUCCESS;
}