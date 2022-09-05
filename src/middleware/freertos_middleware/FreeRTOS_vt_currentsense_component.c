/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "FreeRTOS_vt_currentsense_component.h"

#include "FreeRTOS.h"
#include "task.h"

/* Azure Provisioning/IoT Hub library includes */
#include "azure_iot_hub_client.h"
#include "azure_iot_hub_client_properties.h"

/* Azure JSON includes */
#include "azure_iot_json_reader.h"
#include "azure_iot_json_writer.h"

#include "FreeRTOS_verified_telemetry.h"
#include "FreeRTOS_vt_currentsense_component.h"

#include "vt_debug.h"
#include "vt_defs.h"
#include "vt_cs_api.h"

#include <stdlib.h>

#define LOCAL_BUFFER_SIZE 64

static const CHAR command_reset_fingerprint[]   = "setResetFingerprintTemplate";
static const CHAR command_retrain_fingerprint[] = "retrainFingerprintTemplate";
static uint8_t ucPropertyPayloadBuffer[1024];

static const CHAR telemetry_name_telemetry_status[]     = "telemetryStatus";
static const CHAR fingerprint_type_property[]           = "fingerprintType";
static const CHAR template_confidence_metric_property[] = "fingerprintTemplateConfidenceMetric";
static const CHAR fingerprint_type_value[]              = "CurrentSense";
static const CHAR fingerprint_template_property[]       = "fingerprintTemplate";
static const CHAR template_type_json_property[]            = "templateType";
static const CHAR average_current_on_json_property[]       = "averageCurrentOn";
static const CHAR average_current_off_json_property[]      = "averageCurrentOff";
static const CHAR num_repeating_signatures_json_property[] = "numRepeatingSignatures";
static const CHAR offset_curr_json_property[]              = "offsetCurr";
static const CHAR lowest_sample_freq_json_property[]       = "lowestSampleFreq";
static const CHAR sampling_freq_json_property[]            = "samplingFreq";
static const CHAR signal_freq_json_property[]              = "signalFreq";
static const CHAR sec_signal_freq_json_property[]              = "secSignalFreq";
static const CHAR curr_diff_json_property[]                = "currDiff";
static const CHAR curr_cluster_1_standby_json_property[]   = "currCluster1Standby";
static const CHAR curr_cluster_2_active_json_property[]    = "currCluster2Active";
static const CHAR curr_active_json_property[]                = "currAverage";
static const CHAR duty_cycle_json_property[]               = "dutyCycle";

AzureIoTResult_t FreeRTOS_vt_currentsense_init(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    CHAR* shared_buffer,
    UINT shared_buffer_size)
{
    AzureIoTResult_t xResult;
    CHAR vt_component_name[VT_COMPONENT_NAME_MAX_LENGTH];
    VT_INT str_manipulation_return;
    VT_INT str_buffer_space_available;

    if (handle == NULL)
    {
        return eAzureIoTErrorFailed;
    }

    memset(vt_component_name, 0, sizeof(vt_component_name));
    str_manipulation_return = snprintf(vt_component_name, sizeof(vt_component_name), "vT");
    if (str_manipulation_return < 0 || (VT_UINT)str_manipulation_return > sizeof(vt_component_name))
    {
        VTLogError("Flattened Database Buffer Overflow! \r\n");
    }
    str_buffer_space_available = sizeof(vt_component_name) - strlen(vt_component_name);
    strncat(vt_component_name, (CHAR*)component_name_ptr, str_buffer_space_available);
    strncpy((CHAR*)handle->component_name_ptr, vt_component_name, sizeof(handle->component_name_ptr) - 1);
    handle->component_name_length = strlen(vt_component_name);
    strncpy((CHAR*)handle->associated_telemetry, (CHAR*)associated_telemetry, sizeof(handle->associated_telemetry) - 1);
    
    handle->property_sent                = 0;
    
    xResult= vt_currentsense_object_initialize(&(handle->cs_object), device_driver, sensor_handle,shared_buffer, shared_buffer_size);

    return xResult;
}

static AzureIoTResult_t reset_reference_currentsense(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle)
{
    
    
    vt_currentsense_object_sensor_calibrate(&(handle->cs_object));
    
    return (eAzureIoTSuccess);
}

static AzureIoTResult_t retrain_reference_currentsense(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle)
{
    
    vt_currentsense_object_sensor_recalibrate(&(handle->cs_object));
    
    return (eAzureIoTSuccess);
}

static AzureIoTResult_t FreeRTOS_vt_currentsense_fingerprint_template_property(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
AzureIoTHubClient_t* xAzureIoTHubClient,
VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db)
{
    AzureIoTJSONWriter_t json_writer;
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void*)ucPropertyPayloadBuffer, 0, sizeof(ucPropertyPayloadBuffer));

    xResult = AzureIoTJSONWriter_Init(&json_writer, ucPropertyPayloadBuffer, sizeof(ucPropertyPayloadBuffer));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&json_writer);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderBeginComponent(
        xAzureIoTHubClient, &json_writer, (const uint8_t*)handle->component_name_ptr, handle->component_name_length);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyName(
        &json_writer, (const uint8_t*)fingerprint_template_property, strlen(fingerprint_template_property));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&json_writer);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)template_type_json_property,
             strlen((const char*)template_type_json_property),
             flattened_db->template_type,
             strlen((CHAR*)flattened_db->template_type));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)average_current_off_json_property,
             strlen((const char*)average_current_off_json_property),
             flattened_db->non_repeating_signature_avg_curr_off,
             strlen((CHAR*)flattened_db->non_repeating_signature_avg_curr_off));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)average_current_on_json_property,
             strlen((const char*)average_current_on_json_property),
             flattened_db->non_repeating_signature_avg_curr_on,
             strlen((CHAR*)flattened_db->non_repeating_signature_avg_curr_on));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)num_repeating_signatures_json_property,
             strlen((const char*)num_repeating_signatures_json_property),
             flattened_db->repeating_signature_num_signatures,
             strlen((CHAR*)flattened_db->repeating_signature_num_signatures));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)offset_curr_json_property,
             strlen((const char*)offset_curr_json_property),
             flattened_db->repeating_signature_offset_curr,
             strlen((CHAR*)flattened_db->repeating_signature_offset_curr));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)lowest_sample_freq_json_property,
             strlen((const char*)lowest_sample_freq_json_property),
             flattened_db->repeating_signature_lowest_sample_freq,
             strlen((CHAR*)flattened_db->repeating_signature_lowest_sample_freq));
    configASSERT(xResult == eAzureIoTSuccess);
    //////////////

   // printf("freq: %s",flattened_db->repeating_signature_sampling_freq );

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)sampling_freq_json_property,
             strlen((const char*)sampling_freq_json_property),
             flattened_db->repeating_signature_sampling_freq,
             strlen((CHAR*)flattened_db->repeating_signature_sampling_freq));
    
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)signal_freq_json_property,
             strlen((const char*)signal_freq_json_property),
             flattened_db->repeating_signature_freq,
             strlen((CHAR*)flattened_db->repeating_signature_freq));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)sec_signal_freq_json_property,
             strlen((const char*)sec_signal_freq_json_property),
             flattened_db->repeating_sec_signature_freq,
             strlen((CHAR*)flattened_db->repeating_sec_signature_freq));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)curr_diff_json_property,
             strlen((const char*)curr_diff_json_property),
             flattened_db->repeating_signature_relative_curr_draw,
             strlen((CHAR*)flattened_db->repeating_signature_relative_curr_draw));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)curr_cluster_1_standby_json_property,
             strlen((const char*)curr_cluster_1_standby_json_property),
             flattened_db->repeating_signature_relative_curr_cluster_1_standby,
             strlen((CHAR*)flattened_db->repeating_signature_relative_curr_cluster_1_standby));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)curr_cluster_2_active_json_property,
             strlen((const char*)curr_cluster_2_active_json_property),
             flattened_db->repeating_signature_relative_curr_cluster_2_active,
             strlen((CHAR*)flattened_db->repeating_signature_relative_curr_cluster_2_active));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)curr_active_json_property,
             strlen((const char*)curr_active_json_property),
             flattened_db->repeating_signature_relative_curr_average,
             strlen((CHAR*)flattened_db->repeating_signature_relative_curr_average));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)duty_cycle_json_property,
             strlen((const char*)duty_cycle_json_property),
             flattened_db->repeating_signature_duty_cycle,
             strlen((CHAR*)flattened_db->repeating_signature_duty_cycle));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendEndObject(&json_writer);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient, &json_writer);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendEndObject(&json_writer);
    configASSERT(xResult == eAzureIoTSuccess);

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed(&json_writer);
    configASSERT(xResult == eAzureIoTSuccess);


    printf(" IoT Hub payload - %s \n", (VT_CHAR*)ucPropertyPayloadBuffer);


    printf(" property update sent to component -  %s \n ", handle->component_name_ptr);

    if (lBytesWritten < 0)
    {
       VTLogError(("Error getting the bytes written for the properties confirmation JSON"));
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported(xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL);
        if (xResult != eAzureIoTSuccess)
        {
            LogError(("There was an error sending the reported properties: 0x%08x", xResult));
        }
    }

    return xResult;
}

static AzureIoTResult_t FreeRTOS_vt_currentsense_fingerprint_template_confidence_metric_property(
    FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle, AzureIoTHubClient_t* xAzureIoTHubClient,UINT template_confidence_metric)
{
    AzureIoTJSONWriter_t xWriter;
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void*)ucPropertyPayloadBuffer, 0, sizeof(ucPropertyPayloadBuffer));

    xResult = AzureIoTJSONWriter_Init(&xWriter, ucPropertyPayloadBuffer, sizeof(ucPropertyPayloadBuffer));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    AzureIoTHubClientProperties_BuilderBeginComponent(xAzureIoTHubClient,
        &xWriter,
        (const uint8_t*)handle->component_name_ptr,
        strlen((const char*)handle->component_name_ptr));

    xResult = AzureIoTJSONWriter_AppendPropertyName(
        &xWriter, (const uint8_t*)template_confidence_metric_property, strlen(template_confidence_metric_property));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendInt32(&xWriter, template_confidence_metric);
    configASSERT(xResult == eAzureIoTSuccess);

    AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient, &xWriter);

    xResult = AzureIoTJSONWriter_AppendEndObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed(&xWriter);

    if (lBytesWritten < 0)
    {
       VTLogError(("Error getting the bytes written for the properties confirmation JSON"));
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported(xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL);

        if (xResult != eAzureIoTSuccess)
        {
           LogError(("There was an error sending the reported properties: 0x%08x", xResult));
        }
    }

    return xResult;
}

static AzureIoTResult_t FreeRTOS_vt_currentsense_fingerprint_template_associated_properties(
    FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle, AzureIoTHubClient_t* xAzureIoTHubClient)
{
    AzureIoTResult_t xResult = eAzureIoTSuccess;

    VT_CURRENTSENSE_DATABASE_FLATTENED flattened_db;
    bool db_required_to_store          = true;
    VT_UINT template_confidence_metric = 0;
    printf("fetch start \n");
    vt_currentsense_object_database_fetch(
        &(handle->cs_object), &flattened_db, &db_required_to_store, &template_confidence_metric);
    printf("fetch complete \n");
    ///here we have a check 
    if (db_required_to_store)
    {       
        printf("db_required_to_store true \n");

        if ((xResult = FreeRTOS_vt_currentsense_fingerprint_template_property(handle, xAzureIoTHubClient, &flattened_db))!=eAzureIoTSuccess)
        {
            VTLogError("Failed to update fingerprint template reported property: error code = 0x%08x\r\n", xResult);
            return xResult;
        }
        handle->cs_object.db_updated = VT_DB_NOT_UPDATED;
        printf("template property done \n");
        if ((xResult = FreeRTOS_vt_currentsense_fingerprint_template_confidence_metric_property(
                 handle, xAzureIoTHubClient, template_confidence_metric))!=eAzureIoTSuccess)
        {
            VTLogError("Failed to update fingerprint template confidence metric reported property: error code = 0x%08x\r\n", xResult);
            return xResult;
        }
    }
    return xResult;
}

AzureIoTResult_t FreeRTOS_vt_currentsense_telemetry_status_property(
    FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle, AzureIoTHubClient_t* xAzureIoTHubClient, bool* deviceStatus,bool toggle_verified_telemetry)
{
    AzureIoTJSONWriter_t xWriter;

    AzureIoTResult_t xResult;
    VT_UINT sensor_status = 0;
    VT_UINT sensor_drift  = 0;
    bool telemetry_status = false;

    if (toggle_verified_telemetry)
    {
        vt_currentsense_object_sensor_fetch_status(&(handle->cs_object), &sensor_status, &sensor_drift);
        telemetry_status = (sensor_status > 0) ? false : true;
    }

    *deviceStatus = *deviceStatus && telemetry_status;
    
    int32_t lBytesWritten;

    memset((void*)ucPropertyPayloadBuffer, 0, sizeof(ucPropertyPayloadBuffer));

    xResult = AzureIoTJSONWriter_Init(&xWriter, ucPropertyPayloadBuffer, sizeof(ucPropertyPayloadBuffer));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderBeginComponent(
        xAzureIoTHubClient, &xWriter, (const uint8_t*)handle->component_name_ptr, handle->component_name_length);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyName(
        &xWriter, (const uint8_t*)telemetry_name_telemetry_status, strlen(telemetry_name_telemetry_status));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBool(&xWriter,telemetry_status);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient, &xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendEndObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed(&xWriter);

    printf(" status_property - %s \n", ucPropertyPayloadBuffer);

    if (lBytesWritten < 0)
    {
       VTLogError(("Error getting the bytes written for the properties confirmation JSON"));
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported(xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL);

        if (xResult != eAzureIoTSuccess)
        {
           LogError(("There was an error sending the reported properties: 0x%08x", xResult));
        }
    }

    return xResult;
}

AzureIoTResult_t FreeRTOS_vt_currentsense_fingerprint_type_property(
    FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle, AzureIoTHubClient_t* xAzureIoTHubClient)

{
    AzureIoTJSONWriter_t xWriter;
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void*)ucPropertyPayloadBuffer, 0, sizeof(ucPropertyPayloadBuffer));

    xResult = AzureIoTJSONWriter_Init(&xWriter, ucPropertyPayloadBuffer, sizeof(ucPropertyPayloadBuffer));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendBeginObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderBeginComponent(
        xAzureIoTHubClient, &xWriter, (const uint8_t*)handle->component_name_ptr, handle->component_name_length);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendPropertyName(
        &xWriter, (const uint8_t*)fingerprint_type_property, strlen(fingerprint_type_property));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendString(&xWriter, (const uint8_t*)fingerprint_type_value, strlen(fingerprint_type_value));
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient, &xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONWriter_AppendEndObject(&xWriter);
    configASSERT(xResult == eAzureIoTSuccess);

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed(&xWriter);

    printf(" fingerprint_type - %s \n", ucPropertyPayloadBuffer);

    if (lBytesWritten < 0)
    {
       VTLogError(("Error getting the bytes written for the properties confirmation JSON"));
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported(xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL);

        if (xResult != eAzureIoTSuccess)
        {
           LogError(("There was an error sending the reported properties: 0x%08x", xResult));
        }
    }

    return xResult;
}

AzureIoTResult_t FreeRTOS_vt_currentsense_reported_properties(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    bool* device_status,
    bool toggle_verified_telemetry)
{
    AzureIoTResult_t xResult;

    if ((xResult = FreeRTOS_vt_currentsense_telemetry_status_property(
             handle, xAzureIoTHubClient, device_status, toggle_verified_telemetry))!= eAzureIoTSuccess)
    {
       VTLogError("Failed FreeRTOS_vt_currentsense_telemetry_status_property for component %.*s: error code = "
                   "0x%08x\r\n\n",
            (INT)handle->component_name_length,
            (CHAR*)handle->component_name_ptr,
            xResult);
    }

    else if ((xResult = FreeRTOS_vt_currentsense_fingerprint_template_associated_properties(handle, xAzureIoTHubClient))!= eAzureIoTSuccess)
    {
       VTLogError("Failed FreeRTOS_vt_currentsense_fingerprint_template_associated_properties for component %.*s: error code = "
                   "0x%08x\r\n\n",
            (INT)handle->component_name_length,
            (CHAR*)handle->component_name_ptr,
            xResult);
    }

    else if (handle->property_sent == 0)
    {
        if ((xResult = FreeRTOS_vt_currentsense_fingerprint_type_property(handle, xAzureIoTHubClient))!= eAzureIoTSuccess)
        {
           VTLogError("Failed FreeRTOS_vt_currentsense_fingerprint_type_property for component %.*s: error code = 0x%08x\r\n",
                (INT)handle->component_name_length,
                (CHAR*)handle->component_name_ptr,
                xResult);
        }

        handle->property_sent = 1;
    }
    return xResult;
}


static AzureIoTResult_t sync_fingerprint_template(AzureIoTJSONReader_t* xReader, 
    FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle)
{

    VT_CURRENTSENSE_DATABASE_FLATTENED flattened_db;

    AzureIoTResult_t xResult;
    uint8_t pucBufferLocal[LOCAL_BUFFER_SIZE];
    uint32_t pusBytesCopied;
    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" template type - %s \n", pucBufferLocal);
    memset(flattened_db.template_type, 0, sizeof(flattened_db.template_type));
    strncpy((VT_CHAR*)flattened_db.template_type, (const char*)pucBufferLocal, sizeof(flattened_db.template_type) - 1);


    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" non_repeating_signature_avg_curr_off - %s \n", pucBufferLocal);
    memset(flattened_db.non_repeating_signature_avg_curr_off, 0, sizeof(flattened_db.non_repeating_signature_avg_curr_off));
    strncpy(
        (VT_CHAR*)flattened_db.non_repeating_signature_avg_curr_off, (const char*)pucBufferLocal, sizeof(flattened_db.non_repeating_signature_avg_curr_off) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" non_repeating_signature_avg_curr_on - %s \n", pucBufferLocal);
    memset(flattened_db.non_repeating_signature_avg_curr_on, 0, sizeof(flattened_db.non_repeating_signature_avg_curr_on));
    strncpy((VT_CHAR*)flattened_db.non_repeating_signature_avg_curr_on, (const char*)pucBufferLocal, sizeof(flattened_db.non_repeating_signature_avg_curr_on) - 1);


    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_num_signatures - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_num_signatures, 0, sizeof(flattened_db.repeating_signature_num_signatures));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_num_signatures, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_num_signatures) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_offset_curr - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_offset_curr, 0, sizeof(flattened_db.repeating_signature_offset_curr));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_offset_curr, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_offset_curr) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_lowest_sample_freq - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_lowest_sample_freq, 0, sizeof(flattened_db.repeating_signature_lowest_sample_freq));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_lowest_sample_freq, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_lowest_sample_freq) - 1);
    
    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_sampling_freq - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_sampling_freq, 0, sizeof(flattened_db.repeating_signature_sampling_freq));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_sampling_freq, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_sampling_freq) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_freq - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_freq, 0, sizeof(flattened_db.repeating_signature_freq));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_freq, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_freq) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_sec_signature_freq - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_sec_signature_freq, 0, sizeof(flattened_db.repeating_sec_signature_freq));
    strncpy((VT_CHAR*)flattened_db.repeating_sec_signature_freq, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_sec_signature_freq) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_relative_currdiff - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_relative_curr_draw, 0, sizeof(flattened_db.repeating_signature_relative_curr_draw));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_relative_curr_draw, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_relative_curr_draw) - 1);
    

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_relative_curr_cluster_1_standby - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_relative_curr_cluster_1_standby, 0, sizeof(flattened_db.repeating_signature_relative_curr_cluster_1_standby));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_relative_curr_cluster_1_standby, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_relative_curr_cluster_1_standby) - 1);
    

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_relative_curr_cluster_2_active - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_relative_curr_cluster_2_active, 0, sizeof(flattened_db.repeating_signature_relative_curr_cluster_2_active));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_relative_curr_cluster_2_active, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_relative_curr_cluster_2_active) - 1);
    
    
    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_relative_curr_average - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_relative_curr_average, 0, sizeof(flattened_db.repeating_signature_relative_curr_average));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_relative_curr_average, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_relative_curr_average) - 1);
    

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess);
    memset(pucBufferLocal, 0, LOCAL_BUFFER_SIZE);
    /* Get desired temperature */
    xResult = AzureIoTJSONReader_GetTokenString(xReader, pucBufferLocal, LOCAL_BUFFER_SIZE, &pusBytesCopied);
    configASSERT(xResult == eAzureIoTSuccess);

    printf(" repeating_signature_duty_cycle - %s \n", pucBufferLocal);
    memset(flattened_db.repeating_signature_duty_cycle, 0, sizeof(flattened_db.repeating_signature_duty_cycle));
    strncpy((VT_CHAR*)flattened_db.repeating_signature_duty_cycle, (const char*)pucBufferLocal, sizeof(flattened_db.repeating_signature_duty_cycle) - 1);

    xResult = AzureIoTJSONReader_NextToken(xReader);
    configASSERT(xResult == eAzureIoTSuccess); 

    vt_currentsense_object_database_sync(&(handle->cs_object), &flattened_db);

    return xResult;
}

AzureIoTResult_t FreeRTOS_vt_currentsense_process_reported_property_sync(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,

    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr)
{

    if (handle == NULL)
    {
        return (eAzureIoTErrorFailed);
    }

    if (handle->component_name_length != component_name_length ||
        strncmp((CHAR*)handle->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        return (eAzureIoTErrorFailed);
    }

    // Property 1: FingerprintDB
    if (AzureIoTJSONReader_TokenIsTextEqual(
            json_reader_ptr, (const uint8_t*)"fingerprintTemplate", strlen("fingerprintTemplate")))
    {

        sync_fingerprint_template(json_reader_ptr, handle);

        VTLogInfo(
            "Successfully synced fingerprint template for component %.*s \r\n\n", component_name_length, component_name_ptr);
        return (eAzureIoTSuccess);
    }

    return (eAzureIoTSuccess);
}


AzureIoTResult_t FreeRTOS_vt_currentsense_process_command(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    UINT* status_code)
{
    AzureIoTResult_t xResult;
    if (handle == NULL)
    {
        return (eAzureIoTErrorInvalidArgument);
    }

    if (handle->component_name_length != component_name_length ||
        strncmp((CHAR*)handle->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        return (eAzureIoTErrorInvalidArgument);
    }
    // printf("reached if else \n");
    // Command 1 : Reset Fingerprint
    if (((pnp_command_name_length == (sizeof(command_reset_fingerprint) - 1)) &&
            (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_reset_fingerprint, pnp_command_name_length)))) == 1)
    {   
        xResult = (reset_reference_currentsense(handle) != eAzureIoTSuccess) ? eAzureIoTErrorFailed : eAzureIoTSuccess;

    }

    // Command 2 : Retrain Fingerprint
    else if (((pnp_command_name_length == (sizeof(command_retrain_fingerprint) - 1)) &&
                 (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_retrain_fingerprint, pnp_command_name_length)))) == 1)
    {
        xResult = (retrain_reference_currentsense(handle) != eAzureIoTSuccess) ? eAzureIoTErrorFailed : eAzureIoTSuccess;
        //printf("Retrain_refernce_fallcurve \n");

    }

    else
    {
        VTLogError("PnP command=%.*s is not supported on vTInfo  component\r\n", pnp_command_name_length, pnp_command_name_ptr);
        xResult = eAzureIoTErrorItemNotFound;
    }

    //*status_code = dm_status;

    return (xResult);
}

AzureIoTResult_t FreeRTOS_vt_currentsense_signature_read(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry,UINT mode)
{
    if (strlen((CHAR*)handle->associated_telemetry) != associated_telemetry_length ||
        strncmp((CHAR*)handle->associated_telemetry, (CHAR*)associated_telemetry, associated_telemetry_length) != 0)
    {
        return (eAzureIoTErrorFailed);
    }
    if (!toggle_verified_telemetry)
    {
        return (eAzureIoTErrorFailed);
    }
    
    vt_currentsense_object_signature_read(&(handle->cs_object),mode);
    
    return (eAzureIoTSuccess);
}

AzureIoTResult_t FreeRTOS_vt_currentsense_signature_process(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry)
{
    if (strlen((CHAR*)handle->associated_telemetry) != associated_telemetry_length ||
        strncmp((CHAR*)handle->associated_telemetry, (CHAR*)associated_telemetry, associated_telemetry_length) != 0)
    {
        return (eAzureIoTErrorFailed);
    }
    if (!toggle_verified_telemetry)
    {
        return (eAzureIoTErrorFailed);
    }
    vt_currentsense_object_signature_process(&(handle->cs_object));
    return (eAzureIoTSuccess);
}

bool FreeRTOS_vt_currentsense_fetch_telemetry_status(FreeRTOS_VT_CURRENTSENSE_COMPONENT* handle, bool toggle_verified_telemetry)
{
    VT_UINT sensor_status = 0;
    VT_UINT sensor_drift  = 0;
    bool telemetry_status = false;

    if (toggle_verified_telemetry)
    {
        vt_currentsense_object_sensor_fetch_status(&(handle->cs_object), &sensor_status, &sensor_drift);
        telemetry_status = (sensor_status > 0) ? false : true;
    }
    return telemetry_status;
}