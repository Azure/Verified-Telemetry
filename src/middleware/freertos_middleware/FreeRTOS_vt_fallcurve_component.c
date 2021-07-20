
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <string.h>
#include <stdio.h>
#include <time.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
 
/* Azure Provisioning/IoT Hub library includes */
#include "azure_iot_hub_client.h"
#include "azure_iot_provisioning_client.h"
#include "azure_iot_hub_client_properties.h"

/* Azure JSON includes */
#include "azure_iot_json_reader.h"
#include "azure_iot_json_writer.h"

/* Exponential backoff retry include. */
//#include "backoff_algorithm.h"

/* Transport interface implementation include header for TLS. */
//#include "transport_tls_socket.h"

/* Crypto helper header. */
//#include "crypto.h"

/* Demo Specific configs. */
//#include "demo_config.h"
//removed a static and commented these headers
#include "FreeRTOS_verified_telemetry.h"
#include "FreeRTOS_vt_fallcurve_component.h"
#include "vt_defs.h"


#include "vt_debug.h"
#include "vt_fc_api.h"
#define SAMPLE_COMMAND_SUCCESS_STATUS (200)
#define SAMPLE_COMMAND_ERROR_STATUS   (500)

#define PROPERTY_NAME_MAX_LENGTH 50

static const CHAR command_reset_fingerprint[]   = "setResetFingerprintTemplate";
static const CHAR command_retrain_fingerprint[] = "retrainFingerprintTemplate";
static uint8_t ucPropertyPayloadBuffer[ 256 ];


static const CHAR telemetry_name_telemetry_status[]     = "telemetryStatus";
static const CHAR fingerprint_type_property[]           = "fingerprintType";
static const CHAR template_confidence_metric_property[] = "fingerprintTemplateConfidenceMetric";
static const CHAR fingerprint_type_value[]              = "FallCurve";
static const CHAR fingerprint_template_property[]       = "fingerprintTemplate";
static const CHAR num_signatures_json_property[]        = "numSignatures";
static const CHAR sampling_interval_us_property[]       = "samplingIntervalus";
static const CHAR falltime_property[]                   = "falltime";
static const CHAR pearson_coeff_property[]              = "pearsonCoeff";

AzureIoTResult_t FreeRTOS_vt_fallcurve_init(FreeRTOS_VT_FALLCURVE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update)
{
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
    handle->telemetry_status             = false;
    handle->property_sent                = 0;
    handle->template_confidence_metric   = 0;
    handle->telemetry_status_auto_update = telemetry_status_auto_update;

    vt_fallcurve_object_initialize(&(handle->fc_object), device_driver, sensor_handle);

    return eAzureIoTSuccess;
}

static AzureIoTResult_t reset_refernce_fallcurve(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTJSONReader_t* json_reader_ptr)
{
    uint8_t confidence_metric;
    UINT status                        = vt_fallcurve_object_sensor_calibrate(&(handle->fc_object), &confidence_metric);
    handle->template_confidence_metric = confidence_metric;
    return (status);
}

static AzureIoTResult_t retrain_refernce_fallcurve(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTJSONReader_t* json_reader_ptr)
{
    uint8_t confidence_metric;
    UINT status                        = vt_fallcurve_object_sensor_recalibrate(&(handle->fc_object), &confidence_metric);
    handle->template_confidence_metric = confidence_metric;
    return (status);
}


static AzureIoTResult_t hub_store_all_db(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTHubClient_t* xAzureIoTHubClient)
{
    UINT response_status = 0;
    AzureIoTJSONWriter_t json_writer;
    AzureIoTResult_t xResult;
    VT_FALLCURVE_DATABASE_FLATTENED flattened_db;
    int32_t lBytesWritten;

    vt_fallcurve_object_database_fetch(&(handle->fc_object), &flattened_db);
    //snprintf(flattened_db.num_signatures, sizeof(flattened_db.num_signatures), "%03d", 2);
    //snprintf(flattened_db.falltime, sizeof(flattened_db.falltime), "%lu", 10);
    //snprintf(flattened_db.pearson_coeff, sizeof(flattened_db.pearson_coeff), "%lu", 95);
    //snprintf(flattened_db.sampling_interval_us, sizeof(flattened_db.sampling_interval_us), "%lu", 4);


    //strncpy((VT_CHAR*)flattened_db.num_signatures, "2", strlen("2"));
    //strncpy((VT_CHAR*)flattened_db.falltime, "10", strlen("10"));
    //strncpy((VT_CHAR*)flattened_db.pearson_coeff, "95", strlen("95"));
    //strncpy((VT_CHAR*)flattened_db.sampling_interval_us, "4", strlen("4"));
 
    memset((void *)ucPropertyPayloadBuffer,0,sizeof(ucPropertyPayloadBuffer));


    xResult = AzureIoTJSONWriter_Init( &json_writer, ucPropertyPayloadBuffer, sizeof( ucPropertyPayloadBuffer ));
    configASSERT( xResult == eAzureIoTSuccess );


    xResult = AzureIoTJSONWriter_AppendBeginObject( &json_writer );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTHubClientProperties_BuilderBeginComponent(xAzureIoTHubClient,&json_writer,
                                                    (const uint8_t *)handle->component_name_ptr,
                                                    handle->component_name_length);
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendPropertyName( &json_writer, (const uint8_t *)fingerprint_template_property,
                                                     strlen( fingerprint_template_property ) );
    configASSERT( xResult == eAzureIoTSuccess );
                                                 
    xResult = AzureIoTJSONWriter_AppendBeginObject( &json_writer );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)num_signatures_json_property,
             strlen((const char*)num_signatures_json_property),
             flattened_db.num_signatures,
             strlen((CHAR*)flattened_db.num_signatures));
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)sampling_interval_us_property,
             strlen((const char*)sampling_interval_us_property),
             flattened_db.sampling_interval_us,
             strlen((CHAR*)flattened_db.sampling_interval_us));

    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)falltime_property,
             strlen((const char*)falltime_property),
             flattened_db.falltime,
             strlen((CHAR*)flattened_db.falltime));

    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
             (UCHAR*)pearson_coeff_property,
             strlen((const char*)pearson_coeff_property),
             flattened_db.pearson_coeff,
             strlen((CHAR*)flattened_db.pearson_coeff));

    configASSERT( xResult == eAzureIoTSuccess );


    xResult = AzureIoTJSONWriter_AppendEndObject( &json_writer );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient,&json_writer);
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendEndObject( &json_writer );
    configASSERT( xResult == eAzureIoTSuccess );

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed( &json_writer );
    configASSERT( xResult == eAzureIoTSuccess );

    printf(" property update sent to component -  %s \n ",handle->component_name_ptr);

    if( lBytesWritten < 0 )
    {
        LogError( ( "Error getting the bytes written for the properties confirmation JSON" ) );
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported( xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL );
        printf(" xResult -%d \n",xResult);
        if( xResult != eAzureIoTSuccess )
        {
            LogError( ( "There was an error sending the reported properties: 0x%08x", xResult ) );
        }
    }

    return xResult;
    
}


AzureIoTResult_t FreeRTOS_vt_fallcurve_fingerprint_template_confidence_metric_property(
    FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTHubClient_t* xAzureIoTHubClient)
{
    UINT response_status = 0;
    AzureIoTJSONWriter_t xWriter;
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void *)ucPropertyPayloadBuffer,0,sizeof(ucPropertyPayloadBuffer));


    xResult = AzureIoTJSONWriter_Init( &xWriter, ucPropertyPayloadBuffer, sizeof( ucPropertyPayloadBuffer ) );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendBeginObject( &xWriter );
    configASSERT( xResult == eAzureIoTSuccess );

    AzureIoTHubClientProperties_BuilderBeginComponent(xAzureIoTHubClient,&xWriter,
                                                    (const uint8_t *)handle->component_name_ptr,
                                                    strlen(handle->component_name_ptr));

    xResult = AzureIoTJSONWriter_AppendPropertyName( &xWriter, (const uint8_t *)template_confidence_metric_property,
                                                     strlen(template_confidence_metric_property ) );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendInt32( &xWriter, handle->template_confidence_metric);
    configASSERT( xResult == eAzureIoTSuccess );

    AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient,&xWriter);

    xResult = AzureIoTJSONWriter_AppendEndObject( &xWriter );
    configASSERT( xResult == eAzureIoTSuccess );

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed( &xWriter );

    if( lBytesWritten < 0 )
    {
        LogError( ( "Error getting the bytes written for the properties confirmation JSON" ) );
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported( xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL );

        if( xResult != eAzureIoTSuccess )
        {
            LogError( ( "There was an error sending the reported properties: 0x%08x", xResult ) );
        }
    }

    return xResult;

}


AzureIoTResult_t FreeRTOS_vt_fallcurve_telemetry_status_property(
    FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTHubClient_t * xAzureIoTHubClient, bool* deviceStatus)
{
    UINT status          = 0;
    UINT response_status = 0;
    AzureIoTJSONWriter_t xWriter;
    *deviceStatus = *deviceStatus && handle->telemetry_status;
    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void *)ucPropertyPayloadBuffer,0,sizeof(ucPropertyPayloadBuffer));


    xResult = AzureIoTJSONWriter_Init( &xWriter, ucPropertyPayloadBuffer, sizeof( ucPropertyPayloadBuffer ) );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendBeginObject( &xWriter );
    configASSERT( xResult == eAzureIoTSuccess );

    AzureIoTHubClientProperties_BuilderBeginComponent(xAzureIoTHubClient,&xWriter,
                                                    (const uint8_t *)handle->component_name_ptr,
                                                    handle->component_name_length);

    xResult = AzureIoTJSONWriter_AppendPropertyName( &xWriter, (const uint8_t *)telemetry_name_telemetry_status,
                                                     strlen( telemetry_name_telemetry_status ) );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendBool( &xWriter, handle->telemetry_status);
    configASSERT( xResult == eAzureIoTSuccess );

    AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient,&xWriter);

    xResult = AzureIoTJSONWriter_AppendEndObject( &xWriter );
    configASSERT( xResult == eAzureIoTSuccess );

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed( &xWriter );
    
    printf(" status_property - %s \n",ucPropertyPayloadBuffer);

    if( lBytesWritten < 0 )
    {
        LogError( ( "Error getting the bytes written for the properties confirmation JSON" ) );
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported( xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL );

        if( xResult != eAzureIoTSuccess )
        {
            LogError( ( "There was an error sending the reported properties: 0x%08x", xResult ) );
        }
    }

    return xResult;

}


AzureIoTResult_t FreeRTOS_vt_fallcurve_fingerprint_type_property(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, AzureIoTHubClient_t * xAzureIoTHubClient)

{
    UINT status          = 0;
    UINT response_status = 0;
    AzureIoTJSONWriter_t xWriter;


    AzureIoTResult_t xResult;
    int32_t lBytesWritten;

    memset((void *)ucPropertyPayloadBuffer,0,sizeof(ucPropertyPayloadBuffer));


    xResult = AzureIoTJSONWriter_Init( &xWriter, ucPropertyPayloadBuffer, sizeof( ucPropertyPayloadBuffer ) );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendBeginObject( &xWriter );
    configASSERT( xResult == eAzureIoTSuccess );

    AzureIoTHubClientProperties_BuilderBeginComponent(xAzureIoTHubClient,&xWriter,
                                                    (const uint8_t *)handle->component_name_ptr,
                                                    handle->component_name_length);

    xResult = AzureIoTJSONWriter_AppendPropertyName( &xWriter, (const uint8_t *)fingerprint_type_property,
                                                     strlen( fingerprint_type_property ) );
    configASSERT( xResult == eAzureIoTSuccess );

    xResult = AzureIoTJSONWriter_AppendString( &xWriter, fingerprint_type_value,strlen(fingerprint_type_value));
    configASSERT( xResult == eAzureIoTSuccess );

    AzureIoTHubClientProperties_BuilderEndComponent(xAzureIoTHubClient,&xWriter);

    xResult = AzureIoTJSONWriter_AppendEndObject( &xWriter );
    configASSERT( xResult == eAzureIoTSuccess );

    lBytesWritten = AzureIoTJSONWriter_GetBytesUsed( &xWriter );
    
    printf(" fingerprint_type - %s \n",ucPropertyPayloadBuffer);


    if( lBytesWritten < 0 )
    {
        LogError( ( "Error getting the bytes written for the properties confirmation JSON" ) );
    }
    else
    {
        xResult = AzureIoTHubClient_SendPropertiesReported( xAzureIoTHubClient, ucPropertyPayloadBuffer, lBytesWritten, NULL );

        if( xResult != eAzureIoTSuccess )
        {
            LogError( ( "There was an error sending the reported properties: 0x%08x", xResult ) );
        }
    }

    return xResult;
}

static UINT sync_fingerprint_template(AzureIoTJSONReader_t* xReader, FreeRTOS_VT_FALLCURVE_COMPONENT* handle)
{

    VT_FALLCURVE_DATABASE_FLATTENED flattened_db;

    AzureIoTResult_t xResult;
    uint8_t pucBufferLocal[64];
    uint32_t pusBytesCopied;
                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                /* Get desired temperature */
                xResult = AzureIoTJSONReader_GetTokenString( xReader, pucBufferLocal, 64,&pusBytesCopied);
                configASSERT( xResult == eAzureIoTSuccess );

                printf(" num_signatures - %s \n",pucBufferLocal);
                memset(flattened_db.num_signatures, 0, sizeof(flattened_db.num_signatures));
                strncpy((VT_CHAR*)flattened_db.num_signatures, pucBufferLocal, sizeof(flattened_db.num_signatures) - 1);



                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );
                memset(pucBufferLocal, 0, 64);
                /* Get desired temperature */
                xResult = AzureIoTJSONReader_GetTokenString( xReader, pucBufferLocal, 64,&pusBytesCopied);
                configASSERT( xResult == eAzureIoTSuccess );

                printf(" sampling_interval_us - %s \n",pucBufferLocal);
                memset(flattened_db.sampling_interval_us, 0, sizeof(flattened_db.sampling_interval_us));
                strncpy((VT_CHAR*)flattened_db.sampling_interval_us, pucBufferLocal, sizeof(flattened_db.sampling_interval_us) - 1);



                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );
                memset(pucBufferLocal, 0, 64);
                /* Get desired temperature */
                xResult = AzureIoTJSONReader_GetTokenString( xReader, pucBufferLocal, 64,&pusBytesCopied);
                configASSERT( xResult == eAzureIoTSuccess );

                printf(" falltime - %s \n",pucBufferLocal);
                memset(flattened_db.falltime, 0, sizeof(flattened_db.falltime));
                strncpy((VT_CHAR*)flattened_db.falltime, pucBufferLocal, sizeof(flattened_db.falltime) - 1);



                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );
                memset(pucBufferLocal, 0, 64);
                /* Get desired temperature */
                xResult = AzureIoTJSONReader_GetTokenString( xReader, pucBufferLocal, 64,&pusBytesCopied);
                configASSERT( xResult == eAzureIoTSuccess );

                printf(" pearson_coeff - %s \n",pucBufferLocal);
                memset(flattened_db.pearson_coeff, 0, sizeof(flattened_db.pearson_coeff));
                strncpy((VT_CHAR*)flattened_db.pearson_coeff, pucBufferLocal, sizeof(flattened_db.pearson_coeff) - 1);


                xResult = AzureIoTJSONReader_NextToken( xReader );
                configASSERT( xResult == eAzureIoTSuccess );

                vt_fallcurve_object_database_sync(&(handle->fc_object), &flattened_db);


    return xResult;

}


AzureIoTResult_t FreeRTOS_vt_fallcurve_process_reported_property_sync(FreeRTOS_VT_FALLCURVE_COMPONENT* handle,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr,
    UINT version)
{
    CHAR property_name[PROPERTY_NAME_MAX_LENGTH];
    UINT property_name_length = 0;

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
    if( AzureIoTJSONReader_TokenIsTextEqual( json_reader_ptr,
                                                        (const uint8_t *)"fingerprintTemplate",
                                                        strlen( "fingerprintTemplate" ) ) )
    {


            sync_fingerprint_template(json_reader_ptr, handle);

            VTLogInfo(
                "Successfully synced fingerprint template for component %.*s \r\n\n", component_name_length, component_name_ptr);
            return (eAzureIoTSuccess);
        
    }

    return (eAzureIoTSuccess);
}


AzureIoTResult_t FreeRTOS_vt_fallcurve_compute_sensor_status_global(FreeRTOS_VT_FALLCURVE_COMPONENT* handle, bool toggle_verified_telemetry)
{
    if (!toggle_verified_telemetry)
    {
        handle->telemetry_status = false;
        return eAzureIoTSuccess;
    }
    if (handle->telemetry_status_auto_update == false)
    {
        return eAzureIoTSuccess;
    }

    /* Compute fallcurve classification */
    VT_UINT sensor_status = 0;
    VT_UINT sensor_drift  = 100;
    printf("calling vt_fallcurve_object_sensor_status \n");
    vt_fallcurve_object_sensor_status(&(handle->fc_object), &sensor_status, &sensor_drift);
    handle->telemetry_status = (sensor_status > 0) ? false : true;
    return eAzureIoTSuccess;
}

AzureIoTResult_t FreeRTOS_vt_fallcurve_process_command(FreeRTOS_VT_FALLCURVE_COMPONENT* handle,
    AzureIoTHubClient_t * xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    AzureIoTJSONReader_t* json_reader_ptr,
    AzureIoTJSONWriter_t* json_response_ptr,
    UINT* status_code)
{
    UINT dm_status;

    if (handle == NULL)
    {
        return (eAzureIoTErrorInvalidArgument);
    }

    if (handle->component_name_length != component_name_length ||
        strncmp((CHAR*)handle->component_name_ptr, (CHAR*)component_name_ptr, component_name_length) != 0)
    {
        return (eAzureIoTErrorInvalidArgument);
    }
        //printf("reached if else \n");
    // Command 1 : Reset Fingerprint
    if (((pnp_command_name_length == (sizeof(command_reset_fingerprint) - 1)) &&
            (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_reset_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (reset_refernce_fallcurve(handle, json_reader_ptr) != eAzureIoTSuccess) ? SAMPLE_COMMAND_ERROR_STATUS
                                                                                                : SAMPLE_COMMAND_SUCCESS_STATUS;
        printf("reset_refernce_fallcurve \n");
        if (hub_store_all_db(handle, xAzureIoTHubClient))
        {
            VTLogError("Failed to update db in cloud\r\n");
        }
        if (FreeRTOS_vt_fallcurve_fingerprint_template_confidence_metric_property(handle, xAzureIoTHubClient))
        {
            VTLogError("Failed to update Fingerprint Template Confidence Metric\r\n");
        }
    }

    // Command 2 : Retrain Fingerprint
    else if (((pnp_command_name_length == (sizeof(command_retrain_fingerprint) - 1)) &&
                 (!(strncmp((CHAR*)pnp_command_name_ptr, (CHAR*)command_retrain_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (retrain_refernce_fallcurve(handle, json_reader_ptr) != eAzureIoTSuccess) ? SAMPLE_COMMAND_ERROR_STATUS
                                                                                                  : SAMPLE_COMMAND_SUCCESS_STATUS;
        printf("Retrain_refernce_fallcurve \n");
        if (hub_store_all_db(handle, xAzureIoTHubClient))
        {
            VTLogError("Failed to update db in cloud\r\n");
        }
        if (FreeRTOS_vt_fallcurve_fingerprint_template_confidence_metric_property(handle, xAzureIoTHubClient))
        {
            VTLogError("Failed to update Fingerprint Template Confidence Metric\r\n");
        }
    }

    else
    {
        VTLogError("PnP command=%.*s is not supported on vTInfo  component\r\n", pnp_command_name_length, pnp_command_name_ptr);
        dm_status = 404;
    }

    *status_code = dm_status;

    return (eAzureIoTSuccess);
}