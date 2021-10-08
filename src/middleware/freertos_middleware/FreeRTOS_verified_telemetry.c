
/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "FreeRTOS_verified_telemetry.h"

#include "FreeRTOS.h"
#include "task.h"

/* Azure Provisioning/IoT Hub library includes */
#include "azure_iot_hub_client.h"
#include "azure_iot_hub_client_properties.h"
#include "azure_iot_provisioning_client.h"

/* Azure JSON includes */
#include "azure_iot_json_reader.h"
#include "azure_iot_json_writer.h"

#include "FreeRTOS_verified_telemetry.h"
#include "FreeRTOS_vt_fallcurve_component.h"

#include "vt_debug.h"
#include "vt_defs.h"

#define azureiotPROPERTY_SUCCESS                   "success"
#define azureiotPROPERTY_STATUS_SUCCESS            200
#define azureiotPROPERTY_ENABLE_VERIFIED_TELEMETRY "enableVerifiedTelemetry"
#define PROPERTY_NAME_MAX_LENGTH                   50
#define azureiotMESSAGEPROPERTYSYMBOL              "$.sub"

static const CHAR device_status_property[]             = "deviceStatus";
static const CHAR enable_verified_telemetry_property[] = "enableVerifiedTelemetry";

static uint8_t ucScratchBuffer[256];
static CHAR scratch_buffer[200];

AzureIoTResult_t FreeRTOS_vt_init(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    UCHAR* component_name_ptr,
    bool enable_verified_telemetry,
    VT_DEVICE_DRIVER* device_driver)
{
    strncpy((CHAR*)verified_telemetry_DB->component_name_ptr,
        (CHAR*)component_name_ptr,
        sizeof(verified_telemetry_DB->component_name_ptr));
    verified_telemetry_DB->component_name_length       = strlen((const char*)component_name_ptr);
    verified_telemetry_DB->enable_verified_telemetry   = enable_verified_telemetry;
    verified_telemetry_DB->device_status_property_sent = false;
    verified_telemetry_DB->components_num              = 0;
    verified_telemetry_DB->first_component             = NULL;
    verified_telemetry_DB->last_component              = NULL;

    verified_telemetry_DB->device_driver = device_driver;

    return eAzureIoTSuccess;
}

AzureIoTResult_t FreeRTOS_vt_signature_init(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    FreeRTOS_VT_OBJECT* handle,
    UCHAR* component_name_ptr,
    UINT signature_type,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update,
    VT_SENSOR_HANDLE* sensor_handle)
{
    if (signature_type != VT_SIGNATURE_TYPE_FALLCURVE)
    {
        return eAzureIoTErrorFailed;
    }
    verified_telemetry_DB->components_num++;
    if (verified_telemetry_DB->first_component == NULL)
    {
        verified_telemetry_DB->first_component = (void*)handle;
    }
    else
    {
        ((FreeRTOS_VT_OBJECT*)(verified_telemetry_DB->last_component))->next_component = (void*)handle;
    }
    verified_telemetry_DB->last_component = (void*)handle;

    handle->next_component = NULL;
    handle->signature_type = signature_type;
    if (signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
    {
        FreeRTOS_vt_fallcurve_init(&(handle->component.fc),
            component_name_ptr,
            verified_telemetry_DB->device_driver,
            sensor_handle,
            associated_telemetry,
            telemetry_status_auto_update);
    }
    return eAzureIoTSuccess;
}

AzureIoTResult_t FreeRTOS_vt_verified_telemetry_message_create_send(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    const UCHAR* telemetry_data,
    UINT data_size)

{
    AzureIoTMessageProperties_t telemetrymessageProperties;
    uint8_t telemetrymessagePropertyBuffer[128];
    AzureIoTMessage_PropertiesInit(&telemetrymessageProperties, telemetrymessagePropertyBuffer, 0, 128);
    AzureIoTMessage_PropertiesAppend(&telemetrymessageProperties,
        (const uint8_t*)azureiotMESSAGEPROPERTYSYMBOL,
        sizeof(azureiotMESSAGEPROPERTYSYMBOL) - 1,
        (const uint8_t*)component_name_ptr,
        component_name_length);

    AzureIoTResult_t xResult;
    AzureIoTJSONReader_t json_reader;
    AzureIoTJSONReader_t json_reader_copy;
    UINT components_num     = verified_telemetry_DB->components_num;
    void* component_pointer = verified_telemetry_DB->first_component;
    UCHAR property_name[PROPERTY_NAME_MAX_LENGTH];
    memset(property_name, 0, sizeof(property_name));
    uint32_t bytes_copied = 0;
    CHAR vt_property_name[PROPERTY_NAME_MAX_LENGTH];
    memset(vt_property_name, 0, sizeof(vt_property_name));
    memset(scratch_buffer, 0, sizeof(scratch_buffer));
    UINT token_found     = 0;
    UCHAR* token_pointer = NULL;
    UINT tokens          = 0;

    xResult = AzureIoTJSONReader_Init(&json_reader, telemetry_data, data_size);

    for (UINT i = 0; i < components_num; i++)
    {
        json_reader_copy = json_reader;
        token_found      = 0;
        while (token_found == 0 && AzureIoTJSONReader_NextToken(&json_reader_copy) == eAzureIoTSuccess)
        {
            xResult = AzureIoTJSONReader_GetTokenString(&json_reader_copy, property_name, sizeof(property_name), &bytes_copied);

            if (((FreeRTOS_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
            {
                if ((token_pointer =
                            (UCHAR*)strstr((CHAR*)(((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc.associated_telemetry),
                                (CHAR*)property_name)))
                {
                    memset(vt_property_name, 0, sizeof(vt_property_name));
                    memset(scratch_buffer, 0, sizeof(scratch_buffer));

                    snprintf(vt_property_name, sizeof(vt_property_name), "vT");
                    strcat(vt_property_name, (CHAR*)token_pointer);

                    strcat(scratch_buffer,
                        (((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc.telemetry_status > 0) ? "true" : "false");

                    xResult = AzureIoTMessage_PropertiesAppend(&telemetrymessageProperties,
                        (const uint8_t*)vt_property_name,
                        strlen(vt_property_name),
                        (const uint8_t*)scratch_buffer,
                        strlen(scratch_buffer));
                    configASSERT(xResult == eAzureIoTSuccess);

                    token_found = 1;
                    tokens++;
                }
            }
        }
        component_pointer = (((FreeRTOS_VT_OBJECT*)component_pointer)->next_component);
    }

    if ((xResult = AzureIoTHubClient_SendTelemetry(
             xAzureIoTHubClient, telemetry_data, data_size, &telemetrymessageProperties, eAzureIoTHubMessageQoS1, NULL)))
    {
        printf("Telemetry message send failed!: error code = 0x%08x\r\n", xResult);
        return (xResult);
    }
    return (xResult);
}

AzureIoTResult_t FreeRTOS_vt_process_command(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    UINT* status_code)
{
    AzureIoTResult_t xResult = eAzureIoTErrorFailed;

    UINT iter               = 0;
    UINT components_num     = verified_telemetry_DB->components_num;
    void* component_pointer = verified_telemetry_DB->first_component;

    for (iter = 0; iter < components_num; iter++)
    {
        if (((FreeRTOS_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            if ((xResult = FreeRTOS_vt_fallcurve_process_command(&(((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc),
                     xAzureIoTHubClient,
                     component_name_ptr,
                     component_name_length,
                     pnp_command_name_ptr,
                     pnp_command_name_length,
                     status_code)) == eAzureIoTSuccess)
            {
                VTLogInfo("Successfully executed command %.*s on %.*s component\r\n\n",
                    pnp_command_name_length,
                    pnp_command_name_ptr,
                    component_name_length,
                    component_name_ptr);
                return eAzureIoTSuccess;
            }
        }
        component_pointer = (((FreeRTOS_VT_OBJECT*)component_pointer)->next_component);
    }

    return xResult;
}

static AzureIoTResult_t send_reported_property(AzureIoTHubClient_t* xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    bool status,
    UINT version,
    UCHAR* property_name,
    UINT property_name_length)
{
    AzureIoTJSONWriter_t xWriter;
    uint8_t local_scratch_buffer[256];
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    xResult = AzureIoTJSONWriter_Init(&xWriter, local_scratch_buffer, sizeof(local_scratch_buffer));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderBeginComponent(
        xAzureIoTHubClient, &xWriter, (const uint8_t*)component_name_ptr, component_name_length);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderBeginResponseStatus(xAzureIoTHubClient,
        &xWriter,
        (const uint8_t*)property_name,
        property_name_length,
        azureiotPROPERTY_STATUS_SUCCESS,
        version,
        (const uint8_t*)azureiotPROPERTY_SUCCESS,
        strlen(azureiotPROPERTY_SUCCESS));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBool(&xWriter, status);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderEndResponseStatus(xAzureIoTHubClient, &xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient, &xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendEndObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed(&xWriter);

    if (lBytesWritten < 0)
    {
        LogError(("Error getting the bytes written for the properties confirmation JSON"));
    }
    else
    {
        LogDebug(("Sending acknowledged writable property. Payload: %.*s", lBytesWritten, local_scratch_buffer));
        xResult = AzureIoTHubClient_SendPropertiesReported(xAzureIoTHubClient, local_scratch_buffer, lBytesWritten, NULL);

        if (xResult != eAzureIoTSuccess)
        {
            LogError(("There was an error sending the reported properties: 0x%08x", xResult));
        }
    }

    return xResult;
}

UINT FreeRTOS_vt_device_status_property(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, bool device_status, AzureIoTHubClient_t* xAzureIoTHubClient)
{
    AzureIoTJSONWriter_t xWriter;
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void*)ucScratchBuffer, 0, sizeof(ucScratchBuffer));

    xResult = AzureIoTJSONWriter_Init(&xWriter, ucScratchBuffer, sizeof(ucScratchBuffer));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderBeginComponent(xAzureIoTHubClient,
        &xWriter,
        (const uint8_t*)verified_telemetry_DB->component_name_ptr,
        verified_telemetry_DB->component_name_length);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult =
        AzureIoTJSONWriter_AppendPropertyName(&xWriter, (const uint8_t*)device_status_property, strlen(device_status_property));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBool(&xWriter, device_status);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient, &xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendEndObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed(&xWriter);

    printf(" vt_device_status - %s \n", ucScratchBuffer);

    if (lBytesWritten < 0)
    {
        LogError(("Error getting the bytes written for the properties confirmation JSON"));
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported(xAzureIoTHubClient, ucScratchBuffer, lBytesWritten, NULL);

        if (xResult != eAzureIoTSuccess)
        {
            LogError(("There was an error sending the reported properties: 0x%08x", xResult));
        }
    }

    return xResult;
}

AzureIoTResult_t FreeRTOS_vt_send_desired_property_after_boot(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, AzureIoTHubClient_t* xAzureIoTHubClient, UINT message_type)
{
    AzureIoTResult_t xResult;

    if (message_type == FREERTOS_AZURE_IOT_PNP_PROPERTIES)
    {
        xResult = send_reported_property(xAzureIoTHubClient,
            verified_telemetry_DB->component_name_ptr,
            verified_telemetry_DB->component_name_length,
            verified_telemetry_DB->enable_verified_telemetry,
            1,
            (UCHAR*)enable_verified_telemetry_property,
            sizeof(enable_verified_telemetry_property) - 1);
        configASSERT(xResult == eAzureIoTSuccess);

        printf("enable Verified Telemetry property %d \n", verified_telemetry_DB->enable_verified_telemetry);
    }
    return eAzureIoTSuccess;
}

AzureIoTResult_t FreeRTOS_vt_process_reported_property_sync(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr)
{
    UINT iter               = 0;
    UINT components_num     = verified_telemetry_DB->components_num;
    void* component_pointer = verified_telemetry_DB->first_component;
    for (iter = 0; iter < components_num; iter++)
    {
        if (((FreeRTOS_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            if (FreeRTOS_vt_fallcurve_process_reported_property_sync(&(((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc),
                    component_name_ptr,
                    component_name_length,
                    json_reader_ptr) == eAzureIoTSuccess)
            {
                return eAzureIoTSuccess;
            }
        }
        component_pointer = (((FreeRTOS_VT_OBJECT*)component_pointer)->next_component);
    }
    return eAzureIoTErrorFailed;
}

AzureIoTResult_t FreeRTOS_vt_compute_evaluate_fingerprint_all_sensors(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB)
{
    AzureIoTResult_t xResult       = eAzureIoTErrorFailed;
    UINT iter                      = 0;
    UINT components_num            = verified_telemetry_DB->components_num;
    void* component_pointer        = verified_telemetry_DB->first_component;
    bool enable_verified_telemetry = verified_telemetry_DB->enable_verified_telemetry;
    for (iter = 0; iter < components_num; iter++)
    {
        if (((FreeRTOS_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {

            xResult = FreeRTOS_vt_fallcurve_compute_sensor_status_global(
                &(((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc), enable_verified_telemetry);
            configASSERT(xResult == eAzureIoTSuccess);
        }
        component_pointer = (((FreeRTOS_VT_OBJECT*)component_pointer)->next_component);
    }
    return xResult;
}

AzureIoTResult_t FreeRTOS_vt_process_property_update(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* xReader,
    UINT version)
{
    bool parsed_value = false;
    AzureIoTResult_t xResult;

    if (verified_telemetry_DB->component_name_length != component_name_length ||
        strncmp((CHAR*)verified_telemetry_DB->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        return (eAzureIoTErrorFailed);
    }

    if (AzureIoTJSONReader_TokenIsTextEqual(xReader,
            (const uint8_t*)azureiotPROPERTY_ENABLE_VERIFIED_TELEMETRY,
            strlen(azureiotPROPERTY_ENABLE_VERIFIED_TELEMETRY)))
    {
        xResult = AzureIoTJSONReader_NextToken(xReader);
        configASSERT(xResult == eAzureIoTSuccess);

        /* Get desired temperature */
        xResult = AzureIoTJSONReader_GetTokenBool(xReader, &parsed_value);
        configASSERT(xResult == eAzureIoTSuccess);

        xResult = AzureIoTJSONReader_NextToken(xReader);
        configASSERT(xResult == eAzureIoTSuccess);

        verified_telemetry_DB->enable_verified_telemetry = (bool)parsed_value;
        VTLogInfo("Received Enable Verified Telemetry Twin update with value %s\r\n", (bool)parsed_value ? "true" : "false");

        xResult = send_reported_property(xAzureIoTHubClient,
            component_name_ptr,
            component_name_length,
            (bool)parsed_value,
            version,
            (UCHAR*)"enableVerifiedTelemetry",
            strlen("enableVerifiedTelemetry"));
        configASSERT(xResult == eAzureIoTSuccess);

        printf("response done");

        return eAzureIoTSuccess;
    }
    else
    {
        xResult = AzureIoTJSONReader_NextToken(xReader);
        configASSERT(xResult == eAzureIoTSuccess);

        xResult = AzureIoTJSONReader_SkipChildren(xReader);
        configASSERT(xResult == eAzureIoTSuccess);

        xResult = AzureIoTJSONReader_NextToken(xReader);
        configASSERT(xResult == eAzureIoTSuccess);
    }

    return eAzureIoTErrorFailed;
}

AzureIoTResult_t FreeRTOS_vt_properties(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, AzureIoTHubClient_t* xAzureIoTHubClient)
{
    AzureIoTResult_t xResult = eAzureIoTErrorFailed;

    UINT components_num     = verified_telemetry_DB->components_num;
    void* component_pointer = verified_telemetry_DB->first_component;
    bool device_status      = true;
    for (UINT i = 0; i < components_num; i++)
    {

        if (((FreeRTOS_VT_OBJECT*)component_pointer)->signature_type == VT_SIGNATURE_TYPE_FALLCURVE)
        {
            if ((xResult = FreeRTOS_vt_fallcurve_telemetry_status_property(
                     &(((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc), xAzureIoTHubClient, &device_status)))
            {
                VTLogError("Failed FreeRTOS_vt_fallcurve_telemetry_status_property for component %.*s: error code = "
                           "0x%08x\r\n\n",
                    (INT)((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc.component_name_length,
                    (CHAR*)((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc.component_name_ptr,
                    xResult);
            }

            if (((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc.property_sent == 0)
            {
                if ((xResult = FreeRTOS_vt_fallcurve_fingerprint_type_property(
                         &(((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc), xAzureIoTHubClient)))
                {
                    VTLogError("Failed FreeRTOS_vt_fallcurve_fingerprint_type_property: error code = 0x%08x\r\n", xResult);
                }

                ((FreeRTOS_VT_OBJECT*)component_pointer)->component.fc.property_sent = 1;
            }
        }
        component_pointer = (((FreeRTOS_VT_OBJECT*)component_pointer)->next_component);
    }

    if (device_status != verified_telemetry_DB->device_status || verified_telemetry_DB->device_status_property_sent == 0)
    {

        verified_telemetry_DB->device_status = device_status;
        if ((xResult = FreeRTOS_vt_device_status_property(
                 (FreeRTOS_VERIFIED_TELEMETRY_DB*)verified_telemetry_DB, device_status, xAzureIoTHubClient)))
        {
            VTLogError("Failed pnp_vt_device_status_property: error code = 0x%08x\r\n", xResult);
        }
        else if (verified_telemetry_DB->device_status_property_sent == 0)
        {
            verified_telemetry_DB->device_status_property_sent = 1;
        }
    }

    return xResult;
}
