/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <stdio.h>

#include "nx_vt_middleware_helper.h"
#include "nx_verified_telemetry.h"
#include "vt_debug.h"

static const CHAR enable_verified_telemetry_property[] = "enableVerifiedTelemetry";
static const CHAR device_status_property[]            = "deviceStatus";

static const CHAR temp_response_description_success[] = "success";
static const CHAR temp_response_description_failed[]  = "failed";

static CHAR scratch_buffer[200];

static VOID send_reported_property(NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    bool status,
    INT status_code,
    UINT version,
    const CHAR* description,
    UCHAR* property_name,
    UINT property_name_length)
{
    UINT response_status;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    if (nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER))
    {
        VTLogError("Failed to build reported response\r\n");
        return;
    }

    if (nx_azure_iot_pnp_client_reported_property_component_begin(
            iotpnp_client_ptr, &json_writer, component_name_ptr, component_name_length) ||
        nx_azure_iot_pnp_client_reported_property_status_begin(iotpnp_client_ptr,
            &json_writer,
            (UCHAR*)property_name,
            property_name_length,
            status_code,
            version,
            (const UCHAR*)description,
            strlen(description)) ||
        nx_azure_iot_json_writer_append_bool(&json_writer, status) ||
        nx_azure_iot_pnp_client_reported_property_status_end(iotpnp_client_ptr, &json_writer) ||
        nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr, &json_writer))
    {
        nx_azure_iot_json_writer_deinit(&json_writer);
        VTLogError("Failed to build reported response\r\n");
    }
    else
    {
        if (nx_azure_iot_pnp_client_reported_properties_send(
                iotpnp_client_ptr, &json_writer, NX_NULL, &response_status, NX_NULL, (5 * NX_IP_PERIODIC_RATE)))
        {
            VTLogError("Failed to send reported response\r\n");
        }

        nx_azure_iot_json_writer_deinit(&json_writer);
    }
}

UINT nx_vt_device_status_property(
    NX_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, bool device_status, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr, &json_writer, NX_WAIT_FOREVER)))
    {
        VTLogError("Failed create reported properties: error code = 0x%08x\r\n", status);
        return (status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(iotpnp_client_ptr,
             &json_writer,
             verified_telemetry_DB->component_name_ptr,
             verified_telemetry_DB->component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_bool_value(
             &json_writer, (const UCHAR*)device_status_property, sizeof(device_status_property) - 1, device_status)) ||
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

UINT nx_vt_process_command(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code)
{

    UINT status = 0;
    UINT iter   = 0;
    UINT components_num = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->components_num;
    void* component_pointer = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->first_component;

    for (iter = 0; iter < components_num; iter++)
    {
        printf("[VT CS] Command checkpoint 1)\r\n");
        if(((NX_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            printf("[VT CS] Command checkpoint 2)\r\n");
            if ((status = nx_vt_fallcurve_process_command(&(((NX_VT_OBJECT*)component_pointer)->component.fc),
                 iotpnp_client_ptr,
                 component_name_ptr,
                 component_name_length,
                 pnp_command_name_ptr,
                 pnp_command_name_length,
                 json_reader_ptr,
                 json_response_ptr,
                 status_code)) == NX_AZURE_IOT_SUCCESS)
            {
                VTLogInfo("Successfully executed command %.*s on %.*s component\r\n\n",
                    pnp_command_name_length,
                    pnp_command_name_ptr,
                    component_name_length,
                    component_name_ptr);
                return NX_AZURE_IOT_SUCCESS;
            }
        }
        component_pointer = (((NX_VT_OBJECT*)component_pointer)->next_component);
    }

    return NX_NOT_SUCCESSFUL;
}

UINT nx_vt_process_property_update(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version)
{
    UINT parsed_value = 0;
    INT status_code;
    const CHAR* description;
    CHAR property_name[50];
    UINT property_name_length = 0;

    if (((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->component_name_length != component_name_length ||
        strncmp((CHAR*)((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->component_name_ptr,
            (CHAR*)component_name_ptr,
            component_name_length) != 0)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    // Get Property Name
    nx_azure_iot_json_reader_token_string_get(
        name_value_reader_ptr, (UCHAR*)property_name, sizeof(property_name), &property_name_length);

    // Property 1: Enable Verified Telemetry
    if (nx_azure_iot_json_reader_token_is_text_equal(name_value_reader_ptr,
            (UCHAR*)enable_verified_telemetry_property,
            sizeof(enable_verified_telemetry_property) - 1) == NX_TRUE)
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

            ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->enable_verified_telemetry = (bool)parsed_value;
            VTLogInfo("Received Enable Verified Telemetry Twin update with value %s\r\n",
                (bool)parsed_value ? "true" : "false");
        }
        send_reported_property(iotpnp_client_ptr,
            component_name_ptr,
            component_name_length,
            (bool)parsed_value,
            status_code,
            version,
            description,
            (UCHAR*)property_name,
            property_name_length);
        return NX_AZURE_IOT_SUCCESS;
    }

    return NX_NOT_SUCCESSFUL;
}

UINT nx_vt_process_reported_property_sync(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version)
{
    UINT iter = 0;
    UINT components_num = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->components_num;
    void* component_pointer = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->first_component;
    for (iter = 0; iter < components_num; iter++)
    {
        if(((NX_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            if (nx_vt_fallcurve_process_reported_property_sync(&(((NX_VT_OBJECT*)component_pointer)->component.fc),
                    iotpnp_client_ptr,
                    component_name_ptr,
                    component_name_length,
                    name_value_reader_ptr,
                    version) == NX_AZURE_IOT_SUCCESS)
            {
                return NX_AZURE_IOT_SUCCESS;
            }
        }
        component_pointer = (((NX_VT_OBJECT*)component_pointer)->next_component);
    }
    return NX_NOT_SUCCESSFUL;
}

UINT nx_vt_send_desired_property_after_boot(
    void* verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr, UINT message_type)
{
    if (message_type == NX_AZURE_IOT_PNP_PROPERTIES)
    {
        send_reported_property(iotpnp_client_ptr,
            ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->component_name_ptr,
            ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->component_name_length,
            ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->enable_verified_telemetry,
            200,
            1,
            temp_response_description_success,
            (UCHAR*)enable_verified_telemetry_property,
            sizeof(enable_verified_telemetry_property) - 1);
    }
    return NX_AZURE_IOT_SUCCESS;
}

UINT nx_vt_properties(void* verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr)
{
    UINT status = 0;

    UINT components_num = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->components_num;
    void* component_pointer = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->first_component;
    bool enable_verified_telemetry  = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->enable_verified_telemetry;
    bool device_status             = true;
    for (UINT i = 0; i < components_num; i++)
    {

        if(((NX_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            if ((status = nx_vt_fallcurve_telemetry_status_property(
                &(((NX_VT_OBJECT*)component_pointer)->component.fc), iotpnp_client_ptr, enable_verified_telemetry, &device_status)))
            {
                VTLogError("Failed nx_vt_fallcurve_telemetry_status_property for component %.*s: error code = "
                        "0x%08x\r\n\n",
                    (INT)((NX_VT_OBJECT*)component_pointer)->component.fc.component_name_length,
                    (CHAR*)((NX_VT_OBJECT*)component_pointer)->component.fc.component_name_ptr,
                    status);
            }

            if (((NX_VT_OBJECT*)component_pointer)->component.fc.property_sent == 0)
            {
                if ((status = nx_vt_fallcurve_fingerprint_type_property(&(((NX_VT_OBJECT*)component_pointer)->component.fc), iotpnp_client_ptr)))
                {
                    VTLogError("Failed nx_vt_fallcurve_fingerprint_type_property: error code = 0x%08x\r\n", status);
                }

                ((NX_VT_OBJECT*)component_pointer)->component.fc.property_sent = 1;
            }
        }
        component_pointer = (((NX_VT_OBJECT*)component_pointer)->next_component);
        
    }

    if (device_status != ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->device_status ||
        ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->device_status_property_sent == 0)
    {

        ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->device_status = device_status;
        if ((status = nx_vt_device_status_property(
                 (NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB, device_status, iotpnp_client_ptr)))
        {
            VTLogError("Failed pnp_vt_device_status_property: error code = 0x%08x\r\n", status);
        }
        else if (((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->device_status_property_sent == 0)
        {
            ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->device_status_property_sent = 1;
        }
    }

    return status;
}

UINT nx_vt_init(void* verified_telemetry_DB,
    UCHAR* component_name_ptr,
    bool enable_verified_telemetry,
    VT_DEVICE_DRIVER* device_driver)
{
    printf("[VT CS] Entered vt_init()\r\n");
    NX_VERIFIED_TELEMETRY_DB* VT_DB     = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB);
    strncpy((CHAR*)VT_DB->component_name_ptr, (CHAR*)component_name_ptr, sizeof(VT_DB->component_name_ptr));
    VT_DB->component_name_length     = strlen((const char*)component_name_ptr);
    VT_DB->enable_verified_telemetry   = enable_verified_telemetry;
    VT_DB->device_status_property_sent = false;
    VT_DB->components_num = 0;
    VT_DB->first_component = NULL;
    VT_DB->last_component = NULL;

    VT_DB->device_driver = device_driver;
    printf("[VT CS] Leaving vt_init()\r\n");

    return NX_AZURE_IOT_SUCCESS;
}

UINT nx_vt_signature_init(void* verified_telemetry_DB,
    NX_VT_OBJECT* handle,
    UCHAR* component_name_ptr,
    UINT signature_type,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update,
    VT_SENSOR_HANDLE* sensor_handle)
{
    printf("[VT CS] Entered vt_signature_init()\r\n");
    if(signature_type != VT_SIGNATURE_TYPE_FALLCURVE && signature_type != VT_SIGNATURE_TYPE_CURRENTSENSE)
    {
        return NX_AZURE_IOT_FAILURE;
    }
    printf("[VT CS] Pointer stuff starting\r\n");
    NX_VERIFIED_TELEMETRY_DB* VT_DB  = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB);
    VT_DB->components_num++;
    if(VT_DB->first_component == NULL)
    {
        VT_DB->first_component = (void*) handle;
    }
    else
    {
        ((NX_VT_OBJECT*)(VT_DB->last_component))->next_component = (void*) handle;
    }
    VT_DB->last_component = (void*) handle;

    handle->next_component = NULL;
    handle->signature_type = signature_type;
    printf("[VT CS] Pointer stuff end\r\n");
    if(signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
    {
        printf("[VT CS] Entering vt_currentsense_init()\r\n");
        nx_vt_fallcurve_init(&(handle->component.fc),
            component_name_ptr,
            VT_DB->device_driver,
            sensor_handle,
            associated_telemetry,
            telemetry_status_auto_update);
        printf("[VT CS] Left vt_currentsense_init()\r\n");
    }
    printf("[VT CS] Leaving vt_signature_init()\r\n");
    // else if(signature_type == VT_SIGNATURE_TYPE_CURRENTSENSE)
    // {
    //     //implement code
    // }
    return NX_AZURE_IOT_SUCCESS;
}

UINT nx_vt_verified_telemetry_message_create_send(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* pnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    UINT wait_option,
    const UCHAR* telemetry_data,
    UINT data_size)
{
    UINT status;
    NX_PACKET* packet_ptr;
    NX_AZURE_IOT_JSON_READER json_reader;
    NX_AZURE_IOT_JSON_READER json_reader_copy;
    UINT components_num = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->components_num;
    void* component_pointer = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->first_component;
    UCHAR property_name[50];
    memset(property_name, 0, sizeof(property_name));
    UINT bytes_copied = 0;
    CHAR vt_property_name[50];
    memset(vt_property_name, 0, sizeof(vt_property_name));
    memset(scratch_buffer, 0, sizeof(scratch_buffer));
    UINT token_found = 0;
    UINT tokens      = 0;

    nx_azure_iot_json_reader_with_buffer_init(&json_reader, telemetry_data, data_size);

    for (UINT i = 0; i < components_num; i++)
    {
        json_reader_copy = json_reader;
        token_found      = 0;
        while (token_found == 0 && nx_azure_iot_json_reader_next_token(&json_reader_copy) == NX_AZURE_IOT_SUCCESS)
        {
            nx_azure_iot_json_reader_token_string_get(&json_reader_copy, property_name, 
                                                        sizeof(property_name), &bytes_copied);
            if(((NX_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
            {
                if(strstr((CHAR*)(((NX_VT_OBJECT*)component_pointer)->component.fc.associated_telemetry), (CHAR*)property_name))
                {
                    snprintf(vt_property_name, sizeof(vt_property_name), "vT");
                    strcat(vt_property_name, (CHAR*)property_name);
                    if (tokens > 0)
                    {
                        strcat(scratch_buffer, "&");
                    }
                    strcat(scratch_buffer, vt_property_name);
                    strcat(scratch_buffer, "=");
                    strcat(scratch_buffer, (((NX_VT_OBJECT*)component_pointer)->component.fc.telemetry_status > 0) ? "true" : "false");
                    token_found = 1;
                    tokens++;
                }
            }
            
        }
        component_pointer = (((NX_VT_OBJECT*)component_pointer)->next_component);
    }
    /* Create a telemetry message packet. */
    if ((status = nx_azure_iot_pnp_client_telemetry_message_create_with_message_property(pnp_client_ptr,
             component_name_ptr,
             component_name_length,
             &packet_ptr,
             wait_option,
             (UCHAR*)scratch_buffer,
             strlen(scratch_buffer))))
    {
        printf("Telemetry message with message properties create failed!: error code = 0x%08x\r\n", status);
        return (status);
    }
    if ((status = nx_azure_iot_pnp_client_telemetry_send(
             pnp_client_ptr, packet_ptr, telemetry_data, data_size, wait_option)))
    {
        printf("Telemetry message send failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_pnp_client_telemetry_message_delete(packet_ptr);
        return (status);
    }
    return (status);
}

UINT nx_vt_compute_evaluate_fingerprint_all_sensors(void* verified_telemetry_DB)
{
    UINT status = 0;
    UINT iter = 0;
    UINT components_num = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->components_num;
    void* component_pointer = ((NX_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB)->first_component;
    for (iter = 0; iter < components_num; iter++)
    {
        if(((NX_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            status |= nx_vt_fallcurve_compute_sensor_status_global(&(((NX_VT_OBJECT*)component_pointer)->component.fc));
        }
        component_pointer = (((NX_VT_OBJECT*)component_pointer)->next_component);
    }
    return status;
}