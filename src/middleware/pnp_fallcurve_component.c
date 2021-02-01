/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "pnp_fallcurve_component.h"

//Update exposed function, quick fix for now
#include "vt_database.h"

#define DOUBLE_DECIMAL_PLACE_DIGITS                                     (2)
#define SAMPLE_COMMAND_SUCCESS_STATUS                                   (200)
#define SAMPLE_COMMAND_ERROR_STATUS                                     (500)

/* Telemetry key */
// static const CHAR telemetry_name_FallCurve[] = "fallCurveString";
static const CHAR telemetry_name_Telemetry_Status[] = "telemetryStatus";
// static const CHAR telemetry_name_Status[] = "sensorDetected";

/* Names of properties for desired/reporting */
// static const CHAR reported_raw_telemetry[] = "associatedTelemetry";
/* Init Telemetry Values */
static const CHAR init_fingerprint[] = "--";

/* Pnp command supported */
static const CHAR command_reset_fingerprint[] = "resetFingerprintTemplate";

/* Default Telemetry Values */
static const CHAR fail_fingerprint[] = "--";
static const CHAR collection_off_fingerprint[] = "-";

static const CHAR classification_status_default[] = "uninitiated";
static const CHAR classification_status_fail[] = "failure";
static const CHAR classification_status_no_eval[] = "Evaluation OFF";
static const CHAR classification_status_dbempty[] = "DB Empty";
static const CHAR classification_status_unidentified[] = "Unidentified";
static const CHAR fingerprintType_property[] = "fingerprintType";
static const CHAR fingerprintType_value[] = "FallCurve";
static const CHAR fingerprintTemplate_property[] = "fingerprintTemplate";
static const CHAR sensorID_json_property[] = "sensorID";
static const CHAR sensorName_json_property[] = "sensorName";
static const CHAR fallTime_json_property[] = "fallTime";
static const CHAR pearsonCoeff_json_property[] = "pearsonCoeff";

// static UCHAR scratch_buffer[2500]; // Dependent on fallcurve length

char fallcurvestring[1500];
char sensorConnected[50];

bool addFallcurveToDB(PNP_FALLCURVE_COMPONENT **fallcurve_components, 
                        PNP_FALLCURVE_COMPONENT *handle, 
                        UINT numberVerifiedTelemetries)
{
    bool fallcurveAdded = false;
    for(UINT iter = 0; iter < numberVerifiedTelemetries; iter++)
    {
        if(fallcurve_components[iter] == NULL)
        {
            fallcurve_components[iter] = handle;
            fallcurveAdded = true;
            break;
        }
    }
    return (!fallcurveAdded);
}

bool addSensorNameToDB(CHAR **connected_sensors, UCHAR *associatedSensor, UINT numberUniqueSensors)
{
    bool status;
    bool sensorPresent = false; 
    bool sensorAdded = false; 
    for(UINT iter = 0; iter < numberUniqueSensors; iter++)
    {
        if(connected_sensors[iter] != NULL)
        {
            if (!(strncmp((CHAR *)connected_sensors[iter], (CHAR *)associatedSensor, strlen(connected_sensors[iter]))))
            {
                sensorPresent = true;
                break;
            }
        }
        else
        {
            break;
        }
        
    }
    if(!sensorPresent)
    {
        for(UINT iter = 0; iter < numberUniqueSensors; iter++)
        {
            if(connected_sensors[iter] == NULL)
            {
                connected_sensors[iter] = (CHAR *)associatedSensor;
                sensorAdded = true;
                break;
            }
        }
    }  
    status = sensorPresent || sensorAdded;
    return (!status); 
}

static UINT identify_ground_truth_label(UCHAR *ground_truth_label_str, INT ground_truth_label_str_len,
                                    PNP_FALLCURVE_COMPONENT *fallcurve_component, INT *ground_truth_label)
{
    UINT iter = 0;
    for(iter = 0; iter < fallcurve_component -> connected_sensors_num; iter++ )
    {
        if ((ground_truth_label_str_len == (int)(strlen((CHAR *)fallcurve_component -> connected_sensors[iter]))) && 
        (!(strncmp((CHAR *)ground_truth_label_str, (CHAR *)fallcurve_component -> connected_sensors[iter], ground_truth_label_str_len))))
        {
            *ground_truth_label = iter + 1;
            return(NX_AZURE_IOT_SUCCESS);
        }
    }
    return(NX_AZURE_IOT_NOT_FOUND);
}

UINT pnp_fallcurve_init(PNP_FALLCURVE_COMPONENT *handle,
                                UCHAR *component_name_ptr,
                                GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, 
                                ADC_CONTROLLER_TYPEDEF * ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel, 
                                TIMER_HANDLE_TYPEDEF* Timer,
                                PNP_FALLCURVE_COMPONENT **fallcurve_components,
                                CHAR **connected_sensors, UCHAR *associatedTelemetry,
                                UINT numberVerifiedTelemetries)
{
    if (handle == NX_NULL)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    handle -> component_name_ptr = component_name_ptr;
    handle -> component_name_length = strlen((const char *) component_name_ptr);
    handle -> fallcurveString =  (CHAR *) init_fingerprint;
    handle -> sensorConnected = (CHAR *) classification_status_default;
    handle -> vTInfo_property_sent = 0;
    handle -> associatedSensor = (CHAR *)associatedTelemetry;
    handle -> connected_sensors = connected_sensors;
    handle -> connected_sensors_num = numberVerifiedTelemetries;
    // vt_database_initialize(&(handle->fingerprintdb), 0x080E0000);

    
    if (addFallcurveToDB(fallcurve_components, handle, numberVerifiedTelemetries)) 
    {
        printf("Fingerprint Registration Error, number of fingerprints registered exceeding number defined by user\r\n");
    }

    if (addSensorNameToDB(connected_sensors, associatedTelemetry, numberVerifiedTelemetries))
    {
        printf("Verified Telemetry Registration Error, number of telemetries registered exceeding number defined by user\r\n");
    }

    /* VT Library calls */
    if(vt_sensor_initialize(&handle -> portInfo, (CHAR *)component_name_ptr, GPIOx, GPIO_Pin, ADC_Controller, ADC_Channel, Timer))
    {
        printf("Port Registration Error, kindly check if all the pins are initialized\r\n");
        return(NX_NOT_SUCCESSFUL);
    }

    return(NX_AZURE_IOT_SUCCESS);
}

UINT get_fallcurve(PNP_FALLCURVE_COMPONENT *handle, VT_DATABASE *fingerprintdb, bool toggleVerifiedTelemetry)
{
    if (handle == NX_NULL)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    /* VT Library calls */
    uint32_t fallcurvearray[100];
    int sensorid;
    if (toggleVerifiedTelemetry)
    {
        if (vt_sensor_read_fingerprint(&handle->portInfo, fallcurvearray, fallcurvestring))
        {
            printf("Failed to measure Fall Curve\r\n");
            handle -> fallcurveString = (CHAR *)fail_fingerprint;
            return(NX_NOT_SUCCESSFUL);
        }
        else
        {   
            handle -> fallcurveString = fallcurvestring;
        }

        if (vt_sensor_read_status(&handle->portInfo, fingerprintdb, fallcurvearray, &sensorid))
        {
            printf("Failed to classify FallCurve\r\n");
            handle -> sensorConnected = (CHAR *)classification_status_fail;
            handle -> telemetryStatus = 0;
            return(NX_NOT_SUCCESSFUL);
        }
        else
        {
            printf("Fingerprint Evaluation Successful\r\n");
            if(sensorid > 0)
            {
                handle -> sensorConnected = (CHAR *)handle -> connected_sensors[sensorid-1];
                if(((strlen(handle -> sensorConnected) == strlen(handle -> associatedSensor)) &&
                    (!(strncmp((CHAR *)handle -> sensorConnected, (CHAR *)handle -> associatedSensor, (strlen(handle -> sensorConnected)))))) == 1)
                {
                    handle -> telemetryStatus = 1;
                }
                else
                {
                    handle -> telemetryStatus = 0;
                }
                    
            }
            else if (sensorid == 0)
            {
                handle -> sensorConnected = (CHAR *)classification_status_unidentified;
                handle -> telemetryStatus = 0;
            }
            else 
            {
                handle -> sensorConnected = (CHAR *)classification_status_dbempty;
                handle -> telemetryStatus = 0;
            }
        }
        
    }
    else
    {
        printf("Verified Telemetry OFF\r\n");
        handle -> fallcurveString = (CHAR *)collection_off_fingerprint;
        handle -> sensorConnected = (CHAR *)classification_status_no_eval;
        handle -> telemetryStatus = 0;
    }

    return(NX_AZURE_IOT_SUCCESS);
}

/* reset fingerprint method implementation */
static UINT reset_golden_fallcurve(PNP_FALLCURVE_COMPONENT *handle,
                                            NX_AZURE_IOT_JSON_READER *json_reader_ptr)
{
    UINT status = 0;
    INT ground_truth_label;
    uint32_t fallcurvearray[100];

    vt_database_clear(&(handle->fingerprintdb));
    printf("Cleared DB\r\n");

    if(identify_ground_truth_label((UCHAR *)handle -> associatedSensor, strlen(handle -> associatedSensor),
                                                handle, &ground_truth_label))
    {
        printf("Sensor Name does not match with any registered sensors\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
    else if(vt_sensor_read_fingerprint(&(handle -> portInfo) , fallcurvearray, fallcurvestring))
    {
        printf("Error in collecting requested Fall Curve\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
    else
    {
        // _vt_fingerprint_calculate_falltime_pearsoncoefficient(fallcurvearray, 100, fallcurve_components[index]->portInfo.vt_sampling_frequency , &falltime, &pearson_coefficient);
        if(vt_database_store(&(handle->fingerprintdb), fallcurvearray, handle->portInfo.vt_sampling_frequency, ground_truth_label))
        {
            printf("Failed to collect and store Golden Fingerprint\r\n");
        }
    }

    return(status);

}

static UINT hub_store_all_db( PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;

    INT iter = 0;
    int sensorid = 0;
    CHAR sensorIDStr[3];
    int fallTime = 0;
    CHAR fallTimeStr[5];
    double pearsonCoeff = 0;
    CHAR pearsonCoeffStr[7];
    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr,
                                                                     &json_writer, NX_WAIT_FOREVER)))
    {
        printf("Failed create reported properties: error code = 0x%08x\r\n", status);
        return(status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(iotpnp_client_ptr,
                                                                            &json_writer,
                                                                            handle -> component_name_ptr,
                                                                            handle -> component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_name(&json_writer, 
                                                                (UCHAR*)fingerprintTemplate_property, 
                                                                sizeof(fingerprintTemplate_property)-1)) || 
        (status = nx_azure_iot_json_writer_append_begin_object(&json_writer)))
    {
        printf("Failed to start building reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    if((status = nx_azure_iot_json_writer_append_property_name(&json_writer,
        (UCHAR*)sensorName_json_property, strlen((const char*)sensorName_json_property))))
    {
        printf("Failed to append sensorName DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }
    if((status = nx_azure_iot_json_writer_append_string(&json_writer, (UCHAR*)handle -> associatedSensor, strlen(handle -> associatedSensor))))
    {
        printf("Failed to append sensorName DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    if((status = nx_azure_iot_json_writer_append_property_name(&json_writer,
        (UCHAR*)sensorID_json_property, strlen((const char*)sensorID_json_property))))
    {
        printf("Failed to append sensorID DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }
    if(identify_ground_truth_label((UCHAR *)handle -> associatedSensor, strlen(handle -> associatedSensor),
                                                handle, &sensorid))
    {
        printf("Sensor Name does not match with any registered sensors\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
    snprintf(sensorIDStr, sizeof(sensorIDStr), "%d", sensorid);
    if((status = nx_azure_iot_json_writer_append_string(&json_writer, (UCHAR*)sensorIDStr, strlen(sensorIDStr))))
    {
        printf("Failed to append sensorID DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    if((status = nx_azure_iot_json_writer_append_property_name(&json_writer,
        (UCHAR*)fallTime_json_property, strlen((const char*)fallTime_json_property))))
    {
        printf("Failed to append FallTime DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }
    iter = 0;
    vt_database_falltime_fetch(&(handle->fingerprintdb), &iter, &fallTime, &sensorid);
    snprintf(fallTimeStr, sizeof(fallTimeStr), "%04d", fallTime);
    if(iter<0)
    {
        if((status = nx_azure_iot_json_writer_append_null(&json_writer)))
        {
            printf("Failed to append FallTime DB data: error code = 0x%08x\r\n", status);
            nx_azure_iot_json_writer_deinit(&json_writer);
            return(status);
        }
    }
    else
    {
        if((status = nx_azure_iot_json_writer_append_string(&json_writer, (UCHAR*)fallTimeStr, sizeof(fallTimeStr)-1)))
        {
            printf("Failed to append FallTime DB data: error code = 0x%08x\r\n", status);
            nx_azure_iot_json_writer_deinit(&json_writer);
            return(status);
        }
    }
    
    if((status = nx_azure_iot_json_writer_append_property_name(&json_writer,
        (UCHAR*)pearsonCoeff_json_property, strlen((const char*)pearsonCoeff_json_property))))
    {
        printf("Failed to append Pearson Coefficient DB data: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }
    iter = 0;
    vt_database_pearsoncoefficient_fetch(&(handle->fingerprintdb), &iter, &pearsonCoeff, &sensorid);
    int pearsonCoeffInt1 = pearsonCoeff;
    float pearsonCoeffFrac = pearsonCoeff - pearsonCoeffInt1;
    int pearsonCoeffInt2 = pearsonCoeffFrac*10000;
    snprintf(pearsonCoeffStr, sizeof(pearsonCoeffStr), "%d.%04d", pearsonCoeffInt1, pearsonCoeffInt2);
    if(iter<0)
    {
        if((status = nx_azure_iot_json_writer_append_null(&json_writer)))
        {
            printf("Failed to append Pearson Coefficient DB data: error code = 0x%08x\r\n", status);
            nx_azure_iot_json_writer_deinit(&json_writer);
            return(status);
        }
    }
    else
    {
        if((status = nx_azure_iot_json_writer_append_string(&json_writer, (UCHAR*)pearsonCoeffStr, sizeof(pearsonCoeffStr)-1)))
        {
            printf("Failed to append Pearson Coefficient DB data: error code = 0x%08x\r\n", status);
            nx_azure_iot_json_writer_deinit(&json_writer);
            return(status);
        }
    }

    if ((status = nx_azure_iot_json_writer_append_end_object(&json_writer)) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr,
                                                                          &json_writer)))
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

static UINT sync_fingerprintTemplate(NX_AZURE_IOT_JSON_READER* property_value_reader_ptr, PNP_FALLCURVE_COMPONENT *handle)
{
    int sensorid = 0;
    CHAR jsonKey[50];
    CHAR jsonValue[50];
    UINT bytes_copied = 0; 
    int32_t fallTime = 0;
    double pearsonCoeff = 0;

    if(handle->flash_address != 0x00)
    {
        return NX_SUCCESS;
    }

    vt_database_clear(&(handle->fingerprintdb));
    printf("Cleared DB\r\n");

    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonKey, 
                                                    sizeof(jsonKey), &bytes_copied))
    {
        return NX_SUCCESS;
    }
    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonValue, 
                                                    sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if (((strlen(jsonKey) == (sizeof(sensorName_json_property) - 1)) &&
        (!(strncmp((CHAR *)jsonKey, (CHAR *)sensorName_json_property, strlen(jsonKey))))) == 1)
    {
        // If required use sensorName string
    }

    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonKey, 
                                                    sizeof(jsonKey), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonValue, 
                                                    sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if (((strlen(jsonKey) == (sizeof(sensorID_json_property) - 1)) &&
        (!(strncmp((CHAR *)jsonKey, (CHAR *)sensorID_json_property, strlen(jsonKey))))) == 1)
    {
        sensorid = atoi((CHAR*)jsonValue);
    }

    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonKey, 
                                                    sizeof(jsonKey), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonValue, 
                                                    sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if (((strlen(jsonKey) == (sizeof(fallTime_json_property) - 1)) &&
        (!(strncmp((CHAR *)jsonKey, (CHAR *)fallTime_json_property, strlen(jsonKey))))) == 1)
    {
        fallTime = atoi((CHAR*)jsonValue);
        _vt_database_store_falltime(&(handle->fingerprintdb), fallTime, sensorid);
    }

    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonKey, 
                                                    sizeof(jsonKey), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if(nx_azure_iot_json_reader_next_token(property_value_reader_ptr) || nx_azure_iot_json_reader_token_string_get(   property_value_reader_ptr, (UCHAR *) jsonValue, 
                                                    sizeof(jsonValue), &bytes_copied))
    {
        return NX_NOT_SUCCESSFUL;
    }
    if (((strlen(jsonKey) == (sizeof(pearsonCoeff_json_property) - 1)) &&
        (!(strncmp((CHAR *)jsonKey, (CHAR *)pearsonCoeff_json_property, strlen(jsonKey))))) == 1)
    {
        pearsonCoeff = atof((CHAR*)jsonValue);
        _vt_database_store_pearsoncoefficient(&(handle->fingerprintdb), pearsonCoeff, sensorid);
    }

    return NX_SUCCESS;
}

UINT pnp_fallcurve_telemetryStatus_property(PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr, 
                                        bool toggleVerifiedTelemetry, bool *deviceStatus)
{
    UINT status;
    UINT response_status = 0;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    bool oldTelemetryStatus = handle -> telemetryStatus;
    /* Get fallcurve data. */
    if ((status = get_fallcurve(handle, &(handle->fingerprintdb), toggleVerifiedTelemetry)))
    {
        printf("Fetching Fall Curve failed!: error code = 0x%08x\r\n", status);
        return(status);
    }

    *deviceStatus &= handle -> telemetryStatus;

    if(oldTelemetryStatus == handle -> telemetryStatus && handle -> vTInfo_property_sent != 0)
    {
        // printf("Telemetry Status is the same, not updating digitalTwin!\r\n");
        return(status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_properties_create(iotpnp_client_ptr,
                                                                     &json_writer, NX_WAIT_FOREVER)))
    {
        printf("Failed create reported properties: error code = 0x%08x\r\n", status);
        return(status);
    }

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(iotpnp_client_ptr,
                                                                            &json_writer,
                                                                            handle -> component_name_ptr,
                                                                            handle -> component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_bool_value(&json_writer,
                                                                             (const UCHAR *)telemetry_name_Telemetry_Status,
                                                                             sizeof(telemetry_name_Telemetry_Status) - 1,
                                                                             handle -> telemetryStatus)) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr,
                                                                          &json_writer)))
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

UINT pnp_fallcurve_fingerprintType_property(PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr)
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

    if ((status = nx_azure_iot_pnp_client_reported_property_component_begin(iotpnp_client_ptr,
                                                                            &json_writer,
                                                                            handle -> component_name_ptr,
                                                                            handle -> component_name_length)) ||
        (status = nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
                                                                             (const UCHAR *)fingerprintType_property,
                                                                             sizeof(fingerprintType_property) - 1,
                                                                             (const UCHAR *)fingerprintType_value,
                                                                             strlen((const char*)fingerprintType_value))) ||
        (status = nx_azure_iot_pnp_client_reported_property_component_end(iotpnp_client_ptr,
                                                                          &json_writer)))
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

UINT pnp_fallcurve_process_command(PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                            UCHAR *component_name_ptr, UINT component_name_length,
                                            UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length,
                                            NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                            NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code)
{
    UINT dm_status;

    if (handle == NX_NULL)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    if (handle -> component_name_length != component_name_length ||
        strncmp((CHAR *)handle -> component_name_ptr, (CHAR *)component_name_ptr, component_name_length) != 0)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    //Command 1 : Reset Fingerprint
    if (((pnp_command_name_length == (sizeof(command_reset_fingerprint) - 1)) &&
        (!(strncmp((CHAR *)pnp_command_name_ptr, (CHAR *)command_reset_fingerprint, pnp_command_name_length)))) == 1)
    {
        dm_status = (reset_golden_fallcurve(handle, json_reader_ptr) != NX_AZURE_IOT_SUCCESS) ? SAMPLE_COMMAND_ERROR_STATUS :
                                                                                                SAMPLE_COMMAND_SUCCESS_STATUS;      
        
        if(hub_store_all_db(handle, iotpnp_client_ptr))
        {
            printf("Failed to update db in cloud\r\n");
        }                                                                                                                                                
    }
    
    else
    {
        printf("PnP command=%.*s is not supported on vTInfo  component\r\n", pnp_command_name_length, pnp_command_name_ptr);
        dm_status = 404;
    }

    *status_code = dm_status;

    return(NX_AZURE_IOT_SUCCESS);
}

UINT pnp_fallcurve_process_reported_property_sync(PNP_FALLCURVE_COMPONENT *handle,
                                            NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                            const UCHAR *component_name_ptr, UINT component_name_length,
                                            NX_AZURE_IOT_JSON_READER *name_value_reader_ptr, UINT version)
{
    CHAR property_name[50];
    UINT property_name_length = 0;

    if (handle == NX_NULL)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    if (handle -> component_name_length != component_name_length ||
        strncmp((CHAR *)handle -> component_name_ptr, (CHAR *)component_name_ptr, component_name_length) != 0)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    //Get Property Name
    nx_azure_iot_json_reader_token_string_get(name_value_reader_ptr, (UCHAR*)property_name, sizeof(property_name), &property_name_length);

    //Property 1: FingerprintDB
    if (nx_azure_iot_json_reader_token_is_text_equal(name_value_reader_ptr,
                                                     (UCHAR *)fingerprintTemplate_property,
                                                     sizeof(fingerprintTemplate_property) - 1) == NX_TRUE)
    {
        if (nx_azure_iot_json_reader_next_token(name_value_reader_ptr) ||
        sync_fingerprintTemplate(name_value_reader_ptr, handle))
        {
            printf("Error in syncing fingerprint template\r\n");
            return(NX_AZURE_IOT_FAILURE);
        }
        else
        {
            printf("Successfully synced fingerprint template\r\n");
            return(NX_AZURE_IOT_SUCCESS);
        }
    }
    
    return(NX_AZURE_IOT_SUCCESS);
}