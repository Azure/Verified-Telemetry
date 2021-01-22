/* Copyright (c) Microsoft Corporation. */

#ifndef PNP_FALLCURVE_COMPONENT_H
#define PNP_FALLCURVE_COMPONENT_H

#ifdef __cplusplus
extern   "C" {
#endif

#include "nx_azure_iot_pnp_client.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"

#include "vt_api.h"

typedef struct PNP_FALLCURVE_COMPONENT_TAG
{
    /* Name of this component */
    UCHAR *component_name_ptr;

    UINT component_name_length;

    /* Fall Curve data in CSV string of this fallcurve component */
    CHAR * fallcurveString;
    
    /* Fall Curve classification status of this fallcurve component  */
    CHAR * sensorConnected;

    /* Associated telemetry status of this fallcurve component  */
    bool telemetryStatus;

    /* Sensor asscociated with this FallCurve  */
    CHAR *associatedSensor;

    /* Stores Fall Curve collection information  */
    VT_SENSOR portInfo;

    /* Stores names of the sensors connected */
    CHAR **connected_sensors;   

    /* Number of sensor names stored  */
    UINT connected_sensors_num;

    /* Status of reported properties sent  */
    UINT vTInfo_property_sent;

    /* Fingerprint DB */
    VT_DATABASE fingerprintdb;

    /* Flash Address for storage of verified telemetry properties */
    UINT flash_address;

} PNP_FALLCURVE_COMPONENT;

UINT pnp_fallcurve_init(PNP_FALLCURVE_COMPONENT *handle,
                                UCHAR *component_name_ptr,
                                GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, 
                                ADC_CONTROLLER_TYPEDEF * ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel,
                                TIMER_HANDLE_TYPEDEF* Timer, 
                                PNP_FALLCURVE_COMPONENT **fallcurve_components,
                                CHAR **connected_sensors, UCHAR *associatedTelemetry,
                                UINT numberVerifiedTelemetries);

UINT pnp_fallcurve_telemetryStatus_property(PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr, 
                                        bool toggleVerifiedTelemetry, bool *deviceStatus);

UINT pnp_fallcurve_fingerprintType_property(PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr);

UINT pnp_fallcurve_process_command(PNP_FALLCURVE_COMPONENT *handle, NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                           UCHAR *component_name_ptr, UINT component_name_length,
                                           UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length,
                                           NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                           NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code);

UINT pnp_fallcurve_process_reported_property_sync(PNP_FALLCURVE_COMPONENT *handle,
                                            NX_AZURE_IOT_PNP_CLIENT *iotpnp_client_ptr,
                                            const UCHAR *component_name_ptr, UINT component_name_length,
                                            NX_AZURE_IOT_JSON_READER *name_value_reader_ptr, UINT version);

#ifdef __cplusplus
}
#endif
#endif /* PNP_FALLCURVE_COMPONENT_H */
